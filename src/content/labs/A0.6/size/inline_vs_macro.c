/**
 * @file    inline_vs_macro.c
 * @brief   A0.6: what the compiler makes of a macro vs a static inline function.
 *
 * Build for Cortex-M4 and read the disassembly (see the lesson, advanced layer):
 *   arm-none-eabi-gcc -std=c11 -mcpu=cortex-m4 -mthumb -mfloat-abi=hard
 *       -mfpu=fpv4-sp-d16 -Os -Wall -Wextra -I../Core/Inc -c inline_vs_macro.c
 *   arm-none-eabi-objdump -d inline_vs_macro.o
 */
#include <stdint.h>

#include "hygiene.h"
#include "legacy_macros.h"

uint32_t adc_read(void);                  /* external: the compiler can't see inside */

/* Plain arguments: macro and inline give identical code. */
uint32_t max_macro(uint32_t a, uint32_t b)  { return MAX(a, b); }
uint32_t max_inline(uint32_t a, uint32_t b) { return max_u32(a, b); }

/* An argument with a side effect: the macro calls adc_read() twice. */
uint32_t peak_macro(uint32_t limit)  { return MAX(adc_read(), limit); }
uint32_t peak_inline(uint32_t limit) { return max_u32(adc_read(), limit); }

/* A bigger helper used from four places: does the compiler inline it? */
static inline uint32_t crc8_step(uint32_t crc, uint32_t byte)
{
    crc ^= byte;
    for (uint32_t bit = 0U; bit < 8U; bit++)
    {
        crc = ((crc & 0x80U) != 0U) ? (((crc << 1U) ^ 0x07U) & 0xFFU) : ((crc << 1U) & 0xFFU);
    }
    return crc;
}

#ifdef FORCE_INLINE
__attribute__((always_inline))
#endif
static inline uint32_t crc8_4(const uint8_t *p)
{
    uint32_t crc = 0U;

    crc = crc8_step(crc, p[0]);
    crc = crc8_step(crc, p[1]);
    crc = crc8_step(crc, p[2]);
    crc = crc8_step(crc, p[3]);
    return crc;
}

uint32_t frame_crc_a(const uint8_t *p) { return crc8_4(p); }
uint32_t frame_crc_b(const uint8_t *p) { return crc8_4(&p[4]); }
uint32_t frame_crc_c(const uint8_t *p) { return crc8_4(&p[8]); }
uint32_t frame_crc_d(const uint8_t *p) { return crc8_4(&p[12]); }
