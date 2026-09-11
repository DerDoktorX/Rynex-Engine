project(Rynex-Editor)

file(
        GLOB_RECURSE
        SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_executable(
        Rynex-Editor
        ${SOURCE_FILES}
)

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

rynex_deploy_resources(
      Rynex-Editor
      
      SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
      
      # LINK_DIRS
        # Editor-Assets
        # Resources
        # SandboxProject
        # mono
      
      LINK_FILES
        imgui.ini
      
      MAKE_DIRS
        DLLs
        Profile
)
