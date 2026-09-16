#pragma once

#include <Rynex/Scene/ScenenCamera.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/Text/Font.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Memory/VectorData2D.h>
#include <Rynex/Renderer/RenderProxy/Proxy.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>



#define RY_REALTION_SCHIP_ID_COMP 0
#define RY_REALTION_SCHIP_ARRAY_COMP 1


#ifndef RY_SCENE_HEADER
#define RY_SCENE_HEADER
#else
#error "Scene headers is alraedy!"
#endif

namespace Rynex {

#pragma region Identfing_Componts


	struct IDComponent
	{
		UUID m_ID;

		IDComponent() {}
		IDComponent(const IDComponent&) = default;

		explicit  IDComponent(const UUID id)
			: m_ID(id) {}
	};

	struct TagComponent
	{
		std::string m_Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;

        explicit TagComponent(const std::string& tag)
			: m_Tag(tag) {}
	};

#pragma endregion


	enum class RenderSignale {
		None = 0,
		NotInit,
		Draw,
		UpdateData
	};

	struct TransformComponent
	{
		glm::vec3 m_Transform{ 0.f, 0.f, 0.f };
		glm::vec3 m_Rotation{ 0.f, 0.f, 0.f };
		glm::vec3 m_Scale{ 1.f, 1.f, 1.f };
		bool m_Change = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

        explicit TransformComponent(const glm::vec3& translation)
			: m_Transform(translation)
            , m_Rotation(0.f, 0.f, 0.f)
	        , m_Scale(1.f, 1.f, 1.f )
            , m_Change(false)
        {}

		glm::mat4 GetTransform() const
		{
			const glm::quat orientation(m_Rotation);
			const glm::mat4 rotation = glm::toMat4(orientation);
			
			const glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_Transform);
			const glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

			const glm::mat4 matrix = translate * rotation * scale;

			return matrix;
		}

		void SetTransform(const glm::mat4& matrix)
		{
			m_Transform = TransformComponent::ExtraxtTranslation(matrix);
			m_Scale = TransformComponent::ExtraxtScale(matrix);
			m_Rotation = TransformComponent::ExtraxtRotation(matrix, m_Scale);

			glm::mat4 matrixCheck = GetTransform();
			
			for(uint32_t x = 0; x < 4; x++)
			{
				for (uint32_t y = 0; y < 4; y++)
				{
					const float& matV = matrix[x][y];
					const float& matCheckV = matrixCheck[x][y];
					if(matV != matCheckV)
					{
						RY_CORE_WARN("Matrix[{}][{}] value before: {} and after: {} are not equal", x, y, matV, matCheckV);
					}
				}
			}
			
		}
		static glm::vec3 ExtraxtTranslation(const glm::mat4& matrix)
		{
			const glm::vec4& positionRow = matrix[3];
			glm::vec3 translation(positionRow.x, positionRow.y, positionRow.z);
			translation *= positionRow.w;
			return translation;
		}

		static glm::vec3 ExtraxtRotation(const glm::mat4& matrix)
		{
			glm::mat3 rotationMatrix(matrix);
			glm::vec3 scale = ExtraxtScale(matrix);
			rotationMatrix[0] /= scale[0];
			rotationMatrix[1] /= scale[1];
			rotationMatrix[2] /= scale[2];

			const glm::quat orientation = glm::quat_cast(rotationMatrix);
			const glm::vec3 eulerOrientation = glm::eulerAngles(orientation);
			return eulerOrientation;
		}

		static glm::vec3 ExtraxtRotation(const glm::mat4& matrix, const glm::vec3& scale)
		{
			glm::mat3 rotationMatrix(matrix);
			rotationMatrix[0] /= scale[0];
			rotationMatrix[1] /= scale[1];
			rotationMatrix[2] /= scale[2];

			const glm::quat orientation = glm::quat_cast(rotationMatrix);
			const glm::vec3 eulerOrientation = glm::eulerAngles(orientation);
			return eulerOrientation;
		}

