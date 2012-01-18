// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapmJSON.h"

#include "yapValue.h"
#include "yapVM.h"

#include <stdlib.h>
#include "cJSON.h"

static yapValue *jsonRecurse(struct yapVM *vm, cJSON *json)
{
    cJSON *child;
    yapValue *ret = yapValueNullPtr;
    switch(json->type)
    {
    case cJSON_False:
    {
        ret = yapValueSetInt(vm, yapValueAcquire(vm), 0);
    }
    break;

    case cJSON_True:
    {
        ret = yapValueSetInt(vm, yapValueAcquire(vm), 1);
    }
    break;

    case cJSON_NULL:
    {
        // ret is already null
    }
    break;

    case cJSON_Number:
    {
        ret = yapValueSetFloat(vm, yapValueAcquire(vm), json->valuedouble);
    }
    break;

    case cJSON_String:
    {
        ret = yapValueSetString(vm, yapValueAcquire(vm), json->valuestring);
    }
    break;

    case cJSON_Array:
    {
        int i;
        int size = cJSON_GetArraySize(json);
        ret = yapValueArrayCreate(vm);
        child = json->child;
        while(child)
        {
            yapArrayPush(ret->arrayVal, jsonRecurse(vm, child));
            child = child->next;
        }
    }
    break;

    case cJSON_Object:
    {
        ret = yapValueObjectCreate(vm, NULL, 0);
        child = json->child;
        while(child)
        {
            yapValueObjectSetMember(vm, ret, child->string, jsonRecurse(vm, child));
            child = child->next;
        }
    }
    break;

    };
    return ret;
}

static yU32 json_parse(struct yapVM *vm, yU32 argCount)
{
    yapValue *ret = yapValueNullPtr;
    cJSON *json;
    yapValue *jsonValue;

    if(!yapVMGetArgs(vm, argCount, "s", &jsonValue))
        return yapVMArgsFailure(vm, argCount, "json_parse([string] json)");

    json = cJSON_Parse(yapStringSafePtr(&jsonValue->stringVal));
    if(json)
    {
        ret = jsonRecurse(vm, json);
        cJSON_Delete(json);
    }

    yapArrayPush(&vm->stack, ret);
    return 1;
}

void yapModuleRegisterJSON(struct yapVM *vm)
{
    cJSON_Hooks hooks = { yapAlloc, yapFree };
    cJSON_InitHooks(&hooks);

    yapVMRegisterGlobalFunction(vm, "json_parse", json_parse);
}
