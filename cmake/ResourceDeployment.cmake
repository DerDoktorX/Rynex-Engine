# ============================================================================
# cmake/ResourceDeployment.cmake
#
# Stellt bereit:
#   rynex_deploy_resources(<target>
#       SOURCE_DIR  <absoluter-pfad>
#       [LINK_DIRS  <ordner1> <ordner2> ...]   # Junction/Symlink oder Kopie
#       [LINK_FILES <datei1>  <datei2>  ...]   # Hardlink/Symlink oder Kopie
#       [MAKE_DIRS  <ordner1> <ordner2> ...]   # Immer anlegen (z.B. DLLs/)
#   )
# ============================================================================

set(_RYNEX_DEPLOY_SCRIPT
      "${CMAKE_CURRENT_LIST_DIR}/scripts/deploy_resources.cmake"
      CACHE INTERNAL "Pfad zum Resource-Deployment-Script")

function(rynex_deploy_resources target)
    cmake_parse_arguments(_RD
          ""
          "SOURCE_DIR"
          "LINK_DIRS;LINK_FILES;MAKE_DIRS"
          ${ARGN}
    )
    
    if(NOT _RD_SOURCE_DIR)
        message(FATAL_ERROR "[rynex_deploy_resources] SOURCE_DIR missing for target '${target}'")
    endif()
    
    # Lists withe '|' connected – avoid Semicolon-Escaping-Problem in COMMAND
    string(JOIN "|" _dirs_str   ${_RD_LINK_DIRS})
    string(JOIN "|" _files_str  ${_RD_LINK_FILES})
    string(JOIN "|" _mkdirs_str ${_RD_MAKE_DIRS})
    
    # add_custom_command(
    #       TARGET  ${target}
    #       POST_BUILD
    #       VERBATIM
    #       COMMAND ${CMAKE_COMMAND}
    #       "-DBUILD_CONFIG=$<CONFIG>"
    #       "-DRESOURCE_SOURCE_DIR=${_RD_SOURCE_DIR}"
    #       "-DTARGET_DIR=$<TARGET_FILE_DIR:${target}>"
    #       "-DLINK_DIRS_STR=${_dirs_str}"
    #       "-DLINK_FILES_STR=${_files_str}"
    #       "-DMAKE_DIRS_STR=${_mkdirs_str}"
    #       -P "${_RYNEX_DEPLOY_SCRIPT}"
    #       COMMENT "Deploying resources for ${target} [$<CONFIG>]..."
    # )
    
    message(STATUS "[ResourceDeployment] Register for: ${target}")
    
    set(_bin_dir     "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/${_folder}")
    
    if(_RD_LINK_DIRS)
        message(STATUS "    LINK_DIRS  : ${_RD_LINK_DIRS}")
        
        foreach (FILE_LINK  ${_RD_LINK_FILES})
            message("LinkDir ${FILE_LINK}: ${CMAKE_CURRENT_LIST_DIR}/${FILE_LINK}  ${_bin_dir}${FILE_LINK}")
            add_custom_target(${target} ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}/${FILE_LINK}" "${_bin_dir}${FILE_LINK}")
        endforeach()
    endif()
    if(_RD_LINK_FILES)
        message(STATUS "    LINK_FILES : ${_RD_LINK_FILES}")
        
        foreach (DIR_LINK ${_RD_LINK_DIRS})
            message("LinkDir ${DIR_LINK}: ${CMAKE_CURRENT_LIST_DIR}/${DIR_LINK}  ${_bin_dir}${DIR_LINK}")
            add_custom_target(${target} ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}/${DIR_LINK}" "${_bin_dir}${DIR_LINK}")
        endforeach()
    endif()
    if(_RD_MAKE_DIRS)
        message(STATUS "    MAKE_DIRS  : ${_RD_MAKE_DIRS}")
        
        foreach (DIR_CREATE ${_RD_MAKE_DIRS})
            message("LinkDir ${DIR_CREATE}: ${CMAKE_CURRENT_LIST_DIR}/${DIR_CREATE} ${_bin_dir}${DIR_CREATE}")
            add_custom_target(${target} ALL COMMAND ${_RD_MAKE_DIRS} -E create_symlink "${CMAKE_CURRENT_LIST_DIR}${D/IR_CREATE}" "${_bin_dir}${_RD_MAKE_DIRS}")
        endforeach()
    endif()

endfunction()