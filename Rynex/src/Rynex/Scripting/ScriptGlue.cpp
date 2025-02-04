#include "rypch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include "Rynex/Core/UUID.h"
#include "Rynex/Core/KeyCodes.h"
#include "Rynex/Core/Input.h"
#include "Rynex/Asset/Base/AssetManager.h"

#include "Rynex/Scene/Scene.h"
#include "Rynex/Scene/Entity.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>


#define USE_HASCOMPONET_FUNC_CS 1

#define ENABLE_MATERIL_COMPONET 0 // TODO: ReWhrite Materil CS API

namespace Rynex {

	namespace Utils {

		std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			std::string str(cStr);
			mono_free(cStr);
			return str;
		}

		static Entity GetFromSceneEntity(UUID entityID)
		{
			Scene* scene = ScriptingEngine::GetSceneContext();
			return scene->GetEntitiyByUUID(entityID);
		}
	}

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityAddComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityRemoveComponentFuncs;


#define RY_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Rynex.InternalCalls::" #Name, Name)


	template<typename T>
	static T GetFromEntityComponent(Entity entity)
	{
		if (entity.HasComponent<T>())
			return entity.GetComponent<T>();
		else
			return nullptr;
	}

	// Exampels Defins
	static void NativLog(MonoString* text)
	{
		char* cStr = mono_string_to_utf8(text);
		printf("C++ says ->%s\n", cStr);
		mono_free(cStr);
	}

	static void NativLog_Vec3(glm::vec3* parmeter, glm::vec3* outresult)
	{
		RY_CORE_WARN("Value: {0}, {1}, {2}", parmeter->x, parmeter->y, parmeter->z);

		*outresult = glm::normalize(*parmeter);

	}

	static float NativLog_Vec3Dot(glm::vec3* parmeter)
	{
		RY_CORE_WARN("Value: {0}, {1}, {2}", parmeter->x, parmeter->y, parmeter->z);
		return glm::dot(*parmeter, *parmeter);

	}

#pragma region Entity

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();

		RY_CORE_ASSERT(scene, ("Scene Error"));
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Entity entity Error");

		MonoType* managedType = mono_reflection_type_get_type(componentType);

		RY_CORE_WARN("managedType is not nulptr {0}, nmae: {1}", "Core", (void*)managedType);
		RY_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), ("ERORO not finding in list!"));
		bool value = false;
		if (s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end())
		{
			value = s_EntityHasComponentFuncs.at(managedType)(entity);
		}
		return value;
	}

	static void Entity_AddComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();

		RY_CORE_ASSERT(scene, ("Scene Error"));
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Entity entity Error");

		MonoType* managedType = mono_reflection_type_get_type(componentType);

		RY_CORE_WARN("managedType is not nulptr {0}, nmae: {1}", "Core", (void*)managedType);
		RY_CORE_ASSERT(s_EntityAddComponentFuncs.find(managedType) != s_EntityAddComponentFuncs.end(), ("ERORO not finding in list!"));
		if (s_EntityAddComponentFuncs.find(managedType) != s_EntityAddComponentFuncs.end())
		{
			s_EntityAddComponentFuncs.at(managedType)(entity);
		}
		
	}

	static void Entity_RemoveComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();

		RY_CORE_ASSERT(scene, ("Scene Error"));
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Entity entity Error");

		MonoType* managedType = mono_reflection_type_get_type(componentType);

		RY_CORE_WARN("managedType is not nulptr {0}, nmae: {1}", "Core", (void*)managedType);
		RY_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), ("ERORO not finding in list!"));
		if (s_EntityAddComponentFuncs.find(managedType) != s_EntityAddComponentFuncs.end())
		{
			s_EntityAddComponentFuncs.at(managedType)(entity);
		}
	}

	static void Entity_CreateChilde(UUID parentID, MonoString* nameM, UUID* childrenID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();

		RY_CORE_ASSERT(scene, ("Scene Error"));
		Entity parent = scene->GetEntitiyByUUID(parentID);
		RY_CORE_ASSERT(parent, "Entity entity Error");
		std::string name = Utils::MonoStringToString(nameM);
		Entity childe = parent.AddChildrenEntity(name);
		*childrenID = childe.GetUUID();
	}

	static void Entity_UpdateMatrixe(UUID entityID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		RY_CORE_ASSERT(scene, ("Scene Error"));
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Entity entity Error");
		entity.UpdateMatrix();
	}

	static void Entity_Dealete(UUID entityID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		RY_CORE_ASSERT(scene, ("Scene Error"));
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Entity entity Error");
		entity.DestroyEntity();
		scene->DestroyEntity(entity);
		RY_CORE_WARN("Entity Destroy not finale Imp!");
	}

#pragma endregion

#pragma region ScriptComponentAPI

#pragma region TransformComponent

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TransformComponent>());
		TransformComponent& trasformC = entity.GetComponent<TransformComponent>();
		*outTranslation = trasformC.Transaltion;
		
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* inTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TransformComponent>());
		TransformComponent& trasformC = entity.GetComponent<TransformComponent>();
		trasformC.Transaltion = *inTranslation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TransformComponent>());
		TransformComponent& trasformC = entity.GetComponent<TransformComponent>();
		*outTranslation = trasformC.Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* inTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TransformComponent>());
		TransformComponent& trasformC = entity.GetComponent<TransformComponent>();
		trasformC.Rotation = *inTranslation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TransformComponent>());
		TransformComponent& trasformC = entity.GetComponent<TransformComponent>();
		*outTranslation = trasformC.Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* inTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TransformComponent>());
		TransformComponent& trasformC = entity.GetComponent<TransformComponent>();
		trasformC.Scale = *inTranslation;
	}


#pragma endregion

#pragma region GeometryComponent

#pragma region New

	static void GeometryComponent_GetGeometry(UUID entityID, AssetHandle* outTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		if (geomtryC.Geometry)
			*outTranslation = geomtryC.Geometry->Handle;
		else
			*outTranslation = AssetHandle(0);
	}

	static void GeometryComponent_SetGeometry(UUID entityID, AssetHandle* inTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		geomtryC.Geometry = AssetManager::GetAsset<VertexArray>(*inTranslation);

	}

	static void GeometryComponent_GetBuffer(UUID entityID, AssetHandle* outTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		if (geomtryC.Buffer)
			*outTranslation = geomtryC.Buffer->Handle;
		else
			*outTranslation = AssetHandle(0);
		
	}

	static void GeometryComponent_SetBuffer(UUID entityID, AssetHandle* inTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		geomtryC.Buffer = AssetManager::GetAsset<VertexBuffer>(*inTranslation);
	}

#pragma endregion

#pragma region Old

	struct GeomtryInput
	{
		glm::vec3 position; 
		glm::vec2 uv;
		glm::vec3 nomale;
	};

	static void GeometryComponent_SetVertex(UUID entityID, void* vertex, uint32_t byteSize)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());

		GeomtryInput* vertexInput = (GeomtryInput*)vertex;
		uint32_t count = byteSize / sizeof(GeomtryInput);
		
		GeomtryComponent& geomtry = entity.GetComponent<GeomtryComponent>();

		// Save Settings, Befor Destroying The Object.

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(byteSize);
		vertexBuffer->SetData(vertex, byteSize);
		vertexBuffer->SetLayout(geomtry.Buffer->GetLayout());
		Ref<VertexArray> vertexArray = VertexArray::Create();
		

		vertexArray->SetPrimitv(geomtry.Geometry->GetPrimitv());
		vertexArray->AddVertexBuffer(vertexBuffer);

		// Overide the old, Creat New Buffer.
		geomtry.Buffer = vertexBuffer;
		geomtry.Geometry = vertexArray;

		for (int i = 0; i < count; i++)
			RY_CORE_TRACE("postion[{0}] : x{1}, y{2}, z{3}", i, vertexInput[i].position.x, vertexInput[i].position.y, vertexInput[i].position.z);
		for (int i = 0; i < count; i++)
			RY_CORE_TRACE("normal[{0}] : x{1}, y{2}, z{3}", i, vertexInput[i].nomale.x, vertexInput[i].nomale.y, vertexInput[i].nomale.z);
		for (int i = 0; i < count; i++)
			RY_CORE_TRACE("uv[{0}] : x{1}, y{2}", i, vertexInput[i].uv.x, vertexInput[i].uv.y);

		vertexBuffer->GetBufferData();
	}

	static void GeomtryComponent_SetIndex(UUID entityID, uint32_t* index, uint32_t count)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());

		Ref<VertexArray>& vertex = entity.GetComponent<GeomtryComponent>().Geometry;
		Ref<IndexBuffer> inicies = IndexBuffer::Create(index, count);
		vertex->SetIndexBuffer(inicies);
	}

	static void GeomtryComponent_SetPrimitv(UUID entityID, int primitv)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity.HasComponent<GeomtryComponent>());

		auto& geometryC = entity.GetComponent<GeomtryComponent>();
		if(geometryC.Geometry)
			geometryC.Geometry->SetPrimitv((VertexArray::Primitv)primitv);
		else
		{
			geometryC.Geometry = VertexArray::Create();
			geometryC.Geometry->SetPrimitv((VertexArray::Primitv)primitv);
		}
	}

