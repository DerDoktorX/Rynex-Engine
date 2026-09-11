# ============================================================================
# cmake/OutputDirectories.cmake
#
# Set Output Directory analog to the Premake5-Script:
#
#   Premake5:
#     target dir    "<Absolute Root Directory>/bin/<Config>-<OS>-<Arch>/<ProjectName>/"
#     objdir        "<Absolute Root Directory>/bin-int/<Config>-<OS>-<Arch>/<ProjectName>/"
#
#   CMake (this file):
#     RUNTIME / LIBRARY  ->  <ROOT>/bin/<Config>/<Target>/
#     ARCHIVE            ->  <ROOT>/bin-int/<Config>/<Target>/
#     VS_INTERMEDIATE    ->  <ROOT>/bin-int/<Config>/<Target>/   (only MSVC/VS)
#
# $<CONFIG> is a generator expression – it is evaluated only
# at build time. Correct for multi-config (Visual Studio) AND
# single-config (Ninja/MinGW) generators.
#
# Use:
#   rynex_set_output_dirs(<target>)
#
# Optional with other folder name:
#   rynex_set_output_dirs(
#                           <target>
#                           optional:
#                               ROOT            <path root directory> = ${CMAKE_SOURCE_DIR}
#                               FOLDER_NAME     <name>
# )
# ============================================================================

function(rynex_set_output_dirs target)
    set(oneValueArgs        ROOT FOLDER_NAME)
    cmake_parse_arguments(_ROD "" "${oneValueArgs}" "" ${ARGN})
    if(NOT _ROD_ROOT)
        set(_ROD_ROOT       ${CMAKE_SOURCE_DIR})
    else ()
        message("Not Default Path: ${_ROD_ROOT}")
    endif()

    if(_ROD_FOLDER_NAME)
        set(_folder "${_ROD_FOLDER_NAME}")
    else()
        set(_folder "${target}")
    endif()

    set(_bin_dir     "${_ROD_ROOT}/bin/$<CONFIG>/${_folder}")
    set(_bin_int_dir "${_ROD_ROOT}/bin-int/$<CONFIG>/${_folder}")

    
    set_target_properties(
            ${target}
            PROPERTIES
                # Executables (.exe) and DLLs (.dll / .so)
                RUNTIME_OUTPUT_DIRECTORY "${_bin_dir}"
                # Shared Libraries (.so / .dylib)
                LIBRARY_OUTPUT_DIRECTORY "${_bin_dir}"
                # Static Libraries und Import-Libs (.lib / .a)
                ARCHIVE_OUTPUT_DIRECTORY "${_bin_int_dir}"
          
                OBJECT_OUTPUTS "${_bin_int_dir}"
    )

    if(MSVC)
        set_target_properties(
                ${target}
                PROPERTIES
                    # Place PDB symbol files next to .exe / .dll
                    PDB_OUTPUT_DIRECTORY "${_bin_dir}"
              
              
        )
    endif()
    file(RELATIVE_PATH root "${_ROD_ROOT}" "${_ROD_ROOT}")
    
    get_target_property(PROP ${target} PROPERTIES)
 
    message(STATUS "[OutputDirectories] ${target}")
    message(STATUS "    bin     -> ${root}/bin/$<CONFIG>/${_folder}/")
    message(STATUS "    bin-int -> ${root}/bin-int/$<CONFIG>/${_folder}/")

endfunction()