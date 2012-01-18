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
#include "yapVM.h"

#include <stdio.h>

static yU32 make_array(struct yapVM *vm, yU32 argCount)
{
    yapValue *a;
    a = yapValueArrayCreate(vm);
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapVMGetArg(vm, i, argCount);
            yapValueArrayPush(vm, a, v);
        }
        yapVMPopValues(vm, argCount);
    }
    yapArrayPush(&vm->stack, a);
    return 1;
}

yU32 array_push(struct yapVM *vm, yU32 argCount)
{
    int i;
    yapValue *a;
    yapArray values = {0};
    if(!yapVMGetArgs(vm, argCount, "a.", &a, &values))
        return yapVMArgsFailure(vm, argCount, "push([array] a, ... values)");

    for(i=0; i<values.count; i++)
    {
        yapValue *v = (yapValue *)values.data[i];
        yapValueArrayPush(vm, a, v);
    }

    yapArrayClear(&values, NULL);
    return 0;
}

yU32 array_length(struct yapVM *vm, yU32 argCount)
{
    yapValue *a;
    yapValue *c = yapValueNullPtr;
    if(!yapVMGetArgs(vm, argCount, "a", &a))
        return yapVMArgsFailure(vm, argCount, "length([array] a)");

    c = yapValueSetInt(vm, yapValueAcquire(vm), a->arrayVal->count);
    yapArrayPush(&vm->stack, c);
    return 1;
}

static yU32 make_object(struct yapVM *vm, yU32 argCount)
{
    yapValue *v;
    v = yapValueObjectCreate(vm, NULL, argCount);
    yapArrayPush(&vm->stack, v);
    return 1;
}

struct keyIterateInfo
{
    struct yapVM *vm;
    struct yapValue *arrayVal;
};

static void yapAppendKey(struct keyIterateInfo *info, yapHashEntry *entry)
{
    yapValue *keyVal = yapValueSetString(info->vm, yapValueAcquire(info->vm), entry->key);
    yapValueArrayPush(info->vm, info->arrayVal, keyVal);
}

static yU32 keys(struct yapVM *vm, yU32 argCount)
{
    yapValue *object;
    struct keyIterateInfo info;

    if(!yapVMGetArgs(vm, argCount, "o", &object))
        return yapVMArgsFailure(vm, argCount, "keys([object/dict] o)");

    info.vm = vm;
    info.arrayVal = yapValueArrayCreate(vm);
    yapHashIterateP1(object->objectVal->hash, (yapIterateCB1)yapAppendKey, &info);

    yapArrayPush(&vm->stack, info.arrayVal);
    return 1;
}

static yU32 super(struct yapVM *vm, yU32 argCount)
{
    yapObject *object = NULL;
    if(argCount)
    {
        yapValue *arg = yapVMGetArg(vm, 0, argCount);
        if(arg->type == YVT_REF)
            arg = *arg->refVal;
        if(arg->type == YVT_OBJECT)
            object = arg->objectVal;
    }

    yapVMPopValues(vm, argCount);

    if(object && object->isa)
    {
        yapArrayPush(&vm->stack, object->isa);
    }
    else
    {
        yapArrayPush(&vm->stack, &yapValueNull);
    }

    return 1;
}

static yU32 eval(struct yapVM *vm, yU32 argCount)
{
    yapValue *ret = NULL;
    if(argCount)
    {
        int i;
        yapValue *v = yapVMGetArg(vm, 0, argCount);
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapVMGetArg(vm, i, argCount);
            if(v->type == YVT_STRING)
            {
                yapVMEval(vm, yapStringSafePtr(&v->stringVal), 0);
                if(vm->error)
                {
                    // steal the error from the VM so we can recover and THEN give it back as a yapValue
                    char *error = vm->error;
                    vm->error = NULL;

                    yapVMRecover(vm);
                    ret = yapValueSetString(vm, yapValueAcquire(vm), error);
                    yapFree(error);
                }
            }
        }
        yapVMPopValues(vm, argCount);
    }
    if(!ret)
    {
        ret = yapValueSetInt(vm, yapValueAcquire(vm), 0);
    }
    yapArrayPush(&vm->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

static yU32 standard_print(struct yapVM *vm, yU32 argCount)
{
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapVMGetArg(vm, i, argCount);
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
                v = yapValueToString(vm, v);
                printf("%s", yapStringSafePtr(&v->stringVal));
                break;
            };
        }
        yapVMPopValues(vm, argCount);
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

static yU32 import(struct yapVM *vm, yU32 argCount)
{
    yU32 ret;
    char *code;
    yapValue *filenameValue = NULL;
    yapValue *codeValue = NULL;

    if(argCount)
    {
        filenameValue = yapVMGetArg(vm, 0, argCount);
        if(filenameValue->type != YVT_STRING)
            filenameValue = NULL;
    }

    if(filenameValue)
        code = loadFile(yapStringSafePtr(&filenameValue->stringVal));

    yapVMPopValues(vm, argCount);

    if(!filenameValue)
    {
        yapValue *reason = yapValueSetString(vm, yapValueAcquire(vm), "import() takes a single string argument");
        yapArrayPush(&vm->stack, reason);
        return 1;
    }

    if(!code)
    {
        yapValue *reason = yapValueSetString(vm, yapValueAcquire(vm), "can't read file");
        yapArrayPush(&vm->stack, reason);
        return 1;
    }

    codeValue = yapValueSetKString(vm, yapValueAcquire(vm), code);
    yapArrayPush(&vm->stack, codeValue);
    ret = eval(vm, 1);
    yapFree(code);
    return ret;
}

// ---------------------------------------------------------------------------

yU32 type(struct yapVM *vm, yU32 argCount)
{
    if(argCount)
    {
        yapValue *a = yapVMGetArg(vm, 0, argCount);
        yapValue *ret = yapValueSetKString(vm, yapValueAcquire(vm), (char *)yapValueTypeName(vm, a->type));
        yapVMPopValues(vm, argCount);
        yapArrayPush(&vm->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

void yapIntrinsicsRegister(struct yapVM *vm)
{
    yapVMRegisterGlobalFunction(vm, "array", make_array);
    yapVMRegisterGlobalFunction(vm, "length", array_length);
    yapVMRegisterGlobalFunction(vm, "push", array_push);

    yapVMRegisterGlobalFunction(vm, "object", make_object);
    yapVMRegisterGlobalFunction(vm, "dict", make_object); // alias
    yapVMRegisterGlobalFunction(vm, "keys", keys);
    yapVMRegisterGlobalFunction(vm, "super", super);
    yapVMRegisterGlobalFunction(vm, "eval", eval);
    yapVMRegisterGlobalFunction(vm, "type", type);

    // TODO: Move these out of here
    yapVMRegisterGlobalFunction(vm, "print", standard_print);
    yapVMRegisterGlobalFunction(vm, "import", import);
}
