cmake_minimum_required(VERSION 2.6)

project(yap)

# ----------------------------------------------------------------------------

set(YAP_MOD_LIBS ";")

set(YAP_ENABLE_MOD_JSON "ON" CACHE BOOL "Enables JSON support.")
if(YAP_ENABLE_MOD_JSON)
    add_definitions(-DYAP_ENABLE_MOD_JSON=1)
    list(APPEND YAP_MOD_LIBS cjson)
endif()

set(YAP_ENABLE_MOD_PCRE "ON" CACHE BOOL "Enables PCRE support.")
if(YAP_ENABLE_MOD_PCRE)
    add_definitions(-DYAP_ENABLE_MOD_PCRE=1 -DPCRE_STATIC=1)
    list(APPEND YAP_MOD_LIBS pcre)
endif()

set(YAP_ENABLE_EXT_DISASM "ON" CACHE BOOL "Enables Yap disassembly support.")
if(YAP_ENABLE_EXT_DISASM)
    add_definitions(-DYAP_ENABLE_EXT_DISASM=1)
endif()

set(YAP_ENABLE_EXT_DOT "ON" CACHE BOOL "Enables Yap Graphviz (Dot) output support.")
if(YAP_ENABLE_EXT_DOT)
    add_definitions(-DYAP_ENABLE_EXT_DOT=1)
endif()

set(YAP_RE2C_PATH "re2c" CACHE STRING "Path to re2c executable.")

if(UNIX)
    list(APPEND YAP_MOD_LIBS m)
endif()

# ----------------------------------------------------------------------------

if(UNIX)
    set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_C_FLAGS "-g")

    set(COVERAGE OFF CACHE BOOL "Enable coverage")
    if(COVERAGE)
        MESSAGE(STATUS "Coverage enabled.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0")
        set(COVERAGE_HTML_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage" CACHE STRING "Coverage HTML output")
        MESSAGE(STATUS "Coverage HTML Output: ${COVERAGE_HTML_OUTPUT_DIR}")

        add_definitions(-DYAP_TRACE_MEMORY=1)
        add_definitions(-DYAP_TRACE_MEMORY_STATS_ONLY=1)
        add_definitions(-DYAP_TRACE_EXECUTION=1)
        add_definitions(-DYAP_TRACE_REFS=1)
    else()
        MESSAGE(STATUS "Coverage disabled.")
    endif()
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

add_custom_target(tests
    COMMAND perl ${CMAKE_SOURCE_DIR}/scripts/tests.pl "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
    DEPENDS yaptest
)

if(COVERAGE)
    add_custom_target(coverage_clean
        COMMAND lcov -z -d ${CMAKE_BINARY_DIR} -o ${CMAKE_BINARY_DIR}/yap.info
    )
    add_dependencies(tests coverage_clean)
    add_custom_target(coverage
        COMMAND lcov -c -d ${CMAKE_BINARY_DIR} -o ${CMAKE_BINARY_DIR}/yap.info > ${CMAKE_BINARY_DIR}/lcov.ouptut
        COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND genhtml --highlight --legend --output-directory ${COVERAGE_HTML_OUTPUT_DIR} ${CMAKE_BINARY_DIR}/yap.info > ${CMAKE_BINARY_DIR}/genhtml.output
        DEPENDS tests
    )
endif()
