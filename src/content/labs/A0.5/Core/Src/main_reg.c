/**
 * @file    main_reg.c
 * @brief   A0.5 Register maps & layouts: register-level version
 *          (NUCLEO-F446RE, no HAL, no CMSIS peripheral structs).
 *
 * Every peripheral access goes through OUR overlays from regmap.h:
 * MY_RCC, MY_GPIOA, MY_GPIOC, MY_USART2. The CMSIS header is included only
 * for the core (SCB, DWT, CoreDebug) and for the compile-time cross-check.
 *
 * Optional fault demos (set LAB_UNALIGNED_DEMO):
 *   0  none (default)
 *   1  enable CCR.UNALIGN_TRP, then do one unaligned 32-bit LDR -> UsageFault
 *   2  unaligned 64-bit read (LDRD) -> UsageFault even with UNALIGN_TRP = 0
 *
 * Clock: HSI 16 MHz (reset state).  Console: USART2 115200 8N1.
 * B1 = PC13 (active low), LD2 = PA5.
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "regmap.h"
#include "regmap_check.h"
#include "layout_report.h"
#include "sensor_frame.h"

#ifndef LAB_UNALIGNED_DEMO
#define LAB_UNALIGNED_DEMO    (0)
#endif

/* ------------------------------------------------------------------------- */
/* Constants (bit positions from RM0390)                                     */
/* ------------------------------------------------------------------------- */
#define CPU_CLOCK_HZ          (16000000UL)
#define CYCLES_PER_MS         (CPU_CLOCK_HZ / 1000UL)
#define UART_BAUD             (115200UL)
#define BUTTON_SAMPLE_MS      (10U)

#define LED_PIN               (5U)
#define BUTTON_PIN            (13U)
#define UART_TX_PIN           (2U)
#define UART_RX_PIN           (3U)

#define RCC_AHB1ENR_GPIOA     (1UL << 0)
#define RCC_AHB1ENR_GPIOC     (1UL << 2)
#define RCC_APB1ENR_USART2    (1UL << 17)

#define USART_SR_TXE_BIT      (1UL << 7)
#define USART_SR_TC_BIT       (1UL << 6)
#define USART_CR1_RE_BIT      (1UL << 2)
#define USART_CR1_TE_BIT      (1UL << 3)
#define USART_CR1_UE_BIT      (1UL << 13)

#define MODER_OUTPUT          (1UL)
#define MODER_AF              (2UL)
#define PUPDR_PULL_UP         (1UL)
#define AF7_USART2            (7UL)

#define PIN2_POS(pin)         ((pin) * 2U)
#define PIN2_MASK(pin)        (3UL << PIN2_POS(pin))
#define AFRL_POS(pin)         ((pin) * 4U)
#define AFRL_MASK(pin)        (0xFUL << AFRL_POS(pin))

/* CMSIS startup calls this before main(): enable the FPU (A0.1 / A1.6). */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
}

