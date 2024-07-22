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

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

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
	// TransformComponent
	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		*outTranslation = entity.GetComponent<TransformComponent>().Transaltion;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* inTranslation)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		entity.GetComponent<TransformComponent>().Transaltion = *inTranslation;
	}

	
	struct GeomtryInput
	{
		glm::vec3 position; 
		glm::vec2 uv;
		glm::vec3 nomale;
	};

	// GeomtryComponent
	static void GeometryComponent_SetVertex(UUID entityID, void* vertex, uint32_t byteSize)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;


		GeomtryInput* vertexInput = (GeomtryInput*)vertex;
		//																	 |
		// Byte Size														 V
		entity.GetComponent<GeomtryComponent>().Buffer->SetData(vertexInput, byteSize);
		
		for (int i = 0; i < 3; i++)
			RY_CORE_TRACE("postion : {0}, {1}, {2}", vertexInput[i].position.x, vertexInput[i].position.y, vertexInput[i].position.z);
		for (int i = 0; i < 3; i++)
			RY_CORE_TRACE("normal : {0}, {1}, {2}", vertexInput[i].nomale.x, vertexInput[i].nomale.y, vertexInput[i].nomale.z);
		for (int i = 0; i < 3; i++)
			RY_CORE_TRACE("uv : {0}, {1}", vertexInput[i].uv.x, vertexInput[i].uv.y);
		

	}

	static void GeomtryComponent_SetIndex(UUID entityID, uint32_t* index, uint32_t count)
	{

		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;
		entity.GetComponent<GeomtryComponent>().Geometry->GetIndexBuffers()->SetData(index, count);
	}


	// Components Defins
#if USE_HASCOMPONET_FUNC_CS
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
#if 0
		Scene* scene = ScriptingEngine::GetSceneContext();
		RY_CORE_ASSERT(scene, "Error: Entity_HasComponent");
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Error: Entity_HasComponent");

	
		MonoType* manageType = mono_reflection_type_get_type(componentType);
		auto end = s_EntityHasComponentFunc.end();
		auto find = s_EntityHasComponentFunc.find(manageType);
		auto& result = s_EntityHasComponentFunc[manageType];
		RY_CORE_ASSERT(s_EntityHasComponentFunc.find(manageType) != s_EntityHasComponentFunc.end(), "Error: Entity_HasComponent");
		
		return s_EntityHasComponentFunc.at(manageType)(entity);
#else
		return false;
#endif
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
					
				}
				else
				{
					RY_CORE_INFO("Could find component type {}", managedTypename);
					s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
				}
			}(), ...);


	}

	template<typename... Component>
	static void RegisterComponents(ComponentGroup<Component...>)
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
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetVertex);

		// Input
		RY_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}


	void ScriptGlue::RegisterAllComponets()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponents(AllComponents{});
	}
}