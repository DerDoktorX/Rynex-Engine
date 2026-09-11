
# --- Multi-Config (Visual Studio) -------------------------------------------
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;RelInfoEasy;RelWithDebInfo;Release" CACHE STRING "Build-Configuration" FORCE)
endif()

# --- Single-Config Fallback (Ninja/MinGW) -----------------------------------
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build-Configuration" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "RelInfoEasy" "RelWithDebInfo" "Release")
    
    
    
endif()

# ============================================================================
# RelInfoEasy = Premake "Release"  (-O2 + symbol, NO -DNDEBUG)
#
# Names rule: CMAKE_<LANG>_FLAGS_<TOUPPER(config)>
#   string(TOUPPER "RelInfoEasy") -> "RELINFOEASAY"
# ============================================================================
if(MSVC)
    set(CMAKE_C_FLAGS_RELINFOEASY   "/O2 /Ob1 /Zi" CACHE STRING "C-Flags for RelInfoEasy"   FORCE)
    set(CMAKE_CXX_FLAGS_RELINFOEASY "/O2 /Ob1 /Zi" CACHE STRING "CXX-Flags for RelInfoEasy" FORCE)
    
    set(CMAKE_EXE_LINKER_FLAGS_RELINFOEASY    "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}"    CACHE STRING "Linker-Flags for RelInfoEasy"         FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_RELINFOEASY "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "Shared-Linker-Flags for RelInfoEasy"  FORCE)
    set(CMAKE_STATIC_LINKER_FLAGS_RELINFOEASY "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "Static-Linker-Flags for RelInfoEasy"  FORCE)
    
else()
    # GCC / Clang (MinGW, Linux)
    set(CMAKE_C_FLAGS_RELINFOEASY   "-O2 -g" CACHE STRING "C-Flags for RelInfoEasy"   FORCE)
    set(CMAKE_CXX_FLAGS_RELINFOEASY "-O2 -g" CACHE STRING "CXX-Flags for RelInfoEasy" FORCE)
    
    set(CMAKE_EXE_LINKER_FLAGS_RELINFOEASY    "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}"    CACHE STRING "Linker-Flags for RelInfoEasy"         FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_RELINFOEASY "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "Shared-Linker-Flags for RelInfoEasy"  FORCE)
    set(CMAKE_STATIC_LINKER_FLAGS_RELINFOEASY "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "Static-Linker-Flags for RelInfoEasy"  FORCE)
    
endif()
message(" Set ub c: ${CMAKE_C_FLAGS_RELINFOEASY} | c++: ${CMAKE_CXX_FLAGS_RELINFOEASY} | build type: ${CMAKE_CONFIGURATION_TYPES}")