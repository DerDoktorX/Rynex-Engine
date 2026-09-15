project(Sandbox)

file(
      GLOB_RECURSE
      SOURCE_FILES
      CONFIGURE_DEPENDS
      ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp
)

add_executable(
      Sandbox
      ${SOURCE_FILES}
)

target_include_directories(
      Sandbox
      PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/src/
)

target_link_libraries(
      Sandbox
      PRIVATE
      Rynex
)

target_compile_definitions(
      Sandbox
      PRIVATE
      RY_SANDBOX
      BUILD_SYSTEM_CMAKE
      $<$<CONFIG:Debug>:RY_DEBUG>
      $<$<CONFIG:RelInfoEasy>:RY_RELEASE>        # equals Premake "Release"
      $<$<CONFIG:RelWithDebInfo>:RY_RELEASE>     # equals Premake "Release" just off crack
      $<$<CONFIG:Release>:RY_DIST>               # equals Premake "Dist"
)
setupBinary(Sandbox)
if(WIN32)
    target_compile_definitions(
          Sandbox
          PRIVATE
          RY_PLATFORM_WINDOWS
    )
else()
    target_compile_definitions(
          Sandbox
          PRIVATE
          RY_PLATFORM_LINUX
    )
endif()
