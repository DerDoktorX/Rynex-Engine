#include "rypch.h"
#include "StaticMeshSerialzation.h"

#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Asset/Base/AssetManager.h>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#include "YAML.h"


namespace Rynex {

	namespace Utils {

		static bool SerializerAssetFormate(YAML::Emitter& out, const std::string& name, AssetHandle handle)
		{
			Ref<Project> project = Project::GetActive();
			Ref<EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();
			if (!editorAssetManger->IsAssetHandleValid(handle))
			{
				uint64_t handleV = handle;
				RY_CORE_ERROR("We Coud not Serialize! Because we dient finde AssetHandle: ({})  in Assetmaneger!", handleV);
				return false;
			}
			AssetMetadata metaData = editorAssetManger->GetMetadata(handle);
			const std::filesystem::path& filePath = metaData.FilePath;
			const std::filesystem::path& pathMarked = metaData.PathMarker;

			if (!editorAssetManger->IsAssetHandleValid(filePath))
			{
				RY_CORE_ERROR("We Coud not Serialize! Because we dient finde Filepath: ({}) in Assetmaneger!", filePath);
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
				const Ref<MeshSingle>& meshSingle = object._MeshSingle;
				const std::string& meshName = meshSingle->GetName();
				uint32_t localeMeshIndex = meshSingle->GetModelLocalMesheIndex();
				out << YAML::Key << "Name" << YAML::Value << meshName;
				out << YAML::Key << "LocaleIndex" << YAML::Value << localeMeshIndex;
				out << YAML::EndMap;
			}
			out << YAML::Key << "Materiel" << YAML::Value;
			{
				Ref<Project> project = Project::GetActive();
				Ref<EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();

				const Ref<Material>& material = object._Material;
				AssetHandle materielHandle = material->Handle;

				out << YAML::BeginMap;
				if(editorAssetManger->IsAssetHandleValid(materielHandle) && !editorAssetManger->IsAssetInteral(materielHandle))
				{
					AssetMetadata metadata = editorAssetManger->GetMetadata(materielHandle);
					std::filesystem::path filePath = metadata.FilePath;
					out << YAML::Key << "Handle" << materielHandle;
					out << YAML::Key << "FilePath" << filePath;
				}
				else
				{
					out << YAML::Key << "Name" << YAML::Value << object.NodeName;
					out << YAML::Key << "LocaleIndex" << object.LocaleIndexMateriel;
				}
				out << YAML::EndMap;
			}
			out << YAML::Key << "Node" << YAML::Value;
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

		template< typename T>
		static bool DeserializeAssetFormate(YAML::Node& nodeE, Ref<T>& storeAsset)
		{
			if (!nodeE)
			{
				const std::string& tag = nodeE.Scalar();
				RY_CORE_ERROR("Not Found Node: {}", tag);
				return false;
			}
			std::string markedPath = "";
			std::string path = "";

			if(YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
				markedPath = nodeAtribut.as<std::string>();
			if (YAML::Node nodeAtribut = nodeE["Path"])
				path = nodeAtribut.as<std::string>();
			AssetHandle handle = nodeE["Handle"].as<uint64_t>();

			AssetFindeInfo info = AssetFindeInfo(handle, markedPath, path);
			storeAsset = AssetManager::FindeAsset<T>(info);

			return storeAsset != nullptr;
		}


		static bool DeserializeSingleMesh(const std::string& name, uint32_t localeIndex, MeshStatic::SingleObjectMeshData& singleObjectMeshData, Ref<MeshSource>& sourceMesh)
		{
			const std::vector<Ref<MeshSingle>>& meshSingleVec = sourceMesh->GetMeshSingleVecConst();
			uint32_t index = 0u;
			Ref<MeshSingle> meshSingleLocaleIndexPast = nullptr;
			if (localeIndex >= meshSingleVec.size())
			{
				RY_CORE_WARN("We can't identify the Mesh not withe the stroed locale Index {}, we need to recreate the file probly!", localeIndex);
			}

			for (const Ref<MeshSingle>& meshSingle : meshSingleVec)
			{
				const std::string& meshSingleName = meshSingle->GetName();
				if (meshSingleName == name)
				{
					singleObjectMeshData._MeshSingle = meshSingle;
					singleObjectMeshData.LocaleIndexMesh = index;
					return true;
				}
				else if (index == localeIndex)
				{
					meshSingleLocaleIndexPast = meshSingle;
				}

				index++;
			}
			if(localeIndex < index)
			{
				singleObjectMeshData._MeshSingle = meshSingleLocaleIndexPast;
				singleObjectMeshData.LocaleIndexMesh = index;

				RY_CORE_WARN("We dont found name we use the locale Mesh Index to identify MeshSingle");
				return true;
			}
			RY_CORE_ERROR("We cude not identify by name ({}) or localeMesh Index ({}) the Mesh  ", name, localeIndex);
			return false;

		}

		static bool DeserializeMateriel(const std::string& name, uint32_t localeIndex, MeshStatic::SingleObjectMeshData& singleObjectMeshData, Ref<MeshSource>& sourceMesh)
		{
			const std::vector<MeshSource::_Material>& materielSourceVec = sourceMesh->GetMaterialsSourcesConst();
			const std::vector<Ref<Material>>& materielVec = sourceMesh->GetMaterialsVecConst();

			uint32_t index = 0u;
			MeshSource::_Material materielLocaleIndexPast;
			if (localeIndex >= materielVec.size())
			{
				RY_CORE_WARN("We can't identify the Mesh not withe the stroed locale Index {}, we need to recreate the file probly!", localeIndex);
			}

			for (const MeshSource::_Material& mateiel : materielSourceVec)
			{
				const std::string& meshSingleName = mateiel.NameMateriel;
				if (meshSingleName == name)
				{
					uint32_t materilLocaleIndex = mateiel.MaterielIndex;
					RY_CORE_ASSERT(materilLocaleIndex < materielVec.size(), "out side of range!");
					singleObjectMeshData._Material = materielVec.at(materilLocaleIndex);
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
				singleObjectMeshData._Material = materielVec.at(materilLocaleIndex);
				singleObjectMeshData.LocaleIndexMateriel = materilLocaleIndex;

				RY_CORE_WARN("We dont found name we use the locale Mesh Index to identify MeshSingle");
				return true;
			}
			RY_CORE_ERROR("We cude not identify by name ({}) or localeMesh Index ({}) the Mesh  ", name, localeIndex);
			return false;

		}
	
		static bool DeserializeMateriel(AssetHandle materilHandle, const std::filesystem::path& path, MeshStatic::SingleObjectMeshData& singleObjectMeshData)
		{
			Ref<Project> project = Project::GetActive();
			Ref<EditorAssetManegerThreade> assetManager = project->GetEditorAssetManger();
			AssetHandle handleP = assetManager->GetAssetHandle(path);
			RY_CORE_ASSERT(handleP == materilHandle, "Not Simulare Asset Handle!");

			singleObjectMeshData.LocaleIndexMateriel = static_cast<uint32_t>(-1);
			Ref<Material>& materiel = singleObjectMeshData._Material;
			if(assetManager->IsAssetHandleValid(materilHandle))
			{
				materiel = AssetManager::GetAsset<Material>(materilHandle);
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
			if (const YAML::Node& meshSingleNode = meshNodes["MeshSingle"])
			{
				std::string name = meshSingleNode["Name"].as<std::string>();
				uint32_t localeIndex = meshSingleNode["LocaleIndex"].as<uint32_t>();

				bool resultSingleMesh = Utils::DeserializeSingleMesh(name, localeIndex, singleObjectMeshData, sourceMesh);
			}	

			if (const YAML::Node& materielNode = meshNodes["Materiel"])
			{
				if(const YAML::Node& materielNameNode = materielNode["Name"])
				{
					std::string name = materielNameNode.as<std::string>();
					uint32_t localeIndex = materielNode["LocaleIndex"].as<uint32_t>();
					
					bool resultMateriel = Utils::DeserializeMateriel(name, localeIndex, singleObjectMeshData, sourceMesh);
				}
				else if(const YAML::Node& materielHandleNode = materielNode["Handle"])
				{
					AssetHandle materielHandle = materielHandleNode.as<uint64_t>();
					std::filesystem::path materielFilPath = materielNode["FilePath"].as<std::string>();
					bool resultMateriel = Utils::DeserializeMateriel(materielHandle, materielFilPath, singleObjectMeshData);
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
		else if (nullptr == meshSource)
		{
			RY_CORE_ERROR("Not sucess by loding MeshSource Asset");
			return false;
		}
		else if (!DeserializeMeshNodes(data["MeshNodes"], meshSource))
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