/**
 * @file    main_reg.c
 * @brief   A0.6 Hygiene bench: register-level version (NUCLEO-F446RE, no HAL).
 *
 * The same bench and event counter as the HAL version, with the board code
 * written the "hygienic" way: static inline helpers instead of macros, an
 * enum for pin modes, static_assert on the CMSIS register layout and on the
 * UART configuration, and every file-private symbol declared static.
 *
 * Clock: HSI 16 MHz (reset state).  Console: USART2 115200 8N1.
 * B1 = PC13 (active low), LD2 = PA5.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "console.h"
#include "event_counter.h"
#include "hygiene.h"
#include "hygiene_bench.h"

/* ------------------------------------------------------------------------- */
/* Constants: enums and typed constants, not bare #defines                   */
/* ------------------------------------------------------------------------- */

#define CPU_CLOCK_HZ       APB1_CLOCK_HZ   /* 16 MHz HSI, all buses /1 */
#define CYCLES_PER_MS      (CPU_CLOCK_HZ / 1000UL)

enum
{
    LED_PIN      = 5,     /**< PA5 = LD2              */
    BUTTON_PIN   = 13,    /**< PC13 = B1, active low  */
    UART_TX_PIN  = 2,     /**< PA2 = USART2_TX (AF7)  */
    UART_RX_PIN  = 3,     /**< PA3 = USART2_RX (AF7)  */
    AF7_USART2   = 7
};

/** GPIOx_MODER field values (RM0390, GPIO registers). */
typedef enum
{
    PIN_MODE_INPUT  = 0,
    PIN_MODE_OUTPUT = 1,
    PIN_MODE_AF     = 2,
    PIN_MODE_ANALOG = 3
} PinMode;

/* The CMSIS struct must match the reference manual, or every access is wrong (A0.5). */
static_assert(offsetof(GPIO_TypeDef, MODER) == 0x00U, "RM0390: GPIOx_MODER at 0x00");
static_assert(offsetof(GPIO_TypeDef, PUPDR) == 0x0CU, "RM0390: GPIOx_PUPDR at 0x0C");
static_assert(offsetof(GPIO_TypeDef, IDR)   == 0x10U, "RM0390: GPIOx_IDR at 0x10");
static_assert(offsetof(GPIO_TypeDef, BSRR)  == 0x18U, "RM0390: GPIOx_BSRR at 0x18");
static_assert(offsetof(GPIO_TypeDef, AFR)   == 0x20U, "RM0390: GPIOx_AFRL at 0x20");
static_assert(offsetof(USART_TypeDef, BRR)  == 0x08U, "RM0390: USART_BRR at 0x08");

/* ------------------------------------------------------------------------- */
/* Hardware helpers: static inline, typed, each argument evaluated once      */
/* ------------------------------------------------------------------------- */

static inline void gpio_set_mode(GPIO_TypeDef *port, uint32_t pin, PinMode mode)
{
    const uint32_t pos = pin * 2U;

    port->MODER = (port->MODER & ~(3UL << pos)) | ((uint32_t)mode << pos);
}

static inline void gpio_set_pull_up(GPIO_TypeDef *port, uint32_t pin)
{
    const uint32_t pos = pin * 2U;

    port->PUPDR = (port->PUPDR & ~(3UL << pos)) | (1UL << pos);
}

static inline void gpio_set_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    const uint32_t reg = pin / 8U;
    const uint32_t pos = (pin % 8U) * 4U;

    port->AFR[reg] = (port->AFR[reg] & ~(0xFUL << pos)) | ((af & 0xFUL) << pos);
}

static inline bool gpio_read(const GPIO_TypeDef *port, uint32_t pin)
{
    return (port->IDR & (1UL << pin)) != 0U;
}

static inline void gpio_toggle(GPIO_TypeDef *port, uint32_t pin)
{
    const uint32_t mask = 1UL << pin;

    /* One BSRR write: set the pin if it is low, reset it if it is high (A0.2). */
    port->BSRR = ((port->ODR & mask) != 0U) ? (mask << 16U) : mask;
}

/* CMSIS startup calls this before main(): enable the FPU (see A0.1 / A1.6). */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
}

/* ------------------------------------------------------------------------- */
/* Console, timing, init (file-private: static)                              */
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

static void board_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;                              /* wait for the clock (A4.5) */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  /* DWT cycle counter */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    gpio_set_af(GPIOA, UART_TX_PIN, AF7_USART2);     /* AF number before AF mode */
    gpio_set_af(GPIOA, UART_RX_PIN, AF7_USART2);
    gpio_set_mode(GPIOA, UART_TX_PIN, PIN_MODE_AF);
    gpio_set_mode(GPIOA, UART_RX_PIN, PIN_MODE_AF);
    gpio_set_mode(GPIOA, LED_PIN, PIN_MODE_OUTPUT);
    gpio_set_mode(GPIOC, BUTTON_PIN, PIN_MODE_INPUT);
    gpio_set_pull_up(GPIOC, BUTTON_PIN);

    USART2->BRR = USART_BRR_VALUE;                   /* 139, range-checked by static_assert */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

/** @brief true once per press. 'previous' persists between calls (function-local static). */
static bool button_pressed_edge(void)
{
    static bool previous = false;                    /* false = released; .bss */
    const bool  now      = !gpio_read(GPIOC, BUTTON_PIN);   /* active low */
    const bool  edge     = now && !previous;

    previous = now;
    return edge;
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    board_init();
    Console_Init(uart_write);

    Console_Printf("\r\n=== A0.6 Hygiene bench (registers) ===\r\n");
    Console_Printf("event_counter v0x%08lX\r\n", (unsigned long)g_eventCounterVersion);
    (void)HygieneBench_Run();

    Console_Printf("\r\n--- Where the module's objects live ---\r\n");
    EventCounter_PrintStorage();
    Console_Printf("\r\nPress B1: each press is counted by event_counter.c and toggles LD2.\r\n");

    uint32_t msSinceTick = 0U;

    for (;;)
    {
        delay_ms(10U);
        msSinceTick += 10U;
        if (msSinceTick >= 1000U)
        {
            msSinceTick = 0U;
            EventCounter_Record(EVENT_TICK);
        }

        if (button_pressed_edge())
        {
            EventCounter_Record(EVENT_BUTTON);
            gpio_toggle(GPIOA, LED_PIN);
            Console_Printf("%s #%lu at %s %lu s\r\n",
                           EventCounter_Name(EVENT_BUTTON), (unsigned long)EventCounter_Get(EVENT_BUTTON),
                           EventCounter_Name(EVENT_TICK), (unsigned long)EventCounter_Get(EVENT_TICK));
        }
    }
}
