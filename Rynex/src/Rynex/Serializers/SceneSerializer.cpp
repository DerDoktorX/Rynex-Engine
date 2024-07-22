#include "rypch.h"
#include "YAML.h"
#include "SceneSerializer.h"

#include "Rynex/Scene/Entity.h"
#include "Rynex/Scene/Components.h"
#include "Rynex/Scene/ScenenCamera.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/EditorAssetManager.h"
#include <glm/ext/matrix_clip_space.hpp>

namespace Rynex {

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}


	static void SerializerEntity(YAML::Emitter& out, Entity entity)
	{
		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
	
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
	
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Transaltion" << YAML::Value << tc.Transaltion;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = cc.Camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType"		<< YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectivVerticleFOV" << YAML::Value << camera.GetPerspectivVerticleFOV();
			out << YAML::Key << "PerspectivNearClipe"	<< YAML::Value << camera.GetPerspectivNearClipe();
			out << YAML::Key << "PerspectivFarClipe"	<< YAML::Value << camera.GetPerspectivFarClipe();
			out << YAML::Key << "OrthographicSize"		<< YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClipe" << YAML::Value << camera.GetOrthographicNearClipe();
			out << YAML::Key << "OrthographicFarClipe"	<< YAML::Value << camera.GetOrthographicFarClipe();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRotaion" << YAML::Value << cc.FixedAspectRotaion;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			auto& sc = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << sc.Name;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& sc = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << sc.Color;
			if (sc.Texture)
			{
				out << YAML::Key << "AssetHandle" << YAML::Value << sc.Texture->Handle;
				out << YAML::Key << "TexturePath" << YAML::Value << editorAssetManger->GetMetadata(sc.Texture->Handle).FilePath.string();
			}

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}


	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untiteld";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registery.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;
	
			SerializerEntity(out, entity);
		});
	

		out << YAML::EndMap;
	
		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime not Implementet!");
	}


	bool SceneSerializer::Deserialize(const std::string& filepath)
	{

		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		YAML::Node data = YAML::Load(strStream.str());
		if(!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		RY_CORE_ASSERT("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				RY_CORE_ASSERT("Deserializing entity withe ID = {0}, name = {1}", sceneName);

				Entity deserializedEntity = m_Scene->CreateEntityWitheUUID(uuid, name);


				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Transaltion = transformComponent["Transaltion"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectivVerticleFOV(cameraProps["PerspectivVerticleFOV"].as<float>());
					cc.Camera.SetPerspectivNearClipe(cameraProps["PerspectivNearClipe"].as<float>());
					cc.Camera.SetPerspectivFarClipe(cameraProps["PerspectivFarClipe"].as<float>());

					cc.Camera.SetOrthograficSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthograficNearClipe(cameraProps["OrthographicNearClipe"].as<float>());
					cc.Camera.SetOrthograficFarClipe(cameraProps["OrthographicFarClipe"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRotaion = cameraComponent["FixedAspectRotaion"].as<bool>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& tc = deserializedEntity.AddComponent<ScriptComponent>();
					tc.Name = scriptComponent["ClassName"].as<std::string>();

				}


				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& tc = deserializedEntity.AddComponent<SpriteRendererComponent>();
					tc.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["AssetHandle"]) // TODO: AssetHandle ADD for Deserialize!
					{
					}
					if (spriteRendererComponent["TexturePath"])
					{
						std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
						tc.Texture = AssetManager::GetAsset<Texture2D>(editorAssetManger->GetAssetHandle(texturePath));
					}
					
					
				}

			}
		}
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime not Implementet!");
		return false;
	}

}




