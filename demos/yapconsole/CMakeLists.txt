
project(yapconsole C)

if(UNIX)
#    add_definitions(-DHAVE_READLINE=1)
endif()

include_directories (
    ${yap_SOURCE_DIR}/core
    ${yap_SOURCE_DIR}/intrinsics
    ${yap_SOURCE_DIR}/modules
    ${yap_SOURCE_DIR}/extras
)
add_executable(yapconsole
    main.c
)
target_link_libraries (yapconsole yap)

