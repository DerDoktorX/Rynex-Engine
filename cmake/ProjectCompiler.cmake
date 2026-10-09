

# for compiler and std libery equal c++ version
function(SET_CPP_COMPILER_VERSION_PROJECT project version)
    set_target_properties(${project} PROPERTIES CXX_STANDARD ${version})
    if(MSVC)
        set(CPP_VERSION_COMPILE_VERSION_OPTIONS /std:c++${version})
        target_compile_options(${project} PRIVATE ${CPP_VERSION_COMPILE_VERSION_OPTIONS})
    else()
        # for other Compiler (GCC, Clang) functionary Default-Way
        set(CPP_VERSION_COMPILE_VERSION_OPTIONS -std=c++${version})
        target_compile_options(${project} PRIVATE ${CPP_VERSION_COMPILE_VERSION_OPTIONS})
    endif()
    set(CPP_VERSION_COMPILE_VERSION_FEATURES cxx_std_${version})
    target_compile_features(${project} PRIVATE ${CPP_VERSION_COMPILE_VERSION_FEATURES})
    
    target_compile_definitions(${project} PRIVATE RY_CPP_${CPP_VERSION_USING})
    message("Project ${project} setup with c++ ${version}")
endfunction()



# for binary setup off target
set(RYNEX_BINARY_TARGET "${RYNEX_COMPILER_NAME}_$<CONFIG>")

set(RYNEX_ROOT_PROJECT                                  ${CMAKE_CURRENT_LIST_DIR})
set(RYNEX_PROJECT_BINARY_FOLDER_OUTPUT                  ${RYNEX_ROOT_PROJECT}/bin/${RYNEX_BINARY_TARGET})
set(RYNEX_PROJECT_LIB_FOLDER_OUTPUT                     ${RYNEX_ROOT_PROJECT}/lib/${RYNEX_BINARY_TARGET})
set(RYNEX_PROJECT_BINARY_INTERMEDIATE_FOLDER_OUTPUT     ${RYNEX_ROOT_PROJECT}/bin-int/${RYNEX_BINARY_TARGET})
set(RYNEX_PROJECT_ENGINE_RESOURCES_OUTPUT               ${RYNEX_PROJECT_BINARY_FOLDER_OUTPUT}/Engine-Resources)

function(SET_BINARY_RESOURCE target)
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