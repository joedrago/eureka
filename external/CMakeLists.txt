cmake_minimum_required(VERSION 2.8)

add_subdirectory(lemon)
if(YAP_ENABLE_MOD_JSON)
    add_subdirectory(cJSON)
endif()
