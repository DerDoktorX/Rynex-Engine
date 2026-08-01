#include "rypch.h"
#include "SceneSerializer.h"
#include "YAML.h"

#include <Rynex/Scene/Entity.h>
#include <Rynex/Scene/Components.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/API/Buffer.h>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>


#include <magic_enum.hpp>



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
		GeomtryComponent GeomtryComponent;
		MaterialComponent MaterialComponent;
		RealtionShipUUIDComponent RealtionShipComponent;
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

				switch (ellement.type)
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
			Ref<EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();
			AssetMetadata metadata = editorAssetManger->GetMetadata(handle);
			const std::filesystem::path& filePath = metadata.FilePath;
			const std::filesystem::path& pathMarked = metadata.PathMarker;

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
			Ref<EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();
			if (!editorAssetManger->IsAssetHandleValid(handle))
			{
				RY_CORE_ERROR("Asset has no vild Handle Serialized in {}", name);
				return;
			}

			AssetMetadata metadata = editorAssetManger->GetMetadata(handle);

			if (metadata.Type != type)
			{
				std::string_view metadataType = Asset::AssetTypeToString(metadata.Type);
				std::string_view needMetadataType = Asset::AssetTypeToString(type);
				RY_CORE_ERROR("Asset not Serialized in {} ({} == {})", name, metadataType, needMetadataType);
				return;
			}
			const std::filesystem::path& filePath = metadata.FilePath;
			const std::filesystem::path& pathMarked = metadata.PathMarker;
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

