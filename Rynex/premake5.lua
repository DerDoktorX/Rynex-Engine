project "Rynex"
    --location "Rynex"
    --kind "SharedLib"
	kind "StaticLib"
    language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "rypch.h"
	pchsource "src/rypch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		-- vendor add Source Files to Rynex
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
		--"vendor/filewatch/**.h",
		--"vendor/filewatch/**.cpp",
		
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

	includedirs
	{
		-- Source Files Directory
		"src",
		-- Runtime
		"vendor/spdlog/include",	-- Logs

		"%{IncludeDir.entt}",		-- Entity
		"%{IncludeDir.mono}",		-- C#
		-- Math
		"%{IncludeDir.glm}",
		-- Grafic API
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		-- Files
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.assimp}",
		 --"%{IncludeDir.assimp}",
			--"%{IncludeDir._config}",
			--"%{IncludeDir.assimp_config}",
			--"%{IncludeDir.assimp_include}",
		-- Runtime Visuelle configs
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
	}

	links
	{
		-- Grafic API
		"Glad",
		"GLFW",
		"opengl32.lib",	

		"yaml-cpp",			-- Files
		"ImGui", 			-- Runtime Visuelle configs
		--"%{Library.assimp}"	-- Runtime C# Scripts Reloade
	}
	
	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		defines
		{
		-- For Compile Rynex in .dll |
		--							 V
			--"RY_PLATFORM_WINDOWS",
			--"RY_BUILD_DLL",
			--"GLFW_INCLUDE_NONE"
		--							 ^
		}
		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.Bcrypt}",
		}
	-- For Compile Rynex in .dll |
	--							 V
		--postbuildcommands
		--{
		--	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		--	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Rynex-Editor")
		--}
	--							 ^
	
	filter "configurations:Debug"
		defines "RY_DEBUG"
	-- For Compile Rynex in .dll |
	--							 V
		--buildoptions "/MDd"
	--							 ^
		runtime "Debug"
		symbols "on"
		links
		{
		--	"%{Library.ShaderC_Debug}",
		--	"%{Library.SPIRV_Cross_Debug}",
		--	"%{Library.SPIRV_Cross_GLSL_Debug}"
			"%{Library.mono_Debug}",
			"%{Library.assimp_Debug}"
		}
		--editandcontinue "Off"
		--buildoptions 
		--{ 
		--	"/Zi", "/fsanitize=address"
		--}
      	--linkoptions 
		--{ 
		--	"/fsanitize=address" 
		--}

	filter "configurations:Release"
		defines "RY_REALSE"
	-- For Compile Rynex in .dll |
	--							 V	
		--buildoptions "/MD"
	--							 ^
		runtime "Release"
		optimize "on"
		links
		{
		--	"%{Library.ShaderC_Release}",
		--	"%{Library.SPIRV_Cross_Release}",
		--	"%{Library.SPIRV_Cross_GLSL_Release}"
			"%{Library.mono_Release}",
			"%{Library.assimp_Release}"
		}

	filter "configurations:Dist"
		defines "RY_DIST"
	-- For Compile Rynex in .dll |
	--							 V
		--buildoptions "/MD"
	--							 ^
		runtime "Release"
		optimize "on"
		links
		{
			--"%{Library.ShaderC_Release}",
			--"%{Library.SPIRV_Cross_Release}",
			--"%{Library.SPIRV_Cross_GLSL_Release}"
			"%{Library.mono_Release}",
			"%{Library.assimp_Release}"
		}

