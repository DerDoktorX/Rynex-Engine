
set(_rynex_prev_policy_min 4.3)
set(CMAKE_POLICY_VERSION_MINIMUM ${_rynex_prev_policy_min})

include(FetchContent)


set(RYNEX_VER_SPDLOG            v1.15.3)
set(RYNEX_VER_GLM               1.0.1)
set(RYNEX_VER_YAML_CPP          0.8.0)
set(RYNEX_VER_IMGUI             v1.91.9-docking)
set(RYNEX_VER_GLFW              3.4)
set(RYNEX_VER_ASSIMP            v6.0.4) #  v5.4.3
set(RYNEX_VER_MESHOPTIMIZER     v0.22)
# set(RYNEX_VER_MSDF_ATLAS_GEN    v1.3)
set(RYNEX_VER_MSDF_ATLAS_GEN    v1.2.2)
set(RYNEX_VER_FREETYPE          VER-2-13-2) # libery from msdfgen-atlas
set(RYNEX_VER_ZLIB              v1.3.1) # libery from msdfgen-atlas
set(RYNEX_VER_LIBPNG            v1.6.43) # libery from msdfgen-atlas

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_fetch" CACHE PATH "Network Verzeichnis fuer FetchContent-Verwaltungsdaten")
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Kein erneuter Netzwerkzugriff wenn Quellverzeichnis bereits vorhanden")
set(RYNEX_VENDOR_DIR "${CMAKE_SOURCE_DIR}/Rynex/vendor")


macro(rynex_make_available _fc_name)
    message("\n")
    message("==================================================================================================================")
    message("=== Make available Libery ${_fc_name}: \t  ${RYNEX_VENDOR_DIR}/${_fc_name}")
    message("==================================================================================================================\n")

    set(oneValueArgs POLICY_VERSION FOLDER_NAME)


    cmake_parse_arguments(_RD ""  "${oneValueArgs}" "" ${ARGN})


    if (${_RD_POLICY_VERSION})
        message(NOTICE "Set CMAKE POLICY VERSION MINIMUM temporary for this Libery to ${_RD_POLICY_VERSION}")
        set(CMAKE_POLICY_VERSION_MINIMUM ${_RD_POLICY_VERSION})
    endif()
    if(_RD_FOLDER_NAME)
        FetchContent_MakeAvailable(
                ${_fc_name}
                SOURCE_DIR      ${RYNEX_VENDOR_DIR}/${_RD_FOLDER_NAME}
        )
    else()
        FetchContent_MakeAvailable(${_fc_name})
    endif()
    if (${_RD_POLICY_VERSION})
        message(NOTICE "Rest CMAKE POLICY VERSION MINIMUM ${_rynex_prev_policy_min}")
        set(CMAKE_POLICY_VERSION_MINIMUM ${_rynex_prev_policy_min})
    endif()

endmacro()

macro(rynex_declare _fc_name _fc_repo _fc_tag)


    set(options SUBMODULES OVERRIDE_FIND_PACKAGE)
    set(oneValueArgs FOLDER_NAME)
    cmake_parse_arguments(_RD "${options}" "${oneValueArgs}" "" ${ARGN})

    message("${_fc_name}: ${_fc_repo}  ${_fc_tag}   (${_RD_SUBMODULES}/${_RD_OVERRIDE_FIND_PACKAGE})")
    if(_RD_FOLDER_NAME)
        FetchContent_Declare(
                ${_fc_name}
                GIT_REPOSITORY          ${_fc_repo}
                GIT_TAG                 ${_fc_tag}
                GIT_SHALLOW             TRUE
                GIT_PROGRESS            TRUE
                SOURCE_DIR              ${RYNEX_VENDOR_DIR}/${_RD_FOLDER_NAME}
                GIT_SUBMODULES_RECURSE  ${_RD_SUBMODULES}
                OVERRIDE_FIND_PACKAGE   ${_RD_OVERRIDE_FIND_PACKAGE}
        )
    else()
        FetchContent_Declare(
                ${_fc_name}
                GIT_REPOSITORY          ${_fc_repo}
                GIT_TAG                 ${_fc_tag}
                GIT_SHALLOW             TRUE
                GIT_PROGRESS            TRUE
                SOURCE_DIR              ${RYNEX_VENDOR_DIR}/${_fc_name}
                GIT_SUBMODULES_RECURSE  ${_RD_SUBMODULES}
                OVERRIDE_FIND_PACKAGE   ${_RD_OVERRIDE_FIND_PACKAGE}
        )
    endif()