		static glm::vec3 ExtraxtScale(const glm::mat4& matrix)
		{
			const glm::vec3 scaleRow[3] = {
				    glm::vec3(matrix[0]),
			        glm::vec3(matrix[1]),
			        glm::vec3(matrix[2])
			};

			const float scaleLength[3] = {
				glm::length(scaleRow[0]),
				glm::length(scaleRow[1]),
				glm::length(scaleRow[2])
			};

			const glm::vec3 scale(scaleLength[0], scaleLength[1], scaleLength[2]);
			return scale;
		}

	};

	// TODO: Add TailingFactor for texture
	struct SpriteRendererComponent
	{
		glm::vec4 m_Color{ 1.0f, 0.0f, 1.0f, 1.0f };
		Weak<Texture> m_Texture;
				

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		explicit SpriteRendererComponent(const glm::vec4 color)
			: m_Color(color) {}

	};


	struct TextComponent
	{
		
		std::string m_TextString = "";
		Ref<Font> m_FontAsset = Font::GetDefault();
		glm::vec4 m_Color{ 1.0f };
		float m_Kerning = 0.0f;
		float m_LineSpacing = 0.0f;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;

	};

	struct CameraComponent
	{
		SceneCamera m_Camera;
		bool m_Primary = true;
		bool m_FixedAspectRotation = false;
		bool m_ViewFrustum = false;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

	};

	
	struct ScriptComponent
	{
		std::string m_Name = "None";
		int m_SelectedScript = 0;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

	};

	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* m_Instance = nullptr;

		ScriptableEntity* (*m_InstantiateScriptFunc)();
		void (*m_DestroyScriptFunc)(NativeScriptComponent*);



		template<typename T>
		void Bind()
		{
			RY_CORE_MEMORY_ALICATION("InstantiateScript", "NativeSripteComponent::Bind", T);
			m_InstantiateScriptFunc = []() -> ScriptableEntity* { return static_cast<ScriptableEntity*>(new T()); };
			m_DestroyScriptFunc = [](NativeScriptComponent* nsc)-> void { delete nsc->m_Instance; nsc->m_Instance = nullptr; };
		}


	};

	struct GeometryComponent
	{
		Ref<VertexArray> m_Geometry = nullptr;
		Ref<VertexBuffer> m_Buffer = nullptr;

		GeometryComponent() = default;
		GeometryComponent(const GeometryComponent&) = default;
	};

	struct MaterialComponent
	{
		Ref<Material> m_Material = nullptr;
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

#pragma region Matrix_Compents
	struct ModelMatrixComponent
	{
		glm::mat4 m_Locale = glm::mat4(1.0);
		glm::mat4 m_Global = glm::mat4(1.0);
		bool m_Change = false;

		ModelMatrixComponent() = default;
		ModelMatrixComponent(const ModelMatrixComponent&) = default;
	};
	
	// View Matrix: often used by Cameras and Matrix for rendering, but also often using Model Matrix,
	// this Matrix is the glm::inverse version from Model Matrix.
	struct ViewMatrixComponent
	{
		glm::mat4 m_Locale = glm::mat4(1.0);
		glm::mat4 m_Global = glm::mat4(1.0);

		ViewMatrixComponent() = default;
		ViewMatrixComponent(const ViewMatrixComponent&) = default;

		inline glm::mat4 ModelViewProjection(const glm::mat4& model, const glm::mat4& projection) const
		{
			return projection * m_Global * model;
		}

		inline glm::mat4 ViewProjection( const glm::mat4& projection) const
		{
			return projection * m_Global;
		}

		// transforms from From Canonical View Volume to Global space Usefully like debuting your camera from a other camera perspective
		inline glm::mat4 GlobalCameraSpace(const glm::mat4& projection) const
		{
			return glm::inverse(projection * m_Global);
		}

		void CalculateGlobalShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction)
		{
            constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
		    const glm::vec3 eye = center + direction;
			m_Global = glm::lookAt(eye, center, up);
		}

		void CalculateLocaleShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction)
		{
			constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
		    const glm::vec3 eye = center + direction;
			m_Locale = glm::lookAt(eye, center, up);
		}
	};


	struct ProjectionViewMatrixComponent
	{
		glm::mat4 m_Locale = glm::mat4(1.0);
		glm::mat4 m_Global = glm::mat4(1.0);

		ProjectionViewMatrixComponent() = default;
		ProjectionViewMatrixComponent(const ProjectionViewMatrixComponent&) = default;
	};

	struct InverseProjectionViewMatrixComponent
	{
		glm::mat4 m_Locale = glm::mat4(1.0);
		glm::mat4 m_Global = glm::mat4(1.0);

		InverseProjectionViewMatrixComponent() = default;
		InverseProjectionViewMatrixComponent(const InverseProjectionViewMatrixComponent&) = default;
	};


	struct WorldViewFrustumComponent
	{
		std::array<glm::vec4, 8> m_ViewFrustum;

		WorldViewFrustumComponent() = default;
		WorldViewFrustumComponent(const WorldViewFrustumComponent&) = default;
	};

	struct AABBBoxComponent
	{
		BoundingVolume m_Local;
		BoundingVolume m_Global;

		AABBBoxComponent() = default;
		AABBBoxComponent(const AABBBoxComponent&) = default;
	};

	struct PrimitiveComponent
	{
		PrimitiveComponent() = default;
		PrimitiveComponent(const PrimitiveComponent&) = default;

		std::vector<Ref<StaticMeshInstanceBatchRenderProxy>> m_ProxyVec;

	};

	struct RenderTargetComponent
	{
		Ref<RenderTarget> m_Target;
		std::string m_RenderPassName = "None";
		bool m_RenderImage = true;
		bool m_ClearImage = true;
		bool m_ClearPipline = false;
		bool m_UpdatePipline = false;
		bool RenderImageOnce = true;

		bool m_FrustumCulling = false;
		bool m_InsideMainFrustum = false;

		bool m_Render3DMeshes = true;
		bool m_RenderParticles = true;
		bool m_Render2DSprites = true;
		bool m_Render2DText = true;
		bool m_RenderIcons = true;
		uint32_t m_StoreIndex = std::numeric_limits<uint32_t>::max();
		
		RenderTargetComponent() = default;
		RenderTargetComponent(const RenderTargetComponent& renderTargetC)
			: m_Target(RenderTarget::Copy(renderTargetC.m_Target))
			, m_RenderPassName(renderTargetC.m_RenderPassName)
			, m_RenderImage(renderTargetC.m_RenderImage)
			, m_ClearImage(renderTargetC.m_ClearImage)
			, m_ClearPipline(renderTargetC.m_ClearPipline)
			, m_UpdatePipline(renderTargetC.m_UpdatePipline)
			, RenderImageOnce(renderTargetC.RenderImageOnce)
			, m_FrustumCulling(renderTargetC.m_FrustumCulling)
			, m_InsideMainFrustum(renderTargetC.m_InsideMainFrustum)
			, m_Render3DMeshes(renderTargetC.m_Render3DMeshes)
			, m_Render2DSprites(renderTargetC.m_Render2DSprites)
			, m_RenderIcons(renderTargetC.m_RenderIcons)
			, m_StoreIndex(std::numeric_limits<uint32_t>::max())
		{
		}
	};

	struct Matrix3x3Component
	{
		glm::mat3 m_Matrix;
	};


