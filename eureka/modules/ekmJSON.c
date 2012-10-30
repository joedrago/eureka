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

static ekValue *jsonRecurse(struct ekContext *Y, cJSON *json)
{
    cJSON *child;
    ekValue *ret = ekValueNullPtr;
    switch(json->type)
    {
        case cJSON_False:
        {
            ret = ekValueCreateInt(Y, 0);
        }
        break;

        case cJSON_True:
        {
            ret = ekValueCreateInt(Y, 1);
        }
        break;

        case cJSON_NULL:
        {
            // ret is already null
        }
        break;

        case cJSON_Number:
        {
            ret = ekValueCreateFloat(Y, json->valuedouble);
        }
        break;

        case cJSON_String:
        {
            ret = ekValueCreateString(Y, json->valuestring);
        }
        break;

        case cJSON_Array:
        {
            int i;
            int size = cJSON_GetArraySize(json);
            ret = ekValueCreateArray(Y);
            child = json->child;
            while(child)
            {
                ekArrayPush(Y, &ret->arrayVal, jsonRecurse(Y, child));
                child = child->next;
            }
        }
        break;

        case cJSON_Object:
        {
            ret = ekValueCreateObject(Y, NULL, 0, yFalse);
            child = json->child;
            while(child)
            {
                ekValueObjectSetMember(Y, ret, child->string, jsonRecurse(Y, child));
                child = child->next;
            }
        }
        break;

    };
    return ret;
}

static yU32 json_parse(struct ekContext *Y, yU32 argCount)
{
    ekValue *ret = ekValueNullPtr;
    cJSON *json;
    ekValue *jsonValue;

    if(!ekContextGetArgs(Y, argCount, "s", &jsonValue))
    {
        return ekContextArgsFailure(Y, argCount, "json_parse([string] json)");
    }

    json = cJSON_Parse(ekStringSafePtr(&jsonValue->stringVal));
    if(json)
    {
        ret = jsonRecurse(Y, json);
        cJSON_Delete(json);
    }

    ekArrayPush(Y, &Y->stack, ret);
    return 1;
}

void ekModuleRegisterJSON(struct ekContext *Y)
{
    cJSON_Hooks hooks = { ekAlloc, ekFree };
    cJSON_InitHooks(&hooks);

    ekContextRegisterGlobalFunction(Y, "json_parse", json_parse);
}
