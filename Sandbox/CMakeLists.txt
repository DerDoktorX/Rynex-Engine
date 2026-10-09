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
set_target_properties(Sandbox PROPERTIES CXX_STANDARD ${CPP_VERSION_USING})
if(MSVC)
    set(CPP_VERSION_COMPILE_VERSION_OPTIONS /std:c++${CPP_VERSION_USING})
    target_compile_options(Sandbox PRIVATE ${CPP_VERSION_COMPILE_VERSION_OPTIONS})
    
    set(CPP_VERSION_COMPILE_VERSION_FEATURES cxx_std_${CPP_VERSION_USING})
    target_compile_features(Sandbox PRIVATE ${CPP_VERSION_COMPILE_VERSION_FEATURES})
else()
    # for other Compiler (GCC, Clang) functionary Default-Way
    set(CPP_VERSION_COMPILE_VERSION_OPTIONS -std=c++${CPP_VERSION_USING})
    target_compile_options(Sandbox PRIVATE ${CPP_VERSION_COMPILE_VERSION_OPTIONS})
    
    set(CPP_VERSION_COMPILE_VERSION_FEATURES cxx_std_${CPP_VERSION_USING})
    target_compile_features(Sandbox PRIVATE ${CPP_VERSION_COMPILE_VERSION_FEATURES})
endif()

SET_CPP_COMPILER_VERSION_PROJECT(Sandbox ${CPP_VERSION_USING})

target_compile_definitions(
      Sandbox
      PRIVATE
      RY_SANDBOX
      BUILD_SYSTEM_CMAKE
      $<$<CONFIG:Debug>:RY_DEBUG>
      $<$<CONFIG:RelInfoEasy>:RY_RELEASE>        # equals Premake "Release"
      $<$<CONFIG:RelWithDebInfo>:RY_RELEASE>     # equals Premake "Release" just off crack
      $<$<CONFIG:Release>:RY_DIST>               # equals Premake "Dist"
      RY_CPP_${CPP_VERSION_USING}
)

SET_BINARY_RESOURCE(Sandbox)

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
