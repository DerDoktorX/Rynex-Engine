#pragma once

#include "ScenenCamera.h"
#include "Scene.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/VertexArray.h"
#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Renderer/Objects/Geomtrys.h"
#include "Rynex/Renderer/Objects/Mesh.h"
#include "Rynex/Renderer/Objects/Model.h"
#include "Rynex/Renderer/Rendering/Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <Rynex/Renderer/Text/Font.h>


namespace Rynex {

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
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			//glm::rotate(glm::mat4(1.0f), Rotation.x, { 1.0f, 0.0f ,0.0f })
			//* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0.0f, 1.0f ,0.0f })
			//* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0.0f, 0.0f ,1.0f });
			return glm::translate(glm::mat4(1.0f), Transaltion)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		void SetTransform(glm::mat4& matrix)
		{
			glm::vec3 skew = {};
			glm::vec4 perspectiv = {};
			glm::qua qaurt = glm::quat(Rotation);
			glm::decompose(matrix, Scale, qaurt, Transaltion, skew, perspectiv);
			Rotation = glm::eulerAngles(qaurt);

		}
	};

	// TODO: Add TailingFactor for texture
	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 0.0f, 1.0f, 1.0f };
#if RY_DISABLE_WEAK_PTR
		Ref<Texture> Texture;
#else
		Weak<Texture> Texture;
#endif
		bool RenderSingle = false;

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
#if 0
		Ref<Shader> Shader = nullptr;
#if 0
		Ref<Texture> Texture = nullptr;

		std::vector<UniformElement> UniformLayoute;
#else
		Ref<UniformBuffer> UniformBuffer;
#endif
		glm::vec3 Color{ 1.0f, 0.0f, 1.0f };
		int AlgorithmFlags = Renderer::CallFace_Back | Renderer::Death_Buffer | Renderer::A_Buffer | Renderer::Ligths; // 
#else
		Ref<Material> Materiel = nullptr;
#endif
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

	struct Matrix4x4Component
	{
		glm::mat4 Matrix4x4 = glm::mat4(1.0); // Locale Matrix
		glm::mat4 GlobleMatrix4x4 = glm::mat4(1.0);
	};

	struct Matrix3x3Component
	{
		glm::mat3 Matrix3x3;
	};

	struct FrameBufferComponent
	{
		Ref<Framebuffer> FrameBuffer = nullptr;
		

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

	struct MeshComponent
	{
		Ref<Model> ModelR = nullptr;
		MeshMode MeshModeE = MeshMode::None;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;

		MeshComponent(Ref<Model> modelR, MeshMode meshModeE)
			: ModelR(modelR), MeshModeE(meshModeE) {  }
	};

	struct StaticMeshComponent
	{
		Ref<Model> ModelR = nullptr;
		std::vector<glm::mat4> GlobleMeshMatrix;
		std::vector<glm::mat4> LocaleMeshMatrix;
		bool UseAlleMeshes = true;
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;

		StaticMeshComponent(Ref<Model> modelR)
		{
			ModelR = modelR;
		}

	};

	struct DynamicMeshComponent
	{
		Ref<Mesh> MeshR = nullptr;
		DynamicMeshComponent() = default;
		DynamicMeshComponent(const DynamicMeshComponent&) = default;

		DynamicMeshComponent(Ref<Model> modelR, int meshIndex = 0)
			: MeshR(modelR->GetMesh(meshIndex))
		{
		}

	};

	struct MainViewPortComponent
	{
		Ref<SceneCamera> Camera;
		Ref<EditorCamera> EditorCamera;
		Ref<Framebuffer> FrameBuffer;

		MainViewPortComponent() = default;
		MainViewPortComponent(const MainViewPortComponent&) = default;
	};


	
	struct AmbientLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensitie = 0.1f;

		AmbientLigthComponent() = default;
		AmbientLigthComponent(const AmbientLigthComponent&) = default;
	};

	struct DrirektionleLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensitie = 0.1f;
		SceneCamera CameraLigth;
		Ref<Framebuffer> ShadowFrameBuffer = nullptr;

		DrirektionleLigthComponent()
		{
			CameraLigth.SetViewPortSize(2048, 2048);
			CameraLigth.SetOrthoGrafic(15, 1, 15);
		};
		DrirektionleLigthComponent(const DrirektionleLigthComponent&) = default;

	};

	struct PointLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Distence = 0.0f;
		float Intensitie = 1.0f;
		glm::mat4 Projection;
		Ref<Framebuffer> ShadowFrameBuffer = nullptr;

		PointLigthComponent() = default;
		PointLigthComponent(const PointLigthComponent&) = default;
	};

	struct SpotLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Distence = 1.0f;
		float Intensitie = 1.0;

		float Inner = 0.9f;
		float Outer = 0.1f;

		glm::mat4 Projection;

		Ref<Framebuffer> ShadowFrameBuffer = nullptr;
		SpotLigthComponent() = default;
		SpotLigthComponent(const SpotLigthComponent&) = default;
	};

	// TOOD: make a Partikel Component System
	struct ParticelComponente
	{

		glm::vec3 SpawnPositon;
		glm::vec3 SpawnPositonRndOffset;
		float MaxLifeTime;
		
		uint32_t Count = 1;
		
	};

	struct PostProcessingComponent
	{
		Ref<Shader> Shader = nullptr;
		glm::vec<3, uint16_t> Dispatch = { 1, 1, 1 };
		// this is an Index how Defined the using order
		uint8_t Order;

		// PostProcessingFlags Are Flags vor Memory Barrier In the GPU 
		// or Flags Like Screen Size Indipenden
		int PostProcessingFlags = 0;

		PostProcessingComponent() = default;
		PostProcessingComponent(const PostProcessingComponent&) = default;

		void SetDispatch(glm::vec<3, uint16_t> dispatch)
		{

		}
	};

	struct RealtionShipComponent
	{
#if 0
		int Previus = -1;
		int First = -1;
		int Next = -1;
		int Parent = -1;
#else
		UUID PreviusID = 0;
		UUID FirstID = 0;
		UUID NextID = 0;
		UUID ParentID = 0;
#endif
		RealtionShipComponent() = default;
		RealtionShipComponent(const RealtionShipComponent&) = default;
#if 0
		RealtionShipComponent(int previus, int parent = -1, int next = -1, int first = -1)
			: Previus(previus), Parent(parent), Next(next), First(first) {}
#else
		RealtionShipComponent(UUID previusID, UUID parentID = 0, UUID nextID = 0, UUID firstID = 0)
			: PreviusID(previusID), ParentID(parentID), NextID(nextID), FirstID(firstID) {}
#endif
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		/*CircleRendererComponent,*/ CameraComponent, ScriptComponent,
		MaterialComponent,
		GeomtryComponent,
		Matrix3x3Component, Matrix4x4Component,
		FrameBufferComponent, MainViewPortComponent, RealtionShipComponent,
		MeshComponent, StaticMeshComponent, DynamicMeshComponent,
		NativeSripteComponent,
		AmbientLigthComponent, DrirektionleLigthComponent, PointLigthComponent, SpotLigthComponent,
		ParticelComponente
		/*, Rigidbody2DComponent,*/ /*BoxCollider2DComponent,*/
		/*CircleCollider2DComponent,*/ >;
}