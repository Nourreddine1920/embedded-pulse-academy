/**
 * @file    regs_report.c
 * @brief   A1.2: self-test and tables for the register model.
 */
#include "regs_report.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define LINE_MAX_LEN   (128U)
#define PRIO_BITS      (4U)     /* STM32F446: __NVIC_PRIO_BITS */

static Regs_WriteFn s_write;

void Regs_Init(Regs_WriteFn write)
{
    s_write = write;
}

void Regs_Printf(const char *fmt, ...)
{
    char    line[LINE_MAX_LEN];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    if (s_write != NULL)
    {
        s_write(line);
    }
}

/* ------------------------------------------------------------------------- */
/* Self-test                                                                 */
/* ------------------------------------------------------------------------- */

static uint32_t s_checks;
static uint32_t s_failed;

static void check(bool condition, const char *what)
{
    s_checks++;
    if (!condition)
    {
        s_failed++;
        Regs_Printf("  FAIL: %s\r\n", what);
    }
}

static bool same_flags(Flags f, bool n, bool z, bool c, bool v)
{
    return (f.n == n) && (f.z == z) && (f.c == c) && (f.v == v);
}

uint32_t Regs_SelfTest(void)
{
    s_checks = 0U;
    s_failed = 0U;

    /* ADDS: carry is the unsigned carry out, overflow the signed one. */
    check(same_flags(Flags_Add(0x7FFFFFFFUL, 1U), true, false, false, true), "0x7FFFFFFF + 1: N V");
    check(same_flags(Flags_Add(0xFFFFFFFFUL, 1U), false, true, true, false), "0xFFFFFFFF + 1: Z C");
    check(same_flags(Flags_Add(0x80000000UL, 0x80000000UL), false, true, true, true), "0x80000000 + 0x80000000: Z C V");
    check(same_flags(Flags_Add(2U, 3U), false, false, false, false), "2 + 3: no flags");

    /* SUBS / CMP: C means "no borrow", i.e. a >= b unsigned. */
    check(same_flags(Flags_Sub(5U, 5U), false, true, true, false), "5 - 5: Z C");
    check(same_flags(Flags_Sub(0U, 1U), true, false, false, false), "0 - 1: N, borrow (C clear)");
    check(same_flags(Flags_Sub(0x80000000UL, 1U), false, false, true, true), "0x80000000 - 1: C V");
    check(same_flags(Flags_Sub(3U, 5U), true, false, false, false), "3 - 5: N, C clear");

    /* xPSR of a thread-mode function: Thumb bit set, no exception. */
    XpsrFields x;

    Xpsr_Decode(0x01000000UL, &x);
    check(x.t && (x.isr == 0U) && !x.n && !x.z && !x.c && !x.v, "xPSR 0x01000000: Thumb, thread mode");
    Xpsr_Decode(0x61000016UL, &x);
    check(x.z && x.c && !x.n && !x.v && x.isr == 22U && x.t, "xPSR 0x61000016: Z C, IRQ6 (exception 22)");
    Xpsr_Decode(0x0100000FUL, &x);
    check(x.isr == 15U, "xPSR ISR field 15 = SysTick");

    char scratch[16];

    check(Exception_Name(22U, scratch, sizeof scratch)[3] == '6', "exception 22 is IRQ6");

    ControlFields c;

    Control_Decode(0x3UL, &c);
    check(c.nPriv && c.spSel && !c.fpca, "CONTROL 3: unprivileged, PSP");
    Control_Decode(0x4UL, &c);
    check(!c.nPriv && !c.spSel && c.fpca, "CONTROL 4: FP context active");

    /* Priority encoding with 4 implemented bits. */
    check(Priority_Encode(5U, PRIO_BITS) == 0x50U, "level 5 is priority byte 0x50");
    check(Priority_Level(0xF0U, PRIO_BITS) == 15U, "priority byte 0xF0 is level 15");
    check(Basepri_ReadBack(0x55U, PRIO_BITS) == 0x50U, "BASEPRI = 0x55 reads back 0x50");

    /* Masking. */
    MaskState none = { false, false, 0U };
    MaskState prim = { true, false, 0U };
    MaskState fault = { false, true, 0U };
    MaskState base5 = { false, false, 0x50U };

    check(Irq_MaskedLevels(&none, PRIO_BITS) == 0x0000U, "no mask: nothing blocked");
    check(Irq_MaskedLevels(&prim, PRIO_BITS) == 0xFFFFU, "PRIMASK: all 16 levels blocked");
    check(Irq_MaskedLevels(&fault, PRIO_BITS) == 0xFFFFU, "FAULTMASK: all configurable levels blocked");
    check(Irq_MaskedLevels(&base5, PRIO_BITS) == 0xFFE0U, "BASEPRI = 0x50: levels 5..15 blocked, 0..4 run");
    check(!Irq_IsMasked(&base5, PRIO_BITS, 0x40U) && Irq_IsMasked(&base5, PRIO_BITS, 0x50U),
          "BASEPRI blocks priority >= its value");

    const MaskState base55 = { false, false, 0x55U };

    check(Irq_IsMasked(&base55, PRIO_BITS, 0x50U), "BASEPRI = 0x55 acts as 0x50 (low bits not implemented)");

    Regs_Printf("register model self-test: %lu/%lu checks passed\r\n",
                (unsigned long)(s_checks - s_failed), (unsigned long)s_checks);
    return s_failed;
}

