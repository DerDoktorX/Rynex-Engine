project "ImGui"

	if BuildProjectConf == "Static" or BuildProjectConf == "StaticLib" then
		-- staticruntime "on" -- orig
		staticruntime "on" 
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "Static2Lib"   then
		staticruntime "on" -- orig
	end
	if BuildProjectConf == "Dynamic" then
		 staticruntime "off"
	end
	kind "StaticLib"
	language "C++"
	

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp"
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
