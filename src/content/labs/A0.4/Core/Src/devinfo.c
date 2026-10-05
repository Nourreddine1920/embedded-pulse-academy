/**
 * @file    devinfo.c
 * @brief   A0.4: identification registers, read through const volatile pointers.
 */
#include "devinfo.h"

#include <stddef.h>

#define IDCODE_DEV_ID_MASK    (0x00000FFFUL)
#define IDCODE_REV_ID_POS     (16U)
#define CPUID_PARTNO_POS      (4U)
#define CPUID_PARTNO_MASK     (0xFFFUL)
#define CPUID_VARIANT_POS     (20U)
#define CPUID_NIBBLE_MASK     (0xFUL)

void DevInfo_Read(const DevInfo_Sources *src, DevInfo *out)
{
    /* Each *pointer is exactly one load from the bus: the pointers are
     * volatile, so the compiler may neither drop nor repeat these reads. */
    out->cpuid    = *src->cpuid;
    out->idcode   = *src->idcode;
    out->uid[0]   = src->uid[0];          /* 0x1FFF7A10 */
    out->uid[1]   = src->uid[1];          /* 0x1FFF7A14: +1 element = +4 bytes */
    out->uid[2]   = src->uid[2];          /* 0x1FFF7A18 */
    out->flashKiB = *src->flashKiB;       /* 16-bit load (LDRH), not 32-bit */

    out->devId    = out->idcode & IDCODE_DEV_ID_MASK;
    out->revId    = out->idcode >> IDCODE_REV_ID_POS;
    out->partNo   = (out->cpuid >> CPUID_PARTNO_POS) & CPUID_PARTNO_MASK;
    out->variant  = (out->cpuid >> CPUID_VARIANT_POS) & CPUID_NIBBLE_MASK;
    out->revision = out->cpuid & CPUID_NIBBLE_MASK;
}

const char *DevInfo_DeviceName(uint32_t devId)
{
    static const struct
    {
        uint32_t    id;
        const char *name;
    } table[] = {
        { 0x413U, "STM32F40x/41x" },
        { 0x419U, "STM32F42x/43x" },
        { 0x421U, "STM32F446xx" },
        { 0x431U, "STM32F411xx" },
        { 0x441U, "STM32F412xx" },
        { 0x463U, "STM32F413/423" },
    };

    for (size_t i = 0U; i < (sizeof table / sizeof table[0]); i++)
    {
        if (table[i].id == devId)
        {
            return table[i].name;
        }
    }
    return "unknown";
}

const char *DevInfo_CoreName(uint32_t partNo)
{
    switch (partNo)
    {
        case 0xC20U: return "Cortex-M0";
        case 0xC60U: return "Cortex-M0+";
        case 0xC23U: return "Cortex-M3";
        case 0xC24U: return "Cortex-M4";
        case 0xC27U: return "Cortex-M7";
        case 0xD21U: return "Cortex-M33";
        default:     return "unknown";
    }
}
