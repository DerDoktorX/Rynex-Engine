#pragma once

#ifdef RY_PLATFORM_WINDOWS

	extern Rynex::Application* Rynex::CreateApplication(ApplicationCommandLineArgs spec);




	int main(int argc, char** argv)
	{
		printf("Rynex Engin\n");
		Rynex::Log::Get().Init();
		Rynex::Log::Get();
		
		RY_PROFILE_BEGIN_SESSION("Startup", "Profile/RynexPrifile-Startup.json");
		RY_CORE_INFO("Initlatione Log!");
		RY_INFO("Initlatione Log!");
#ifdef RY_ENABLE_DEFAULT_PROJECT
		if (1==argc)
		{
			argv[1] = RY_DEFAULT_PATH_PROJECT_FILE;
			argc = 2;
		}
		Rynex::Application* app = Rynex::CreateApplication({ argc, argv });
#else
		Rynex::Application* app = Rynex::CreateApplication({ argc, argv });
#endif
		

		RY_PROFILE_END_SESSION();

		RY_PROFILE_BEGIN_SESSION("Runtime", "Profile/RynexPrifile-Runtime.json");
		app->Run();
		RY_PROFILE_END_SESSION();

		RY_PROFILE_BEGIN_SESSION("Shutdown", "Profile/RynexPrifile-Shutdown.json");
		delete app;
		RY_PROFILE_END_SESSION();
		Rynex::Log::Get().Shutdown();
	}

#endif // RY_PLATFORM_WINDOWS
