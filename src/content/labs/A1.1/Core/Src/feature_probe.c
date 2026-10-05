/**
 * @file    feature_probe.c
 * @brief   A1.1: identity card, feature decoding checks, core comparison table.
 */
#include "feature_probe.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "core_info.h"

#define LINE_MAX_LEN   (128U)

static FeatureProbe_WriteFn s_write;

void FeatureProbe_Init(FeatureProbe_WriteFn write)
{
    s_write = write;
}

void FeatureProbe_Printf(const char *fmt, ...)
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

static const char *yes_no(bool value)
{
    return value ? "yes" : "no";
}

/* ------------------------------------------------------------------------- */
/* Self-test: the decoders on values built from the documented fields        */
/* ------------------------------------------------------------------------- */

/** @brief CPUID built from its fields: implementer, variant, architecture, part, revision. */
static uint32_t make_cpuid(uint32_t implementer, uint32_t variant, uint32_t architecture,
                           uint32_t part, uint32_t revision)
{
    return (implementer << 24) | (variant << 20) | (architecture << 16) | (part << 4) | revision;
}

static uint32_t s_checks;
static uint32_t s_failed;

static void check(bool condition, const char *what)
{
    s_checks++;
    if (!condition)
    {
        s_failed++;
        FeatureProbe_Printf("  FAIL: %s\r\n", what);
    }
}

uint32_t FeatureProbe_SelfTest(void)
{
    CoreId id;
    char   text[32];

    s_checks = 0U;
    s_failed = 0U;

    /* CPUID of an STM32F446: read by A0.4 from a real board. */
    CoreInfo_ParseCpuid(0x410FC241UL, &id);
    check(id.implementer == 0x41U, "implementer is Arm (0x41)");
    check(id.variant == 0U, "variant 0");
    check(id.architecture == 0xFU, "architecture nibble 0xF");
    check(id.part == 0xC24U, "part 0xC24");
    check(id.revision == 1U, "revision 1");
    CoreInfo_Format(&id, text, sizeof text);
    check(strcmp(text, "Cortex-M4 r0p1") == 0, "formatted as Cortex-M4 r0p1");

    /* A value built from the fields must round-trip. */
    CoreInfo_ParseCpuid(make_cpuid(0x41U, 1U, 0xFU, 0xC27U, 1U), &id);
    check(id.part == 0xC27U && id.variant == 1U && id.revision == 1U, "Cortex-M7 r1p1 round trip");
    check(CoreInfo_Spec(0xC27U) != NULL && CoreInfo_Spec(0xC27U)->pipeline == 6U, "M7 has a 6-stage pipeline");
    check(CoreInfo_Spec(0xFFFU) == NULL, "unknown part has no table row");

    /* FPv4-SP (Cortex-M4F), FPU enabled, 4 DWT comparators + cycle counter, 8 MPU regions. */
    FeatureRegs m4f = { .mvfr0 = 0x10110021UL, .isar0 = 0x01141110UL, .dwtCtrl = 0x40000000UL,
                        .mpuType = 0x00000800UL, .cpacr = 0x00F00000UL };
    Features    f;

    CoreInfo_DecodeFeatures(&m4f, &f);
    check(f.fpuSingle && !f.fpuDouble && f.fpuEnabled, "M4F: single-precision FPU, enabled");
    check(f.hwDivide && f.cycleCounter && f.dwtComparators == 4U && f.mpuRegions == 8U,
          "M4F: divide, cycle counter, 4 comparators, 8 MPU regions");

    /* The same FPU, not yet enabled: CPACR is 0 after reset. */
    m4f.cpacr = 0U;
    CoreInfo_DecodeFeatures(&m4f, &f);
    check(f.fpuSingle && !f.fpuEnabled, "FPU present but disabled when CPACR = 0");

    /* Only CP10 enabled is NOT enough: both CP10 and CP11 must be full access. */
    m4f.cpacr = 0x00300000UL;
    CoreInfo_DecodeFeatures(&m4f, &f);
    check(!f.fpuEnabled, "CP10 alone does not enable the FPU");

    /* A core with no FPU, no divide, no MPU, no cycle counter (Cortex-M0 style). */
    const FeatureRegs m0 = { .mvfr0 = 0U, .isar0 = 0U, .dwtCtrl = 0x02000000UL, .mpuType = 0U, .cpacr = 0U };

    CoreInfo_DecodeFeatures(&m0, &f);
    check(!f.fpuSingle && !f.hwDivide && !f.cycleCounter && f.mpuRegions == 0U,
          "M0 style: no FPU, divide, cycle counter or MPU");

    /* FPv5 double precision (Cortex-M7 with DP FPU). */
    const FeatureRegs m7 = { .mvfr0 = 0x10110221UL, .isar0 = 0x01141110UL, .dwtCtrl = 0x40000000UL,
                             .mpuType = 0x00001000UL, .cpacr = 0x00F00000UL };

    CoreInfo_DecodeFeatures(&m7, &f);
    check(f.fpuDouble && f.mpuRegions == 16U, "M7 DP: double-precision FPU, 16 MPU regions");
    check(strcmp(CoreInfo_FpuName(&f), "single + double precision") == 0, "M7 DP: FPU name");

    FeatureProbe_Printf("feature self-test: %lu/%lu checks passed\r\n",
                        (unsigned long)(s_checks - s_failed), (unsigned long)s_checks);
    return s_failed;
}

