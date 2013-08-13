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
#include <sys/types.h>
#include <dirent.h>
#define USE_DIRENT
#ifdef PLATFORM_MINGW
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif
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
    ekBool permanent; // permanently in a state (stdin, stdout, stderr)
} ekFile;

// ---------------------------------------------------------------------------
// File Helpers

static int isDir(struct ekContext *E, ekValue *filenameValue)
{
    struct stat s;
    if(stat(ekValueSafeStr(filenameValue), &s) != -1)
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
    if(file->permanent)
    {
        ekContextSetError(E, EVE_RUNTIME, "attempting to change file state on a permanent file: file.%s", ekValueSafeStr(file->filename));
        return 0; // never change the state on a permanent file!
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
    if(file->handle == stdin)
    {
        char readBuffer[1024];
        if(fgets(readBuffer, 1024, stdin))
        {
            ret = ekValueCreateString(E, readBuffer);
        }
    }
    else if(file->handle)
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

    ekContextReturn(E, ret);
}

static ekU32 fileAbs(struct ekContext *E, ekU32 argCount)
{
    ekValue *filenameValue = NULL;
    struct stat s;
    int exists = 0;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file.exists(path)");
    }

    filenameValue = absolutePath(E, filenameValue);

    ekContextReturn(E, filenameValue);
}

static ekU32 fileExists(struct ekContext *E, ekU32 argCount)
{
    ekValue *filenameValue = NULL;
    struct stat s;
    int exists = 0;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file.exists(path)");
    }

    if(stat(ekValueSafeStr(filenameValue), &s) != -1)
    {
        exists = 1;
    }

    ekValueRemoveRefNote(E, filenameValue, "fileExists doesnt need filename anymore");
    ekContextReturnInt(E, exists);
}

static ekU32 fileLs(struct ekContext *E, ekU32 argCount)
{
    ekValue *filenameValue = NULL;
    ekValue *arrayValue = NULL;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file.ls(dirpath)");
    }

    arrayValue = ekValueCreateArray(E);

#ifdef USE_DIRENT
    {
        DIR *dir = opendir(ekValueSafeStr(filenameValue));
        if(dir)
        {
            struct dirent *entry;
            while(entry = readdir(dir))
            {
                if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                {
                    ekValueArrayPush(E, arrayValue, ekValueCreateString(E, entry->d_name));
                }
            }
            closedir(dir);
        }
    }
#endif

    ekValueRemoveRefNote(E, filenameValue, "fileExists doesnt need filename anymore");
    ekContextReturn(E, arrayValue);
}

static ekU32 fileIsDir(struct ekContext *E, ekU32 argCount)
{
    ekValue *filenameValue;
    int isdir = 0;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file.isdir(path)");
    }

    isdir = isDir(E, filenameValue);

    ekValueRemoveRefNote(E, filenameValue, "fileExists doesnt need filename anymore");
    ekContextReturnInt(E, isdir);
}

static ekU32 fileOpenInternal(struct ekContext *E, ekValue *fileValue, ekValue *modesValue)
{
    ekValue *ret = ekValueNullPtr;
    ekFile *file = (ekFile *)fileValue->ptrVal;
    if(isDir(E, file->filename))
    {
        return ekContextArgsFailure(E, 0, "file.open() does not work on directories");
    }

    {
        const char *c;
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
            ret = fileValue;
        }
        else
        {
            ekValueRemoveRefNote(E, fileValue, "fileOpen failed, so forget about fileValue");
        }
    }

    ekContextReturn(E, ret);
}

static ekU32 fileMemberOpen(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *modesValue = NULL;
    if(!ekContextGetArgs(E, argCount, "*F|s", &thisValue, &modesValue)) // assumes "r" if absent
    {
        return ekContextArgsFailure(E, argCount, "fileVal.open([string] modes)");
    }
    return fileOpenInternal(E, thisValue, modesValue);
}


static ekU32 fileOpen(struct ekContext *E, ekU32 argCount)
{
    ekValue *fileValue = ekValueNullPtr;
    ekValue *filenameValue;
    ekValue *modesValue = NULL;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "s|s", &filenameValue, &modesValue)) // assumes "r" if absent
    {
        return ekContextArgsFailure(E, argCount, "file.open([string] filename, [string] modes)");
    }

    fileValue = ekValueCreate(E, ekValueTypeId(E, 'F'));
    file = (ekFile *)ekAlloc(sizeof(ekFile));
    file->filename = absolutePath(E, filenameValue); // takes ownership
    fileValue->ptrVal = file;

    return fileOpenInternal(E, fileValue, modesValue);
}

