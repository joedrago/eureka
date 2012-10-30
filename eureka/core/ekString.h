// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKASTRING_H
#define EUREKASTRING_H

#include "ekTypes.h"

int ekStrlen(const char *s);

#define EUREKA_CONSTANT_STRING (-1)

typedef struct ekString
{
    char *text;
    int len;
    int maxlen;             // allowed to be: EUREKA_CONSTANT_STRING
} ekString;

void ekStringClear(struct ekContext *Y, ekString *str); // frees memory

const char *ekStringSafePtr(ekString *str);

void ekStringSetLen(struct ekContext *Y, ekString *str, const char *text, int len);
void ekStringSetStr(struct ekContext *Y, ekString *str, ekString *src);
void ekStringSet(struct ekContext *Y, ekString *str, const char *text);
void ekStringDonate(struct ekContext *Y, ekString *str, char *text);                // grants ownership of text to str, assumes (maxlen == len)
void ekStringDonateStr(struct ekContext *Y, ekString *str, ekString *donation);    // grants ownership of 'donation' contents to str
void ekStringSetK(struct ekContext *Y, ekString *str, const char *text);            // use constant string. This assumes that the ptr given will live forever

void ekStringConcatLen(struct ekContext *Y, ekString *str, const char *text, int len);
void ekStringConcatStr(struct ekContext *Y, ekString *str, ekString *src);
void ekStringConcat(struct ekContext *Y, ekString *str, const char *text);

int ekStringCmpStr(struct ekContext *Y, ekString *a, ekString *b);

#endif
