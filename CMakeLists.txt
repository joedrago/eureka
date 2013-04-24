cmake_minimum_required(VERSION 2.6)

project(eureka)

# ----------------------------------------------------------------------------

set(EUREKA_COMMON_LIBS ";")

set(EUREKA_ENABLE_MOD_JSON "ON" CACHE BOOL "Enables JSON support.")
if(EUREKA_ENABLE_MOD_JSON)
    add_definitions(-DEUREKA_ENABLE_MOD_JSON=1)
    list(APPEND EUREKA_COMMON_LIBS cjson)
endif()

set(EUREKA_ENABLE_MOD_PCRE "ON" CACHE BOOL "Enables PCRE support.")
if(EUREKA_ENABLE_MOD_PCRE)
    add_definitions(-DEUREKA_ENABLE_MOD_PCRE=1 -DPCRE_STATIC=1)
    list(APPEND EUREKA_COMMON_LIBS pcre)
endif()

set(EUREKA_ENABLE_MOD_FILE "ON" CACHE BOOL "Enables file support.")
if(EUREKA_ENABLE_MOD_FILE)
    add_definitions(-DEUREKA_ENABLE_MOD_FILE=1)
endif()

set(EUREKA_ENABLE_EXT_DISASM "ON" CACHE BOOL "Enables Eureka disassembly support.")
if(EUREKA_ENABLE_EXT_DISASM)
    add_definitions(-DEUREKA_ENABLE_EXT_DISASM=1)
endif()

set(EUREKA_ENABLE_EXT_DOT "ON" CACHE BOOL "Enables Eureka Graphviz (Dot) output support.")
if(EUREKA_ENABLE_EXT_DOT)
    add_definitions(-DEUREKA_ENABLE_EXT_DOT=1)
endif()

set(EUREKA_RE2C_PATH "re2c" CACHE STRING "Path to re2c executable.")

# ----------------------------------------------------------------------------

set(TEST_TRACING OFF CACHE BOOL "Turn on traces suitable for running make tests")

if(UNIX)
    set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_C_FLAGS "-g")

    set(COVERAGE OFF CACHE BOOL "Enable coverage")
    if(COVERAGE)
        MESSAGE(STATUS "Coverage enabled.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0")
        set(COVERAGE_HTML_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage" CACHE STRING "Coverage HTML output")
        MESSAGE(STATUS "Coverage HTML Output: ${COVERAGE_HTML_OUTPUT_DIR}")

        set(TEST_TRACING ON)
    else()
        MESSAGE(STATUS "Coverage disabled.")
    endif()
endif()

if(TEST_TRACING)
    add_definitions(-DEUREKA_TRACE_MEMORY=1)
    add_definitions(-DEUREKA_TRACE_MEMORY_STATS_ONLY=1)
    add_definitions(-DEUREKA_TRACE_EXECUTION=1)
    add_definitions(-DEUREKA_TRACE_REFS=1)
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

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext/pcre/cmake) # for FindReadline.cmake
find_package( Readline )

if(UNIX)
    set(EUREKA_COMMON_LIBS ${EUREKA_COMMON_LIBS} m)
endif()

if(READLINE_FOUND)
    add_definitions(-DHAVE_READLINE=1)
    include_directories(${READLINE_INCLUDE_DIR})
    set(EUREKA_COMMON_LIBS ${EUREKA_COMMON_LIBS} ${READLINE_LIBRARY})
endif()

add_subdirectory(ext)

include_directories(lib)
add_subdirectory(lib)
add_subdirectory(eureka)
add_subdirectory(tools)

add_custom_target(tests
    COMMAND perl ${CMAKE_SOURCE_DIR}/tools/scripts/tests.pl "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
    DEPENDS ektest
)

add_custom_target(dot
    COMMAND perl ${CMAKE_SOURCE_DIR}/tools/scripts/tests.pl "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" dot
    DEPENDS ektest
)

if(UNIX)
    add_custom_target(astyle
        COMMAND bash ${CMAKE_SOURCE_DIR}/tools/scripts/astyleAll.sh
    )
endif()

set(DOCTXT)
file(GLOB_RECURSE DOCSRC ${CMAKE_SOURCE_DIR}/docs/src/*.ekd)
foreach(dsrc ${DOCSRC})
    get_filename_component(basename "${dsrc}" NAME_WE)
    set(ddst "${CMAKE_SOURCE_DIR}/docs/${basename}.txt")
    add_custom_command(OUTPUT ${ddst}
        COMMAND ekdoc text "${dsrc}" > "${ddst}"
        DEPENDS ${dsrc}
        DEPENDS ekdoc
    )
    LIST(APPEND DOCTXT ${ddst})
    set(ddst "${CMAKE_BINARY_DIR}/wordpress/${basename}.wordpress")
    add_custom_command(OUTPUT ${ddst}
        COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/wordpress
        COMMAND ekdoc wordpress "${dsrc}" > "${ddst}"
        DEPENDS ${dsrc}
        DEPENDS ekdoc
    )
    LIST(APPEND DOCTXT ${ddst})
endforeach()

add_custom_target(docs
    ALL DEPENDS ${DOCTXT}
)

if(COVERAGE)
    add_custom_target(coverage_clean
        COMMAND lcov -z -d ${CMAKE_BINARY_DIR} -o ${CMAKE_BINARY_DIR}/eureka.info
    )
    add_dependencies(tests coverage_clean)
    add_custom_target(coverage
        COMMAND lcov -c -d ${CMAKE_BINARY_DIR} -o ${CMAKE_BINARY_DIR}/eureka.info > ${CMAKE_BINARY_DIR}/lcov.ouptut
        COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND genhtml --highlight --legend --output-directory ${COVERAGE_HTML_OUTPUT_DIR} ${CMAKE_BINARY_DIR}/eureka.info > ${CMAKE_BINARY_DIR}/genhtml.output
        DEPENDS tests
    )
endif()
