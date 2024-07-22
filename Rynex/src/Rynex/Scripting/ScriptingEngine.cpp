#include "rypch.h"
#include "ScriptingEngine.h"
#include "ScriptGlue.h"

#include "Rynex/Core/Application.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include "FileWatch.h"

#define RY_AKICE_CS 1
namespace Rynex {

	
	


	///////////////////////////////////////////////////////////////////////
	///// ScriptingEngineData /////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	struct ScriptingEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		ScriptClass EntityClass;

		std::string* ClassList;
		uint32_t ListIndex = 0;

		Scene* SceneContext = nullptr;
		
		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
		
		Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyRelodingPennding = false;

		bool AsseblyReloading = false;
	};



	static ScriptingEngineData* s_Data = nullptr;
	


	///////////////////////////////////////////////////////////////////////
	///// Utils ///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	
	namespace Utils {

		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);
			
			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}
			std::string string = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, string.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}
		
		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				//MonoClass* monoClass = mono_class_from_name(s_Data->CoreAssemblyImage, nameSpace, name);
				//MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Rynex", "Entity");

				//bool isEntity = mono_class_from_name(s_Data->CoreAssemblyImage, "Rynex", "Entity");

				RY_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}

	}



	///////////////////////////////////////////////////////////////////////
	///// ScriptingEngine -> public ///////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	
	void ScriptingEngine::Init()
	{
		s_Data = new ScriptingEngineData();

		InitMono();

		// Funktion in C++ defined executed in C#
		ScriptGlue::RegisterFunktion();
		

		LoadAssambly("Resources/Scripts/Rynex-ScriptingCore.dll");
		LoadAppAssambly("SandboxProject/Assets/Scripts/Binaries/Sanbox.dll");
		LoadAssemblyClasses();

		ScriptGlue::RegisterAllComponets();

		//ExecuteScriptClass();
		s_Data->EntityClass = ScriptClass("Rynex", "Entity", true);
#if 0
		MonoObject* instance = s_Data->EntityClass.Instantiate();

		MonoMethod* onCreateFunc = s_Data->EntityClass.GetMethode("OnCreate");
		s_Data->EntityClass.InvokeMethode(onCreateFunc, instance);


		// Class in C# defined executed in C++
		
#endif
#if 0
		filewatch::FileWatch<std::string>* watch = new filewatch::FileWatch<std::string>(
			"SandboxProject/Assets/Scripts/Binaries",
			[](const std::string& path, const filewatch::Event change_type) {
				RY_CORE_INFO("FileWatcher: {0}\n> {1}", path.c_str(), (int)change_type);

			}
		);
#endif
	}

	void ScriptingEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}

	void ScriptingEngine::LoadAssambly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_Data->AppDomain = mono_domain_create_appdomain("RynexScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Maby move this
		s_Data->CoreAssemblyFilepath = filepath;
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		
		ScriptGlue::RegisterAllComponets();
	}

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		RY_CORE_INFO("FileWatcher: {0}\n> {1}",(int)change_type , path.c_str());
		if (s_Data->AssemblyRelodingPennding && change_type == filewatch::Event::modified)
		{
			
#if 1
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
#endif

			RY_CORE_INFO("Thread Execute!");
			Application::Get().SubmiteToMainThreedQueue([]() 
			{ 
				s_Data->AppAssemblyFileWatcher.reset();
				ScriptingEngine::ReloadAssambly(); 
				
			});
		}
		else if (!s_Data->AssemblyRelodingPennding && change_type == filewatch::Event::modified)
		{
			s_Data->AssemblyRelodingPennding = true;
		}


	}


	void ScriptingEngine::LoadAppAssambly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_Data->EntityClasses.clear();
		s_Data->AppAssemblyFilepath = filepath;
		s_Data->AppDomain = mono_domain_create_appdomain("RynexScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Maby move this
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
		//Utils::PrintAssemblyTypes(s_Data->AppAssambly);
		
		s_Data->AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
		s_Data->AsseblyReloading = true;
		s_Data->AssemblyRelodingPennding = false;
	}

	void ScriptingEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptingEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;
	}

	void ScriptingEngine::ReloadAssambly()
	{
		mono_domain_set(mono_get_root_domain(), false);
		
		//mono_domain_free(s_Data->AppDomain, true);
		mono_domain_unload(s_Data->AppDomain);

		LoadAssambly(s_Data->CoreAssemblyFilepath);
		LoadAppAssambly(s_Data->AppAssemblyFilepath);
		LoadAssemblyClasses();

		s_Data->EntityClass = ScriptClass("Rynex", "Entity", true);
	}

	bool ScriptingEngine::ReloadeScriptAvaible()
	{
		bool state = s_Data->AsseblyReloading;
		s_Data->AsseblyReloading = false;
		return state;
	}

