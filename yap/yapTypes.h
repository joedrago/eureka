#ifndef YAPTYPES_H
#define YAPTYPES_H

// ---------------------------------------------------------------------------
// Core Types

typedef int	yS32;
typedef unsigned int yU32;
typedef float yF32;
typedef char yS8;
typedef unsigned short yU16;
typedef short yS16;
typedef unsigned char yU8;
typedef unsigned int ySize;
typedef int yFlag;

typedef yS16 yOpcode;
typedef yS16 yOperand;

typedef int yBool;
#define yTrue 1
#define yFalse 0

#ifndef NULL
#define NULL 0
#endif

// ---------------------------------------------------------------------------
// Memory Routines

void * yapAlloc(ySize bytes);
void * yapRealloc(void *ptr, ySize bytes);
void yapFree(void *ptr);
char * yapStrdup(const char *s);

// ---------------------------------------------------------------------------
// Dynamic Array macros

typedef struct yap32Array
{
    int count;
    int capacity;
    yU32 *data;
} yap32Array;

yOperand yap32ArrayPushUnique(yap32Array *p, yU32 v);
yOperand yap32ArrayPush(yap32Array *p, yU32 v);
yU32 yap32ArrayPop(yap32Array *p);
void yap32ArrayClear(yap32Array *p);

typedef struct yapArray
{
    int count;
    int capacity;
    void **data;
} yapArray;

yOperand yapArrayPushUniqueString(yapArray *p, const char *s);
yOperand yapArrayPushUniqueStringLen(yapArray *p, const char *s, int len);
yOperand yapArrayPush(yapArray *p, void *v);
void * yapArrayPop(yapArray *p);
void * yapArrayTop(yapArray *p);
yU32 yapArrayCount(yapArray *p);
void yapArraySquash(yapArray *p);  // Removes all NULL entries

typedef void (*yapDestroyCB)(void *p);
void yapArrayClear(yapArray *p, yapDestroyCB cb);

// ---------------------------------------------------------------------------
// Debug Functions

//#define yapTrace(ARGS)
#define yapTrace(ARGS) printf ARGS

// ---------------------------------------------------------------------------

#endif
