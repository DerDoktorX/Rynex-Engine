#pragma once

#ifdef RY_PLATFORM_WINDOWS

	extern Rynex::Application* Rynex::CreateApplication();

	int main(int argc, char** argv)
	{

		printf("Rynex Engin\n");
		Rynex::Log::Init();
		RY_CORE_INFO("Initlatione Log!");
		RY_WARN("Initlatione Log!");
		
			auto app = Rynex::CreateApplication();
			app->Run();
			delete app;
			
		
		
		
	}

#endif // RY_PLATFORM_WINDOWS
