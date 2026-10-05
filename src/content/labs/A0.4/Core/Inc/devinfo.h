/**
 * @file    devinfo.h
 * @brief   A0.4: reads the chip's identification registers through pointers.
 *
 * The four sources are passed in as pointers, so the same code reads the
 * real addresses on the STM32 and a fake memory block in a unit test.
 */
#ifndef DEVINFO_H
#define DEVINFO_H

#include <stdint.h>

#include "mmio.h"

/** @brief Where to read each item. All read-only: pointers to const volatile. */
typedef struct
{
    const volatile uint32_t *cpuid;      /**< SCB->CPUID       0xE000ED00 */
    const volatile uint32_t *idcode;     /**< DBGMCU->IDCODE   0xE0042000 */
    const volatile uint32_t *uid;        /**< UID[0..2]        0x1FFF7A10 */
    const volatile uint16_t *flashKiB;   /**< flash size (KiB) 0x1FFF7A22 */
} DevInfo_Sources;

/** @brief Decoded identification. */
typedef struct
{
    uint32_t cpuid;
    uint32_t idcode;
    uint32_t uid[3];
    uint16_t flashKiB;
    uint32_t devId;      /**< IDCODE[11:0]:  0x421 = STM32F446xx          */
    uint32_t revId;      /**< IDCODE[31:16]: 0x1000 = revision A          */
    uint32_t partNo;     /**< CPUID[15:4]:   0xC24 = Cortex-M4            */
    uint32_t variant;    /**< CPUID[23:20]:  the "r" in r0p1              */
    uint32_t revision;   /**< CPUID[3:0]:    the "p" in r0p1              */
} DevInfo;

/** @brief The real addresses of an STM32F4 (RM0390 / DUI 0553). */
#define DEVINFO_SOURCES_STM32F4                                            \
    {                                                                      \
        .cpuid    = (const volatile uint32_t *)LAB_SCB_CPUID_ADDR,         \
        .idcode   = (const volatile uint32_t *)LAB_DBGMCU_IDCODE_ADDR,     \
        .uid      = (const volatile uint32_t *)LAB_UID_ADDR,               \
        .flashKiB = (const volatile uint16_t *)LAB_FLASHSIZE_ADDR,         \
    }

/** @brief Reads every source exactly once and decodes the fields. */
void DevInfo_Read(const DevInfo_Sources *src, DevInfo *out);

/** @brief "STM32F446xx", "STM32F40x/41x", ... or "unknown". */
const char *DevInfo_DeviceName(uint32_t devId);

/** @brief "Cortex-M4", "Cortex-M7", ... or "unknown". */
const char *DevInfo_CoreName(uint32_t partNo);

#endif /* DEVINFO_H */
