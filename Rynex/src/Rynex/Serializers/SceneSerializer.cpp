#include <rypch.h>
#include "SceneSerializer.h"
#include "YAML.h"

#include <Rynex/Scene/Entity.h>
#include <Rynex/Scene/Components.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Serializers/VersionSerializer.h>

#include <yaml-cpp/yaml.h>
#include <fstream>


// #define RY_INLINE_ENTITY_COMPENT_SERIALIZATION
// #define RY_INLINE_ENTITY_COMPENT_DESERIALIZATION
namespace Rynex {


	template<typename T>
	using SceneLodePromisType = LodePromisType<T, Scene, int>;

	template<typename T>
	using RefSceneLodePromisType = Ref<SceneLodePromisType<T>>;

    namespace Utils {
	    namespace Serializer {

	        template<typename T>
	        static void CastDataToValueType(YAML::Emitter& out, const std::string& name, const std::vector<unsigned char>& data)
	        {
	            RY_CORE_ASSERT(data.size() == sizeof(T), "Invalid cast size!");
	            const T* ptr = reinterpret_cast<const T*>(data.data());
	            T value = *ptr;
	            out << YAML::Key << name << YAML::Value << value;
	        }

	    	static void SerializerDynamicData(YAML::Emitter& out, const std::string& name, ShaderDataType type, const std::vector<unsigned char>& data)
	        {
	        	switch (type)
	        	{
	        	case ShaderDataType::Float:
	        	{
	        	    CastDataToValueType<float>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Float2:
	        	{
	        	    CastDataToValueType<glm::vec2>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Float3:
	        	{
	        	    CastDataToValueType<glm::vec3>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Float4:
	        	{
	        	    CastDataToValueType<glm::vec4>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Float3x3:
	        	{
	        	    CastDataToValueType<glm::mat3>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Float4x4:
	        	{
	        	    CastDataToValueType<glm::mat4>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Int:
	        	{
	        	    CastDataToValueType<int>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Int2:
	        	{
	        	    CastDataToValueType<glm::ivec2>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Int3:
	        	{
	        	    CastDataToValueType<glm::ivec3>(out, name, data);
	        		break;
	        	}
	        	case ShaderDataType::Int4:
	        	{
	        	    CastDataToValueType<glm::ivec4>(out, name, data);
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

	        			switch(ellement.m_Type)
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
                        case ShaderDataType::None:
                            break;
                        case ShaderDataType::Int3x3:
                            break;
                        case ShaderDataType::Int4x4:
                            break;
                        case ShaderDataType::Uint:
                            break;
                        case ShaderDataType::Uint2:
                            break;
                        case ShaderDataType::Uint3:
                            break;
                        case ShaderDataType::Uint4:
                            break;
                        case ShaderDataType::Uint3x3:
                            break;
                        case ShaderDataType::Uint4x4:
                            break;
                        case ShaderDataType::Texture:
                            break;
                        case ShaderDataType::Texture2D:
                            break;
                        case ShaderDataType::TextureCube:
                            break;
                        case ShaderDataType::TextureArray:
                            break;
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
	    			RY_CORE_ERROR("Asset has no valid Handle Serialized in {}", name);
	    			return;
	    		}

	    		const AssetMetadata metadata = editorAssetManger->GetMetadata(handle);

	    		if (metadata.m_Type != type)
	    		{
	    			std::string_view metadataType = Asset::AssetTypeToString(metadata.m_Type);
	    			std::string_view needMetadataType = Asset::AssetTypeToString(type);
	    			RY_CORE_ERROR("Asset not Serialized in {} ({} == {})", name, metadataType, needMetadataType);
	    			return;
	    		}
	    		const FileSystem::Path& filePath = metadata.m_Path;


	    		out << YAML::Key << name.c_str();
	    		out << YAML::BeginMap;
	    		out << YAML::Key << "Path" << filePath.GetPathString();
	    		out << YAML::Key << "Path-ProjectMarker" << filePath.GetMarkedPathString();
	    		out << YAML::Key << "Handle" << handle;
	    		out << YAML::EndMap;
	    	}


	        template<typename Component>
            static void SerializerComponent(YAML::Emitter& out, const Component& component, const Ref<Scene>& scene);


	        template<typename Component>
	        void SerializerComponent(YAML::Emitter& out, const Component& component, const Ref<Scene>& scene)
	        {
	            static_assert(false, "No dafault implemtion allwoed!");
	        }

	        template<>
            void SerializerComponent<TagComponent>(YAML::Emitter& out, const TagComponent& component, const Ref<Scene>& scene)
	        {
	            const std::string& tag = component.m_Tag;
	            out << YAML::Key << "Tag" <<  tag;
	        }

	        template<>
            void SerializerComponent<TransformComponent>(YAML::Emitter& out, const TransformComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Transaltion" << component.m_Transform;
	            out << YAML::Key << "Rotation" << component.m_Rotation;
	            out << YAML::Key << "Scale" << component.m_Scale;
	        }

	        template<>
            void SerializerComponent<CameraComponent>(YAML::Emitter& out, const CameraComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Camera" << YAML::Value << component.m_Camera;
	            out << YAML::Key << "Primary" << YAML::Value << component.m_Primary;
	            out << YAML::Key << "FixedAspectRotaion" << YAML::Value << component.m_FixedAspectRotation;
	        }

	        template<>
            void SerializerComponent<ScriptComponent>(YAML::Emitter& out, const ScriptComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "ClassName" << YAML::Value << component.m_Name;
	            out << YAML::Key << "SelectedScript" << YAML::Value << component.m_SelectedScript;
	        }

	        template<>
            void SerializerComponent<SpriteRendererComponent>(YAML::Emitter& out, const SpriteRendererComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Color" << YAML::Value << component.m_Color;
	            if (Ref<Texture> texture = component.m_Texture.lock())
	            {
	                ::Serializer::AssetFormate(out, "Texture",texture->m_Handle);
	            }
	        }

	        template<>
            void SerializerComponent<RelationshipUUIDComponent>(YAML::Emitter& out, const RelationshipUUIDComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "ParentID" << YAML::Value << component.m_Parent;
	            out << YAML::Key << "ChildrenIDs" << YAML::Value;
	            out << YAML::Flow;
	            out << YAML::BeginSeq;
	            for (const UUID& idChild : component.m_Childrens)
	            {
	                out << YAML::Value << idChild;
	            }
	            out << YAML::EndSeq;
	        }

	        template<>
            void SerializerComponent<ModelMatrixComponent>(YAML::Emitter& out, const ModelMatrixComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Locale" << YAML::Value << component.m_Locale;
	            out << YAML::Key << "Global" << YAML::Value << component.m_Global;
	        }

	        template<>
            void SerializerComponent<ViewMatrixComponent>(YAML::Emitter& out, const ViewMatrixComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Locale" << YAML::Value << component.m_Locale;
	            out << YAML::Key << "Global" << YAML::Value << component.m_Global;
	        }

	        template<>
            void SerializerComponent<ModelMangerComponent>(YAML::Emitter& out, const ModelMangerComponent& component, const Ref<Scene>& scene)
	        {
	            const Ref<MeshStatic>& meshStatic = component.m_MeshStatic;

	            if (nullptr != meshStatic)
	            {
	                const AssetHandle& handle = meshStatic->m_Handle;
	                ::Serializer::AssetFormate(out, "StaticMesh", handle);
	            }
	        }

	        template<>
            void SerializerComponent<TextComponent>(YAML::Emitter& out, const TextComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Color" << YAML::Value << component.m_Color;
	            out << YAML::Key << "Kerning" << YAML::Value << component.m_Kerning;
	            out << YAML::Key << "LineSpacing" << YAML::Value << component.m_LineSpacing;
	            out << YAML::Key << "TextString" << YAML::Value << component.m_TextString;
	        }

	        template<>
            void SerializerComponent<DirectionLightComponent>(YAML::Emitter& out, const DirectionLightComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Color" << YAML::Value << component.m_Color;
	            out << YAML::Key << "Intensity" << YAML::Value << component.m_Intensity;
	        }

	        template<>
            void SerializerComponent<PointLightComponent>(YAML::Emitter& out, const PointLightComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Color" << YAML::Value << component.m_Color;
	            out << YAML::Key << "Intensity" << YAML::Value << component.m_Intensity;
	            out << YAML::Key << "Distance" << YAML::Value << component.m_Distance;
	            out << YAML::Key << "Constant" << YAML::Value << component.m_Constant;
	            out << YAML::Key << "Linear" << YAML::Value << component.m_Linear;
	            out << YAML::Key << "Quadratic" << YAML::Value << component.m_Quadratic;
	        }

	        template<>
            void SerializerComponent<SpotLightComponent>(YAML::Emitter& out, const SpotLightComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Color" << YAML::Value << component.m_Color;
	            out << YAML::Key << "Intensity" << YAML::Value << component.m_Intensity;
	            out << YAML::Key << "Distance" << YAML::Value << component.m_Distance;
	            out << YAML::Key << "Inner" << YAML::Value << component.m_Inner;
	            out << YAML::Key << "Outer" << YAML::Value << component.m_Outer;
	        }

	        template<>
            void SerializerComponent<FrameBufferComponent>(YAML::Emitter& out, const FrameBufferComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "ClearColor" << YAML::Value << component.m_ClearColor;
	            out << YAML::Key << "FrameBufferLayoutIndex" << YAML::Value << component.m_FrameBufferLayoutIndex;
	            out << YAML::Key << "FramebufferSize" << YAML::Value << component.m_FramebufferSize;
                if (const Ref<Framebuffer>& framebuffer = component.m_FrameBuffer)
	            {
	                const FramebufferSpecification& framebufferSpecification= framebuffer->GetFramebufferSpecification();
	                out << YAML::Key << "FramebufferSpecification" << YAML::Value << framebufferSpecification;
	            }
	        }

	        template<>
            void SerializerComponent<VisibleComponent>(YAML::Emitter& out, const VisibleComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Visible" << YAML::Value << component.m_Visible;
	        }

	        template<>
            void SerializerComponent<RenderTargetComponent>(YAML::Emitter& out, const RenderTargetComponent& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "RenderPassName" << YAML::Value << component.m_RenderPassName;
	            const Ref<RenderTarget>& renderTarget = component.m_Target;
	            if (nullptr != renderTarget)
	            {
	                out << YAML::Key << "RenderTarget";
	                out << YAML::BeginMap;
	                glm::vec4 viewSize = renderTarget->GetRenderViewSize();
	                out << YAML::Key << "RenderViewSize" << YAML::Value << viewSize;
	                const Ref<Framebuffer>& framebuffer = renderTarget->GetFramebuffer();
	                if (nullptr != framebuffer)
	                {
	                    const FramebufferSpecification& framebufferSpecification= framebuffer->GetFramebufferSpecification();
	                    out << YAML::Key << "FramebufferSpecification" << YAML::Value << framebufferSpecification;
	                }
	                out << YAML::EndMap;
	            }
	        }

#if 0
	        template<>
            void SerializerComponent<EnvironmentMap>(YAML::Emitter& out, const EnvironmentMap& component, const Ref<Scene>& scene)
	        {
	            out << YAML::Key << "Visible" << YAML::Value << component.;
	        }
#endif


	        template<typename Component>
	        static void SerializerSetupComponent(YAML::Emitter& out, const Entity entity, const Ref<Scene>& scene)
	        {
	            if (entity.HasComponent<Component>())
	            {
	                std::string_view viewComponentName = typeid(Component).name();
	                constexpr const char* ptr = "struct Rynex::";
	                constexpr size_t count = 6 + 1 + 5 + 2;
	                const std::string componentName(viewComponentName.begin() + count, viewComponentName.end());

	                out << YAML::Key << componentName;
	                out << YAML::BeginMap;

	                const Component& component = entity.GetComponentC<Component>();
	                SerializerComponent<Component>(out, component, scene);

	                out << YAML::EndMap;
	            }
	        }

		    template<typename ...Component>
	        static void SerializerAnyComponent(YAML::Emitter& out, const Entity entity, const Ref<Scene>& scene)
	        {
	            ([&]()
	            {
	                SerializerSetupComponent<Component>(out, entity, scene);
	            }(), ...);
	        }


	        template<typename... Component>
            static void SerializerGroupComponent(ComponentGroup<Component ...>, YAML::Emitter& out, const Entity entity, const Ref<Scene>& scene)
	        {
	            SerializerAnyComponent<Component...>(out,  entity, scene);
	        }




		    static void SerializerEntity(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene)
		    {
		    	RY_LOG_DISABLE_NUMBER;

		    	Ref<EditorAssetManagerThread> editorAssetManger = Project::GetActive()->GetEditorAssetManger();


		    	RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
		    	out << YAML::BeginMap;
		    	out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
#ifdef RY_INLINE_ENTITY_COMPENT_SERIALIZATION

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
					SerializerAssetFormate(out, "Texture", tex->m_Handle);
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
					const AssetHandle& handle = meshStatic->m_Handle;
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

#else
	            SerializerSetupComponent<TagComponent>(out, entity, scene);
	            SerializerGroupComponent(SerializeComponents{}, out, entity,scene);

#endif

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

		        std::string pathStr;
		        std::string makredPathStr;
		        if (YAML::Node nodeAtribut = nodeE["Path"])
			        pathStr = nodeAtribut.as<std::string>();
		        if (YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
			        makredPathStr = nodeAtribut.as<std::string>();

		        AssetHandle handle = nodeE["Handle"].as<uint64_t>();

		        FileSystem::Path path(pathStr);
		        FileSystem::Path makredPath(makredPathStr);
		        AssetFindeInfo info = AssetFindeInfo(handle, path, makredPath);
		        *entityC = AssetManager::FindAsset<T>(info);

		        return true;
	        }

	        template<typename Comp, typename T>
	        static Ref<LodePromisType<T, Scene, int>> DeserializeAssetFormate(YAML::Node & nodeE, const Entity & entity, AssetType type)
	        {
		        Ref<SceneLodePromisType<T>> loadePromis = Ref<SceneLodePromisType<T>>(nullptr);
		        if (!nodeE)
			        return loadePromis;

		        std::string pathStr;
		        std::string makredPathStr;
		        if (YAML::Node nodeAtribut = nodeE["Path"])
			        pathStr = nodeAtribut.as<std::string>();
		        if (YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
			        makredPathStr = nodeAtribut.as<std::string>();
        #if 1
		        uint64_t version = 0;
		        VersionSerializer handleSerializer(0, {{0, "Handle"}, {1, "m_Handle"}});

		        uint64_t vhandle;
		        handleSerializer.Deserialize(nodeE, vhandle);
		        AssetHandle handle {vhandle};
        #else
		        AssetHandle handle = nodeE["Handle"].as<uint64_t>();
        #endif
                FileSystem::Path path(pathStr);
		        FileSystem::Path makredPath(makredPathStr);
		        AssetFindeInfo info(handle, path, makredPath);

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

	        template<typename Component>
            static void DeserializeComponent(YAML::Node& node, Component& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec);

	        template<typename ...Component>
            static void DeserializeAnyComponent(YAML::Node& parent, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            ([&]()
                {
                    const std::string_view viewComponentName = typeid(Component).name();
                    constexpr const char* ptr = "struct Rynex::";
                    constexpr size_t count = 6 + 1 + 5 + 2;
                    const std::string componentName(viewComponentName.begin() + count, viewComponentName.end());


                    if (YAML::Node node = parent[componentName])
                    {
                        if (!entity.HasComponent<Component>())
                            entity.AddComponent<Component>();

                        Component& component = entity.GetComponent<Component>();
                        DeserializeComponent<Component>(node, component, entity, loadingPromisVec);

                    }
                }(), ...);
	        }

	        template<typename... Component>
            static void DeserializeGroupComponent(ComponentGroup<Component ...>, YAML::Node& parent, const Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            DeserializeAnyComponent<Component...>(parent, entity, loadingPromisVec);
	        }

	        template<typename Component>
            void DeserializeComponent(YAML::Node& node, Component& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            static_assert(false, "No dafault implemtion allwoed!");
	        }

	        template<>
            void DeserializeComponent<TagComponent>(YAML::Node& node, TagComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Tag = node["Tag"].as<std::string>();
	        }

	        template<>
            void DeserializeComponent<TransformComponent>(YAML::Node& node, TransformComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Transform = node[ "Transaltion"].as<glm::vec3>();
	            component.m_Rotation = node[ "Rotation"].as<glm::vec3>();
	            component.m_Scale = node[ "Scale"].as<glm::vec3>();
	        }

	        template<>
            void DeserializeComponent<CameraComponent>(YAML::Node& node, CameraComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Camera = node["Camera"].as<SceneCamera>();
	            component.m_Primary = node["Primary"].as<bool>();
	            component.m_FixedAspectRotation = node["FixedAspectRotaion"].as<bool>();
	        }

	        template<>
            void DeserializeComponent<ScriptComponent>(YAML::Node& node, ScriptComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Name = node["ClassName"].as<std::string>();
	            component.m_SelectedScript = node["SelectedScript"].as<int>();
	        }

	        template<>
            void DeserializeComponent<SpriteRendererComponent>(YAML::Node& node, SpriteRendererComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Color = node["Color"].as<glm::vec4>();

	            YAML::Node textureNode = node["Texture"];
	            RefSceneLodePromisType<Texture> promis = Utils::Deserialize::DeserializeAssetFormate<SpriteRendererComponent, Texture>(textureNode, entity, AssetType::Texture2D);
	            if (nullptr != promis)
	                loadingPromisVec.emplace_back(promis);
	        }

	        template<>
            void DeserializeComponent<RelationshipUUIDComponent>(YAML::Node& node, RelationshipUUIDComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Parent = UUID(node["ParentID"].as<uint64_t>());
	            YAML::Node nodeChildrenIDVec = node["ChildrenIDs"];

	            component.m_Childrens.reserve(nodeChildrenIDVec.size());
	            for (YAML::Node nodeChildrenID : nodeChildrenIDVec)
	            {
	                const uint64_t number = nodeChildrenID.as<uint64_t>();
	                component.m_Childrens.emplace_back(UUID(number));
	            }
	        }

	        template<>
            void DeserializeComponent<ModelMatrixComponent>(YAML::Node& node, ModelMatrixComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Locale = node["Locale"].as<glm::mat4>();
	            component.m_Global = node["Global"].as<glm::mat4>();
	        }

	        template<>
            void DeserializeComponent<ViewMatrixComponent>(YAML::Node& node, ViewMatrixComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Locale = node["Locale"].as<glm::mat4>();
	            component.m_Global = node["Global"].as<glm::mat4>();
	        }

	        template<>
            void DeserializeComponent<ModelMangerComponent>(YAML::Node& node, ModelMangerComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            YAML::Node textureNode = node["StaticMesh"];
	            RefSceneLodePromisType<MeshStatic> promis = Utils::Deserialize::DeserializeAssetFormate<ModelMangerComponent, MeshStatic>(textureNode, entity, AssetType::MeshStatic);
	            if (nullptr != promis)
	                loadingPromisVec.emplace_back(promis);
	        }

	        template<>
            void DeserializeComponent<TextComponent>(YAML::Node& node, TextComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Color = node["Color"].as<glm::vec4>();
	            component.m_Kerning = node["Kerning"].as<float>();

	            component.m_LineSpacing = node["LineSpacing"].as<float>();
	            component.m_TextString = node["TextString"].as<std::string>();
	        }

	        template<>
            void DeserializeComponent<DirectionLightComponent>(YAML::Node& node, DirectionLightComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Color = node["Color"].as<glm::vec3>();
	            component.m_Intensity = node["Intensity"].as<float>();
	        }

	        template<>
            void DeserializeComponent<PointLightComponent>(YAML::Node& node, PointLightComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Color = node["Color"].as<glm::vec3>();
	            component.m_Intensity = node["Intensity"].as<float>();
	            component.m_Distance = node["Distance"].as<float>();
	            component.m_Constant = node["Constant"].as<float>();
	            component.m_Linear = node["Linear"].as<float>();
	            component.m_Quadratic = node["Quadratic"].as<float>();
	        }

	        template<>
            void DeserializeComponent<SpotLightComponent>(YAML::Node& node, SpotLightComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Color = node["Color"].as<glm::vec3>();
	            component.m_Intensity = node["Intensity"].as<float>();
	            component.m_Distance = node["Distance"].as<float>();
	            component.m_Inner = node["Inner"].as<float>();
	            component.m_Outer = node["Outer"].as<float>();
	        }

	        template<>
            void DeserializeComponent<FrameBufferComponent>(YAML::Node& node, FrameBufferComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_ClearColor = node["ClearColor"].as<glm::vec3>();
	            component.m_FrameBufferLayoutIndex = node["FrameBufferLayoutIndex"].as<uint32_t>();
	            component.m_FramebufferSize = node["FramebufferSize"].as<FrameBufferImageSize>();

	            if (const YAML::Node framebufferSpecificationNode = node["FramebufferSpecification"])
	            {
	                const FramebufferSpecification framebufferSpecification = framebufferSpecificationNode.as<FramebufferSpecification>();
	                component.m_FrameBuffer = Framebuffer::Create(framebufferSpecification);
	            }
	        }

	        template<>
            void DeserializeComponent<VisibleComponent>(YAML::Node& node, VisibleComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_Visible = node["Visible"].as<bool>();
	        }

	        template<>
            void DeserializeComponent<RenderTargetComponent>(YAML::Node& node, RenderTargetComponent& component, Entity entity, std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec)
	        {
	            component.m_RenderPassName = node["RenderPassName"].as<std::string>();

	            if (YAML::Node renderTargetNode = node["RenderTarget"])
	            {
	                component.m_Target = CreateRef<RenderTarget>();
	                const glm::vec4 renderViewSize = renderTargetNode["RenderViewSize"].as<glm::vec4>(glm::vec4{1,1,0,0});
	                if (const YAML::Node framebufferSpecificationNode = renderTargetNode["FramebufferSpecification"])
	                {
	                    const FramebufferSpecification framebufferSpecification = framebufferSpecificationNode.as<FramebufferSpecification>();
	                    const Ref<Framebuffer> framebuffer = Framebuffer::Create(framebufferSpecification);
	                    component.m_Target->SetFramebuffer(framebuffer);
	                }
	                component.m_Target->ResizeView(renderViewSize);
	            }

	        }

	}
}

	


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}


	


	void SceneSerializer::Serialize(const FileSystem::Path& path)
	{
		RY_CORE_WARN("Begin Serialize a Scene from '{}'", path);
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
#ifdef RY_INLINE_ENTITY_COMPENT_SERIALIZATION
        constexpr uint64_t version = 0;
#else
        constexpr uint64_t version = 1;
#endif

        out << YAML::Key << "Version" << YAML::Value << version;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityID)-> void
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				Utils::Serializer::SerializerEntity(out, entity, m_Scene);
			});
		out << YAML::EndMap;

		std::ofstream fout(path.GetPathString());
		RY_CORE_ASSERT(fout);
		fout << out.c_str();

		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Scene.get() };
			entity.UpdateMatrix();
		});
		RY_CORE_INFO("Ende Scene Serialization");
	}

	void SceneSerializer::SerializeRuntime(const FileSystem::Path& path)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime not Implemented!");
	}


	bool SceneSerializer::Deserialize(const FileSystem::Path& systemPath)
	{

		RY_CORE_WARN("Begin Serialize a Scene from '{}'", systemPath);
        Application::Get().SubmiteToMainThreedQueueWait([this]() -> void
            {
                m_Scene->ClearAll();
            }
        );

        std::filesystem::path path = systemPath.GetPath();
		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		Ref<EditorAssetManagerThread> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		RY_CORE_ASSERT("Deserialize Scene '{0}'", sceneName);
		if (YAML::Node versionNode = data["Version"])
		{
		    uint64_t version = versionNode.as<uint64_t>();
 		    RY_CORE_INFO("Scene Version: {}", version);
		}
		std::vector<Ref<LodePromis<Scene>>>& loadingPromisVec = m_Scene->m_LoadingPromisVec;
		loadingPromisVec.clear();



		if (YAML::Node entities = data["Entities"])
		{
			for (YAML::detail::iterator_value entityNode : entities)
			{
				UUID uuid { entityNode["Entity"].as<uint64_t>(AssetHandle::Zero().GetHash()) };
			    std::string name;
			    if (YAML::Node tagComponent = entityNode["TagComponent"])
			        name = tagComponent["Tag"].as<std::string>();
			    Entity entity = m_Scene->CreateEntityWitheUUID(uuid, name);
#ifdef RY_INLINE_ENTITY_COMPENT_DESERIALIZATION

				if (YAML::Node transformComponent = entityNode["TransformComponent"])
				{
					// Entities always have transforms
					TransformComponent& tc = entity.GetComponent<TransformComponent>();
					tc.m_Transform = transformComponent["Transaltion"].as<glm::vec3>();
					tc.m_Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.m_Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				if (YAML::Node realtionShipComponent = entityNode["RealtionShipComponent"])
				{
					RelationshipUUIDComponent& rSc = entity.GetComponent<RelationshipUUIDComponent>();
					if(realtionShipComponent["Parent"])
					{
						UUID parent = realtionShipComponent["Parent"].as<uint64_t>();
						Entity parentEntity = m_Scene->GetEntitiyByUUID(parent);
						if(parentEntity)
						{
							rSc.m_Parent = parent;
							RelationshipUUIDComponent& rSCparent = parentEntity.GetComponent<RelationshipUUIDComponent>();
							UUID id = entity.GetUUID();
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

				if (YAML::Node cameraComponent = entityNode["CameraComponent"])
				{
					CameraComponent& cc = entity.AddComponent<CameraComponent>();

					YAML::Node cameraProps = cameraComponent["Camera"];
					cc.m_Camera = cameraProps.as<SceneCamera>();
					cc.m_Primary = cameraComponent["Primary"].as<bool>();
					cc.m_FixedAspectRotation = cameraComponent["FixedAspectRotaion"].as<bool>();
				}

				if (YAML::Node scriptComponent = entityNode["ScriptComponent"])
				{
					ScriptComponent& tc = entity.AddComponent<ScriptComponent>();
					tc.m_Name = scriptComponent["ClassName"].as<std::string>();
				}

				if (YAML::Node spriteRendererComponent = entityNode["SpriteRendererComponent"])
				{
					SpriteRendererComponent& sc = entity.AddComponent<SpriteRendererComponent>();
					sc.m_Color = spriteRendererComponent["Color"].as<glm::vec4>();
					YAML::Node spriteRendererComponentTexture = spriteRendererComponent["Texture"];
					RefSceneLodePromisType<Texture> promis = Utils::Deserialize::DeserializeAssetFormate<SpriteRendererComponent, Texture>(spriteRendererComponentTexture, entity, AssetType::Texture2D);
					if (nullptr != promis)
						loadingPromisVec.emplace_back(promis);
				}
				


				if (YAML::Node modelMatrixComponentN = entityNode["ModelMatrixComponent"])
				{
					if(!entity.HasComponent<ModelMatrixComponent>())
						entity.AddComponent<ModelMatrixComponent>();
					ModelMatrixComponent& m4c = entity.GetComponent<ModelMatrixComponent>();
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

				if (YAML::Node viewMatrixComponentN = entityNode["ViewMatrixComponent"])
				{
					if (!entity.HasComponent<ViewMatrixComponent>())
						entity.AddComponent<ViewMatrixComponent>();
					ViewMatrixComponent& viewMatC = entity.GetComponent<ViewMatrixComponent>();
					viewMatC.m_Locale = viewMatrixComponentN["Locale"].as<glm::mat4>();
					viewMatC.m_Global = viewMatrixComponentN["Globle"].as<glm::mat4>();
				}

				if (YAML::Node staticMeshComponent = entityNode["StaticMeshComponent"])
				{
					ModelMangerComponent& smc = entity.AddComponent<ModelMangerComponent>();
					YAML::Node staticMeshComponentStaticMesh = staticMeshComponent["StaticMesh"];
					RefSceneLodePromisType<MeshStatic> promis = Utils::Deserialize::DeserializeAssetFormate<ModelMangerComponent, MeshStatic>(staticMeshComponentStaticMesh, entity, AssetType::MeshStatic);
					if(nullptr != promis)
						loadingPromisVec.emplace_back(promis);
				}


				if (YAML::Node textComponent = entityNode["TextComponent"])
				{
					entity.AddComponent<TextComponent>();
					TextComponent& textC = entity.GetComponent<TextComponent>();
					textC.m_FontAsset = Font::GetDefault();
					textC.m_TextString = textComponent["TextString"].as<std::string>();
					textC.m_Color = textComponent["Color"].as<glm::vec4>();
					
					textC.m_LineSpacing = textComponent["LineSpacing"].as<float>();
					textC.m_Kerning = textComponent["Kerning"].as<float>();

					RY_CORE_ASSERT(entity.HasComponent<TextComponent>())
				}

				if (YAML::Node drirektionleComponent = entityNode["DrirektionleLigthComponent"])
				{
					DirectionLightComponent& drirektionleC = entity.AddComponent<DirectionLightComponent>();
					drirektionleC.m_Color = drirektionleComponent["Color"].as<glm::vec3>();
					drirektionleC.m_Intensity = drirektionleComponent["Intensitie"].as<float>();
				}

				if (YAML::Node pointLigthComponent = entityNode["PointLigthComponent"])
				{
					PointLightComponent& pointLigthC = entity.AddComponent<PointLightComponent>();
					pointLigthC.m_Color = pointLigthComponent["Color"].as<glm::vec3>();
					pointLigthC.m_Distance = pointLigthComponent["Distence"].as<float>();

					if(YAML::Node constantCompN = pointLigthComponent["Constant"])
						pointLigthC.m_Constant = constantCompN.as<float>();

					if (YAML::Node intensitieCompN = pointLigthComponent["Intensitie"])
						pointLigthC.m_Intensity = intensitieCompN.as<float>();

					if (YAML::Node quadraticCompN = pointLigthComponent["Quadratic"])
						pointLigthC.m_Quadratic = quadraticCompN.as<float>();

				}

				if (YAML::Node spotLigthComponent = entityNode["SpotLigthComponent"])
				{
					SpotLightComponent& spotLigthC = entity.AddComponent<SpotLightComponent>();
					spotLigthC.m_Color = spotLigthComponent["Color"].as<glm::vec3>();
					spotLigthC.m_Intensity = spotLigthComponent["Intensitie"].as<float>();
					spotLigthC.m_Distance = spotLigthComponent["Distence"].as<float>();

					spotLigthC.m_Inner = spotLigthComponent["Inner"].as<float>();
					spotLigthC.m_Outer = spotLigthComponent["Outer"].as<float>();
					
				}

				if (YAML::Node frameBufferComponent = entityNode["FrameBufferComponent"])
				{
					FrameBufferComponent& frameC = entity.AddComponent<FrameBufferComponent>();

					if (YAML::Node framebufferSpecifcationNode = frameBufferComponent["FramebufferSpecifcation"])
					{
						FramebufferSpecification frame = framebufferSpecifcationNode.as<FramebufferSpecification>();
						frameC.m_FrameBuffer = Framebuffer::Create(frame);
					}
					frameC.m_ClearColor = frameBufferComponent["ClearColor"].as<glm::vec3>();
					frameC.m_FramebufferSize = (FrameBufferImageSize)frameBufferComponent["FramebufferSize"].as<int>();

				}
#else
			    Utils::Deserialize::DeserializeGroupComponent(SerializeComponents{}, entityNode , entity, loadingPromisVec);
#endif

		    }
		}
		RY_CORE_INFO("Ende Scene Deserialization");
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const FileSystem::Path& path)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime not Implemented!");
		return false;
	}

}




