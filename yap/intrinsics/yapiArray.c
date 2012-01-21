// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapiArray.h"

#include "yapTypes.h"
#include "yapHash.h"
#include "yapObject.h"
#include "yapValue.h"
#include "yapContext.h"

#include <stdio.h>

static yU32 make_array(struct yapContext *Y, yU32 argCount)
{
    yapValue *a;
    a = yapValueArrayCreate(Y);
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapContextGetArg(Y, i, argCount);
            yapValueArrayPush(Y, a, v);
        }
        yapContextPopValues(Y, argCount);
    }
    yapArrayPush(&Y->stack, a);
    return 1;
}

yU32 array_push(struct yapContext *Y, yU32 argCount)
{
    int i;
    yapValue *a;
    yapArray values = {0};
    if(!yapContextGetArgs(Y, argCount, "a.", &a, &values))
        return yapContextArgsFailure(Y, argCount, "push([array] a, ... values)");

    for(i=0; i<values.count; i++)
    {
        yapValue *v = (yapValue *)values.data[i];
        yapValueArrayPush(Y, a, v);
    }

    yapArrayClear(&values, NULL);
    return 0;
}

yU32 array_length(struct yapContext *Y, yU32 argCount)
{
    yapValue *a;
    yapValue *c = yapValueNullPtr;
    if(!yapContextGetArgs(Y, argCount, "a", &a))
        return yapContextArgsFailure(Y, argCount, "length([array] a)");

    c = yapValueSetInt(Y, yapValueAcquire(Y), a->arrayVal->count);
    yapArrayPush(&Y->stack, c);
    return 1;
}

static yU32 make_object(struct yapContext *Y, yU32 argCount)
{
    yapValue *v;
    v = yapValueObjectCreate(Y, NULL, argCount);
    yapArrayPush(&Y->stack, v);
    return 1;
}

struct keyIterateInfo
{
    struct yapContext *Y;
    struct yapValue *arrayVal;
};

static void yapAppendKey(struct keyIterateInfo *info, yapHashEntry *entry)
{
    yapValue *keyVal = yapValueSetString(info->Y, yapValueAcquire(info->Y), entry->key);
    yapValueArrayPush(info->Y, info->arrayVal, keyVal);
}

static yU32 keys(struct yapContext *Y, yU32 argCount)
{
    yapValue *object;
    struct keyIterateInfo info;

    if(!yapContextGetArgs(Y, argCount, "o", &object))
        return yapContextArgsFailure(Y, argCount, "keys([object/dict] o)");

    info.Y = Y;
    info.arrayVal = yapValueArrayCreate(Y);
    yapHashIterateP1(object->objectVal->hash, (yapIterateCB1)yapAppendKey, &info);

    yapArrayPush(&Y->stack, info.arrayVal);
    return 1;
}

static yU32 super(struct yapContext *Y, yU32 argCount)
{
    yapObject *object = NULL;
    if(argCount)
    {
        yapValue *arg = yapContextGetArg(Y, 0, argCount);
        if(arg->type == YVT_REF)
            arg = *arg->refVal;
        if(arg->type == YVT_OBJECT)
            object = arg->objectVal;
    }

    yapContextPopValues(Y, argCount);

    if(object && object->isa)
    {
        yapArrayPush(&Y->stack, object->isa);
    }
    else
    {
        yapArrayPush(&Y->stack, &yapValueNull);
    }

    return 1;
}

