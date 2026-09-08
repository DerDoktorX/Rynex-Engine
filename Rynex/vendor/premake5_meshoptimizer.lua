


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
    language "C++"
	cppdialect "C++17"

    local projectDir = "_meshoptimizer/"
	location "_meshoptimizer/"

    targetdir ( "" .. projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir ("" .. projectDir .. "bin/-int/" .. outputdir .. "/%{prj.name}")
    

    files 
    { 
        (projectDir .. "src/*.h"), 
        (projectDir .. "src/*.cpp") 
    }


    filter "system:windows"
		systemversion "latest"
    filter {}
    
	filter "system:linux"
		pic "On"
		systemversion "latest"
    filter {}
        
    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
    filter {}

    filter "configurations:Release"
        runtime "Release"       
        optimize "On"
    filter {}

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"
        symbols "off"
    filter {}

