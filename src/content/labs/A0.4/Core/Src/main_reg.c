/**
 * @file    main_reg.c
 * @brief   A0.4 Register probe: register-level version (NUCLEO-F446RE, no HAL).
 *
 * The same registers are reached three ways, and the program proves they
 * are the same memory locations:
 *   1. a raw cast:      REG32(0x40020000 + 0x14)
 *   2. base + offset:   reg_at((volatile uint32_t *)0x40020000, 0x14)
 *   3. the CMSIS struct: GPIOA->ODR
 *
 * Clock: HSI 16 MHz (reset state).  Console: USART2 115200 8N1.  LD2 = PA5.
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "devinfo.h"
#include "led_drv.h"
#include "mmio.h"
#include "mmio_selftest.h"
#include "probe.h"

#define CPU_CLOCK_HZ     (16000000UL)
#define CYCLES_PER_MS    (CPU_CLOCK_HZ / 1000UL)
#define UART_BAUD        (115200UL)
#define LED_PIN          (5U)
#define BLINK_MS         (500U)
#define BLINKS_PRINTED   (4U)

/* The CMSIS header and our hand-written map must agree, at compile time. */
_Static_assert(GPIOA_BASE == LAB_GPIOA_BASE, "GPIOA base differs from RM0390");
_Static_assert(RCC_BASE == LAB_RCC_BASE, "RCC base differs from RM0390");
_Static_assert(UID_BASE == LAB_UID_ADDR, "UID address differs from RM0390");
_Static_assert(FLASHSIZE_BASE == LAB_FLASHSIZE_ADDR, "flash-size address differs");

/* CMSIS startup calls this before main(): enable the FPU (see A0.1 / A1.6). */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
}

/* ------------------------------------------------------------------------- */
/* Console and timing                                                        */
/* ------------------------------------------------------------------------- */

static void uart_write(const char *text)
{
    while (*text != '\0')
    {
        while ((USART2->SR & USART_SR_TXE) == 0U)
        {
        }
        USART2->DR = (uint32_t)(uint8_t)*text++;
    }
    while ((USART2->SR & USART_SR_TC) == 0U)
    {
    }
}

static void delay_ms(uint32_t ms)
{
    const uint32_t start = DWT->CYCCNT;
    const uint32_t ticks = ms * CYCLES_PER_MS;

    while ((DWT->CYCCNT - start) < ticks)
    {
    }
}

/* ------------------------------------------------------------------------- */
/* Initialisation                                                            */
/* ------------------------------------------------------------------------- */

static void clocks_init(void)
{
    /* Raw cast, exactly what RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN expands to. */
    REG32(LAB_RCC_BASE + RCC_AHB1ENR_OFS) |= (1UL << RCC_AHB1ENR_GPIOAEN_BIT);
    (void)REG32(LAB_RCC_BASE + RCC_AHB1ENR_OFS);     /* wait for the clock (A4.5) */

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;             /* the same idea via CMSIS  */
    (void)RCC->APB1ENR;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   /* DWT cycle counter        */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static void uart_init(void)
{
    /* PA2 = TX, PA3 = RX: MODER fields 2 (AF), AFRL fields 7 (USART2). */
    GPIOA->MODER  = (GPIOA->MODER & ~(0xFUL << 4U)) | (0xAUL << 4U);
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(0xFFUL << 8U)) | (0x77UL << 8U);

    USART2->BRR = (uint32_t)((CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD);  /* 0x8B */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

/* ------------------------------------------------------------------------- */
/* Demos                                                                     */
/* ------------------------------------------------------------------------- */

/** @brief Three spellings, one register: compare addresses and values. */
static void demo_three_ways(void)
{
    volatile uint32_t *const port = (volatile uint32_t *)LAB_GPIOA_BASE;

    const uintptr_t addrRaw    = (uintptr_t)&REG32(LAB_GPIOA_BASE + GPIO_MODER_OFS);
    const uintptr_t addrOffset = (uintptr_t)reg_at(port, GPIO_MODER_OFS);
    const uintptr_t addrCmsis  = (uintptr_t)&GPIOA->MODER;

    const uint32_t viaRaw    = REG32(LAB_GPIOA_BASE + GPIO_MODER_OFS);
    const uint32_t viaOffset = *reg_at(port, GPIO_MODER_OFS);
    const uint32_t viaCmsis  = GPIOA->MODER;

    Probe_Printf("REG32(0x40020000)         @0x%08lX = 0x%08lX\r\n",
                 (unsigned long)addrRaw, (unsigned long)viaRaw);
    Probe_Printf("*reg_at(port, 0x00)       @0x%08lX = 0x%08lX\r\n",
                 (unsigned long)addrOffset, (unsigned long)viaOffset);
    Probe_Printf("GPIOA->MODER              @0x%08lX = 0x%08lX\r\n",
                 (unsigned long)addrCmsis, (unsigned long)viaCmsis);
    Probe_Printf("same address: %s, same value: %s\r\n",
                 ((addrRaw == addrOffset) && (addrOffset == addrCmsis)) ? "yes" : "NO",
                 ((viaRaw == viaOffset) && (viaOffset == viaCmsis)) ? "yes" : "NO");
}

/** @brief DevInfo through our pointers vs the CMSIS names for the same data. */
static void demo_device_id(void)
{
    const DevInfo_Sources src = DEVINFO_SOURCES_STM32F4;
    DevInfo info;

    DevInfo_Read(&src, &info);
    Probe_PrintDevInfo(&info);

    const bool sameId    = (info.idcode == DBGMCU->IDCODE);
    const bool sameCpu   = (info.cpuid == SCB->CPUID);
    const bool sameFlash = (info.flashKiB == *(const volatile uint16_t *)FLASHSIZE_BASE);

    Probe_Printf("matches DBGMCU->IDCODE / SCB->CPUID / FLASHSIZE_BASE: %s\r\n",
                 (sameId && sameCpu && sameFlash) ? "yes" : "NO");
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    clocks_init();
    uart_init();
    Probe_Init(uart_write);

    Probe_Printf("\r\n=== A0.4 Register probe (registers) ===\r\n");
    (void)MmioSelfTest_Run();

    Probe_Printf("\r\n--- Address map ---\r\n");
    Probe_AddressTable();

    Probe_Printf("\r\n--- Pointer arithmetic ---\r\n");
    Probe_PointerArithmetic();

    LedDrv led;
    LedDrv_Init(&led, (volatile uint32_t *)LAB_GPIOA_BASE, LED_PIN);   /* the one cast */

    Probe_Printf("\r\n--- One register, three spellings (after LedDrv_Init) ---\r\n");
    demo_three_ways();

    Probe_Printf("\r\n--- Device identification ---\r\n");
    demo_device_id();

    Probe_Printf("\r\n--- LD2 blinks through BSRR (0x40020018) ---\r\n");
    for (uint32_t n = 1U; ; n++)
    {
        LedDrv_Toggle(&led);
        if (n <= BLINKS_PRINTED)
        {
            Probe_Printf("toggle %lu: ODR bit 5 = %u\r\n", (unsigned long)n, LedDrv_IsOn(&led) ? 1U : 0U);
        }
        delay_ms(BLINK_MS);
    }
}
