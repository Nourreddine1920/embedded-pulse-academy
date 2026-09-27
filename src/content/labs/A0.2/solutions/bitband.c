/**
 * @file  bitband.c
 * @brief Exercise A0.2 🔴: bit-band alias access on Cortex-M3/M4.
 */
#include <stdint.h>
#include "stm32f4xx.h"

#define BITBAND_PERIPH_REGION  (0x40000000UL)  /**< Peripheral bit-band region base */
#define BITBAND_PERIPH_ALIAS   (0x42000000UL)  /**< Its 32 MB alias region          */
#define BITBAND_WORDS_PER_BYTE (32UL)          /**< 8 bits x 4-byte alias words     */
#define BITBAND_BYTES_PER_BIT  (4UL)

/** @brief Alias word for bit @p bit of the peripheral register at @p addr. */
#define BITBAND_PERIPH(addr, bit)                                               \
    (*(volatile uint32_t *)(BITBAND_PERIPH_ALIAS +                              \
        (((uint32_t)(addr) - BITBAND_PERIPH_REGION) * BITBAND_WORDS_PER_BYTE) + \
        ((uint32_t)(bit) * BITBAND_BYTES_PER_BIT)))

#define LED_PIN  (5U)

_Static_assert((BITBAND_PERIPH_ALIAS + ((0x40020014UL - BITBAND_PERIPH_REGION) * 32UL) + (5UL * 4UL)) == 0x42400294UL,
               "GPIOA->ODR bit 5 alias must be 0x42400294");

void led_on_bitband(void)
{
    BITBAND_PERIPH(&GPIOA->ODR, LED_PIN) = 1U;   /* one STR; the bus does the RMW */
}

void led_toggle_bitband(void)
{
    BITBAND_PERIPH(&GPIOA->ODR, LED_PIN) ^= 1U;  /* LDR + EOR + STR on the alias: NOT atomic */
}
