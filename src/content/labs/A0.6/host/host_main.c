/**
 * @file    host_main.c
 * @brief   A0.6: runs the hygiene bench on a PC.
 */
#include <stdio.h>

#include "console.h"
#include "event_counter.h"
#include "hygiene_bench.h"

static void host_write(const char *text)
{
    (void)fputs(text, stdout);
}

int main(void)
{
    Console_Init(host_write);
    Console_Printf("=== A0.6 Hygiene bench (PC) ===\n");
    Console_Printf("event_counter v0x%08lX\n", (unsigned long)g_eventCounterVersion);

    const uint32_t failures = HygieneBench_Run();

    return (failures == 0U) ? 0 : 1;
}
