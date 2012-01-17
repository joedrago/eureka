// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPVARIABLE_H
#define YAPVARIABLE_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;
struct yapVM;

// ---------------------------------------------------------------------------

typedef struct yapVariable
{
    struct yapValue *value;
    yFlag used: 1;
} yapVariable;

yapVariable *yapVariableCreate(struct yapVM *vm, struct yapValue *value);
void yapVariableDestroy(yapVariable *v);

void yapVariableMark(struct yapVM *vm, yapVariable *variable); // used by yapVMGC()

#endif
