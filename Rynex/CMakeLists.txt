project(Rynex)


file(
        GLOB_RECURSE
        SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_library(
        Rynex
        ${SOURCE_FILES}
)
set_target_properties(Rynex PROPERTIES CXX_STANDARD 17)
if(MSVC)
    target_compile_options(Rynex PRIVATE /std:c++17)
else()
    # for other Compiler (GCC, Clang) functionary Default-Way
    target_compile_features(Rynex PRIVATE cxx_std_17)
endif()

target_include_directories(
        Rynex
      
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/vendor/fileWatch/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/magic_enum/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/robin-hood-hashing/include/
            ${CMAKE_CURRENT_LIST_DIR}/vendor/entt/include/
)

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



target_precompile_headers(
        Rynex
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/rypch.h
)


target_compile_definitions(
      Rynex
      PRIVATE
        $<$<CONFIG:Debug>:RY_DEBUG>
        $<$<CONFIG:RelEasy>:RY_RELEASE>            # equals Premake "Release"
        $<$<CONFIG:RelWithDebInfo>:RY_RELEASE>     # equals Premake "Release" just off crack
        $<$<CONFIG:Release>:RY_DIST>               # equals Premake "Dist"
)

if(WIN32)
    target_compile_definitions(
            Rynex
            PRIVATE
                RY_PLATFORM_WINDOWS
    )
else()
    target_compile_definitions(
            Rynex
            PRIVATE
                RY_PLATFORM_LINUX
    )
endif()

setupBinary(Rynex)

