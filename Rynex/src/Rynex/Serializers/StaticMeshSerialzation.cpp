#include "rypch.h"
#include "StaticMeshSerialzation.h"

#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Asset/Base/AssetManager.h>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#include <Rynex/Serializers/YAML.h>


namespace Rynex {

	namespace Utils {

		static bool SerializerAssetFormate(YAML::Emitter& out, const std::string& name, AssetHandle handle)
		{
			const Ref<Project> project = Project::GetActive();
			const Ref<EditorAssetManagerThread> editorAssetManger = project->GetEditorAssetManger();
			if (!editorAssetManger->IsAssetHandleValid(handle))
			{

				RY_CORE_ERROR("We could not Serialize! Because we didn't find AssetHandle: ({})  in AssetManager!", handle);
				return false;
			}
			const AssetMetadata metaData = editorAssetManger->GetMetadata(handle);
			const std::filesystem::path& filePath = metaData.m_FilePath;
			const std::filesystem::path& pathMarked = metaData.m_PathMarker;

			if (!editorAssetManger->IsAssetHandleValid(filePath))
			{
				RY_CORE_ERROR("We could not Serialize! Because we didn't find Filepath: ({}) in AssetManager!", filePath);
				return false;
			}
			

			out << YAML::Key << name.c_str();
			out << YAML::BeginMap;
			out << YAML::Key << "Path-ProjectMarker" << pathMarked;
			out << YAML::Key << "Handle" << handle;
			out << YAML::EndMap;
			return true;
		}