endmacro()

# =========================================================================================
# ========= spdlog ========================================================================
# =========================================================================================

set(SPDLOG_BUILD_SHARED     OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL          OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE    OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS      OFF CACHE BOOL "" FORCE)

rynex_declare(
        spdlog
        https://github.com/gabime/spdlog.git
        ${RYNEX_VER_SPDLOG}
)

# =========================================================================================
# ========= yaml-cpp ======================================================================
# =========================================================================================

set(YAML_CPP_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS    OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB  OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL        OFF CACHE BOOL "" FORCE)

rynex_declare(
        yaml-cpp
        https://github.com/jbeder/yaml-cpp.git
        ${RYNEX_VER_YAML_CPP}
)

# =========================================================================================
# ========= glm ===========================================================================
# =========================================================================================

set(GLM_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL   OFF CACHE BOOL "" FORCE)

rynex_declare(
        glm
        https://github.com/g-truc/glm.git
        ${RYNEX_VER_GLM}
)

# =========================================================================================
# ========= zlib ==========================================================================
# =========================================================================================

# rynex_declare(
#         zlib
#         https://github.com/madler/zlib.git
#         ${RYNEX_VER_ZLIB}
# )


# =========================================================================================
# ========= zlib ==========================================================================
# =========================================================================================

# rynex_declare(
#         libpng
#         https://github.com/glennrp/libpng.git
#         ${RYNEX_VER_LIBPNG}
# )

# =========================================================================================
# ========= freetype ======================================================================
# =========================================================================================

set(FT_DISABLE_HARFBUZZ   ON CACHE BOOL "" FORCE)
rynex_declare(
        freetype
        https://github.com/freetype/freetype.git
        ${RYNEX_VER_FREETYPE}
)

# FetchContent_Declare(
#         Freetype::Freetype
#         GIT_REPOSITORY          https://github.com/freetype/freetype.git
#         GIT_TAG                  ${RYNEX_VER_FREETYPE}
#         GIT_SHALLOW             TRUE
#         GIT_PROGRESS            TRUE
#         SOURCE_DIR              ${RYNEX_VENDOR_DIR}/freetype
#         BINARY_DIR              ${RYNEX_VENDOR_DIR}/freetype/freetype
# )



# =========================================================================================
# ========= msdf-atlas-gen ================================================================
# =========================================================================================

set(MSDF_ATLAS_BUILD_STANDALONE     OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_INSTALL              OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_VCPKG            OFF CACHE BOOL "" FORCE)
set(MSDFGEN_INSTALL                 OFF CACHE BOOL "" FORCE)
set(MSDFGEN_BUILD_STANDALONE        OFF CACHE BOOL "" FORCE)
set(MSDFGEN_USE_VCPKG               OFF CACHE BOOL "" FORCE)
set(MSDFGEN_USE_SYSTEM_FREETYPE     OFF CACHE BOOL "" FORCE)


rynex_declare(
        msdf-atlas-gen
        https://github.com/Chlumsky/msdf-atlas-gen.git
        ${RYNEX_VER_MSDF_ATLAS_GEN}
        # OVERRIDE_FIND_PACKAGE
)

# =========================================================================================
# ========= imgui =========================================================================
# =========================================================================================

rynex_declare(
        imgui
        https://github.com/ocornut/imgui.git
        ${RYNEX_VER_IMGUI}
)

