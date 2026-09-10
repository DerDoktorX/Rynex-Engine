project(Glad)

add_library(
        Glad
        STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/glad.c
)

target_include_directories(
        Glad
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include/
)

