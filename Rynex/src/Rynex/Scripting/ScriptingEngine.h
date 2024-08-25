#pragma once

#include "Rynex/Scene/Scene.h"
#include "Rynex/Scene/Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Rynex {

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		void Shutdown();

		MonoObject* Instantiate();
		MonoMethod* GetMethode(const std::string& name, int prameCount = 0);
		MonoObject* InvokeMethode(MonoMethod* methode, MonoObject* instance, void** params = nullptr);
	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeCreate();
		void InvokeOnUpdate(float ts);
		void InvokeOnDrawn();
		void InvokeOnDestroy();
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
		MonoMethod* m_OnDrawMethod = nullptr;
		MonoMethod* m_OnDestroyMethod = nullptr;

		friend class ScriptEngine;
		
	};

	class ScriptingEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssambly(const std::filesystem::path& filepath);
		static void LoadAppAssambly(const std::filesystem::path& filepath);
		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static void ReloadAssambly();
		static bool ReloadeScriptAvaible();

		static std::string* GetListExistClasses();
		static std::string& GetListClassName(uint32_t count);
		static uint32_t GetClassLength();

		static bool ClassExists(const std::string& fullClassName);
		static void OnCreatEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, float ts);
		static void OnDrawEntity(Entity entity);
		static Scene* GetSceneContext();

		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
		
		static MonoImage* GetCoreAssemblyImage();
	private:
		static void InitMono();
		static void ShutdownMono();
		static void ExecuteScriptClass();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();
		
		friend class ScriptClass;
		friend class ScriptGlue;
	};
	

	
}


