project "yaml-cpp"
	kind "StaticLib"
	language "C++"

	local projectDir = "_yaml-cpp/"
	location "_yaml-cpp"

	targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		projectDir .. "src/**.h",
		projectDir .. "src/**.cpp",
		
		projectDir .. "include/**.h"
	}

	includedirs
	{
		projectDir .. "include"
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"
	filter {}
	
	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"
	filter {}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
	filter {}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	filter {}
