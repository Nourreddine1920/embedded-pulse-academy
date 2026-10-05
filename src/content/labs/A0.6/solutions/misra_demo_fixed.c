/**
 * @file    misra_demo_fixed.c
 * @brief   A0.6 exercise 3: misra_demo.c made MISRA C:2012 compliant,
 *          with one documented deviation (Rule 11.4, register address).
 *
 * Changes, by rule:
 *   21.6  no <stdio.h>: results are returned, the caller decides how to log
 *   20.7  SCALE() replaced by a static inline function
 *   8.4   every external function declared in misra_demo_fixed.h
 *   8.7   g_mode only used here: now static s_mode, set via demo_set_mode()
 *   10.4  sample converted explicitly before mixing with a signed operand
 *   15.6  every if/else body is a compound statement
 *   16.4  switch has a default: clause
 *   10.1  signed >> replaced by division (the intent was "halve")
 *   14.4  if conditions are essentially Boolean
 *   17.7  return values are used or explicitly cast to void
 *   15.5  single point of exit (advisory; project policy follows it)
 */
#include "misra_demo_fixed.h"

#define CLAMP_MAX      (100)
#define SCALE_FACTOR   (10)
#define LED_PIN_MASK   (0x20UL)                 /* PA5 */

static DemoMode s_mode = MODE_PASS;

static inline int32_t scale(int32_t x)
{
    return x * SCALE_FACTOR;
}

static int32_t clamp_add(int32_t offset, uint8_t sample)
{
    int32_t total = offset + (int32_t)sample;   /* 10.4: one essential type */

    if (total > CLAMP_MAX)
    {
        total = CLAMP_MAX;                      /* 15.6: braces */
    }
    return total;
}

void demo_set_mode(DemoMode mode)
{
    s_mode = mode;
}

int32_t process(uint8_t sample, int32_t offset)
{
    int32_t result = clamp_add(offset, sample);

    switch (s_mode)
    {
        case MODE_SCALE:
            result = scale(result + 1);
            break;
        case MODE_HALVE:
            result = result / 2;                /* 10.1: no shift on a signed value */
            break;
        default:                                /* 16.4 */
            /* MODE_PASS: result unchanged */
            break;
    }
    return result;
}

/*
 * DEVIATION D-001, MISRA C:2012 Rule 11.4 (advisory):
 *   "A conversion should not be performed between a pointer to object and an integer type."
 * Reason:  memory-mapped I/O. GPIOA_ODR is a hardware register at a fixed
 *          address (RM0390: GPIOA base 0x4002 0000 + ODR offset 0x14).
 * Scope:   this one cast. Production code uses the CMSIS GPIOA->ODR, where the
 *          same cast lives in the vendor header under a project-wide deviation.
 * Approved: <reviewer>, <date>.
 */
void led_set(uint32_t on)
{
    /* cppcheck-suppress misra-c2012-11.4 ; deviation D-001 */
    volatile uint32_t *const odr = (volatile uint32_t *)0x40020014UL;

    if (on != 0U)
    {
        *odr |= LED_PIN_MASK;
    }
    else
    {
        *odr &= ~LED_PIN_MASK;
    }
}
