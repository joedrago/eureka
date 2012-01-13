#include "yapmAll.h"

#ifdef YAP_MOD_JSON
#include "yapmJSON.h"
#endif

void yapModuleRegisterAll(struct yapVM *vm)
{
#ifdef YAP_MOD_JSON
    yapModuleRegisterJSON(vm);
#endif
}
