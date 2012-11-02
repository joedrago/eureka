// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

// TODO: nest li depths

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dynString.h"

enum
{
    TYPE_ROOT = 0,
    TYPE_BLANK,
    TYPE_TEXT,
    TYPE_CODE,
    TYPE_LINK,
    TYPE_TITLE1,
    TYPE_TITLE2,
    TYPE_LINE1,
    TYPE_LINE2,
    TYPE_LIST_START,
    TYPE_LIST_ENTRY,
    TYPE_LIST_END,

    TYPE_COUNT
};

typedef struct ekdNode
{
    int type;
    int depth;
    char *text;
    union
    {
        char *key;
        char *style;
    };
    struct ekdNode *next;
} ekdNode;

ekdNode *ekdNodeCreate(int type)
{
    ekdNode *node = (ekdNode *)calloc(1, sizeof(ekdNode));
    node->type = type;
    return node;
}

void ekdNodeDestroy(ekdNode *n)
{
    while(n)
    {
        ekdNode *freeme = n;
        n = n->next;

        if(freeme->style)
        {
            free(freeme->style);
        }
        if(freeme->text)
        {
            free(freeme->text);
        }
        free(freeme);
    }
}

ekdNode *ekdNodeCreateText(int type, const char *text, int len)
{
    ekdNode *node = ekdNodeCreate(type);
    if(len == -1)
    {
        len = strlen(text);
    }
    node->text = (char *)calloc(1, len + 1 /* null terminator */);
    memcpy(node->text, text, len);
    return node;
}

ekdNode *ekdNodeCreateTextDepth(int type, const char *text, int len, int depth)
{
    ekdNode *node = ekdNodeCreateText(type, text, len);
    node->depth = depth;
    return node;
}

ekdNode *ekdNodeCreateTextKey(int type, const char *text, int len, const char *key)
{
    ekdNode *node = ekdNodeCreateText(type, text, len);
    if(key)
    {
        node->key = strdup(key);
    }
    return node;
}

ekdNode *ekdNodeCreateTextStyleDepth(int type, const char *text, int len, const char *style, int depth)
{
    ekdNode *node = ekdNodeCreateText(type, text, len);
    if(style)
    {
        node->style = strdup(style);
    }
    node->depth = depth;
    return node;
}

void ekdNodeAppendText(ekdNode *node, const char *text, int len, char sep)
{
    char *prevText = node->text;
    int prevLen = (int)strlen(prevText);
    int useSep = 0;
    if(len == -1)
    {
        len = (int)strlen(text);
    }
    if(prevLen)
    {
        useSep = 1;
    }
    node->text = (char *)calloc(1, prevLen + len + useSep + 1 /* null terminator */);
    memcpy(node->text, prevText, prevLen);
    node->text[prevLen] = sep;
    memcpy(node->text + prevLen + useSep, text, len);
    free(prevText);
}

ekdNode *ekdNodeAppend(ekdNode *parent, ekdNode *child)
{
    parent->next = child;
    return child;
}

// -------------------------------------------------------------------------------------------
// Line tokenizers

static char *nextLine(char **linePtr)
{
    char *line = *linePtr;
    char *endLine;
    if(!*line)
    {
        return NULL;
    }
    endLine = strchr(line, '\n');
    if(endLine)
    {
        *endLine = 0;
    }
    *linePtr = line + strlen(line) + (endLine ? 1 : 0);
    if(endLine && (endLine != line))
    {
        --endLine;
        for(; endLine != line; --endLine)
        {
            if(*endLine == '\r')
            {
                *endLine = 0;
            }
            else
            {
                break;
            }
        }
    }
    return line;
}

static char *nextWrap(char **linePtr, int maxLen)
{
    char *line = *linePtr;
    char *currBreak = NULL;
    char *nextBreak;
    if(!*line)
    {
        return NULL;
    }
    if(strlen(line) > maxLen)
    {
        while(1)
        {
            nextBreak = strchr(currBreak ? currBreak+1 : line, ' ');
            if(nextBreak)
            {
                if((int)(nextBreak - line) > maxLen)
                {
                    break;
                }
                else
                {
                    currBreak = nextBreak;
                }
            }
            else
            {
                break;
            }
        }
    }
    if(currBreak)
    {
        *currBreak = 0;
    }
    *linePtr = line + strlen(line) + (currBreak ? 1 : 0);
    return line;
}

