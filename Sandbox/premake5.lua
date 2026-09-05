project "Sandbox"

	if BuildProjectConf == "Static" or BuildProjectConf == "Static2Lib" then
		-- staticruntime "off" -- orig
		staticruntime "on" 
		io.write("Sandbox.Conf::on\n")
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "StaticLib" then
		staticruntime "off" -- orig
		io.write("Sandbox.Conf::off\n")
	end
	if BuildProjectConf == "Dynamic" then
	 	staticruntime "off"
		io.write("Sandbox.Conf::off\n")
	end
	 
	kind "ConsoleApp"
    language "C++"
	cppdialect "C++17"
	-- toolset = Compiler





	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	debugdir  ("%{cfg.targetdir}")
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	

	includedirs
	{
		"%{wks.location}/Rynex/vendor/spdlog/include",
		"%{wks.location}/Rynex/src",
		"%{wks.location}/Rynex/vendor",
		"%{IncludeDir.magic_enum}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.robin_hood_hashing}"  -- has map

	}

	defines
	{
		"RY_SANDBOX",
		"BUILD_SYSTEM_PREMAKE"
	}
	
	links
	{
		"Rynex"
	}

	filter "configurations:Debug or configurations:Release"
		postbuildcommands 
		{
			"{MKDIR} %{cfg.targetdir}/DLLs",
			"{MKDIR} %{cfg.targetdir}/../Rynex-Editor",

			'cmd /c if not exist "%{cfg.targetdir}/../Rynex-Editor/Editor-Assets" mklink /J "%{cfg.targetdir}/../Rynex-Editor/Editor-Assets" "%{wks.location}Rynex-Editor/Editor-Assets"',
			'cmd /c if not exist "%{cfg.targetdir}/../Rynex-Editor/Resources" mklink /J "%{cfg.targetdir}/../Rynex-Editor/Resources" "%{wks.location}Rynex-Editor/Resources"',
			'cmd /c if not exist "%{cfg.targetdir}/../Rynex-Editor/SandboxProject" mklink /J "%{cfg.targetdir}/../Rynex-Editor/SandboxProject" "%{wks.location}Rynex-Editor/SandboxProject"',
			'cmd /c if not exist "%{cfg.targetdir}/../Rynex-Editor/Profile" mklink /J "%{cfg.targetdir}/../Rynex-Editor/Profile" "%{wks.location}Rynex-Editor/Profile"',
			'cmd /c if not exist "%{cfg.targetdir}/../Rynex-Editor/mono" mklink /J "%{cfg.targetdir}/../Rynex-Editor/mono" "%{wks.location}Rynex-Editor/mono"',
		}
	filter {}

	filter "configurations:Dist"
		postbuildcommands 
		{
			"{MKDIR} %{cfg.targetdir}/DLLs",
			

			"{COPY} %{cfg.targetdir}../Editor-Assets %{wks.location}/Rynex-Editor/Editor-Assets",
			"{COPY} %{cfg.targetdir}../Resources %{wks.location}/Rynex-Editor/Resources",
			"{COPY} %{cfg.targetdir}../SandboxProject %{wks.location}/Rynex-Editor/SandboxProject",
			"{COPY} %{cfg.targetdir}../Profile %{wks.location}/Rynex-Editor/Profile",
			"{COPY} %{cfg.targetdir}../mono %{wks.location}/Rynex-Editor/mono",
		}
	filter {}

	filter "system:windows"
		systemversion "latest"
		defines
		{
		}
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



