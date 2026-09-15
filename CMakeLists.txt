
cmake_minimum_required(VERSION 4.3)


project(Rynex-Rendering)

include(cmake/FetchExternalLiberys.cmake)


if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")
endif()

set(RYNEX_BINARY_TARGET "$<CONFIG>")
# set(RYNEX_BINARY_TARGET "[CONFIG]")
set(RYNEX_ROOT_PROJECT                                  ${CMAKE_CURRENT_LIST_DIR})
set(RYNEX_PROJECT_BINARY_FOLDER_OUTPUT                  ${RYNEX_ROOT_PROJECT}/bin/${RYNEX_BINARY_TARGET})
set(RYNEX_PROJECT_LIB_FOLDER_OUTPUT                     ${RYNEX_ROOT_PROJECT}/lib/${RYNEX_BINARY_TARGET})
set(RYNEX_PROJECT_BINARY_INTERMEDIATE_FOLDER_OUTPUT     ${RYNEX_ROOT_PROJECT}/bin-int/${RYNEX_BINARY_TARGET})
set(RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT               ${RYNEX_PROJECT_BINARY_FOLDER_OUTPUT}/Engine-Resources)


function(setupBinary target)
    
    set_target_properties(
          ${target}
          PROPERTIES
          ARCHIVE_OUTPUT_DIRECTORY    ${RYNEX_PROJECT_BINARY_FOLDER_OUTPUT}/${target}
          RUNTIME_OUTPUT_DIRECTORY    ${RYNEX_PROJECT_BINARY_FOLDER_OUTPUT}/${target}
          # EXECUTABLE_OUTPUT_PATH    ${RYNEX_PROJECT_BINARY_FOLDER_OUTPUT}/${target}
          LIBRARY_OUTPUT_DIRECTORY    ${RYNEX_PROJECT_LIB_FOLDER_OUTPUT}/${target}
    )

    add_custom_command(
          TARGET  ${target}
          PRE_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}"
          COMMAND ${CMAKE_COMMAND} -E make_directory  "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/Profile"
    )
    if(true)
         add_custom_command(
               TARGET  ${target}
               PRE_BUILD
               COMMAND ${CMAKE_COMMAND} -E create_symlink "${RYNEX_ROOT_PROJECT}/Rynex-Editor/Editor-Assets"    "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/Editor-Assets"
               COMMAND ${CMAKE_COMMAND} -E create_symlink "${RYNEX_ROOT_PROJECT}/Rynex-Editor/Resources"        "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/Resources"
               COMMAND ${CMAKE_COMMAND} -E create_symlink "${RYNEX_ROOT_PROJECT}/Rynex-Editor/SandboxProject"   "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/SandboxProject"
               COMMAND ${CMAKE_COMMAND} -E create_symlink "${RYNEX_ROOT_PROJECT}/Rynex-Editor/mono"             "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/mono"
               COMMAND ${CMAKE_COMMAND} -E create_symlink "${RYNEX_ROOT_PROJECT}/Rynex-Editor/imgui.ini"        "${RYNEX_PROJECT_BINARY_FOLDER_OUTPUT}/imgui.ini"
        )
    else()
        add_custom_command(
              TARGET  ${target}
              PRE_BUILD
              COMMAND ${CMAKE_COMMAND} -E copy "${RYNEX_ROOT_PROJECT}/Rynex-Editor/Editor-Assets"    "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/Editor-Assets"
              COMMAND ${CMAKE_COMMAND} -E copy "${RYNEX_ROOT_PROJECT}/Rynex-Editor/Resources"        "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/Resources"
              COMMAND ${CMAKE_COMMAND} -E copy "${RYNEX_ROOT_PROJECT}/Rynex-Editor/SandboxProject"   "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/SandboxProject"
              COMMAND ${CMAKE_COMMAND} -E copy "${RYNEX_ROOT_PROJECT}/Rynex-Editor/mono"             "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/mono"
              COMMAND ${CMAKE_COMMAND} -E copy "${RYNEX_ROOT_PROJECT}/Rynex-Editor/imgui.ini"        "${RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT}/imgui.ini"
        )
        
    endif ()
endfunction()




add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex/vendor/Glad/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex/vendor/stb_image/)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Sandbox)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Rynex-Editor)

message("cxx dbg flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message("cxx flags: ${CMAKE_CXX_FLAGS}")
