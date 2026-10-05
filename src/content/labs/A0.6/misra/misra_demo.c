/*
 * misra_demo.c  (A0.6) -- deliberately NON-compliant with MISRA C:2012.
 * GCC 10.3 with -Wall -Wextra -Wconversion -Wsign-conversion -Wshadow: 0 warnings.
 * Violations are listed in the lesson by reading the code against the rules (no MISRA checker was run).
 */
#include <stdint.h>
#include <stdio.h>

#define SCALE(x)   x * 10

uint8_t g_mode;

static int32_t clamp_add(int32_t offset, uint8_t sample)
{
    int32_t total = offset + sample;

    if (total > 100)
        total = 100;
    return total;
}

int32_t process(uint8_t sample, int32_t offset)
{
    int32_t result = clamp_add(offset, sample);

    switch (g_mode)
    {
        case 0U:
            result = SCALE(result + 1);
            break;
        case 1U:
            result = result >> 1;
            break;
    }
    if (g_mode)
    {
        printf("result=%ld\n", (long)result);
    }
    clamp_add(0, sample);
    return result;
}

void led_set(uint32_t on)
{
    volatile uint32_t *const odr = (volatile uint32_t *)0x40020014UL;

    if (on != 0U)
    {
        *odr |= 0x20U;
        return;
    }
    *odr &= ~0x20U;
}
