/**
 * @file    host_main.c
 * @brief   A1.1: runs the core decoders on a PC against SIMULATED registers.
 *
 * The register values below are built for the test (documented field layouts),
 * not read from silicon. They let you see what the decoder reports for a core
 * you may not own. The board runs the same FeatureProbe_Report() on the real
 * registers of your STM32F446.
 */
#include <stdio.h>

#include "feature_probe.h"

static void host_write(const char *text)
{
    (void)fputs(text, stdout);
}

/** @brief One simulated core: its register values. */
typedef struct
{
    const char *label;
    uint32_t    cpuid, mvfr0, isar0, dwtCtrl, mpuType, cpacr;
} SimCore;

int main(void)
{
    static const SimCore sims[] = {
        { "Cortex-M0, no FPU",         0x410CC200UL, 0x00000000UL, 0x00000000UL, 0x02000000UL, 0x00000000UL, 0x00000000UL },
        { "Cortex-M4F, FPU enabled",   0x410FC241UL, 0x10110021UL, 0x01141110UL, 0x40000000UL, 0x00000800UL, 0x00F00000UL },
        { "Cortex-M7, double FPU",     0x411FC271UL, 0x10110221UL, 0x01141110UL, 0x40000000UL, 0x00001000UL, 0x00F00000UL },
        { "Cortex-M33, FPU not yet on", 0x410FD213UL, 0x10110021UL, 0x01141110UL, 0x40000000UL, 0x00000800UL, 0x00000000UL },
    };

    FeatureProbe_Init(host_write);
    FeatureProbe_Printf("=== A1.1 Core probe (host, simulated registers) ===\r\n");

    const uint32_t failures = FeatureProbe_SelfTest();

    FeatureProbe_PrintTable();

    for (size_t i = 0U; i < (sizeof sims / sizeof sims[0]); i++)
    {
        const SimCore *s = &sims[i];
        const FeatureSources src = { .cpuid = &s->cpuid, .mvfr0 = &s->mvfr0, .isar0 = &s->isar0,
                                     .dwtCtrl = &s->dwtCtrl, .mpuType = &s->mpuType, .cpacr = &s->cpacr };

        FeatureProbe_Printf("\r\n########## simulated: %s ##########", s->label);
        FeatureProbe_Report(&src);
    }
    return (failures == 0U) ? 0 : 1;
}