#pragma endregion

#if 0
	static void GeomtryComponent_SetLayout(UUID entityID, int conte, )
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;

		auto& geometryC = entity.GetComponent<GeomtryComponent>();
		geometryC.Buffer->SetLayout();

		_SetPrimitv

	}
#endif //TODO: Create SetPrimitv Funktion!

#pragma endregion

#pragma region TagComponent

	static void TagComponent_SetTag(UUID entityID, MonoString* tag)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TagComponent>());
		TagComponent& tagC = entity.GetComponent<TagComponent>();
		char* name = mono_string_to_utf8(tag);
		tagC.Tag = std::string(name);
		mono_free(name);
	}

	static void TagComponent_GetTag(UUID entityID, MonoString* outTranslation)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TagComponent>());
		TagComponent& tagC = entity.GetComponent<TagComponent>();
		outTranslation = mono_string_new_wrapper(tagC.Tag.c_str());
	}

#pragma endregion

#pragma region SpriteRendererComponent

	static void SpriteRendererComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpriteRendererComponent>());
		entity.GetComponent<SpriteRendererComponent>().Color = *color;
	}

	static void SpriteRendererComponent_GetColor(UUID entityID, glm::vec4* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpriteRendererComponent>());
		*color = entity.GetComponent<SpriteRendererComponent>().Color;
	}

	static void SpriteRendererComponent_SetTexture(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		if (!entity.HasComponent<SpriteRendererComponent>());
		entity.GetComponent<SpriteRendererComponent>().Texture = AssetManager::GetAsset<Texture>(*handle);
	}

	static void SpriteRendererComponent_GetTexture(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpriteRendererComponent>());
#if RY_DISABLE_WEAK_PTR
		if (Ref<Texture> tex = entity.GetComponent<SpriteRendererComponent>().Texture)
			*handle = tex->Handle;
#else
		if(Ref<Texture> tex = entity.GetComponent<SpriteRendererComponent>().Texture.lock())
			*handle = tex->Handle;
#endif
	}

#pragma endregion

#pragma region CameraComponent

	static void CameraComponent_SetPrimary(UUID entityID, bool* primary)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Primary = *primary;
	}

	static void CameraComponent_GetPrimary(UUID entityID, bool* primary)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		*primary = entity.GetComponent<CameraComponent>().Primary;
	}

	static void CameraComponent_SetFixedAspectRotaion(UUID entityID, bool* fixedAspectRotaion)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().FixedAspectRotaion = *fixedAspectRotaion;
	}

	static void CameraComponent_GetFixedAspectRotaion(UUID entityID, bool* fixedAspectRotaion)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		*fixedAspectRotaion = entity.GetComponent<CameraComponent>().FixedAspectRotaion;
	}

#pragma region SceneCamer

	static void CameraComponent_Camera_SetViewPortSize(UUID entityID, uint32_t withe, uint32_t heigth)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetViewPortSize(withe, heigth);
	}

	static void CameraComponent_Camera_SetOrthoGrafic(UUID entityID, float sizen, float nearClip, float farClip)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetOrthoGrafic(sizen, nearClip, farClip);
	}

	static void CameraComponent_Camera_SetPerspectiv(UUID entityID, float verticleFow, float nearClip, float farClip)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetOrthoGrafic(verticleFow, nearClip, farClip);
	}

	static void CameraComponent_Camera_SetOrthograficSize(UUID entityID, float size)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetOrthograficSize(size);
	}

	static void CameraComponent_Camera_SetOrthograficNearClipe(UUID entityID, float size)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetOrthograficNearClipe(size);
	}

	static void CameraComponent_Camera_SetOrthograficFarClipe(UUID entityID, float size)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetOrthograficFarClipe(size);
	}


	static float CameraComponent_Camera_GetOrthographicSize(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetOrthographicSize();
	}

	static float CameraComponent_Camera_GetOrthographicNearClipe(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetOrthographicNearClipe();
	}

	static float CameraComponent_Camera_GetOrthographicFarClipe(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetOrthographicFarClipe();
	}


	static void CameraComponent_Camera_SetPerspectivVerticleFOV(UUID entityID, float verticleVow)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetPerspectivVerticleFOV(verticleVow);
	}

	static void CameraComponent_Camera_SetPerspectivNearClipe(UUID entityID, float nearClip)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetPerspectivNearClipe(nearClip);
	}

	static void CameraComponent_SetPerspectivFarClipe(UUID entityID, float farClipe)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetPerspectivFarClipe(farClipe);
	}


	static float CameraComponent_Camera_GetPerspectivVerticleFOV(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetPerspectivVerticleFOV();
	}

	static float CameraComponent_Camera_GetPerspectivNearClipe(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetPerspectivNearClipe();
	}

	static float CameraComponent_Camera_GetPerspectivFarClipe(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetPerspectivFarClipe();
	}


	static SceneCamera::ProjectionType CameraComponent_Camera_GetProjectionType(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetProjectionType();
	}

	static void CameraComponent_Camera_SetProjectionType(UUID entityID, SceneCamera::ProjectionType type)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera.SetProjectionType(type);
	}

#pragma endregion

#pragma region Camera

	static glm::mat4 CameraComponent_Camera_GetProjektion(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		return entity.GetComponent<CameraComponent>().Camera.GetProjektion();
	}

#if 0
	static void CameraComponent_Camera_GetProjektion(UUID entityID, glm::mat4* matrix)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<CameraComponent>());
		entity.GetComponent<CameraComponent>().Camera = Camera(*matrix);
	}
#endif // TODO: Decide if we expose that

#pragma endregion

#pragma endregion

#pragma region ScriptComponent

	static void ScriptComponent_SetName(UUID entityID, MonoString* nameM)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<ScriptComponent>());
		ScriptComponent& scriptC = entity.GetComponent<ScriptComponent>();
		std::string name = Utils::MonoStringToString(nameM);
		RY_CORE_ASSERT(ScriptingEngine::ClassExists(name), "Not Existing Class");
		scriptC.Name = name;
		
	}

	static void ScriptComponent_GetName(UUID entityID, MonoString* nameM)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<ScriptComponent>());
		ScriptComponent& scriptC = entity.GetComponent<ScriptComponent>();
		nameM = mono_string_new_wrapper(scriptC.Name.c_str());
	}

#pragma endregion

#pragma region MaterialComponent
#if ENABLE_MATERIL_COMPONET
	static void MaterialComponent_SetShader(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MaterialComponent>());
		entity.GetComponent<MaterialComponent>().Materiel->SetShader(AssetManager::GetAsset<Shader>(*handle));
	}

	static void MaterialComponent_GetShader(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MaterialComponent>());
		*handle = entity.GetComponent<MaterialComponent>().Materiel->GetShader()->Handle;
	}
	
	static void MaterialComponent_SetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MaterialComponent>());
		entity.GetComponent<MaterialComponent>().Color = *color;
	}

	static void MaterialComponent_GetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MaterialComponent>());
		*color = entity.GetComponent<MaterialComponent>().Color;
	}
#endif
#pragma endregion

#pragma region 


	static void ModelMatrixComponent_SetMatrix4x4(UUID entityID, glm::mat4* matrix)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<ModelMatrixComponent>());
		entity.GetComponent<ModelMatrixComponent>().Locale = *matrix;
	}

	static glm::mat4 ModelMatrixComponent_GetMatrix4x4(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<ModelMatrixComponent>());
		return entity.GetComponent<ModelMatrixComponent>().Locale;
	}

	static void ModelMatrixComponent_SetGlobleMatrix4x4(UUID entityID, glm::mat4* matrix)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<ModelMatrixComponent>());
		entity.GetComponent<ModelMatrixComponent>().Globle = *matrix;
	}

	static glm::mat4 ModelMatrixComponent_GetGlobleMatrix4x4(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<ModelMatrixComponent>());
		return entity.GetComponent<ModelMatrixComponent>().Globle;
	}