// -------------------------------------------------------------------------------------------
// Parser

ekdNode *ekdParse(char *text)
{
    static const char *seps = "\n";
    ekdNode *root = ekdNodeCreate(TYPE_ROOT);
    ekdNode *prev = root;
    char *line = nextLine(&text);
    int listDepth = 0;
    for(; line; line = nextLine(&text))
    {
        if(*line == '#')
        {
            // Comment: Ignore it.
        }
        else if(*line == '^')
        {
            // Link!
            char *key = line + 1;
            char *link = key;
            while(*link && (*link != ' '))
            {
                ++link;
            }
            if(*link)
            {
                *link = 0;
                ++link;
                if(*link)
                {
                    prev = ekdNodeAppend(prev, ekdNodeCreateTextKey(TYPE_LINK, link, -1, key));
                }
            }
        }
        else if(*line == ' ')
        {
            if((prev->type == TYPE_TEXT) || (prev->type == TYPE_LIST_ENTRY) || (prev->type == TYPE_CODE))
            {
                int depth = 0;
                int extraDepth = (prev->type == TYPE_LIST_ENTRY) ? 1 : 0;
                while(*line == ' ')
                {
                    ++depth;
                    ++line;
                    if(depth == prev->depth + extraDepth)
                    {
                        break;
                    }
                }

                // Append to previous block of text
                ekdNodeAppendText(prev, line, -1, (prev->type == TYPE_CODE) ? '\n' : ' ');
            }
            else
            {
                // New Code!
                char *style = NULL;
                int depth = 0;
                while(*line == ' ')
                {
                    ++depth;
                    ++line;
                }
                if(*line == ':')
                {
                    // Style!
                    style = line + 1;
                    line += strlen(line);
                }
                prev = ekdNodeAppend(prev, ekdNodeCreateTextStyleDepth(TYPE_CODE, line, -1, style, depth));
            }
        }
        else if(*line == '*')
        {
            // A new list entry!
            int depth = 0;
            while(*line == '*')
            {
                ++depth;
                ++line;
            }
            while(*line == ' ')
            {
                ++line;
            }
            while(listDepth < depth)
            {
                prev = ekdNodeAppend(prev, ekdNodeCreate(TYPE_LIST_START));
                ++listDepth;
            }
            while(listDepth > depth)
            {
                prev = ekdNodeAppend(prev, ekdNodeCreate(TYPE_LIST_END));
                --listDepth;
            }

            prev = ekdNodeAppend(prev, ekdNodeCreateTextDepth(TYPE_LIST_ENTRY, line, -1, depth));
        }
        else
        {
            // Not a list entry. Clean up any running list.
            while(listDepth)
            {
                prev = ekdNodeAppend(prev, ekdNodeCreate(TYPE_LIST_END));
                --listDepth;
            }

            if(!*line)
            {
                // Blank line
                if(prev->type != TYPE_LINK)
                {
                    prev = ekdNodeAppend(prev, ekdNodeCreate(TYPE_BLANK));
                }
            }
            else if((strstr(line, "---") == line) || (strstr(line, "===") == line))
            {
                if(prev->type == TYPE_TEXT)
                {
                    // Upgrade previous block to a title
                    prev->type = (line[0] == '-') ? TYPE_TITLE1 : TYPE_TITLE2;
                }
                else
                {
                    // Horizontal line
                    prev = ekdNodeAppend(prev, ekdNodeCreate((line[0] == '-') ? TYPE_LINE1 : TYPE_LINE2));
                }
            }
            else
            {
                if(prev->type == TYPE_TEXT)
                {
                    // Append to previous block of text
                    ekdNodeAppendText(prev, line, -1, ' ');
                }
                else
                {
                    // New block of text
                    prev = ekdNodeAppend(prev, ekdNodeCreateText(TYPE_TEXT, line, -1));
                }
            }
        }
    }
    while(listDepth)
    {
        prev = ekdNodeAppend(prev, ekdNodeCreate(TYPE_LIST_END));
        --listDepth;
    }
    return root;
}

