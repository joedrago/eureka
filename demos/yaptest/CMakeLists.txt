# ---------------------------------------------------------------------------
#                   Copyright Joe Drago 2010 - 2011.
#         Distributed under the Boost Software License, Version 1.0.
#            (See accompanying file LICENSE_1_0.txt or copy at
#                  http:#www.boost.org/LICENSE_1_0.txt)
# ---------------------------------------------------------------------------

project(yaptest C)

include_directories (
    ${yap_SOURCE_DIR}/core
    ${yap_SOURCE_DIR}/intrinsics
    ${yap_SOURCE_DIR}/modules
    ${yap_SOURCE_DIR}/extras
)

add_executable(yaptest
    main.c
)
target_link_libraries (yaptest yap ${YAP_MOD_LIBS})

