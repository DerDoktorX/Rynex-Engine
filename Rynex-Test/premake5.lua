project "Rynex-Test"
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
		"src",
		-- Rynex Source Files
		"%{wks.location}/Rynex/src",	-- Rynex
		"%{wks.location}/Rynex/vendor",	-- Dependecies
		"%{wks.location}/Rynex-Test/vendor",

		-- Runtime
		"%{IncludeDir.gtest}",
		-- Math
		"%{IncludeDir.glm}",
		-- Filse
		"%{IncludeDir.filewatch}",
		-- Entity
		-- "%{IncludeDir.assimp}",
		"%{IncludeDir.magic_enum}",

		"%{IncludeDir.entt}",
		-- Runtime Visuelle configs
		"%{IncludeDir.ImGuizmo}",
		"%{wks.location}/Rynex/vendor/spdlog/include",
		-- "%{IncludeDir.msdfgen}",
		-- "%{IncludeDir.msdf_atlas_gen}",
		-- "%{IncludeDir.freetype}"
	}

	links
	{
		"Rynex",
		"gtest"
	}
	
	filter "system:windows"
		systemversion "latest"
	
	filter "configurations:Debug"
		defines "RY_DEBUG"
		runtime "Debug"
		symbols "on"


	filter "configurations:Release"
		defines "RY_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RY_DIST"
		runtime "Release"
		optimize "on"