// -------------------------------------------------------------------------------------------
// Simple markup system

struct ekdMarkup;
typedef char *(*ekdReplaceFunc)(ekdNode *root, struct ekdMarkup *markup, char *tag, int *lenReplaced, int *freeRequired, int replaceCount);

typedef struct ekdMarkup
{
    const char *tag;
    const char *replaceBegin;
    const char *replaceEnd;
    ekdReplaceFunc replaceFunc;
} ekdMarkup;

char *ekdDefaultReplaceFunc(ekdNode *root, struct ekdMarkup *markup, char *tag, int *lenReplaced, int *freeRequired, int replaceCount)
{
    *lenReplaced = strlen(markup->tag);
    *freeRequired = 0;
    if(markup->replaceEnd && (replaceCount % 2))
    {
        return (char *)markup->replaceEnd;
    }
    return (char *)markup->replaceBegin;
}

char *ekdInterpolate(ekdNode *root, const char *rawText, ekdMarkup *markup)
{
    char *output = dsDup(rawText);
    char *front;
    ekdMarkup *m = markup;
    for(; m->tag; ++m)
    {
        int replaceCount = 0;
        char *front = output;
        while(*front)
        {
            char *foundTag = strstr(front, m->tag);
            if(foundTag)
            {
                ekdReplaceFunc func = (m->replaceFunc) ? m->replaceFunc : ekdDefaultReplaceFunc;
                int lenReplaced = strlen(m->tag);
                int freeRequired = 0;
                char *replaceText = func(root, m, foundTag, &lenReplaced, &freeRequired, replaceCount);
                int replaceTextLen = strlen(replaceText);
                int sizeDelta = replaceTextLen - lenReplaced;
                if(sizeDelta > 0)
                {
                    // growing; make room first
                    foundTag -= (size_t)output;
                    dsSetLength(&output, dsLength(&output) + sizeDelta);
                    foundTag += (size_t)output;
                    memmove(foundTag + sizeDelta, foundTag, strlen(foundTag) - sizeDelta);
                    memcpy(foundTag, replaceText, replaceTextLen);
                }
                else
                {
                    // shrinking; move data first
                    memmove(foundTag, foundTag - sizeDelta, strlen(foundTag) - sizeDelta);
                    memcpy(foundTag, replaceText, replaceTextLen);
                    dsSetLength(&output, dsLength(&output) + sizeDelta);
                }
                ++replaceCount;
                front = foundTag + 1;
                if(freeRequired)
                {
                    free(replaceText);
                }
            }
            else
            {
                front += strlen(front);
            }
        }
    }
    return output;
}

char *ekdLinkFunc(ekdNode *root, struct ekdMarkup *markup, char *tag, int *lenReplaced, int *freeRequired, int replaceCount)
{
    ekdNode *n = root;
    const char *linkFormat = markup->replaceBegin;
    char *key = tag + 2;
    char *end = key;
    char *link = "unknown";
    char *replaceText;
    while(*end && (*end != '>'))
    {
        ++end;
    }
    *lenReplaced = (end - tag) + 1;
    *end = 0;
    for(; n; n = n->next)
    {
        if(n->type == TYPE_LINK)
        {
            if(!strcmp(n->key, key))
            {
                link = n->text;
            }
        }
    }
    *end = ' ';

    replaceText = calloc(1, (strlen(linkFormat) - 2) + strlen(link) + 1);
    sprintf(replaceText, linkFormat, link);
    *freeRequired = 1;
    return replaceText;
}