#pragma endregion

#pragma region FrameBufferComponent

	static void FrameBufferComponent_SetFrameBuffer(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<FrameBufferComponent>());
		entity.GetComponent<FrameBufferComponent>().FrameBuffer = AssetManager::GetAsset<Framebuffer>(*handle);
	}

	static void FrameBufferComponent_GetFrameBuffer(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<FrameBufferComponent>());
		*handle = entity.GetComponent<FrameBufferComponent>().FrameBuffer->Handle;
	}

#pragma endregion

#pragma region MeshComponent

	static void MeshComponent_SetModelR(UUID entityID, AssetHandle handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MeshComponent>());
		entity.GetComponent<MeshComponent>().ModelR = AssetManager::GetAsset<Model>(handle);
	}

	static void MeshComponent_GetModelR(UUID entityID, AssetHandle* handle)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MeshComponent>());
		*handle = entity.GetComponent<MeshComponent>().ModelR->Handle;
	}

	static void MeshComponent_SetMeshMode(UUID entityID, MeshMode meshMode)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MeshComponent>());
		MeshComponent& mesh = entity.GetComponent<MeshComponent>();
		if (mesh.MeshModeE != meshMode && mesh.ModelR)
		{
			if (mesh.MeshModeE == MeshMode::Dynamic)
			{
				entity.DestroyEntityChildrens();

			}

			switch (meshMode)
			{
				case MeshMode::None:
			{
				if (entity.HasComponent<StaticMeshComponent>())
					entity.RemoveComponent<StaticMeshComponent>();
				break;
			}
				case MeshMode::Statitic:
			{
				if (!entity.HasComponent<StaticMeshComponent>())
					entity.AddComponent<StaticMeshComponent>();
				StaticMeshComponent& staticMesh = entity.GetComponent<StaticMeshComponent>();
				staticMesh.ModelR = mesh.ModelR;
				ModelMatrixComponent& parentMat4C = entity.GetComponent<ModelMatrixComponent>();
#if RY_MODEL_NODE
				auto& modelData = mesh.ModelR->GetNodes();
				staticMesh.Meshes.reserve(modelData.size());
				for (auto& node : modelData)
				{
					for (auto& meshNode : node.Meshes)
					{
						staticMesh.Meshes.emplace_back(MeshStatic{
							parentMat4C.Globle * node.Matrix,
							node.Matrix,
							meshNode,
							nullptr});
					}
				}

#else
				auto& modelData = mesh.ModelR->GetRootDatas();
				staticMesh.LocaleMeshMatrix.reserve(modelData.size());

				for (auto& meshData : modelData)
				{
					staticMesh.LocaleMeshMatrix.emplace_back(meshData.NodeMatrix);
					staticMesh.GlobleMeshMatrix.emplace_back(parentMat4C.Globle * meshData.NodeMatrix);
				}

#endif
				break;
			}
				case MeshMode::Dynamic:
				{
					if (!entity.HasComponent<ModelMatrixComponent>())
						entity.AddComponent<ModelMatrixComponent>();
					ModelMatrixComponent& parentMat4C = entity.GetComponent<ModelMatrixComponent>();

					if (!entity.HasComponent<RealtionShipComponent>())
						entity.AddComponent<RealtionShipComponent>();
					if (!entity.HasComponent<MaterialComponent>())
						entity.AddComponent<MaterialComponent>();
					MaterialComponent& materialCParent = entity.GetComponent<MaterialComponent>();

#if RY_MODEL_NODE
					auto& nodeRoot = mesh.ModelR->GetNodes();


					for (const auto& node : nodeRoot)
					{
						const std::vector<Ref<Mesh>>& mesh = node.Meshes;
						Entity childeEntity = entity.AddChildrenEntity(node.Name);

						childeEntity.AddComponent<DynamicMeshComponent>();
						childeEntity.AddComponent<MaterialComponent>();

						if (!childeEntity.HasComponent<ModelMatrixComponent>())
							childeEntity.AddComponent<ModelMatrixComponent>();
						ModelMatrixComponent& childMat4C = childeEntity.GetComponent<ModelMatrixComponent>();
						childMat4C.Locale = node.Matrix;
						childMat4C.Globle = parentMat4C.Locale * node.Matrix;

						if (!childeEntity.HasComponent<TransformComponent>())
							childeEntity.AddComponent<TransformComponent>();
						TransformComponent& transformC = childeEntity.GetComponent<TransformComponent>();
						transformC.SetTransform(childMat4C.Locale);

						if (!childeEntity.HasComponent<DynamicMeshComponent>())
							childeEntity.AddComponent<DynamicMeshComponent>();
						DynamicMeshComponent& dynamicMeshC = childeEntity.GetComponent<DynamicMeshComponent>();

						dynamicMeshC.MeshD.clear();
						dynamicMeshC.MeshD.reserve(mesh.size());
						for (const auto& meshDC : mesh)
						{
							dynamicMeshC.MeshD.emplace_back(meshDC);
						}
						

						if (!childeEntity.HasComponent<MaterialComponent>())
							childeEntity.AddComponent<MaterialComponent>();
					}

#else
					std::vector<MeshRootData> rootDatas = mesh.ModelR->GetRootDatas();
					std::vector<Ref<Mesh>> meshes = mesh.ModelR->GetMeshes();
					uint32_t size = rootDatas.size() == meshes.size() ? meshes.size() : 0;

					for (uint32_t i = 0; i < size; i++)
					{
						MeshRootData& rootData = rootDatas[i];
						Ref<Mesh>& mesh = meshes[i];
						Entity childeEntity = entity.AddChildrenEntity(rootData.NodeName);

						childeEntity.AddComponent<DynamicMeshComponent>();
						childeEntity.AddComponent<MaterialComponent>();

						if (!childeEntity.HasComponent<ModelMatrixComponent>())
							childeEntity.AddComponent<ModelMatrixComponent>();
						ModelMatrixComponent& childMat4C = childeEntity.GetComponent<ModelMatrixComponent>();
						childMat4C.Locale = rootData.NodeMatrix;
						childMat4C.Globle = parentMat4C.Locale * rootData.NodeMatrix;

						if (!childeEntity.HasComponent<TransformComponent>())
							childeEntity.AddComponent<TransformComponent>();
						TransformComponent& transformC = childeEntity.GetComponent<TransformComponent>();
						transformC.SetTransform(childMat4C.Locale);

						if (!childeEntity.HasComponent<DynamicMeshComponent>())
							childeEntity.AddComponent<DynamicMeshComponent>();
						DynamicMeshComponent& dynamicMeshC = childeEntity.GetComponent<DynamicMeshComponent>();
						dynamicMeshC.MeshR = mesh;

						if (!childeEntity.HasComponent<MaterialComponent>())
							childeEntity.AddComponent<MaterialComponent>();


					}
#endif
					break;
				}
				default:
				{
					RY_CORE_ASSERT(false);
					break;
				}
			}
		}
		mesh.MeshModeE = meshMode;
	}

	static MeshMode MeshComponent_GetMeshMode(UUID entityID)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<MeshComponent>());
		return entity.GetComponent<MeshComponent>().MeshModeE;
	}

#pragma endregion


#pragma region AmbientLigthComponent

	static void AmbientLigthComponent_SetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<AmbientLigthComponent>());
		entity.GetComponent<AmbientLigthComponent>().Color = *color;
	}

	static void AmbientLigthComponent_GetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<AmbientLigthComponent>());
		*color = entity.GetComponent<AmbientLigthComponent>().Color;
	}

	static void AmbientLigthComponent_SetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<AmbientLigthComponent>());
		entity.GetComponent<AmbientLigthComponent>().Intensitie = *intensitie;
	}

	static void AmbientLigthComponent_GeIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		if (!entity.HasComponent<AmbientLigthComponent>()) return;
		*intensitie = entity.GetComponent<AmbientLigthComponent>().Intensitie;
	}

#pragma endregion

#pragma region DrirektionleLigthComponent


	static void DrirektionleLigthComponent_SetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<DrirektionleLigthComponent>());
		entity.GetComponent<DrirektionleLigthComponent>().Color = *color;
	}

	static void DrirektionleLigthComponent_GetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<DrirektionleLigthComponent>());
		*color = entity.GetComponent<DrirektionleLigthComponent>().Color;
	}

	static void DrirektionleLigthComponent_SetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<DrirektionleLigthComponent>());
		entity.GetComponent<DrirektionleLigthComponent>().Intensitie = *intensitie;
	}

	static void DrirektionleLigthComponent_GetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<DrirektionleLigthComponent>());
		*intensitie = entity.GetComponent<DrirektionleLigthComponent>().Intensitie;
	}

