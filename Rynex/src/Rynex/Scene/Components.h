#pragma once

#include "ScenenCamera.h"
#include "Scene.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/VertexArray.h"
#include "Rynex/Renderer/Objects/Geomtrys.h"
#include "Rynex/Renderer/API/Framebuffer.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>



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
		bool change = false;

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
	};

	// TODO: Add TailingFactor for texture
	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 0.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		
		bool RenderSingle = false;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: Color(color) {}

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
		std::string Name;
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
		Ref<VertexArray> Geometry;
		Ref<VertexBuffer> Buffer;
		// Make By User or Defind Shape like Cube, Plane, Sphere ...

		GeomtryComponent() = default;
		GeomtryComponent(const GeomtryComponent&) = default;
	};
#endif // TODO: Geomtry System
	

	struct MaterialComponent
	{
		Ref<Shader> Shader;
		Ref<Texture2D> Texture;

		std::vector<UniformElement> UniformLayoute;
		glm::vec3 Color{ 1.0f, 0.0f, 1.0f };
		int AlgorithmFlags; // Shader::Algorithm

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};


	struct Matrix4x4Component
	{
		glm::mat<4, 4, float> Matrix4x4;
	};

	struct Matrix3x3Component
	{
		glm::mat<3, 3, float> Matrix3x3;
	};

	struct FrameBufferComponent 
	{
		Ref<Framebuffer> FrameBuffer;
		FramebufferSpecification FramebufferSpecification;

		glm::vec<3, float> ClearColor;
		FrameBufferImageSize FramebufferSize;
		int EntiyOrigShader;

		FrameBufferComponent() = default;
		FrameBufferComponent(const FrameBufferComponent&) = default;
		FrameBufferComponent(Ref<Framebuffer> frameBuffer)
			: FrameBuffer(frameBuffer) {}
	};
#if 0
	using Framebuffers = std::vector<Ref<Framebuffer>>;
	struct FrameBufferComponents
	{
		Ref<Framebuffers> FrameBuffers;
	};
#endif // TODO: Add MultyFrambuffer Component System, sigle Entity!
	struct SceneComponent
	{
		Ref<Framebuffer> FrameBuffer;
		Ref<Scene> Scene;

		SceneComponent() = default;
		SceneComponent(const SceneComponent&) = default;
	};

	struct MainViewPortComponent
	{
		Ref<SceneCamera> Camera;
		Ref<EditorCamera> EditorCamera;
		Ref<Framebuffer> FrameBuffer;

		MainViewPortComponent() = default;
		MainViewPortComponent(const MainViewPortComponent&) = default;
	};

#if 0 
	struct MaterialTransperent : MaterialComponent
	{
		float Trasperent = 1.0f;

		MaterialTransperent() = default;
		MaterialTransperent(const MaterialTransperent&) = default;
	};
#endif
	
	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		/*CircleRendererComponent,*/ CameraComponent, ScriptComponent,
		MaterialComponent, GeomtryComponent,
		Matrix3x3Component, Matrix4x4Component,
		FrameBufferComponent, MainViewPortComponent, SceneComponent,
		NativeSripteComponent /*, Rigidbody2DComponent,*/ /*BoxCollider2DComponent,*/
		/*CircleCollider2DComponent,*/ /*TextComponent*/>;
}