static void ekdWrapAndIndent(char **output, const char *text, int width, int indent, char prefix, int onlyPrefixFirst)
{
    int indentSpaces = indent * 2;
    int maxLen = width - indentSpaces;
    char *buffer = strdup(text);
    char *front = buffer;
    char *line = nextLine(&front);
    for(; line; line = nextLine(&front))
    {
        char *wrapBuffer = strdup(line);
        char *wrapFront = wrapBuffer;
        char *subLine = nextWrap(&wrapFront, maxLen);
        int curr = 0;
        for(; subLine; subLine = nextWrap(&wrapFront, maxLen))
        {
            int i;
            for(i = 0; i < indent; ++i)
            {
                dsConcatf(output, "  ");
            }
            if(prefix)
            {
                if(curr && onlyPrefixFirst)
                {
                    dsConcatf(output, "  ");
                }
                else
                {
                    dsConcatf(output, "%c ", prefix);
                }
            }
            dsConcatf(output, "%s\n", subLine);
            ++curr;
        }
        free(wrapBuffer);
    }
    free(buffer);
}

// -------------------------------------------------------------------------------------------
// HTML Output

static ekdMarkup htmlMarkup[] =
{
    { "**", "<b>",              "</b>", NULL        },
    { "__", "<i>",              "</i>", NULL        },
    { "I<", "<img src=\"%s\">", NULL,   ekdLinkFunc },
    { "^<", "<a href=\"%s\">",  NULL,   ekdLinkFunc },
    { "^^", "</a>",             NULL,   NULL        },
    { "{{", "<pre>",            NULL,   NULL        },
    { "}}", "</pre>",           NULL,   NULL        },
    { NULL }
};

void ekdOutputHTML(ekdNode *root, char **output)
{
    ekdNode *n;
    for(n = root; n; n = n->next)
    {
        switch(n->type)
        {
            case TYPE_BLANK:
                dsConcatf(output, "<br>\n");
                break;
            case TYPE_LIST_START:
                dsConcatf(output, "<ul>\n");
                break;
            case TYPE_LIST_END:
                dsConcatf(output, "</ul>\n");
                break;
            case TYPE_LINE1:
                dsConcatf(output, "<hr size=1>\n");
                break;
            case TYPE_LINE2:
                dsConcatf(output, "<hr size=2>\n");
                break;
            case TYPE_LIST_ENTRY:
            {
                char *t = ekdInterpolate(root, n->text, htmlMarkup);
                dsConcatf(output, "<li>%s</li>\n", t);
                dsDestroy(&t);
            }
            break;
            case TYPE_TEXT:
            {
                char *t = ekdInterpolate(root, n->text, htmlMarkup);
                dsConcatf(output, "%s<br>\n", t);
                dsDestroy(&t);
            }
            break;
            case TYPE_CODE:
                dsConcatf(output, "<!--%s--><blockquote><pre>%s</pre></blockquote>\n", n->style ? n->style : "--", n->text);
                break;
            case TYPE_TITLE1:
                dsConcatf(output, "<h1>%s</h1>\n", n->text);
                break;
            case TYPE_TITLE2:
                dsConcatf(output, "<h2>%s</h2>\n", n->text);
                break;
        };
    }
}

// -------------------------------------------------------------------------------------------
// Wordpress Output

static ekdMarkup wordpressMarkup[] =
{
    { "**", "<b>",              "</b>", NULL        },
    { "__", "<i>",              "</i>", NULL        },
    { "I<", "<img src=\"%s\">", NULL,   ekdLinkFunc },
    { "^<", "<a href=\"%s\">",  NULL,   ekdLinkFunc },
    { "^^", "</a>",             NULL,   NULL        },
    { "{{", "<pre>",            NULL,   NULL        },
    { "}}", "</pre>",           NULL,   NULL        },
    { NULL }
};

