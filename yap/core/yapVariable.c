// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapVariable.h"

#include "yapTypes.h"
#include "yapValue.h"
#include "yapContext.h"

yapVariable *yapVariableCreate(struct yapContext *Y, struct yapValue *value)
{
    yapVariable *v = (yapVariable *)yapAlloc(sizeof(yapVariable));
    v->value = value;
    yapArrayPush(&Y->usedVariables, v);
    return v;
}

void yapVariableDestroy(yapVariable *v)
{
    yapFree(v);
}

void yapVariableMark(struct yapContext *Y, yapVariable *variable)
{
    variable->used = yTrue;
    yapValueMark(Y, variable->value);
}
