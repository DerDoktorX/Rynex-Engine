#pragma once

#include <Rynex/Scene/ScenenCamera.h>
#include <Rynex/Core/UUID.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Framebuffer.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <Rynex/Renderer/Text/Font.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Memory/VectorData2D.h>
#include <Rynex/Renderer/RenderProxy/Proxy.h>

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
		UUID ID;

		IDComponent() {}
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID id)
			: ID(id) {}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string tag)
			: Tag(tag) {}
	};

#pragma endregion


	enum class RenderSginale {
		None = 0,
		NotInit,
		Darw,
		UpdateData
	};

	struct TransformComponent
	{
		glm::vec3 Transaltion{ 0.f, 0.f, 0.f };
		glm::vec3 Rotation{ 0.f, 0.f, 0.f };
		glm::vec3 Scale{ 1.f, 1.f, 1.f };
		bool Change = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 transaltion)
			: Transaltion(transaltion) {}

		glm::mat4 GetTransform() const
		{
			glm::quat oriaention = glm::quat(Rotation);
			glm::mat4 rotation = glm::toMat4(oriaention);
			
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), Transaltion);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);

			glm::mat4 matrix = translate * rotation * scale;

			return matrix;
		}

		void SetTransform(const glm::mat4& matrix)
		{
			Transaltion = TransformComponent::ExtraxtTransaltion(matrix);
			Scale = TransformComponent::ExtraxtScale(matrix);
			Rotation = TransformComponent::ExtraxtRotation(matrix, Scale);

			glm::mat4 matrixCheck = GetTransform();
			
			for(uint32_t x = 0; x < 4; x++)
			{
				for (uint32_t y = 0; y < 4; y++)
				{
					const float& matV = matrix[x][y];
					const float& matCheckV = matrixCheck[x][y];
					if(matV != matCheckV)
					{
						RY_CORE_WARN("Matrix[{}][{}] value befor: {} and after: {} are not equel", x, y, matV, matCheckV);
					}
				}
			}
			
		}
		static glm::vec3 ExtraxtTransaltion(const glm::mat4& matrix)
		{
			const glm::vec4& postionRow = matrix[3];
			glm::vec3 transaltion = glm::vec3(postionRow.x, postionRow.y, postionRow.z);
			transaltion *= postionRow.w;
			return transaltion;
		}

		static glm::vec3 ExtraxtRotation(const glm::mat4& matrix)
		{
			glm::mat3 rotationMatrix = glm::mat3(matrix);
			glm::vec3 scale = ExtraxtScale(matrix);
			rotationMatrix[0] /= scale[0];
			rotationMatrix[1] /= scale[1];
			rotationMatrix[2] /= scale[2];

			glm::quat oriention = glm::quat_cast(rotationMatrix);
			glm::vec3 eulerOriantion = glm::eulerAngles(oriention);
			return eulerOriantion;
		}

		static glm::vec3 ExtraxtRotation(const glm::mat4& matrix, const glm::vec3& scale)
		{
			glm::mat3 rotationMatrix = glm::mat3(matrix);
			rotationMatrix[0] /= scale[0];
			rotationMatrix[1] /= scale[1];
			rotationMatrix[2] /= scale[2];

			glm::quat oriention = glm::quat_cast(rotationMatrix);
			glm::vec3 eulerOriantion = glm::eulerAngles(oriention);
			return eulerOriantion;
		}

		static glm::vec3 ExtraxtScale(const glm::mat4& matrix)
		{
			glm::vec3 scaleRow[3] = { 
				glm::vec3(matrix[0])
				, glm::vec3(matrix[1])
				, glm::vec3(matrix[2])
			};

			float scaleLength[3] = {
				glm::length(scaleRow[0]),
				glm::length(scaleRow[1]),
				glm::length(scaleRow[2])
			};

			glm::vec3 scale = glm::vec3(scaleLength[0], scaleLength[1], scaleLength[2]);
			return scale;
		}

	};

	// TODO: Add TailingFactor for texture
	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 0.0f, 1.0f, 1.0f };
		Weak<Texture> Texture;
				

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: Color(color) {}

	};


	struct TextComponent
	{
		
		std::string TextString = "";
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;

	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRotaion = false;
		bool ViewFustrum = false;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

	};

	
	struct ScriptComponent
	{
		std::string Name = "None";
		int selectedScript = 0;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

	};

	class ScriptableEntity;

	struct NativeSripteComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeSripteComponent*);



		template<typename T>
		void Bind()
		{
			RY_CORE_MEMORY_ALICATION("InstantiateScript", "NativeSripteComponent::Bind", T);
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeSripteComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}


	};

	struct GeomtryComponent
	{
		Ref<VertexArray> Geometry = nullptr;
		Ref<VertexBuffer> Buffer = nullptr;

		GeomtryComponent() = default;
		GeomtryComponent(const GeomtryComponent&) = default;
	};

	struct MaterialComponent
	{
		Ref<Material> material = nullptr;
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

#pragma region Matrix_Compents
	struct ModelMatrixComponent
	{
		glm::mat4 Locale = glm::mat4(1.0);
		glm::mat4 Globle = glm::mat4(1.0);
		bool Change = false;

		ModelMatrixComponent() = default;
		ModelMatrixComponent(const ModelMatrixComponent&) = default;
	};
	
	// View Matrix: often used by Cameras and Matrix for rendering, but also often using Model Matrix,
	// this Matrix is the glm::inverse version from Model Matrix.
	struct ViewMatrixComponent
	{
		glm::mat4 Locale = glm::mat4(1.0);
		glm::mat4 Globle = glm::mat4(1.0);

		ViewMatrixComponent() = default;
		ViewMatrixComponent(const ViewMatrixComponent&) = default;

		inline glm::mat4 ModelViewProjection(const glm::mat4& model, const glm::mat4& projetion) const
		{
			return projetion * Globle * model;
		}

		inline glm::mat4 ViewProjection( const glm::mat4& projetion) const
		{
			return projetion * Globle;
		}

		// transforms from From Canonicel View Volume to Globelspace Usfuell like debuging your camera from a nother camera
		inline glm::mat4 GlobleCameraSpace(const glm::mat4& projetion) const
		{
			return glm::inverse(projetion * Globle);
		}

		void CalculteGlobelShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction)
		{
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			Globle = glm::lookAt(center + direction, center, up);
		}

		void CalculteLocaleShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction)
		{
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			Locale = glm::lookAt(center + direction, center, up);
		}
	};


	struct ProjtionViewMatrixComponent
	{
		glm::mat4 Locale = glm::mat4(1.0);
		glm::mat4 Globle = glm::mat4(1.0);

		ProjtionViewMatrixComponent() = default;
		ProjtionViewMatrixComponent(const ProjtionViewMatrixComponent&) = default;
	};

	struct InverseProjtionViewMatrixComponent
	{
		glm::mat4 Locale = glm::mat4(1.0);
		glm::mat4 Globle = glm::mat4(1.0);

		InverseProjtionViewMatrixComponent() = default;
		InverseProjtionViewMatrixComponent(const InverseProjtionViewMatrixComponent&) = default;
	};


	struct WorldViewFustrumComponent
	{
		std::array<glm::vec4, 8> ViewFustrum;

		WorldViewFustrumComponent() = default;
		WorldViewFustrumComponent(const WorldViewFustrumComponent&) = default;
	};

	struct AABBBoxComponent
	{
		BoundingVolume Local;
		BoundingVolume Globel;

		AABBBoxComponent() = default;
		AABBBoxComponent(const AABBBoxComponent&) = default;
	};

	struct PrimitvComponent
	{
		PrimitvComponent() = default;
		PrimitvComponent(const PrimitvComponent&) = default;

		std::vector<Ref<StaticMeshInstanceBatchRenderProxy>> proxyVec;

	};

	struct RenderTargetComponent
	{
		Ref<RenderTarget> Target;
		std::string RenderPassName = "None";
		bool RenderImage = true;
		bool ClearImage = true;
		bool ClearPiplines = false;
		bool UpdatePiplines = false;
		bool RenderImageOnce = true;

		bool FustremCulling = false;
		bool InsideMainFustrem = false;

		bool Render3DMeshes = true;
		bool RenderParicels = true;
		bool Render2DSprites = true;
		bool Render2DText = true;
		bool RenderIcons = true;
		uint32_t StroeIndex = MAXUINT32;
		
		RenderTargetComponent() = default;
		RenderTargetComponent(const RenderTargetComponent& renderTargetC)
			: Target(RenderTarget::Copy(renderTargetC.Target))
			, RenderPassName(renderTargetC.RenderPassName)
			, RenderImage(renderTargetC.RenderImage)
			, ClearImage(renderTargetC.ClearImage)
			, ClearPiplines(renderTargetC.ClearPiplines)
			, UpdatePiplines(renderTargetC.UpdatePiplines)
			, RenderImageOnce(renderTargetC.RenderImageOnce)
			, FustremCulling(renderTargetC.FustremCulling)
			, InsideMainFustrem(renderTargetC.InsideMainFustrem)
			, Render3DMeshes(renderTargetC.Render3DMeshes)
			, Render2DSprites(renderTargetC.Render2DSprites)
			, RenderIcons(renderTargetC.RenderIcons)
			, StroeIndex(MAXUINT32)
		{
		}
	};

	struct Matrix3x3Component
	{
		glm::mat3 Matrix;
	};


