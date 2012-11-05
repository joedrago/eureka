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

static ekU32 make_array(struct ekContext *Y, ekU32 argCount)
{
    ekValue *a;
    a = ekValueCreateArray(Y);
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(Y, i, argCount);
            ekValueArrayPush(Y, a, v);
            ekValueAddRefNote(Y, v, "make_array push");
        }
        ekContextPopValues(Y, argCount);
    }
    ekArrayPush(Y, &Y->stack, a);
    return 1;
}

ekU32 array_push(struct ekContext *Y, ekU32 argCount)
{
    int i;
    ekValue *a;
    ekValue **values = NULL;
    if(!ekContextGetArgs(Y, argCount, "a.", &a, &values))
    {
        return ekContextArgsFailure(Y, argCount, "push([array] a, ... values)");
    }

    for(i=0; i<ekArraekSize(Y, &values); i++)
    {
        ekValue *v = (ekValue *)values[i];
        ekValueArrayPush(Y, a, v);
    }

    ekValueRemoveRefNote(Y, a, "array_push a done");
    ekArrayDestroy(Y, &values, NULL);
    return 0;
}

ekU32 length(struct ekContext *Y, ekU32 argCount)
{
    ekValue *a;
    ekValue *c = ekValueNullPtr;
    if(!ekContextGetArgs(Y, argCount, "a", &a))
    {
        return ekContextArgsFailure(Y, argCount, "length([array] a)");
    }

    c = ekValueCreateInt(Y, ekArraekSize(Y, &a->arrayVal));
    ekValueRemoveRefNote(Y, a, "length a done");
    ekArrayPush(Y, &Y->stack, c);
    return 1;
}

static void ekAppendKey(struct ekContext *Y, ekValue *arrayVal, ekMapEntry *entry)
{
    ekValue *keyVal = ekValueCreateString(Y, entry->keyStr);
    ekValueArrayPush(Y, arrayVal, keyVal);
}

static ekU32 keys(struct ekContext *Y, ekU32 argCount)
{
    ekValue *object;
    ekValue *arrayVal = ekValueCreateArray(Y);

    if(!ekContextGetArgs(Y, argCount, "o", &object))
    {
        return ekContextArgsFailure(Y, argCount, "keys([map/object] o)");
    }

    ekMapIterateP1(Y, object->objectVal->hash, ekAppendKey, arrayVal);

    ekValueRemoveRefNote(Y, object, "keys object done");
    ekArrayPush(Y, &Y->stack, arrayVal);
    return 1;
}

