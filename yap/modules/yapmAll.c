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
