// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekString.h"
#include "ekContext.h"

#include <string.h>

int ekStrlen(const char *s)
{
    return (int)strlen(s);
}

void ekStringClear(struct ekContext *E, ekString *str)
{
    if((str->maxlen > 0) && str->text)
    {
        ekFree(str->text);
    }

    str->text = NULL;
    str->len = 0;
    str->maxlen = 0;
}

const char *ekStringSafePtr(ekString *str)
{
    if(str->text)
    {
        return str->text;
    }
    return "";
}

// Helper function
static ekBool ekStringMakeRoom(struct ekContext *E, ekString *str, int len, ekBool append)
{
    int newlen;

    if(str->maxlen == EUREKA_CONSTANT_STRING)
    {
        ekStringClear(E, str);
    }

    newlen = len;
    if(append)
    {
        newlen += str->len;
    }
    if(!str->maxlen || (newlen > str->maxlen))
    {
        str->text = ekRealloc(str->text, newlen+1);
        str->maxlen = newlen;
    }

    return ekTrue;
}

void ekStringSetLen(struct ekContext *E, ekString *str, const char *text, int len)
{
    ekStringMakeRoom(E, str, len, ekFalse);
    memcpy(str->text, text, len);
    str->text[len] = 0;
    str->len = len;
}

void ekStringSetStr(struct ekContext *E, ekString *str, ekString *src)
{
    if(src->maxlen == EUREKA_CONSTANT_STRING)
    {
        ekStringSetK(E, str, src->text);
    }
    else
    {
        ekStringSetLen(E, str, src->text, src->len);
    }
}

void ekStringSet(struct ekContext *E, ekString *str, const char *text)
{
    ekStringSetLen(E, str, text, (int)strlen(text));
}

void ekStringDonate(struct ekContext *E, ekString *str, char *text)
{
    ekStringClear(E, str);
    str->text = text;
    str->len = ekStrlen(text);
    str->maxlen = str->len;
}

void ekStringDonateStr(struct ekContext *E, ekString *str, ekString *donation)
{
    ekStringClear(E, str);

    str->text = donation->text;
    str->len = donation->len;
    str->maxlen = donation->maxlen;

    donation->text = NULL;
    donation->len = 0;
    donation->maxlen = 0;
}

void ekStringSetK(struct ekContext *E, ekString *str, const char *text)
{
    ekStringClear(E, str);
    str->text = (char *)text;
    str->len = ekStrlen(str->text);
    str->maxlen = EUREKA_CONSTANT_STRING;
}

void ekStringConcatLen(struct ekContext *E, ekString *str, const char *text, int len)
{
    if(!len)
    {
        return;
    }

    ekStringMakeRoom(E, str, len, ekTrue);
    memcpy(&str->text[str->len], text, len);
    str->len += len;
    str->text[str->len] = 0;
}

void ekStringConcatStr(struct ekContext *E, ekString *str, ekString *src)
{
    ekStringConcatLen(E, str, ekStringSafePtr(src), src->len);
}

void ekStringConcat(struct ekContext *E, ekString *str, const char *text)
{
    ekStringConcatLen(E, str, text, ekStrlen(text));
}

int ekStringCmpStr(struct ekContext *E, ekString *a, ekString *b)
{
    return strcmp(ekStringSafePtr(a), ekStringSafePtr(b));
}
