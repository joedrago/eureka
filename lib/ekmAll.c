// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekmAll.h"

#ifdef EUREKA_ENABLE_MOD_JSON
#include "ekmJSON.h"
#endif

#ifdef EUREKA_ENABLE_MOD_REGEX
#include "ekmRegex.h"
#endif

#ifdef EUREKA_ENABLE_MOD_FILE
#include "ekmFile.h"
#endif

void ekModuleRegisterAll(struct ekContext *E)
{
#ifdef EUREKA_ENABLE_MOD_JSON
    ekModuleRegisterJSON(E);
#endif
#ifdef EUREKA_ENABLE_MOD_REGEX
    ekModuleRegisterRegex(E);
#endif
#ifdef EUREKA_ENABLE_MOD_FILE
    ekModuleRegisterFile(E);
#endif
}
