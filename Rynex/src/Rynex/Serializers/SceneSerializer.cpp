#include "rypch.h"
#include "SceneSerializer.h"
#include "YAML.h"

#include <Rynex/Scene/Entity.h>
#include <Rynex/Scene/Components.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/API/Buffer.h>


#include <yaml-cpp/yaml.h>
#include <fstream>




namespace Rynex {

	struct EntitySerialData
	{
		uint64_t UUID;
		uint32_t Component = 0;
		TagComponent TagComponent;
		TransformComponent TransformComponent;
		CameraComponent CameraComponent;
		ScriptComponent ScriptComponent;
		SpriteRendererComponent SpriteRendererComponent;
		GeometryComponent GeomtryComponent;
		MaterialComponent MaterialComponent;
		RelationshipUUIDComponent RealtionShipComponent;
		ModelMatrixComponent ModelMatrixComponent;
		ModelMangerComponent ModelMangerComponent;
		DynamicMeshComponent DynamicMeshComponent;
	};

	struct SceneSerialData
	{
		uint32_t EntitiySize;
		std::vector<EntitySerialData> Enitity;
	};

	template<typename T>
	using SceneLodePromisType = LodePromisType<T, Scene, int>;

	template<typename T>
	using RefSceneLodePromisType = Ref<SceneLodePromisType<T>>;


	using RefSceneLodePromis = Ref<LodePromis<Scene>>;

	

	struct LodingAsyncPromise
	{
		std::vector<RefSceneLodePromisType<Texture>> PromiseTextureLodeVec;
		std::vector<RefSceneLodePromisType<MeshStatic>> PromiseMeshStaticLodeVec;
	};

namespace Utils {
	namespace Serializer {

