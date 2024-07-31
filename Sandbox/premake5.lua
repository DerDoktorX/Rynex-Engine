project "Sandbox"
    --location "Rynex"
    --kind "SharedLib"
	kind "ConsoleApp"
    language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
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

