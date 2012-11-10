// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValueType.h"

#include "ekContext.h"
#include "ekMap.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------

ekValueType *ekValueTypeCreate(struct ekContext *E, const char *name)
{
    ekValueType *type = ekAlloc(sizeof(ekValueType));
    strcpy(type->name, name);
    return type;
}

void ekValueTypeDestroy(struct ekContext *E, ekValueType *type)
{
    if(type->funcDestroyUserData)
    {
        type->funcDestroyUserData(E, type);
    }
    if(type->intrinsics)
    {
        ekMapDestroy(E, type->intrinsics, NULL);
    }
    ekFree(type);
}

void ekValueTypeAddIntrinsic(struct ekContext *E, ekValueType *type, const char *name, ekCFunction func)
{
    if(!type->intrinsics)
    {
        type->intrinsics = ekMapCreate(E, EMKT_STRING);
    }
    ekMapGetS2P(E, type->intrinsics, name) = func;
}

struct ekValue *ekValueTypeGetIntrinsic(struct ekContext *E, ekU32 type, struct ekValue *index, ekBool lvalue)
{
    ekMap *map = E->types[type]->intrinsics;
    if(map && index && (index->type == EVT_STRING))
    {
        ekMapEntry *mapEntry = ekMapGetS(E, map, ekStringSafePtr(&index->stringVal), ekFalse);
        if(mapEntry && mapEntry->valuePtr)
        {
            if(lvalue)
            {
                ekContextSetError(E, EVE_RUNTIME, "cannot use intrinsic %s function as an lvalue", ekValueTypeName(E, type));
            }
            else
            {
                return ekValueCreateCFunction(E, mapEntry->valuePtr);
            }
        }
    }
    return NULL;
}

ekS32 ekValueTypeRegister(struct ekContext *E, ekValueType *newType)
{
    ekS32 i;
    for(i=0; i<ekArraySize(E, &E->types); i++)
    {
        ekValueType *t = E->types[i];
        if(!strcmp(newType->name, t->name))
        {
            return EVT_INVALID;
        }
    }

    // If you are hitting one of these asserts, then your custom type isn't handling a required function.
    // If you're a C++ person, pretend the compiler is telling you that you forgot to implement a pure virtual.
    // If you don't want to do anything for a particular function, explicitly set it to ekValueTypeFuncNotUsed.
    ekAssert(newType->funcClear);
    ekAssert(newType->funcClone);
    ekAssert(newType->funcToBool);
    ekAssert(newType->funcToInt);
    ekAssert(newType->funcToFloat);
    ekAssert(newType->funcToString);
    ekAssert(newType->funcIter);
    ekAssert(newType->funcArithmetic);
    ekAssert(newType->funcCmp);
    ekAssert(newType->funcIndex);
    ekAssert(newType->funcDump);
    ekAssert(newType->funcDump != ekValueTypeFuncNotUsed); // required!

    newType->id = ekArrayPush(E, &E->types, newType);
    return newType->id;
}

// ---------------------------------------------------------------------------

void ekValueTypeRegisterAllBasicTypes(struct ekContext *E)
{
    ekValueTypeRegisterNull(E);
    ekValueTypeRegisterBlock(E);
    ekValueTypeRegisterCFunction(E);
    ekValueTypeRegisterInt(E);
    ekValueTypeRegisterFloat(E);
    ekValueTypeRegisterString(E);
    ekValueTypeRegisterArray(E);
    ekValueTypeRegisterObject(E);
    ekValueTypeRegisterRef(E);
}
