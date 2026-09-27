project(Rynex-Test)

file(
      GLOB_RECURSE
      SOURCE_FILES
      CONFIGURE_DEPENDS
      ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_executable(
      Rynex-Test
      ${SOURCE_FILES}
)
set_target_properties(Rynex-Test PROPERTIES CXX_STANDARD 17)
if(MSVC)
    target_compile_options(Rynex-Test PRIVATE /std:c++17)
else()
    # for other Compiler (GCC, Clang) functionary Default-Way
    target_compile_features(Rynex-Test PRIVATE cxx_std_17)
endif()

target_include_directories(
      Rynex-Test
      PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/src/
)

target_link_libraries(
      Rynex-Test
        PRIVATE
            Rynex
            gtest
            gtest_main
)

target_compile_definitions(
      Rynex-Test
      PRIVATE
        RY_EDITOR
        BUILD_SYSTEM_CMAKE
        $<$<CONFIG:Debug>:RY_DEBUG>
        $<$<CONFIG:RelInfoEasy>:RY_RELEASE>        # equals Premake "Release"
        $<$<CONFIG:RelWithDebInfo>:RY_RELEASE>     # equals Premake "Release" just off crack
        $<$<CONFIG:Release>:RY_DIST>               # equals Premake "Dist"
)

setupBinary(Rynex-Test)

if(WIN32)
    target_compile_definitions(Rynex-Test PRIVATE RY_PLATFORM_WINDOWS )
else()
    target_compile_definitions(Rynex-Test PRIVATE RY_PLATFORM_LINUX)
endif()