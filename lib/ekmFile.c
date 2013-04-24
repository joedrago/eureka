// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekmFile.h"

#include "ekContext.h"
#include "ekFrame.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

// ---------------------------------------------------------------------------
// File Data Structure

typedef struct ekFile
{
    ekValue *filename;
    int separator;    // defaults to newline
    FILE *handle;
} ekFile;

// ---------------------------------------------------------------------------
// File Funcs

static void fileFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekFile *file = (ekFile *)p->ptrVal;
    ekValueRemoveRefNote(E, file->filename, "file doesnt need filename anymore");
    ekFree(file);
}

static void fileFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "fileFuncClone not implemented");
}

static ekBool fileFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 fileFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 fileFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static struct ekValue *fileFuncToString(struct ekContext *E, struct ekValue *p)
{
    ekFile *file = (ekFile *)p->ptrVal;
    ekValue *filenameValue = file->filename;
    ekValueAddRefNote(E, filenameValue, "fileFuncToString adding filename ref");
    ekValueRemoveRefNote(E, p, "fileFuncToString doesnt need file anymore");
    return filenameValue;
}

#if 0

static ekU32 objectIterator(struct ekContext *E, ekU32 argCount)
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

static ekU32 objectCreateIterator(struct ekContext *E, ekU32 argCount)
{
    ekValue *m = NULL;
    ekValue *closure;
    ekValue *keys;
    if(!ekContextGetArgs(E, argCount, "m", &m))
    {
        return ekContextArgsFailure(E, argCount, "pairs(map)");
    }
    closure = ekValueCreateCFunction(E, objectIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    keys = ekValueCreateArray(E);
    ekMapIterateP1(E, m->objectVal->hash, ekAppendKey, keys);
    ekMapGetS2P(E, closure->closureVars, "map") = m;
    ekMapGetS2P(E, closure->closureVars, "keys") = keys;
    ekMapGetS2P(E, closure->closureVars, "index") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

static ekCFunction *fileFuncIter(struct ekContext *E, struct ekValue *p)
{
    return objectCreateIterator;
}

#endif

static void fileFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekFile *file = (ekFile *)p->ptrVal;
    ekStringConcat(E, &params->output, "{ ");
    ekStringConcat(E, &params->output, "FILE: ");
    ekStringConcat(E, &params->output, ekStringSafePtr(&file->filename->stringVal));
    ekStringConcat(E, &params->output, " }");
}

// ---------------------------------------------------------------------------
// File intrinsics

static ekValue *thisFile(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue = ekContextThis(E);
    // TODO: lots of sanity checking (not a null ptr, is a file, etc)
    return thisValue;
}

static ekU32 fileExists(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    int exists = 0;
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue;
    ekFile *file;

    if((thisValue = thisFile(E, argCount)) == NULL)
    {
        return 0;
    }

    ekContextPopValues(E, argCount); // ignore any arguments (warn?)

    file = (ekFile *)thisValue->ptrVal;
    {
        struct stat s;
        if(stat(ekStringSafePtr(&file->filename->stringVal), &s) != -1)
        {
            ret = ekValueCreateInt(E, 1);
        }
    }

    ekValueRemoveRefNote(E, thisValue, "fileExists doesnt need this anymore");
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

static ekU32 fileOpen(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    int exists = 0;
    ekValue *ret = ekValueNullPtr;
    ekValue *modesValue = NULL;
    ekFile *file;

    if((thisValue = thisFile(E, argCount)) == NULL)
    {
        return 0;
    }

    if(!ekContextGetArgs(E, argCount, "|s", &modesValue)) // assumes "r" if absent
    {
        return ekContextArgsFailure(E, argCount, "file.open([string] modes)");
    }

    file = (ekFile *)thisValue->ptrVal;
    {
        const char *modes = "r";
        if(modesValue)
        {
            modes = ekStringSafePtr(&modesValue->stringVal);
            // TODO: do not just blindly pass this through
        }
        if(file->handle)
        {
            fclose(file->handle);
            file->handle = NULL;
        }
        file->handle = fopen(ekStringSafePtr(&file->filename->stringVal), modes);
        if(file->handle)
        {
            ret = thisValue;
        }
        else
        {
            ekValueRemoveRefNote(E, thisValue, "fileOpen failed, so forget about thisValue");
        }
    }

    ekArrayPush(E, &E->stack, ret); // will be Null if we failed to open, otherwise thisValue
    return 1;
}

static ekU32 fileWrite(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    int exists = 0;
    ekValue *ret = ekValueNullPtr;
    ekValue *dataValue = NULL;
    ekFile *file;

    if((thisValue = thisFile(E, argCount)) == NULL)
    {
        return 0;
    }

    if(!ekContextGetArgs(E, argCount, "s", &dataValue))
    {
        return ekContextArgsFailure(E, argCount, "file.write([string] data)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(file->handle)
    {
        const char *data = ekStringSafePtr(&dataValue->stringVal);
        int len = strlen(data);
        int bytesWritten = fwrite(data, 1, len, file->handle);
        if(bytesWritten == len)
        {
            ret = ekValueCreateInt(E, 1);
        }
    }
    ekValueRemoveRefNote(E, thisValue, "fileWrite no longer needs thisValue");
    ekArrayPush(E, &E->stack, ret); // will be true if we successfully wrote
    return 1;
}

static ekU32 fileClose(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    int exists = 0;
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue;
    ekFile *file;

    if((thisValue = thisFile(E, argCount)) == NULL)
    {
        return 0;
    }

    ekContextPopValues(E, argCount); // ignore any arguments (warn?)

    file = (ekFile *)thisValue->ptrVal;
    {
        if(file->handle)
        {
            fclose(file->handle);
            file->handle = NULL;
            ret = ekValueCreateInt(E, 1);
        }
    }

    ekValueRemoveRefNote(E, thisValue, "fileClose doesnt need this anymore");
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------

static ekU32 newFile(struct ekContext *E, ekU32 argCount)
{
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue = NULL;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file([string] filename)");
    }

    ret = ekValueCreate(E);
    ret->type = ekValueTypeId(E, "file");
    file = (ekFile *)ekAlloc(sizeof(ekFile));
    file->filename = filenameValue; // takes ownership
    file->separator = '\n';
    ret->ptrVal = file;

    ekArrayPush(E, &E->stack, ret);
    return 1;
}

static void ekValueTypeRegisterFile(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "file");
    type->funcClear      = fileFuncClear;
    type->funcClone      = fileFuncClone;
    type->funcToBool     = fileFuncToBool;
    type->funcToInt      = fileFuncToInt;
    type->funcToFloat    = fileFuncToFloat;
    type->funcToString   = fileFuncToString;
#if 0
    type->funcIter       = fileFuncIter;
    type->funcIndex      = fileFuncIndex;
#endif
    type->funcDump       = fileFuncDump;

    ekValueTypeAddIntrinsic(E, type, "exists", fileExists);
    ekValueTypeAddIntrinsic(E, type, "open", fileOpen);
    ekValueTypeAddIntrinsic(E, type, "write", fileWrite);
    ekValueTypeAddIntrinsic(E, type, "close", fileClose);
    ekValueTypeRegister(E, type);
}

void ekModuleRegisterFile(struct ekContext *E)
{
    ekValueTypeRegisterFile(E);

    ekContextAddIntrinsic(E, "file", newFile);
}

