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
#define RY_REALTION_SCHIP_ID_COMP 1
#define RY_REALTION_SCHIP_ARRAY_COMP 0
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
		bool ViewFustrum = true;
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

#if 1
	struct GeomtryComponent
	{
		Ref<VertexArray> Geometry = nullptr;
		Ref<VertexBuffer> Buffer = nullptr;

		GeomtryComponent() = default;
		GeomtryComponent(const GeomtryComponent&) = default;
	};

	struct MaterialComponent
	{
		Ref<Material> Materiel = nullptr;
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};
#endif

#pragma region Matrix_Compents

	// Model Matrix: used by nearly every 3d Object, Ligths and Cameras.
	struct ModelMatrixComponent
	{
		glm::mat4 Locale = glm::mat4(1.0);
		glm::mat4 Globle = glm::mat4(1.0);

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
		BoxAABB Local;
		BoxAABB Globel;

		AABBBoxComponent() = default;
		AABBBoxComponent(const AABBBoxComponent&) = default;
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

	struct MeshComponent
	{
		Ref<Model> ModelR = nullptr;
		MeshMode MeshModeE = MeshMode::None;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;

		MeshComponent(Ref<Model> modelR, MeshMode meshModeE)
			: ModelR(modelR), MeshModeE(meshModeE) {  }
	};

	struct MeshStatic
	{
		glm::mat4 Locale;
		glm::mat4 Globle;
		
		Ref<Mesh> MeshR;
		Ref<Material> Materiel;
	};

	struct StaticMeshComponent
	{
		Ref<Model> ModelR = nullptr;
		// TODO: Replace all 4 seprate vector, withe a MeshStatic vector!
#if RY_MODEL_NODE
		std::vector<MeshStatic> Meshes;
#else
		std::vector<glm::mat4> GlobleMeshMatrix;
		std::vector<glm::mat4> LocaleMeshMatrix;
		std::vector<Ref<Mesh>> Meshes;
		std::vector<Ref<Material>> Materiels;
		bool UseAlleMeshes = true;
#endif
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;

		StaticMeshComponent(Ref<Model> modelR)
		{
			ModelR = modelR;
		}

	};

	struct MeshDynamic
	{
		Ref<Mesh> MeshR;
		Ref<Material> Materiel;

		MeshDynamic(const Ref<Mesh>& mesh)
			: MeshR(mesh), Materiel(nullptr){ }
	};

	struct DynamicMeshComponent
	{
#if RY_MODEL_NODE
		std::vector<MeshDynamic> MeshD;
#else
		Ref<Mesh> MeshR = nullptr;
		Ref<Material> Materiel = nullptr;
#endif
		

		DynamicMeshComponent() = default;
#if RY_MODEL_NODE
		DynamicMeshComponent(const DynamicMeshComponent& dMC)
			: MeshD(dMC.MeshD) {}

		DynamicMeshComponent(Ref<Model> modelR, int meshIndex = 0)
		{
			auto& nodeRoots = modelR->GetNodes();
			auto& node = nodeRoots[meshIndex];
			MeshD.reserve(node.Meshes.size());
			for (auto& mesh : node.Meshes)
			{
				MeshD.emplace_back(mesh);
			}
		}
#else
		DynamicMeshComponent(const DynamicMeshComponent&) = default;
		DynamicMeshComponent(Ref<Model> modelR, int meshIndex = 0)
			: MeshR(modelR->GetMesh(meshIndex))
		{ }
#endif
		

	};

#pragma endregion



#pragma region LigthsComponts
	
	struct AmbientLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensitie = 0.05f;

		AmbientLigthComponent() = default;
		AmbientLigthComponent(const AmbientLigthComponent&) = default;
	};

	struct DrirektionleLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensitie = 0.5f;

		DrirektionleLigthComponent(const DrirektionleLigthComponent&) = default;

	};

	struct PointLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Distence = 0.5f;
		float Intensitie = 0.5f;


		PointLigthComponent() = default;
		PointLigthComponent(const PointLigthComponent&) = default;
	};

	struct SpotLigthComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Distence = 50.0f;
		float Intensitie = 0.5;

		float Inner = 0.95f;
		float Outer = 0.9f;

		SpotLigthComponent() = default;
		SpotLigthComponent(const SpotLigthComponent&) = default;
	};

#pragma endregion

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

#if RY_REALTION_SCHIP_ID_COMP
	struct RealtionShipComponent
	{
		UUID PreviusID = 0;
		UUID FirstID = 0;
		UUID NextID = 0;
		UUID ParentID = 0;
		RealtionShipComponent() = default;
		RealtionShipComponent(const RealtionShipComponent&) = default;
		RealtionShipComponent(UUID previusID, UUID parentID = 0, UUID nextID = 0, UUID firstID = 0)
			: PreviusID(previusID), ParentID(parentID), NextID(nextID), FirstID(firstID) {}
	};
#elif RY_REALTION_SCHIP_ARRAY_COMP
	struct RealtionShipsComponent
	{
		int Parent = -1;
		std::vector<int> Childrens;

		RealtionShipsComponent() = default;
		RealtionShipsComponent(const RealtionShipsComponent&) = default;
	};
#endif


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		/*CircleRendererComponent,*/ CameraComponent, ScriptComponent,
		MaterialComponent,
		GeomtryComponent,
		Matrix3x3Component, ModelMatrixComponent, ViewMatrixComponent,
		FrameBufferComponent,
#if RY_REALTION_SCHIP_ID_COMP
		RealtionShipComponent,
#elif RY_REALTION_SCHIP_ARRAY_COMP
		RealtionShipsComponent,
#endif
		MeshComponent, StaticMeshComponent, DynamicMeshComponent,
		NativeSripteComponent,
		AmbientLigthComponent, DrirektionleLigthComponent, PointLigthComponent, SpotLigthComponent,
		ParticelComponente
		/*, Rigidbody2DComponent,*/ /*BoxCollider2DComponent,*/
		/*CircleCollider2DComponent,*/ >;
}