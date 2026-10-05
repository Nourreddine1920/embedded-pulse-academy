/**
 * @file    main_reg.c
 * @brief   A1.2 Core registers: register-level version (NUCLEO-F446RE, no HAL).
 *
 * Prints the special registers in thread mode, then checks the interrupt-mask
 * model of core_regs.c against the real NVIC. HSI 16 MHz, USART2 115200 8N1,
 * LD2 on PA5 (blinks forever after the report).
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "core_regs.h"
#include "regs_hw.h"
#include "regs_report.h"

#define CPU_CLOCK_HZ          (16000000UL)
#define CYCLES_PER_MS         (CPU_CLOCK_HZ / 1000UL)
#define UART_BAUD             (115200UL)
#define LED_PIN               (5U)
#define BLINK_MS              (500U)

/* CMSIS startup calls this before main(): enable the FPU (A1.1, A1.6). */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
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

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(0xFFUL << 8U)) | (0x77UL << 8U);     /* PA2/PA3 = AF7 */
    GPIOA->MODER  = (GPIOA->MODER & ~((0xFUL << 4U) | (0x3UL << 10U)))
                  | (0xAUL << 4U) | (0x1UL << 10U);                         /* AF, AF, PA5 out */

    USART2->BRR = (uint32_t)((CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD);   /* 0x8B */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

int main(void)
{
    board_init();
    Regs_Init(uart_write);

    Regs_Printf("\r\n=== A1.2 Core registers (registers) ===\r\n");
    (void)Regs_SelfTest();
    Regs_PrintRegisterRoles();

    const uint32_t mismatches = RegsHw_PrintReport();

    Regs_Printf("\r\nmodel and hardware disagree in %lu case(s)\r\n", (unsigned long)mismatches);

    for (;;)
    {
        GPIOA->BSRR = ((GPIOA->ODR & (1UL << LED_PIN)) != 0U) ? (1UL << (LED_PIN + 16U)) : (1UL << LED_PIN);
        delay_ms(BLINK_MS);
    }
}
