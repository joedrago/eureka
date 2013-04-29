// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekContext.h"
#include "ekFrame.h"
#include "ekLexer.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

static char *NULL_STRING_FORM = "(null)";

// ---------------------------------------------------------------------------

ekValue ekValueNull = {EVT_NULL};
ekValue *ekValueNullPtr = &ekValueNull;

// ---------------------------------------------------------------------------

#ifdef EUREKA_TRACE_REFS
static ekS32 sEurekaValueDebugCount = 0;
ekS32 ekValueDebugCount()
{
    return sEurekaValueDebugCount;
}
static ekS32 sEurekaValueHighWatermark = 0;
ekS32 ekValueDebugHighWatermark()
{
    return sEurekaValueHighWatermark;
}
#endif

// ---------------------------------------------------------------------------

ekValue *ekValueCreate(ekContext *E, int type)
{
    ekValue *value;
    if(ekArraySize(E, &E->freeValues) > 0)
    {
        value = ekArrayPop(E, &E->freeValues);
        memset(value, 0, sizeof(ekValue));
        ekValueTraceRefs(E, value, 1, "ekValueCreate (reuse)");
        ekTraceValues(("ekValueCreate (reuse) %p\n", value));
    }
    else
    {
        value = ekAlloc(sizeof(ekValue));
        ekValueTraceRefs(E, value, 1, "ekValueCreate (new)");
        ekTraceValues(("ekValueCreate (new) %p\n", value));
#ifdef EUREKA_TRACE_REFS
        ++sEurekaValueDebugCount;
        if(sEurekaValueHighWatermark < sEurekaValueDebugCount)
        {
            sEurekaValueHighWatermark = sEurekaValueDebugCount;
        }
#endif
    }
    value->refs = 1;
    value->type = type;
    return value;
}

void ekValueDestroy(struct ekContext *E, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    ekValueClear(E, p);
    ekTraceValues(("ekValueDestroy %p\n", p));
    ekFree(p);
#ifdef EUREKA_TRACE_REFS
    --sEurekaValueDebugCount;
#endif
}

void ekValueClear(struct ekContext *E, ekValue *p)
{
    ekValueTypeSafeCall(p->type, Clear)(E, p);

    memset(p, 0, sizeof(*p));
    p->type = EVT_NULL;
}

void ekValueAddRef(struct ekContext *E, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    ekAssert(p->refs);
    ++p->refs;
}

void ekValueRemoveRef(struct ekContext *E, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    ekAssert(p->refs && (p->refs != 0xaaaaaaaa) && "refcount has gone rogue!");

    --p->refs;
    if(p->refs == 0)
    {
        if((E->maxFreeValues == EMFV_UNLIMITED) || (ekArraySize(E, &E->freeValues) < E->maxFreeValues))
        {
            ekTraceValues(("ekValueRemoveRef (pool) %p\n", p));
            ekValueClear(E, p);
            ekArrayPush(E, &E->freeValues, p);
        }
        else
        {
            ekTraceValues(("ekValueRemoveRef (free) %p\n", p));
            ekValueDestroy(E, p);
        }
    }
}

// ---------------------------------------------------------------------------

ekValue *ekValueCreateInt(struct ekContext *E, ekS32 v)
{
    ekValue *p = ekValueCreate(E, EVT_INT);
    p->intVal = v;
    ekTraceValues(("ekValueCreateInt %p [%d]\n", p, v));
    return p;
}

ekValue *ekValueCreateFloat(struct ekContext *E, ekF32 v)
{
    ekValue *p = ekValueCreate(E, EVT_FLOAT);
    p->floatVal = v;
    ekTraceValues(("ekValueCreateFloat %p [%f]\n", p, v));
    return p;
}

ekValue *ekValueCreateKString(struct ekContext *E, const char *s)
{
    ekValue *p = ekValueCreate(E, EVT_STRING);
    ekStringSetK(E, &p->stringVal, s);
    ekTraceValues(("ekValueCreateKString %p\n", p));
    return p;
}