/* ------------------------------------------------------------------------- */
/* Tables                                                                    */
/* ------------------------------------------------------------------------- */

static const char *flag_text(bool value)
{
    return value ? "1" : "0";
}

void Regs_PrintFlagTable(void)
{
    static const struct
    {
        uint32_t a;
        uint32_t b;
    } pairs[] = {
        { 2UL, 3UL }, { 0x7FFFFFFFUL, 1UL }, { 0xFFFFFFFFUL, 1UL }, { 0x80000000UL, 0x80000000UL },
    };

    Regs_Printf("\r\n--- ADDS a, b ---\r\n");
    Regs_Printf("%-10s %-10s  N Z C V\r\n", "a", "b");
    for (unsigned i = 0U; i < (sizeof pairs / sizeof pairs[0]); i++)
    {
        const Flags f = Flags_Add(pairs[i].a, pairs[i].b);

        Regs_Printf("0x%08lX 0x%08lX  %s %s %s %s\r\n", (unsigned long)pairs[i].a, (unsigned long)pairs[i].b,
                    flag_text(f.n), flag_text(f.z), flag_text(f.c), flag_text(f.v));
    }

    static const struct
    {
        uint32_t a;
        uint32_t b;
    } subs[] = {
        { 5UL, 5UL }, { 3UL, 5UL }, { 0UL, 1UL }, { 0x80000000UL, 1UL },
    };

    Regs_Printf("\r\n--- SUBS a, b (CMP a, b) ---\r\n");
    Regs_Printf("%-10s %-10s  N Z C V\r\n", "a", "b");
    for (unsigned i = 0U; i < (sizeof subs / sizeof subs[0]); i++)
    {
        const Flags f = Flags_Sub(subs[i].a, subs[i].b);

        Regs_Printf("0x%08lX 0x%08lX  %s %s %s %s\r\n", (unsigned long)subs[i].a, (unsigned long)subs[i].b,
                    flag_text(f.n), flag_text(f.z), flag_text(f.c), flag_text(f.v));
    }
}

void Regs_PrintMaskTable(void)
{
    static const struct
    {
        const char *name;
        MaskState   state;
    } cases[] = {
        { "none            ", { false, false, 0x00U } },
        { "BASEPRI = 0x10  ", { false, false, 0x10U } },
        { "BASEPRI = 0x50  ", { false, false, 0x50U } },
        { "BASEPRI = 0xF0  ", { false, false, 0xF0U } },
        { "PRIMASK = 1     ", { true, false, 0x00U } },
        { "FAULTMASK = 1   ", { false, true, 0x00U } },
    };

    Regs_Printf("\r\n--- Which priority levels run (R) or are blocked (-), 4 implemented bits ---\r\n");
    Regs_Printf("                  level: 0 1 2 3 4 5 6 7 8 9 A B C D E F\r\n");
    for (unsigned i = 0U; i < (sizeof cases / sizeof cases[0]); i++)
    {
        const uint16_t blocked = Irq_MaskedLevels(&cases[i].state, PRIO_BITS);
        char           row[40];
        unsigned       n = 0U;

        for (unsigned level = 0U; level < 16U; level++)
        {
            row[n++] = ((blocked >> level) & 1U) != 0U ? '-' : 'R';
            row[n++] = ' ';
        }
        row[n] = '\0';
        Regs_Printf("%s       %s\r\n", cases[i].name, row);
    }
}

void Regs_PrintRegisterRoles(void)
{
    Regs_Printf("\r\n--- Registers and their AAPCS roles ---\r\n");
    Regs_Printf("r0-r3    arguments and return value; the callee may overwrite them\r\n");
    Regs_Printf("r4-r11   callee-saved: a function that uses them must restore them\r\n");
    Regs_Printf("r12      scratch (IP): the linker may use it in veneers\r\n");
    Regs_Printf("r13 SP   MSP or PSP; AAPCS needs 8-byte alignment at calls\r\n");
    Regs_Printf("r14 LR   return address (or EXC_RETURN in a handler)\r\n");
    Regs_Printf("r15 PC   bit 0 is always read as 0; branches need bit 0 = 1 (Thumb)\r\n");
}

void Regs_PrintXpsr(const char *label, uint32_t xpsr)
{
    XpsrFields x;
    char       scratch[16];

    Xpsr_Decode(xpsr, &x);
    Regs_Printf("%s xPSR = 0x%08lX  N=%u Z=%u C=%u V=%u Q=%u T=%u ISR=%u (%s)\r\n", label, (unsigned long)xpsr,
                (unsigned)x.n, (unsigned)x.z, (unsigned)x.c, (unsigned)x.v, (unsigned)x.q, (unsigned)x.t,
                (unsigned)x.isr, Exception_Name(x.isr, scratch, sizeof scratch));
}

void Regs_PrintControl(uint32_t control)
{
    ControlFields c;

    Control_Decode(control, &c);
    Regs_Printf("CONTROL = 0x%08lX  nPRIV=%u (%s)  SPSEL=%u (%s)  FPCA=%u\r\n", (unsigned long)control,
                (unsigned)c.nPriv, c.nPriv ? "unprivileged" : "privileged", (unsigned)c.spSel,
                c.spSel ? "PSP" : "MSP", (unsigned)c.fpca);
}
