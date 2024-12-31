local RynexRootDir = '../../../..'
include (RynexRootDir .. "/vendor/bin/premake/premake_customization/solution_items.lua")
workspace "Sanbox"
	architecture "x86_64"
	startproject "Sanbox"

	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

	--solution_items
	--{
	--	".editorconfig"
	--}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Sanbox"
	kind "SharedLib"
    language "C#"
	dotnetframework "4.7.2"
	clr "unsafe"
	targetdir ("Binaries")
	objdir ("Intermediates")

	files
	{
		"Source/**.cs",
		--"Assets/Scripts/**.cs",
		--"Assets/**.cs",
		"Properties/**.cs"
	}

	links
	{
		"Rynex-ScriptingCore"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
		

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Default"

group "Rynex"
	include (RynexRootDir .. "/Rynex-ScriptingCore")
group ""