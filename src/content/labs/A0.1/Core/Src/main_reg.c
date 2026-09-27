/**
 * @file    main_reg.c
 * @brief   A0.1 Integer lab: register-level version (NUCLEO-F446RE, no HAL).
 *
 * After reset the F446 runs from HSI = 16 MHz with every bus prescaler at /1,
 * and Flash latency 0 is valid at 16 MHz, so no clock setup is needed.
 *
 * Only the CMSIS device header is used: every register and bit name below is
 * the one printed in RM0390 (STM32F446xx reference manual).
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"          /* needs STM32F446xx defined by the build   */
#include "int_lab.h"

/* ------------------------------------------------------------------------- */
/* Board / clock constants                                                   */
/* ------------------------------------------------------------------------- */

#define CPU_CLOCK_HZ        (16000000UL)   /**< HSI after reset.            */
#define UART_BAUD           (115200UL)
#define LED_PIN             (5U)           /**< LD2 = PA5.                  */
#define UART_TX_PIN         (2U)           /**< PA2 = USART2_TX (AF7).      */
#define UART_RX_PIN         (3U)           /**< PA3 = USART2_RX (AF7).      */
#define GPIO_AF7_USART      (7U)

#define GPIO_MODER_OUTPUT   (1UL)          /**< MODER field value 0b01.     */
#define GPIO_MODER_AF       (2UL)          /**< MODER field value 0b10.     */
#define GPIO_MODER_FIELD    (3UL)          /**< 2-bit field mask.           */
#define GPIO_AFR_FIELD      (0xFUL)        /**< 4-bit field mask.           */

#define LED_HALF_PERIOD_PASS_MS   (500U)
#define LED_HALF_PERIOD_FAIL_MS   (100U)
#define CYCLES_PER_MS             (CPU_CLOCK_HZ / 1000UL)

/* Forward note (A1.6): the CMSIS startup calls SystemInit() before main().
 * We enable the FPU there, because the project is built with
 * -mfloat-abi=hard and the compiler may emit FPU instructions anywhere. */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U)); /* CP10, CP11 */
}

/* ------------------------------------------------------------------------- */
/* GPIO + USART2                                                             */
/* ------------------------------------------------------------------------- */

/** @brief PA5 output, PA2/PA3 alternate function 7 (USART2). */
static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;          /* read back: wait for the clock (see A4.5) */

    /* Note every shift amount is unsigned and every mask is UL:
     * exactly the rules this lesson teaches.                               */
    GPIOA->MODER &= ~((GPIO_MODER_FIELD << (LED_PIN * 2U)) |
                      (GPIO_MODER_FIELD << (UART_TX_PIN * 2U)) |
                      (GPIO_MODER_FIELD << (UART_RX_PIN * 2U)));
    GPIOA->MODER |=  (GPIO_MODER_OUTPUT << (LED_PIN * 2U)) |
                     (GPIO_MODER_AF     << (UART_TX_PIN * 2U)) |
                     (GPIO_MODER_AF     << (UART_RX_PIN * 2U));

    /* AFR[0] (AFRL) holds pins 0..7, four bits per pin. */
    GPIOA->AFR[0] &= ~((GPIO_AFR_FIELD << (UART_TX_PIN * 4U)) |
                       (GPIO_AFR_FIELD << (UART_RX_PIN * 4U)));
    GPIOA->AFR[0] |=  (GPIO_AF7_USART << (UART_TX_PIN * 4U)) |
                      (GPIO_AF7_USART << (UART_RX_PIN * 4U));
}

/**
 * @brief USART2 115200 8N1 at PCLK1 = 16 MHz, oversampling by 16.
 *
 * USARTDIV = 16 000 000 / (16 * 115 200) = 8.6806
 * Mantissa = 8, Fraction = round(0.6806 * 16) = 11  -> BRR = (8 << 4) | 11
 * Real baud = 16e6 / (16 * 8.6875) = 115 108 -> error -0.08 % (see A8.2).
 * Integer rounding: (f + baud/2) / baud gives 16*USARTDIV rounded = 139 = 0x8B.
 */
static void uart_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    USART2->BRR = (uint32_t)((CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD);
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;   /* 8N1, OVER8 = 0          */
    USART2->CR1 |= USART_CR1_UE;                 /* enable last             */
}

/** @brief Blocking string output: wait TXE before each byte, TC at the end. */
static void uart_write(const char *text)
{
    while (*text != '\0')
    {
        while ((USART2->SR & USART_SR_TXE) == 0U)
        {
        }
        /* char -> uint8_t -> uint32_t: explicit, no sign extension possible
         * (and on ARM plain char is unsigned anyway, see T0).             */
        USART2->DR = (uint32_t)(uint8_t)*text++;
    }
    while ((USART2->SR & USART_SR_TC) == 0U)
    {
    }
}

/* ------------------------------------------------------------------------- */
/* DWT cycle counter                                                         */
/* ------------------------------------------------------------------------- */

static void cycle_counter_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static uint32_t cycle_counter_read(void)
{
    return DWT->CYCCNT;
}

/** @brief Busy-wait using the cycle counter. Wrap-safe (unsigned delta). */
static void delay_ms(uint32_t ms)
{
    const uint32_t start = DWT->CYCCNT;
    const uint32_t ticks = ms * CYCLES_PER_MS;

    while ((DWT->CYCCNT - start) < ticks)
    {
    }
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    gpio_init();
    uart_init();
    cycle_counter_init();

    const bool     allPassed  = IntLab_Run(uart_write, cycle_counter_read);
    const uint32_t halfPeriod = allPassed ? LED_HALF_PERIOD_PASS_MS : LED_HALF_PERIOD_FAIL_MS;

    for (;;)
    {
        GPIOA->ODR ^= (1UL << LED_PIN);   /* fine here: no ISR touches GPIOA
                                             (A5.3 explains when to use BSRR) */
        delay_ms(halfPeriod);
    }
}
