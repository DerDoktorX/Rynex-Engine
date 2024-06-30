#pragma once

#include "ScenenCamera.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Renderer/Texture.h"
#include "Rynex/Renderer/Shader.h"
#include "Rynex/Renderer/VertexArray.h"
#include "Rynex/Renderer/Geomtrys.h"

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
		glm::vec3 Color{ 1.0f, 0.0f, 1.0f };
		Shader::Algorithm Flags;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
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
		NativeSripteComponent /*, Rigidbody2DComponent,*/ /*BoxCollider2DComponent,*/
		/*CircleCollider2DComponent,*/ /*TextComponent*/>;
}