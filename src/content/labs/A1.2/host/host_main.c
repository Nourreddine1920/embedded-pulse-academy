/**
 * @file    host_main.c
 * @brief   A1.2: runs the register model on a PC.
 *
 * Self-test, ADDS/SUBS flag tables, the masking table, and a cross-check of the
 * flag arithmetic against the compiler's own overflow built-ins on many operand
 * pairs. Nothing here touches a core register: the board compares this model
 * with the real core in regs_hw.c.
 */
#include <stdio.h>

#include "regs_report.h"

static void host_write(const char *text)
{
    (void)fputs(text, stdout);
}

/** @brief Simple 32-bit LCG: deterministic operands for the cross-check. */
static uint32_t next_random(uint32_t *state)
{
    *state = (*state * 1664525UL) + 1013904223UL;
    return *state;
}

/** @brief Compares Flags_Add/Flags_Sub with GCC's __builtin_*_overflow on many pairs. */
static unsigned cross_check(void)
{
    uint32_t     seed   = 12345UL;
    unsigned     failed = 0U;
    const unsigned total = 100000U;

    for (unsigned i = 0U; i < total; i++)
    {
        /* Mix random values with the interesting edges. */
        uint32_t a = next_random(&seed);
        uint32_t b = next_random(&seed);

        if ((i % 4U) == 0U)
        {
            a = (a & 1U) != 0U ? 0x7FFFFFFFUL : 0x80000000UL;
        }
        if ((i % 5U) == 0U)
        {
            b = (b & 1U) != 0U ? 1UL : 0xFFFFFFFFUL;
        }

        int32_t signedResult;
        uint32_t unsignedResult;
        const Flags fa = Flags_Add(a, b);
        const bool  vAdd = __builtin_add_overflow((int32_t)a, (int32_t)b, &signedResult);
        const bool  cAdd = __builtin_add_overflow(a, b, &unsignedResult);

        if ((fa.v != vAdd) || (fa.c != cAdd))
        {
            failed++;
        }

        const Flags fs = Flags_Sub(a, b);
        const bool  vSub = __builtin_sub_overflow((int32_t)a, (int32_t)b, &signedResult);
        const bool  noBorrow = !__builtin_sub_overflow(a, b, &unsignedResult);   /* C = NOT borrow */

        if ((fs.v != vSub) || (fs.c != noBorrow))
        {
            failed++;
        }
    }
    Regs_Printf("cross-check of %u operand pairs against __builtin_*_overflow: %u mismatches\r\n", total, failed);
    return failed;
}

int main(void)
{
    Regs_Init(host_write);
    Regs_Printf("=== A1.2 Core registers (host model) ===\r\n");

    unsigned failures = Regs_SelfTest();

    Regs_PrintRegisterRoles();
    Regs_PrintFlagTable();
    Regs_PrintMaskTable();
    Regs_Printf("\r\n");
    failures += cross_check();
    Regs_PrintXpsr("example:", 0x61000016UL);
    Regs_PrintControl(0x3UL);

    return (failures == 0U) ? 0 : 1;
}
