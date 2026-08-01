project "Glad"    
    if BuildProjectConf == "Static" or BuildProjectConf == "StaticLib" then
		-- staticruntime "on" -- orig
		staticruntime "on" 
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "Static2Lib" then
		staticruntime "on" -- orig
	end
	if BuildProjectConf == "Dynamic" then
		 staticruntime "off"
	end
    kind "StaticLib"
    language "C"

    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"
        

    --filter { "system:windows", "configurations:Release" }
    --    buildoptions "/MT"