#pragma endregion

#pragma region PointLigthComponent

	static void PointLigthComponent_SetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<PointLigthComponent>());
		entity.GetComponent<PointLigthComponent>().Color = *color;
	}

	static void PointLigthComponent_GetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<PointLigthComponent>());
		*color = entity.GetComponent<PointLigthComponent>().Color;
	}

	static void PointLigthComponent_SetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<PointLigthComponent>());
		entity.GetComponent<PointLigthComponent>().Intensitie = *intensitie;
	}

	static void PointLigthComponent_GetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<PointLigthComponent>());
		*intensitie = entity.GetComponent<PointLigthComponent>().Intensitie;
	}

	static void PointLigthComponent_SetDistence(UUID entityID, float* distence)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<PointLigthComponent>());
		entity.GetComponent<PointLigthComponent>().Distence = *distence;
	}

	static void PointLigthComponent_GetDistence(UUID entityID, float* distence)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<PointLigthComponent>());
		*distence = entity.GetComponent<PointLigthComponent>().Distence;
	}

#pragma endregion

#pragma region SpotLigthComponent

	static void SpotLigthComponent_SetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		entity.GetComponent<SpotLigthComponent>().Color = *color;
	}

	static void SpotLigthComponent_GetColor(UUID entityID, glm::vec3* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		*color = entity.GetComponent<SpotLigthComponent>().Color;
	}

	static void SpotLigthComponent_SetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		entity.GetComponent<SpotLigthComponent>().Intensitie = *intensitie;
	}

	static void SpotLigthComponent_GetIntensitie(UUID entityID, float* intensitie)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		*intensitie = entity.GetComponent<SpotLigthComponent>().Intensitie;
	}

	static void SpotLigthComponent_SetDistence(UUID entityID, float* distence)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		entity.GetComponent<SpotLigthComponent>().Distence = *distence;
	}

	static void SpotLigthComponent_GetDistence(UUID entityID, float* distence)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		*distence = entity.GetComponent<SpotLigthComponent>().Distence;
	}

	static void SpotLigthComponent_SetInner(UUID entityID, float* inner)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		entity.GetComponent<SpotLigthComponent>().Inner = *inner;
	}

	static void SpotLigthComponent_GetInner(UUID entityID, float* inner)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<SpotLigthComponent>());
		*inner = entity.GetComponent<SpotLigthComponent>().Inner;
	}

#pragma endregion

#pragma region TextComponent

	static void TextComponent_SetString(UUID entityID, MonoString* textM)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		std::string name = Utils::MonoStringToString(textM);
		tecxtC.TextString = name;

	}

	static void TextComponent_GetString(UUID entityID, MonoString* textM)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		textM = mono_string_new_wrapper(tecxtC.TextString.c_str());
	}

	static void TextComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		tecxtC.Color = *color;
	}

	static void TextComponent_GetColor(UUID entityID, glm::vec4* color)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		*color = tecxtC.Color;
	}

	static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		tecxtC.LineSpacing = lineSpacing;
	}

	static void TextComponent_GetLineSpacing(UUID entityID, float* lineSpacing)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		*lineSpacing = tecxtC.LineSpacing;
	}
		
	static void TextComponent_SetKerning(UUID entityID, float kerning)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		tecxtC.Kerning = kerning;
	}

	static void TextComponent_GetKerning(UUID entityID, float* kerning)
	{
		Entity entity = Utils::GetFromSceneEntity(entityID);
		RY_CORE_ASSERT(entity.HasComponent<TextComponent>());
		TextComponent& tecxtC = entity.GetComponent<TextComponent>();
		*kerning = tecxtC.Kerning;
	}

#pragma endregion

#pragma endregion


#pragma region ScriptClassAPI

#pragma region TextureClass
#if 0
	static void Texture_Create_Withe_Heigth(AssetHandle* handle, uint32_t withe, uint32_t height)
	{
		Ref<Texture> texture = Texture::Create(withe, height);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}
#endif

	static void Texture_Create_Spec_Data_ByteSize(AssetHandle* handle, TextureSpecification* spec, void* data, uint32_t size)
	{
		Ref<Texture> texture = Texture::Create(*spec, data, size);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}

	static void Texture_Create_Spec(AssetHandle* handle, TextureSpecification* spec)
	{

		Ref<Texture> texture = Texture::Create(*spec);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}

	static void Texture_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<Texture>(handle), "Texture_Destroy Texture is there, it shoud by Destroyt!");
	}

	static void Texture_GetSpecification(AssetHandle handle, TextureSpecification* spec)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetWidth Texture is nullptr!");
		*spec = texture->GetSpecification();
	}

	static void Texture_GetWidth(AssetHandle handle, uint32_t* width)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetWidth Texture is nullptr!");
		*width = texture->GetWidth();
	}

	static void Texture_GetHeight(AssetHandle handle, uint32_t* height)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetHeight Texture is nullptr!");
		*height = texture->GetHeight();
	}

	static void Texture_GetRenderID(AssetHandle handle, uint32_t* renderID)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetRenderID Texture is nullptr!");
		*renderID = texture->GetRenderID();
	}

	static void Texture_SetData(AssetHandle handle, void* data, uint32_t size)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_SetData Texture is nullptr!");
		texture->SetData(data, size);
	}

	static void Texture_Resize(AssetHandle handle, uint32_t height, uint32_t width)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_Resize Texture is nullptr!");
		texture->Resize(width, height);
	}

	static void Texture_Bind(AssetHandle handle, uint32_t slot)
	{
		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_Bind Texture is nullptr!");
		texture->Bind(slot);
	}

#pragma endregion

#pragma region ShaderClass

	static void Shader_Create_Source_Name(AssetHandle* handle, MonoString* source, MonoString* name)
	{
		char* c_strSource = mono_string_to_utf8(source);
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = Shader::Create(c_strSource, c_strName);
		shader->Handle = AssetHandle();
		shader->Handle = *handle = AssetManager::CreatLocaleAsset<Shader>(shader);
		RY_CORE_ASSERT(AssetManager::GetAsset<Shader>(shader->Handle) == shader, "Shader_Create_Source_Name Created Shader is not the same like in the Regestry!");
		RY_CORE_INFO("Shader Create form C# id = {0}", (uint32_t)*handle);
		mono_free(c_strSource);
		mono_free(c_strName);
	}

	static void Shader_Create_Source_Name_VertexSrc_FragmentSrc(AssetHandle* handle, MonoString* name, MonoString* vertexSrc, MonoString* fragmentSrc)
	{
		char* c_strVertexSrc = mono_string_to_utf8(vertexSrc);
		char* c_strFragmentSrc = mono_string_to_utf8(fragmentSrc);
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = Shader::Create(c_strName, c_strVertexSrc, c_strFragmentSrc);
		shader->Handle = AssetHandle();
		shader->Handle = *handle = AssetManager::CreatLocaleAsset<Shader>(shader);
		RY_CORE_ASSERT(AssetManager::GetAsset<Shader>(shader->Handle) == shader, "Shader_Create_Source_Name Shader Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Shader Create form C# id = {0}", (uint32_t)*handle);
		mono_free(c_strName);
		mono_free(c_strVertexSrc);
		mono_free(c_strFragmentSrc);
	}
	
	static void Shader_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<Shader>(handle), "Shader_Destroy Shader is there, it shoud by Destroyt!");
	}

	static void Shader_ReganrateShader(AssetHandle handle, MonoString* source)
	{
		char* c_strSource = mono_string_to_utf8(source);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_ReganrateShader Shader is nullptr!");
		shader->ReganrateShader(c_strSource);
		mono_free(c_strSource);
	}

	static void Shader_Bind(AssetHandle handle)
	{
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_Bind Shader is nullptr!");
		shader->Bind();
	}

	static void Shader_UnBind(AssetHandle handle)
	{
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_UnBind Shader is nullptr!");
		shader->UnBind();
	}

	static void Shader_SetUniformValue(AssetHandle handle, MonoString* name, void* value, ShaderDataType type)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetUniformValue Shader is nullptr!");
		shader->SetUniformValue(c_strName, value, type);
		mono_free(c_strName);

	}

	static void Shader_SetInt(AssetHandle handle, MonoString* name, int value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetInt Shader is nullptr!");
		shader->SetInt(c_strName, value);
		mono_free(c_strName);
	}

	static void Shader_SetIntArray(AssetHandle handle, MonoString* name, int* value, uint32_t count)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetIntArray Shader is nullptr!");
		shader->SetIntArray(c_strName, value, count);
		mono_free(c_strName);
	}

	static void Shader_SetFloat3(AssetHandle handle, MonoString* name, glm::vec3* value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetFloat3 Shader is nullptr!");
		shader->SetFloat3(c_strName, *value);
		mono_free(c_strName);
	}

	static void Shader_SetFloat4(AssetHandle handle, MonoString* name, glm::vec4* value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetFloat4 Shader is nullptr!");
		shader->SetFloat4(c_strName, *value);
		mono_free(c_strName);
	}

	static void Shader_SetMat4(AssetHandle handle, MonoString* name, glm::mat4* value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetMat4 Shader is nullptr!");
		shader->SetMat4(c_strName, *value);
		mono_free(c_strName);
	}

	static void Shader_GetName(AssetHandle handle, MonoString* name)
	{
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_GetName Shader is nullptr!");
		name = mono_string_new_wrapper(shader->GetName().c_str());
	}

