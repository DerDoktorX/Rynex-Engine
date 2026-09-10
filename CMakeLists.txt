
cmake_minimum_required(VERSION 4.3)


project(Rynex-Rendering)

include(cmake/FetchExternalLiberys.cmake)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

endif()


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex/vendor/Glad/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex/vendor/stb_image/)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex-Editor)