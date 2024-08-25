#pragma once

#ifdef RY_PLATFORM_WINDOWS

	extern Rynex::Application* Rynex::CreateApplication(ApplicationCommandLineArgs spec);
#if 0
	struct AlicationMetrics
	{
		uint64_t TotalAliction = 0;
		uint64_t TotalFree = 0;
		bool MainEnde = false;
		bool MainStart = false;
		uint64_t CurrentUsage() const { return TotalAliction - TotalFree; }
	};

	static AlicationMetrics s_AlicationMetrics;

	static void PrintMemoryUsage()
	{
		printf("Memory Usage: { \n\t\tfree:  %i bytes, \n\t\talloc: %i bytes,\n\t\tcurent: %i bytes\n}\n", s_AlicationMetrics.TotalFree, s_AlicationMetrics.TotalAliction, s_AlicationMetrics.CurrentUsage());
		
	}

	void* operator new(size_t size)
	{
		if (s_AlicationMetrics.MainStart)
			s_AlicationMetrics.TotalAliction += size;
		return malloc(size);
	}

	void operator delete(void* memory, size_t size)
	{
		if (s_AlicationMetrics.MainStart)
			s_AlicationMetrics.TotalFree += size;
		if (s_AlicationMetrics.MainEnde)
			PrintMemoryUsage();
		return free(memory);
	}

#endif


	


	int main(int argc, char** argv)
	{
		//s_AlicationMetrics.MainStart = true;
		printf("Rynex Engin\n");
		
		Rynex::Log::Init();
		RY_PROFILE_BEGIN_SESSION("Startup", "Profile/RynexPrifile-Startup.json");

		RY_CORE_INFO("Initlatione Log!");
		
		auto app = Rynex::CreateApplication({ argc, argv });
		RY_INFO("Initlatione Log!");

		RY_PROFILE_END_SESSION();

		RY_PROFILE_BEGIN_SESSION("Runtime", "Profile/RynexPrifile-Runtime.json");
		app->Run();
		RY_PROFILE_END_SESSION();


		RY_PROFILE_BEGIN_SESSION("Shutdown", "Profile/RynexPrifile-Shutdown.json");
		delete app;
		RY_PROFILE_END_SESSION();
		//s_AlicationMetrics.MainEnde = true;
		//PrintMemoryUsage();
	}

#endif // RY_PLATFORM_WINDOWS
