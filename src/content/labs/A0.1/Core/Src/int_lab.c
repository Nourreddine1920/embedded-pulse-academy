/**
 * @file    int_lab.c
 * @brief   A0.1 lab: fixed-width types and integer-promotion pitfalls.
 *
 * Every test prints the "buggy" expression result next to the "fixed" one.
 * All buggy expressions here are well-defined C (wrong, but not undefined),
 * so the output is deterministic. The one undefined-behaviour demo is
 * compiled only when LAB_RUN_UB_DEMO is defined (see exercise 🔴).
 */
#include "int_lab.h"

#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>

/* ------------------------------------------------------------------------- */
/* Configuration                                                             */
/* ------------------------------------------------------------------------- */

#define LAB_LINE_LEN          (128U)  /**< Max length of one report line.    */
#define LAB_SUM_BUFFER_LEN    (200U)  /**< Must stay <= 255 for the u8 loop. */
#define LAB_TICK_TIMEOUT      (10U)   /**< Timeout used in the wrap test.    */

/** @brief Names the static type of an expression (C11 _Generic). */
#define TYPE_NAME(x) _Generic((x),                 \
    char:               "char",                    \
    signed char:        "signed char",             \
    unsigned char:      "unsigned char",           \
    short:              "short",                   \
    unsigned short:     "unsigned short",          \
    int:                "int",                     \
    unsigned int:       "unsigned int",            \
    long:               "long",                    \
    unsigned long:      "unsigned long",           \
    long long:          "long long",               \
    unsigned long long: "unsigned long long",      \
    default:            "other")

/** @brief Small enum used to show the ARM EABI "short enum" rule. */
typedef enum
{
    LAB_STATE_IDLE = 0,
    LAB_STATE_RUN  = 1
} LabState;

/* ------------------------------------------------------------------------- */
/* Test inputs                                                               */
/* ------------------------------------------------------------------------- */
/* volatile stops the compiler from folding the tests at compile time, so the
 * CPU really executes each expression and you can single-step it.          */

static volatile uint8_t  s_u8Pattern   = 0x0FU;   /**< ~ test input.        */
static volatile int32_t  s_i32Negative = -1;      /**< Signed side.         */
static volatile uint32_t s_u32One      = 1U;      /**< Unsigned side.       */
static volatile uint16_t s_tickStart   = 65530U;  /**< 16-bit timer start.  */
static volatile uint16_t s_tickNow     = 5U;      /**< ...after wrapping.   */
static volatile uint8_t  s_rawMsb      = 0xFFU;   /**< Sensor MSB (-200).   */
static volatile uint8_t  s_rawLsb      = 0x38U;   /**< Sensor LSB (-200).   */

static uint8_t s_sumBuffer[LAB_SUM_BUFFER_LEN];
/** Runtime length: if it were a constant, GCC could prove the u8 index never
 *  wraps and generate identical code for both loops (try it: see exercise). */
static volatile uint32_t s_sumLength = LAB_SUM_BUFFER_LEN;

/* ------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* ------------------------------------------------------------------------- */

static IntLab_WriteFn s_write;

/** @brief printf-style output through the injected write function. */
static void lab_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

static void lab_printf(const char *fmt, ...)
{
    char line[LAB_LINE_LEN];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    s_write(line);
}

/** @brief Prints the verdict of one test and returns whether it passed. */
static bool lab_verdict(bool fixedIsCorrect)
{
    lab_printf("      -> %s\r\n", fixedIsCorrect ? "PASS" : "FAIL");
    return fixedIsCorrect;
}

static const char *bool_str(bool value)
{
    return value ? "true" : "false";
}

/* ------------------------------------------------------------------------- */
/* T0: what the types really are on this target                              */
/* ------------------------------------------------------------------------- */

