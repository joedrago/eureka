#include "yapCompiler.h"
#include "yapContext.h"
#include "yapChunk.h"
#include "yapContext.h"

#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

yapContext *sVM = NULL;

#ifndef WIN32
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

static unsigned int GetTickCount()
{
    uint64_t        t;
    uint64_t        ns;
    Nanoseconds     nano;

    t = mach_absolute_time();
    nano = AbsoluteToNanoseconds(*(AbsoluteTime *) &t);
    ns = * (uint64_t *) &nano;

    return (unsigned int)(ns / 1000000);
}
#endif

unsigned int sLastTick = 0;

static int getInt(struct yapContext *Y, int arg, yU32 argCount)
{
    yapValue *a = yapContextGetArg(Y, arg, argCount);
    if(a)
    {
        a = yapValueToInt(Y, a);
        return a->intVal;
    }
    return 0;
}

static yU32 drawBox(struct yapContext *Y, yU32 argCount)
{
    if(argCount == 7)
    {
        int left   = getInt(Y, 0, argCount);
        int top    = getInt(Y, 1, argCount);
        int right  = getInt(Y, 2, argCount);
        int bottom = getInt(Y, 3, argCount);
        int r      = getInt(Y, 4, argCount);
        int g      = getInt(Y, 5, argCount);
        int b      = getInt(Y, 6, argCount);

        glColor3f((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);

        glBegin(GL_TRIANGLES);
        glVertex3f(left, top, 0);
        glVertex3f(left, bottom, 0);
        glVertex3f(right, top, 0);
        glVertex3f(right, top, 0);
        glVertex3f(left, bottom, 0);
        glVertex3f(right, bottom, 0);
        glEnd();
    }
    yapContextPopValues(Y, argCount);
    return 0;
}

// ---------------------------------------------------------------------------

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);

    {
        yapValue *msv;
        unsigned int tick = GetTickCount();
        if(tick > (sLastTick + 33))
        {
            unsigned int ms = tick - sLastTick;
            sLastTick = tick;

            msv = yapValueSetInt(sVM, yapValueAcquire(sVM), ms);
            yapArrayPush(&sVM->stack, msv);
            yapContextCallFuncByName(sVM, "update", 1);
        }
        yapContextCallFuncByName(sVM, "render", 0);
    }

    glutSwapBuffers();
}

void loadChunk(const char *code)
{
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

        buffer = (char *)malloc(size + 1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }

    printf("cant open '%s' for read\n", filename);
    return NULL;
}

// ---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(212);
#endif
    {
        int i;
        char *filename = NULL;
        for(i = 1; i < argc; i++)
        {
            if(argv[i][0] == '-')
            {
                //                switch(argv[i][1])
                //                {
                //                case 'd':
                //                    mode = YTM_DOT;
                //                    break;
                //                };
            }
            filename = argv[i];
        }

        if(filename)
        {
            char *code = loadFile(filename);
            if(code)
            {
                yapContext *context = yapContextCreate();
                sVM = Y;

                // init GLUT and create Window
                glutInit(&argc, argv);
                glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
                glutInitWindowPosition(100,100);
                glutInitWindowSize(800, 600);
                glutCreateWindow("Yap Game!");

                yapContextEval(Y, code, 0);
                free(code);
                if(yapContextGetError(context))
                {
                    printf("VM Bailed out: %s\n", yapContextGetError(context));
                    yapContextFree(context);
                    exit(0);
                }

                yapContextRegisterGlobalFunction(Y, "drawBox", drawBox);

                glutDisplayFunc(renderScene);
                glutIdleFunc(renderScene);

                yapContextCallFuncByName(sVM, "init", 0);

                glutMainLoop();
            }
        }
        else
        {
            printf("yapgame [filename.yap]\n");
        }
    }

#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