#pragma endregion

#pragma region VertexBufferClass

	static void VertexBuffer_Create_Vertices_Size(AssetHandle* handle, void* vertices, uint32_t size)
	{
		
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, size);
		vertexBuffer->Handle = AssetHandle();
		vertexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<VertexBuffer>(vertexBuffer);
		RY_CORE_ASSERT(AssetManager::GetAsset<VertexBuffer>(vertexBuffer->Handle) == vertexBuffer, "VertexBuffer_Create_Vertices_Size Created VertexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("VertexBuffer Create form C# id = {0}", (uint32_t)*handle);
	}

	static void VertexBuffer_Create_Vertices_Size_Usage(AssetHandle* handle, void* vertices, uint32_t size, BufferDataUsage usage)
	{
		

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, size, usage);
		vertexBuffer->Handle = AssetHandle();
		vertexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<VertexBuffer>(vertexBuffer);
		RY_CORE_ASSERT(AssetManager::GetAsset<VertexBuffer>(vertexBuffer->Handle) == vertexBuffer, "VertexBuffer_Create_Vertices_Size Created VertexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("VertexBuffer Create form C# id = {0}", (uint32_t)*handle);
		
	}

	static void VertexBuffer_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<VertexBuffer>(handle), "VertexBuffer_Destroy VertexBuffer is there, it shoud by Destroyt!");
	}

	static void VertexBuffer_Bind(AssetHandle handle)
	{
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_Bind VertexBuffer is nullptr!");
		vertexBuffer->Bind();
	}

	static void VertexBuffer_UnBind(AssetHandle handle)
	{
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_UnBind VertexBuffer is nullptr!");
		vertexBuffer->UnBind();
	}

	static void VertexBuffer_SetBufferElementToLayout(AssetHandle handle,
		uint32_t elementCount,
		uint32_t elementIndex,
		MonoString* bufferElementName,
		ShaderDataType bufferElementType, bool bufferElementnormilized)
	{
		char* name = mono_string_to_utf8(bufferElementName);

		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_SetLayout VertexBuffer is nullptr!");
		const BufferLayout& layoutChache = vertexBuffer->GetLayout();
		std::vector<BufferElement> bufferElements;

		int index = 0;
		for (auto& bufferElement : layoutChache.GetElements())
		{
			if (index >= elementIndex)
				break;
			bufferElements.emplace_back(bufferElement.Type, bufferElement.Name, bufferElement.Normilized);
			index++;
		}
		bufferElements.emplace_back(bufferElementType, name, bufferElementnormilized);

		vertexBuffer->SetLayout(bufferElements);
		mono_free(name);
	}

	static void VertexBuffer_GetElementFromLayout(AssetHandle handle,
		uint32_t* elementCount,
		uint32_t elementIndex,
		MonoString* bufferElementName,
		ShaderDataType* bufferElementType, uint32_t* bufferElementSize, bool* bufferElementnormilized, uint32_t* bufferElementOffset, uint32_t* bufferElementCount)
	{
	
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_GetLayout VertexBuffer is nullptr!");

		const BufferLayout& layout = vertexBuffer->GetLayout();
		*elementCount = (uint32_t)layout.GetLength();
		if(0 < *elementCount)
		{
			std::vector<BufferElement> bufferElements = layout.GetElements();
			bufferElementName = mono_string_new_wrapper(bufferElements[elementIndex].Name.c_str());
			bufferElements[elementIndex].Type = *bufferElementType;
			bufferElements[elementIndex].Size = *bufferElementSize;
			bufferElements[elementIndex].Offset = *bufferElementOffset;
			bufferElements[elementIndex].Normilized = *bufferElementnormilized;
		}
		
	}

	static void VertexBuffer_SetData(AssetHandle handle, void* data, uint32_t size)
	{

		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_SetData VertexBuffer is nullptr!");
		vertexBuffer->SetData(data, size);
	}

	static void VertexBuffer_GetByteSize(AssetHandle handle, uint32_t* size)
	{
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_GetByteSize VertexBuffer is nullptr!");
		*size = vertexBuffer->GetByteSize();
	}

#pragma endregion

#pragma region IndexBufferClass

	static void IndexBuffer_Create_Indices32_Size_Usage(AssetHandle* handle, uint32_t* indices, uint32_t size, BufferDataUsage usage)
	{
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, size, usage);
		indexBuffer->Handle = AssetHandle();
		indexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<IndexBuffer>(indexBuffer);
		RY_CORE_ASSERT(AssetManager::GetAsset<IndexBuffer>(indexBuffer->Handle) == indexBuffer, "IndexBuffer_Create_Indices32_Size_Usage Created IndexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("IndexBuffer Create form C# id = {0}, renderID = {1}", (uint32_t)*handle, indexBuffer->GetRenderID());
		
	}

	static void IndexBuffer_Create_Indices16_Size_Usage(AssetHandle* handle, uint16_t* indices, uint32_t size, BufferDataUsage usage)
	{
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, size, usage);
		indexBuffer->Handle = AssetHandle();
		indexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<IndexBuffer>(indexBuffer);
		RY_CORE_ASSERT(AssetManager::GetAsset<IndexBuffer>(indexBuffer->Handle) == indexBuffer, "IndexBuffer_Create_Indices16_Size_Usage Created IndexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("IndexBuffer Create form C# id = {0}", (uint32_t)*handle);
	}

	static void IndexBuffer_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<IndexBuffer>(handle), "IndexBuffer_Destroy IndexBuffer is there, it shoud by Destroyt!");
	}

	static void IndexBuffer_Bind(AssetHandle handle)
	{
		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_Bind IndexBuffer is nullptr!");
		indexBuffer->Bind();
	}

	static void IndexBuffer_UnBind(AssetHandle handle)
	{
		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_UnBind IndexBuffer is nullptr!");
		RY_CORE_INFO("IndexBuffer UnBind form C# id = {0}, renderID = {1}", (uint32_t)handle, indexBuffer->GetRenderID());
		indexBuffer->UnBind();
	}

	static void IndexBuffer_SetData32(AssetHandle handle, uint32_t* indices, uint32_t size)
	{
		
		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_SetData32 IndexBuffer is nullptr!");
		indexBuffer->SetData(indices, size);
	}

	static void IndexBuffer_SetData16(AssetHandle handle, uint16_t* indices, uint32_t size)
	{

		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_SetData16 IndexBuffer is nullptr!");
		indexBuffer->SetData(indices, size);
	}

	static void IndexBuffer_GetCount(AssetHandle handle,  uint32_t* count)
	{

		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_GetCount IndexBuffer is nullptr!");
		*count = indexBuffer->GetCount();
	}

	static void IndexBuffer_GetElementByte(AssetHandle handle, uint32_t* elementByte)
	{
		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_GetElementByte IndexBuffer is nullptr!");
		*elementByte = indexBuffer->GetElementByte();
	}

#pragma endregion

