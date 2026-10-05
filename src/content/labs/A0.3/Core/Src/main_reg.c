/**
 * @file    main_reg.c
 * @brief   A0.3 volatile & const lab: register-level version (NUCLEO-F446RE, no HAL).
 *
 * TIM6 interrupts at 20 kHz and calls VcLab_TickIsr(). DWT_CYCCNT is the
 * cycle counter. LD2 blinks at 1 Hz if every fixed variant passed, 5 Hz if not.
 *
 * Clock: HSI 16 MHz (reset state).  Console: USART2 115200 8N1.
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "vc_lab.h"

/* ------------------------------------------------------------------------- */
/* Constants                                                                 */
/* ------------------------------------------------------------------------- */

#define CPU_CLOCK_HZ        (16000000UL)
#define CYCLES_PER_MS       (CPU_CLOCK_HZ / 1000UL)
#define UART_BAUD           (115200UL)
#define FAST_TICK_HZ        (20000UL)                 /**< TIM6 update rate.    */
#define TIM6_IRQ_PRIORITY   (5U)

#define LED_PIN             (5U)                      /**< PA5 = LD2            */
#define UART_TX_PIN         (2U)
#define UART_RX_PIN         (3U)
#define MODER_OUTPUT        (1UL)
#define MODER_AF            (2UL)
#define AF7_USART2          (7UL)

#define BLINK_PASS_MS       (500U)                    /**< 1 Hz                 */
#define BLINK_FAIL_MS       (100U)                    /**< 5 Hz                 */

/* STM32F446RE memory map (RM0390, Memory map; datasheet: 512 KB Flash, 128 KB SRAM). */
#define FLASH_START         (0x08000000UL)
#define FLASH_END           (0x08080000UL)
#define SRAM_START          (0x20000000UL)
#define SRAM_END            (0x20020000UL)

/* CMSIS startup calls this before main(): enable the FPU (see A0.1 / A1.6). */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
}

/* ------------------------------------------------------------------------- */
/* Platform hooks for the lab                                                */
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

static void fast_tick(bool enable)
{
    if (enable)
    {
        TIM6->CNT = 0U;
        TIM6->SR  = ~TIM_SR_UIF;          /* rc_w0: clear only UIF (A0.2) */
        TIM6->CR1 |= TIM_CR1_CEN;
    }
    else
    {
        TIM6->CR1 &= ~TIM_CR1_CEN;
        __DSB();                          /* the write has reached TIM6...          */
        __ISB();                          /* ...and a last pending IRQ is taken now */
    }
}

static const char *region_of(const void *address)
{
    const uintptr_t a = (uintptr_t)address;

    if ((a >= FLASH_START) && (a < FLASH_END))
    {
        return "Flash";
    }
    if ((a >= SRAM_START) && (a < SRAM_END))
    {
        return "SRAM";
    }
    return "other";
}

/** @brief TIM6 update interrupt: the lab's "fast tick". */
void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR = ~TIM_SR_UIF;               /* clear first, so it can't re-trigger */
    VcLab_TickIsr();
}

/* ------------------------------------------------------------------------- */
/* Initialisation                                                            */
/* ------------------------------------------------------------------------- */

static void clocks_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;                              /* wait for the clock (A4.5) */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM6EN;
    (void)RCC->APB1ENR;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  /* DWT cycle counter       */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static void gpio_uart_init(void)
{
    GPIOA->MODER = (GPIOA->MODER & ~((3UL << (LED_PIN * 2U)) |
                                     (3UL << (UART_TX_PIN * 2U)) | (3UL << (UART_RX_PIN * 2U))))
                 | (MODER_OUTPUT << (LED_PIN * 2U))
                 | (MODER_AF << (UART_TX_PIN * 2U)) | (MODER_AF << (UART_RX_PIN * 2U));
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~((0xFUL << (UART_TX_PIN * 4U)) | (0xFUL << (UART_RX_PIN * 4U))))
                  | (AF7_USART2 << (UART_TX_PIN * 4U)) | (AF7_USART2 << (UART_RX_PIN * 4U));

    USART2->BRR = (uint32_t)((CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD);  /* 0x8B */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

/** @brief TIM6: 16 MHz / (PSC+1) / (ARR+1) = 16 MHz / 1 / 800 = 20 kHz. */
static void tim6_init(void)
{
    TIM6->PSC  = 0U;
    TIM6->ARR  = (uint32_t)(CPU_CLOCK_HZ / FAST_TICK_HZ) - 1U;   /* 799 */
    TIM6->EGR  = TIM_EGR_UG;          /* load PSC/ARR now (sets UIF as a side effect) */
    TIM6->SR   = ~TIM_SR_UIF;         /* ...so clear it before enabling the IRQ       */
    TIM6->DIER = TIM_DIER_UIE;

    NVIC_SetPriority(TIM6_DAC_IRQn, TIM6_IRQ_PRIORITY);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

static void delay_ms(uint32_t ms)
{
    const uint32_t start = DWT->CYCCNT;
    const uint32_t ticks = ms * CYCLES_PER_MS;

    while ((DWT->CYCCNT - start) < ticks)   /* DWT->CYCCNT is volatile (__IO) */
    {
    }
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    clocks_init();
    gpio_uart_init();
    tim6_init();

    const VcLab_Platform platform = {
        .write        = uart_write,
        .cycleCounter = &DWT->CYCCNT,
        .cyclesPerMs  = CYCLES_PER_MS,
        .fastTick     = fast_tick,
        .regionOf     = region_of,
        .cpuid        = &SCB->CPUID,     /* __IM uint32_t: volatile const */
    };

    const uint32_t blinkMs = VcLab_Run(&platform) ? BLINK_PASS_MS : BLINK_FAIL_MS;

    for (;;)
    {
        GPIOA->BSRR = (GPIOA->ODR & (1UL << LED_PIN)) ? (1UL << (LED_PIN + 16U)) : (1UL << LED_PIN);
        delay_ms(blinkMs);
    }
}