static yU32 eval(struct yapContext *Y, yU32 argCount)
{
    yapValue *ret = NULL;
    if(argCount)
    {
        int i;
        yapValue *v = yapContextGetArg(Y, 0, argCount);
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapContextGetArg(Y, i, argCount);
            if(v->type == YVT_STRING)
            {
                yapContextEval(Y, yapStringSafePtr(&v->stringVal), 0);
                if(Y->error)
                {
                    // steal the error from the VM so we can recover and THEN give it back as a yapValue
                    char *error = Y->error;
                    Y->error = NULL;

                    yapContextRecover(Y);
                    ret = yapValueSetString(Y, yapValueAcquire(Y), error);
                    yapFree(error);
                }
            }
        }
        yapContextPopValues(Y, argCount);
    }
    if(!ret)
    {
        ret = yapValueSetInt(Y, yapValueAcquire(Y), 0);
    }
    yapArrayPush(&Y->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

static yU32 standard_print(struct yapContext *Y, yU32 argCount)
{
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapContextGetArg(Y, i, argCount);
            switch(v->type)
            {
            case YVT_STRING:
                printf("%s", yapStringSafePtr(&v->stringVal));
                break;
            case YVT_INT:
                printf("%d", v->intVal);
                break;
            case YVT_FLOAT:
                printf("%f", v->floatVal);
                break;
            default:
                v = yapValueToString(Y, v);
                printf("%s", yapStringSafePtr(&v->stringVal));
                break;
            };
        }
        yapContextPopValues(Y, argCount);
    }
    else
    {
        printf("(printing nothing)\n");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// import()

static char *loadFile(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char *)yapAlloc(size + 1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }
    return NULL;
}

static yU32 import(struct yapContext *Y, yU32 argCount)
{
    yU32 ret;
    char *code;
    yapValue *filenameValue = NULL;
    yapValue *codeValue = NULL;

    if(argCount)
    {
        filenameValue = yapContextGetArg(Y, 0, argCount);
        if(filenameValue->type != YVT_STRING)
            filenameValue = NULL;
    }

    if(filenameValue)
        code = loadFile(yapStringSafePtr(&filenameValue->stringVal));

    yapContextPopValues(Y, argCount);

    if(!filenameValue)
    {
        yapValue *reason = yapValueSetString(Y, yapValueAcquire(Y), "import() takes a single string argument");
        yapArrayPush(&Y->stack, reason);
        return 1;
    }

    if(!code)
    {
        yapValue *reason = yapValueSetString(Y, yapValueAcquire(Y), "can't read file");
        yapArrayPush(&Y->stack, reason);
        return 1;
    }

    codeValue = yapValueSetKString(Y, yapValueAcquire(Y), code);
    yapArrayPush(&Y->stack, codeValue);
    ret = eval(Y, 1);
    yapFree(code);
    return ret;
}

// ---------------------------------------------------------------------------

yU32 type(struct yapContext *Y, yU32 argCount)
{
    if(argCount)
    {
        yapValue *a = yapContextGetArg(Y, 0, argCount);
        yapValue *ret = yapValueSetKString(Y, yapValueAcquire(Y), (char *)yapValueTypeName(Y, a->type));
        yapContextPopValues(Y, argCount);
        yapArrayPush(&Y->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

yU32 dump(struct yapContext *Y, yU32 argCount)
{
    if(argCount)
    {
        yapValue *a = yapContextGetArg(Y, 0, argCount);
        yapDumpParams *params = yapDumpParamsCreate(Y);
        yapValue *ret = yapValueSetKString(Y, yapValueAcquire(Y), "");

        yapValueDump(params, a);
        yapStringDonateStr(&ret->stringVal, &params->output);

        yapDumpParamsDestroy(params);
        yapContextPopValues(Y, argCount);
        yapArrayPush(&Y->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

void yapIntrinsicsRegister(struct yapContext *Y)
{
    yapContextRegisterGlobalFunction(Y, "array", make_array);
    yapContextRegisterGlobalFunction(Y, "length", array_length);
    yapContextRegisterGlobalFunction(Y, "push", array_push);

    yapContextRegisterGlobalFunction(Y, "object", make_object);
    yapContextRegisterGlobalFunction(Y, "dict", make_object); // alias
    yapContextRegisterGlobalFunction(Y, "keys", keys);
    yapContextRegisterGlobalFunction(Y, "super", super);
    yapContextRegisterGlobalFunction(Y, "eval", eval);
    yapContextRegisterGlobalFunction(Y, "type", type);
    yapContextRegisterGlobalFunction(Y, "dump", dump);

    // TODO: Move these out of here
    yapContextRegisterGlobalFunction(Y, "print", standard_print);
    yapContextRegisterGlobalFunction(Y, "import", import);
}
