project(Rynex)

file(GLOB_RECURSE SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/**.h"
)

add_executable(
        ${PROJECT_NAME}
        ${SOURCE_FILES}
)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        GLFW
        # Glad
        Assimp
)


