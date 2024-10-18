#pragma once

// For Globle Definse in Rynex

// For def from Rynex only
#define RY_DEV 1
#define RY_DISABLE_CODE_TODO 1	// Like Kommenting out
#define RY_CONSOLE 1
#define RY_CONSOLE_ALICATION_OFF_MEMORY 0
#if RY_DEV
	#define RY_WINDOW_PROZENT_SIZE 0.7
	#define RY_IMGUI_DEMO_WINDOW 0
	#define RY_ENABLE_ASSERTS 1
	#define RY_PROFILE 0
	#define RY_KONSOLE_FPS 0
	#define RY_ASSETMANGER_FILE_WATCHER 1
	#define RY_CONSOLE_LOG_FUNKTION_OPENGL 0
	#define RY_ANABLE_RENDERER_ICONE 0

	#if RY_DISABLE_CODE_TODO
		#define RY_TODO_APPLICATION_REMABER 1
		#define RY_TODO_APPLICATION_MULTI_THREAD 0
		#define RY_MESHSTRIPS 1
		#define RY_MESH 0
		#define RY_SCRIPT_PATH 1
		#define RY_PATH_IN_LINE 1
		#define RY_NEW_ASSET_REGISTER_SYSTEM 0
	#endif
#endif


