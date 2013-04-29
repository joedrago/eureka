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

#ifdef PLATFORM_WIN32
#include <windows.h>
#define MAX_PATH_LENGTH MAX_PATH
#define realpath(N,R) _fullpath((R),(N),MAX_PATH)
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#else
#define MAX_PATH_LENGTH 2048
#endif

// ---------------------------------------------------------------------------
// File Definitions and Data Structures

enum // Eureka File State
{
    EFS_CLOSED = 0,
    EFS_READ,
    EFS_WRITE,
    EFS_APPEND
};


typedef struct ekFile
{
    ekValue *filename;
    FILE *handle;
    int state;
} ekFile;

// ---------------------------------------------------------------------------
// File Helpers

static int isDir(struct ekContext *E, ekFile *file)
{
    struct stat s;
    if(stat(ekValueSafeStr(file->filename), &s) != -1)
    {
        if(S_ISDIR(s.st_mode))
        {
            return 1;
        }
    }
    return 0;
}

static int switchState(struct ekContext *E, ekFile *file, int newState)
{
    const char *mode = NULL;
    if(file->state == newState)
    {
        return 0;
    }
    if(file->handle)
    {
        fclose(file->handle);
        file->handle = NULL;
    }
    switch(newState)
    {
        case EFS_CLOSED:
            // do nothing, we already closed it
            break;

        case EFS_READ:
            mode = "rb";
            break;

        case EFS_WRITE:
            mode = "wb";
            break;

        case EFS_APPEND:
            mode = "ab";
            break;
    }
    if(mode)
    {
        file->handle = fopen(ekValueSafeStr(file->filename), mode);
    }
    file->state = newState;
    return 1;
}

