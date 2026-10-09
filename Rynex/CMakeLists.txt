project(Rynex)

# alle **.cpp in src/ add to the engine core libery.
file(
        GLOB_RECURSE
        SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_library(
        Rynex
        ${SOURCE_FILES}
)

# alle pulice include directory to the engine and to single file header liberys.
target_include_directories(
        Rynex
      
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/vendor/fileWatch/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/magic_enum/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/robin-hood-hashing/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/plf_colony/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/entt/include/
)

# linke alle liberys that int the engine provide public access for the implementing side
target_link_libraries(
        Rynex
      
        PUBLIC
            glm
            imgui
            spdlog

        PRIVATE
            glfw
            Glad
            assimp
            yaml-cpp
            meshoptimizer
            stb_image
            msdf-atlas-gen
)


# precompiled header for the core engine.
target_precompile_headers(
        Rynex
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/rypch.h
)

SET_CPP_COMPILER_VERSION_PROJECT(Rynex ${CPP_VERSION_USING})

# define compiler set defines.
target_compile_definitions(
      Rynex
      PRIVATE
        $<$<CONFIG:Debug>:RY_DEBUG>                # equals Premake "Debug"
        $<$<CONFIG:RelEasy>:RY_RELEASE>            # equals Premake "Release"
        $<$<CONFIG:RelWithDebInfo>:RY_RELEASE>     # equals Premake "Release" just off crack
        $<$<CONFIG:Release>:RY_DIST>               # equals Premake "Dist"
)

SET_BINARY_RESOURCE(Rynex)

# define compiler set define for target platform. (no WIN64 in cmake defined!)
if(WIN32)
    target_compile_definitions(
            Rynex
            PRIVATE
                RY_PLATFORM_WINDOWS
    )
    message("Platform Windows")
else()
    target_compile_definitions(
            Rynex
            PRIVATE
                RY_PLATFORM_LINUX
    )
    message("Platform Linux")
endif()