#if 1
	std::string* ScriptingEngine::GetListExistClasses()
	{
		return s_Data->ClassList;
	}

	std::string& ScriptingEngine::GetListClassName(uint32_t count)
	{
		return s_Data->ClassList[count];
	}

	uint32_t ScriptingEngine::GetClassLength()
	{
		return s_Data->ListIndex;
	}
#endif

	bool ScriptingEngine::ClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	void ScriptingEngine::OnCreatEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptingEngine::ClassExists(sc.Name))
		{
			UUID entityID = entity.GetUUID();
			auto& clases = s_Data->EntityClasses; 
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.Name], entity);
			s_Data->EntityInstances[entityID] = instance;
			
			instance->InvokeCreate();
		}
	}

	void ScriptingEngine::OnUpdateEntity(Entity entity, float ts)
	{
		UUID entityID = entity.GetUUID();
		
		RY_CORE_ASSERT(s_Data->EntityInstances.find(entityID) != s_Data->EntityInstances.end(), "Error: OnUpdateEntity()");

		Ref<ScriptInstance> instance = s_Data->EntityInstances[entityID];
		instance->InvokeOnUpdate(ts);
	}

	void ScriptingEngine::OnDrawEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptingEngine::ClassExists(sc.Name))
		{
			UUID entityID = entity.GetUUID();
			auto& clases = s_Data->EntityClasses;
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.Name], entity);
			s_Data->EntityInstances[entityID] = instance;

			instance->InvokeOnDrawn();

		}

	}

	Scene* ScriptingEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptingEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	

	///////////////////////////////////////////////////////////////////////
	///// ScriptingEngine -> privat ///////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	void ScriptingEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("RynexJITRuntime");
		RY_CORE_ASSERT(rootDomain, "Sripting Engin Faild");

		// Store the root domain pointer
		s_Data->RootDomain = rootDomain;
		
	}

	void ScriptingEngine::ShutdownMono()
	{
		
		mono_domain_set(mono_get_root_domain(), false);


		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
		//RY_CORE_ASSERT(false, "ScriptingEngine::ShutdownMono");
		
	}

	void ScriptingEngine::ExecuteScriptClass()
	{
		//s_Data->EntityClass = ScriptClass("Rynex", "Entity");
		s_Data->EntityClass = ScriptClass("Rynex", "Entity");
		MonoObject* instance = s_Data->EntityClass.Instantiate();

		// Funktion in C# defined executed in C++
		{
			MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethode("PrintMessage");
			s_Data->EntityClass.InvokeMethode(printMessageFunc, instance);
		}

		{
			MonoMethod* printIntFunc = s_Data->EntityClass.GetMethode("PrintCustemInt", 1);
			int value = 5;
			void* param = &value;
			s_Data->EntityClass.InvokeMethode(printIntFunc, instance, &param);
		}

		{
			MonoString* monoString = mono_string_new(s_Data->AppDomain, "I am C++!");
			MonoMethod* printCustemMessageFunc = s_Data->EntityClass.GetMethode("PrintCustemMessage", 1);
			void* stringParam = monoString;
			s_Data->EntityClass.InvokeMethode(printCustemMessageFunc, instance, &stringParam);
		}
	}

	MonoObject* ScriptingEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptingEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

	
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Rynex", "Entity");
		
		
		s_Data->ClassList = new std::string[numTypes];
		s_Data->ListIndex = 0;
		
		

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);
			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);
			if (!isEntity)
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, name);
			
			
			s_Data->ClassList[s_Data->ListIndex] = fullName;
			s_Data->ListIndex++;

			s_Data->EntityClasses[fullName] = scriptClass;
			RY_CORE_TRACE("{}.{}", nameSpace, name);
			
		}
		s_Data->ClassList[0] = std::string("None");
		
	}

	MonoImage* ScriptingEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}


	
	///////////////////////////////////////////////////////////////////////
	///// ScriptClass /////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace)
		, m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, m_ClassNamespace.c_str(), m_ClassName.c_str());
	}
	
	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptingEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethode(const std::string& name, int prameCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), prameCount);
	}

	MonoObject* ScriptClass::InvokeMethode(MonoMethod* methode, MonoObject* instance, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(methode, instance, params, &exception);
	}



	///////////////////////////////////////////////////////////////////////
	///// ScriptInstance //////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.GetMethode(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethode("OnCreate");
		m_OnUpdateMethod = scriptClass->GetMethode("OnUpdate", 1);
		m_OnDrawMethod = scriptClass->GetMethode("OnDraw");

		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethode(m_Constructor, m_Instance, &param);
		}
	}

	void ScriptInstance::InvokeCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethode(m_OnCreateMethod, m_Instance);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		void* param = &ts;
		m_ScriptClass->InvokeMethode(m_OnUpdateMethod, m_Instance, &param);
	}

	void ScriptInstance::InvokeOnDrawn()
	{
		if (m_OnDrawMethod)
			m_ScriptClass->InvokeMethode(m_OnDrawMethod, m_Instance);
	}
}