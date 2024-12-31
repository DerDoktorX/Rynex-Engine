#pragma once

// For Globle Definse in Rynex

// For def from Rynex only
#define RY_DEV 1
#define RY_DISABLE_CODE_TODO 1	// Like Kommenting out

#define RY_CONSOLE 1
#define RY_CONSOLE_ALICATION_OFF_MEMORY 0
#define RY_CONSOLE_MOUSE_POSTION 0
#define RY_CONSOLE_WINDOW_SIZE 0
#define RY_CONSOLE_WINDOW_POSTION 0

#if RY_DEV

	#define RY_WINDOW_PROZENT_SIZE 0.4
	#define RY_IMGUI_DEMO_WINDOW 0
	#define RY_ENABLE_ASSERTS 1
	#define RY_ENABLE_ASSERTS_MSG 1
	#define RY_PROFILE 0
	#define RY_KONSOLE_FPS 0
	#define RY_ASSETMANGER_FILE_WATCHER 1
	#define RY_CONSOLE_LOG_FUNKTION_OPENGL 0
	#define RY_ANABLE_RENDERER_ICONE 0
	#define RY_SCRIPTING_HAZEL 0
	#define RY_DISABLE_WEAK_PTR 0
	#define RY_ENABLE_WEBGPU_PILINE 0
	#define RY_ENABLE_FONT 1
	#define RY_EDITOR_ASSETMANGER_THREADE 1		// Debgu -> 0: time ~= 5s  | 1: time ~= 36s 
	#define RY_STD_PTR_IMPL 1
	#define RY_INSTECING 0
	#define RY_ENABLE_GRIFIC_API_ERROR_MASGES 1 // Callback Funktion
	#define RY_ENABLE_GRIFIC_API_WARN_MASGES 0 // Callback Funktion
	
	#define RY_ENABLE_DEFAULT_PROJECT 1
	#define RY_DEFAULT_PROJECT_PATH "D:\\dev\\Rynex-Test-Projects\\Test-Project-System\\Test-Project-System.rproj"
	// 


	#if RY_DISABLE_CODE_TODO

		#define RY_TODO_APPLICATION_REMABER 1
		#define RY_TODO_APPLICATION_MULTI_THREAD 1
		#define RY_MESHSTRIPS 1
		#define RY_MESH 0
		#define RY_SCRIPT_PATH 0
		#define RY_PATH_IN_LINE 1
		#define RY_NEW_ASSET_REGISTER_SYSTEM 0
		#define RY_MAX_MAIN_THREAD_QUEUE_PER_FRAME 1

	#endif

#endif


