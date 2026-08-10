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
	toolset = Compiler





	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	
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
		"%{IncludeDir.robin_hood_hashing}", -- has map

	}

	defines
	{
		"RY_SANDBOX"
	}
	
	links
	{
		"Rynex"
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
		}

	filter "system:linux"
		systemversion "latest"
		
	
	filter "configurations:Debug"
		defines "RY_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RY_REALSE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RY_DIST"
		runtime "Release"
		optimize "on"



