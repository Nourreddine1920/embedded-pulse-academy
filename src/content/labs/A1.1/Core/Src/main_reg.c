/**
 * @file    main_reg.c
 * @brief   A1.1 Core probe: register-level version (NUCLEO-F446RE, no HAL).
 *
 * The same feature_probe.c as the HAL version, on a bare CMSIS project:
 * HSI 16 MHz, USART2 115200 8N1 through the ST-LINK VCP, LD2 on PA5.
 *
 * The FPU is switched on in SystemInit(), as the CMSIS startup expects. Print
 * CPACR before and after to see the difference (set LAB_FPU_OFF to 1 to leave
 * it off and watch "enabled: no").
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "core_info.h"
#include "feature_probe.h"

#ifndef LAB_FPU_OFF
#define LAB_FPU_OFF           (0)
#endif

#define CPU_CLOCK_HZ          (16000000UL)
#define CYCLES_PER_MS         (CPU_CLOCK_HZ / 1000UL)
#define UART_BAUD             (115200UL)
#define LED_PIN               (5U)
#define BLINK_MS              (500U)

/* CMSIS startup calls this before main(). CP10 and CP11 full access = FPU on (A1.6). */
void SystemInit(void)
{
#if LAB_FPU_OFF == 0
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
#endif
}

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

static void board_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;                                 /* wait for the clock (A4.5) */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;     /* DWT: turn the trace block on  */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    /* PA2/PA3 = AF7 (USART2), PA5 = output (LD2). */
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(0xFFUL << 8U)) | (0x77UL << 8U);
    GPIOA->MODER  = (GPIOA->MODER & ~((0xFUL << 4U) | (0x3UL << 10U)))
                  | (0xAUL << 4U) | (0x1UL << 10U);

    USART2->BRR = (uint32_t)((CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD);   /* 0x8B */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

int main(void)
{
    const FeatureSources src = FEATURE_SOURCES_CORTEX_M4;

    board_init();
    FeatureProbe_Init(uart_write);

    FeatureProbe_Printf("\r\n=== A1.1 Core probe (registers) ===\r\n");
    (void)FeatureProbe_SelfTest();
    FeatureProbe_PrintTable();
    FeatureProbe_Report(&src);

    /* The DWT cycle counter exists on M3/M4/M7/M33 and not on M0/M0+ ("cycle counter" above). */
    const uint32_t t0 = DWT->CYCCNT;

    delay_ms(BLINK_MS);
    const uint32_t elapsed = DWT->CYCCNT - t0;

    FeatureProbe_Printf("\r\n--- DWT cycle counter ---\r\n");
    FeatureProbe_Printf("delay_ms(%u) took %lu cycles (about %lu expected at %lu Hz)\r\n", (unsigned)BLINK_MS,
                        (unsigned long)elapsed, (unsigned long)(BLINK_MS * CYCLES_PER_MS),
                        (unsigned long)CPU_CLOCK_HZ);

    for (;;)
    {
        GPIOA->BSRR = ((GPIOA->ODR & (1UL << LED_PIN)) != 0U) ? (1UL << (LED_PIN + 16U)) : (1UL << LED_PIN);
        delay_ms(BLINK_MS);
    }
}
