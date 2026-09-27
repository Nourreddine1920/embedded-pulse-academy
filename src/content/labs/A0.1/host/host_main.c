/** @file host_main.c  @brief Runs the A0.1 lab on a PC (no cycle counter). */
#include <stdio.h>
#include "int_lab.h"

static void host_write(const char *text) { fputs(text, stdout); }

int main(void)
{
    return IntLab_Run(host_write, NULL) ? 0 : 1;
}
