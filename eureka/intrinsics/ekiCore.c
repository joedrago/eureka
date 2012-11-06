// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekiCore.h"

#include "ekTypes.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekContext.h"

#include <stdio.h>

static ekU32 make_array(struct ekContext *E, ekU32 argCount)
{
    ekValue *a;
    a = ekValueCreateArray(E);
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(E, i, argCount);
            ekValueArrayPush(E, a, v);
            ekValueAddRefNote(E, v, "make_array push");
        }
        ekContextPopValues(E, argCount);
    }
    ekArrayPush(E, &E->stack, a);
    return 1;
}

ekU32 array_push(struct ekContext *E, ekU32 argCount)
{
    int i;
    ekValue *a;
    ekValue **values = NULL;
    if(!ekContextGetArgs(E, argCount, "a.", &a, &values))
    {
        return ekContextArgsFailure(E, argCount, "push([array] a, ... values)");
    }

    for(i=0; i<ekArraySize(E, &values); i++)
    {
        ekValue *v = (ekValue *)values[i];
        ekValueArrayPush(E, a, v);
    }

    ekValueRemoveRefNote(E, a, "array_push a done");
    ekArrayDestroy(E, &values, NULL);
    return 0;
}

static void ekAppendKey(struct ekContext *E, ekValue *arrayVal, ekMapEntry *entry)
{
    ekValue *keyVal = ekValueCreateString(E, entry->keyStr);
    ekValueArrayPush(E, arrayVal, keyVal);
}

static ekU32 keys(struct ekContext *E, ekU32 argCount)
{
    ekValue *object;
    ekValue *arrayVal = ekValueCreateArray(E);

    if(!ekContextGetArgs(E, argCount, "o", &object))
    {
        return ekContextArgsFailure(E, argCount, "keys([map/object] o)");
    }

    ekMapIterateP1(E, object->objectVal->hash, ekAppendKey, arrayVal);

    ekValueRemoveRefNote(E, object, "keys object done");
    ekArrayPush(E, &E->stack, arrayVal);
    return 1;
}

static ekU32 eval(struct ekContext *E, ekU32 argCount)
{
    ekValue *ret = NULL;
    if(argCount)
    {
        int i;
        ekValue *v = ekContextGetArg(E, 0, argCount);
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(E, i, argCount);
            if(v->type == EVT_STRING)
            {
                ekContextEval(E, ekStringSafePtr(&v->stringVal), 0);
                if(E->error)
                {
                    // steal the error from the VM so we can recover and THEN give it back as a ekValue
                    char *error = E->error;
                    E->error = NULL;

                    ekContextRecover(E);
                    ret = ekValueCreateString(E, error);
                    ekFree(error);
                }
            }
        }
        ekContextPopValues(E, argCount);
    }
    if(!ret)
    {
        ret = ekValueCreateInt(E, 0);
    }
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

static ekU32 standard_print(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(E, i, argCount);
            switch(v->type)
            {
                case EVT_STRING:
                    printf("%s", ekStringSafePtr(&v->stringVal));
                    break;
                case EVT_INT:
                    printf("%d", v->intVal);
                    break;
                case EVT_FLOAT:
                    printf("%f", v->floatVal);
                    break;
                default:
                    v = ekValueToString(E, v);
                    printf("%s", ekStringSafePtr(&v->stringVal));
                    ekValueRemoveRefNote(E, v, "done with temp string (default)");
                    break;
            };
        }
        ekContextPopValues(E, argCount);
    }
    else
    {
        printf("(printing nothing)\n");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// import()

static char *loadFile(struct ekContext *E, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char *)ekAlloc(size + 1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }
    return NULL;
}

static ekU32 import(struct ekContext *E, ekU32 argCount)
{
    ekU32 ret;
    char *code;
    ekValue *filenameValue = NULL;
    ekValue *codeValue = NULL;

    if(argCount)
    {
        filenameValue = ekContextGetArg(E, 0, argCount);
        if(filenameValue->type != EVT_STRING)
        {
            filenameValue = NULL;
        }
    }

    if(filenameValue)
    {
        code = loadFile(E, ekStringSafePtr(&filenameValue->stringVal));
    }

    ekContextPopValues(E, argCount);

    if(!filenameValue)
    {
        ekValue *reason = ekValueCreateString(E, "import() takes a single string argument");
        ekArrayPush(E, &E->stack, reason);
        return 1;
    }

    if(!code)
    {
        ekValue *reason = ekValueCreateString(E, "can't read file");
        ekArrayPush(E, &E->stack, reason);
        return 1;
    }

    codeValue = ekValueCreateKString(E, code);
    ekArrayPush(E, &E->stack, codeValue);
    ret = eval(E, 1);
    ekFree(code);
    return ret;
}

// ---------------------------------------------------------------------------

ekU32 type(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(E, 0, argCount);
        ekValue *ret = ekValueCreateKString(E, (char *)ekValueTypeName(E, a->type));
        ekContextPopValues(E, argCount);
        ekArrayPush(E, &E->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 dump(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(E, 0, argCount);
        ekDumpParams *params = ekDumpParamsCreate(E);
        ekValue *ret = ekValueCreateKString(E, "");

        ekValueDump(E, params, a);
        ekStringDonateStr(E, &ret->stringVal, &params->output);

        ekDumpParamsDestroy(E, params);
        ekContextPopValues(E, argCount);
        ekArrayPush(E, &E->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 ek_assert(struct ekContext *E, ekU32 argCount)
{
    ekBool doAssert = ekTrue;
    ekValue *v = NULL;
    ekValue *s = NULL;
    if(!ekContextGetArgs(E, argCount, "|?s", &v, &s))
    {
        return ekContextArgsFailure(E, argCount, "assert(expr, [string] explanation)");
    }

    if(v)
    {
        v = ekValueToInt(E, v);
        doAssert = (!v->intVal) ? ekTrue : ekFalse;
        ekValueRemoveRefNote(E, v, "doAssert temp");
    }

    if(doAssert)
    {
        const char *reason = "(unknown)";
        if(s && s->type == EVT_STRING)
        {
            reason = ekStringSafePtr(&s->stringVal);
        }
        ekContextSetError(E, EVE_RUNTIME, "Eureka Runtime Assert: %s", reason);
    }
    if(s)
    {
        ekValueRemoveRefNote(E, s, "temporary reason string");
    }
    return 0;
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegisterCore(struct ekContext *E)
{
    ekContextRegisterGlobalFunction(E, "array", make_array);
    ekContextRegisterGlobalFunction(E, "push", array_push);

    ekContextRegisterGlobalFunction(E, "keys", keys);
    ekContextRegisterGlobalFunction(E, "eval", eval);
    ekContextRegisterGlobalFunction(E, "type", type);
    ekContextRegisterGlobalFunction(E, "dump", dump);
    ekContextRegisterGlobalFunction(E, "assert", ek_assert);

    // TODO: Move these out of here
    ekContextRegisterGlobalFunction(E, "print", standard_print);
    ekContextRegisterGlobalFunction(E, "import", import);
}
