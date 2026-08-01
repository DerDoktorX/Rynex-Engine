project "Rynex-Editor"

	if BuildProjectConf == "Static" or BuildProjectConf == "Static2Lib"   then
		-- staticruntime "off" -- orig
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
		"%{wks.location}/Rynex/vendor/spdlog/include",
		-- Math
		"%{IncludeDir.glm}",
		-- Filse
		"%{IncludeDir.filewatch}",
		-- Entity
		--"%{IncludeDir.assimp}",
		"%{IncludeDir.magic_enum}",

		"%{IncludeDir.entt}",
		"%{IncludeDir.robin_hood_hashing}", -- has map
		-- Runtime Visuelle configs
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.ImPlot}",
		"%{IncludeDir.ImGui}",


		-- "%{IncludeDir.msdfgen}",
		-- "%{IncludeDir.msdf_atlas_gen}",
		-- "%{IncludeDir.freetype}"
	}

	links
	{
		"Rynex"
	}
	
	

	filter "system:windows"
		systemversion "latest"
	
	filter "configurations:Debug"
		defines "RY_DEBUG"
		runtime "Debug"
		symbols "on"
		--editandcontinue "Off"
		--buildoptions 
		--{ 
		--	"/Zi", "/fsanitize=address"
		--}
      	--linkoptions 
		--{ 
		--	"/fsanitize=address" 
		--}
		-- links
		-- {
		-- 	"%{Library.gtest_Debug}"
		-- }

	filter "configurations:Release"
		defines "RY_REALSE"
		runtime "Release"
		optimize "on"
		-- links
		-- {
		-- 	"%{Library.gtest_Release}"	
		-- }

	filter "configurations:Dist"
		defines "RY_DIST"
		runtime "Release"
		optimize "on"
		-- links
		-- {
		-- 	"%{Library.gtest_Release}"
		-- }

	-- filter "configurations:Preprocess"
	-- 	flags { "NoPCH" }
  	-- 	buildoptions { "/P" }

