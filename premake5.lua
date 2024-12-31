include "./vendor/bin/premake/premake_customization/solution_items.lua"

workspace "Rynex-Engine"
	architecture "x86_64"
	startproject "Rynex-Editor"
	--startproject "Sandbox"

	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

-- Include directories relativ to root folder (solutione directory)
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

defines
{
	"RY_PLATFORM_WINDOWS"
}

IncludeDir = {}
-- Runtime
IncludeDir["entt"] 		= "%{wks.location}/Rynex/vendor/entt/include"
IncludeDir["mono"] 	= "%{wks.location}/Rynex/vendor/mono/include"
-- Math
IncludeDir["glm"] 		= "%{wks.location}/Rynex/vendor/glm"
-- Grafic API
IncludeDir["GLFW"] 		= "%{wks.location}/Rynex/vendor/GLFW/include"
IncludeDir["Glad"] 		= "%{wks.location}/Rynex/vendor/Glad/include"
-- Files
IncludeDir["stb_image"] = "%{wks.location}/Rynex/vendor/stb_image"
IncludeDir["yaml_cpp"] 	= "%{wks.location}/Rynex/vendor/yaml-cpp/include"
IncludeDir["filewatch"] = "%{wks.location}/Rynex/vendor/filewatch"
IncludeDir["assimp"] = "%{wks.location}/Rynex/vendor/assimp/include"

IncludeDir["magic_enum"] = "%{wks.location}/Rynex/vendor/magic_enum"
-- Runtime Visuelle configs
IncludeDir["ImGui"] 	= "%{wks.location}/Rynex/vendor/imgui"
IncludeDir["ImGuizmo"] 	= "%{wks.location}/Rynex/vendor/ImGuizmo"

IncludeDir["msdfgen"] 	= "%{wks.location}/Rynex/vendor/msdf-atelas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] 	= "%{wks.location}/Rynex/vendor/msdf-atelas-gen/msdf-atlas-gen"
IncludeDir["freetype"] 	= "%{wks.location}/Rynex/vendor/msdf-atelas-gen/msdfgen/freetype/include"
-- Test
IncludeDir["gtest"] = "%{wks.location}/Rynex/vendor/gtest/"

LibraryDir = {}
Library = {}

LibraryDir["mono"] = "%{wks.location}/Rynex/vendor/mono/lib/%{cfg.buildcfg}"
LibraryDir["mono_Release"] = "%{wks.location}/Rynex/vendor/mono/lib/Release"
LibraryDir["mono_Debug"] = "%{wks.location}/Rynex/vendor/mono/lib/Debug"

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
Library["mono_Release"] = "%{LibraryDir.mono_Release}/libmono-static-sgen.lib"
Library["mono_Debug"] = "%{LibraryDir.mono_Debug}/libmono-static-sgen.lib"



	LibraryDir["assimp"] 	= "%{wks.location}/Rynex/vendor/assimp/lib/%{cfg.buildcfg}"
	LibraryDir["assimp_Debug"] 	= "%{wks.location}/Rynex/vendor/assimp/lib/Debug"
	LibraryDir["assimp_Release"] 	= "%{wks.location}/Rynex/vendor/assimp/lib/Release"

	Library["assimp"] = "%{LibraryDir.assimp}/assimp-vc143-mtd.lib"
	Library["assimp_Debug"] = "%{LibraryDir.assimp_Debug}/assimp-vc143-mtd.lib"
	Library["assimp_Release"] = "%{LibraryDir.assimp_Release}/assimp-vc143-mt.lib"



LibraryDir["gtest"] 	= "%{wks.location}/Rynex/vendor/gtest/lib/%{cfg.buildcfg}"
LibraryDir["gtest_Debug"] 	= "%{wks.location}/Rynex/vendor/gtest/lib/Debug"
LibraryDir["gtest_Release"] 	= "%{wks.location}/Rynex/vendor/gtest/lib/Release"

Library["gtest"] = "%{LibraryDir.gtest}/gtest.lib"
Library["gtest_Debug"] = "%{LibraryDir.gtest_Debug}/gtest_main.lib"
Library["gtest_Release"] = "%{LibraryDir.gtest_Release}/gtest_main.lib"


-- Windows only
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["Bcrypt"] = "Bcrypt.lib"

group "Dependencies"
	--include "vendor/bin/premake" <- Why include in Hazel
	include "Rynex/vendor/GLFW"
	include "Rynex/vendor/Glad"
	include "Rynex/vendor/yaml-cpp"
	include "Rynex/vendor/imgui"
	include "Rynex/vendor/msdf-atelas-gen"
	--include "Rynex/vendor/assimp"
-- Not Yet Includede	|
--						V
-- include "Hazel/vendor/msdf-atlas-gen"
group ""

--group "MainWorking"
include "Rynex"
include "Rynex-Editor"
-- include "Rynex-Test"
include "Sandbox"
--group ""

group "Core"	
	include "Rynex-ScriptingCore"
group ""
--group "Tools"
--group ""


