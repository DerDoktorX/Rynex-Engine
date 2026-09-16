project(Rynex-Editor)

file(
        GLOB_RECURSE
        SOURCE_FILES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_executable(
        Rynex-Editor
        ${SOURCE_FILES}
)
set_target_properties(Rynex-Editor PROPERTIES CXX_STANDARD 17)
if(MSVC)
    target_compile_options(Rynex-Editor PRIVATE /std:c++17)
else()
    # for other Compiler (GCC, Clang) functionary Default-Way
    target_compile_features(Rynex-Editor PRIVATE cxx_std_17)
endif()

target_include_directories(
        Rynex-Editor
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/
)

target_link_libraries(
        Rynex-Editor
        PRIVATE
        Rynex
)


target_compile_definitions(
      Rynex-Editor
      PRIVATE
        RY_EDITOR
        BUILD_SYSTEM_CMAKE
        $<$<CONFIG:Debug>:RY_DEBUG>
        $<$<CONFIG:RelInfoEasy>:RY_RELEASE>        # equals Premake "Release"
        $<$<CONFIG:RelWithDebInfo>:RY_RELEASE>     # equals Premake "Release" just off crack
        $<$<CONFIG:Release>:RY_DIST>               # equals Premake "Dist"
)
setupBinary(Rynex-Editor)
if(WIN32)
    target_compile_definitions(
            Rynex-Editor
            PRIVATE
                RY_PLATFORM_WINDOWS
    )
else()
    target_compile_definitions(
            Rynex-Editor
            PRIVATE
                RY_PLATFORM_LINUX
    )
endif()
