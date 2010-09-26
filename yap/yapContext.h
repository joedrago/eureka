#ifndef YAPCONTEXT_H
#define YAPCONTEXT_H

#include "yapTypes.h"

typedef struct yapContext
{
    struct yapVM *vm;
} yapContext;

yapContext * yapContextCreate(void);
void yapContextFree(yapContext *context);
const char * yapContextGetError(yapContext *context);

#endif