# =========================================================================================
# ======== GLFW ===========================================================================
# =========================================================================================

set(GLFW_BUILD_DOCS         OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL            OFF CACHE BOOL "" FORCE)

rynex_declare(
        glfw
        https://github.com/glfw/glfw.git
        ${RYNEX_VER_GLFW}
)

# =========================================================================================
# ======= assimp ==========================================================================
# =========================================================================================

set(ASSIMP_BUILD_TESTS                      OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL                          OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB                      OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS               OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES                    OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS               OFF CACHE BOOL "" FORCE)

# all Importer deactivate, then selective activate
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT   OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER               ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER              ON  CACHE BOOL "" FORCE)  # deckt glTF + glTF2 ab
set(ASSIMP_BUILD_OBJ_IMPORTER               ON  CACHE BOOL "" FORCE)

# all Exporter deactivate
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT   OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_USD_IMPORTER               OFF CACHE BOOL "" FORCE)

rynex_declare(
        assimp
        https://github.com/assimp/assimp.git
        ${RYNEX_VER_ASSIMP}
)



# =========================================================================================
# ======= meshoptimizer ===================================================================
# =========================================================================================

rynex_declare(
        meshoptimizer
        https://github.com/zeux/meshoptimizer.git
        ${RYNEX_VER_MESHOPTIMIZER}
)


# =========================================================================================
# =========================================================================================
# =========================================================================================




rynex_make_available(
        yaml-cpp
        POLICY_VERSION  3.5
)





rynex_make_available(assimp)
rynex_make_available(spdlog)
rynex_make_available(glm)
rynex_make_available(yaml-cpp)
rynex_make_available(glfw)
rynex_make_available(meshoptimizer)


# =========================================================================================
# =========================================================================================
# =========================================================================================
FetchContent_MakeAvailable(imgui)


set(IMGUI_DIR "${RYNEX_VENDOR_DIR}/imgui")

if(NOT TARGET imgui)
    add_library(
            imgui
            STATIC
            "${IMGUI_DIR}/imgui.cpp"
            "${IMGUI_DIR}/imgui_demo.cpp"
            "${IMGUI_DIR}/imgui_draw.cpp"
            "${IMGUI_DIR}/imgui_tables.cpp"
            "${IMGUI_DIR}/imgui_widgets.cpp"

            "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
            "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    )

    target_include_directories(imgui
            PUBLIC
            "${IMGUI_DIR}"
            "${IMGUI_DIR}/backends"
    )

    target_link_libraries(
            imgui
            PUBLIC
            glfw
    )


    target_compile_definitions(
            imgui
            PUBLIC
            IMGUI_IMPL_OPENGL_LOADER_GLAD
    )

    add_library(
            imgui::imgui
            ALIAS
            imgui
    )
endif()

# FetchContent_MakeAvailable(zlib)
# FetchContent_MakeAvailable(libpng)
FetchContent_MakeAvailable(freetype)

if(NOT freetype_POPULATED)
    FetchContent_Populate(freetype)

    # Hilfsvariablen für find_package setzen, die FreeType erwartet
    set(ZLIB_LIBRARY "ZLIB_LIBRARY-NOTFOUND" CACHE STRING "" FORCE)
    set(PNG_LIBRARY "PNG_LIBRARY-NOTFOUND" CACHE STRING "" FORCE)
    set(PNG_PNG_INCLUDE_DIR "PNG_PNG_INCLUDE_DIR-NOTFOUND" CACHE STRING "" FORCE)
    # Baue FreeType
    add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR})

endif()
add_library(Freetype::Freetype ALIAS freetype)


get_target_property(_aliased Freetype::Freetype ALIASED_TARGET)
if(_aliased)
    message(STATUS "The name Freetype::Freetype is an ALIAS for ${_aliased}.")
else()
    message(WARNING "The name Freetype::Freetype is not an ALIAS for ${_aliased}.")
endif()


rynex_make_available(msdf-atlas-gen)
unset(_rynex_prev_policy_min)