add_executable("Sandbox"
  "Sandbox/src/Sandbox2D.cpp"
  "Sandbox/src/Sandbox2D.h"
  "Sandbox/src/SendboxApp.cpp"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_dependencies("Sandbox"
    "Rynex"
  )
  set_target_properties("Sandbox" PROPERTIES
    OUTPUT_NAME "Sandbox"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Debug-windows-x86_64/Sandbox"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Debug-windows-x86_64/Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Debug-windows-x86_64/Sandbox"
  )
endif()
target_include_directories("Sandbox" PRIVATE
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/spdlog/include>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/src>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/magic_enum>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/glm>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/entt/include>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/robin-hood-hashing/include>
)
target_compile_definitions("Sandbox" PRIVATE
  $<$<CONFIG:Debug>:RY_PLATFORM_WINDOWS>
  $<$<CONFIG:Debug>:RY_SANDBOX>
  $<$<CONFIG:Debug>:RY_DEBUG>
)
target_link_directories("Sandbox" PRIVATE
)
target_link_libraries("Sandbox"
  $<$<CONFIG:Debug>:Rynex>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Sandbox" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Sandbox" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Sandbox" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_dependencies("Sandbox"
    "Rynex"
  )
  set_target_properties("Sandbox" PROPERTIES
    OUTPUT_NAME "Sandbox"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Release-windows-x86_64/Sandbox"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Release-windows-x86_64/Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Release-windows-x86_64/Sandbox"
  )
endif()
target_include_directories("Sandbox" PRIVATE
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor/spdlog/include>
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/src>
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor>
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor/magic_enum>
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor/glm>
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor/entt/include>
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor/robin-hood-hashing/include>
)
target_compile_definitions("Sandbox" PRIVATE
  $<$<CONFIG:Release>:RY_PLATFORM_WINDOWS>
  $<$<CONFIG:Release>:RY_SANDBOX>
  $<$<CONFIG:Release>:RY_REALSE>
)
target_link_directories("Sandbox" PRIVATE
)
target_link_libraries("Sandbox"
  $<$<CONFIG:Release>:Rynex>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("Sandbox" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Sandbox" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("Sandbox" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  add_dependencies("Sandbox"
    "Rynex"
  )
  set_target_properties("Sandbox" PROPERTIES
    OUTPUT_NAME "Sandbox"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Dist-windows-x86_64/Sandbox"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Dist-windows-x86_64/Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Dist-windows-x86_64/Sandbox"
  )
endif()
target_include_directories("Sandbox" PRIVATE
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor/spdlog/include>
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/src>
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor>
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor/magic_enum>
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor/glm>
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor/entt/include>
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor/robin-hood-hashing/include>
)
target_compile_definitions("Sandbox" PRIVATE
  $<$<CONFIG:Dist>:RY_PLATFORM_WINDOWS>
  $<$<CONFIG:Dist>:RY_SANDBOX>
  $<$<CONFIG:Dist>:RY_DIST>
)
target_link_directories("Sandbox" PRIVATE
)
target_link_libraries("Sandbox"
  $<$<CONFIG:Dist>:Rynex>
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("Sandbox" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Sandbox" PRIVATE
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("Sandbox" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()