#pragma endregion

	struct FrameBufferComponent
	{
		Ref<Framebuffer> m_FrameBuffer = nullptr;
		
		uint32_t m_FrameBufferLayoutIndex = 0;
		glm::vec3 m_ClearColor{0.0f,0.0f,0.0f};
		FrameBufferImageSize m_FramebufferSize = FrameBufferImageSize::StaticSize;
		

		FrameBufferComponent() = default;
		FrameBufferComponent(const FrameBufferComponent&) = default;

        explicit FrameBufferComponent(const Ref<Framebuffer>& framebuffer)
			: m_FrameBuffer(framebuffer)
			, m_ClearColor(0.0f,0.0f,0.0f)
			, m_FramebufferSize(FrameBufferImageSize::StaticSize)
		{ 
		}
	};

#pragma region Mesh
#define RY_DISABLE_FLAT_2D_VEC 0
	struct ModelMangerComponent
	{
		Ref<MeshStatic> m_MeshStatic;
		std::vector<UUID> m_SingleMeshes;
		std::vector<std::vector<uint32_t>> m_RenderStoreIndexVec2;

		Memory::VectorData2D<ObjectRendereIndex> m_ObjectRenderIndexPiplineVec2;


		ModelMangerComponent() = default;
		ModelMangerComponent(const ModelMangerComponent&) = default;
	};

	struct StaticMeshComponent
	{
		UUID m_EntitySource;
		Ref<Material> m_Material;
		Ref<MeshSingle> m_MeshSingle;
		std::vector<uint32_t> m_RenderStoreIndexVec;
		std::vector<ObjectRendereIndex> m_ObjectRenderIndexPiplineVec;
		StaticMeshComponent()
			: m_EntitySource(0ull)
			, m_Material(nullptr)
			, m_MeshSingle(nullptr)

		{
			RY_CORE_WARN("Entity Component StaticMeshComponent, should be created with init args!");
		}

		StaticMeshComponent(const UUID& uuid, const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel)
			: m_EntitySource(uuid)
			, m_Material(materiel)
			, m_MeshSingle(meshSingle)
			, m_RenderStoreIndexVec()
			, m_ObjectRenderIndexPiplineVec()
		{
			RY_CORE_WARN_IF(0ull == uuid,"Entity Component StaticMeshComponent, should be created with init args!");
		}
		StaticMeshComponent(const StaticMeshComponent&) = default;

	};

	struct DynamicMeshComponent
	{
		Ref<MeshDynamic> m_MeshR = nullptr;
		Ref<Material> m_Material = nullptr;
		

		DynamicMeshComponent() = default;
		DynamicMeshComponent(const DynamicMeshComponent&) = default;		

	};

	struct VisibleComponent
	{
		bool m_Visible = true;

		VisibleComponent() = default;
		VisibleComponent(const VisibleComponent&) = default;

	};

	struct EnvironmentMap
	{
		Ref<TextureCubeMap> m_TextureCubeMap;
	    std::vector<ObjectRendereIndex> m_IndexPiplineVec;

		EnvironmentMap() = default;
		EnvironmentMap(const EnvironmentMap&) = default;



	};

