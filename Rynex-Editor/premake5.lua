
project "Rynex-Editor"

	if BuildProjectConf == "Static" or BuildProjectConf == "Static2Lib"   then
		staticruntime "on" 
		io.write("Rynex-Editor.Conf::on\n")
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "StaticLib" then
		staticruntime "off" -- orig
		io.write("Rynex-Editor.Conf::off\n")
	end
	if BuildProjectConf == "Dynamic" then
	 	staticruntime "off"
		io.write("Rynex-Editor.Conf::off\n")
	end
	kind "ConsoleApp"
    language "C++"
	cppdialect "C++17"
	

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	debugdir  ("%{cfg.targetdir}")
	-- toolset = Compiler

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		-- Rynex Source Files
		"%{wks.location}/Rynex/src",	-- Rynex
		"%{wks.location}/Rynex/vendor",	-- Dependecies

		-- Runtime
		"%{IncludeDir.spdlog}",
		-- Math
		"%{IncludeDir.glm}",
		-- Filse
		"%{IncludeDir.filewatch}",
		-- Entity
		"%{IncludeDir.magic_enum}",

		"%{IncludeDir.entt}",
		"%{IncludeDir.robin_hood_hashing}", -- has map
		-- Runtime Visuelle configs
		-- "%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.ImGui}",
	}


	links
	{
		"Rynex"
	}
	


	defines
	{
		"RY_EDITOR",
		"BUILD_SYSTEM_PREMAKE"
	}
	
	filter "configurations:Debug or configurations:Release"
		postbuildcommands 
		{
			"{MKDIR} %{cfg.targetdir}/DLLs",
			"{MKDIR} %{cfg.targetdir}/Profile",

			'cmd /c if not exist "%{cfg.targetdir}/Editor-Assets" mklink /J "%{cfg.targetdir}/Editor-Assets" "%{wks.location}Rynex-Editor/Editor-Assets"',
			'cmd /c if not exist "%{cfg.targetdir}/Resources" mklink /J "%{cfg.targetdir}/Resources" "%{wks.location}Rynex-Editor/Resources"',
			'cmd /c if not exist "%{cfg.targetdir}/SandboxProject" mklink /J "%{cfg.targetdir}/SandboxProject" "%{wks.location}Rynex-Editor/SandboxProject"',
			'cmd /c if not exist "%{cfg.targetdir}/mono" mklink /J "%{cfg.targetdir}/mono" "%{wks.location}Rynex-Editor/mono"',
			'cmd /c if not exist "%{wks.location}bin\\' .. outputdir .. '\\%{prj.name}\\imgui.ini" mklink /H "%{wks.location}bin\\' .. outputdir .. '\\%{prj.name}\\imgui.ini" "%{wks.location}%{prj.name}\\imgui.ini"',
		}
	filter {}

	filter "configurations:Dist"
		postbuildcommands 
		{
			"{MKDIR} %{cfg.targetdir}/DLLs",
			"{MKDIR} %{cfg.targetdir}/Profile",

			"{COPY} %{wks.location}Rynex-Editor/Editor-Assets %{cfg.targetdir}/Editor-Assets",
			"{COPY} %{wks.location}Rynex-Editor/Resources %{cfg.targetdir}/Resources",
			"{COPY} %{wks.location}Rynex-Editor/SandboxProject %{cfg.targetdir}/SandboxProject",
			-- "{COPY} %{wks.location}Rynex-Editor/imgui.ini %{cfg.targetdir}/imgui.ini",
		}
	filter {}
	

	filter "system:windows"
		systemversion "latest"
	filter {}

	filter "system:linux"
		systemversion "latest"
		pic "On"
	filter {}
	
	filter "configurations:Debug"
		defines "RY_DEBUG"
		runtime "Debug"
		symbols "on"
	filter {}

	filter "configurations:Release"
		defines "RY_REALSE"
		runtime "Release"
		optimize "on"
		symbols "on"
	filter {}

	filter "configurations:Dist"
		defines "RY_DIST"
		runtime "Release"
		optimize "on"
	filter {}

		

