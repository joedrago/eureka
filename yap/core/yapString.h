// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPSTRING_H
#define YAPSTRING_H

#include "yapTypes.h"

int yapStrlen(const char *s);

#define YAP_CONSTANT_STRING (-1)

typedef struct yapString
{
    char *text;
    int len;
    int maxlen;             // allowed to be: YAP_CONSTANT_STRING
} yapString;

void yapStringClear(yapString *str); // frees memory

const char *yapStringSafePtr(yapString *str);

void yapStringSetLen(yapString *str, const char *text, int len);
void yapStringSetStr(yapString *str, yapString *src);
void yapStringSet(yapString *str, const char *text);
void yapStringDonate(yapString *str, char *text);                // grants ownership of text to str, assumes (maxlen == len)
void yapStringSetK(yapString *str, const char *text);            // use constant string. This assumes that the ptr given will live forever

void yapStringConcatLen(yapString *str, const char *text, int len);
void yapStringConcatStr(yapString *str, yapString *src);
void yapStringConcat(yapString *str, const char *text);

int yapStringCmpStr(yapString *a, yapString *b);

#endif
