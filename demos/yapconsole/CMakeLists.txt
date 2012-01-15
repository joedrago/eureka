
project(yapconsole C)

include_directories (
    ${yap_SOURCE_DIR}/core
    ${yap_SOURCE_DIR}/intrinsics
    ${yap_SOURCE_DIR}/modules
    ${yap_SOURCE_DIR}/extras
)

add_executable(yapconsole
    main.c
)
target_link_libraries (yapconsole ${YAP_MOD_LIBS} yap)

