cmake_minimum_required(VERSION 2.6)

project(yap)

# ----------------------------------------------------------------------------

set(YAP_ENABLE_MOD_JSON "ON" CACHE BOOL "Enables JSON support.")
if(YAP_ENABLE_MOD_JSON)
    add_definitions(-DYAP_ENABLE_MOD_JSON=1)
endif()

set(YAP_ENABLE_EXT_DISASM "ON" CACHE BOOL "Enables Yap disassembly support.")
if(YAP_ENABLE_EXT_DISASM)
    add_definitions(-DYAP_ENABLE_EXT_DISASM=1)
endif()

set(YAP_ENABLE_EXT_DOT "ON" CACHE BOOL "Enables Yap Graphviz (Dot) output support.")
if(YAP_ENABLE_EXT_DOT)
    add_definitions(-DYAP_ENABLE_EXT_DOT=1)
endif()

# ----------------------------------------------------------------------------

if(UNIX)
    set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_C_FLAGS "-g")
endif()

if(MSVC)
    set(CMAKE_C_FLAGS /wd4996)
endif()

if(APPLE)
    add_definitions(-DPLATFORM_OSX)
else()
    if(UNIX)
        add_definitions(-DPLATFORM_LINUX)
    endif()

    if(MSVC)
        add_definitions(-DPLATFORM_WIN32)
    endif()
endif()

add_subdirectory(external)
add_subdirectory(yap)
add_subdirectory(demos)

