/**
 * @file    main_reg.c
 * @brief   A0.2 Register X-ray: register-level version (NUCLEO-F446RE, no HAL).
 *
 * Every GPIO operation is a bit operation from bits.h on a register named
 * exactly as in RM0390. The X-ray prints each register before and after.
 *
 * Clock: HSI 16 MHz (reset state).  Console: USART2 115200 8N1.
 * B1 = PC13 (active low), LD2 = PA5.
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "bits.h"
#include "bits_selftest.h"
#include "xray.h"

/* ------------------------------------------------------------------------- */
/* Constants                                                                 */
/* ------------------------------------------------------------------------- */

#define CPU_CLOCK_HZ          (16000000UL)
#define CYCLES_PER_MS         (CPU_CLOCK_HZ / 1000UL)
#define UART_BAUD             (115200UL)
#define BUTTON_SAMPLE_MS      (10U)

#define LED_PIN               (5U)     /**< PA5 = LD2                    */
#define BUTTON_PIN            (13U)    /**< PC13 = B1, active low        */
#define UART_TX_PIN           (2U)     /**< PA2 = USART2_TX              */
#define UART_RX_PIN           (3U)     /**< PA3 = USART2_RX              */

#define MODER_INPUT           (0U)     /**< MODER field values (RM0390)  */
#define MODER_OUTPUT          (1U)
#define MODER_AF              (2U)
#define PUPDR_PULL_UP         (1U)
#define AF7_USART2            (7U)

/** 2-bit-per-pin fields (MODER, OSPEEDR, PUPDR) and 4-bit AFR fields. */
#define PIN2_POS(pin)         ((pin) * 2U)
#define PIN2_MASK(pin)        FIELD_MASK(2U, PIN2_POS(pin))
#define AFRL_POS(pin)         ((pin) * 4U)
#define AFRL_MASK(pin)        FIELD_MASK(4U, AFRL_POS(pin))

/** BSRR: bits 0..15 set a pin, bits 16..31 reset it. Single write = atomic. */
#define BSRR_SET(pin)         BIT(pin)
#define BSRR_RESET(pin)       BIT((pin) + 16U)

/* CMSIS startup calls this before main(): enable the FPU (see A0.1 / A1.6). */
void SystemInit(void)
{
    SCB->CPACR |= (3UL << (10U * 2U)) | (3UL << (11U * 2U));
}

/* ------------------------------------------------------------------------- */
/* Console and timing (same as A0.1)                                         */
/* ------------------------------------------------------------------------- */

