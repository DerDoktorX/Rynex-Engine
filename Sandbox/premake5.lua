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

	links
	{
		"Rynex"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			--"RY_PLATFORM_WINDOWS",
			--"RY_BUILD_DLL",
			--"GLFW_INCLUDE_NONE"
		}

		--postbuildcommands
		--{
		--	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		--}
	
	filter "configurations:Debug"
		defines "RY_DEBUG"
		--buildoptions "/MDd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RY_REALSE"
		--buildoptions "/MD"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RY_DIST"
		--buildoptions "/MD"
		runtime "Release"
		optimize "on"

