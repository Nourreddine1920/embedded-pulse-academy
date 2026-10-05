/**
 * @file    feature_probe.h
 * @brief   A1.1: reads the core's ID and feature registers through pointers,
 *          prints an identity card, and compares it with the core table.
 *
 * The register addresses are passed in, so the board reads the real core and a
 * PC test reads fake values for any other core (A0.4 pattern).
 */
#ifndef FEATURE_PROBE_H
#define FEATURE_PROBE_H

#include <stdint.h>

/** @brief Where to read each register. All read-only: pointers to const volatile. */
typedef struct
{
    const volatile uint32_t *cpuid;     /**< SCB->CPUID    0xE000ED00 */
    const volatile uint32_t *mvfr0;     /**< FPU->MVFR0    0xE000EF40 */
    const volatile uint32_t *isar0;     /**< SCB->ISAR[0]  0xE000ED60 */
    const volatile uint32_t *dwtCtrl;   /**< DWT->CTRL     0xE0001000 */
    const volatile uint32_t *mpuType;   /**< MPU->TYPE     0xE000ED90 */
    const volatile uint32_t *cpacr;     /**< SCB->CPACR    0xE000ED88 */
} FeatureSources;

/** @brief The real addresses of a Cortex-M3/M4/M7 (DUI 0553). */
#define FEATURE_SOURCES_CORTEX_M4                                   \
    {                                                               \
        .cpuid   = (const volatile uint32_t *)0xE000ED00UL,         \
        .mvfr0   = (const volatile uint32_t *)0xE000EF40UL,         \
        .isar0   = (const volatile uint32_t *)0xE000ED60UL,         \
        .dwtCtrl = (const volatile uint32_t *)0xE0001000UL,         \
        .mpuType = (const volatile uint32_t *)0xE000ED90UL,         \
        .cpacr   = (const volatile uint32_t *)0xE000ED88UL,         \
    }

/** @brief Writes a NUL-terminated string to the console. */
typedef void (*FeatureProbe_WriteFn)(const char *text);

/** @brief Selects the console used by every FeatureProbe_* function. */
void FeatureProbe_Init(FeatureProbe_WriteFn write);

/** @brief printf-style helper through the same console. */
void FeatureProbe_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/** @brief Decoder checks on built values. @return number of failed checks. */
uint32_t FeatureProbe_SelfTest(void);

/** @brief Reads the registers once, prints the identity card and the comparison with the table. */
void FeatureProbe_Report(const FeatureSources *src);

/** @brief Prints the whole comparison table (one row per core). */
void FeatureProbe_PrintTable(void);

#endif /* FEATURE_PROBE_H */
