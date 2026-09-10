project(Rynex-Editor)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_library(
        Rynex-Editor
        ${SOURCE_FILES}
)

target_include_directories(
        Rynex-Editor
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/
)

target_link_libraries(
        Rynex-Editor
        PRIVATE
        Rynex
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