--group "Misc"
	
--group ""



--	project "Rynex"
--    location "Rynex"
--    --kind "SharedLib"
--	kind "StaticLib"
--    language "C++"
--    staticruntime "on"
--	cppdialect "C++17"
--
--	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
--
--	pchheader "rypch.h"
--	pchsource "Rynex/src/rypch.cpp"
--
--	files
--	{
--		"%{prj.name}/src/**.h",
--		"%{prj.name}/src/**.cpp",
--		"%{prj.name}/vendor/stb_image/**.h",
--		"%{prj.name}/vendor/stb_image/**.cpp",
--		"%{prj.name}/vendor/glm/glm/**.hpp",
--		"%{prj.name}/vendor/glm/glm/**.inl",
--	}
--
--	includedirs
--	{
--		"%{prj.name}/src/",
--		"%{prj.name}/src/Rynex/",
--		"%{prj.name}/vendor/spdlog/include",
--		"%{IncludeDir.GLFW}",
--		"%{IncludeDir.Glad}",
--		"%{IncludeDir.ImGui}",
--		"%{IncludeDir.glm}",
--		"%{IncludeDir.stb_image}",
--		"%{IncludeDir.entt}",
--		"%{IncludeDir.yaml_cpp}",
--		
--	}
--
--	links
--	{
--		"Glad",
--		"GLFW",
--		"ImGui",
--		"yaml-cpp",
--		"opengl32.lib",	
--	}
--
--	filter "system:windows"
--		
--		
--		systemversion "latest"
--
--		defines
--		{
--			"RY_PLATFORM_WINDOWS",
--			"RY_BUILD_DLL",
--			"GLFW_INCLUDE_NONE"
--		}
--
--		--postbuildcommands
--		--{
--		--	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
--		--}
--	
--	filter "configurations:Debug"
--		defines "RY_DEBUG"
--		--buildoptions "/MDd"
--		runtime "Debug"
--		symbols "on"
--
--	filter "configurations:Release"
--		defines "RY_REALSE"
--		--buildoptions "/MD"
--		runtime "Release"
--		optimize "on"
--
--	filter "configurations:Dist"
--		defines "RY_DIST"
--		--buildoptions "/MD"
--		runtime "Release"
--		optimize "on"
--
--	project "Sandbox"
--	location "Sandbox"
--    kind "ConsoleApp"
--    language "C++"
--	cppdialect "C++17"
--	staticruntime "on"
--
--	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
--
--	files
--	{
--		"%{prj.name}/src/**.h",
--		"%{prj.name}/src/**.cpp",
--	}
--
--	includedirs
--	{
--		"Rynex/vendor/spdlog/include",
--		"Rynex/src",
--		"Rynex/vendor",
--		"%{IncludeDir.ImGui}",
--		"%{IncludeDir.glm}",
--		"%{IncludeDir.entt}",
--		"%{IncludeDir.yaml_cpp}",
--	}
--
--	links
--	{
--		"Rynex"
--	}
--
--	filter "system:windows"
--		staticruntime "on"
--		systemversion "latest"
--
--		defines
--		{
--			"RY_PLATFORM_WINDOWS",
--		}
--
--	filter "configurations:Debug"
--		defines "RY_DEBUG"
--		--buildoptions "/MDd"
--		runtime "Debug"
--		symbols "on"
--
--	filter "configurations: Release"
--		defines "RY_REALSE"
--		--buildoptions "/MD"
--		runtime "Release"
--		symbols "on"
--
--	filter "configurations:Dist"
--		defines "RY_DIST"
--		--buildoptions "/MD"
--		runtime "Release"
--		symbols "on"
--		
--	project "Rynex-Editor"
--	location "Rynex-Editor"
--		kind "ConsoleApp"
--		language "C++"
--		cppdialect "C++17"
--		staticruntime "on"
--		
--	
--		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
--	
--		files
--		{
--			"%{prj.name}/src/**.h",
--			"%{prj.name}/src/**.cpp",
--		}
--	
--		includedirs
--		{
--			"Rynex/vendor/spdlog/include",
--			"Rynex/src",
--			"Rynex/vendor",
--			"%{IncludeDir.ImGui}",
--			"%{IncludeDir.glm}",
--			"%{IncludeDir.entt}",
--		}
--		links
--		{
--			"Rynex"
--		}
--	
--		filter "system:windows"
--			staticruntime "on"
--			systemversion "latest"
--	
--			defines
--			{
--				"RY_PLATFORM_WINDOWS",
--			}
--	
--		filter "configurations:Debug"
--			defines "RY_DEBUG"
--			--buildoptions "/MDd"
--			runtime "Debug"
--			symbols "on"
--	
--		filter "configurations: Release"
--			defines "RY_REALSE"
--			--buildoptions "/MD"
--			runtime "Release"
--			symbols "on"
--	
--		filter "configurations:Dist"
--			defines "RY_DIST"
--			--buildoptions "/MD"
--			runtime "Release"
--			symbols "on"
			