static ekU32 fileReadLine(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *chompValue = NULL;
    ekBool chomp = ekFalse;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F|?", &thisValue, &chompValue))
    {
        return ekContextArgsFailure(E, argCount, "file.read([optional bool] chompNewline)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(chompValue)
    {
        chompValue = ekValueToBool(E, chompValue);
        chomp = chompValue->boolVal;
        ekValueRemoveRefNote(E, chompValue, "chompValue temporary no longer needed");
    }

    switchState(E, file, EFS_READ);
    ret = readLineInternal(E, file, chomp);

    ekValueRemoveRefNote(E, thisValue, "fileReadLine no longer needs thisValue");
    ekContextReturn(E, ret); // will be the data if we succesfully read
}

static ekU32 fileLines(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekValue *lineValue;
    ekValue *chompValue = NULL;
    ekBool chomp = ekFalse;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F|?", &thisValue, &chompValue))
    {
        return ekContextArgsFailure(E, argCount, "file.lines([optional bool] chompNewline)");
    }

    file = (ekFile *)thisValue->ptrVal;
    if(chompValue)
    {
        chompValue = ekValueToBool(E, chompValue);
        chomp = chompValue->boolVal;
        ekValueRemoveRefNote(E, chompValue, "chompValue temporary no longer needed");
    }

    switchState(E, file, EFS_READ);

    ret = ekValueCreateArray(E);
    while((lineValue = readLineInternal(E, file, chomp)) != ekValueNullPtr)
    {
        ekValueArrayPush(E, ret, lineValue);
    }

    ekValueRemoveRefNote(E, thisValue, "lines no longer needs thisValue");
    ekContextReturn(E, ret); // will be the data if we succesfully read
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
    ekContextReturn(E, ret); // will be the data if we succesfully read
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
    ekContextReturn(E, ret); // will be true if we successfully wrote
}

static ekU32 fileSize(struct ekContext *E, ekU32 argCount)
{
    ekValue *ret = ekValueNullPtr;
    ekValue *filenameValue;
    ekFile *file;
    struct stat s;

    if(!ekContextGetArgs(E, argCount, "s", &filenameValue))
    {
        return ekContextArgsFailure(E, argCount, "file.size(path)");
    }

    if(stat(ekValueSafeStr(file->filename), &s) != -1)
    {
        if(S_ISDIR(s.st_mode))
        {
            return ekContextArgsFailure(E, 0, "file.size() does not work on directories");
        }
        else
        {
            ret = ekValueCreateInt(E, s.st_size);
        }
    }

    ekValueRemoveRefNote(E, filenameValue, "fileSize doesnt need filename anymore");
    ekContextReturn(E, ret);
}

static ekU32 fileMemberSize(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue;
    ekValue *ret = ekValueNullPtr;
    ekFile *file;

    if(!ekContextGetArgs(E, argCount, "*F", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "file.size()");
    }

    file = (ekFile *)thisValue->ptrVal;

    if(file->handle)
    {
        int end;
        int currentPos = ftell(file->handle);
        fseek(file->handle, 0, SEEK_END);
        end = ftell(file->handle);
        fseek(file->handle, currentPos, SEEK_SET);
        ret = ekValueCreateInt(E, end);
    }

    ekValueRemoveRefNote(E, thisValue, "fileMemberSize doesnt need this anymore");
    ekContextReturn(E, ret);
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

    if(switchState(E, file, EFS_CLOSED))
    {
        ret = ekValueCreateInt(E, 1);
    }

    ekValueRemoveRefNote(E, thisValue, "fileClose doesnt need this anymore");
    ekContextReturn(E, ret);
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
    ekContextReturn(E, readLineInternal(E, file, chompVal->intVal));
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
    ekContextReturn(E, closure);
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
    ekContextReturn(E, closure);
}

// ---------------------------------------------------------------------------
// File Funcs

static void fileFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekFile *file = (ekFile *)p->ptrVal;
    if(!file->permanent)
    {
        switchState(E, file, EFS_CLOSED);
    }
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

    ekValueTypeAddIntrinsic(E, type, "read", fileRead);
    ekValueTypeAddIntrinsic(E, type, "readline", fileReadLine);
    ekValueTypeAddIntrinsic(E, type, "iterate", fileIterate);
    ekValueTypeAddIntrinsic(E, type, "lines", fileLines);
    ekValueTypeAddIntrinsic(E, type, "write", fileWrite);
    ekValueTypeAddIntrinsic(E, type, "size", fileSize);
    ekValueTypeAddIntrinsic(E, type, "close", fileClose);

    ekValueTypeRegister(E, type);
}

static ekModuleFunc fileFuncs[] =
{
    { "abs", fileAbs },
    { "open", fileOpen },
    { "size", fileSize },
    { "isdir", fileIsDir },
    { "exists", fileExists },
    { "ls", fileLs },

    { NULL, NULL }
};

static void addPermanentFile(struct ekContext *E, ekValue *module, const char *name, FILE *f, int permanentState)
{
    ekValue *fileValue = ekValueCreate(E, ekValueTypeId(E, 'F'));
    ekFile *file = (ekFile *)ekAlloc(sizeof(ekFile));
    file->filename = ekValueCreateString(E, name);
    file->state = permanentState;
    file->permanent = ekTrue;
    file->handle = f;
    fileValue->ptrVal = file;

    ekValueObjectSetMember(E, module, name, fileValue);
}

void ekModuleRegisterFile(struct ekContext *E)
{
    ekValue *module = ekContextAddModule(E, "file", fileFuncs);
    ekValueTypeRegisterFile(E);

    addPermanentFile(E, module, "stdin", stdin, EFS_READ);
    addPermanentFile(E, module, "stdout", stdout, EFS_WRITE);
    addPermanentFile(E, module, "stderr", stderr, EFS_WRITE);
}
