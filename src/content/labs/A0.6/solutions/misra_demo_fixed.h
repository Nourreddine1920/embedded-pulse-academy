/**
 * @file    misra_demo_fixed.h
 * @brief   A0.6 exercise 3: public interface of the compliant module (Rule 8.4).
 */
#ifndef MISRA_DEMO_FIXED_H
#define MISRA_DEMO_FIXED_H

#include <stdint.h>

/** Operating modes: an enum instead of magic 0/1 (and a default: branch). */
typedef enum
{
    MODE_SCALE = 0,
    MODE_HALVE = 1,
    MODE_PASS  = 2
} DemoMode;

void    demo_set_mode(DemoMode mode);
int32_t process(uint8_t sample, int32_t offset);
void    led_set(uint32_t on);

#endif /* MISRA_DEMO_FIXED_H */
