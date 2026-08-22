project "Rynex"

	if BuildProjectConf == "Static" or BuildProjectConf == "Static2Lib"  then
	   kind "StaticLib"
	   staticruntime "on" 
	   io.write("Rynex.Conf:StaticLib::on\n")
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "StaticLib" then
		kind "StaticLib"
		staticruntime "off" -- orig
		io.write("Rynex.Conf:StaticLib::off\n")
	end
	if BuildProjectConf == "Dynamic" then
	 	kind "SharedLib"
		staticruntime "off"
		io.write("Rynex.Conf:SharedLib::off\n")
	end
	
    language "C++"
	cppdialect "C++17"
	

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "rypch.h"
	pchsource "src/rypch.cpp"
	toolset = Compiler
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

		"vendor/magic_enum/**.hpp",
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
		"%{IncludeDir.robin_hood_hashing}", -- has map
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
		"%{IncludeDir.magic_enum}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.meshoptimizer}",
		-- Runtime Visuelle configs
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}"
	}


	print('Other Builds then Windows don\'t build use [Mono, Assimp] Curently!')

	links
	{
		-- Grafic API
		"Glad",
		"GLFW",
		"meshoptimizer",
		"opengl32.lib",	

		"yaml-cpp",			-- Files
		"ImGui", 			-- Runtime Visuelle configs
		"ImPolt",

		"msdf-atlas-gen"
	}


	filter "files:vendor/ImGuizmo/**.cpp"
		enablepch "off"

	filter "files:vendor/impolt/*.cpp"
		enablepch "off"

	filter "system:windows"
		systemversion "latest"
		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.Bcrypt}",
		}
	-- For Compile Rynex in .dll |
	--							 V
		if BuildProjectConf == "Dynamic" then
			defines
			{
				"RY_BUILD_DLL",
			}
			postbuildcommands
			{
				("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
				("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Rynex-Editor")
			}
		end
	--							 ^
	filter "configurations:Debug"
		defines "RY_DEBUG"
		runtime "Debug"

	filter {"system:windows", "configurations:Debug"}
		links
		{
			"%{Library.mono_Debug}",
			"%{Library.assimp_Debug}"
		}

	filter "configurations:Release"
		defines "RY_REALSE"
		runtime "Release"
		symbols "on"
		optimize "on"	

	filter {"system:windows", "configurations:Release"}
		links
		{
			"%{Library.mono_Release}",
			"%{Library.assimp_Release}"
		}
		

	filter "configurations:Dist"
		defines "RY_DIST"
		runtime "Release"
		optimize "on"

	filter {"system:windows", "configurations:Dist"}
		links
		{
			"%{Library.mono_Release}",
			"%{Library.assimp_Release}"
		}

	

	

