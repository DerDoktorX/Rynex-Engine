project "assimp"

	if BuildProjectConf == "Static" or BuildProjectConf == "StaticLib" then
		staticruntime "on"
	end
	if BuildProjectConf == "Static2" or BuildProjectConf == "Static2Lib" then
		staticruntime "off"
	end
	if BuildProjectConf == "Dynamic" then
		staticruntime "off"
	end

	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	
	
	local projectDir = "_assimp/"
	location "_assimp"

	
	
	targetdir (projectDir .. "bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectDir .. "bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		projectDir .. "include/**.h",
		projectDir .. "include/**.hpp",
		projectDir .. "include/**.inl",

		-- Kern-Infrastruktur (Importer/Exporter-Framework, Szene, Postprocessing, Material, Geometrie)
		projectDir .. "code/Common/**.cpp",
		projectDir .. "code/Common/**.h",
		projectDir .. "code/CApi/**.cpp",
		projectDir .. "code/CApi/**.h",
		projectDir .. "code/Material/**.cpp",
		projectDir .. "code/Material/**.h",
		projectDir .. "code/PostProcessing/**.cpp",
		projectDir .. "code/PostProcessing/**.h",
		projectDir .. "code/Geometry/**.cpp",
		projectDir .. "code/Geometry/**.h",

		-- Nur die aktuell benoetigten Formate
		projectDir .. "code/AssetLib/FBX/**.cpp",
		projectDir .. "code/AssetLib/FBX/**.h",
		projectDir .. "code/AssetLib/glTF/**.cpp",
		projectDir .. "code/AssetLib/glTF/**.h",
		projectDir .. "code/AssetLib/glTF/**.inl",
		projectDir .. "code/AssetLib/glTF2/**.cpp",
		projectDir .. "code/AssetLib/glTF2/**.h",
		projectDir .. "code/AssetLib/glTF2/**.inl",
		projectDir .. "code/AssetLib/Obj/**.cpp",
		projectDir .. "code/AssetLib/Obj/**.h",

		-- zlib und unzip werden von Common/ZipArchiveIOSystem.cpp + FBX (komprimierte Streams)
		-- IMMER unbedingt gebraucht, unabhaengig vom Format
		projectDir .. "contrib/zlib/*.c",
		projectDir .. "contrib/zlib/*.h",
		projectDir .. "contrib/unzip/*.c",
		projectDir .. "contrib/unzip/*.h",

		-- generischer XML-Parser (kleine, guenstige Absicherung falls Common intern genutzt)
		-- projectDir .. "projectDircontrib/pugixml/src/pugixml.cpp",
		-- projectDir .. "projectDircontrib/pugixml/src/*.hpp",

		projectDir .. "contrib/pugixml/src/pugixml.cpp",
		projectDir .. "contrib/pugixml/src/*.hpp"
	}

	includedirs
	{
		projectDir .. ".",
		projectDir .. "code",
		projectDir .. "include",

		projectDir .. "contrib",                     -- loest "stb/stb_image.h" auf (Common/StbCommon.h)
		projectDir .. "contrib/zlib",
		projectDir .. "contrib/unzip",
		projectDir .. "contrib/pugixml/src",
		projectDir .. "contrib/rapidjson/include",   -- glTF/glTF2 JSON-Parsing
		projectDir .. "contrib/utf8cpp/source",      -- loest "utf8.h" auf (u.a. Common/BaseImporter.cpp)
	}

	defines
	{
        "RAPIDJSON_HAS_STDSTRING=1",
        "RAPIDJSON_NOMEMBERITERATORCLASS",
        
		-- Importer, die wir NICHT brauchen (nur GLTF/GLTF2/FBX/OBJ bleiben aktiv)
		"ASSIMP_BUILD_NO_USD_IMPORTER",
		"ASSIMP_BUILD_NO_X_IMPORTER",
		"ASSIMP_BUILD_NO_AMF_IMPORTER",
		"ASSIMP_BUILD_NO_3DS_IMPORTER",
		"ASSIMP_BUILD_NO_MD3_IMPORTER",
		"ASSIMP_BUILD_NO_MDL_IMPORTER",
		"ASSIMP_BUILD_NO_MD2_IMPORTER",
		"ASSIMP_BUILD_NO_PLY_IMPORTER",
		"ASSIMP_BUILD_NO_ASE_IMPORTER",
		"ASSIMP_BUILD_NO_HMP_IMPORTER",
		"ASSIMP_BUILD_NO_SMD_IMPORTER",
		"ASSIMP_BUILD_NO_MDC_IMPORTER",
		"ASSIMP_BUILD_NO_MD5_IMPORTER",
		"ASSIMP_BUILD_NO_STL_IMPORTER",
		"ASSIMP_BUILD_NO_LWO_IMPORTER",
		"ASSIMP_BUILD_NO_DXF_IMPORTER",
		"ASSIMP_BUILD_NO_NFF_IMPORTER",
		"ASSIMP_BUILD_NO_RAW_IMPORTER",
		"ASSIMP_BUILD_NO_SIB_IMPORTER",
		"ASSIMP_BUILD_NO_OFF_IMPORTER",
		"ASSIMP_BUILD_NO_AC_IMPORTER",
		"ASSIMP_BUILD_NO_BVH_IMPORTER",
		"ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
		"ASSIMP_BUILD_NO_IRR_IMPORTER",
		"ASSIMP_BUILD_NO_Q3D_IMPORTER",
		"ASSIMP_BUILD_NO_B3D_IMPORTER",
		"ASSIMP_BUILD_NO_COLLADA_IMPORTER",
		"ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
		"ASSIMP_BUILD_NO_CSM_IMPORTER",
		"ASSIMP_BUILD_NO_3D_IMPORTER",
		"ASSIMP_BUILD_NO_LWS_IMPORTER",
		"ASSIMP_BUILD_NO_OGRE_IMPORTER",
		"ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
		"ASSIMP_BUILD_NO_MS3D_IMPORTER",
		"ASSIMP_BUILD_NO_COB_IMPORTER",
		"ASSIMP_BUILD_NO_BLEND_IMPORTER",
		"ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
		"ASSIMP_BUILD_NO_NDO_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"ASSIMP_BUILD_NO_XGL_IMPORTER",
		"ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_3MF_IMPORTER",
		"ASSIMP_BUILD_NO_X3D_IMPORTER",
		"ASSIMP_BUILD_NO_MMD_IMPORTER",
		"ASSIMP_BUILD_NO_M3D_IMPORTER",
		"ASSIMP_BUILD_NO_IQM_IMPORTER",

		-- Exporter, deren Quellordner wir gar nicht erst kompilieren
		-- (sonst: "unresolved external symbol" in Exporter.cpp)
		"ASSIMP_BUILD_NO_X_EXPORTER",
		"ASSIMP_BUILD_NO_STEP_EXPORTER",
		"ASSIMP_BUILD_NO_STL_EXPORTER",
		"ASSIMP_BUILD_NO_PLY_EXPORTER",
		"ASSIMP_BUILD_NO_3DS_EXPORTER",
		"ASSIMP_BUILD_NO_COLLADA_EXPORTER",
		"ASSIMP_BUILD_NO_ASSBIN_EXPORTER",
		"ASSIMP_BUILD_NO_ASSXML_EXPORTER",
		"ASSIMP_BUILD_NO_X3D_EXPORTER",
		"ASSIMP_BUILD_NO_M3D_EXPORTER",
		"ASSIMP_BUILD_NO_3MF_EXPORTER",
		"ASSIMP_BUILD_NO_ASSJSON_EXPORTER",
		"ASSIMP_BUILD_NO_PBRT_EXPORTER",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"WIN32_LEAN_AND_MEAN",
			"UNICODE",
			"_UNICODE",
		}
	filter {}
	
	filter "system:linux"
    	systemversion "latest"
    	pic "On"
	filter {}

	filter "toolset:msc*"
		buildoptions { "/bigobj" }
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
	filter {}

