
cmake_minimum_required(VERSION 4.3)

project(Rynex-Rendering)
include(FetchContent)

add_subdirectory(Rynex/vendor/assimp)
add_subdirectory(Rynex/vendor/GLFW)
add_subdirectory(Rynex/vendor/glm)
add_subdirectory(Rynex/vendor/meshoptimizer)
add_subdirectory(Rynex/vendor/spdlog)

FetchContent_Declare(
        yaml-cpp
        # GIT_REPOSITORY https://github.com/ocornut/imgui.git
        # GIT_TAG v1.91.9b-docking
        # DOWNLOAD_EXTRACT_TIMESTAMP OFF
)
add_subdirectory(Rynex/vendor/yaml-cpp)