static void test_type_report(void)
{
    uint8_t a = 1U;

    lab_printf("[T0] Type report\r\n");
    lab_printf("     sizeof: char=%u short=%u int=%u long=%u llong=%u ptr=%u\r\n",
               (unsigned)sizeof(char), (unsigned)sizeof(short),
               (unsigned)sizeof(int), (unsigned)sizeof(long),
               (unsigned)sizeof(long long), (unsigned)sizeof(void *));
    lab_printf("     plain char is %s\r\n", (CHAR_MIN == 0) ? "UNSIGNED" : "SIGNED");
    lab_printf("     sizeof(LabState enum) = %u\r\n", (unsigned)sizeof(LabState));
    lab_printf("     int32_t      is '%s'\r\n", TYPE_NAME((int32_t)0));
    lab_printf("     uint8_t+uint8_t is '%s'\r\n", TYPE_NAME(a + a));
    lab_printf("     0xFFFFFFFF   is '%s'\r\n", TYPE_NAME(0xFFFFFFFF));
    lab_printf("     4294967295   is '%s'\r\n", TYPE_NAME(4294967295));
}

/* ------------------------------------------------------------------------- */
/* T1: bitwise NOT on a small unsigned type                                  */
/* ------------------------------------------------------------------------- */

static bool test_complement(void)
{
    uint8_t value    = s_u8Pattern;
    uint8_t inverted = (uint8_t)~value;     /* truncate back to 8 bits      */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"   /* the bug is the point   */
    bool buggy = (~value == 0xF0);          /* ~ acts on the promoted int   */
#pragma GCC diagnostic pop
    bool fixed = (inverted == 0xF0U);

    lab_printf("[T1] ~u8 == 0xF0    buggy=%-5s fixed=%-5s (~u8 as int = %d = 0x%X)\r\n",
               bool_str(buggy), bool_str(fixed), ~value, (unsigned)~value);
    return lab_verdict(fixed && !buggy);
}

/* ------------------------------------------------------------------------- */
/* T2: signed vs unsigned comparison                                         */
/* ------------------------------------------------------------------------- */

static bool test_signed_unsigned(void)
{
    int32_t  negative = s_i32Negative;
    uint32_t one      = s_u32One;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"   /* the bug is the point   */
    bool buggy = (negative < one);                /* -1 becomes 0xFFFFFFFF  */
#pragma GCC diagnostic pop
    bool fixed = (negative < 0) || ((uint32_t)negative < one);

    lab_printf("[T2] -1 < 1u        buggy=%-5s fixed=%-5s ((uint32_t)-1 = %" PRIu32 ")\r\n",
               bool_str(buggy), bool_str(fixed), (uint32_t)negative);
    return lab_verdict(fixed && !buggy);
}

/* ------------------------------------------------------------------------- */
/* T3: elapsed time with a 16-bit counter that wrapped                       */
/* ------------------------------------------------------------------------- */

static bool test_tick16_wrap(void)
{
    uint16_t start = s_tickStart;
    uint16_t now   = s_tickNow;

    int      buggyDelta = now - start;              /* int arithmetic        */
    uint16_t fixedDelta = (uint16_t)(now - start);  /* modulo 2^16           */

    bool buggy = (buggyDelta >= (int)LAB_TICK_TIMEOUT);
    bool fixed = (fixedDelta >= LAB_TICK_TIMEOUT);

    lab_printf("[T3] 16-bit elapsed buggy=%d fixed=%u (timeout %u: buggy=%s fixed=%s)\r\n",
               buggyDelta, (unsigned)fixedDelta, LAB_TICK_TIMEOUT,
               bool_str(buggy), bool_str(fixed));
    return lab_verdict(fixed && !buggy);
}

/* ------------------------------------------------------------------------- */
/* T4: rebuilding a signed 16-bit sensor value from two bytes                */
/* ------------------------------------------------------------------------- */

static bool test_sign_extension(void)
{
    uint8_t msb = s_rawMsb;
    uint8_t lsb = s_rawLsb;

    int32_t buggy = (msb << 8) | lsb;   /* int 0x0000FF38: sign bit lost   */
    /* uint16_t -> int16_t for values > INT16_MAX is implementation-defined;
     * GCC documents it as modulo 2^16, which is what every sensor driver
     * relies on. See the lesson for a fully portable alternative.         */
    int32_t fixed = (int16_t)(uint16_t)((uint16_t)(msb << 8) | lsb);

    lab_printf("[T4] 0xFF,0x38->s16 buggy=%" PRId32 " fixed=%" PRId32 "\r\n", buggy, fixed);
    return lab_verdict((fixed == -200) && (buggy != -200));
}