static ekValue *readLineInternal(struct ekContext *E, ekFile *file, int chomp)
{
    ekValue *ret = ekValueNullPtr;
    if(file->handle)
    {
        int readBufferSize = 100;
        int startPos = ftell(file->handle);
        char *readBuffer = NULL;
        char *sepLoc;
        int bytesRead;

        while(ret == ekValueNullPtr)
        {
            readBufferSize *= 2;
            readBuffer = ekRealloc(readBuffer, readBufferSize);
            fseek(file->handle, startPos, SEEK_SET);
            bytesRead = fread(readBuffer, 1, readBufferSize-1, file->handle);

            if(bytesRead > 0)
            {
                readBuffer[bytesRead] = 0;
                sepLoc = strchr(readBuffer, '\n');
                if(sepLoc)
                {
                    int advance = (int)(sepLoc - readBuffer + 1); // how much to advance the file pointer?
                    if(chomp)
                    {
                        while((*sepLoc == '\n') || (*sepLoc == '\r'))
                        {
                            --sepLoc;
                        }
                    }
                    *(sepLoc+1) = 0;
                    ret = ekValueCreateString(E, readBuffer);
                    fseek(file->handle, startPos + advance, SEEK_SET); // move the file ptr back to the beginning of the next line
                }
                else
                {
                    if(bytesRead < (readBufferSize-1))
                    {
                        // we must have reached the end of the file. Just return 'the rest'.
                        ret = ekValueCreateString(E, readBuffer);
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }

        ekFree(readBuffer);
    }
    return ret;
}

static ekValue *absolutePath(struct ekContext *E, ekValue *path)
{
    char temppath[MAX_PATH_LENGTH];
    realpath(ekValueSafeStr(path), temppath);
    ekValueRemoveRefNote(E, path, "absolutePath doesn't need relative path anymore");
    return ekValueCreateString(E, temppath);
}

// ---------------------------------------------------------------------------
// File intrinsics

static ekU32 newFile(struct ekContext *E, ekU32 argCount)
{
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue = NULL;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file([string] filename)");
    }

    ret = ekValueCreate(E, ekValueTypeId(E, 'F'));
    file = (ekFile *)ekAlloc(sizeof(ekFile));
    file->filename = absolutePath(E, filenameValue); // takes ownership
    ret->ptrVal = file;

    ekArrayPush(E, &E->stack, ret);
    return 1;
}

static ekU32 fileExists(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekFile *file;
    struct stat s;
    int exists = 0;

    if(!ekContextGetArgs(E, argCount, "*F", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "file.exists()");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(stat(ekValueSafeStr(file->filename), &s) != -1)
    {
        exists = 1;
    }

    ekValueRemoveRefNote(E, thisValue, "fileExists doesnt need this anymore");
    ekContextReturnInt(E, exists);
}

static ekU32 fileIsDir(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    int exists = 0;
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "file.isdir()");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        ret = ekValueCreateInt(E, 1);
    }

    ekValueRemoveRefNote(E, thisValue, "fileExists doesnt need this anymore");
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

static ekU32 fileOpen(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *modesValue = NULL;
    ekFile *file;
    const char *c;

    if(!ekContextGetArgs(E, argCount, "*F|s", &thisValue, &modesValue)) // assumes "r" if absent
    {
        return ekContextArgsFailure(E, argCount, "file.open([string] modes)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.open() does not work on directories");
    }

    {
        const char *modes = "r";
        int state = EFS_READ;
        if(modesValue)
        {
            modes = ekValueSafeStr(modesValue);
        }
        for(c = modes; *c; ++c)
        {
            switch(*c)
            {
                case 'r':
                    state = EFS_READ;
                    break;
                case 'w':
                    state = EFS_WRITE;
                    break;
                case 'a':
                    state = EFS_APPEND;
                    break;
            };
        }

        switchState(E, file, state);
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

static ekU32 fileReadLine(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *chompValue = NULL;
    int chomp = 0;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F|?", &thisValue, &chompValue))
    {
        return ekContextArgsFailure(E, argCount, "file.read([optional bool] chompNewline)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.readline() does not work on directories");
    }

    if(chompValue)
    {
        chompValue = ekValueToBool(E, chompValue);
        chomp = chompValue->intVal;
        ekValueRemoveRefNote(E, chompValue, "chompValue temporary no longer needed");
    }

    switchState(E, file, EFS_READ);
    ret = readLineInternal(E, file, chomp);

    ekValueRemoveRefNote(E, thisValue, "fileReadLine no longer needs thisValue");
    ekArrayPush(E, &E->stack, ret); // will be the data if we succesfully read
    return 1;
}

static ekU32 fileLines(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *lineValue;
    ekValue *chompValue = NULL;
    int chomp = 0;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F|?", &thisValue, &chompValue))
    {
        return ekContextArgsFailure(E, argCount, "file.lines([optional bool] chompNewline)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.lines() does not work on directories");
    }

    if(chompValue)
    {
        chompValue = ekValueToBool(E, chompValue);
        chomp = chompValue->intVal;
        ekValueRemoveRefNote(E, chompValue, "chompValue temporary no longer needed");
    }

    switchState(E, file, EFS_READ);

    ret = ekValueCreateArray(E);
    while((lineValue = readLineInternal(E, file, chomp)) != ekValueNullPtr)
    {
        ekValueArrayPush(E, ret, lineValue);
    }

    ekValueRemoveRefNote(E, thisValue, "fileReadLine no longer needs thisValue");
    ekArrayPush(E, &E->stack, ret); // will be the data if we succesfully read
    return 1;
}

static ekU32 fileRead(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *bytesValue = NULL;
    int bytes = 0;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F|i", &thisValue, &bytesValue))
    {
        return ekContextArgsFailure(E, argCount, "file.read([optional int] bytes)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.read() does not work on directories");
    }

    switchState(E, file, EFS_READ);

    if(file->handle)
    {
        if(bytesValue)
        {
            bytesValue = ekValueToInt(E, bytesValue);
            bytes = bytesValue->intVal;
            ekValueRemoveRefNote(E, bytesValue, "bytesValue temporary no longer needed");
        }
        else
        {
            // we want "the rest" of the file (could be the whole thing if you just opened it)
            int end;
            int currentPos = ftell(file->handle);
            fseek(file->handle, 0, SEEK_END);
            end = ftell(file->handle);
            fseek(file->handle, currentPos, SEEK_SET);
            bytes = end - currentPos;
        }

        if(bytes > 0)
        {
            char *data = ekAlloc(bytes+1);
            int bytesRead = fread(data, 1, bytes, file->handle);
            if(bytesRead >= 0)
            {
                data[bytesRead] = 0;
                ret = ekValueDonateString(E, data);
            }
            else
            {
                ekFree(data);
            }
        }
    }

    ekValueRemoveRefNote(E, thisValue, "fileRead no longer needs thisValue");
    ekArrayPush(E, &E->stack, ret); // will be the data if we succesfully read
    return 1;
}

static ekU32 fileWrite(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *dataValue = NULL;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*Fs", &thisValue, &dataValue))
    {
        return ekContextArgsFailure(E, argCount, "file.write([string] data)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.write() does not work on directories");
    }

    if((file->state != EFS_WRITE) && (file->state != EFS_APPEND))
    {
        switchState(E, file, EFS_WRITE);
    }
    if(file->handle)
    {
        const char *data = ekValueSafeStr(dataValue);
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

static ekU32 fileSize(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "file.size()");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.size() does not work on directories");
    }

    ekContextPopValues(E, argCount); // ignore any arguments (warn?)

    {
        if(file->handle)
        {
            int end;
            int currentPos = ftell(file->handle);
            fseek(file->handle, 0, SEEK_END);
            end = ftell(file->handle);
            fseek(file->handle, currentPos, SEEK_SET);
            ret = ekValueCreateInt(E, end);
        }
        else
        {
            struct stat s;
            if(stat(ekValueSafeStr(file->filename), &s) != -1)
            {
                ret = ekValueCreateInt(E, s.st_size);
            }
        }
    }

    ekValueRemoveRefNote(E, thisValue, "fileSize doesnt need this anymore");
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

static ekU32 fileClose(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "file.close()");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, argCount, "file.close() does not work on directories");
    }

    if(switchState(E, file, EFS_CLOSED))
    {
        ret = ekValueCreateInt(E, 1);
    }

    ekValueRemoveRefNote(E, thisValue, "fileClose doesnt need this anymore");
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

// feeds one readline() at a time
static ekU32 fileIterator(struct ekContext *E, ekU32 argCount)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekValue *fileVal;
    ekValue *chompVal;
    ekFile *file;

    ekAssert(frame->closure && frame->closure->closureVars);
    fileVal = ekMapGetS2P(E, frame->closure->closureVars, "file");
    chompVal = ekMapGetS2P(E, frame->closure->closureVars, "chomp");
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);
    file = (ekFile *)fileVal->ptrVal;
    switchState(E, file, EFS_READ);
    ekArrayPush(E, &E->stack, readLineInternal(E, file, chompVal->intVal));
    return 1;
}

static ekU32 fileIterate(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *chompValue = NULL;
    ekFile *file;
    ekValue *closure;

    if(!ekContextGetArgs(E, argCount, "*F|?", &thisValue, &chompValue))
    {
        return ekContextArgsFailure(E, argCount, "file.iterate([optional bool] chompNewline)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(isDir(E, file))
    {
        return ekContextArgsFailure(E, 0, "file.iterate() does not work on directories");
    }

    if(chompValue)
    {
        chompValue = ekValueToBool(E, chompValue);
    }
    else
    {
        chompValue = ekValueCreateInt(E, 0);
    }

    closure = ekValueCreateCFunction(E, fileIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    ekMapGetS2P(E, closure->closureVars, "file") = thisValue;
    ekMapGetS2P(E, closure->closureVars, "chomp") = chompValue;
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

static ekU32 fileCreateIterator(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue = NULL;
    ekValue *closure;

    if(!ekContextGetArgs(E, argCount, "F", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "file iterator missing argument");
    }

    closure = ekValueCreateCFunction(E, fileIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    ekMapGetS2P(E, closure->closureVars, "file") = thisValue;
    ekMapGetS2P(E, closure->closureVars, "chomp") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

// ---------------------------------------------------------------------------
// File Funcs

static void fileFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekFile *file = (ekFile *)p->ptrVal;
    switchState(E, file, EFS_CLOSED);
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
    ekValueAddRefNote(E, file->filename, "fileFuncToString adding filename ref");
    ekValueRemoveRefNote(E, p, "fileFuncToString doesnt need file anymore");
    return file->filename;
}

static void fileFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekFile *file = (ekFile *)p->ptrVal;
    ekStringConcat(E, &params->output, "{ ");
    ekStringConcat(E, &params->output, "FILE: ");
    ekStringConcat(E, &params->output, ekValueSafeStr(file->filename));
    ekStringConcat(E, &params->output, " }");
}

static ekCFunction *fileFuncIter(struct ekContext *E, struct ekValue *p)
{
    return fileCreateIterator;
}

static void ekValueTypeRegisterFile(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "file", 'F');
    type->funcClear      = fileFuncClear;
    type->funcClone      = fileFuncClone;
    type->funcToBool     = fileFuncToBool;
    type->funcToInt      = fileFuncToInt;
    type->funcToFloat    = fileFuncToFloat;
    type->funcToString   = fileFuncToString;
    type->funcIter       = fileFuncIter;
    type->funcDump       = fileFuncDump;

    ekValueTypeAddIntrinsic(E, type, "exists", fileExists);
    ekValueTypeAddIntrinsic(E, type, "isdir", fileIsDir);
    ekValueTypeAddIntrinsic(E, type, "open", fileOpen);
    ekValueTypeAddIntrinsic(E, type, "read", fileRead);
    ekValueTypeAddIntrinsic(E, type, "readline", fileReadLine);
    ekValueTypeAddIntrinsic(E, type, "iterate", fileIterate);
    ekValueTypeAddIntrinsic(E, type, "lines", fileLines);
    ekValueTypeAddIntrinsic(E, type, "write", fileWrite);
    ekValueTypeAddIntrinsic(E, type, "size", fileSize);
    ekValueTypeAddIntrinsic(E, type, "close", fileClose);
    // needs .files() for directories, and fileCreateIterator needs to automatically choose .files() for dirs
    ekValueTypeRegister(E, type);
}

void ekModuleRegisterFile(struct ekContext *E)
{
    ekValueTypeRegisterFile(E);

    ekContextAddIntrinsic(E, "file", newFile);
}
