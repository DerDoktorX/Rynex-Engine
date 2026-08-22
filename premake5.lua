include "./vendor/bin/premake/premake_customization/solution_items.lua"

workspace "Rynex-Rendering"
	architecture "x86_64"
	startproject "Rynex-Editor"

	BuildProjectConf = "Static2"
	libraryWarnig = "on"

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


	multiprocessorcompile "on"

	filter "not toolset:msc*"
     	buildoptions { "-finput-charset=UTF-8" }
	 	print('utf-8 set compile NOT MSVC!')
	 	
	 filter "toolset:msc*"
     	buildoptions { "/utf-8" }
	 	print('utf-8 set compile MSVC!')
		

	-- conformancemode "Off" -- disable in vs2026 -> /permissive-
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- filter "system:windows"
-- 	defines
-- 	{
-- 		"RY_PLATFORM_WINDOWS"
-- 	}
-- filter "system:linux"	
-- 	defines
-- 	{
-- 		"RY_PLATFORM_LINUX"
-- 	}
defines
{
	"RY_PLATFORM_WINDOWS"
}
if BuildProjectConf == "Dynamic" then
defines
{
	"RY_DYNAMIC_LINK"
}
end

IncludeDir = {}
-- Runtime
IncludeDir["entt"] 		= "%{wks.location}/Rynex/vendor/entt_single_header/include"
IncludeDir["robin_hood_hashing"] 		= "%{wks.location}/Rynex/vendor/robin-hood-hashing/include"

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
IncludeDir["meshoptimizer"] = "%{wks.location}/Rynex/vendor/meshoptimizer/src"

IncludeDir["magic_enum"] = "%{wks.location}/Rynex/vendor/magic_enum"
-- Runtime Visuelle configs
IncludeDir["ImGui"] 	= "%{wks.location}/Rynex/vendor/imgui"

IncludeDir["ImGuizmo"] 	= "%{wks.location}/Rynex/vendor/ImGuizmo"

IncludeDir["msdfgen"] 	= "%{wks.location}/Rynex/vendor/msdf-atelas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] 	= "%{wks.location}/Rynex/vendor/msdf-atelas-gen/msdf-atlas-gen"
IncludeDir["freetype"] 	= "%{wks.location}/Rynex/vendor/msdf-atelas-gen/msdfgen/freetype/include"
-- Test
IncludeDir["gtest"] = "%{wks.location}/Rynex-Test/vendor/gtest/googletest/include"

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


Compiler ="msv" -- "msv" | "gcc" | "clang" | "dotnet"

group "Dependencies"
	include "Rynex/vendor/GLFW"
	include "Rynex/vendor/Glad"
	include "Rynex/vendor/yaml-cpp"
	include "Rynex/vendor/imgui"
	

	include "Rynex/vendor/msdf-atelas-gen"
	include "Rynex/vendor/meshoptimizer"
group ""

include "Rynex"
include "Rynex-Editor"
include "Sandbox"


group "Core"	
	include "Rynex-ScriptingCore"
group ""

			