static ekU32 eval(struct ekContext *Y, ekU32 argCount)
{
    ekValue *ret = NULL;
    if(argCount)
    {
        int i;
        ekValue *v = ekContextGetArg(Y, 0, argCount);
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(Y, i, argCount);
            if(v->type == YVT_STRING)
            {
                ekContextEval(Y, ekStringSafePtr(&v->stringVal), 0);
                if(Y->error)
                {
                    // steal the error from the VM so we can recover and THEN give it back as a ekValue
                    char *error = Y->error;
                    Y->error = NULL;

                    ekContextRecover(Y);
                    ret = ekValueCreateString(Y, error);
                    ekFree(error);
                }
            }
        }
        ekContextPopValues(Y, argCount);
    }
    if(!ret)
    {
        ret = ekValueCreateInt(Y, 0);
    }
    ekArrayPush(Y, &Y->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

static ekU32 standard_print(struct ekContext *Y, ekU32 argCount)
{
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(Y, i, argCount);
            switch(v->type)
            {
                case YVT_STRING:
                    printf("%s", ekStringSafePtr(&v->stringVal));
                    break;
                case YVT_INT:
                    printf("%d", v->intVal);
                    break;
                case YVT_FLOAT:
                    printf("%f", v->floatVal);
                    break;
                default:
                    v = ekValueToString(Y, v);
                    printf("%s", ekStringSafePtr(&v->stringVal));
                    ekValueRemoveRefNote(Y, v, "done with temp string (default)");
                    break;
            };
        }
        ekContextPopValues(Y, argCount);
    }
    else
    {
        printf("(printing nothing)\n");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// import()

static char *loadFile(struct ekContext *Y, const char *filename)
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

static ekU32 import(struct ekContext *Y, ekU32 argCount)
{
    ekU32 ret;
    char *code;
    ekValue *filenameValue = NULL;
    ekValue *codeValue = NULL;

    if(argCount)
    {
        filenameValue = ekContextGetArg(Y, 0, argCount);
        if(filenameValue->type != YVT_STRING)
        {
            filenameValue = NULL;
        }
    }

    if(filenameValue)
    {
        code = loadFile(Y, ekStringSafePtr(&filenameValue->stringVal));
    }

    ekContextPopValues(Y, argCount);

    if(!filenameValue)
    {
        ekValue *reason = ekValueCreateString(Y, "import() takes a single string argument");
        ekArrayPush(Y, &Y->stack, reason);
        return 1;
    }

    if(!code)
    {
        ekValue *reason = ekValueCreateString(Y, "can't read file");
        ekArrayPush(Y, &Y->stack, reason);
        return 1;
    }

    codeValue = ekValueCreateKString(Y, code);
    ekArrayPush(Y, &Y->stack, codeValue);
    ret = eval(Y, 1);
    ekFree(code);
    return ret;
}

// ---------------------------------------------------------------------------

ekU32 type(struct ekContext *Y, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(Y, 0, argCount);
        ekValue *ret = ekValueCreateKString(Y, (char *)ekValueTypeName(Y, a->type));
        ekContextPopValues(Y, argCount);
        ekArrayPush(Y, &Y->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 dump(struct ekContext *Y, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(Y, 0, argCount);
        ekDumpParams *params = ekDumpParamsCreate(Y);
        ekValue *ret = ekValueCreateKString(Y, "");

        ekValueDump(Y, params, a);
        ekStringDonateStr(Y, &ret->stringVal, &params->output);

        ekDumpParamsDestroy(Y, params);
        ekContextPopValues(Y, argCount);
        ekArrayPush(Y, &Y->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 ek_assert(struct ekContext *Y, ekU32 argCount)
{
    ekBool doAssert = ekTrue;
    ekValue *v = NULL;
    ekValue *s = NULL;
    if(!ekContextGetArgs(Y, argCount, "|?s", &v, &s))
    {
        return ekContextArgsFailure(Y, argCount, "assert(expr, [string] explanation)");
    }

    if(v)
    {
        v = ekValueToInt(Y, v);
        doAssert = (!v->intVal) ? ekTrue : ekFalse;
        ekValueRemoveRefNote(Y, v, "doAssert temp");
    }

    if(doAssert)
    {
        const char *reason = "(unknown)";
        if(s && s->type == YVT_STRING)
        {
            reason = ekStringSafePtr(&s->stringVal);
        }
        ekContextSetError(Y, YVE_RUNTIME, "Eureka Runtime Assert: %s", reason);
    }
    if(s)
    {
        ekValueRemoveRefNote(Y, s, "temporary reason string");
    }
    return 0;
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegisterCore(struct ekContext *Y)
{
    ekContextRegisterGlobalFunction(Y, "array", make_array);
    ekContextRegisterGlobalFunction(Y, "length", length);
    ekContextRegisterGlobalFunction(Y, "push", array_push);

    ekContextRegisterGlobalFunction(Y, "keys", keys);
    ekContextRegisterGlobalFunction(Y, "eval", eval);
    ekContextRegisterGlobalFunction(Y, "type", type);
    ekContextRegisterGlobalFunction(Y, "dump", dump);
    ekContextRegisterGlobalFunction(Y, "assert", ek_assert);

    // TODO: Move these out of here
    ekContextRegisterGlobalFunction(Y, "print", standard_print);
    ekContextRegisterGlobalFunction(Y, "import", import);
}
