/**
 * @file    timestamp.c
 * @brief   A0.3 exercise 🔴: a 48-bit microsecond timestamp built from the
 *          16-bit TIM3 counter plus an overflow count kept by its ISR.
 *
 * The value lives in two places (s_overflows in RAM, CNT in the timer), so
 * no single load can read it. volatile makes each read happen; it cannot make
 * the pair consistent. Two correct readers are shown.
 */
#include <stdint.h>
#include "stm32f4xx.h"

#define TIMER_CLOCK_HZ     (16000000UL)
#define TICK_HZ            (1000000UL)          /**< 1 tick = 1 µs */

static volatile uint32_t s_overflows;          /**< Written only by TIM3_IRQHandler. */

void Timestamp_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    (void)RCC->APB1ENR;

    TIM3->PSC  = (uint32_t)(TIMER_CLOCK_HZ / TICK_HZ) - 1U;   /* 15 -> 1 MHz        */
    TIM3->ARR  = 0xFFFFU;                                     /* wrap every 65.536 ms */
    TIM3->EGR  = TIM_EGR_UG;
    TIM3->SR   = ~TIM_SR_UIF;
    TIM3->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1  = TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
    TIM3->SR = ~TIM_SR_UIF;
    s_overflows++;                                            /* single writer */
}

/** @brief BUG: if CNT wraps between the two reads, the result jumps back 65.5 ms. */
uint64_t Timestamp_NowUs_Buggy(void)
{
    const uint32_t high = s_overflows;
    const uint32_t low  = TIM3->CNT;          /* wrap here -> old high + new low */

    return ((uint64_t)high << 16) | low;
}

/**
 * @brief Fixed, lock-free: re-read until the overflow count did not change.
 *        Correct from thread mode and from ISRs of LOWER priority than TIM3.
 */
uint64_t Timestamp_NowUs(void)
{
    uint32_t high;
    uint32_t low;

    do
    {
        high = s_overflows;
        low  = TIM3->CNT;
    } while (high != s_overflows);            /* an overflow ISR ran: try again */

    return ((uint64_t)high << 16) | low;
}

/**
 * @brief Fixed for any context, even with interrupts disabled or from an ISR
 *        of higher priority: the overflow ISR cannot run there, so check the
 *        pending flag ourselves.
 */
uint64_t Timestamp_NowUs_AnyContext(void)
{
    const uint32_t primask = __get_PRIMASK();

    __disable_irq();
    uint32_t high = s_overflows;
    uint32_t low  = TIM3->CNT;
    if ((TIM3->SR & TIM_SR_UIF) != 0U)        /* wrapped, ISR not run yet  */
    {
        high++;
        low = TIM3->CNT;                      /* re-read: now after the wrap */
    }
    __set_PRIMASK(primask);

    return ((uint64_t)high << 16) | low;
}
