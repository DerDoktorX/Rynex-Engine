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
	cppdialect "C++17"


	local projectDir = "_imgui/"
	location "_imgui"

	targetdir (projectDir .. "/bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		projectDir .. "/imconfig.h",
		projectDir .. "/imgui.h",
		projectDir .. "/imgui.cpp",
		projectDir .. "/imgui_draw.cpp",
		projectDir .. "/imgui_internal.h",
		projectDir .. "/imgui_tables.cpp",
		projectDir .. "/imgui_widgets.cpp",
		projectDir .. "/imstb_rectpack.h",
		projectDir .. "/imstb_textedit.h",
		projectDir .. "/imstb_truetype.h",
		projectDir .. "/imgui_demo.cpp"
	}

	includedirs
	{
		projectDir .. ""
	}



	filter "system:windows"
		systemversion "latest"
	filter {}

	filter "system:linux"
		pic "On"
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
