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

void yapStringClear(struct yapContext *Y, yapString *str); // frees memory

const char *yapStringSafePtr(yapString *str);

void yapStringSetLen(struct yapContext *Y, yapString *str, const char *text, int len);
void yapStringSetStr(struct yapContext *Y, yapString *str, yapString *src);
void yapStringSet(struct yapContext *Y, yapString *str, const char *text);
void yapStringDonate(struct yapContext *Y, yapString *str, char *text);                // grants ownership of text to str, assumes (maxlen == len)
void yapStringDonateStr(struct yapContext *Y, yapString *str, yapString *donation);    // grants ownership of 'donation' contents to str
void yapStringSetK(struct yapContext *Y, yapString *str, const char *text);            // use constant string. This assumes that the ptr given will live forever

void yapStringConcatLen(struct yapContext *Y, yapString *str, const char *text, int len);
void yapStringConcatStr(struct yapContext *Y, yapString *str, yapString *src);
void yapStringConcat(struct yapContext *Y, yapString *str, const char *text);

int yapStringCmpStr(struct yapContext *Y, yapString *a, yapString *b);

#endif
