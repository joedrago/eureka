// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapString.h"
#include "yapContext.h"

#include <string.h>

int yapStrlen(const char *s)
{
    return (int)strlen(s);
}

void yapStringClear(struct yapContext *Y, yapString *str)
{
    if((str->maxlen > 0) && str->text)
    {
        yapFree(str->text);
    }

    str->text = NULL;
    str->len = 0;
    str->maxlen = 0;
}

const char *yapStringSafePtr(yapString *str)
{
    if(str->text)
    {
        return str->text;
    }
    return "";
}

// Helper function
static yBool yapStringMakeRoom(struct yapContext *Y, yapString *str, int len, yBool append)
{
    int newlen;

    if(str->maxlen == YAP_CONSTANT_STRING)
    {
        yapStringClear(Y, str);
    }

    newlen = len;
    if(append)
    {
        newlen += str->len;
    }
    if(!str->maxlen || (newlen > str->maxlen))
    {
        str->text = yapRealloc(str->text, newlen+1);
        str->maxlen = newlen;
    }

    return yTrue;
}

void yapStringSetLen(struct yapContext *Y, yapString *str, const char *text, int len)
{
    yapStringMakeRoom(Y, str, len, yFalse);
    memcpy(str->text, text, len);
    str->text[len] = 0;
    str->len = len;
}

void yapStringSetStr(struct yapContext *Y, yapString *str, yapString *src)
{
    if(src->maxlen == YAP_CONSTANT_STRING)
    {
        yapStringSetK(Y, str, src->text);
    }
    else
    {
        yapStringSetLen(Y, str, src->text, src->len);
    }
}

void yapStringSet(struct yapContext *Y, yapString *str, const char *text)
{
    yapStringSetLen(Y, str, text, (int)strlen(text));
}

void yapStringDonate(struct yapContext *Y, yapString *str, char *text)
{
    yapStringClear(Y, str);
    str->text = text;
    str->len = yapStrlen(text);
    str->maxlen = str->len;
}

void yapStringDonateStr(struct yapContext *Y, yapString *str, yapString *donation)
{
    yapStringClear(Y, str);

    str->text = donation->text;
    str->len = donation->len;
    str->maxlen = donation->maxlen;

    donation->text = NULL;
    donation->len = 0;
    donation->maxlen = 0;
}

void yapStringSetK(struct yapContext *Y, yapString *str, const char *text)
{
    yapStringClear(Y, str);
    str->text = (char *)text;
    str->len = yapStrlen(str->text);
    str->maxlen = YAP_CONSTANT_STRING;
}

void yapStringConcatLen(struct yapContext *Y, yapString *str, const char *text, int len)
{
    if(!len)
    {
        return;
    }

    yapStringMakeRoom(Y, str, len, yTrue);
    memcpy(&str->text[str->len], text, len);
    str->len += len;
    str->text[str->len] = 0;
}

void yapStringConcatStr(struct yapContext *Y, yapString *str, yapString *src)
{
    yapStringConcatLen(Y, str, yapStringSafePtr(src), src->len);
}

void yapStringConcat(struct yapContext *Y, yapString *str, const char *text)
{
    yapStringConcatLen(Y, str, text, yapStrlen(text));
}

int yapStringCmpStr(struct yapContext *Y, yapString *a, yapString *b)
{
    return strcmp(yapStringSafePtr(a), yapStringSafePtr(b));
}