#pragma region VertexArrayClass

	static void VertexArray_Create(AssetHandle* handle)
	{

		Ref<VertexArray> vertexArray = VertexArray::Create();
		vertexArray->Handle = AssetHandle();
		vertexArray->Handle = *handle = AssetManager::CreatLocaleAsset<VertexArray>(vertexArray);
		RY_CORE_ASSERT(AssetManager::GetAsset<VertexArray>(vertexArray->Handle) == vertexArray, "VertexArray_Create Created VertexArray is not the same like in the Regestry!");
		RY_CORE_INFO("VertexArray Create form C# id = {0}", (uint32_t)*handle);
	}

	static void VertexArray_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<VertexArray>(handle), "VertexArray_Destroy VertexArray is there, it shoud by Destroyt!");
	}

	static void VertexArray_Bind(AssetHandle handle)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_Bind VertexArray is nullptr!");
		vertexArray->Bind();
	}

	static void VertexArray_UnBind(AssetHandle handle)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_UnBind VertexArray is nullptr!");
		vertexArray->UnBind();
	}

	static void VertexArray_AddVertexBuffer(AssetHandle handle, AssetHandle handleVertex)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_AddVertexBuffer VertexArray is nullptr!");
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetAsset<VertexBuffer>(handleVertex);
		RY_CORE_ASSERT(vertexBuffer, "VertexArray_AddVertexBuffer VertexBuffer is nullptr!");
		vertexArray->AddVertexBuffer(vertexBuffer);
	}

	static void VertexArray_SetIndexBuffer(AssetHandle handle, AssetHandle handleIndex)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_SetIndexBuffer Texture is nullptr!");
		Ref<IndexBuffer> indexBuffer = AssetManager::GetAsset<IndexBuffer>(handleIndex);
		RY_CORE_ASSERT(indexBuffer, "VertexArray_SetIndexBuffer IndexBuffer is nullptr!");
		vertexArray->SetIndexBuffer(indexBuffer);
	}

	static void VertexArray_SetPrimitv(AssetHandle handle, VertexArray::Primitv primitv)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_SetPrimitv VertexArray is nullptr!");
		vertexArray->SetPrimitv(primitv);
		
	}

	static void VertexArray_GetPrimitv(AssetHandle handle, VertexArray::Primitv* primitv)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_GetPrimitv VertexArray is nullptr!");
		*primitv = vertexArray->GetPrimitv();
	}
	
#pragma endregion

#pragma region SceneClass

	static void Scene_CreateEntity(MonoString* nameM ,UUID* entityID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		std::string name = Utils::MonoStringToString(nameM);
		Entity entity = scene->CreateEntity(name);
		*entityID = entity.GetUUID();
	}

	static bool Scene_GetEntityByName(MonoString* entityNameM, UUID* entityID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		std::string entityTag = Utils::MonoStringToString(entityNameM);
		Entity entity = scene->GetEntityByName(entityTag);
		*entityID = entity ? entity.GetUUID() : UUID(0);
		return entity;
	}

	static bool Scene_GetEntityByEntityID(UUID entityID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		return entity;
	}

	static void Scene_SetBackGround(glm::vec4* backGround)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		scene->SetBackgroundColor(*backGround);
	}

#pragma endregion

#pragma region FramebufferClass

	static void Framebuffer_Create_Spec(AssetHandle* handle, FramebufferTextureSpecification* attachments, uint32_t size, uint32_t width, uint32_t height, bool swapChainTarget)
	{
		std::vector<FramebufferTextureSpecification> fBspec(size);
		std::memcpy(attachments, fBspec.data(), size * sizeof(FramebufferTextureSpecification));
		FramebufferSpecification spec;
		spec.Attachments.Attachments = fBspec;
		spec.SwapChainTarget = swapChainTarget;
		spec.Width = width;
		spec.Height = height;
		Ref<Framebuffer> framebuffer = Framebuffer::Create(spec);
		framebuffer->Handle = AssetHandle();
		framebuffer->Handle = *handle = AssetManager::CreatLocaleAsset<Framebuffer>(framebuffer);
		RY_CORE_ASSERT(AssetManager::GetAsset<Framebuffer>(framebuffer->Handle) == framebuffer, "Framebuffer_Create_Spec Created Framebuffer is not the same like in the Regestry!");
		RY_CORE_INFO("VertexBuffer Create form C# id = {0}", (uint32_t)*handle);
	}

	static void Framebuffer_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<Framebuffer>(handle), "Texture_Destroy Texture is there, it shoud by Destroyt!");
	}

	static void Framebuffer_GetFramebufferSpecification(AssetHandle handle, uint32_t* size, uint32_t* width, uint32_t* height,  bool* swapChainTarget)
	{
		Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);
		RY_CORE_ASSERT(frambuffer, "Framebuffer is nullptr!");
		const FramebufferSpecification& spec = frambuffer->GetFramebufferSpecification();
		*size = spec.Attachments.Attachments.size();
		*width = spec.Width;
		*height = spec.Height;
		
		*swapChainTarget = spec.SwapChainTarget;
	}

	static void Framebuffer_GetFramebufferTextureSpecification(AssetHandle handle, uint32_t index, FramebufferTextureSpecification* attachment)
	{
		Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);
		RY_CORE_ASSERT(frambuffer, "Framebuffer is nullptr!");
		const FramebufferSpecification& spec = frambuffer->GetFramebufferSpecification();
		*attachment = spec.Attachments.Attachments[index];
	}


	static void Framebuffer_Resize(AssetHandle handle, uint32_t withe, uint32_t heigth)
	{
		Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);
		RY_CORE_ASSERT(frambuffer, "Framebuffer is nullptr!");
		frambuffer->Resize(withe, heigth);
	}

	static void Framebuffer_ReadPixel(AssetHandle handle, uint32_t attachmentsIndex, int x, int y, int* value)
	{
		RY_CORE_WARN("This Funktion can, make the Programm Slow Don't call to many times!, or in Game not at all if posible!");
		Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);
		RY_CORE_ASSERT(frambuffer, "Framebuffer is nullptr!");
		*value=frambuffer->ReadPixel(attachmentsIndex, x, y);
	}

	static void Framebuffer_GetFrambufferSize(AssetHandle handle, glm::vec2* size)
	{
		Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);		
		RY_CORE_ASSERT(frambuffer, "Framebuffer is nullptr!");
		*size = (glm::vec2)frambuffer->GetFrambufferSize();
	}

#pragma endregion

