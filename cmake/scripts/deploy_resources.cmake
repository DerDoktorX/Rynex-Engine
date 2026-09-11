# ============================================================================
# cmake/scripts/deploy_resources.cmake
#
# Wird zur POST_BUILD-Zeit aufgerufen via:
#   cmake -P deploy_resources.cmake -DBUILD_CONFIG=... -D...
#
# Parameter (alle per -D):
#   BUILD_CONFIG          Debug | RelInfoEasy | RelWithDebInfo | Release
#   RESOURCE_SOURCE_DIR   Absoluter Pfad des Quellprojekt-Ordners
#   TARGET_DIR            Absoluter Pfad des Executable-Ausgabeordners
#   LINK_DIRS_STR         Pipe-getrennte (|) Ordnernamen → linken oder kopieren
#   LINK_FILES_STR        Pipe-getrennte (|) Dateinamen  → linken oder kopieren
#   MAKE_DIRS_STR         Pipe-getrennte (|) Ordnernamen → immer anlegen
#
# Verhalten je Config:
#   Debug / RelInfoEasy / RelWithDebInfo  -->  Junction (Win) / Symlink (Unix)
#   Release  (= Premake Dist)             -->  Physische Kopie
# ============================================================================

foreach(_req BUILD_CONFIG RESOURCE_SOURCE_DIR TARGET_DIR)
    if(NOT ${_req})
        message(FATAL_ERROR "[deploy_resources] Fehlender Parameter: ${_req}")
    endif()
endforeach()

message(STATUS "[ResourceDeploy] ---- ${BUILD_CONFIG} ----")
message(STATUS "[ResourceDeploy] src : ${RESOURCE_SOURCE_DIR}")
message(STATUS "[ResourceDeploy] dst : ${TARGET_DIR}")

# Pipe-Strings -> CMake-Listen
string(REPLACE "|" ";" LINK_DIRS  "${LINK_DIRS_STR}")
string(REPLACE "|" ";" LINK_FILES "${LINK_FILES_STR}")
string(REPLACE "|" ";" MAKE_DIRS  "${MAKE_DIRS_STR}")

# Folder create (DLLs/, Profile/ etc.)
foreach(_dir IN LISTS MAKE_DIRS)
    if(_dir)
        file(MAKE_DIRECTORY "${TARGET_DIR}/${_dir}")
        message(STATUS "[ResourceDeploy] mkdir   ${_dir}/")
    endif()
endforeach()

# Release (= Dist) -> Copy, alle everything else -> linken
if(BUILD_CONFIG STREQUAL "Release")
    set(_do_copy TRUE)
else()
    set(_do_copy FALSE)
endif()

# ===========================================================================
# Verzeichnisse deployen
# ===========================================================================
foreach(_name IN LISTS LINK_DIRS)
    if(NOT _name)
        continue()
    endif()
    
    set(_src "${RESOURCE_SOURCE_DIR}/${_name}")
    set(_dst "${TARGET_DIR}/${_name}")
    
    if(NOT IS_DIRECTORY "${_src}")
        message(WARNING "[ResourceDeploy] Source not found: ${_src}")
        continue()
    endif()
    
    if(_do_copy)
        # Release: recursive Copy
        if(EXISTS "${_dst}")
            file(REMOVE_RECURSE "${_dst}")
        endif()
        file(COPY "${_src}" DESTINATION "${TARGET_DIR}")
        message(STATUS "[ResourceDeploy] COPY     ${_name}/")
    else()
        if(EXISTS "${_dst}" OR IS_SYMLINK "${_dst}")
            message(STATUS "[ResourceDeploy] SKIP     ${_name}/ (existiert bereits)")
        elseif(CMAKE_HOST_WIN32)
            # /J = Directory Junction, no Admin neassery
            # execute_process(
            #       COMMAND cmd /c mklink /J "${_dst}" "${_src}"
            #       RESULT_VARIABLE _rc
            #       OUTPUT_QUIET ERROR_QUIET
            # )
            
            ADD_CUSTOM_TARGET(
                  link_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ${target} ${link}
                  RESULT_VARIABLE _rc
            )
            
           
        else()
            execute_process(
                  COMMAND "${CMAKE_COMMAND}" -E create_symlink "${_src}" "${_dst}"
                  RESULT_VARIABLE _rc
            )
            if(_rc)
                message(WARNING "[ResourceDeploy] symlink ERROR: ${_name}/")
            else()
                message(STATUS "[ResourceDeploy] SYMLINK  ${_name}/")
            endif()
        endif()
    endif()
endforeach()

# ===========================================================================
# Single File deployed
# ===========================================================================
foreach(_fname IN LISTS LINK_FILES)
    if(NOT _fname)
        continue()
    endif()
    
    set(_src "${RESOURCE_SOURCE_DIR}/${_fname}")
    set(_dst "${TARGET_DIR}/${_fname}")
    
    if(NOT EXISTS "${_src}")
        message(WARNING "[ResourceDeploy] Source not found: ${_src}")
        continue()
    endif()
    
    if(_do_copy)
        file(COPY "${_src}" DESTINATION "${TARGET_DIR}")
        message(STATUS "[ResourceDeploy] COPY     ${_fname}")
    else()
        if(EXISTS "${_dst}" OR IS_SYMLINK "${_dst}")
            message(STATUS "[ResourceDeploy] SKIP     ${_fname} (exist already)")
        elseif(CMAKE_HOST_WIN32)
            # /H = Hard Link, no Admin necessary (need to be one the same drive)
            execute_process(
                  COMMAND cmd /c mklink /H "${_dst}" "${_src}"
                  RESULT_VARIABLE _rc
                  OUTPUT_QUIET ERROR_QUIET
            )
            if(_rc)
                # Fallback in case hard drive
                file(COPY "${_src}" DESTINATION "${TARGET_DIR}")
                message(STATUS "[ResourceDeploy] COPY     ${_fname} (hardlink fallback)")
            else()
                message(STATUS "[ResourceDeploy] HARDLINK ${_fname}")
            endif()
        else()
            execute_process(
                  COMMAND "${CMAKE_COMMAND}" -E create_symlink "${_src}" "${_dst}"
                  RESULT_VARIABLE _rc
            )
            if(_rc)
                message(WARNING "[ResourceDeploy] symlink ERROR: ${_fname}")
            else()
                message(STATUS "[ResourceDeploy] SYMLINK  ${_fname}")
            endif()
        endif()
    endif()
endforeach()

message(STATUS "[ResourceDeploy] Ready.")