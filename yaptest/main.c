#include "yapCompiler.h"
#include "yapContext.h"
#include "yapModule.h"
#include "yapxDot.h"
#include "yapVM.h"

#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

yU32 standard_print(struct yapVM *vm, yU32 argCount)
{
    if(argCount)
    {
        int i;
        for(i=0; i<argCount; i++)
        {
            yapValue *v = yapVMGetArg(vm, i, argCount);
            switch(v->type)
            {
            case YVT_STRING: printf("%s", v->stringVal); break;
            case YVT_INT:    printf("%d", v->intVal); break;
            default: printf("***hurrrrr***"); break;
            };
        }
        yapVMPopValues(vm, argCount);
    }
    else
    {
        printf("(printing nothing)\n");
    }
    return 0;
}

yU32 print_nothing(struct yapVM *vm, yU32 argCount)
{
    if(argCount)
        yapVMPopValues(vm, argCount);
    return 0;
}

// ---------------------------------------------------------------------------

void loadModule(const char *code)
{
    yapContext *context = yapContextCreate();

    yapVMRegisterIntrinsic(context->vm, "print", standard_print);
    yapVMLoadModule(context->vm, "main", code);
    if(yapContextGetError(context))
    {
        printf("VM Bailed out: %s\n", yapContextGetError(context));
    }
    yapContextFree(context);
}

// ---------------------------------------------------------------------------

void outputDot(const char *code)
{
    yapCompiler *compiler = yapCompilerCreate();
    yapCompile(compiler, code, YCO_KEEP_SYNTAX_TREE|YCO_OPTIMIZE);
    if(compiler->root)
        yapSyntaxDot(compiler->root);
    else
        printf("ERROR: Failed to build syntax tree\n");
    yapCompilerDestroy(compiler);
}

// ---------------------------------------------------------------------------

char *loadFile(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char*)malloc(size+1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }
    
    printf("cant open '%s' for read\n", filename);
    return NULL;
}

// ---------------------------------------------------------------------------

// Yap Test Mode
enum
{
    YTM_LOADMODULE,
    YTM_DOT
};

int main(int argc, char* argv[])
{
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(212);
#endif
    {
        int mode = YTM_LOADMODULE;
        int i;
        char *filename = NULL;
        for(i=1;i<argc; i++)
        {
            if(argv[i][0] == '-')
            {
                switch(argv[i][1])
                {
                case 'd': mode = YTM_DOT; break;
                };
            }
            filename = argv[i];
        }

        if(filename)
        {
            char *code = loadFile(filename);
            if(code)
            {
                switch(mode)
                {
                case YTM_LOADMODULE: loadModule(code); break;
                case YTM_DOT:        outputDot(code);  break;
                };
                free(code);
            }
        }
        else
        {
            printf("yaptest [filename.yap]\n");
        }
    }

#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
	return 0;
}