ekValue *ekValueCreateString(struct ekContext *E, const char *s)
{
    ekValue *p = ekValueCreate(E, EVT_STRING);
    ekStringSet(E, &p->stringVal, s);
    ekTraceValues(("ekValueCreateString %p\n", p));
    return p;
}

ekValue *ekValueCreateStringLen(struct ekContext *E, const char *s, ekS32 len)
{
    ekValue *p = ekValueCreate(E, EVT_STRING);
    ekStringSetLen(E, &p->stringVal, s, len);
    ekTraceValues(("ekValueCreateStringLen %p\n", p));
    return p;
}

ekValue *ekValueDonateString(struct ekContext *E, char *s)
{
    ekValue *p = ekValueCreate(E, EVT_STRING);
    ekStringDonate(E, &p->stringVal, s);
    ekTraceValues(("ekValueDonateString %p\n", p));
    return p;
}

ekValue *ekValueCreateFunction(struct ekContext *E, struct ekBlock *block)
{
    ekValue *p = ekValueCreate(E, EVT_BLOCK);
    p->closureVars = NULL;
    p->blockVal = block;
    ekTraceValues(("ekValueCreateFunction %p\n", p));
    return p;
}

ekValue *ekValueCreateCFunction(struct ekContext *E, ekCFunction func)
{
    ekValue *p = ekValueCreate(E, EVT_CFUNCTION);
    p->cFuncVal = func;
    ekTraceValues(("ekValueCreateCFunction %p\n", p));
    return p;
}

ekValue *ekValueCreateRef(struct ekContext *E, struct ekValue **ref)
{
    ekValue *p = ekValueCreate(E, EVT_REF);
    p->refVal = ref;
    ekTraceValues(("ekValueCreateRef %p\n", p));
    return p;
}

static ekBool ekValueCheckRef(struct ekContext *E, ekValue *ref, ekValue *p)
{
    if(!p)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: empty stack!");
        return ekFalse;
    }
    if(!ref)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: empty stack!");
        return ekFalse;
    }
    if(!(*ref->refVal))
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: missing ref!");
        return ekFalse;
    }
    if(ref->type != EVT_REF)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: value on top of stack, ref underneath");
        return ekFalse;
    }
    return ekTrue;
}

ekBool ekValueSetRefVal(struct ekContext *E, ekValue *ref, ekValue *p)
{
    if(!ekValueCheckRef(E, ref, p))
    {
        return ekFalse;
    }

    ekValueRemoveRefNote(E, *(ref->refVal), "SetRefVal: forgetting previous val");
    *(ref->refVal) = p;
    ekValueAddRefNote(E, p, "SetRefVal: taking ownership of val");

    ekTraceValues(("ekValueSetRefVal %p = %p\n", ref, p));
    return ekTrue;
}

static void ekValueAddClosureVar(ekContext *E, ekMap *closureVars, ekMapEntry *entry)
{
    ekValueAddRefNote(E, entry->valuePtr, "+ref closure variable");
    ekMapGetS2P(E, closureVars, entry->keyStr) = entry->valuePtr;
}

void ekValueAddClosureVars(struct ekContext *E, ekValue *p)
{
    ekFrame *frame;
    ekS32 frameIndex;

    if(p->type != EVT_BLOCK)
    {
        // TODO: add impossibly catastrophic error here? (can't happen?)
        return;
    }

    ekAssert(p->closureVars == NULL);

    for(frameIndex = ekArraySize(E, &E->frames) - 1; frameIndex >= 0; frameIndex--)
    {
        frame = E->frames[frameIndex];
        if((frame->type & (EFT_CHUNK|EFT_FUNC)) == EFT_FUNC)  // we are inside of an actual function!
        {
            break;
        }
    }

    if(frameIndex >= 0)
    {
        for(; frameIndex < ekArraySize(E, &E->frames); frameIndex++)
        {
            frame = E->frames[frameIndex];
            if(frame->locals->count)
            {
                if(!p->closureVars)
                {
                    p->closureVars = ekMapCreate(E, EMKT_STRING);
                }
                ekMapIterateP1(E, frame->locals, ekValueAddClosureVar, p->closureVars);
            }
        }
    }
}