#pragma endregion



#pragma region LigthsComponts
	
	struct DirectionLightComponent
	{
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
		float m_Intensity = 0.5f;

		uint32_t m_BatchIndex = 0xFFFFFFFFu;

		DirectionLightComponent() = default;
		DirectionLightComponent(const DirectionLightComponent&) = default;

	};

	struct PointLightComponent
	{
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
		float m_Distance = 10.0f;
		float m_Intensity = 0.5f;

		float m_Constant = 1.0f;
		float m_Linear =  0.022f;
		float m_Quadratic = 0.0019f;

		uint32_t m_BatchIndex = 0xFFFFFFFFu;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};

	struct SpotLightComponent
	{
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
		float m_Distance = 30.0f;
		float m_Intensity = 0.5;

		float m_Inner = 0.95f;
		float m_Outer = 0.9f;

		uint32_t m_BatchIndex = 0xFFFFFFFFu;

		SpotLightComponent() = default;
		SpotLightComponent(const SpotLightComponent&) = default;
	};

#pragma endregion


	// TODO: make a Particle Component System
	struct ParticleComponent
	{

		glm::vec3 m_SpawnPositon;
		glm::vec3 m_SpawnPositonRndOffset;
		float m_MaxLifeTime;
		
		uint32_t m_Count = 1;
		
	};

	struct PostProcessingComponent
	{
		Ref<Shader> m_Shader = nullptr;
		glm::vec<3, uint16_t> m_Dispatch = { 1, 1, 1 };
		// this is an Index how Defined the using order
		uint8_t m_Order;

		// PostProcessingFlags Are Flags vor Memory Barrier In the GPU 
		// or Flags Like Screen Size Independent
		int m_PostProcessingFlags = 0;

		PostProcessingComponent() = default;
		PostProcessingComponent(const PostProcessingComponent&) = default;

		
	};
	struct RelationshipUUIDComponent
	{
		UUID m_Parent = 0;
		std::vector<UUID> m_Childrens;

		RelationshipUUIDComponent() = default;
		RelationshipUUIDComponent(const RelationshipUUIDComponent&) = default;
	};


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<
		    TransformComponent,
            SpriteRendererComponent,
		    CameraComponent,
            ScriptComponent,
		    MaterialComponent,
		    GeometryComponent,
		    Matrix3x3Component,
		    ModelMatrixComponent,
            ViewMatrixComponent,
		    FrameBufferComponent,
		    StaticMeshComponent,
		    RelationshipUUIDComponent,
		    VisibleComponent,
		    ModelMangerComponent,
            DynamicMeshComponent,
		    NativeScriptComponent,
		    DirectionLightComponent,
            PointLightComponent,
            SpotLightComponent,
		    ParticleComponent,
		    TextComponent,
		    RenderTargetComponent
		 >;
}