#pragma region AssetManger

	static void AssetManger_GetAsset_Path(MonoString* pathM, AssetType type, AssetHandle* handle)
	{
		std::string path = Utils::MonoStringToString(pathM);
		switch (type)
		{
		case AssetType::None:
		{
			*handle = AssetHandle(0);
			break;
		}
		case  AssetType::Scene:
		{
			Ref<Scene> asset = AssetManager::GetAsset<Scene>(path);
			*handle = asset->Handle;
			break;
		}
		case  AssetType::Shader:
		{
			Ref<Shader> asset = AssetManager::GetAsset<Shader>(path);
			*handle = asset->Handle;
			break;
		}
		case  AssetType::Texture:
		{
			Ref<Texture> asset = AssetManager::GetAsset<Texture>(path);
			*handle = asset->Handle;
			break;
		}
		case  AssetType::Framebuffer:
		{
			Ref<Framebuffer> asset = AssetManager::GetAsset<Framebuffer>(path);
			*handle = asset->Handle;
			break;
		}
		case  AssetType::VertexBuffer:
		{
			Ref<VertexBuffer> asset = AssetManager::GetAsset<VertexBuffer>(path);
			*handle = asset->Handle;
			break;
		}
		case  AssetType::IndexBuffer:
		{
			Ref<IndexBuffer> asset = AssetManager::GetAsset<IndexBuffer>(path);
			*handle = asset->Handle;
			break;
		}
		case AssetType::StorageBuffer:
		{
			RY_CORE_WARN("StorageBuffer is not many times teste or used befor!");
			Ref<StorageBuffer> asset = AssetManager::GetAsset<StorageBuffer>(path);
			*handle = asset->Handle;
			break;
		}
		case  AssetType::VertexArray:
		{
			Ref<VertexArray> asset = AssetManager::GetAsset<VertexArray>(path);
			*handle = asset->Handle;
			break;
		}
#if 0
		case  AssetType::StorageArray:
		{
			Ref<StorageArray> asset = AssetManager::GetAsset<StorageArray>(path);
			*handle = asset->Handle;
			break;
		}
#endif // TODO: Add StorageArray to a usebile State!
		case AssetType::Model:
		{
			Ref<Model> asset = AssetManager::GetAsset<Model>(path);
			*handle = asset->Handle;
			break;
		}
		default:
			RY_CORE_ASSERT(false);
			break;
		}
		RY_CORE_ASSERT(*handle != 0);
	}

	static void AssetManger_GetAsset_Handle(AssetHandle handle, AssetType type, AssetHandle* outhandle)
	{
		switch (type)
		{
		case AssetType::None:
		{
			*outhandle = AssetHandle(0);
			break;
		}
		case  AssetType::Scene:
		{
			Ref<Scene> asset = AssetManager::GetAsset<Scene>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case  AssetType::Shader:
		{
			Ref<Shader> asset = AssetManager::GetAsset<Shader>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case  AssetType::Texture:
		{
			Ref<Texture> asset = AssetManager::GetAsset<Texture>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case  AssetType::Framebuffer:
		{
			Ref<Framebuffer> asset = AssetManager::GetAsset<Framebuffer>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case  AssetType::VertexBuffer:
		{
			Ref<VertexBuffer> asset = AssetManager::GetAsset<VertexBuffer>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case  AssetType::IndexBuffer:
		{
			Ref<IndexBuffer> asset = AssetManager::GetAsset<IndexBuffer>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case AssetType::StorageBuffer:
		{
			RY_CORE_WARN("StorageBuffer is not many times teste or used befor!");
			Ref<StorageBuffer> asset = AssetManager::GetAsset<StorageBuffer>(handle);
			*outhandle = asset->Handle;
			break;
		}
		case  AssetType::VertexArray:
		{
			Ref<VertexArray> asset = AssetManager::GetAsset<VertexArray>(handle);
			*outhandle = asset->Handle;
			break;
		}
#if 0
		case  AssetType::StorageArray:
		{
			Ref<StorageArray> asset = AssetManager::GetAsset<StorageArray>(path);
			*handle = asset->Handle;
			break;
		}
#endif // TODO: Add StorageArray to a usebile State!
		case AssetType::Model:
		{
			Ref<Model> asset = AssetManager::GetAsset<Model>(handle);
			*outhandle = asset->Handle;
			break;
		}
		default:
			RY_CORE_ASSERT(false);
			break;
		}
		RY_CORE_ASSERT(*outhandle != 0);

	}

#pragma endregion

#pragma region ModelClass

	static void Model_Create(AssetHandle* handle, TextureSpecification* spec)
	{

		Ref<Texture> texture = Texture::Create(*spec);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}

	static void Model_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetAsset<Texture>(handle), "Texture_Destroy Texture is there, it shoud by Destroyt!");
	}

#pragma endregion



#pragma region Event

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	static bool Input_IsMouseOnViewPort()
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		return scene->IsViewPortHovered();
	}

	static bool Input_IsWindowResize()
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		return scene->IsWindowResize();
	}

	static UUID Input_GetEntityID_From_MainScreen(uint32_t x, uint32_t y)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		return scene->IsWindowResize();
	}

#pragma endregion


#pragma region AplicationClass

	static glm::vec2 Application_GetWindowSize()
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		return scene->GetViewPortSize();
	}

	static glm::vec2 Application_GetMousePixelPosition()
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		return scene->GetMousPixelPos();
	}

#pragma endregion


#pragma endregion	


#pragma region ScriptEntityComponetRegister

	template<typename Component>
	static void CheckComponentListCore(MonoType* managedType, const std::string& managedTypename)
	{
		if (!managedType)
		{
			RY_CORE_ERROR("Could not find component type {} in Core: nullptr",  managedTypename.c_str());
			return;
		}
		else
		{
			RY_CORE_INFO("Could find component type {} as '{}' in Cora",  managedTypename.c_str(), (void*)managedType);
		}


		s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		s_EntityAddComponentFuncs[managedType] = [](Entity entity) { entity.AddComponent<Component>(); };
		s_EntityRemoveComponentFuncs[managedType] = [](Entity entity) {  entity.RemoveComponent<Component>(); };
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([&]()
			{
				std::string_view typeName = typeid(Component).name();
				
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Rynex.{}", structName);

				MonoType* managedTypeCore = mono_reflection_type_from_name(managedTypename.data(), ScriptingEngine::GetCoreAssemblyImage());
				MonoType* managedTypeApp = mono_reflection_type_from_name(managedTypename.data(), ScriptingEngine::GetAppAssemblyImage());
				
				RY_CORE_ASSERT(!managedTypeApp, ("Componet shound not be in App image!"));

				CheckComponentListCore<Component>(managedTypeCore, managedTypename);
			}(), ...);

	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		s_EntityAddComponentFuncs.clear();
		s_EntityRemoveComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

#pragma endregion


	void ScriptGlue::RegisterFunctions()
	{
		RY_ADD_INTERNAL_CALL(NativLog);
		RY_ADD_INTERNAL_CALL(NativLog_Vec3);
		RY_ADD_INTERNAL_CALL(NativLog_Vec3Dot);


#pragma region Entity

		RY_ADD_INTERNAL_CALL(Entity_HasComponent);
		RY_ADD_INTERNAL_CALL(Entity_AddComponent);
		RY_ADD_INTERNAL_CALL(Entity_RemoveComponent);
		RY_ADD_INTERNAL_CALL(Entity_CreateChilde);
		RY_ADD_INTERNAL_CALL(Entity_UpdateMatrixe);
		RY_ADD_INTERNAL_CALL(Entity_Dealete);

#pragma endregion


#pragma region TransformComponent

		RY_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		RY_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		RY_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetScale);


#pragma endregion

#pragma region GeomtryComponent

		RY_ADD_INTERNAL_CALL(GeomtryComponent_SetIndex);
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetVertex);
		RY_ADD_INTERNAL_CALL(GeomtryComponent_SetPrimitv);
		// new
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetBuffer);
		RY_ADD_INTERNAL_CALL(GeometryComponent_GetBuffer);
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetGeometry);
		RY_ADD_INTERNAL_CALL(GeometryComponent_GetGeometry);

#pragma endregion

#pragma region TagComponent

		RY_ADD_INTERNAL_CALL(TagComponent_SetTag);
		RY_ADD_INTERNAL_CALL(TagComponent_GetTag);

#pragma endregion

#pragma region SpriteRendererComponent

		RY_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		RY_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		RY_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTexture);
		RY_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTexture);

#pragma endregion

#pragma region CameraComponent

		RY_ADD_INTERNAL_CALL(CameraComponent_SetPrimary);
		RY_ADD_INTERNAL_CALL(CameraComponent_GetPrimary);
		RY_ADD_INTERNAL_CALL(CameraComponent_SetFixedAspectRotaion);
		RY_ADD_INTERNAL_CALL(CameraComponent_GetFixedAspectRotaion);

#pragma region SceneCamer

		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetViewPortSize);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetOrthoGrafic);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetPerspectiv);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetOrthograficSize);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetOrthograficNearClipe);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetOrthograficFarClipe);

		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetOrthographicSize);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetOrthographicNearClipe);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetOrthographicNearClipe);

		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetPerspectivVerticleFOV);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetPerspectivNearClipe);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetPerspectivFarClipe);

		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetPerspectivVerticleFOV);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetPerspectivNearClipe);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetPerspectivFarClipe);

		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_GetProjectionType);
		RY_ADD_INTERNAL_CALL(CameraComponent_Camera_SetProjectionType);

#pragma endregion

#pragma endregion

#pragma region ScriptComponent

		RY_ADD_INTERNAL_CALL(ScriptComponent_SetName);
		RY_ADD_INTERNAL_CALL(ScriptComponent_GetName);

#pragma endregion

#pragma region MaterialComponent
#if ENABLE_MATERIL_COMPONET
		RY_ADD_INTERNAL_CALL(MaterialComponent_SetColor);
		RY_ADD_INTERNAL_CALL(MaterialComponent_GetColor);
		RY_ADD_INTERNAL_CALL(MaterialComponent_SetShader);
		RY_ADD_INTERNAL_CALL(MaterialComponent_GetShader);
#endif
#pragma endregion

#pragma region ModelMatrixComponent

		RY_ADD_INTERNAL_CALL(ModelMatrixComponent_SetMatrix4x4);
		RY_ADD_INTERNAL_CALL(ModelMatrixComponent_GetMatrix4x4);
		RY_ADD_INTERNAL_CALL(ModelMatrixComponent_SetGlobleMatrix4x4);
		RY_ADD_INTERNAL_CALL(ModelMatrixComponent_GetGlobleMatrix4x4);

#pragma endregion

#pragma region FrameBufferComponent

		RY_ADD_INTERNAL_CALL(FrameBufferComponent_SetFrameBuffer);
		RY_ADD_INTERNAL_CALL(FrameBufferComponent_GetFrameBuffer);

#pragma endregion

