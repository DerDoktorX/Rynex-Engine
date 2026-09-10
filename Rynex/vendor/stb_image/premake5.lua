project "stb_image"    
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
        "include/stb_image/stb_image.h",
        "include/stb_image/stb_image_write.h",
        "src/stb_image.cpp"
    }

    includedirs
    {
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"
        -- filter { "configurations:Release" }
        -- buildoptions "/MT"
    filter {}
    
    filter "system:linux"
        systemversion "latest"
    filter {}
    
