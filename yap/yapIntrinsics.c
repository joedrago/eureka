#include "yapIntrinsics.h"

#include "yapTypes.h"
#include "yapObject.h"
#include "yapValue.h"
#include "yapVM.h"

#include <stdio.h>

yU32 make_array(struct yapVM *vm, yU32 argCount)
{
    yapValue *v;
    yapVMPopValues(vm, argCount);
    v = yapValueArrayCreate(vm);
    yapArrayPush(&vm->stack, v);
    return 1;
}

yU32 array_push(struct yapVM *vm, yU32 argCount)
{
    if(argCount)
    {
        int i;
        yapValue *a = yapVMGetArg(vm, 0, argCount);
        for(i = 1; i < argCount; i++)
        {
            yapValue *v = yapVMGetArg(vm, i, argCount);
            yapValueArrayPush(vm, a, v);
        }
        yapVMPopValues(vm, argCount);
    }
    return 0;
}

yU32 array_length(struct yapVM *vm, yU32 argCount)
{
    if(argCount)
    {
        yapValue *a = yapVMGetArg(vm, 0, argCount);
        yapValue *c = yapValueSetInt(vm, yapValueAcquire(vm), a->arrayVal->count);
        yapVMPopValues(vm, argCount);
        yapArrayPush(&vm->stack, c);
    }
    return 1;
}

yU32 make_object(struct yapVM *vm, yU32 argCount)
{
    yapValue *v;
    yapVMPopValues(vm, argCount);
    v = yapValueObjectCreate(vm, NULL);
    yapArrayPush(&vm->stack, v);
    return 1;
}

yU32 super(struct yapVM *vm, yU32 argCount)
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

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

yU32 standard_print(struct yapVM *vm, yU32 argCount)
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
                printf("%s", v->stringVal);
                break;
            case YVT_INT:
                printf("%d", v->intVal);
                break;
            default:
                printf("***hurrrrr***");
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

void yapIntrinsicsRegister(struct yapVM *vm)
{
    yapVMRegisterIntrinsic(vm, "push", array_push);
    yapVMRegisterIntrinsic(vm, "array", make_array);
    yapVMRegisterIntrinsic(vm, "length", array_length);
    yapVMRegisterIntrinsic(vm, "object", make_object);
    yapVMRegisterIntrinsic(vm, "super", super);

    // TODO: Move this out of here
    yapVMRegisterIntrinsic(vm, "print", standard_print);
}
