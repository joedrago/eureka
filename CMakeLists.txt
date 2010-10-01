cmake_minimum_required(VERSION 2.6)

project(yap)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_C_FLAGS "-g")
endif()

if(MSVC)
	set(CMAKE_C_FLAGS /wd4996)
endif()

include_directories(${CMAKE_SOURCE_DIR}/yap)

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

add_subdirectory(lemon)
add_subdirectory(yap)
add_subdirectory(yaptest)

