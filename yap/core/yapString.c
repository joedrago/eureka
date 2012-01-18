// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapString.h"

int yapStrlen(const char *s)
{
    return (int)strlen(s);
}

void yapStringClear(yapString *str)
{
    if((str->maxlen > 0) && str->text)
        free(str->text);

    str->text = NULL;
    str->len = 0;
    str->maxlen = 0;
}

const char *yapStringSafePtr(yapString *str)
{
    if(str->text)
        return str->text;
    return "";
}

// Helper function
static yBool yapStringMakeRoom(yapString *str, int len, yBool append)
{
    int newlen;

    if(str->maxlen == YAP_CONSTANT_STRING)
        yapStringClear(str);

    newlen = len;
    if(append)
        newlen += str->len;
    if(!str->maxlen || (newlen > str->maxlen))
    {
        str->text = yapRealloc(str->text, newlen+1);
        str->maxlen = newlen;
    }

    return yTrue;
}

void yapStringSetLen(yapString *str, const char *text, int len)
{
    yapStringMakeRoom(str, len, yFalse);
    memcpy(str->text, text, len);
    str->text[len] = 0;
    str->len = len;
}

void yapStringSetStr(yapString *str, yapString *src)
{
    if(src->maxlen == YAP_CONSTANT_STRING)
        yapStringSetK(str, src->text);
    else
        yapStringSetLen(str, src->text, src->len);
}

void yapStringSet(yapString *str, const char *text)
{
    yapStringSetLen(str, text, (int)strlen(text));
}

void yapStringDonate(yapString *str, char *text)
{
    yapStringClear(str);
    str->text = text;
    str->len = yapStrlen(text);
    str->maxlen = str->len;
}

void yapStringSetK(yapString *str, const char *text)
{
    yapStringClear(str);
    str->text = (char*)text;
    str->len = yapStrlen(str->text);
    str->maxlen = YAP_CONSTANT_STRING;
}

void yapStringConcatLen(yapString *str, const char *text, int len)
{
    if(!len)
        return;

    yapStringMakeRoom(str, len, yTrue);
    memcpy(&str->text[str->len], text, len);
    str->len += len;
    str->text[str->len] = 0;
}

void yapStringConcatStr(yapString *str, yapString *src)
{
    yapStringConcatLen(str, yapStringSafePtr(src), src->len);
}

void yapStringConcat(yapString *str, const char *text)
{
    yapStringConcatLen(str, text, yapStrlen(text));
}

int yapStringCmpStr(yapString *a, yapString *b)
{
    return strcmp(yapStringSafePtr(a), yapStringSafePtr(b));
}
