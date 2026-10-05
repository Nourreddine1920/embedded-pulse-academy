/**
 * @file    mmio.h
 * @brief   A0.4: memory-mapped I/O without a device header.
 *
 * Every address below is copied from RM0390 (STM32F446) or from the
 * Cortex-M4 Generic User Guide, so you can check each one against the
 * documents. The CMSIS device header defines the same values under other
 * names (GPIOA_BASE, RCC_BASE, UID_BASE, ...); the LAB_ prefix avoids a
 * clash when both are included in one file.
 *
 * Nothing in this file touches hardware by itself: the macros only build
 * pointers. The access happens where the pointer is dereferenced.
 */
#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

/* ---- CMSIS access qualifiers (identical to core_cm4.h) ------------------ */
/* Defined here only if no CMSIS header was included first.                 */
#ifndef __I
#define __I     volatile const   /**< read only                            */
#endif
#ifndef __O
#define __O     volatile         /**< write only (NOT enforced by the compiler) */
#endif
#ifndef __IO
#define __IO    volatile         /**< read / write                         */
#endif

/* ---- Raw register access: an integer becomes a volatile lvalue ---------- */

/** @brief 32-bit register at absolute address @p addr (an integer constant). */
#define REG32(addr)   (*(volatile uint32_t *)(uintptr_t)(addr))
/** @brief 16-bit register / data item (e.g. the flash-size word). */
#define REG16(addr)   (*(volatile uint16_t *)(uintptr_t)(addr))
/** @brief 8-bit access. Only for registers the RM allows byte access to. */
#define REG8(addr)    (*(volatile uint8_t *)(uintptr_t)(addr))

/**
 * @brief Register at @p byteOffset from @p base.
 *
 * @p base points to 32-bit words, so pointer arithmetic counts in words:
 * base + 5 is 20 bytes further. Dividing the byte offset from the RM by
 * sizeof(uint32_t) keeps the offsets in this code identical to the manual.
 */
static inline volatile uint32_t *reg_at(volatile uint32_t *base, uint32_t byteOffset)
{
    return base + (byteOffset / (uint32_t)sizeof(uint32_t));
}

/* ---- Memory map (RM0390, "Memory map" and "Register boundary addresses" table) */

#define LAB_PERIPH_BASE        (0x40000000UL)   /**< start of the peripheral region  */
#define LAB_APB1_BASE          (LAB_PERIPH_BASE + 0x00000000UL)
#define LAB_AHB1_BASE          (LAB_PERIPH_BASE + 0x00020000UL)

#define LAB_GPIOA_BASE         (LAB_AHB1_BASE + 0x0000UL)   /**< 0x4002 0000 */
#define LAB_GPIOC_BASE         (LAB_AHB1_BASE + 0x0800UL)   /**< 0x4002 0800 */
#define LAB_RCC_BASE           (LAB_AHB1_BASE + 0x3800UL)   /**< 0x4002 3800 */
#define LAB_USART2_BASE        (LAB_APB1_BASE + 0x4400UL)   /**< 0x4000 4400 */

/* GPIO register offsets (RM0390, GPIO register map) */
#define GPIO_MODER_OFS         (0x00UL)
#define GPIO_OTYPER_OFS        (0x04UL)
#define GPIO_OSPEEDR_OFS       (0x08UL)
#define GPIO_PUPDR_OFS         (0x0CUL)
#define GPIO_IDR_OFS           (0x10UL)
#define GPIO_ODR_OFS           (0x14UL)
#define GPIO_BSRR_OFS          (0x18UL)
#define GPIO_BLOCK_WORDS       (10U)            /**< MODER .. AFRH = 0x00 .. 0x24 */

/* RCC register offsets */
#define RCC_AHB1ENR_OFS        (0x30UL)
#define RCC_APB1ENR_OFS        (0x40UL)
#define RCC_AHB1ENR_GPIOAEN_BIT (0U)

/* USART register offsets */
#define USART_SR_OFS           (0x00UL)
#define USART_DR_OFS           (0x04UL)

/* ---- Identification registers ------------------------------------------ */

/** Cortex-M4 System Control Block, CPUID (read-only). DUI 0553, "System control block". */
#define LAB_SCB_CPUID_ADDR     (0xE000ED00UL)
/** DBGMCU_IDCODE: DEV_ID [11:0], REV_ID [31:16]. RM0390, "Debug support: MCU device ID code". */
#define LAB_DBGMCU_IDCODE_ADDR (0xE0042000UL)
/** 96-bit unique device ID, three 32-bit words. RM0390, "Device electronic signature". */
#define LAB_UID_ADDR           (0x1FFF7A10UL)
/** Flash size in KiB, a 16-bit value. RM0390, "Flash size". */
#define LAB_FLASHSIZE_ADDR     (0x1FFF7A22UL)

/* Compile-time proof that the arithmetic above gives the RM0390 numbers. */
_Static_assert(LAB_GPIOA_BASE == 0x40020000UL, "GPIOA base");
_Static_assert(LAB_GPIOA_BASE + GPIO_ODR_OFS == 0x40020014UL, "GPIOA_ODR");
_Static_assert(LAB_GPIOA_BASE + GPIO_BSRR_OFS == 0x40020018UL, "GPIOA_BSRR");
_Static_assert(LAB_RCC_BASE + RCC_AHB1ENR_OFS == 0x40023830UL, "RCC_AHB1ENR");
_Static_assert(LAB_USART2_BASE + USART_DR_OFS == 0x40004404UL, "USART2_DR");
_Static_assert((LAB_FLASHSIZE_ADDR % 2UL) == 0UL, "flash size is halfword aligned, not word aligned");

#endif /* MMIO_H */
