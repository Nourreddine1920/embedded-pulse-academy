/**
 * @file    core_info.c
 * @brief   A1.1: CPUID and feature-register decoding, and the core table.
 */
#include "core_info.h"

#include <stdio.h>

#define CPUID_IMPLEMENTER_POS   (24U)
#define CPUID_VARIANT_POS       (20U)
#define CPUID_ARCHITECTURE_POS  (16U)
#define CPUID_PART_POS          (4U)
#define NIBBLE_MASK             (0xFUL)
#define PART_MASK               (0xFFFUL)

#define MVFR0_SINGLE_POS        (4U)    /* 0 = none, non-zero = single precision */
#define MVFR0_DOUBLE_POS        (8U)    /* 0 = none, non-zero = double precision */
#define ISAR0_DIVIDE_POS        (24U)   /* 0 = none, 1 = SDIV/UDIV               */
#define DWT_CTRL_NUMCOMP_POS    (28U)
#define DWT_CTRL_NOCYCCNT_MSK   (1UL << 25)
#define MPU_TYPE_DREGION_POS    (8U)
#define MPU_TYPE_DREGION_MASK   (0xFFUL)
#define CPACR_CP10_CP11_MSK     (0xFUL << 20)   /* CP11 [23:22] and CP10 [21:20]  */
#define CPACR_CP10_CP11_FULL    (0xFUL << 20)   /* both fields 0b11 = full access */

/* One row per core. Source: Arm Technical Reference Manuals and DUI 0553 / DUI 0662.
 * The lesson says which cells were also confirmed by compiling for the core. */
static const CoreSpec k_cores[] = {
    /* part   name          architecture        pipe irqs  div    dsp    fpu             bitband tz     */
    { 0xC20U, "Cortex-M0",  "ARMv6-M",          3U,  32U,  false, false, "none",         false,  false },
    { 0xC60U, "Cortex-M0+", "ARMv6-M",          2U,  32U,  false, false, "none",         false,  false },
    { 0xC23U, "Cortex-M3",  "ARMv7-M",          3U,  240U, true,  false, "none",         true,   false },
    { 0xC24U, "Cortex-M4",  "ARMv7E-M",         3U,  240U, true,  true,  "opt. SP",      true,   false },
    { 0xC27U, "Cortex-M7",  "ARMv7E-M",         6U,  240U, true,  true,  "opt. SP/DP",   false,  false },
    { 0xD21U, "Cortex-M33", "ARMv8-M Mainline", 3U,  480U, true,  true,  "opt. SP",      false,  true  },
};

void CoreInfo_ParseCpuid(uint32_t cpuid, CoreId *out)
{
    out->implementer  = (uint8_t)(cpuid >> CPUID_IMPLEMENTER_POS);
    out->variant      = (uint8_t)((cpuid >> CPUID_VARIANT_POS) & NIBBLE_MASK);
    out->architecture = (uint8_t)((cpuid >> CPUID_ARCHITECTURE_POS) & NIBBLE_MASK);
    out->part         = (uint16_t)((cpuid >> CPUID_PART_POS) & PART_MASK);
    out->revision     = (uint8_t)(cpuid & NIBBLE_MASK);
}

const char *CoreInfo_Name(uint16_t part)
{
    const CoreSpec *spec = CoreInfo_Spec(part);

    return (spec != NULL) ? spec->name : "unknown core";
}

const char *CoreInfo_ArchitectureField(uint8_t architecture)
{
    switch (architecture)
    {
        case 0xCU: return "ARMv6-M";
        case 0xFU: return "ARMv7-M or ARMv8-M: read the ID registers";
        default:   return "other";
    }
}

void CoreInfo_Format(const CoreId *id, char *buf, size_t size)
{
    (void)snprintf(buf, size, "%s r%up%u", CoreInfo_Name(id->part),
                   (unsigned)id->variant, (unsigned)id->revision);
}

void CoreInfo_DecodeFeatures(const FeatureRegs *regs, Features *out)
{
    out->fpuSingle      = ((regs->mvfr0 >> MVFR0_SINGLE_POS) & NIBBLE_MASK) != 0U;
    out->fpuDouble      = ((regs->mvfr0 >> MVFR0_DOUBLE_POS) & NIBBLE_MASK) != 0U;
    out->fpuEnabled     = (regs->cpacr & CPACR_CP10_CP11_MSK) == CPACR_CP10_CP11_FULL;
    out->hwDivide       = ((regs->isar0 >> ISAR0_DIVIDE_POS) & NIBBLE_MASK) != 0U;
    out->cycleCounter   = (regs->dwtCtrl & DWT_CTRL_NOCYCCNT_MSK) == 0U;
    out->dwtComparators = (uint8_t)(regs->dwtCtrl >> DWT_CTRL_NUMCOMP_POS);
    out->mpuRegions     = (uint8_t)((regs->mpuType >> MPU_TYPE_DREGION_POS) & MPU_TYPE_DREGION_MASK);
}

const char *CoreInfo_FpuName(const Features *features)
{
    if (features->fpuDouble)
    {
        return "single + double precision";
    }
    return features->fpuSingle ? "single precision" : "none";
}

const CoreSpec *CoreInfo_Spec(uint16_t part)
{
    for (size_t i = 0U; i < (sizeof k_cores / sizeof k_cores[0]); i++)
    {
        if (k_cores[i].part == part)
        {
            return &k_cores[i];
        }
    }
    return NULL;
}

size_t CoreInfo_SpecCount(void)
{
    return sizeof k_cores / sizeof k_cores[0];
}

const CoreSpec *CoreInfo_SpecAt(size_t index)
{
    return (index < CoreInfo_SpecCount()) ? &k_cores[index] : NULL;
}
