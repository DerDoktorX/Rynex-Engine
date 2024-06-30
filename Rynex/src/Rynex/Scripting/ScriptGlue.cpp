#include "rypch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include <glm/glm.hpp>

#include "mono/metadata/object.h"
#include <mono/metadata/reflection.h>

#include <Rynex/Core/KeyCodes.h>
#include <Rynex/Core/Input.h>

#define USE_HASCOMPONET_FUNC_CS 0

namespace Rynex {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFunc;

#define RY_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Rynex.InternalCalls::" #Name, Name)
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


	// Entity Defins
	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().Transaltion;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* inTranslation)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		entity.GetComponent<TransformComponent>().Transaltion = *inTranslation;
	}

	static void GeomtryComponent_SetVertex(UUID entityID, const void* vertex, uint32_t size)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		entity.GetComponent<GeomtryComponent>().Buffer->SetData(vertex, size);
	}

	static void GeomtryComponent_SetIndex(UUID entityID, const uint32_t* index, uint32_t size)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		entity.GetComponent<GeomtryComponent>().Geometry->GetIndexBuffers()->SetData(index, size);
	}


	// Components Defins
#if USE_HASCOMPONET_FUNC_CS
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		RY_CORE_ASSERT(scene, "Error: Entity_HasComponent");
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Error: Entity_HasComponent");

	
		MonoType* monoComponentType = mono_reflection_type_get_type(componentType);
		auto end = s_EntityHasComponentFunc.end();
		auto find = s_EntityHasComponentFunc.find(monoComponentType);
		auto& result = s_EntityHasComponentFunc[monoComponentType];
		RY_CORE_ASSERT(s_EntityHasComponentFunc.find(monoComponentType) != s_EntityHasComponentFunc.end(), "Error: Entity_HasComponent");
		
		return s_EntityHasComponentFunc.at(monoComponentType)(entity);
		
	}
#endif // TODO: Find out what is going on why is That not working?

	// Input Defins
	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename ... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Rynex.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptingEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					
					RY_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				RY_CORE_INFO("Could find component type {}", managedTypename);
				s_EntityHasComponentFunc[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}


	void ScriptGlue::RegisterFunktion()
	{
		// Exampels
		RY_ADD_INTERNAL_CALL(NativLog);
		RY_ADD_INTERNAL_CALL(NativLog_Vec3);
		RY_ADD_INTERNAL_CALL(NativLog_Vec3Dot);

		// Components
#if USE_HASCOMPONET_FUNC_CS
		RY_ADD_INTERNAL_CALL(Entity_HasComponent);
#endif
		// Entity
		RY_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);		

		// Geomtry
		RY_ADD_INTERNAL_CALL(GeomtryComponent_SetIndex);
		RY_ADD_INTERNAL_CALL(GeomtryComponent_SetVertex);

		// Input
		RY_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}


	void ScriptGlue::RegisterComponets()
	{
		s_EntityHasComponentFunc.clear();
		RegisterComponent(AllComponents{});
	}
}