#ifndef YAPCORE_H
#define YAPCORE_H

#include "yapTypes.h"
#include "yapVM.h"

typedef struct yapContext
{
    yapVM *vm;
} yapContext;

yapContext * yapContextCreate(void);
void yapContextFree(yapContext *context);

#endif