#pragma endregion

	struct FrameBufferComponent
	{
		Ref<Framebuffer> FrameBuffer = nullptr;
		
		uint32_t FrameBufferLayoutIndex = 0;
		glm::vec3 ClearColor;
		FrameBufferImageSize FramebufferSize;
		

		FrameBufferComponent() = default;
		FrameBufferComponent(const FrameBufferComponent&) = default;
		FrameBufferComponent(Ref<Framebuffer> frameBuffer)
			: FrameBuffer(frameBuffer)
			, ClearColor({ 0.0,0.0,0.0 })
			, FramebufferSize(FrameBufferImageSize::StaticSize)
		{ 
		}
	};

#pragma region Mesh
#define RY_DISABLE_FLAT_2D_VEC 0
	struct ModelMangerComponent
	{
		Ref<MeshStatic> meshStatic;
		std::vector<UUID> singleMeshes;
		std::vector<std::vector<uint32_t>> rendereStoreIndexVec2;

		Memory::VectorData2D<ObjectRendereIndex> objectRendereIndexPiplineVec2;


		ModelMangerComponent() = default;
		ModelMangerComponent(const ModelMangerComponent&) = default;
	};

	struct StaticMeshComponent
	{
		UUID entitySource;
		Ref<Material> material;
		Ref<MeshSingle> meshSingle;
		std::vector<uint32_t> rendereStoreIndexVec;
		std::vector<ObjectRendereIndex> objectRendereIndexPiplineVec;
		StaticMeshComponent()
			: entitySource(0ull)
			, material(nullptr)
			, meshSingle(nullptr)

		{
			RY_CORE_WARN("Entity Component StaticSingleComponetsMeshComponent, shoud be created with init args!");
		}

		StaticMeshComponent(const UUID& uuid, const Ref<MeshSingle>& meshSingel, const Ref<Material>& materiel)
			: entitySource(uuid)
			, material(materiel)
			, meshSingle(meshSingel)
			, rendereStoreIndexVec()
			, objectRendereIndexPiplineVec()
		{
			RY_CORE_WARN("Entity Component StaticSingleComponetsMeshComponent, shoud be created with init args!");
		}
		StaticMeshComponent(const StaticMeshComponent&) = default;

	};

	struct DynamicMeshComponent
	{
		Ref<MeshDynamic> meshR = nullptr;
		Ref<Material> material = nullptr;
		

		DynamicMeshComponent() = default;
		DynamicMeshComponent(const DynamicMeshComponent&) = default;		

	};

	struct VisibleComponent
	{
		bool isVisable = true;

		VisibleComponent() = default;
		VisibleComponent(const VisibleComponent&) = default;

	};

	struct InvirementMap 
	{
		Ref<TextureCubeMap> textureCubeMap;
		InvirementMap() = default;
		InvirementMap(const InvirementMap&) = default;

		std::vector<ObjectRendereIndex> indexPiplineVec;

	};

