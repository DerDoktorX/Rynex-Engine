add_executable("Rynex-Editor"
  "Rynex-Editor/src/EditorLayer.cpp"
  "Rynex-Editor/src/EditorLayer.h"
    "Rynex-Editor/src/ImGuiObj/ImGuiObjects.cpp"
    "Rynex-Editor/src/ImGuiObj/ImGuiObjects.h"
    "Rynex-Editor/src/Launcher/LauncherLayer.cpp"
    "Rynex-Editor/src/Launcher/LauncherLayer.h"
    "Rynex-Editor/src/Pannel/ContentBrowserPannel.cpp"
    "Rynex-Editor/src/Pannel/ContentBrowserPannel.h"
    "Rynex-Editor/src/Pannel/MaterielPannel.cpp"
    "Rynex-Editor/src/Pannel/MaterielPannel.h"
    "Rynex-Editor/src/Pannel/MenuBarPannel.cpp"
    "Rynex-Editor/src/Pannel/MenuBarPannel.h"
    "Rynex-Editor/src/Pannel/MeshPannel.cpp"
    "Rynex-Editor/src/Pannel/MeshPannel.h"
    "Rynex-Editor/src/Pannel/ProjectPannel.cpp"
    "Rynex-Editor/src/Pannel/ProjectPannel.h"
    "Rynex-Editor/src/Pannel/RendererPannel.cpp"
    "Rynex-Editor/src/Pannel/RendererPannel.h"
    "Rynex-Editor/src/Pannel/SceneHierachyPannel.cpp"
    "Rynex-Editor/src/Pannel/SceneHierachyPannel.h"
    "Rynex-Editor/src/Pannel/ViewPortPannel.cpp"
    "Rynex-Editor/src/Pannel/ViewPortPannel.h"
  "Rynex-Editor/src/RynexEditorApp.cpp"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_dependencies("Rynex-Editor"
    "Rynex"
  )
  set_target_properties("Rynex-Editor" PROPERTIES
    OUTPUT_NAME "Rynex-Editor"
    ARCHIVE_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Debug-windows-x86_64/Rynex-Editor"
    LIBRARY_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Debug-windows-x86_64/Rynex-Editor"
    RUNTIME_OUTPUT_DIRECTORY "D:/dev/Rynex-Rendering/bin/Debug-windows-x86_64/Rynex-Editor"
  )
endif()
target_include_directories("Rynex-Editor" PRIVATE
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex-Editor/src>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/src>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/spdlog/include>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/glm>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/filewatch>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/magic_enum>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/entt/include>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/robin-hood-hashing/include>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/ImGuizmo>
  $<$<CONFIG:Debug>:D:/dev/Rynex-Rendering/Rynex/vendor/imgui>
)
target_compile_definitions("Rynex-Editor" PRIVATE
  $<$<CONFIG:Debug>:RY_PLATFORM_WINDOWS>
  $<$<CONFIG:Debug>:RY_DEBUG>
)
target_link_directories("Rynex-Editor" PRIVATE
)
target_link_libraries("Rynex-Editor"
  $<$<CONFIG:Debug>:Rynex>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Rynex-Editor" PROPERTIES COMPILE_FLAGS "-finput-charset=UTF-8 ")
endif()
target_compile_options("Rynex-Editor" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Rynex-Editor" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
