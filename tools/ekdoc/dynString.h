// ---------------------------------------------------------------------------
//                         Copyright Joe Drago 2012.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include "dynBase.h"

#include <stdarg.h>

// creation / destruction / cleanup
void dsCreate(char **dsptr);
void dsDestroy(char **dsptr);
void dsDestroyIndirect(char *ds);
void dsClear(char **dsptr);
char *dsDup(const char *text);
char *dsDupf(const char *format, ...);

// manipulation
void dsCopyLen(char **dsptr, const char *text, dynSize len);
void dsCopy(char **dsptr, const char *text);
void dsConcatLen(char **dsptr, const char *text, dynSize len);
void dsConcat(char **dsptr, const char *text);
void dsPrintf(char **dsptr, const char *format, ...);
void dsConcatv(char **dsptr, const char *format, va_list args);
void dsConcatf(char **dsptr, const char *format, ...);
void dsSetLength(char **dsptr, dynSize newLength);
void dsCalcLength(char **dsptr);
void dsSetCapacity(char **dsptr, dynSize newCapacity);

// information / testing
int dsCmp(char **dsptr, char **other);
dynSize dsLength(char **dsptr);
dynSize dsCapacity(char **dsptr);

#endif
