/**
 * @file    console.c
 * @brief   A0.6: printf-style console (see console.h).
 */
#include "console.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define CONSOLE_LINE_LEN   (128U)

/* Internal linkage: no other file can see or change the writer. */
static Console_WriteFn s_write = NULL;

void Console_Init(Console_WriteFn write)
{
    s_write = write;
}

void Console_Printf(const char *fmt, ...)
{
    char    line[CONSOLE_LINE_LEN];    /* automatic: one copy per call, reentrant */
    va_list args;

    if (s_write == NULL)
    {
        return;
    }
    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    s_write(line);
}