static void uart_write(const char *text)
{
    while (*text != '\0')
    {
        while (!bits_any_set(USART2->SR, USART_SR_TXE))
        {
        }
        USART2->DR = (uint32_t)(uint8_t)*text++;
    }
    while (!bits_any_set(USART2->SR, USART_SR_TC))
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
/* Initialisation, one X-ray per register write                              */
/* ------------------------------------------------------------------------- */

static void clocks_init(void)
{
    RCC->AHB1ENR = bits_set(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN);
    (void)RCC->AHB1ENR;                              /* wait for the clock (A4.5) */
    RCC->APB1ENR = bits_set(RCC->APB1ENR, RCC_APB1ENR_USART2EN);
    (void)RCC->APB1ENR;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  /* DWT cycle counter       */
    DWT->CYCCNT = 0U;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static void uart_init(void)
{
    reg_modify(&GPIOA->MODER, PIN2_MASK(UART_TX_PIN) | PIN2_MASK(UART_RX_PIN),
               (MODER_AF << PIN2_POS(UART_TX_PIN)) | (MODER_AF << PIN2_POS(UART_RX_PIN)));
    reg_modify(&GPIOA->AFR[0], AFRL_MASK(UART_TX_PIN) | AFRL_MASK(UART_RX_PIN),
               (AF7_USART2 << AFRL_POS(UART_TX_PIN)) | (AF7_USART2 << AFRL_POS(UART_RX_PIN)));

    USART2->BRR = (uint32_t)((CPU_CLOCK_HZ + (UART_BAUD / 2UL)) / UART_BAUD);  /* 0x8B */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 = bits_set(USART2->CR1, USART_CR1_UE);
}

static void led_and_button_init(void)
{
    uint32_t before;

    Xray_Printf("\r\n--- PA5 -> output: MODER field [11:10] = 01 ---\r\n");
    Xray_Ruler();
    before = GPIOA->MODER;
    GPIOA->MODER = field_set(GPIOA->MODER, PIN2_MASK(LED_PIN), PIN2_POS(LED_PIN), MODER_OUTPUT);
    Xray_Diff("GPIOA->MODER", before, GPIOA->MODER);

    Xray_Printf("\r\n--- PC13 -> input with pull-up: MODER[27:26] = 00, PUPDR[27:26] = 01 ---\r\n");
    Xray_Ruler();
    before = GPIOC->MODER;
    GPIOC->MODER = field_set(GPIOC->MODER, PIN2_MASK(BUTTON_PIN), PIN2_POS(BUTTON_PIN), MODER_INPUT);
    Xray_Diff("GPIOC->MODER", before, GPIOC->MODER);
    before = GPIOC->PUPDR;
    GPIOC->PUPDR = field_set(GPIOC->PUPDR, PIN2_MASK(BUTTON_PIN), PIN2_POS(BUTTON_PIN), PUPDR_PULL_UP);
    Xray_Diff("GPIOC->PUPDR", before, GPIOC->PUPDR);
}

/** @brief ODR read-modify-write vs BSRR single write, side by side. */
static void demo_odr_vs_bsrr(void)
{
    uint32_t before;

    Xray_Printf("\r\n--- Three ways to drive PA5 ---\r\n");
    Xray_Ruler();

    before = GPIOA->ODR;
    GPIOA->BSRR = BSRR_SET(LED_PIN);                          /* 1 store, atomic */
    Xray_Diff("BSRR set", before, GPIOA->ODR);

    before = GPIOA->ODR;
    GPIOA->BSRR = BSRR_RESET(LED_PIN);                        /* bit 21 resets 5 */
    Xray_Diff("BSRR reset", before, GPIOA->ODR);

    before = GPIOA->ODR;
    GPIOA->ODR = bits_toggle(GPIOA->ODR, BIT(LED_PIN));       /* LDR, EOR, STR   */
    Xray_Diff("ODR ^= BIT(5)", before, GPIOA->ODR);

    before = GPIOA->ODR;
    GPIOA->ODR = bits_toggle(GPIOA->ODR, BIT(LED_PIN));
    Xray_Diff("ODR ^= BIT(5)", before, GPIOA->ODR);
}

/** @brief Bit tricks on the reset value of GPIOA->MODER. */
static void demo_tricks(void)
{
    const uint32_t moderReset = 0xA8000000UL;

    Xray_Printf("\r\n--- Bit tricks on 0x%08lX (GPIOA->MODER reset value) ---\r\n", (unsigned long)moderReset);
    Xray_Printf("ones = %lu, lowest set bit = %lu, PA15 mode = %lu, PA13 mode = %lu\r\n",
                (unsigned long)bits_count(moderReset),
                (unsigned long)bits_lowest_index(moderReset),
                (unsigned long)field_get(moderReset, PIN2_MASK(15U), PIN2_POS(15U)),
                (unsigned long)field_get(moderReset, PIN2_MASK(13U), PIN2_POS(13U)));
}

/* ------------------------------------------------------------------------- */

int main(void)
{
    clocks_init();
    uart_init();
    Xray_Init(uart_write);

    Xray_Printf("\r\n=== A0.2 Register X-ray (registers) ===\r\n");
    (void)BitsSelfTest_Run();
    led_and_button_init();
    demo_odr_vs_bsrr();
    demo_tricks();
    Xray_Printf("\r\nPress B1: LED follows the button; each edge prints GPIOC->IDR.\r\n");

    uint32_t previous = GPIOC->IDR;
    uint32_t presses  = 0U;

    for (;;)
    {
        const uint32_t now     = GPIOC->IDR;
        const uint32_t changed = now ^ previous;           /* 1 where a pin changed */

        if (bits_any_set(changed, BIT(BUTTON_PIN)))
        {
            const bool pressed = !bits_any_set(now, BIT(BUTTON_PIN));   /* active low */

            GPIOA->BSRR = pressed ? BSRR_SET(LED_PIN) : BSRR_RESET(LED_PIN);
            if (pressed)
            {
                presses++;
            }
            Xray_Printf("%s #%lu\r\n", pressed ? "PRESS  " : "RELEASE", (unsigned long)presses);
            Xray_Diff("GPIOC->IDR", previous, now);
        }
        previous = now;
        delay_ms(BUTTON_SAMPLE_MS);
    }
}