		static bool SerializeMeshNode(YAML::Emitter& out, const MeshStatic::SingleObjectMeshData& object)
		{			
			out << YAML::BeginMap;
			out << YAML::Key << "MeshSingle" << YAML::Value;
			{
				out << YAML::BeginMap;
				const Ref<MeshSingle>& meshSingle = object.m_MeshSingle;
				const std::string& meshName = meshSingle->GetName();
				const uint32_t localeMeshIndex = meshSingle->GetModelLocalMeshIndex();
				out << YAML::Key << "Name" << YAML::Value << meshName;
				out << YAML::Key << "LocaleIndex" << YAML::Value << localeMeshIndex;
				out << YAML::EndMap;
			}

			{
				const Ref<Project> project = Project::GetActive();
				const Ref<EditorAssetManagerThread> editorAssetManger = project->GetEditorAssetManger();

				const Ref<Material>& material = object.m_Material;
				const AssetHandle materielHandle = material->Handle;

				const std::string key = "Material";
				if(editorAssetManger->IsAssetHandleValid(materielHandle) && !editorAssetManger->IsAssetInteral(materielHandle))
				{
				    SerializerAssetFormate(out, key, materielHandle);
				}
				else
				{
				    out << YAML::Key << key << YAML::Value;
				    out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << object.NodeName;
					out << YAML::Key << "LocaleIndex" << object.LocaleIndexMateriel;
				    out << YAML::EndMap;
				}

			}

			{
				out << YAML::BeginMap;
				out << YAML::Key << "MeshName" << YAML::Value << object.NodeName;
				out << YAML::Key << "LocaleCildrenMatrix" << YAML::Value << object.LocaleCildrenMatrix;
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
			return true;
		}

		static bool SerializeNodes(YAML::Emitter& out, const Ref<MeshStatic>& meshStatic)
		{
			out << YAML::BeginSeq;
			const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshStatic->GetSingleObjectMesDataVec();
			for (const MeshStatic::SingleObjectMeshData& object : singleObjectMeshDataVec)
			{
				SerializeMeshNode(out, object);
			}
			out << YAML::EndSeq;
			return true;
		}

		template<typename T>
		static bool DeserializeAssetFormate(YAML::Node& nodeE, Ref<T>& storeAsset)
		{
			if (!nodeE)
			{
				const std::string& tag = nodeE.Scalar();
				RY_CORE_ERROR("Not Found Node: {}", tag);
				return false;
			}

            FileSystem::Path path;
		    FileSystem::Path markedPath;
			if(YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
				markedPath = nodeAtribut.as<std::string>();
			if (YAML::Node nodeAtribut = nodeE["Path"])
				path =  nodeAtribut.as<std::string>();
			const AssetHandle handle = nodeE["Handle"].as<uint64_t>();


			const AssetFindeInfo info(handle, path, markedPath);
			storeAsset = AssetManager::FindAsset<T>(info);

			return nullptr != storeAsset;
		}


		static bool DeserializeSingleMesh(const std::string& name, uint32_t localeIndex, MeshStatic::SingleObjectMeshData& singleObjectMeshData, Ref<MeshSource>& sourceMesh)
		{
			const std::vector<Ref<MeshSingle>>& meshSingleVec = sourceMesh->GetMeshSingleVecConst();
			uint32_t index = 0u;
			Ref<MeshSingle> meshSingleLocaleIndexPast = nullptr;
		    const uint32_t count = meshSingleVec.size();
			if (count <= localeIndex)
			{
				RY_CORE_WARN("We can't identify the Mesh not withe the stroed locale Index {}, we need to recreate the file probly!", localeIndex);
			}

			for (const Ref<MeshSingle>& meshSingle : meshSingleVec)
			{
				const std::string& meshSingleName = meshSingle->GetName();
				if (meshSingleName == name)
				{
					singleObjectMeshData.m_MeshSingle = meshSingle;
					singleObjectMeshData.LocaleIndexMesh = index;
					return true;
				}

				if (index == localeIndex)
				{
					meshSingleLocaleIndexPast = meshSingle;
				}

				index++;
			}
			if(localeIndex < index)
			{
				singleObjectMeshData.m_MeshSingle = meshSingleLocaleIndexPast;
				singleObjectMeshData.LocaleIndexMesh = index;

				RY_CORE_WARN("We dont found name we use the locale Mesh Index to identify MeshSingle");
				return true;
			}
			RY_CORE_ERROR("We cude not identify by name ({}) or localeMesh Index ({}) the Mesh  ", name, localeIndex);
			return false;

		}

		static bool DeserializeMateriel(const std::string& name, uint32_t localeIndex, MeshStatic::SingleObjectMeshData& singleObjectMeshData, Ref<MeshSource>& sourceMesh)
		{
			const std::vector<MeshSource::MaterialMesh>& materielSourceVec = sourceMesh->GetMaterialsSourcesConst();
			const std::vector<Ref<Material>>& materielVec = sourceMesh->GetMaterialsVecConst();

			uint32_t index = 0u;
			MeshSource::MaterialMesh materielLocaleIndexPast;
		    uint32_t count = materielSourceVec.size();
			if (count <= localeIndex)
			{
				RY_CORE_WARN("We can't identify the Mesh not withe the stroed locale Index {}, we need to recreate the file probly!", localeIndex);
			}

			for (const MeshSource::MaterialMesh& mateiel : materielSourceVec)
			{
				const std::string& meshSingleName = mateiel.m_NameMateriel;
				if (meshSingleName == name)
				{
					uint32_t materilLocaleIndex = mateiel.MaterielIndex;
					RY_CORE_ASSERT(materilLocaleIndex < materielVec.size(), "out side of range!");
					singleObjectMeshData.m_Material = materielVec.at(materilLocaleIndex);
					singleObjectMeshData.LocaleIndexMateriel = materilLocaleIndex;
					return true;
				}
				else if (index == localeIndex)
				{
					materielLocaleIndexPast = mateiel;
				}

				index++;
			}
			if (localeIndex < index)
			{
				uint32_t materilLocaleIndex = materielLocaleIndexPast.MaterielIndex;
				RY_CORE_ASSERT(materilLocaleIndex < materielVec.size(), "out side of range!");
				singleObjectMeshData.m_Material = materielVec.at(materilLocaleIndex);
				singleObjectMeshData.LocaleIndexMateriel = materilLocaleIndex;

				RY_CORE_WARN("We dont found name we use the locale Mesh Index to identify MeshSingle");
				return true;
			}
			RY_CORE_ERROR("We cude not identify by name ({}) or localeMesh Index ({}) the Mesh  ", name, localeIndex);
			return false;

		}

	    // TODO: Replace function in the long run.
		static bool DeserializeMateriel(AssetHandle materialHandle, const std::filesystem::path& path, MeshStatic::SingleObjectMeshData& singleObjectMeshData)
		{
		    RY_REMBER_FUNC_CHANGE("Remove function replace withe DeserializeAssetFormate!");
			const Ref<Project> project = Project::GetActive();
			const Ref<EditorAssetManagerThread> assetManager = project->GetEditorAssetManger();
			const AssetHandle handleP = assetManager->GetAssetHandle(path);
			RY_CORE_ASSERT(handleP == materialHandle, "Not Simulare Asset Handle!");

			singleObjectMeshData.LocaleIndexMateriel = static_cast<uint32_t>(-1);
			Ref<Material>& materiel = singleObjectMeshData.m_Material;
			if(assetManager->IsAssetHandleValid(materialHandle))
			{
				materiel = AssetManager::GetAsset<Material>(materialHandle);
				return true;
			}
			else
			{
				materiel = AssetManager::GetAsset<Material>(handleP);
				return true;
			}

			return false;

		}


		static bool DeserializeMeshObject(const YAML::Node& meshNodes, MeshStatic::SingleObjectMeshData& singleObjectMeshData, Ref<MeshSource>& sourceMesh)
		{
			if (const YAML::Node meshSingleNode = meshNodes["MeshSingle"])
			{
				const std::string name = meshSingleNode["Name"].as<std::string>();
				uint32_t localeIndex = meshSingleNode["LocaleIndex"].as<uint32_t>();

				bool resultSingleMesh = Utils::DeserializeSingleMesh(name, localeIndex, singleObjectMeshData, sourceMesh);
			}	
            RY_REMBER_FUNC_CHANGE("Remove function DeserializeMateriel and replace complett withe DeserializeAssetFormate!");
			if (YAML::Node materielNode = meshNodes["Materiel"])
			{
			    bool resultMateriel;
				if(const YAML::Node materielNameNode = materielNode["Name"])
				{
					const std::string name = materielNameNode.as<std::string>();
					uint32_t localeIndex = materielNode["LocaleIndex"].as<uint32_t>();
					
					resultMateriel = Utils::DeserializeMateriel(name, localeIndex, singleObjectMeshData, sourceMesh);
				}
				else if(const YAML::Node materielHandleNode = materielNode["Handle"])
				{

				    if (const YAML::Node materielFilePathNode = materielNode["FilePath"])
				    {
				        AssetHandle materielHandle = materielHandleNode.as<uint64_t>();

					    std::filesystem::path materielFilPath = materielNode["FilePath"].as<std::string>();
					    resultMateriel = Utils::DeserializeMateriel(materielHandle, materielFilPath, singleObjectMeshData);
				    }
					else
					{
					   resultMateriel = DeserializeAssetFormate<Material>(materielNode, singleObjectMeshData.m_Material);
					}
				}
				else
				{
					return false;
				}

			}
			if (const YAML::Node& objectNode = meshNodes["Node"])
			{
				std::string objectName = objectNode["MeshName"].as<std::string>();
				glm::mat4 matrix = objectNode["LocaleCildrenMatrix"].as<glm::mat4>();
				singleObjectMeshData.LocaleCildrenMatrix = matrix;
				singleObjectMeshData.NodeName = objectName;
			}

			return true;
		}

	}
	StaticMeshSerialzation::StaticMeshSerialzation(const Ref<MeshStatic>& meshStatic)
		: m_MeshStatic(meshStatic)
	{
	}

	bool StaticMeshSerialzation::Serialize(const std::filesystem::path& filepath)
	{
		RY_CORE_WARN("Begin Serialize a MeshSource in '{}'", filepath);

		YAML::Emitter out;

		out << YAML::BeginMap;
		{
			Ref<MeshSource> meshSource = m_MeshStatic->GetMeshSource();
			AssetHandle sourceHandle = meshSource->Handle;
			if (!Utils::SerializerAssetFormate(out, "Source-Mesh", sourceHandle))
				return false;

			out << YAML::Key << "MeshNodes";
			if (!meshSource->IsOneToOne(m_MeshStatic))
			{
				RY_CORE_INFO("MeshStatic is one to one");
				out << YAML::BeginSeq;
				out << YAML::EndSeq;
			} 
			else if (!Utils::SerializeNodes(out, m_MeshStatic))
			{
				return false;
			}
		}
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		RY_CORE_ASSERT(fout);
		fout << out.c_str();

		return true;
	}

	bool StaticMeshSerialzation::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}
		