ekBool ekValueTestInherits(struct ekContext *E, ekValue *child, ekValue *parent)
{
    ekValue *p;

    if(child->type != EVT_OBJECT)
    {
        return ekFalse;
    }

    if(parent->type != EVT_OBJECT)
    {
        return ekFalse;
    }

    p = child->objectVal->isa;
    while(p && (p->type == EVT_OBJECT))
    {
        if(p->objectVal == parent->objectVal)
        {
            return ekTrue;
        }

        p = p->objectVal->isa;
    }

    return ekFalse;
}

// ---------------------------------------------------------------------------

ekValue *ekValueCreateArray(struct ekContext *E)
{
    ekValue *p = ekValueCreate(E, EVT_ARRAY);
    p->arrayVal = NULL;
    return p;
}

void ekValueArrayPush(struct ekContext *E, ekValue *p, ekValue *v)
{
    ekAssert(p->type == EVT_ARRAY);

    ekArrayPush(E, &p->arrayVal, v);
}

void ekValueArrayClear(struct ekContext *E, ekValue *p)
{
    ekAssert(p->type == EVT_ARRAY);

    ekArrayClear(E, &p->arrayVal, (ekDestroyCB)ekValueRemoveRefArray);
}

// ---------------------------------------------------------------------------

ekValue *ekValueCreateObject(struct ekContext *E, struct ekValue *isa, ekS32 argCount, ekBool firstArgIsa)
{
    ekValue *p = ekValueCreate(E, EVT_OBJECT);
    if(firstArgIsa)
    {
        ekAssert(argCount);
        ekAssert(isa == NULL);

        if(argCount)
        {
            isa = ekContextGetArg(E, 0, argCount);
            if(!isa || (isa->type == EVT_NULL))
            {
                isa = NULL;
            }
            if(isa && (isa->type != EVT_OBJECT))
            {
                ekContextSetError(E, EVE_RUNTIME, "objects can only inherit from objects");
                isa = NULL;
            }
        }
        else
        {
            isa = NULL;
        }
    }
    p->objectVal = ekObjectCreate(E, isa);

    if(argCount)
    {
        ekS32 i = 0;
        if(firstArgIsa)
        {
            i++;
        }
        for(; i<argCount; i+=2)
        {
            ekValue **ref;
            ekValue *key = ekContextGetArg(E, i, argCount);
            ekValue *val = ekValueNullPtr;
            ekS32 valueArg = i+1;
            if(valueArg < argCount)
            {
                val = ekContextGetArg(E, valueArg, argCount);
            }
            key = ekValueToString(E, key);
            ref = ekObjectGetRef(E, p->objectVal, ekStringSafePtr(&key->stringVal), ekTrue);
            *ref = val;
            ekValueAddRefNote(E, val, "ekValueCreateObject add member value");
        }
        ekContextPopValues(E, argCount);
    }
    return p;
}

void ekValueObjectSetMember(struct ekContext *E, struct ekValue *object, const char *name, struct ekValue *value)
{
    ekValue **ref = NULL;
    ekAssert(object->type == EVT_OBJECT);
    ref = ekObjectGetRef(E, object->objectVal, name, ekTrue);
    ekAssert(ref);
    *ref = value;
}

// ---------------------------------------------------------------------------
ekS32 ekValueCmp(struct ekContext *E, ekValue *a, ekValue *b)
{
    if(a == b)
    {
        return 0;    // this should also handle the NULL case
    }

    if(a && b)
    {
        ekS32 ret = 0;
        if(ekValueTypeSafeCall(a->type, Cmp)(E, a, b, &ret))
        {
            return ret;
        }
    }

    return (ekS32)(a - b); // Fallback case: compare pointers for consistency
}

