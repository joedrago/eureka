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

#ifdef EUREKA_ENABLE_MOD_PCRE
#include "ekmPCRE.h"
#endif

void ekModuleRegisterAll(struct ekContext *Y)
{
#ifdef EUREKA_ENABLE_MOD_JSON
    ekModuleRegisterJSON(Y);
#endif
#ifdef EUREKA_ENABLE_MOD_PCRE
    ekModuleRegisterPCRE(Y);
#endif
}
