/**
 * @file    hygiene_bench.c
 * @brief   A0.6 hygiene bench: macro bugs vs static inline fixes, static
 *          lifetimes, a non-reentrant static buffer, enums and static_assert.
 */
#include "hygiene_bench.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "console.h"
#include "event_counter.h"
#include "hygiene.h"
#include "legacy_macros.h"

#define ADC_LIMIT        (1000U)
#define HEX_TEXT_LEN     (11U)       /**< "0x" + 8 digits + NUL */

/* ---- Bench state: internal linkage, invisible outside this file --------- */

static uint32_t s_failures;          /* fixed versions that went wrong      */
static uint32_t s_ledOnCalls;
static uint32_t s_ledOffCalls;
static uint32_t s_adcReads;
static bool     s_fault;             /* false: nothing should pulse the LED */

/** A recorded ADC trace: each adc_read() consumes the next sample. */
static const uint16_t k_adcTrace[] = { 1650U, 2400U, 900U, 3100U };

void led_on(void)  { s_ledOnCalls++; }
void led_off(void) { s_ledOffCalls++; }

static uint32_t adc_read(void)
{
    const uint32_t sample = k_adcTrace[s_adcReads % (sizeof k_adcTrace / sizeof k_adcTrace[0])];

    s_adcReads++;
    return sample;
}

/** What a result line is: legacy results are expected to be wrong. */
typedef enum
{
    ROW_LEGACY = 0,      /* a legacy_macros.h macro: wrong on purpose */
    ROW_FIXED,           /* its hygiene.h replacement: must be right   */
    ROW_CHECK            /* any other self-check: must be right        */
} RowKind;

static const char *const k_rowTags[] = { "legacy", "fixed", "check" };
static_assert((sizeof k_rowTags / sizeof k_rowTags[0]) == ((size_t)ROW_CHECK + 1U),
              "one tag per RowKind");

/** @brief Prints one result. Fixed and check rows count as failures when wrong. */
static void report(RowKind kind, const char *expr, uint32_t got, uint32_t want)
{
    const bool ok = (got == want);

    if ((kind != ROW_LEGACY) && !ok)
    {
        s_failures++;
    }
    Console_Printf("  %-6s %-26s = %4lu  %s\r\n", k_rowTags[kind], expr,
                   (unsigned long)got, ok ? "ok" : "<- WRONG");
}

/* ---- 1. Precedence ------------------------------------------------------ */

static void demo_precedence(void)
{
    const uint32_t x = 3U;

    Console_Printf("\r\n--- 1. Precedence (x = 3): want (x+1)^2 = 16, 10*(2x) = 60 ---\r\n");
    report(ROW_LEGACY, "SQUARE(x + 1U)", SQUARE(x + 1U), 16U);            /* x + 1U*x + 1U */
    report(ROW_LEGACY, "10U * DOUBLE(x)", 10U * DOUBLE(x), 60U);          /* 10U*(x) + (x) */
    report(ROW_FIXED, "square_u32(x + 1U)", square_u32(x + 1U), 16U);
    report(ROW_FIXED, "10U * double_u32(x)", 10U * double_u32(x), 60U);
}

/* ---- 2. Double evaluation ----------------------------------------------- */

static void demo_double_evaluation(void)
{
    uint32_t peak;
    uint32_t i = 5U;
    uint32_t m;

    Console_Printf("\r\n--- 2. Double evaluation: ADC trace 1650, 2400, ...; limit 1000 ---\r\n");

    s_adcReads = 0U;
    peak = MAX(adc_read(), ADC_LIMIT);                 /* reads 1650, then AGAIN */
    report(ROW_LEGACY, "MAX(adc_read(), 1000U)", peak, 1650U);
    report(ROW_LEGACY, "adc_read() calls", s_adcReads, 1U);

    s_adcReads = 0U;
    peak = max_u32(adc_read(), ADC_LIMIT);
    report(ROW_FIXED, "max_u32(adc_read(), 1000U)", peak, 1650U);
    report(ROW_FIXED, "adc_read() calls", s_adcReads, 1U);

    m = MAX(i++, 3U);                                  /* i++ runs twice        */
    report(ROW_LEGACY, "MAX(i++, 3U), i was 5", m, 5U);
    report(ROW_LEGACY, "i afterwards", i, 6U);
    i = 5U;
    m = max_u32(i++, 3U);
    report(ROW_FIXED, "max_u32(i++, 3U)", m, 5U);
    report(ROW_FIXED, "i afterwards", i, 6U);
}

/* ---- 3. Multi-statement macros ------------------------------------------ */

