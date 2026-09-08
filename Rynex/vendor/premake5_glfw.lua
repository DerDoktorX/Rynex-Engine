project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	warnings "off"


    local projectDir = "_GLFW/"
	location "_GLFW/"
	
	targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

	
	files
	{
		projectDir .. "include/GLFW/glfw3.h",
		projectDir .. "include/GLFW/glfw3native.h",
		projectDir .. "src/context.c",
		projectDir .. "src/init.c",
		projectDir .. "src/input.c",
		projectDir .. "src/monitor.c",

		projectDir .. "src/null_init.c",
		projectDir .. "src/null_joystick.c",
		projectDir .. "src/null_monitor.c",
		projectDir .. "src/null_window.c",

		projectDir .. "src/platform.c",
		projectDir .. "src/vulkan.c",
		projectDir .. "src/window.c",
	}

	filter "system:linux"
		pic "On"

		systemversion "latest"
		
		files
		{
			projectDir .. "src/x11_init.c",
			projectDir .. "src/x11_monitor.c",
			projectDir .. "src/x11_window.c",
			projectDir .. "src/xkb_unicode.c",
			projectDir .. "src/posix_module.c",
			projectDir .. "src/posix_time.c",
			projectDir .. "src/posix_thread.c",
			projectDir .. "src/posix_module.c",
			projectDir .. "src/glx_context.c",
			projectDir .. "src/egl_context.c",
			projectDir .. "src/osmesa_context.c",
			projectDir .. "src/linux_joystick.c"
		}

		defines
		{
			"_GLFW_X11"
		}
	filter {}

	filter "system:macosx"
		pic "On"

		files
		{
			projectDir .. "src/cocoa_init.m",
			projectDir .. "src/cocoa_monitor.m",
			projectDir .. "src/cocoa_window.m",
			projectDir .. "src/cocoa_joystick.m",
			projectDir .. "src/cocoa_time.c",
			projectDir .. "src/nsgl_context.m",
			projectDir .. "src/posix_thread.c",
			projectDir .. "src/posix_module.c",
			projectDir .. "src/osmesa_context.c",
			projectDir .. "src/egl_context.c"
		}

		defines
		{
			"_GLFW_COCOA"
		}
	filter {}

	filter "system:windows"
		systemversion "latest"

		files
		{
			projectDir .. "src/win32_init.c",
			projectDir .. "src/win32_joystick.c",
			projectDir .. "src/win32_module.c",
			projectDir .. "src/win32_monitor.c",
			projectDir .. "src/win32_time.c",
			projectDir .. "src/win32_thread.c",
			projectDir .. "src/win32_window.c",
			projectDir .. "src/wgl_context.c",
			projectDir .. "src/egl_context.c",
			projectDir .. "src/osmesa_context.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
	filter {}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter { "system:windows", "configurations:Debug-AS" }	
		runtime "Debug"
		symbols "on"
		sanitize { "Address" }
		runtimechecks "Off"
		incrementallink "Off"
	filter {}


	filter "configurations:Release"
		runtime "Release"
		optimize "speed"
	filter {}

    filter "configurations:Dist"
		runtime "Release"
		optimize "speed"
        symbols "off"
	filter {}