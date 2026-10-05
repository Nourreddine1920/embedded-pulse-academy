/**
 * @file    core_regs.c
 * @brief   A1.2: model of the Cortex-M special registers (see core_regs.h).
 */
#include "core_regs.h"

#include <stdio.h>

#define SIGN_BIT   (0x80000000UL)

Flags Flags_Add(uint32_t a, uint32_t b)
{
    const uint32_t result = a + b;     /* unsigned: wraps modulo 2^32, defined */
    Flags          f;

    f.n = (result & SIGN_BIT) != 0U;
    f.z = (result == 0U);
    f.c = (result < a);                                   /* wrapped: unsigned carry out */
    /* Signed overflow: the operands have the same sign and the result's sign differs. */
    f.v = (((a ^ result) & (b ^ result) & SIGN_BIT) != 0U);
    return f;
}

Flags Flags_Sub(uint32_t a, uint32_t b)
{
    const uint32_t result = a - b;
    Flags          f;

    f.n = (result & SIGN_BIT) != 0U;
    f.z = (result == 0U);
    f.c = (a >= b);                                       /* no borrow needed */
    /* Signed overflow: the operands have different signs and the result's sign differs from a. */
    f.v = (((a ^ b) & (a ^ result) & SIGN_BIT) != 0U);
    return f;
}

void Xpsr_Decode(uint32_t xpsr, XpsrFields *out)
{
    out->n   = (xpsr & (1UL << 31)) != 0U;
    out->z   = (xpsr & (1UL << 30)) != 0U;
    out->c   = (xpsr & (1UL << 29)) != 0U;
    out->v   = (xpsr & (1UL << 28)) != 0U;
    out->q   = (xpsr & (1UL << 27)) != 0U;
    out->ge  = (uint8_t)((xpsr >> 16) & 0xFUL);
    out->it  = (uint8_t)(((xpsr >> 25) & 0x3UL) | (((xpsr >> 10) & 0x3FUL) << 2));
    out->t   = (xpsr & (1UL << 24)) != 0U;
    out->isr = (uint16_t)(xpsr & 0x1FFUL);
}

const char *Exception_Name(uint16_t isr, char *scratch, unsigned scratchSize)
{
    switch (isr)
    {
        case 0U:  return "Thread mode";
        case 2U:  return "NMI";
        case 3U:  return "HardFault";
        case 4U:  return "MemManage";
        case 5U:  return "BusFault";
        case 6U:  return "UsageFault";
        case 11U: return "SVCall";
        case 12U: return "DebugMonitor";
        case 14U: return "PendSV";
        case 15U: return "SysTick";
        default:  break;
    }
    if (isr >= 16U)
    {
        (void)snprintf(scratch, scratchSize, "IRQ%u", (unsigned)(isr - 16U));
        return scratch;
    }
    return "reserved";
}

void Control_Decode(uint32_t control, ControlFields *out)
{
    out->nPriv = (control & 1UL) != 0U;
    out->spSel = (control & 2UL) != 0U;
    out->fpca  = (control & 4UL) != 0U;
}

uint8_t Priority_Encode(uint8_t level, uint8_t implementedBits)
{
    return (uint8_t)(level << (8U - implementedBits));
}

uint8_t Priority_Level(uint8_t priorityByte, uint8_t implementedBits)
{
    return (uint8_t)(priorityByte >> (8U - implementedBits));
}

uint8_t Basepri_ReadBack(uint8_t value, uint8_t implementedBits)
{
    return (uint8_t)(value & (uint8_t)(0xFFU << (8U - implementedBits)));
}

bool Irq_IsMasked(const MaskState *mask, uint8_t implementedBits, uint8_t priorityByte)
{
    if (mask->primask || mask->faultmask)
    {
        return true;     /* a configurable exception never runs while either is set */
    }
    /* BASEPRI = 0 disables the mask. Otherwise a priority number >= BASEPRI is blocked.
     * The register keeps only the implemented bits, so 0x55 behaves as 0x50 on the F446. */
    const uint8_t basepri = Basepri_ReadBack(mask->basepri, implementedBits);

    return (basepri != 0U) && (priorityByte >= basepri);
}

uint16_t Irq_MaskedLevels(const MaskState *mask, uint8_t implementedBits)
{
    uint16_t blocked = 0U;

    for (unsigned level = 0U; level < (1U << implementedBits); level++)
    {
        if (Irq_IsMasked(mask, implementedBits, Priority_Encode((uint8_t)level, implementedBits)))
        {
            blocked = (uint16_t)(blocked | (1U << level));
        }
    }
    return blocked;
}
