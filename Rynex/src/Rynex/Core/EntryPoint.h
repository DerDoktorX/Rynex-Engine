#pragma once

#ifdef RY_PLATFORM_WINDOWS

	extern Rynex::Application* Rynex::CreateApplication(ApplicationCommandLineArgs spec);

#if 0
	

	static uint64_t s_Free = 0;
	static uint64_t s_Allocated = 0;
	static uint64_t s_Remains = 0;
	static uint64_t s_RemainsSize = 0;
	static std::mutex s_MemoryMutex;

	static void PrintMemoryUsage()
	{
		std::scoped_lock<std::mutex> lock(s_MemoryMutex);
		printf("We have in totale: \n\t\033[35mAllections = %llu,\n\t\033[34mFree = %llu,\n\t\033[32mRemains Allections = %llu\n\t\033[33mRemains Allections Size = %llu\n\033[0m", s_Allocated, s_Free, s_Remains, s_RemainsSize);
	}

	void* operator new(size_t size)
	{
		std::scoped_lock<std::mutex> lock(s_MemoryMutex);
		s_Allocated++;
		s_Remains++;
		s_RemainsSize += size;
		return std::malloc(size);
	}

	void operator delete(void* ptr, size_t size) noexcept
	{
		std::scoped_lock<std::mutex> lock(s_MemoryMutex);
		s_Free++;
		s_Remains--;
		s_RemainsSize -= size;
		std::free(ptr);
	}

#endif

	int main(int argc, char** argv)
	{
		printf("Rynex Engin\n");
		
		Rynex::Log::Init();
		
		RY_PROFILE_BEGIN_SESSION("Startup", "Profile/RynexPrifile-Startup.json");
		RY_CORE_INFO("Initlatione Log!");
#if RY_ENABLE_DEFAULT_PROJECT
		if (1==argc)
		{
			argv[1] = RY_DEFAULT_PROJECT_PATH;
			argc = 2;
		}
		Rynex::Application* app = Rynex::CreateApplication({ argc, argv });
#else
		Rynex::Application* app = Rynex::CreateApplication({ argc, argv });
#endif
		RY_INFO("Initlatione Log!");

		RY_PROFILE_END_SESSION();

		RY_PROFILE_BEGIN_SESSION("Runtime", "Profile/RynexPrifile-Runtime.json");
		app->Run();
		RY_PROFILE_END_SESSION();

		RY_PROFILE_BEGIN_SESSION("Shutdown", "Profile/RynexPrifile-Shutdown.json");
		delete app;
		RY_PROFILE_END_SESSION();
	}

#endif // RY_PLATFORM_WINDOWS