/* ------------------------------------------------------------------------- */
/* Identity card                                                             */
/* ------------------------------------------------------------------------- */

void FeatureProbe_Report(const FeatureSources *src)
{
    /* Each *pointer is exactly one load from the bus (volatile, A0.4). */
    const uint32_t cpuid = *src->cpuid;
    const FeatureRegs regs = {
        .mvfr0   = *src->mvfr0,
        .isar0   = *src->isar0,
        .dwtCtrl = *src->dwtCtrl,
        .mpuType = *src->mpuType,
        .cpacr   = *src->cpacr,
    };
    CoreId   id;
    Features f;
    char     name[32];

    CoreInfo_ParseCpuid(cpuid, &id);
    CoreInfo_DecodeFeatures(&regs, &f);
    CoreInfo_Format(&id, name, sizeof name);

    FeatureProbe_Printf("\r\n--- Core identity (CPUID = 0x%08lX) ---\r\n", (unsigned long)cpuid);
    FeatureProbe_Printf("implementer  0x%02X (%s)\r\n", (unsigned)id.implementer,
                        (id.implementer == 0x41U) ? "Arm" : "other vendor");
    FeatureProbe_Printf("architecture 0x%X (%s)\r\n", (unsigned)id.architecture,
                        CoreInfo_ArchitectureField(id.architecture));
    FeatureProbe_Printf("part         0x%03X\r\n", (unsigned)id.part);
    FeatureProbe_Printf("core         %s\r\n", name);

    FeatureProbe_Printf("\r\n--- Features read from the core ---\r\n");
    FeatureProbe_Printf("FPU            %s (MVFR0 = 0x%08lX), enabled: %s (CPACR = 0x%08lX)\r\n",
                        CoreInfo_FpuName(&f), (unsigned long)regs.mvfr0, yes_no(f.fpuEnabled),
                        (unsigned long)regs.cpacr);
    FeatureProbe_Printf("hardware divide %s (ID_ISAR0 = 0x%08lX)\r\n", yes_no(f.hwDivide),
                        (unsigned long)regs.isar0);
    FeatureProbe_Printf("DWT            cycle counter: %s, %u comparators (CTRL = 0x%08lX)\r\n",
                        yes_no(f.cycleCounter), (unsigned)f.dwtComparators, (unsigned long)regs.dwtCtrl);
    FeatureProbe_Printf("MPU            %u regions (TYPE = 0x%08lX)\r\n", (unsigned)f.mpuRegions,
                        (unsigned long)regs.mpuType);

    const CoreSpec *spec = CoreInfo_Spec(id.part);

    FeatureProbe_Printf("\r\n--- Compared with the table row for this core ---\r\n");
    if (spec == NULL)
    {
        FeatureProbe_Printf("part 0x%03X is not in the table\r\n", (unsigned)id.part);
        return;
    }
    FeatureProbe_Printf("%s: %s, %u-stage pipeline, up to %u interrupt lines\r\n", spec->name,
                        spec->architecture, (unsigned)spec->pipeline, (unsigned)spec->maxIrqs);
    FeatureProbe_Printf("table: divide %s, DSP %s, FPU %s, bit-band %s, TrustZone %s\r\n",
                        yes_no(spec->hwDivide), yes_no(spec->dsp), spec->fpu, yes_no(spec->bitBand),
                        yes_no(spec->trustZone));
    FeatureProbe_Printf("registers agree on divide: %s\r\n", (spec->hwDivide == f.hwDivide) ? "yes" : "NO");
}

void FeatureProbe_PrintTable(void)
{
    FeatureProbe_Printf("\r\n--- Cortex-M comparison table ---\r\n");
    FeatureProbe_Printf("%-11s %-17s %4s %5s %-3s %-3s %-11s %-4s %-3s\r\n", "core", "architecture",
                        "pipe", "IRQs", "div", "DSP", "FPU", "bitb", "TZ");
    for (size_t i = 0U; i < CoreInfo_SpecCount(); i++)
    {
        const CoreSpec *s = CoreInfo_SpecAt(i);

        FeatureProbe_Printf("%-11s %-17s %4u %5u %-3s %-3s %-11s %-4s %-3s\r\n", s->name, s->architecture,
                            (unsigned)s->pipeline, (unsigned)s->maxIrqs, yes_no(s->hwDivide),
                            yes_no(s->dsp), s->fpu, yes_no(s->bitBand), yes_no(s->trustZone));
    }
}