#pragma endregion



#pragma region LigthsComponts
	
	struct DrirectionleLigthComponent
	{
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensitie = 0.5f;

		uint32_t batchIndex = 0xFFFFFFFFu;

		DrirectionleLigthComponent() = default;
		DrirectionleLigthComponent(const DrirectionleLigthComponent&) = default;

	};

	struct PointLigthComponent
	{
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float distence = 10.0f;
		float intensitie = 0.5f;

		float constant = 1.0f;
		float linear =  0.022f;
		float quadratic = 0.0019f;

		uint32_t batchIndex = 0xFFFFFFFFu;

		PointLigthComponent() = default;
		PointLigthComponent(const PointLigthComponent&) = default;
	};

	struct SpotLigthComponent
	{
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float distence = 30.0f;
		float intensitie = 0.5;

		float inner = 0.95f;
		float outer = 0.9f;

		uint32_t batchIndex = 0xFFFFFFFFu;

		SpotLigthComponent() = default;
		SpotLigthComponent(const SpotLigthComponent&) = default;
	};

#pragma endregion


	// TOOD: make a Partikel Component System
	struct ParticelComponente
	{

		glm::vec3 spawnPositon;
		glm::vec3 spawnPositonRndOffset;
		float maxLifeTime;
		
		uint32_t count = 1;
		
	};

	struct PostProcessingComponent
	{
		Ref<Shader> shader = nullptr;
		glm::vec<3, uint16_t> dispatch = { 1, 1, 1 };
		// this is an Index how Defined the using order
		uint8_t order;

		// PostProcessingFlags Are Flags vor Memory Barrier In the GPU 
		// or Flags Like Screen Size Indipenden
		int postProcessingFlags = 0;

		PostProcessingComponent() = default;
		PostProcessingComponent(const PostProcessingComponent&) = default;

		
	};
	struct RealtionShipUUIDComponent
	{
		UUID parent = 0;
		std::vector<UUID> childrens;

		RealtionShipUUIDComponent() = default;
		RealtionShipUUIDComponent(const RealtionShipUUIDComponent&) = default;
	};


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		CameraComponent, ScriptComponent,
		MaterialComponent,
		GeomtryComponent,
		Matrix3x3Component,
		ModelMatrixComponent
		, ViewMatrixComponent,
		FrameBufferComponent,
		StaticMeshComponent,
		RealtionShipUUIDComponent,
		VisibleComponent,
		ModelMangerComponent, DynamicMeshComponent,
		NativeSripteComponent,
		DrirectionleLigthComponent, PointLigthComponent, SpotLigthComponent,
		ParticelComponente,
		TextComponent,
		RenderTargetComponent
		 >;
}