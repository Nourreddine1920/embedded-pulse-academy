/**
 * @file    features_summary.c
 * @brief   A1.1 exercise 2: a one-line feature summary built on core_info.h,
 *          with a host test (build with -DFEATURES_SUMMARY_TEST for the PC).
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_info.h"

/**
 * @brief  Writes e.g. "FPU:sp DIV DWT:4+cyc MPU:8" into buf.
 * @return number of characters the full text needs (like snprintf).
 */
int Features_Summary(const Features *f, char *buf, size_t size)
{
    const char *fpu = f->fpuDouble ? "dp" : (f->fpuSingle ? "sp" : "none");

    return snprintf(buf, size, "FPU:%s%s DIV:%s DWT:%u%s MPU:%u", fpu,
                    (f->fpuSingle && !f->fpuEnabled) ? "(off)" : "",
                    f->hwDivide ? "yes" : "no", (unsigned)f->dwtComparators,
                    f->cycleCounter ? "+cyc" : "", (unsigned)f->mpuRegions);
}

#ifdef FEATURES_SUMMARY_TEST
int main(void)
{
    static const FeatureRegs cases[] = {
        { .mvfr0 = 0x10110021UL, .isar0 = 0x01141110UL, .dwtCtrl = 0x40000000UL, .mpuType = 0x00000800UL, .cpacr = 0x00F00000UL },
        { .mvfr0 = 0x10110021UL, .isar0 = 0x01141110UL, .dwtCtrl = 0x40000000UL, .mpuType = 0x00000800UL, .cpacr = 0x00000000UL },
        { .mvfr0 = 0x10110221UL, .isar0 = 0x01141110UL, .dwtCtrl = 0x40000000UL, .mpuType = 0x00001000UL, .cpacr = 0x00F00000UL },
        { .mvfr0 = 0x00000000UL, .isar0 = 0x00000000UL, .dwtCtrl = 0x02000000UL, .mpuType = 0x00000000UL, .cpacr = 0x00000000UL },
    };
    static const char *const expected[] = {
        "FPU:sp DIV:yes DWT:4+cyc MPU:8",
        "FPU:sp(off) DIV:yes DWT:4+cyc MPU:8",
        "FPU:dp DIV:yes DWT:4+cyc MPU:16",
        "FPU:none DIV:no DWT:0 MPU:0",
    };
    unsigned failed = 0U;

    for (size_t i = 0U; i < (sizeof cases / sizeof cases[0]); i++)
    {
        Features f;
        char     text[64];

        CoreInfo_DecodeFeatures(&cases[i], &f);
        (void)Features_Summary(&f, text, sizeof text);
        const bool same = (strcmp(text, expected[i]) == 0);

        printf("case %zu: %-40s %s\n", i, text, same ? "ok" : "FAIL");
        failed += same ? 0U : 1U;
    }
    printf("%u failed\n", failed);
    return (failed == 0U) ? 0 : 1;
}
#endif