ekS32 ekValueLength(struct ekContext *E, ekValue *p)
{
    return ekValueTypeSafeCall(p->type, Length)(E, p);
}

const char *ekValueSafeStr(ekValue *stringValue)
{
    if(!stringValue || (stringValue->type != EVT_STRING))
    {
        return "";
    }
    return ekStringSafePtr(&stringValue->stringVal);
}

void ekValueCloneData(struct ekContext *E, ekValue *dst, ekValue *src)
{
    dst->type = src->type;
    ekValueTypeSafeCall(dst->type, Clone)(E, dst, src);
}

ekValue *ekValueClone(struct ekContext *E, ekValue *p)
{
    ekValue *n = ekValueCreate(E, p->type);
    ekValueCloneData(E, n, p);
    ekTraceValues(("ekValueClone %p -> %p\n", p, n));
    return n;
}

ekValue *ekValueAdd(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_ADD);
    if(!value)
    {
        ekTraceValues(("Don't know how to add types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueSub(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_SUB);
    if(!value)
    {
        ekTraceValues(("Don't know how to subtract types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueMul(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_MUL);
    if(!value)
    {
        ekTraceValues(("Don't know how to multiply types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueDiv(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_DIV);
    if(!value)
    {
        ekTraceValues(("Don't know how to divide types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueToBool(struct ekContext *E, ekValue *p)
{
    ekBool boolVal = ekValueTypeSafeCall(p->type, ToBool)(E, p);
    ekValue *value = ekValueCreateInt(E, boolVal);
    ekValueRemoveRefNote(E, p, "ekValueToBool");
    return value;
}

ekValue *ekValueToInt(struct ekContext *E, ekValue *p)
{
    ekS32 intVal = ekValueTypeSafeCall(p->type, ToInt)(E, p);
    ekValue *ret = ekValueCreateInt(E, intVal);
    ekValueRemoveRefNote(E, p, "ekValueToInt");
    return ret;
}

ekValue *ekValueToFloat(struct ekContext *E, ekValue *p)
{
    ekF32 floatVal = ekValueTypeSafeCall(p->type, ToFloat)(E, p);
    ekValue *ret = ekValueCreateFloat(E, floatVal);
    ekValueRemoveRefNote(E, p, "ekValueToFloat");
    return ret;
}

ekValue *ekValueToString(struct ekContext *E, ekValue *p)
{
    ekValue *value = ekValueTypeSafeCall(p->type, ToString)(E, p);
    if(!value)
    {
        ekTraceExecution(("ekValueToString: unable to convert type '%s' to string\n", ekValueTypePtr(p->type)->name));
    }
    return value;
}

ekValue *ekValueReverse(struct ekContext *E, ekValue *p)
{
    ekValue *reversed = ekValueTypeSafeCall(p->type, Reverse)(E, p);
    if(!reversed)
    {
        ekContextSetError(E, EVE_RUNTIME, "reverse: unsupported type %s", ekValueTypeName(E, p->type));
        ekValueRemoveRefNote(E, p, "ekValueReverse failed, p done either way");
        reversed = ekValueNullPtr;
    }
    return reversed;
}

ekCFunction *ekValueIter(struct ekContext *E, ekValue *p)
{
    return ekValueTypeSafeCall(p->type, Iter)(E, p);
}

ekValue *ekValueStringFormat(struct ekContext *E, ekValue *format, ekS32 argCount)
{
    char *curr = (char *)ekStringSafePtr(&format->stringVal);
    char *next;

    ekValue *arg;
    ekS32 argIndex = 0;

    ekValue *ret = ekValueCreateString(E, "");
    ekString *str = &ret->stringVal;

    while(curr && (next = strchr(curr, '%')))
    {
        // First, add in all of the stuff before the %
        ekStringConcatLen(E, str, curr, (ekS32)(next - curr));
        next++;

        switch(*next)
        {
            case '\0':
                curr = NULL;
                break;
            case '%':
                ekStringConcatLen(E, str, "%", 1);
                break;
            case 's':
                arg = ekContextGetArg(E, argIndex++, argCount);
                if(arg)
                {
                    ekValueAddRefNote(E, arg, "string conversion");
                    arg = ekValueToString(E, arg);
                    ekStringConcatStr(E, str, &arg->stringVal);
                    ekValueRemoveRefNote(E, arg, "StringFormat 's' done");
                }
                break;
            case 'd':
                arg = ekContextGetArg(E, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    ekValueAddRefNote(E, arg, "ekS32 conversion");
                    arg = ekValueToInt(E, arg);
                    sprintf(temp, "%d", arg->intVal);
                    ekStringConcat(E, str, temp);
                    ekValueRemoveRefNote(E, arg, "StringFormat 'd' done");
                }
                break;
            case 'f':
                arg = ekContextGetArg(E, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    ekValueAddRefNote(E, arg, "float conversion");
                    arg = ekValueToFloat(E, arg);
                    sprintf(temp, "%f", arg->floatVal);
                    ekStringConcat(E, str, temp);
                    ekValueRemoveRefNote(E, arg, "StringFormat 'f' done");
                }
                break;
        };

        curr = next + 1;
    }

    // Add the remainder of the string, if any
    if(curr)
    {
        ekStringConcat(E, str, curr);
    }

    ekContextPopValues(E, argCount);
    ekValueRemoveRefNote(E, format, "FORMAT format done");
    return ret;
}

ekValue *ekValueIndex(struct ekContext *E, ekValue *p, ekValue *index, ekBool lvalue)
{
    ekValue *v = ekValueTypeSafeCall(p->type, Index)(E, p, index, lvalue);
    if(!v)
    {
        v = ekValueTypeGetIntrinsic(E, p->type, index, lvalue);
    }
    return v;
}

const char *ekValueTypeName(struct ekContext *E, ekS32 type)
{
    if((type >= 0) && (type < ekArraySize(E, &E->types)))
    {
        ekValueType *valueType = E->types[type];
        return valueType->name;
    }
    return "unknown";
}

ekDumpParams *ekDumpParamsCreate(struct ekContext *E)
{
    ekDumpParams *params = (ekDumpParams *)ekAlloc(sizeof(ekDumpParams));
    return params;
}

void ekDumpParamsDestroy(struct ekContext *E, ekDumpParams *params)
{
    ekStringClear(E, &params->output);
    ekStringClear(E, &params->tempStr);
    ekFree(params);
}

void ekValueDump(struct ekContext *E, ekDumpParams *params, ekValue *p)
{
    ekValueTypeSafeCall(p->type, Dump)(E, params, p);
}

#ifdef EUREKA_TRACE_REFS
void ekValueTraceRefs(struct ekContext *E, struct ekValue *p, ekS32 delta, const char *note)
{
    const char *destroyed = "";
    char tempPtr[32];
    ekS32 newRefs = p->refs;
    if(p == ekValueNullPtr)
    {
        sprintf(tempPtr, "0xEK_NULL");
        note = "-- ignoring ekValueNull --";
    }
    else
    {
        sprintf(tempPtr, "%10p", p);
        newRefs += delta;
        if((delta < 0) && !newRefs)
        {
            destroyed = " - DESTROYED";
        }
    }
    ekTraceRefs(("\t\t\t\t\t\t\t\tREFS: ekValue %s [%2d delta -> %d]: %s%s\n", tempPtr, delta, newRefs, note, destroyed));
}

void ekValueRemoveRefArray(struct ekContext *E, struct ekValue *p)
{
    ekValueTraceRefs(E, p, -1, "array cleared");
    ekValueRemoveRef(E, p);
}

void ekValueRemoveRefHashed(struct ekContext *E, struct ekValue *p)
{
    ekValueTraceRefs(E, p, -1, "hash cleared");
    ekValueRemoveRef(E, p);
}
#endif
