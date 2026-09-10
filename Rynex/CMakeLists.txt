project(Rynex)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_library(
        Rynex
        ${SOURCE_FILES}
)

target_include_directories(
        Rynex
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/vendor/entt/include/
        ${CMAKE_CURRENT_LIST_DIR}/vendor/fileWatch/include/
        ${CMAKE_CURRENT_LIST_DIR}/vendor/magic_enum/include/
        ${CMAKE_CURRENT_LIST_DIR}/vendor/robin-hood-hashing/include/
)

target_link_libraries(
        Rynex
        PUBLIC
        glm
        imgui
        spdlog

        PRIVATE
        GLFW
        Glad
        assimp
        yaml-cpp
        meshoptimizer
        stb_image
)

target_precompile_headers(
        Rynex
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/rypch.cpp
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

