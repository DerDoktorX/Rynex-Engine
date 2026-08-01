project "ImPolt"
	kind "StaticLib"
	language "C++"
    staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

if libraryWarnig == "off" then
	warnings "off"
end

	files
	{
		"implot.h",
		"implot.cpp",

		"implot_internal.h",
		"implot_items.cpp",

		"implot_demo.cpp"
	}
	
	includedirs
	{
		"%{IncludeDir.ImGui}"
	}

	links 
	{
		"ImGui"
	}
	

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
