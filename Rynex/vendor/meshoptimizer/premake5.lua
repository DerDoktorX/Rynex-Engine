


project( "meshoptimizer" )
    
    
	if BuildProjectConf == "Static" or BuildProjectConf == "StaticLib" then
		-- staticruntime "off" -- orig
		staticruntime "on" 
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "Static2Lib" then
		staticruntime "off" -- orig
	end
	if BuildProjectConf == "Dynamic" then
		 staticruntime "off"
	end
    
    kind "StaticLib"

    

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files { 
        "src/*.h", 
        "src/*.cpp" 
    }

    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        runtime "Release"       
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"
        symbols "off"

