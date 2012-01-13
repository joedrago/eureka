cmake_minimum_required(VERSION 2.6)

project(yap)

set(YAP_MOD_JSON "ON" CACHE BOOL "Enables JSON support.")
if(YAP_MOD_JSON)
    add_definitions(-DYAP_MOD_JSON=1)
endif()

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

