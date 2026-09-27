/**
 * @file    host_main.c
 * @brief   A0.2: runs the bits.h self-test and an X-ray demo on a PC.
 */
#include <stdio.h>

#include "bits.h"
#include "bits_selftest.h"
#include "xray.h"

#define MODER_RESET_GPIOA   (0xA8000000UL)   /**< RM0390: GPIOA_MODER reset value */
#define LED_PIN             (5U)
#define MODER_OUTPUT        (1U)

static void host_write(const char *text)
{
    fputs(text, stdout);
}

int main(void)
{
    Xray_Init(host_write);

    const uint32_t failures = BitsSelfTest_Run();

    /* Simulate configuring PA5 as an output on the reset value of MODER. */
    const uint32_t before = MODER_RESET_GPIOA;
    const uint32_t after  = field_set(before, FIELD_MASK(2U, LED_PIN * 2U), LED_PIN * 2U, MODER_OUTPUT);

    Xray_Ruler();
    Xray_Diff("MODER (sim)", before, after);

    return (failures == 0U) ? 0 : 1;
}
