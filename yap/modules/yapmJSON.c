// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapmJSON.h"

#include "yapValue.h"
#include "yapContext.h"

#include <stdlib.h>
#include "cJSON.h"

static yapValue *jsonRecurse(struct yapContext *Y, cJSON *json)
{
    cJSON *child;
    yapValue *ret = yapValueNullPtr;
    switch(json->type)
    {
        case cJSON_False:
        {
            ret = yapValueCreateInt(Y, 0);
        }
        break;

        case cJSON_True:
        {
            ret = yapValueCreateInt(Y, 1);
        }
        break;

        case cJSON_NULL:
        {
            // ret is already null
        }
        break;

        case cJSON_Number:
        {
            ret = yapValueCreateFloat(Y, json->valuedouble);
        }
        break;

        case cJSON_String:
        {
            ret = yapValueCreateString(Y, json->valuestring);
        }
        break;

        case cJSON_Array:
        {
            int i;
            int size = cJSON_GetArraySize(json);
            ret = yapValueCreateArray(Y);
            child = json->child;
            while(child)
            {
                yap2ArrayPush(Y, &ret->arrayVal, jsonRecurse(Y, child));
                child = child->next;
            }
        }
        break;

        case cJSON_Object:
        {
            ret = yapValueCreateObject(Y, NULL, 0, yFalse);
            child = json->child;
            while(child)
            {
                yapValueObjectSetMember(Y, ret, child->string, jsonRecurse(Y, child));
                child = child->next;
            }
        }
        break;

    };
    return ret;
}

static yU32 json_parse(struct yapContext *Y, yU32 argCount)
{
    yapValue *ret = yapValueNullPtr;
    cJSON *json;
    yapValue *jsonValue;

    if(!yapContextGetArgs(Y, argCount, "s", &jsonValue))
    {
        return yapContextArgsFailure(Y, argCount, "json_parse([string] json)");
    }

    json = cJSON_Parse(yapStringSafePtr(&jsonValue->stringVal));
    if(json)
    {
        ret = jsonRecurse(Y, json);
        cJSON_Delete(json);
    }

    yap2ArrayPush(Y, &Y->stack, ret);
    return 1;
}

void yapModuleRegisterJSON(struct yapContext *Y)
{
    cJSON_Hooks hooks = { yapAlloc, yapFree };
    cJSON_InitHooks(&hooks);

    yapContextRegisterGlobalFunction(Y, "json_parse", json_parse);
}
