#pragma once

#ifdef RY_PLATFORM_WINDOWS

	extern Rynex::Application* Rynex::CreateApplication(ApplicationCommandLineArgs spec);

	int main(int argc, char** argv)
	{

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
		
		
		
	}

#endif // RY_PLATFORM_WINDOWS
