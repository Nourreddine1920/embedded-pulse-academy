/**
 * @file    probe.c
 * @brief   A0.4: address table, pointer arithmetic and device-ID report.
 */
#include "probe.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "mmio.h"

#define PROBE_LINE_LEN   (128U)

static Probe_WriteFn s_write;

void Probe_Init(Probe_WriteFn write)
{
    s_write = write;
}

void Probe_Printf(const char *fmt, ...)
{
    char    line[PROBE_LINE_LEN];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    s_write(line);
}

/** @brief One row of the address table. */
typedef struct
{
    const char *name;
    uint32_t    base;
    uint32_t    offset;
    uint32_t    bits;     /**< access width the RM specifies */
} ProbeRow;

void Probe_AddressTable(void)
{
    static const ProbeRow rows[] = {
        { "GPIOA->MODER",   LAB_GPIOA_BASE,  GPIO_MODER_OFS,  32U },
        { "GPIOA->IDR",     LAB_GPIOA_BASE,  GPIO_IDR_OFS,    32U },
        { "GPIOA->ODR",     LAB_GPIOA_BASE,  GPIO_ODR_OFS,    32U },
        { "GPIOA->BSRR",    LAB_GPIOA_BASE,  GPIO_BSRR_OFS,   32U },
        { "RCC->AHB1ENR",   LAB_RCC_BASE,    RCC_AHB1ENR_OFS, 32U },
        { "USART2->DR",     LAB_USART2_BASE, USART_DR_OFS,    32U },
        { "SCB->CPUID",     LAB_SCB_CPUID_ADDR, 0x0U,         32U },
        { "DBGMCU->IDCODE", LAB_DBGMCU_IDCODE_ADDR, 0x0U,     32U },
        { "UID[0]",         LAB_UID_ADDR,    0x0U,            32U },
        { "FLASH_SIZE",     LAB_FLASHSIZE_ADDR, 0x0U,         16U },
    };

    Probe_Printf("%-16s %-10s   %-10s   %-6s %s\r\n", "register", "address", "base", "offset", " width");
    for (size_t i = 0U; i < (sizeof rows / sizeof rows[0]); i++)
    {
        Probe_Printf("%-16s 0x%08" PRIX32 " = 0x%08" PRIX32 " + 0x%02" PRIX32 "   %2" PRIu32 "-bit\r\n",
                     rows[i].name, rows[i].base + rows[i].offset, rows[i].base, rows[i].offset,
                     rows[i].bits);
    }
}

void Probe_PointerArithmetic(void)
{
    /* Building (not dereferencing) pointers is safe on any machine. */
    volatile uint32_t *const words = (volatile uint32_t *)LAB_GPIOA_BASE;
    volatile uint8_t *const  bytes = (volatile uint8_t *)LAB_GPIOA_BASE;

    Probe_Printf("p  = (volatile uint32_t *)0x%08" PRIXPTR "\r\n", (uintptr_t)words);
    Probe_Printf("p + 5                     -> 0x%08" PRIXPTR "  (5 x 4 bytes: GPIOA->ODR)\r\n",
                 (uintptr_t)(words + 5));
    Probe_Printf("reg_at(p, 0x14)           -> 0x%08" PRIXPTR "  (same register, RM offset)\r\n",
                 (uintptr_t)reg_at(words, GPIO_ODR_OFS));
    Probe_Printf("(volatile uint8_t *)p + 5 -> 0x%08" PRIXPTR "  (5 x 1 byte: inside MODER)\r\n",
                 (uintptr_t)(bytes + 5));
}

void Probe_PrintDevInfo(const DevInfo *info)
{
    Probe_Printf("SCB->CPUID     = 0x%08" PRIX32 "  -> %s r%" PRIu32 "p%" PRIu32 "\r\n",
                 info->cpuid, DevInfo_CoreName(info->partNo), info->variant, info->revision);
    Probe_Printf("DBGMCU->IDCODE = 0x%08" PRIX32 "  -> DEV_ID 0x%03" PRIX32 " (%s), REV_ID 0x%04" PRIX32 "\r\n",
                 info->idcode, info->devId, DevInfo_DeviceName(info->devId), info->revId);
    Probe_Printf("UID            = %08" PRIX32 "-%08" PRIX32 "-%08" PRIX32 "\r\n",
                 info->uid[2], info->uid[1], info->uid[0]);
    Probe_Printf("FLASH_SIZE     = %u KiB\r\n", (unsigned)info->flashKiB);
}
