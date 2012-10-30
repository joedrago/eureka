cmake_minimum_required(VERSION 2.8)

add_subdirectory(lemon)
if(EUREKA_ENABLE_MOD_JSON)
    add_subdirectory(cJSON)
endif()

if(EUREKA_ENABLE_MOD_PCRE)
    set(PCRE_MINIMAL_DEFAULT "OFF")
    add_subdirectory(pcre)
endif()
