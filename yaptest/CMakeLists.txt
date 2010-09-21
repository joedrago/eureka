
project(yaptest C)
include_directories (${yap_SOURCE_DIR}) 
add_executable(yaptest 
    main.c 
)
target_link_libraries (yaptest yap) 