		static void SerializerDynamicData(YAML::Emitter& out, const std::string& name, ShaderDataType type, const std::vector<unsigned char>& data)
	{
		switch (type)
		{
		case ShaderDataType::Float:
		{
			float* value = (float*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float2:
		{
			glm::vec<2, float>* value = (glm::vec<2, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float3:
		{
			glm::vec<3, float>* value = (glm::vec<3, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float4:
		{
			glm::vec<4, float>* value = (glm::vec<4, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float3x3:
		{
			glm::mat<3, 3, float>* value = (glm::mat<3, 3, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float4x4:
		{
			glm::mat<4, 4, float>* value = (glm::mat<4, 4, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int:
		{
			int* value = (int*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int2:
		{
			glm::vec<2, float>* value = (glm::vec<2, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int3:
		{
			glm::vec<3, float>* value = (glm::vec<3, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int4:
		{
			glm::vec<4, float>* value = (glm::vec<4, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		}
	}

		static void SerializerDynamicDataLayout(YAML::Emitter& out, const std::string& name, const BufferLayout& layout, const std::vector<unsigned char>& data)
	{
		out << YAML::Key << name.c_str() << YAML::BeginSeq;


		for (uint64_t offset = 0, size = data.size(); offset < size; offset += layout.GetStride())
		{
			for (const BufferElement& ellement : layout)
			{

				switch (ellement.m_Type)
				{
				case ShaderDataType::Float:
				{
					float* value = (float*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float2:
				{
					glm::vec<2, float>* value = (glm::vec<2, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float3:
				{
					glm::vec<3, float>* value = (glm::vec<3, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float4:
				{
					glm::vec<4, float>* value = (glm::vec<4, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float3x3:
				{
					glm::mat<3, 3, float>* value = (glm::mat<3, 3, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float4x4:
				{
					glm::mat<4, 4, float>* value = (glm::mat<4, 4, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int:
				{
					int* value = (int*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int2:
				{
					glm::vec<2, float>* value = (glm::vec<2, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int3:
				{
					glm::vec<3, float>* value = (glm::vec<3, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int4:
				{
					glm::vec<4, float>* value = (glm::vec<4, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				}
			}
		}
		out << YAML::EndSeq;
	}

		static void SerializerAssetFormate(YAML::Emitter& out, const std::string& name, AssetHandle handle)
		{
			Ref<Project> project = Project::GetActive();
			Ref<EditorAssetManagerThread> editorAssetManger = project->GetEditorAssetManger();
			AssetMetadata metadata = editorAssetManger->GetMetadata(handle);
			const std::filesystem::path& filePath = metadata.m_FilePath;
			const std::filesystem::path& pathMarked = metadata.m_PathMarker;

			std::string filePathStr = filePath.string();
			std::string pathMarkedStr = pathMarked.string();

			out << YAML::Key << name.c_str();
			out << YAML::BeginMap;
			out << YAML::Key << "Path" << filePathStr;
			out << YAML::Key << "Path-ProjectMarker" << pathMarkedStr;
			out << YAML::Key << "Handle" << handle;
			out << YAML::EndMap;
		}

		static void SerializerAssetFormate(YAML::Emitter& out, const std::string& name, AssetHandle handle, AssetType type)
		{
			Ref<Project> project = Project::GetActive();
			Ref<EditorAssetManagerThread> editorAssetManger = project->GetEditorAssetManger();
			if (!editorAssetManger->IsAssetHandleValid(handle))
			{
				RY_CORE_ERROR("Asset has no vild Handle Serialized in {}", name);
				return;
			}

			AssetMetadata metadata = editorAssetManger->GetMetadata(handle);

			if (metadata.m_Type != type)
			{
				std::string_view metadataType = Asset::AssetTypeToString(metadata.m_Type);
				std::string_view needMetadataType = Asset::AssetTypeToString(type);
				RY_CORE_ERROR("Asset not Serialized in {} ({} == {})", name, metadataType, needMetadataType);
				return;
			}
			const std::filesystem::path& filePath = metadata.m_FilePath;
			const std::filesystem::path& pathMarked = metadata.m_PathMarker;
			std::string filePathStr = filePath.string();
			std::string pathMarkedStr = pathMarked.string();

			out << YAML::Key << name.c_str();
			out << YAML::BeginMap;
			out << YAML::Key << "Path" << filePathStr;
			out << YAML::Key << "Path-ProjectMarker" << pathMarkedStr;
			out << YAML::Key << "Handle" << handle;
			out << YAML::EndMap;
		}

		static void SerializerModelDyanmic(YAML::Emitter& out, const std::string& name, DynamicMeshComponent& dMeshC)
		{
			
		}		
		
		
		static void SerializerEntity(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene)
		{
			RY_LOG_DISABLE_NUMBER;

			Ref<EditorAssetManagerThread> editorAssetManger = Project::GetActive()->GetEditorAssetManger();


			RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
			out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

			if (entity.HasComponent<TagComponent>())
			{
				out << YAML::Key << "TagComponent";
				out << YAML::BeginMap;

				std::string& tag = entity.GetComponent<TagComponent>().m_Tag;
				out << YAML::Key << "Tag" <<  tag;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<TransformComponent>())
			{
				out << YAML::Key << "TransformComponent";
				out << YAML::BeginMap;

				TransformComponent& tc = entity.GetComponent<TransformComponent>();
				out << YAML::Key << "Transaltion" << tc.m_Transform;
				out << YAML::Key << "Rotation" << tc.m_Rotation;
				out << YAML::Key << "Scale" << tc.m_Scale;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<CameraComponent>())
			{
				out << YAML::Key << "CameraComponent";
				out << YAML::BeginMap;

				CameraComponent& cc = entity.GetComponent<CameraComponent>();
				SceneCamera& camera = cc.m_Camera;

				out << YAML::Key << "Camera" << camera;
				out << YAML::Key << "Primary" << YAML::Value << cc.m_Primary;
				out << YAML::Key << "FixedAspectRotaion" << YAML::Value << cc.m_FixedAspectRotation;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<ScriptComponent>())
			{
				out << YAML::Key << "ScriptComponent";
				out << YAML::BeginMap;
				ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
				out << YAML::Key << "ClassName" << YAML::Value << sc.m_Name;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				out << YAML::Key << "SpriteRendererComponent";
				out << YAML::BeginMap;

				SpriteRendererComponent& sc = entity.GetComponent<SpriteRendererComponent>();
				out << YAML::Key << "Color" << YAML::Value << sc.m_Color;
				if (Ref<Texture> tex = sc.m_Texture.lock())
				{
					SerializerAssetFormate(out, "Texture", tex->Handle);
				}
				out << YAML::EndMap;
			}


			if (entity.HasComponent<RelationshipUUIDComponent>())
			{
				RelationshipUUIDComponent& rSc = entity.GetComponent<RelationshipUUIDComponent>();

				out << YAML::Key << "RealtionShipComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "ParentID" << YAML::Value << rSc.m_Parent;
				out << YAML::Key << "ChildrenIDs" << YAML::Value;
				out << YAML::Flow;
				out << YAML::BeginSeq;
				for (UUID& idChild : rSc.m_Childrens)
				{
					out << YAML::Value << idChild;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<ModelMatrixComponent>())
			{
				out << YAML::Key << "ModelMatrixComponent";
				out << YAML::BeginMap;
				ModelMatrixComponent& modelMatC = entity.GetComponent<ModelMatrixComponent>();
				out << YAML::Key << "Locale" << YAML::Value << modelMatC.m_Locale;
				out << YAML::Key << "Globle" << YAML::Value << modelMatC.m_Global;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<ViewMatrixComponent>())
			{
				out << YAML::Key << "ViewMatrixComponent";
				out << YAML::BeginMap;
				ViewMatrixComponent& viewMatC = entity.GetComponent<ViewMatrixComponent>();
				out << YAML::Key << "Locale" << YAML::Value << viewMatC.m_Locale;
				out << YAML::Key << "Globle" << YAML::Value << viewMatC.m_Global;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<ModelMangerComponent>())
			{
				out << YAML::Key << "StaticMeshComponent";
				out << YAML::BeginMap;
				ModelMangerComponent& staticMeshC = entity.GetComponent<ModelMangerComponent>();						
				const Ref<MeshStatic>& meshStatic = staticMeshC.m_MeshStatic;

				if (nullptr != meshStatic)
				{
					const AssetHandle& handle = meshStatic->Handle;
					SerializerAssetFormate(out, "StaticMesh", handle, AssetType::MeshStatic);
				}
				out << YAML::EndMap;
			}

			if (entity.HasComponent<TextComponent>())
			{
				out << YAML::Key << "TextComponent";
				out << YAML::BeginMap;
				TextComponent& textC = entity.GetComponent<TextComponent>();


				out << YAML::Key << "TextString" << YAML::Value << textC.m_TextString.c_str();
				out << YAML::Key << "Color" << YAML::Value << textC.m_Color;
				out << YAML::Key << "Kerning" << YAML::Value << textC.m_Kerning;
				out << YAML::Key << "LineSpacing" << YAML::Value << textC.m_LineSpacing;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<DirectionLightComponent>())
			{
				out << YAML::Key << "DrirektionleLigthComponent";
				out << YAML::BeginMap;
				DirectionLightComponent& drirektionleC = entity.GetComponent<DirectionLightComponent>();

				out << YAML::Key << "Color" << YAML::Value << drirektionleC.m_Color;
				out << YAML::Key << "Intensitie" << YAML::Value << drirektionleC.m_Intensity;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<PointLightComponent>())
			{
				out << YAML::Key << "PointLigthComponent";
				out << YAML::BeginMap;
				PointLightComponent& pointC = entity.GetComponent<PointLightComponent>();

				out << YAML::Key << "Color" << YAML::Value << pointC.m_Color;
				out << YAML::Key << "Intensitie" << YAML::Value << pointC.m_Intensity;
				out << YAML::Key << "Distence" << YAML::Value << pointC.m_Distance;
				out << YAML::Key << "Quadratic" << YAML::Value << pointC.m_Quadratic;
				out << YAML::Key << "Linear" << YAML::Value << pointC.m_Linear;
				out << YAML::Key << "Constant" << YAML::Value << pointC.m_Constant;


				out << YAML::EndMap;
			}

			if (entity.HasComponent<SpotLightComponent>())
			{
				out << YAML::Key << "SpotLigthComponent";
				out << YAML::BeginMap;
				SpotLightComponent& spotC = entity.GetComponent<SpotLightComponent>();

				out << YAML::Key << "Color" << YAML::Value << spotC.m_Color;
				out << YAML::Key << "Intensitie" << YAML::Value << spotC.m_Intensity;
				out << YAML::Key << "Distence" << YAML::Value << spotC.m_Distance;
				out << YAML::Key << "Inner" << YAML::Value << spotC.m_Inner;
				out << YAML::Key << "Outer" << YAML::Value << spotC.m_Outer;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<FrameBufferComponent>())
			{
				out << YAML::Key << "FrameBufferComponent";
				{
					
					FrameBufferComponent& frameC = entity.GetComponent<FrameBufferComponent>();
					

					out << YAML::BeginMap;

					
					const FramebufferSpecification& spec = frameC.m_FrameBuffer->GetFramebufferSpecification();
					out << YAML::Key << "FramebufferSpecifcation" << spec;
					out << YAML::Key << "ClearColor" << YAML::Value << frameC.m_ClearColor;
					out << YAML::Key << "FramebufferSize" << YAML::Value << (int)frameC.m_FramebufferSize;


					out << YAML::EndMap;
				}
			}

			out << YAML::EndMap;

			RY_LOG_ENABLE_NUMBER;

		}

	}

	namespace Deserialize {
		static void DeserializeDynamicData(YAML::Node& node, ShaderDataType type, std::vector<unsigned char>& data)
		{
			data.resize(ShaderDataTypeSize(type));
			switch (type)
			{
			case ShaderDataType::Float:
			{
				float* dataV = (float*)data.data();
				*dataV = node.as<float>();
				break;
			}
			case ShaderDataType::Float2:
			{
				glm::vec<2, float>* dataV = (glm::vec<2, float>*)data.data();
				*dataV = node.as<glm::vec<2, float>>();
				break;
			}
			case ShaderDataType::Float3:
			{
				glm::vec<3, float>* dataV = (glm::vec<3, float>*)data.data();
				*dataV = node.as<glm::vec<3, float>>();
				break;
			}
			case ShaderDataType::Float4:
			{
				glm::vec<4, float>* dataV = (glm::vec<4, float>*)data.data();
				*dataV = node.as<glm::vec<4, float>>();
				break;
			}
			case ShaderDataType::Float3x3:
			{
				glm::mat<3, 3, float>* dataV = (glm::mat<3, 3, float>*)data.data();
				*dataV = node.as<glm::mat<3, 3, float>>();
				break;
			}
			case ShaderDataType::Float4x4:
			{
				glm::mat<4, 4, float>* dataV = (glm::mat<4, 4, float>*)data.data();
				*dataV = node.as<glm::mat<4, 4, float>>();
				break;
			}
			case ShaderDataType::Int:
			{
				int* dataV = (int*)data.data();
				*dataV = node.as<int>();
				break;
			}
			case ShaderDataType::Int2:
			{
				glm::vec<2, int>* dataV = (glm::vec<2, int>*)data.data();
				*dataV = node.as<glm::vec<2, int>>();
				break;
			}
			case ShaderDataType::Int3:
			{
				glm::vec<3, int>* dataV = (glm::vec<3, int>*)data.data();
				*dataV = node.as<glm::vec<3, int>>();
				break;
			}
			case ShaderDataType::Int4:
			{
				glm::vec<4, int>* dataV = (glm::vec<4, int>*)data.data();
				*dataV = node.as<glm::vec<4, int>>();
				break;
			}
			}
		}

		static void DeserializeDynamicDataLayout(YAML::Node& node, const BufferLayout& layout, std::vector<unsigned char>& data)
		{
			uint32_t size = layout.GetStride();
			data.resize(size * node.size());
			uint32_t offset = 0;
			uint32_t index = 0;
			const std::vector<BufferElement>& elements = layout.GetElements();
			uint32_t elementsSize = elements.size();
			for (YAML::detail::iterator_value element : node)
			{
				switch (elements[(index % elementsSize)].m_Type)
				{
				case ShaderDataType::Float:
				{
					float* dataV = (float*)(data.data() + offset);
					*dataV = element.as<float>();
					break;
				}
				case ShaderDataType::Float2:
				{
					glm::vec<2, float>* dataV = (glm::vec<2, float>*)(data.data() + offset);
					*dataV = element.as<glm::vec<2, float>>();
					break;
				}
				case ShaderDataType::Float3:
				{
					glm::vec<3, float>* dataV = (glm::vec<3, float>*)(data.data() + offset);
					*dataV = element.as<glm::vec<3, float>>();
					break;
				}
				case ShaderDataType::Float4:
				{
					glm::vec<4, float>* dataV = (glm::vec<4, float>*)(data.data() + offset);
					*dataV = element.as<glm::vec<4, float>>();
					break;
				}
				case ShaderDataType::Float3x3:
				{
					glm::mat<3, 3, float>* dataV = (glm::mat<3, 3, float>*)(data.data() + offset);
					*dataV = element.as<glm::mat<3, 3, float>>();
					break;
				}
				case ShaderDataType::Float4x4:
				{
					glm::mat<4, 4, float>* dataV = (glm::mat<4, 4, float>*)(data.data() + offset);
					*dataV = element.as<glm::mat<4, 4, float>>();
					break;
				}
				case ShaderDataType::Int:
				{
					int* dataV = (int*)(data.data() + offset);
					*dataV = element.as<int>();
					break;
				}
				case ShaderDataType::Int2:
				{
					glm::vec<2, int>* dataV = (glm::vec<2, int>*)(data.data() + offset);
					*dataV = element.as<glm::vec<2, int>>();
					break;
				}
				case ShaderDataType::Int3:
				{
					glm::vec<3, int>* dataV = (glm::vec<3, int>*)(data.data() + offset);
					*dataV = element.as<glm::vec<3, int>>();
					break;
				}
				case ShaderDataType::Int4:
				{
					glm::vec<4, int>* dataV = (glm::vec<4, int>*)(data.data() + offset);
					*dataV = element.as<glm::vec<4, int>>();
					break;
				}
				}
				index++;
			}
		}

		template< typename T>
		static bool DeserializeAssetFormate(YAML::Node& nodeE, Ref<T>*entityC, bool async)
		{
			if (!nodeE)
				return false;

			std::string path;
			std::string makredPathStr;
			if (YAML::Node nodeAtribut = nodeE["Path"])
				path = nodeAtribut.as<std::string>();
			if (YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
				makredPathStr = nodeAtribut.as<std::string>();

			AssetHandle handle = nodeE["Handle"].as<uint64_t>();
			AssetFindeInfo info = AssetFindeInfo(handle, makredPathStr, path);
			*entityC = AssetManager::FindAsset<T>(info);
			
			return true;
		}

		template<typename Comp, typename T>
		static Ref<LodePromisType<T, Scene, int>> DeserializeAssetFormate(YAML::Node & nodeE, const Entity & entity, AssetType type)
		{
			Ref<SceneLodePromisType<T>> loadePromis = Ref<SceneLodePromisType<T>>(nullptr);
			if (!nodeE)
				return loadePromis;

			std::string path = "";
			std::string makredPathStr = "";
			if (YAML::Node nodeAtribut = nodeE["Path"])
				path = nodeAtribut.as<std::string>();
			if (YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
				makredPathStr = nodeAtribut.as<std::string>();
			AssetHandle handle = nodeE["Handle"].as<uint64_t>();

			AssetFindeInfo info(handle, makredPathStr, path);

			int entityID = entity.GetEntityHandle();
			Ref<Scene> scene = entity.GetScene();


			std::function<void(AssetHandle handle)> func = [&loadePromisRefL = loadePromis, sceneL = scene, entityIDL = entityID]
			(AssetHandle handle)
				{
					std::function<void(Ref<T>, Ref<Scene>, int)> onSceneAssetLoadedEntityFunc = Entity::OnAssetLoded<Comp, T>;
					loadePromisRefL = CreateRef<SceneLodePromisType<T>>(sceneL, onSceneAssetLoadedEntityFunc, entityIDL);
					AssetManager::GetAssetAsyncPromis<T, Scene, int>(handle, loadePromisRefL);
				};
			AssetManager::FindeAssetAsync(info, func);

			return loadePromis;
		}

		static bool DeserializeModelDyanmic(YAML::Node & nodeE, DynamicMeshComponent & dMeshC, bool async)
		{

		}
	}
}

	


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}


	


	void SceneSerializer::Serialize(const std::filesystem::path& path)
	{
		RY_CORE_WARN("Begin Serialize a Scene from '{}'", path);
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untiteld";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				Utils::Serializer::SerializerEntity(out, entity, m_Scene);
			});
		out << YAML::EndMap;

		std::ofstream fout(path);
		RY_CORE_ASSERT(fout);
		fout << out.c_str();

		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Scene.get() };
			entity.UpdateMatrix();
		});
		RY_CORE_INFO("Ende Scene Serializetation");
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& path)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime not Implementet!");
	}


	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		RY_CORE_WARN("Begin Serialize a Scene from '{}'", path);

		m_Scene->ClearAll();
		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		Ref<EditorAssetManagerThread> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		RY_CORE_ASSERT("Deserialize scene '{0}'", sceneName);
		std::vector<Ref<LodePromis<Scene>>>& lodingPromisVec = m_Scene->m_LoadingPromisVec;
		lodingPromisVec.clear();

		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (YAML::detail::iterator_value entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				YAML::Node tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				Entity deserializedEntity = m_Scene->CreateEntityWitheUUID(uuid, name);


				if (YAML::Node transformComponent = entity["TransformComponent"])
				{
					// Entities always have transforms
					TransformComponent& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.m_Transform = transformComponent["Transaltion"].as<glm::vec3>();
					tc.m_Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.m_Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				if (YAML::Node realtionShipComponent = entity["RealtionShipComponent"])
				{
					RelationshipUUIDComponent& rSc = deserializedEntity.GetComponent<RelationshipUUIDComponent>();
					if(realtionShipComponent["Parent"])
					{
						UUID parent = realtionShipComponent["Parent"].as<uint64_t>();
						Entity parentEntity = m_Scene->GetEntitiyByUUID(parent);
						if(parentEntity)
						{
							rSc.m_Parent = parent;
							RelationshipUUIDComponent& rSCparent = parentEntity.GetComponent<RelationshipUUIDComponent>();
							UUID id = deserializedEntity.GetUUID();
							rSCparent.m_Childrens.push_back(id);
						}

					} 
					else if(realtionShipComponent["ParentID"])
					{
						rSc.m_Parent = realtionShipComponent["ParentID"].as<uint64_t>();
						YAML::Node childNodes = realtionShipComponent["ChildrenIDs"];
						uint32_t size = childNodes.size();
						rSc.m_Childrens.reserve(size);
						for (YAML::Node childsIDsNode : childNodes)
						{
							rSc.m_Childrens.push_back(childsIDsNode.as<uint64_t>());
						}
					}

					
				}

				if (YAML::Node cameraComponent = entity["CameraComponent"])
				{
					CameraComponent& cc = deserializedEntity.AddComponent<CameraComponent>();

					YAML::Node cameraProps = cameraComponent["Camera"];
					cc.m_Camera = cameraProps.as<SceneCamera>();
					cc.m_Primary = cameraComponent["Primary"].as<bool>();
					cc.m_FixedAspectRotation = cameraComponent["FixedAspectRotaion"].as<bool>();
				}

				if (YAML::Node scriptComponent = entity["ScriptComponent"])
				{
					ScriptComponent& tc = deserializedEntity.AddComponent<ScriptComponent>();
					tc.m_Name = scriptComponent["ClassName"].as<std::string>();
				}

				if (YAML::Node spriteRendererComponent = entity["SpriteRendererComponent"])
				{
					SpriteRendererComponent& sc = deserializedEntity.AddComponent<SpriteRendererComponent>();
					sc.m_Color = spriteRendererComponent["Color"].as<glm::vec4>();
					YAML::Node spriteRendererComponentTexture = spriteRendererComponent["Texture"];
					RefSceneLodePromisType<Texture> promis = Utils::Deserialize::DeserializeAssetFormate<SpriteRendererComponent, Texture>(spriteRendererComponentTexture, deserializedEntity, AssetType::Texture2D);
					if (nullptr != promis)
						lodingPromisVec.emplace_back(promis);
				}
				


				if (YAML::Node modelMatrixComponentN = entity["ModelMatrixComponent"])
				{
					if(!deserializedEntity.HasComponent<ModelMatrixComponent>())
						deserializedEntity.AddComponent<ModelMatrixComponent>();
					ModelMatrixComponent& m4c = deserializedEntity.GetComponent<ModelMatrixComponent>();
					if(modelMatrixComponentN["Matrix4x4"])
					{
						m4c.m_Locale = modelMatrixComponentN["Matrix4x4"].as<glm::mat4>();
						m4c.m_Global = modelMatrixComponentN["GlobleMatrix4x4"].as<glm::mat4>();
						RY_CORE_WARN("Old ModelMatrixComponent Name Confention");
					}
					if (modelMatrixComponentN["Locale"])
					{
						m4c.m_Locale = modelMatrixComponentN["Locale"].as<glm::mat4>();
						m4c.m_Global = modelMatrixComponentN["Globle"].as<glm::mat4>();
					}
				}

				if (YAML::Node viewMatrixComponentN = entity["ViewMatrixComponent"])
				{
					if (!deserializedEntity.HasComponent<ViewMatrixComponent>())
						deserializedEntity.AddComponent<ViewMatrixComponent>();
					ViewMatrixComponent& viewMatC = deserializedEntity.GetComponent<ViewMatrixComponent>();
					viewMatC.m_Locale = viewMatrixComponentN["Locale"].as<glm::mat4>();
					viewMatC.m_Global = viewMatrixComponentN["Globle"].as<glm::mat4>();
				}

				if (YAML::Node staticMeshComponent = entity["StaticMeshComponent"])
				{
					ModelMangerComponent& smc = deserializedEntity.AddComponent<ModelMangerComponent>();
					YAML::Node staticMeshComponentStaticMesh = staticMeshComponent["StaticMesh"];
					RefSceneLodePromisType<MeshStatic> promis = Utils::Deserialize::DeserializeAssetFormate<ModelMangerComponent, MeshStatic>(staticMeshComponentStaticMesh, deserializedEntity, AssetType::MeshStatic);
					if(nullptr != promis)
						lodingPromisVec.emplace_back(promis);
				}


				if (YAML::Node textComponent = entity["TextComponent"])
				{
					deserializedEntity.AddComponent<TextComponent>();
					TextComponent& textC = deserializedEntity.GetComponent<TextComponent>();
					textC.m_FontAsset = Font::GetDefault();
					textC.m_TextString = textComponent["TextString"].as<std::string>();
					textC.m_Color = textComponent["Color"].as<glm::vec4>();
					
					textC.m_LineSpacing = textComponent["LineSpacing"].as<float>();
					textC.m_Kerning = textComponent["Kerning"].as<float>();

					RY_CORE_ASSERT(deserializedEntity.HasComponent<TextComponent>())
				}

				if (YAML::Node drirektionleComponent = entity["DrirektionleLigthComponent"])
				{
					DirectionLightComponent& drirektionleC = deserializedEntity.AddComponent<DirectionLightComponent>();
					drirektionleC.m_Color = drirektionleComponent["Color"].as<glm::vec3>();
					drirektionleC.m_Intensity = drirektionleComponent["Intensitie"].as<float>();
				}

				if (YAML::Node pointLigthComponent = entity["PointLigthComponent"])
				{
					PointLightComponent& pointLigthC = deserializedEntity.AddComponent<PointLightComponent>();
					pointLigthC.m_Color = pointLigthComponent["Color"].as<glm::vec3>();
					pointLigthC.m_Distance = pointLigthComponent["Distence"].as<float>();

					if(YAML::Node constantCompN = pointLigthComponent["Constant"])
						pointLigthC.m_Constant = constantCompN.as<float>();

					if (YAML::Node intensitieCompN = pointLigthComponent["Intensitie"])
						pointLigthC.m_Intensity = intensitieCompN.as<float>();

					if (YAML::Node quadraticCompN = pointLigthComponent["Quadratic"])
						pointLigthC.m_Quadratic = quadraticCompN.as<float>();

				}

				if (YAML::Node spotLigthComponent = entity["SpotLigthComponent"])
				{
					SpotLightComponent& spotLigthC = deserializedEntity.AddComponent<SpotLightComponent>();
					spotLigthC.m_Color = spotLigthComponent["Color"].as<glm::vec3>();
					spotLigthC.m_Intensity = spotLigthComponent["Intensitie"].as<float>();
					spotLigthC.m_Distance = spotLigthComponent["Distence"].as<float>();

					spotLigthC.m_Inner = spotLigthComponent["Inner"].as<float>();
					spotLigthC.m_Outer = spotLigthComponent["Outer"].as<float>();
					
				}

				if (YAML::Node frameBufferComponent = entity["FrameBufferComponent"])
				{
					FrameBufferComponent& frameC = deserializedEntity.AddComponent<FrameBufferComponent>();

					if (YAML::Node framebufferSpecifcationNode = frameBufferComponent["FramebufferSpecifcation"])
					{
						FramebufferSpecification frame = framebufferSpecifcationNode.as<FramebufferSpecification>();
						frameC.m_FrameBuffer = Framebuffer::Create(frame);
					}
					frameC.m_ClearColor = frameBufferComponent["ClearColor"].as<glm::vec3>();
					frameC.m_FramebufferSize = (FrameBufferImageSize)frameBufferComponent["FramebufferSize"].as<int>();

				}
	
		}

			
			
		}
		RY_CORE_INFO("Ende Scene Deserializetion");
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& path)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime not Implementet!");
		return false;
	}

}




