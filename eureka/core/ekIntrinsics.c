// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekIntrinsics.h"

#include "ekTypes.h"
#include "ekFrame.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekContext.h"

#include <stdio.h>

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

static ekU32 inherit(struct ekContext *E, ekU32 argCount)
{
    ekValue *v;
    v = ekValueCreateObject(E, NULL, argCount, ekTrue);
    ekArrayPush(E, &E->stack, v);
    return 1;
}

static ekU32 prototype(struct ekContext *E, ekU32 argCount)
{
    ekValue *object = NULL;
    ekValue *newPrototype = NULL;
    if(!ekContextGetArgs(E, argCount, "o|o", &object, &newPrototype))
    {
        return ekContextArgsFailure(E, argCount, "prototype(object [, newPrototypetype])");
    }

    if(object && newPrototype)
    {
        if(object->objectVal->isa)
        {
            ekValueRemoveRefNote(E, object->objectVal->isa, "prototype removing old isa");
        }
        object->objectVal->isa = newPrototype;
        ekValueAddRefNote(E, object->objectVal->isa, "prototype new isa");
    }

    if(object && object->objectVal->isa)
    {
        ekValueAddRefNote(E, object->objectVal->isa, "prototype return isa");
        ekArrayPush(E, &E->stack, object->objectVal->isa);
    }
    else
    {
        ekArrayPush(E, &E->stack, &ekValueNull);
    }
    ekValueRemoveRefNote(E, object, "prototype object done");
    if(newPrototype)
    {
        ekValueRemoveRefNote(E, newPrototype, "prototype newPrototype done");
    }
    return 1;
}

// ---------------------------------------------------------------------------

static ekU32 convert_to_string(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "string(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToString(E, v));
    return 1;
}

static ekU32 convert_to_int(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "int(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToInt(E, v));
    return 1;
}

static ekU32 convert_to_float(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "float(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToFloat(E, v));
    return 1;
}

// ---------------------------------------------------------------------------

static ekU32 pairs_iterator(struct ekContext *E, ekU32 argCount)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekValue *m;
    ekValue *keys;
    ekValue *index;
    ekValue *v;
    ekAssert(frame->closure && frame->closure->closureVars);
    m = ekMapGetS2P(E, frame->closure->closureVars, "map");
    keys = ekMapGetS2P(E, frame->closure->closureVars, "keys");
    index = ekMapGetS2P(E, frame->closure->closureVars, "index");
    ekAssert((m->type == EVT_OBJECT) && (keys->type == EVT_ARRAY) && (index->type == EVT_INT));
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    if(index->intVal < ekArraySize(E, &keys->arrayVal))
    {
        ekValue *key = keys->arrayVal[index->intVal++];
        ekValueAddRefNote(E, key, "pairs_iterator using key");
        ekArrayPush(E, &E->stack, key);
        v = ekValueIndex(E, m, key, ekFalse); // should addref the value
        if(!v)
        {
            v = ekValueNullPtr;
        }
        ekArrayPush(E, &E->stack, v);
        return 2;
    }

    ekArrayPush(E, &E->stack, ekValueNullPtr);
    return 1;
}

static ekU32 pairs_create(struct ekContext *E, ekU32 argCount)
{
    ekValue *m = NULL;
    ekValue *closure;
    ekValue *keys;
    if(!ekContextGetArgs(E, argCount, "m", &m))
    {
        return ekContextArgsFailure(E, argCount, "pairs(map)");
    }
    closure = ekValueCreateCFunction(E, pairs_iterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    keys = ekValueCreateArray(E);
    ekMapIterateP1(E, m->objectVal->hash, ekAppendKey, keys);
    ekMapGetS2P(E, closure->closureVars, "map") = m;
    ekMapGetS2P(E, closure->closureVars, "keys") = keys;
    ekMapGetS2P(E, closure->closureVars, "index") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}


// ---------------------------------------------------------------------------

static ekU32 iter_iterator(struct ekContext *E, ekU32 argCount)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekValue *array;
    ekValue *index;
    ekAssert(frame->closure && frame->closure->closureVars);
    array = ekMapGetS2P(E, frame->closure->closureVars, "array");
    index = ekMapGetS2P(E, frame->closure->closureVars, "index");
    ekAssert((array->type == EVT_ARRAY) && (index->type == EVT_INT));
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    if(index->intVal < ekArraySize(E, &array->arrayVal))
    {
        ekValue *v = array->arrayVal[index->intVal];
        ekValueAddRefNote(E, v, "array_iterator using value");
        ekArrayPush(E, &E->stack, v);
        ekArrayPush(E, &E->stack, ekValueCreateInt(E, index->intVal++));
        return 2;
    }

    ekArrayPush(E, &E->stack, ekValueNullPtr);
    return 1;
}

static ekU32 iter_create(struct ekContext *E, ekU32 argCount)
{
    ekValue *a = NULL;
    ekValue *closure;
    if(!ekContextGetArgs(E, argCount, "a", &a))
    {
        return ekContextArgsFailure(E, argCount, "iter(array)");
    }
    closure = ekValueCreateCFunction(E, iter_iterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    ekMapGetS2P(E, closure->closureVars, "array") = a;
    ekMapGetS2P(E, closure->closureVars, "index") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

static ekU32 iterator(struct ekContext *E, ekU32 argCount)
{
    return ekContextIterOp(E, argCount);
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegister(struct ekContext *E)
{
    ekContextRegisterGlobalFunction(E, "iter", iter_create);
    ekContextRegisterGlobalFunction(E, "pairs", pairs_create);

    ekContextRegisterGlobalFunction(E, "iterator", iterator);

    ekContextRegisterGlobalFunction(E, "keys", keys);
    ekContextRegisterGlobalFunction(E, "eval", eval);
    ekContextRegisterGlobalFunction(E, "type", type);
    ekContextRegisterGlobalFunction(E, "dump", dump);
    ekContextRegisterGlobalFunction(E, "assert", ek_assert);

    ekContextRegisterGlobalFunction(E, "inherit", inherit);
    ekContextRegisterGlobalFunction(E, "prototype", prototype);

    ekContextRegisterGlobalFunction(E, "string", convert_to_string);
    ekContextRegisterGlobalFunction(E, "int", convert_to_int);
    ekContextRegisterGlobalFunction(E, "float", convert_to_float);

    // TODO: Move these out of here
    ekContextRegisterGlobalFunction(E, "print", standard_print);
    ekContextRegisterGlobalFunction(E, "import", import);
}