		Ref<MeshSource> meshSource = nullptr;
		YAML::Node sourceMesh = data["Source-Mesh"];
		if (!Utils::DeserializeAssetFormate(sourceMesh, meshSource))
		{
			return false;
		}
		if (nullptr == meshSource)
		{
			RY_CORE_ERROR("Not sucess by loding MeshSource Asset");
			return false;
		}
		if (!DeserializeMeshNodes(data["MeshNodes"], meshSource))
		{
			return false;
		}

		return true;
	}

	bool StaticMeshSerialzation::DeserializeMeshNodes(const YAML::Node& meshNodes, Ref<MeshSource>& sourceMesh)
	{
		m_MeshStatic->Cear();
		if (!meshNodes)
		{
			const std::string& tag = meshNodes.Tag();
			RY_CORE_ERROR("Not Found Node: {}", tag);
			return false;
		}

		Ref<MeshStatic> meshStaticSource = Mesh::CreateStaticMesh(sourceMesh);
		std::vector<MeshStatic::SingleObjectMeshData> singleObjectMeshDataVec;
		if (meshNodes.size() != 0)
		{
			for (const YAML::detail::iterator_value& node : meshNodes)
			{
				MeshStatic::SingleObjectMeshData data;
				if (Utils::DeserializeMeshObject(node, data, sourceMesh))
				{
					singleObjectMeshDataVec.emplace_back(data);
				}
			}
			RY_CORE_TRACE("Loade Some Node MeshStatic!");
		}
		else
		{
			singleObjectMeshDataVec = meshStaticSource->GetSingleObjectMesDataVec();
			RY_CORE_TRACE("Loade all MeshStatic objects! {}", singleObjectMeshDataVec.size());
		}
		
		if (singleObjectMeshDataVec.empty())
		{
			RY_CORE_ERROR("Empty Mesh Object list");
			return false;
		}
		m_MeshStatic->Create(sourceMesh, singleObjectMeshDataVec);
		
	}

	

	
}