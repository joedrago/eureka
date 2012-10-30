# ---------------------------------------------------------------------------
#                   Copyright Joe Drago 2010 - 2011.
#         Distributed under the Boost Software License, Version 1.0.
#            (See accompanying file LICENSE_1_0.txt or copy at
#                  http:#www.boost.org/LICENSE_1_0.txt)
# ---------------------------------------------------------------------------

project(ektest C)

include_directories (
    ${ek_SOURCE_DIR}/core
    ${ek_SOURCE_DIR}/intrinsics
    ${ek_SOURCE_DIR}/modules
    ${ek_SOURCE_DIR}/extras
)

add_executable(ektest
    main.c
)
target_link_libraries (ektest eureka ${EUREKA_MOD_LIBS})