#if RY_EDITOR_ASSETMANGER_THREADE
			Ref<EditorAssetManegerThreade> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#else
			Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#endif


			RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
			out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

			if (entity.HasComponent<TagComponent>())
			{
				out << YAML::Key << "TagComponent";
				out << YAML::BeginMap;

				std::string& tag = entity.GetComponent<TagComponent>().Tag;
				out << YAML::Key << "Tag" <<  tag;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<TransformComponent>())
			{
				out << YAML::Key << "TransformComponent";
				out << YAML::BeginMap;

				TransformComponent& tc = entity.GetComponent<TransformComponent>();
				out << YAML::Key << "Transaltion" << tc.Transaltion;
				out << YAML::Key << "Rotation" << tc.Rotation;
				out << YAML::Key << "Scale" << tc.Scale;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<CameraComponent>())
			{
				out << YAML::Key << "CameraComponent";
				out << YAML::BeginMap;

				CameraComponent& cc = entity.GetComponent<CameraComponent>();
				SceneCamera& camera = cc.Camera;
#if 0
				out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap;
				out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
				out << YAML::Key << "PerspectivVerticleFOV" << YAML::Value << camera.GetPerspectivVerticleFOV();
				out << YAML::Key << "PerspectivNearClipe" << YAML::Value << camera.GetPerspectivNearClipe();
				out << YAML::Key << "PerspectivFarClipe" << YAML::Value << camera.GetPerspectivFarClipe();
				out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
				out << YAML::Key << "OrthographicNearClipe" << YAML::Value << camera.GetOrthographicNearClipe();
				out << YAML::Key << "OrthographicFarClipe" << YAML::Value << camera.GetOrthographicFarClipe();
				out << YAML::EndMap;
#else
				out << YAML::Key << "Camera" << camera;
#endif
				out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
				out << YAML::Key << "FixedAspectRotaion" << YAML::Value << cc.FixedAspectRotaion;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<ScriptComponent>())
			{
				out << YAML::Key << "ScriptComponent";
				out << YAML::BeginMap;
				ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
				out << YAML::Key << "ClassName" << YAML::Value << sc.Name;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				out << YAML::Key << "SpriteRendererComponent";
				out << YAML::BeginMap;

				SpriteRendererComponent& sc = entity.GetComponent<SpriteRendererComponent>();
				out << YAML::Key << "Color" << YAML::Value << sc.Color;
#if RY_DISABLE_WEAK_PTR
				if (sc.Texture)
				{
					SerializerAssetFormate(out, "Texture", sc.Texture->Handle);
				}
#else
				if (Ref<Texture> tex = sc.Texture.lock())
				{
					SerializerAssetFormate(out, "Texture", tex->Handle);
				}
#endif
				out << YAML::EndMap;
			}


			if (entity.HasComponent<RealtionShipUUIDComponent>())
			{
				RealtionShipUUIDComponent& rSc = entity.GetComponent<RealtionShipUUIDComponent>();

				// UUID parent = rSc.ParentID;
				// UUID previus = rSc.PreviusID;
				// UUID first = rSc.FirstID;
				// UUID next = rSc.NextID;
				// out << YAML::Key << "RealtionShipComponent";
				// out << YAML::BeginMap;
				// out << YAML::Key << "Previus" << YAML::Value << previus;
				// out << YAML::Key << "First" << YAML::Value << first;
				// out << YAML::Key << "Next" << YAML::Value << next;
				// out << YAML::Key << "Parent" << YAML::Value << parent;
				// out << YAML::EndMap;

				out << YAML::Key << "RealtionShipComponent";
				out << YAML::BeginMap;
				out << YAML::Key << "ParentID" << YAML::Value << rSc.parent;
				out << YAML::Key << "ChildrenIDs" << YAML::Value;
				out << YAML::Flow;
				out << YAML::BeginSeq;
				for (UUID& idChild : rSc.childrens)
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
				out << YAML::Key << "Locale" << YAML::Value << modelMatC.Locale;
				out << YAML::Key << "Globle" << YAML::Value << modelMatC.Globle;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<ViewMatrixComponent>())
			{
				out << YAML::Key << "ViewMatrixComponent";
				out << YAML::BeginMap;
				ViewMatrixComponent& viewMatC = entity.GetComponent<ViewMatrixComponent>();
				out << YAML::Key << "Locale" << YAML::Value << viewMatC.Locale;
				out << YAML::Key << "Globle" << YAML::Value << viewMatC.Globle;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<ModelMangerComponent>())
			{
				out << YAML::Key << "StaticMeshComponent";
				out << YAML::BeginMap;
				ModelMangerComponent& staticMeshC = entity.GetComponent<ModelMangerComponent>();						
				const Ref<MeshStatic>& meshStatic = staticMeshC.meshStatic;

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


				out << YAML::Key << "TextString" << YAML::Value << textC.TextString.c_str();
				out << YAML::Key << "Color" << YAML::Value << textC.Color;
				out << YAML::Key << "Kerning" << YAML::Value << textC.Kerning;
				out << YAML::Key << "LineSpacing" << YAML::Value << textC.LineSpacing;
				out << YAML::EndMap;
			}

			if (entity.HasComponent<DrirectionleLigthComponent>())
			{
				out << YAML::Key << "DrirektionleLigthComponent";
				out << YAML::BeginMap;
				DrirectionleLigthComponent& drirektionleC = entity.GetComponent<DrirectionleLigthComponent>();

				out << YAML::Key << "Color" << YAML::Value << drirektionleC.color;
				out << YAML::Key << "Intensitie" << YAML::Value << drirektionleC.intensitie;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<PointLigthComponent>())
			{
				out << YAML::Key << "PointLigthComponent";
				out << YAML::BeginMap;
				PointLigthComponent& pointC = entity.GetComponent<PointLigthComponent>();

				out << YAML::Key << "Color" << YAML::Value << pointC.color;
				out << YAML::Key << "Intensitie" << YAML::Value << pointC.intensitie;
				out << YAML::Key << "Distence" << YAML::Value << pointC.distence;
				out << YAML::Key << "Quadratic" << YAML::Value << pointC.quadratic;
				out << YAML::Key << "Linear" << YAML::Value << pointC.linear;
				out << YAML::Key << "Constant" << YAML::Value << pointC.constant;


				out << YAML::EndMap;
			}

			if (entity.HasComponent<SpotLigthComponent>())
			{
				out << YAML::Key << "SpotLigthComponent";
				out << YAML::BeginMap;
				SpotLigthComponent& spotC = entity.GetComponent<SpotLigthComponent>();

				out << YAML::Key << "Color" << YAML::Value << spotC.color;
				out << YAML::Key << "Intensitie" << YAML::Value << spotC.intensitie;
				out << YAML::Key << "Distence" << YAML::Value << spotC.distence;
				out << YAML::Key << "Inner" << YAML::Value << spotC.inner;
				out << YAML::Key << "Outer" << YAML::Value << spotC.outer;

				out << YAML::EndMap;
			}

			if (entity.HasComponent<FrameBufferComponent>())
			{
				out << YAML::Key << "FrameBufferComponent";
				{
					
					FrameBufferComponent& frameC = entity.GetComponent<FrameBufferComponent>();
					

					out << YAML::BeginMap;
#if 0				
					out << YAML::Key << "FramebufferSpecifcation";

					{


						out << YAML::BeginMap;
						const FramebufferSpecification& spec = frameC.FrameBuffer->GetFramebufferSpecification();
						out << YAML::Key << "Width" << YAML::Value << spec.Width;
						out << YAML::Key << "Height" << YAML::Value << spec.Height;

						{
							out << YAML::Key << "Attachments";
							out << YAML::BeginSeq;
							for (const FramebufferTextureSpecification& framTexSpec : spec.Attachments.Attachments)
							{
								// out << YAML::Key << "TextureAssetHandle" << YAML::Value << AssetHandle();
								out << YAML::BeginMap;
								out << YAML::Key << "TextureFormat" << YAML::Value << (int)framTexSpec.TextureFormat;
								out << YAML::Key << "TextureWrapping" << YAML::Value;
								out << YAML::BeginMap;
								{
									out << YAML::Key << "R" << YAML::Value << (int)framTexSpec.TextureWrapping.R;
									out << YAML::Key << "T" << YAML::Value << (int)framTexSpec.TextureWrapping.T;
									out << YAML::Key << "S" << YAML::Value << (int)framTexSpec.TextureWrapping.S;
								}
								out << YAML::EndMap;
								out << YAML::Key << "TextureFiltering" << YAML::Value << (int)framTexSpec.TextureFiltering;
								out << YAML::Key << "Samples" << YAML::Value << framTexSpec.Samples;
								out << YAML::EndMap;
							}
							out << YAML::EndSeq;
						}
						out << YAML::Key << "Samples" << YAML::Value << spec.Samples;
						out << YAML::Key << "SwapChainTarget" << YAML::Value << spec.SwapChainTarget;

						out << YAML::EndMap;

					}
#else
					
					const FramebufferSpecification& spec = frameC.FrameBuffer->GetFramebufferSpecification();
					out << YAML::Key << "FramebufferSpecifcation" << spec;
#endif
					out << YAML::Key << "ClearColor" << YAML::Value << frameC.ClearColor;
					out << YAML::Key << "FramebufferSize" << YAML::Value << (int)frameC.FramebufferSize;


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
#if 1
			for (YAML::detail::iterator_value& element : node)
			{
#else

			int count = node.size();
			for (int i = count; 0 < i; i--)
			{
				YAML::detail::iterator_value& element = node[i];
#endif
				switch (elements[(index % elementsSize)].type)
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

			std::string path = "";
			std::string makredPath = "";
			if (YAML::Node& nodeAtribut = nodeE["Path"])
				path = nodeAtribut.as<std::string>();
			if (YAML::Node& nodeAtribut = nodeE["Path-ProjectMarker"])
				makredPath = nodeAtribut.as<std::string>();

			AssetHandle handle = nodeE["Handle"].as<uint64_t>();
			AssetFindeInfo info = AssetFindeInfo(handle, makredPath, path);
			*entityC = AssetManager::FindeAsset<T>(info);
			
			return true;
		}

		template<typename Comp, typename T>
		static Ref<LodePromisType<T, Scene, int>> DeserializeAssetFormate(YAML::Node & nodeE, const Entity & entity, AssetType type)
		{
			Ref<SceneLodePromisType<T>> loadePromis = Ref<SceneLodePromisType<T>>(nullptr);
			if (!nodeE)
				return loadePromis;

			std::string path = "";
			std::string makredPath = "";
			if (YAML::Node& nodeAtribut = nodeE["Path"])
				path = nodeAtribut.as<std::string>();
			if (YAML::Node& nodeAtribut = nodeE["Path-ProjectMarker"])
				makredPath = nodeAtribut.as<std::string>();
			AssetHandle handle = nodeE["Handle"].as<uint64_t>();

			AssetFindeInfo info = AssetFindeInfo(handle, makredPath, path);

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
		RY_LOG_DISABLE_NUMBER;

		RY_CORE_WARN("Begin Serialize a Scene from '{}'", path);
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untiteld";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registery.each([&](auto entityID)
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

		m_Scene->m_Registery.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Scene.get() };
			entity.UpdateMatrix();
		});
		RY_CORE_INFO("Ende Scene Serializetation");
		RY_LOG_ENABLE_NUMBER;

	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& path)
	{
		RY_LOG_DISABLE_NUMBER;

		RY_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime not Implementet!");
		
		
		RY_LOG_ENABLE_NUMBER;
	}


	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		RY_LOG_DISABLE_NUMBER;

		RY_CORE_WARN("Begin Serialize a Scene from '{}'", path);

		m_Scene->ClearAll();
		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		Ref<EditorAssetManegerThreade> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		RY_CORE_ASSERT("Deserialize scene '{0}'", sceneName);
		std::vector<Ref<LodePromis<Scene>>>& lodingPromisVec = m_Scene->m_LodingPromisVec;
		// std::vector<Ref<LodePromis>> lodingPromisVec;
		lodingPromisVec.clear();

		YAML::Node entities = data["Entities"];
		if (entities)
		{
#if 1
			for (YAML::detail::iterator_value entity : entities)
			{
#else
			YAML::iterator itBegin = entities.begin();
			YAML::iterator itEnde = entities.end() - 1;
			itEnde--;
			for (int i = entities.size(); 0 < i; i--)
			{
				YAML::detail::iterator_value& entity = entities[i];
#endif
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				YAML::Node tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				// RY_CORE_ASSERT(sceneName, "Deserializing entity withe ID = {0}, name = {1}");

				Entity deserializedEntity = m_Scene->CreateEntityWitheUUID(uuid, name);


				if (YAML::Node transformComponent = entity["TransformComponent"])
				{
					// Entities always have transforms
					TransformComponent& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Transaltion = transformComponent["Transaltion"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				if (YAML::Node realtionShipComponent = entity["RealtionShipComponent"])
				{
					RealtionShipUUIDComponent& rSc = deserializedEntity.GetComponent<RealtionShipUUIDComponent>();
					if(realtionShipComponent["Parent"])
					{
						UUID parent = realtionShipComponent["Parent"].as<uint64_t>();
						Entity parentEntity = m_Scene->GetEntitiyByUUID(parent);
						if(parentEntity)
						{
							rSc.parent = parent;
							RealtionShipUUIDComponent& rSCparent = parentEntity.GetComponent<RealtionShipUUIDComponent>();
							UUID id = deserializedEntity.GetUUID();
							rSCparent.childrens.push_back(id);
						}

					} 
					else if(realtionShipComponent["ParentID"])
					{
						rSc.parent = realtionShipComponent["ParentID"].as<uint64_t>();
						YAML::Node& childNodes = realtionShipComponent["ChildrenIDs"];
						uint32_t size = childNodes.size();
						rSc.childrens.reserve(size);
						for (YAML::Node& childsIDsNode : childNodes)
						{
							rSc.childrens.push_back(childsIDsNode.as<uint64_t>());
						}
					}

					
				}

				if (YAML::Node cameraComponent = entity["CameraComponent"])
				{
					CameraComponent& cc = deserializedEntity.AddComponent<CameraComponent>();

					YAML::Node& cameraProps = cameraComponent["Camera"];
					
#if 0
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectivVerticleFOV(cameraProps["PerspectivVerticleFOV"].as<float>());
					cc.Camera.SetPerspectivNearClipe(cameraProps["PerspectivNearClipe"].as<float>());
					cc.Camera.SetPerspectivFarClipe(cameraProps["PerspectivFarClipe"].as<float>());

					cc.Camera.SetOrthograficSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthograficNearClipe(cameraProps["OrthographicNearClipe"].as<float>());
					cc.Camera.SetOrthograficFarClipe(cameraProps["OrthographicFarClipe"].as<float>());
#else
					cc.Camera = cameraProps.as<SceneCamera>();
#endif
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRotaion = cameraComponent["FixedAspectRotaion"].as<bool>();
				}

				if (YAML::Node scriptComponent = entity["ScriptComponent"])
				{
					ScriptComponent& tc = deserializedEntity.AddComponent<ScriptComponent>();
					tc.Name = scriptComponent["ClassName"].as<std::string>();
				}

				if (YAML::Node spriteRendererComponent = entity["SpriteRendererComponent"])
				{
					SpriteRendererComponent& sc = deserializedEntity.AddComponent<SpriteRendererComponent>();
					sc.Color = spriteRendererComponent["Color"].as<glm::vec4>();

					RefSceneLodePromisType<Texture> promis = Utils::Deserialize::DeserializeAssetFormate<SpriteRendererComponent, Texture>(spriteRendererComponent["Texture"], deserializedEntity, AssetType::Texture2D);
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
						m4c.Locale = modelMatrixComponentN["Matrix4x4"].as<glm::mat4>();
						m4c.Globle = modelMatrixComponentN["GlobleMatrix4x4"].as<glm::mat4>();
						RY_CORE_WARN("Old ModelMatrixComponent Name Confention");
					}
					if (modelMatrixComponentN["Locale"])
					{
						m4c.Locale = modelMatrixComponentN["Locale"].as<glm::mat4>();
						m4c.Globle = modelMatrixComponentN["Globle"].as<glm::mat4>();
					}
				}

				if (YAML::Node viewMatrixComponentN = entity["ViewMatrixComponent"])
				{
					if (!deserializedEntity.HasComponent<ViewMatrixComponent>())
						deserializedEntity.AddComponent<ViewMatrixComponent>();
					ViewMatrixComponent& viewMatC = deserializedEntity.GetComponent<ViewMatrixComponent>();
					viewMatC.Locale = viewMatrixComponentN["Locale"].as<glm::mat4>();
					viewMatC.Globle = viewMatrixComponentN["Globle"].as<glm::mat4>();
				}

#if 1
				if (YAML::Node staticMeshComponent = entity["StaticMeshComponent"])
				{
					ModelMangerComponent& smc = deserializedEntity.AddComponent<ModelMangerComponent>();
#if 1
					RefSceneLodePromisType<MeshStatic> promis = Utils::Deserialize::DeserializeAssetFormate<ModelMangerComponent, MeshStatic>(staticMeshComponent["StaticMesh"], deserializedEntity, AssetType::MeshStatic);
					if(nullptr != promis)
						lodingPromisVec.emplace_back(promis);
#else
					Utils::DeserializeAssetFormate<MeshStatic>(staticMeshComponent["StaticMesh"], &smc.meshStatic, false);
#endif
				}
#endif
#if 0
				if (YAML::Node dynamicMeshComponent = entity["DynamicMeshComponent"])
				{

					DynamicMeshComponent& dmc = deserializedEntity.AddComponent<DynamicMeshComponent>();

					if (YAML::Node modelDyN = dynamicMeshComponent["Model-D"])
					{
						assetHandleError = Utils::DeserializeModelDyanmic(modelDyN, dmc, async) || assetHandleError;

					}
				}
#endif


				if (YAML::Node textComponent = entity["TextComponent"])
				{
					deserializedEntity.AddComponent<TextComponent>();
					TextComponent& textC = deserializedEntity.GetComponent<TextComponent>();
					textC.FontAsset = Font::GetDefault();
					textC.TextString = textComponent["TextString"].as<std::string>();
					textC.Color = textComponent["Color"].as<glm::vec4>();
					
					textC.LineSpacing = textComponent["LineSpacing"].as<float>();
					textC.Kerning = textComponent["Kerning"].as<float>();

					RY_CORE_ASSERT(deserializedEntity.HasComponent<TextComponent>())
				}

				if (YAML::Node drirektionleComponent = entity["DrirektionleLigthComponent"])
				{
					DrirectionleLigthComponent& drirektionleC = deserializedEntity.AddComponent<DrirectionleLigthComponent>();
					drirektionleC.color = drirektionleComponent["Color"].as<glm::vec3>();
					drirektionleC.intensitie = drirektionleComponent["Intensitie"].as<float>();
				}

				if (YAML::Node pointLigthComponent = entity["PointLigthComponent"])
				{
					PointLigthComponent& pointLigthC = deserializedEntity.AddComponent<PointLigthComponent>();
					pointLigthC.color = pointLigthComponent["Color"].as<glm::vec3>();
					pointLigthC.intensitie = pointLigthComponent["Intensitie"].as<float>();
					pointLigthC.distence = pointLigthComponent["Distence"].as<float>();

					if(YAML::Node constantCompN = pointLigthComponent["Constant"])
						pointLigthC.constant = constantCompN.as<float>();

					if (YAML::Node intensitieCompN = pointLigthComponent["Intensitie"])
						pointLigthC.intensitie = intensitieCompN.as<float>();

					if (YAML::Node quadraticCompN = pointLigthComponent["Quadratic"])
						pointLigthC.quadratic = quadraticCompN.as<float>();

				}

				if (YAML::Node spotLigthComponent = entity["SpotLigthComponent"])
				{
					SpotLigthComponent& spotLigthC = deserializedEntity.AddComponent<SpotLigthComponent>();
					spotLigthC.color = spotLigthComponent["Color"].as<glm::vec3>();
					spotLigthC.intensitie = spotLigthComponent["Intensitie"].as<float>();
					spotLigthC.distence = spotLigthComponent["Distence"].as<float>();

					spotLigthC.inner = spotLigthComponent["Inner"].as<float>();
					spotLigthC.outer = spotLigthComponent["Outer"].as<float>();
					
				}

				if (YAML::Node frameBufferComponent = entity["FrameBufferComponent"])
				{
					FrameBufferComponent& frameC = deserializedEntity.AddComponent<FrameBufferComponent>();
#if 0
					
					if (YAML::Node frameBuffer = frameBufferComponent["FrameBuffer"])
					{
						FramebufferSpecification frame;
						frame.Height = frameBuffer["Height"].as<uint32_t>();
						frame.Width = frameBuffer["Width"].as<uint32_t>();
						uint32_t size = frameBuffer["Attachments"].size();
						frame.Attachments.Attachments.reserve(size);
						for (YAML::Node& attchment : frameBuffer["Attachments"] )
						{
							YAML::Node warp = attchment["TextureWrapping"];

							frame.Attachments.Attachments.emplace_back(FramebufferTextureSpecification(
								(TexFrom)attchment["TextureFormat"].as<int>(),
								attchment["Samples"].as<uint32_t>(),
								{
									(TexWarp)warp["S"].as<int>(),
									(TexWarp)warp["T"].as<int>(),
									(TexWarp)warp["R"].as<int>()
								},
								(TexFilter)attchment["TextureFiltering"].as<int>()
							));
						}
						frameC.FrameBuffer = Framebuffer::Create(frame);
					}
#else
					if (YAML::Node framebufferSpecifcationNode = frameBufferComponent["FramebufferSpecifcation"])
					{
						FramebufferSpecification frame = framebufferSpecifcationNode.as<FramebufferSpecification>();
						frameC.FrameBuffer = Framebuffer::Create(frame);
					}
#endif				
					frameC.ClearColor = frameBufferComponent["ClearColor"].as<glm::vec3>();
					frameC.FramebufferSize = (FrameBufferImageSize)frameBufferComponent["FramebufferSize"].as<int>();

				}
#if 0
				if (YAML::Node frameBufferComponent = entity["RenderTargetComponent"])
				{
					RenderTargetComponent& renderTargetC = deserializedEntity.AddComponent<RenderTargetComponent>();
					if (YAML::Node RenderTargetN = frameBufferComponent["RenderTarget"])
					{
						Ref<RenderTarget> target = CreateRef<RenderTarget>();
						glm::vec4 renderView = RenderTargetN["renderView"].as<glm::vec4>();
						FramebufferSpecification frameSpec;
						
						for (YAML::Node& attchment : RenderTargetN["Attachments"])
						{
							FramebufferTextureSpecification spec;

							spec.TextureFormat;
							std::string textureFormatStr = attchment["TextureFormat"].as<std::string>();
							{
								std::optional<TextureFormat> typeTextureFormat = magic_enum::enum_cast<TextureFormat>(textureFormatStr, magic_enum::case_insensitive);
								if (!typeTextureFormat.has_value())
								{
									RY_CORE_ERROR("No Vaild Texture formate ({})", textureFormatStr);
									continue;
								}

								spec.TextureFormat = typeTextureFormat.value();
							}


							spec.Samples = attchment["Samples"].as<uint32_t>();

							
						}
						frameC.FrameBuffer = Framebuffer::Create(frame);
					}
					frameC.ClearColor = frameBufferComponent["ClearColor"].as<glm::vec3>();
					frameC.FramebufferSize = (FrameBufferImageSize)frameBufferComponent["FramebufferSize"].as<int>();
				}
#endif
	
		}

			
			
		}
		RY_CORE_INFO("Ende Scene Deserializetion");

#if 0
		for(RefSceneLodePromisType<Texture>& promisTex : lodingPromisTypes.PromiseTextureLodeVec)
		{

			if(!promisTex->IsTransferComplet())
				promisTex->WaitForLoding();
		}

		for (RefSceneLodePromisType<MeshStatic>& promisMeshStatic : lodingPromisTypes.PromiseMeshStaticLodeVec)
		{
			if (!promisMeshStatic->IsTransferComplet())
				promisMeshStatic->WaitForLoding();
		}
#endif

#if 0
		for (RefSceneLodePromis& promis : lodingPromisVec)
		{
			if (!promis->IsTransferComplet())
				promis->WaitForLoding();
		}
		lodingPromisVec.clear();
#endif
		
		RY_LOG_ENABLE_NUMBER;

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& path)
	{
		RY_LOG_DISABLE_NUMBER;

		RY_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime not Implementet!");
		RY_LOG_ENABLE_NUMBER;

		
		return false;
	}

}




