project "freetype"
	location "freetype"
	kind "StaticLib"
	language "C"
    staticruntime "off"


	local projectDir = "_msdf-atlas-gen/msdfgen/"
	location "_msdf-atlas-gen/msdfgen/"

	targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		projectDir .. "freetype/include/ft2build.h",
		projectDir .. "freetype/include/freetype/*.h",
		projectDir .. "freetype/include/freetype/config/*.h",
		projectDir .. "freetype/include/freetype/internal/*.h",

		projectDir .. "freetype/src/autofit/autofit.c",
		projectDir .. "freetype/src/base/ftbase.c",
		projectDir .. "freetype/src/base/ftbbox.c",
		projectDir .. "freetype/src/base/ftbdf.c",
		projectDir .. "freetype/src/base/ftbitmap.c",
		projectDir .. "freetype/src/base/ftcid.c",
		projectDir .. "freetype/src/base/ftdebug.c",
		projectDir .. "freetype/src/base/ftfstype.c",
		projectDir .. "freetype/src/base/ftgasp.c",
		projectDir .. "freetype/src/base/ftglyph.c",
		projectDir .. "freetype/src/base/ftgxval.c",
		projectDir .. "freetype/src/base/ftinit.c",
		projectDir .. "freetype/src/base/ftmm.c",
		projectDir .. "freetype/src/base/ftotval.c",
		projectDir .. "freetype/src/base/ftpatent.c",
		projectDir .. "freetype/src/base/ftpfr.c",
		projectDir .. "freetype/src/base/ftstroke.c",
		projectDir .. "freetype/src/base/ftsynth.c",
		projectDir .. "freetype/src/base/ftsystem.c",
		projectDir .. "freetype/src/base/fttype1.c",
		projectDir .. "freetype/src/base/ftwinfnt.c",
		projectDir .. "freetype/src/bdf/bdf.c",
		projectDir .. "freetype/src/bzip2/ftbzip2.c",
		projectDir .. "freetype/src/cache/ftcache.c",
		projectDir .. "freetype/src/cff/cff.c",
		projectDir .. "freetype/src/cid/type1cid.c",
		projectDir .. "freetype/src/gzip/ftgzip.c",
		projectDir .. "freetype/src/lzw/ftlzw.c",
		projectDir .. "freetype/src/pcf/pcf.c",
		projectDir .. "freetype/src/pfr/pfr.c",
		projectDir .. "freetype/src/psaux/psaux.c",
		projectDir .. "freetype/src/pshinter/pshinter.c",
		projectDir .. "freetype/src/psnames/psnames.c",
		projectDir .. "freetype/src/raster/raster.c",
		projectDir .. "freetype/src/sdf/sdf.c",
		projectDir .. "freetype/src/sfnt/sfnt.c",
		projectDir .. "freetype/src/smooth/smooth.c",
		projectDir .. "freetype/src/truetype/truetype.c",
		projectDir .. "freetype/src/type1/type1.c",
		projectDir .. "freetype/src/type42/type42.c",
		projectDir .. "freetype/src/winfonts/winfnt.c"
	}

	includedirs
	{
		projectDir .. "freetype/include"
	}

	defines
	{
		"FT2_BUILD_LIBRARY",
		"_CRT_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"

project "msdfgen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "off"

	projectDir = "_msdf-atlas-gen/msdfgen/"
	location "_msdf-atlas-gen/msdfgen/"

	targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		projectDir .. "core/**.h",
		projectDir .. "core/**.hpp",
		projectDir .. "core/**.cpp",
		projectDir .. "ext/**.h",
		projectDir .. "ext/**.hpp",
		projectDir .. "ext/**.cpp",
		projectDir .. "lib/**.cpp",
		projectDir .. "include/**.h"
	}

	includedirs
	{
		projectDir .. "include",
		projectDir .. "freetype/include"
	}

	defines
	{
		"MSDFGEN_USE_CPP11"
	}

	links
	{
		"freetype"
	}

	filter "system:windows"
		systemversion "latest"
	filter {}
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
	filter {}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	filter {}

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
	filter {}