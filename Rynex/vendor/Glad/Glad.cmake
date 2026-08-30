add_library("Glad" STATIC
      "Rynex/vendor/Glad/include/KHR/khrplatform.h"
      "Rynex/vendor/Glad/include/glad/glad.h"
    "Rynex/vendor/Glad/src/glad.c"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Glad" PROPERTIES
    OUTPUT_NAME "Glad"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Debug-windows-x86_64/Glad"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Debug-windows-x86_64/Glad"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Debug-windows-x86_64/Glad"
  )
endif()
target_include_directories("Glad" PRIVATE
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/Glad/include>
)
target_compile_definitions("Glad" PRIVATE
  $<$<CONFIG:Debug>:RY_PLATFORM_WINDOWS>
)
target_link_directories("Glad" PRIVATE
)
target_link_libraries("Glad"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Glad" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Glad" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("Glad" PROPERTIES
    OUTPUT_NAME "Glad"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Release-windows-x86_64/Glad"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Release-windows-x86_64/Glad"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Release-windows-x86_64/Glad"
  )
endif()
target_include_directories("Glad" PRIVATE
  $<$<CONFIG:Release>:D:/dev/Rynex-Rendering/Rynex/vendor/Glad/include>
)
target_compile_definitions("Glad" PRIVATE
  $<$<CONFIG:Release>:RY_PLATFORM_WINDOWS>
)
target_link_directories("Glad" PRIVATE
)
target_link_libraries("Glad"
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("Glad" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Glad" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-m64>
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("Glad" PROPERTIES
    OUTPUT_NAME "Glad"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Dist-windows-x86_64/Glad"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Dist-windows-x86_64/Glad"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/Rynex/vendor/Glad/bin/Dist-windows-x86_64/Glad"
  )
endif()
target_include_directories("Glad" PRIVATE
  $<$<CONFIG:Dist>:D:/dev/Rynex-Rendering/Rynex/vendor/Glad/include>
)
target_compile_definitions("Glad" PRIVATE
  $<$<CONFIG:Dist>:RY_PLATFORM_WINDOWS>
)
target_link_directories("Glad" PRIVATE
)
target_link_libraries("Glad"
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("Glad" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Glad" PRIVATE
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-m64>
)