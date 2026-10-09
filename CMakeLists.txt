cmake_minimum_required(VERSION 4.3)


project(Rynex-Rendering)

include(cmake/FetchExternalLiberys.cmake)
include(cmake/ProjectCompiler.cmake)

set(CPP_VERSION_USING 17)
set(CMAKE_CXX_STANDARD ${CPP_VERSION_USING})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message("using compiler ${CMAKE_CXX_COMPILER_ID}")
set(RYNEX_COMPILER_NAME "")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # using clang with clang-cl front end
        
    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        # using clang with regular front end
    endif()
    set(RYNEX_COMPILER_NAME "${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}")
    message("using clang with ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT} front end.")
    # using Clang
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using MSVC
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")
    endif()
endif()

if(RYNEX_COMPILER_NAME STREQUAL "")
    set(RYNEX_COMPILER_NAME ${CMAKE_CXX_COMPILER_ID})
endif ()






add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex/vendor/Glad/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex/vendor/stb_image/)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Sandbox)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex-Editor)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex-Test)

message("cxx dbg flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message("cxx flags: ${CMAKE_CXX_FLAGS}")