void ekdOutputWordpress(ekdNode *root, char **output)
{
    ekdNode *n = root;
    ekdNode *prev = n;
    for(; n; prev = n, n = n->next)
    {
        switch(n->type)
        {
            case TYPE_BLANK:
                dsConcatf(output, "<br>\n");
                break;
            case TYPE_LIST_START:
                dsConcatf(output, "<ul>\n");
                break;
            case TYPE_LIST_END:
                dsConcatf(output, "</ul>\n");
                break;
            case TYPE_LINE1:
                dsConcatf(output, "<hr size=1>\n");
                break;
            case TYPE_LINE2:
                dsConcatf(output, "<hr size=2>\n");
                break;
            case TYPE_LIST_ENTRY:
            {
                char *t = ekdInterpolate(root, n->text, wordpressMarkup);
                dsConcatf(output, "<li>%s</li>\n", t);
                dsDestroy(&t);
            }
            break;
            case TYPE_TEXT:
            {
                char *t = ekdInterpolate(root, n->text, wordpressMarkup);
                dsConcatf(output, "%s<br>\n", t);
                dsDestroy(&t);
            }
            break;
            case TYPE_CODE:
                dsConcatf(output, "[cc lang=\"%s\"]\n%s\n[/cc]\n", n->style ? n->style : "--", n->text);
                break;
            case TYPE_TITLE1:
                dsConcatf(output, "<strong>%s</strong>\n", n->text);
                break;
            case TYPE_TITLE2:
                dsConcatf(output, "<strong>%s</strong>\n", n->text);
                break;
        };
    }
}

// -------------------------------------------------------------------------------------------
// Text Output

static ekdMarkup textMarkup[] =
{
    { "**", "*",         "*",    NULL        },
    { "__", "",          "",     NULL        },
    { "I<", "Image: %s", NULL,   ekdLinkFunc },
    { "^<", "(%s) ",     NULL,   ekdLinkFunc },
    { "^^", "",          NULL,   NULL        },
    { "{{", "",          NULL,   NULL        },
    { "}}", "",          NULL,   NULL        },
    { NULL }
};

void ekdOutputText(ekdNode *root, char **output)
{
    ekdNode *n = root;
    ekdNode *prev = n;
    for(; n; prev = n, n = n->next)
    {
        switch(n->type)
        {
            case TYPE_BLANK:
                dsConcatf(output, "\n");
                break;
            case TYPE_LINE1:
                dsConcatf(output, "----------------------------------------------------------------\n");
                break;
            case TYPE_LINE2:
                dsConcatf(output, "================================================================\n");
                break;
            case TYPE_LIST_ENTRY:
            {
                char *t = ekdInterpolate(root, n->text, textMarkup);
                /*
                int depth;
                for(depth = 1; depth < n->depth; ++depth)
                {
                    dsConcatf(output, "  ");
                }
                dsConcatf(output, "* %s\n", t);
                */
                ekdWrapAndIndent(output, t, 80, n->depth, '*', 1);
                dsDestroy(&t);
            }
            break;
            case TYPE_TEXT:
            {
                char *t = ekdInterpolate(root, n->text, textMarkup);
                ekdWrapAndIndent(output, t, 80, 0, 0, 0);
                dsDestroy(&t);
            }
            break;
            case TYPE_CODE:
                ekdWrapAndIndent(output, n->text, 80, 1, '|', 0);
                break;
            case TYPE_TITLE1:
                dsConcatf(output, "%s\n---\n", n->text);
                break;
            case TYPE_TITLE2:
                dsConcatf(output, "%s\n===\n", n->text);
                break;
        };
    }
}

// -------------------------------------------------------------------------------------------
// Entry point for ekdoc processing

void ekdProcess(const char *mode, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    int fileSize;
    char *text = NULL;
    if(!f)
    {
        fprintf(stderr, "Cannot open '%s' for read.\n", filename);
        return;
    }
    fseek(f, 0, SEEK_END);
    fileSize = (int)ftell(f);
    fseek(f, 0, SEEK_SET);
    if(fileSize > 0)
    {
        char *text = (char *)calloc(1, fileSize + 1);
        if(fread(text, fileSize, 1, f) == 1)
        {
            char *output = NULL;
            ekdNode *root = ekdParse(text);
            if(!strcmp(mode, "wordpress"))
            {
                ekdOutputWordpress(root, &output);
            }
            else if(!strcmp(mode, "text"))
            {
                ekdOutputText(root, &output);
            }
            else
            {
                ekdOutputHTML(root, &output);
            }
            printf("%s", output);
            dsDestroy(&output);
            ekdNodeDestroy(root);
        }
        free(text);
    }
    fclose(f);
}

// -------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Syntax: ekdoc [mode] [filename]\n");
        return 1;
    }
    ekdProcess(argv[1], argv[2]);
    return 0;
}