/* ------------------------------------------------------------------------- */
/* T5: building a 32-bit big-endian word from bytes                          */
/* ------------------------------------------------------------------------- */

static bool test_byte_assembly(void)
{
    static const uint8_t frame[4] = { 0x80U, 0x01U, 0x02U, 0x03U };

    uint32_t fixed = ((uint32_t)frame[0] << 24) | ((uint32_t)frame[1] << 16) |
                     ((uint32_t)frame[2] << 8)  |  (uint32_t)frame[3];

    lab_printf("[T5] bytes->u32     fixed=0x%08" PRIX32 "\r\n", fixed);

#if defined(LAB_RUN_UB_DEMO)
    /* frame[0] is promoted to int; 0x80 << 24 does not fit in int -> UB.
     * With -fsanitize=undefined -fsanitize-undefined-trap-on-error this
     * line executes a UDF instruction and the MCU faults.                 */
    volatile uint8_t top = frame[0];
    uint32_t ub = (uint32_t)(top << 24);
    lab_printf("     UB variant       = 0x%08" PRIX32 " (you should not see this with UBSan)\r\n", ub);
#endif

    return lab_verdict(fixed == 0x80010203UL);
}

/* ------------------------------------------------------------------------- */
/* T6: cost of a uint8_t loop counter                                        */
/* ------------------------------------------------------------------------- */

__attribute__((noinline))
static uint32_t sum_with_u8_index(const uint8_t *data, uint32_t length)
{
    uint32_t sum = 0U;
    for (uint8_t i = 0U; i < length; i++)   /* i is re-truncated every pass */
    {
        sum += data[i];
    }
    return sum;
}

__attribute__((noinline))
static uint32_t sum_with_u32_index(const uint8_t *data, uint32_t length)
{
    uint32_t sum = 0U;
    for (uint32_t i = 0U; i < length; i++)
    {
        sum += data[i];
    }
    return sum;
}

static bool test_loop_cost(IntLab_CycleFn cycles)
{
    if (cycles == NULL)
    {
        lab_printf("[T6] loop cost      skipped (no cycle counter)\r\n");
        return true;
    }

    for (uint32_t i = 0U; i < LAB_SUM_BUFFER_LEN; i++)
    {
        s_sumBuffer[i] = (uint8_t)i;
    }

    const uint32_t length = s_sumLength;

    uint32_t t0    = cycles();
    uint32_t sum8  = sum_with_u8_index(s_sumBuffer, length);
    uint32_t t1    = cycles();
    uint32_t sum32 = sum_with_u32_index(s_sumBuffer, length);
    uint32_t t2    = cycles();

    /* Unsigned subtraction is wrap-safe: the same idea as T3, at 32 bits. */
    lab_printf("[T6] loop cost      u8 index=%" PRIu32 " cycles, u32 index=%" PRIu32 " cycles\r\n",
               t1 - t0, t2 - t1);
    return lab_verdict(sum8 == sum32);
}

/* ------------------------------------------------------------------------- */
/* Public API                                                                */
/* ------------------------------------------------------------------------- */

bool IntLab_Run(IntLab_WriteFn write, IntLab_CycleFn cycles)
{
    bool allPassed = true;

    s_write = write;
    lab_printf("\r\n=== A0.1 Integer lab ===\r\n");

    test_type_report();
    allPassed &= test_complement();
    allPassed &= test_signed_unsigned();
    allPassed &= test_tick16_wrap();
    allPassed &= test_sign_extension();
    allPassed &= test_byte_assembly();
    allPassed &= test_loop_cost(cycles);

    lab_printf("=== %s ===\r\n", allPassed ? "ALL PASS" : "SOME TESTS FAILED");
    return allPassed;
}
