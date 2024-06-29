project "Rynex-ScriptingCore"
    --location "Rynex"
	kind "SharedLib"
    language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/Rynex-Editor/Resources/Scripts")
	objdir ("%{wks.location}/Rynex-Editor/Resources/Scripts/Intermediates")

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
		

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Default"