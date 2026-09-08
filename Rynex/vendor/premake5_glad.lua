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

    local projectDir = "Glad/"
	location "Glad/"


    targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        projectDir .. "include/glad/glad.h",
        projectDir .. "include/KHR/khrplatform.h",
        projectDir .. "src/glad.c"
    }

    includedirs
    {
        projectDir .. "include"
    }
    
    filter "system:windows"
        systemversion "latest"

        -- filter { "configurations:Release" }
        -- buildoptions "/MT"
    filter {}
    
    filter "system:linux"
        systemversion "latest"
    filter {}
    
