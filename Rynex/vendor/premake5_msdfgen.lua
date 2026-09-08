include "premake5_freetype.lua"

project "msdf-atlas-gen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "off"
	

	local projectDir = "_msdf-atelas-gen/"
	location "_msdf-atelas-gen/"
	-- local projectDir = ""

	targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		projectDir .. "msdf-atlas-gen/**.h",
    	projectDir .. "msdf-atlas-gen/**.hpp",
    	projectDir .. "msdf-atlas-gen/**.cpp"
	}

	includedirs
	{
		projectDir .. "msdf-atlas-gen",
		projectDir .. "msdfgen",
		projectDir .. "msdfgen/include"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	links
	{
		"msdfgen"
	}

	filter "system:windows"
		systemversion "latest"
	filter {}
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
	filter {}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	filter {}

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
	filter {}