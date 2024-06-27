include "./vendor/bin/premake/premake_customization/solution_items.lua"

workspace "Rynex-Engine"
	architecture "x86_64"
	startproject "Rynex-Editor"

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
IncludeDir["GLFW"] 		= "%{wks.location}/Rynex/vendor/GLFW/include"
IncludeDir["Glad"] 		= "%{wks.location}/Rynex/vendor/Glad/include"
IncludeDir["ImGui"] 	= "%{wks.location}/Rynex/vendor/imgui"
IncludeDir["glm"] 		= "%{wks.location}/Rynex/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Rynex/vendor/stb_image"
IncludeDir["entt"] 		= "%{wks.location}/Rynex/vendor/entt/include"
IncludeDir["yaml_cpp"] 	= "%{wks.location}/Rynex/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] 	= "%{wks.location}/Rynex/vendor/ImGuizmo"

group "Dependencies"
	include "vendor/bin/premake"
	include "Rynex/vendor/GLFW"
	include "Rynex/vendor/Glad"
	include "Rynex/vendor/imgui"
	include "Rynex/vendor/yaml-cpp"
	
group ""

--group "MainWorking"
--group "Core"
	include "Rynex"
--group ""


--group "Tools"
	include "Rynex-Editor"
--group ""
--group ""

group "Misc"
	include "Sandbox"
group ""



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
			