#pragma region MeshComponent

		RY_ADD_INTERNAL_CALL(MeshComponent_SetModelR);
		RY_ADD_INTERNAL_CALL(MeshComponent_GetModelR);
		RY_ADD_INTERNAL_CALL(MeshComponent_SetMeshMode);
		RY_ADD_INTERNAL_CALL(MeshComponent_GetModelR);

#pragma endregion

#pragma region AmbientLigthComponent

		RY_ADD_INTERNAL_CALL(AmbientLigthComponent_SetColor);
		RY_ADD_INTERNAL_CALL(AmbientLigthComponent_GetColor);
		RY_ADD_INTERNAL_CALL(AmbientLigthComponent_SetIntensitie);
		RY_ADD_INTERNAL_CALL(AmbientLigthComponent_GeIntensitie);

#pragma endregion

#pragma region DrirektionleLigthComponent


		RY_ADD_INTERNAL_CALL(DrirektionleLigthComponent_SetColor);
		RY_ADD_INTERNAL_CALL(DrirektionleLigthComponent_GetColor);
		RY_ADD_INTERNAL_CALL(DrirektionleLigthComponent_SetIntensitie);
		RY_ADD_INTERNAL_CALL(DrirektionleLigthComponent_GetIntensitie);

#pragma endregion

#pragma region PointLigthComponent

		RY_ADD_INTERNAL_CALL(PointLigthComponent_SetColor);
		RY_ADD_INTERNAL_CALL(PointLigthComponent_GetColor);
		RY_ADD_INTERNAL_CALL(PointLigthComponent_SetIntensitie);
		RY_ADD_INTERNAL_CALL(PointLigthComponent_GetIntensitie);
		RY_ADD_INTERNAL_CALL(PointLigthComponent_SetDistence);
		RY_ADD_INTERNAL_CALL(PointLigthComponent_GetDistence);

#pragma endregion

#pragma region SpotLigthComponent

		RY_ADD_INTERNAL_CALL(SpotLigthComponent_SetColor);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_GetColor);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_SetIntensitie);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_GetIntensitie);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_SetDistence);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_GetDistence);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_SetInner);
		RY_ADD_INTERNAL_CALL(SpotLigthComponent_GetInner);

#pragma endregion

#pragma region TextComponent

		RY_ADD_INTERNAL_CALL(TextComponent_SetString);
		RY_ADD_INTERNAL_CALL(TextComponent_GetString);
		RY_ADD_INTERNAL_CALL(TextComponent_SetColor);
		RY_ADD_INTERNAL_CALL(TextComponent_GetColor);
		RY_ADD_INTERNAL_CALL(TextComponent_SetKerning);
		RY_ADD_INTERNAL_CALL(TextComponent_GetKerning);
		RY_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);
		RY_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);

#pragma endregion


#pragma region TextureClass

		// RY_ADD_INTERNAL_CALL(Texture_Create_Withe_Heigth);
		RY_ADD_INTERNAL_CALL(Texture_Create_Spec);
		RY_ADD_INTERNAL_CALL(Texture_Create_Spec_Data_ByteSize);
		RY_ADD_INTERNAL_CALL(Texture_Destroy);
		RY_ADD_INTERNAL_CALL(Texture_GetSpecification);
		RY_ADD_INTERNAL_CALL(Texture_GetRenderID);
		RY_ADD_INTERNAL_CALL(Texture_GetHeight);
		RY_ADD_INTERNAL_CALL(Texture_GetWidth);
		RY_ADD_INTERNAL_CALL(Texture_Bind);
		RY_ADD_INTERNAL_CALL(Texture_Resize);
		RY_ADD_INTERNAL_CALL(Texture_SetData);

#pragma endregion

#pragma region ShaderClass

		RY_ADD_INTERNAL_CALL(Shader_Create_Source_Name);
		RY_ADD_INTERNAL_CALL(Shader_Create_Source_Name_VertexSrc_FragmentSrc);
		RY_ADD_INTERNAL_CALL(Shader_Destroy);
		RY_ADD_INTERNAL_CALL(Shader_ReganrateShader);
		RY_ADD_INTERNAL_CALL(Shader_Bind);
		RY_ADD_INTERNAL_CALL(Shader_UnBind);
		RY_ADD_INTERNAL_CALL(Shader_SetUniformValue);
		RY_ADD_INTERNAL_CALL(Shader_SetInt);
		RY_ADD_INTERNAL_CALL(Shader_SetIntArray);

#pragma endregion

#pragma region VertexBufferClass

		RY_ADD_INTERNAL_CALL(VertexBuffer_Create_Vertices_Size);
		RY_ADD_INTERNAL_CALL(VertexBuffer_Create_Vertices_Size_Usage);
		RY_ADD_INTERNAL_CALL(VertexBuffer_Destroy);
		RY_ADD_INTERNAL_CALL(VertexBuffer_GetByteSize);
		RY_ADD_INTERNAL_CALL(VertexBuffer_SetData);
		RY_ADD_INTERNAL_CALL(VertexBuffer_Bind);
		RY_ADD_INTERNAL_CALL(VertexBuffer_UnBind);
		RY_ADD_INTERNAL_CALL(VertexBuffer_GetElementFromLayout);
		RY_ADD_INTERNAL_CALL(VertexBuffer_SetBufferElementToLayout);
		RY_ADD_INTERNAL_CALL(VertexBuffer_SetData);

#pragma endregion

#pragma region IndexBufferClass

		RY_ADD_INTERNAL_CALL(IndexBuffer_Create_Indices32_Size_Usage);
		RY_ADD_INTERNAL_CALL(IndexBuffer_Create_Indices16_Size_Usage);
		RY_ADD_INTERNAL_CALL(IndexBuffer_Destroy);
		RY_ADD_INTERNAL_CALL(IndexBuffer_Bind);
		RY_ADD_INTERNAL_CALL(IndexBuffer_UnBind);
		RY_ADD_INTERNAL_CALL(IndexBuffer_SetData32);
		RY_ADD_INTERNAL_CALL(IndexBuffer_SetData16);
		RY_ADD_INTERNAL_CALL(IndexBuffer_GetCount);
		RY_ADD_INTERNAL_CALL(IndexBuffer_GetElementByte);

#pragma endregion

#pragma region VertexArrayClass

		RY_ADD_INTERNAL_CALL(VertexArray_Create);
		RY_ADD_INTERNAL_CALL(VertexArray_Destroy);
		RY_ADD_INTERNAL_CALL(VertexArray_Bind);
		RY_ADD_INTERNAL_CALL(VertexArray_UnBind);
		RY_ADD_INTERNAL_CALL(VertexArray_AddVertexBuffer);
		RY_ADD_INTERNAL_CALL(VertexArray_SetIndexBuffer);
		RY_ADD_INTERNAL_CALL(VertexArray_SetPrimitv);
		RY_ADD_INTERNAL_CALL(VertexArray_GetPrimitv);

#pragma endregion

#pragma region SceneClass

		RY_ADD_INTERNAL_CALL(Scene_CreateEntity);
		RY_ADD_INTERNAL_CALL(Scene_GetEntityByName);
		RY_ADD_INTERNAL_CALL(Scene_GetEntityByEntityID);

#pragma endregion

#pragma region FrambufferClass

		RY_ADD_INTERNAL_CALL(Framebuffer_Create_Spec);
		RY_ADD_INTERNAL_CALL(Framebuffer_Destroy);
		RY_ADD_INTERNAL_CALL(Framebuffer_GetFramebufferSpecification);
		RY_ADD_INTERNAL_CALL(Framebuffer_GetFramebufferTextureSpecification);
		RY_ADD_INTERNAL_CALL(Framebuffer_Resize);
		RY_ADD_INTERNAL_CALL(Framebuffer_ReadPixel);
		RY_ADD_INTERNAL_CALL(Framebuffer_GetFrambufferSize);

#pragma endregion

#pragma region AssetManagerClass

		RY_ADD_INTERNAL_CALL(AssetManger_GetAsset_Path);
		RY_ADD_INTERNAL_CALL(AssetManger_GetAsset_Handle);

#pragma endregion

#pragma region ApplicationClass

		RY_ADD_INTERNAL_CALL(Application_GetMousePixelPosition);
		RY_ADD_INTERNAL_CALL(Application_GetWindowSize);

#pragma endregion


#pragma region Event

		RY_ADD_INTERNAL_CALL(Input_IsKeyDown);
		RY_ADD_INTERNAL_CALL(Input_IsMouseOnViewPort);
		RY_ADD_INTERNAL_CALL(Input_IsWindowResize);

#pragma endregion

	}

	
}