static void demo_multi_statement(void)
{
    Console_Printf("\r\n--- 3. if (fault) PULSE();  with fault == false: want 0 LED calls ---\r\n");

    s_ledOnCalls = 0U;
    s_ledOffCalls = 0U;
    /* GCC's -Wall already rejects this line (-Wmultistatement-macros).
     * The warning is silenced HERE ONLY, so the bench can show the bug. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultistatement-macros"
    if (s_fault) LED_PULSE();                         /* = if (s_fault) led_on(); led_off(); */
#pragma GCC diagnostic pop
    report(ROW_LEGACY, "LED_PULSE(): led_off()", s_ledOffCalls, 0U);

    s_ledOnCalls = 0U;
    s_ledOffCalls = 0U;
    if (s_fault) LED_PULSE_SAFE();                    /* do { } while (0): one statement */
    report(ROW_FIXED, "LED_PULSE_SAFE(): off", s_ledOffCalls, 0U);

    if (s_fault)
    {
        led_pulse();                                  /* braces + a function: best */
    }
    report(ROW_FIXED, "led_pulse(): on + off", s_ledOnCalls + s_ledOffCalls, 0U);
}

/* ---- 4. static lifetime ------------------------------------------------- */

static uint32_t count_calls_auto(void)
{
    uint32_t calls = 0U;        /* automatic: created and set to 0 on EVERY call */

    calls++;
    return calls;
}

static uint32_t count_calls_static(void)
{
    static uint32_t calls = 0U; /* static: one object for the whole run, zeroed once at boot (.bss) */

    calls++;
    return calls;
}

static void demo_static_lifetime(void)
{
    uint32_t a[3];
    uint32_t s[3];

    Console_Printf("\r\n--- 4. Lifetime: local auto vs local static, 3 calls each ---\r\n");
    for (uint32_t n = 0U; n < 3U; n++)
    {
        a[n] = count_calls_auto();
        s[n] = count_calls_static();
    }
    Console_Printf("  auto   calls = %lu %lu %lu\r\n", (unsigned long)a[0], (unsigned long)a[1], (unsigned long)a[2]);
    Console_Printf("  static calls = %lu %lu %lu\r\n", (unsigned long)s[0], (unsigned long)s[1], (unsigned long)s[2]);
    report(ROW_CHECK, "static: 3rd call returns", s[2], 3U);

    for (uint32_t n = 0U; n < 3U; n++)
    {
        EventCounter_Record(EVENT_SELFTEST);          /* module state: file-scope static */
    }
    report(ROW_CHECK, "EventCounter SELFTEST", EventCounter_Get(EVENT_SELFTEST), 3U);
}

/* ---- 5. Reentrancy: a static buffer is shared by every caller ----------- */

static const char *hex_static(uint32_t value)
{
    static char text[HEX_TEXT_LEN];                   /* ONE buffer for all callers */

    (void)snprintf(text, sizeof text, "0x%04lX", (unsigned long)value);
    return text;
}

static const char *hex_to(char *text, size_t size, uint32_t value)
{
    (void)snprintf(text, size, "0x%04lX", (unsigned long)value);   /* caller owns the buffer */
    return text;
}

static void demo_reentrancy(void)
{
    char first[HEX_TEXT_LEN];
    char second[HEX_TEXT_LEN];

    Console_Printf("\r\n--- 5. Reentrancy: print 0xCAFE and 0xBEEF in one call ---\r\n");
    Console_Printf("  static buffer : %s %s  <- WRONG\r\n", hex_static(0xCAFEU), hex_static(0xBEEFU));
    Console_Printf("  caller buffer : %s %s  ok\r\n",
                   hex_to(first, sizeof first, 0xCAFEU), hex_to(second, sizeof second, 0xBEEFU));
}

/* ---- 6. Enums and build-time checks -------------------------------------- */

static void demo_enums_and_asserts(void)
{
    Console_Printf("\r\n--- 6. enum + static_assert ---\r\n");
    Console_Printf("  sizeof(EventKind) = %u byte(s)  (arm-none-eabi: 1, short enums; PC: 4)\r\n",
                   (unsigned)sizeof(EventKind));
    for (uint32_t k = 0U; k < (uint32_t)EVENT_KIND_COUNT; k++)
    {
        Console_Printf("  EventKind %lu = %s\r\n", (unsigned long)k, EventCounter_Name((EventKind)k));
    }
    Console_Printf("  FrameHeader: sizeof = %u, offsetof(timestamp) = %u\r\n",
                   (unsigned)sizeof(FrameHeader), (unsigned)offsetof(FrameHeader, timestamp));
    Console_Printf("  USART_BRR_VALUE = %lu (0x%lX)\r\n",
                   (unsigned long)USART_BRR_VALUE, (unsigned long)USART_BRR_VALUE);
    Console_Printf("  every static_assert held, or this program would not have been built\r\n");
}

uint32_t HygieneBench_Run(void)
{
    s_failures = 0U;
    demo_precedence();
    demo_double_evaluation();
    demo_multi_statement();
    demo_static_lifetime();
    demo_reentrancy();
    demo_enums_and_asserts();
    Console_Printf("\r\nHygiene bench: %lu failure(s) in the fixed versions\r\n", (unsigned long)s_failures);
    return s_failures;
}
