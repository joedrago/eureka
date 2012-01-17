// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapmAll.h"

#ifdef YAP_ENABLE_MOD_JSON
#include "yapmJSON.h"
#endif

#ifdef YAP_ENABLE_MOD_PCRE
#include "yapmPCRE.h"
#endif

void yapModuleRegisterAll(struct yapVM *vm)
{
#ifdef YAP_ENABLE_MOD_JSON
    yapModuleRegisterJSON(vm);
#endif
#ifdef YAP_ENABLE_MOD_PCRE
    yapModuleRegisterPCRE(vm);
#endif
}
