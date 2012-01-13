#include "yapmAll.h"

#ifdef YAP_ENABLE_MOD_JSON
#include "yapmJSON.h"
#endif

void yapModuleRegisterAll(struct yapVM *vm)
{
#ifdef YAP_ENABLE_MOD_JSON
    yapModuleRegisterJSON(vm);
#endif
}