/* ------------------------------------------------------------------------- */
/* Console and timing, through my_usart_t                                    */
/* ------------------------------------------------------------------------- */
static void uart_write(const char *text)
{
    while (*text != '\0')
    {
        while ((MY_USART2->SR & USART_SR_TXE_BIT) == 0U)
        {
        }
        MY_USART2->DR = (uint32_t)(uint8_t)*text++;
    }
    while ((MY_USART2->SR & USART_SR_TC_BIT) == 0U)
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

static void board_init(void)
{
    MY_RCC->AHB1ENR |= RCC_AHB1ENR_GPIOA | RCC_AHB1ENR_GPIOC;
    (void)MY_RCC->AHB1ENR;                            /* let the clock start */
    MY_RCC->APB1ENR |= RCC_APB1ENR_USART2;
    (void)MY_RCC->APB1ENR;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   /* DWT cycle counter   */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    /* PA2/PA3 -> AF7 (USART2). AFR[0] is AFRL: our struct's 9th word, 0x20. */
    MY_GPIOA->AFR[0] = (MY_GPIOA->AFR[0] & ~(AFRL_MASK(UART_TX_PIN) | AFRL_MASK(UART_RX_PIN)))
                     | (AF7_USART2 << AFRL_POS(UART_TX_PIN)) | (AF7_USART2 << AFRL_POS(UART_RX_PIN));
    MY_GPIOA->MODER  = (MY_GPIOA->MODER & ~(PIN2_MASK(UART_TX_PIN) | PIN2_MASK(UART_RX_PIN) | PIN2_MASK(LED_PIN)))
                     | (MODER_AF << PIN2_POS(UART_TX_PIN)) | (MODER_AF << PIN2_POS(UART_RX_PIN))
                     | (MODER_OUTPUT << PIN2_POS(LED_PIN));

    /* PC13: input (reset state) with pull-up. */
    MY_GPIOC->PUPDR  = (MY_GPIOC->PUPDR & ~PIN2_MASK(BUTTON_PIN)) | (PUPDR_PULL_UP << PIN2_POS(BUTTON_PIN));

    MY_USART2->BRR = (CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD;   /* 0x8B */
    MY_USART2->CR1 = USART_CR1_TE_BIT | USART_CR1_RE_BIT;               /* disabled: plain write */
    MY_USART2->CR1 |= USART_CR1_UE_BIT;
}

/* ------------------------------------------------------------------------- */
/* Alignment and faults                                                      */
/* ------------------------------------------------------------------------- */

/** Prints the UsageFault status and stops. CFSR bit 24 = UNALIGNED. */
void UsageFault_Handler(void)
{
    const uint32_t cfsr = SCB->CFSR;

    Report_Printf("\r\n!!! UsageFault  CFSR = 0x%08lX  UNALIGNED = %lu\r\n",
                  (unsigned long)cfsr, (unsigned long)((cfsr >> 24) & 1UL));
    for (;;)
    {
    }
}

#if LAB_UNALIGNED_DEMO == 2
/**
 * In real code the pointer arrives from elsewhere (a parser, a DMA buffer),
 * so the compiler cannot see its alignment and must assume 8: one LDRD.
 * noipa models that: GCC may not look inside or specialise for the caller.
 * (Inlined with a visible buf+1, GCC 10.3 -O2 splits it into two safe LDRs.)
 */
static uint64_t __attribute__((noipa)) read_u64(const volatile uint64_t *p)
{
    return *p;
}
#endif

static void demo_alignment(void)
{
#if LAB_UNALIGNED_DEMO != 0
    _Alignas(8) static uint8_t buf[16] = { 0x00U, 0x11U, 0x22U, 0x33U, 0x44U, 0x55U, 0x66U, 0x77U,
                                           0x88U, 0x99U, 0xAAU, 0xBBU, 0xCCU, 0xDDU, 0xEEU, 0xFFU };
#endif

    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;   /* UsageFault_Handler instead of HardFault */

    Report_Printf("\r\n--- Alignment on the Cortex-M4 ---\r\n");
    Report_Printf("SCB->CCR = 0x%08lX, UNALIGN_TRP = %lu\r\n", (unsigned long)SCB->CCR,
                  (unsigned long)((SCB->CCR & SCB_CCR_UNALIGN_TRP_Msk) >> SCB_CCR_UNALIGN_TRP_Pos));

#if LAB_UNALIGNED_DEMO == 1
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    __DSB();
    __ISB();
    Report_Printf("UNALIGN_TRP set; reading a uint32_t at buf+1 ...\r\n");
    Report_Printf("value = 0x%08lX\r\n", (unsigned long)*(volatile const uint32_t *)(const volatile void *)&buf[1]);
#elif LAB_UNALIGNED_DEMO == 2
    Report_Printf("reading a uint64_t at buf+1 (LDRD) ...\r\n");
    const uint64_t v64 = read_u64((const volatile uint64_t *)(const volatile void *)&buf[1]);
    Report_Printf("value = 0x%08lX%08lX\r\n", (unsigned long)(v64 >> 32), (unsigned long)(v64 & 0xFFFFFFFFUL));
#else
    Report_Printf("set LAB_UNALIGNED_DEMO to 1 or 2 to see the fault\r\n");
#endif
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    board_init();
    Report_Init(uart_write);

    Report_Printf("\r\n=== A0.5 Register maps & layouts (registers) ===\r\n");
    Report_Printf("RCC->AHB1ENR = 0x%08lX  RCC->APB1ENR = 0x%08lX  USART2->BRR = 0x%08lX\r\n",
                  (unsigned long)MY_RCC->AHB1ENR, (unsigned long)MY_RCC->APB1ENR,
                  (unsigned long)MY_USART2->BRR);
    Report_RegisterMaps();
    Report_Padding();
    Report_Bitfields();
    const uint32_t failures = Report_Frames();
    Report_Printf("\r\nframe checks failed: %lu\r\n", (unsigned long)failures);
    demo_alignment();
    Report_Printf("\r\nPress B1: LED follows the button (my_gpio_t only).\r\n");

    for (;;)
    {
        const bool pressed = (MY_GPIOC->IDR & (1UL << BUTTON_PIN)) == 0U;   /* active low */

        MY_GPIOA->BSRR = pressed ? (1UL << LED_PIN) : (1UL << (LED_PIN + 16U));
        delay_ms(BUTTON_SAMPLE_MS);
    }
}
