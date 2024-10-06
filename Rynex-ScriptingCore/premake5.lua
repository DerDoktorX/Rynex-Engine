project "Rynex-ScriptingCore"
    --location "Rynex"
	kind "SharedLib"
    language "C#"
	clr "unsafe"
	dotnetframework "4.7.2"

	targetdir ("../Rynex-Editor/Resources/Scripts")
	objdir ("../Rynex-Editor/Resources/Scripts/Intermediates")

	

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