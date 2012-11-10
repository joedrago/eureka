// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekmJSON.h"

#include "ekValue.h"
#include "ekContext.h"

#include <stdlib.h>
#include "cJSON.h"

static ekValue *jsonRecurse(struct ekContext *E, cJSON *json)
{
    cJSON *child;
    ekValue *ret = ekValueNullPtr;
    switch(json->type)
    {
        case cJSON_False:
        {
            ret = ekValueCreateInt(E, 0);
        }
        break;

        case cJSON_True:
        {
            ret = ekValueCreateInt(E, 1);
        }
        break;

        case cJSON_NULL:
        {
            // ret is already null
        }
        break;

        case cJSON_Number:
        {
            ret = ekValueCreateFloat(E, json->valuedouble);
        }
        break;

        case cJSON_String:
        {
            ret = ekValueCreateString(E, json->valuestring);
        }
        break;

        case cJSON_Array:
        {
            int i;
            int size = cJSON_GetArraySize(json);
            ret = ekValueCreateArray(E);
            child = json->child;
            while(child)
            {
                ekArrayPush(E, &ret->arrayVal, jsonRecurse(E, child));
                child = child->next;
            }
        }
        break;

        case cJSON_Object:
        {
            ret = ekValueCreateObject(E, NULL, 0, ekFalse);
            child = json->child;
            while(child)
            {
                ekValueObjectSetMember(E, ret, child->string, jsonRecurse(E, child));
                child = child->next;
            }
        }
        break;

    };
    return ret;
}

static ekU32 json_parse(struct ekContext *E, ekU32 argCount)
{
    ekValue *ret = ekValueNullPtr;
    cJSON *json;
    ekValue *jsonValue;

    if(!ekContextGetArgs(E, argCount, "s", &jsonValue))
    {
        return ekContextArgsFailure(E, argCount, "json_parse([string] json)");
    }

    json = cJSON_Parse(ekStringSafePtr(&jsonValue->stringVal));
    if(json)
    {
        ret = jsonRecurse(E, json);
        cJSON_Delete(json);
    }

    ekArrayPush(E, &E->stack, ret);
    return 1;
}

void ekModuleRegisterJSON(struct ekContext *E)
{
    cJSON_Hooks hooks = { ekAlloc, ekFree };
    cJSON_InitHooks(&hooks);

    ekContextAddIntrinsic(E, "json_parse", json_parse);
}
