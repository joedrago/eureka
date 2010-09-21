#ifndef YAPTYPES_H
#define YAPTYPES_H

#ifdef PLATFORM_LINUX
#define yInline inline
#else
#define yInline __forceinline
#endif

typedef int	yS32;
typedef unsigned int yU32;
typedef float yF32;
typedef char yS8;
typedef unsigned short yU16;
typedef short yS16;
typedef unsigned char yU8;
typedef unsigned int ySize;

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

typedef struct yapArray
{
    int count;
    int capacity;
    void **data;
} yapArray;

void yapArrayPush(yapArray *p, void *v);
void * yapArrayPop(yapArray *p);
void * yapArrayTop(yapArray *p);

#define yapArrayClear(ARRAY) ARRAY->count = 0

typedef void (*yapDestroyCB)(void *p);
void yapArrayDestroy(yapArray *p, yapDestroyCB cb);

// ---------------------------------------------------------------------------

#endif
