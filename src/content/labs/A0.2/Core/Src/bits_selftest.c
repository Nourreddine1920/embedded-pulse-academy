/**
 * @file    bits_selftest.c
 * @brief   A0.2: self-test of bits.h. Expected values are written in hex so
 *          you can check each one by hand against the lesson.
 */
#include "bits_selftest.h"

#include "bits.h"
#include "xray.h"

static uint32_t s_checks;
static uint32_t s_failures;

/** @brief Records one check; prints the expression when it fails. */
static void check(bool ok, const char *expr, int line)
{
    s_checks++;
    if (!ok)
    {
        s_failures++;
        Xray_Printf("  FAIL (line %d): %s\r\n", line, expr);
    }
}

#define CHECK(expr)  check((expr), #expr, __LINE__)

uint32_t BitsSelfTest_Run(void)
{
    s_checks   = 0U;
    s_failures = 0U;

    /* Masks */
    CHECK(BIT(0)  == 0x00000001UL);
    CHECK(BIT(31) == 0x80000000UL);
    CHECK(FIELD_MASK(2U, 10U) == 0x00000C00UL);      /* GPIO MODER, pin 5    */
    CHECK(FIELD_MASK(4U, 8U)  == 0x00000F00UL);      /* GPIO AFRL, pin 2     */
    CHECK(FIELD_MASK(32U, 0U) == 0xFFFFFFFFUL);      /* no 1UL << 32 UB      */

    /* Whole-bit operations */
    CHECK(bits_set(0x00000000UL, BIT(5))    == 0x00000020UL);
    CHECK(bits_clear(0xFFFFFFFFUL, BIT(5))  == 0xFFFFFFDFUL);
    CHECK(bits_toggle(0x00000020UL, BIT(5)) == 0x00000000UL);
    CHECK(bits_toggle(0x00000000UL, BIT(5)) == 0x00000020UL);
    CHECK(bits_all_set(0x000000F0UL, 0x30UL));
    CHECK(!bits_all_set(0x000000F0UL, 0x0CUL | 0x10UL));
    CHECK(bits_any_set(0x000000F0UL, 0x0CUL | 0x10UL));
    CHECK(!bits_any_set(0x000000F0UL, 0x0FUL));

    /* Fields: reset value of GPIOA->MODER is 0xA8000000 on STM32F4 */
    CHECK(field_get(0xA8000000UL, FIELD_MASK(2U, 30U), 30U) == 2U);   /* PA15 = AF */
    CHECK(field_set(0xA8000000UL, FIELD_MASK(2U, 10U), 10U, 1U) == 0xA8000400UL);
    CHECK(field_set(0xA8000C00UL, FIELD_MASK(2U, 10U), 10U, 1U) == 0xA8000400UL);
    CHECK(field_set(0x00000000UL, FIELD_MASK(2U, 10U), 10U, 7U) == 0x00000C00UL); /* 7 clipped to 3 */

    /* Signed fields */
    CHECK(field_get_signed(0x00000F00UL, 8U, 4U) == -1);
    CHECK(field_get_signed(0x00000700UL, 8U, 4U) == 7);
    CHECK(field_get_signed(0x00000800UL, 8U, 4U) == -8);
    CHECK(field_get_signed(0x80000000UL, 16U, 16U) == -32768);

    /* Tricks */
    CHECK(bits_count(0x00000000UL) == 0U);
    CHECK(bits_count(0xA8000400UL) == 4U);
    CHECK(bits_count(0xFFFFFFFFUL) == 32U);
    CHECK(bits_lowest_index(0x00002000UL) == 13U);
    CHECK(bits_lowest_index(0U) == 32U);
    CHECK(bits_lowest(0x00000068UL) == 0x00000008UL);
    CHECK(bits_is_pow2(1U) && bits_is_pow2(0x80000000UL));
    CHECK(!bits_is_pow2(0U) && !bits_is_pow2(0x00000006UL));

    /* reg_modify on a plain variable standing in for a register */
    {
        volatile uint32_t fake = 0xA8000C00UL;
        reg_modify(&fake, FIELD_MASK(2U, 10U), 1UL << 10U);
        CHECK(fake == 0xA8000400UL);
    }

    Xray_Printf("bits.h self-test: %u/%u checks passed\r\n",
                (unsigned)(s_checks - s_failures), (unsigned)s_checks);
    return s_failures;
}
