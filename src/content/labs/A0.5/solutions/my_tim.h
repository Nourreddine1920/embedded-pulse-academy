/**
 * @file  my_tim.h
 * @brief Exercise A0.5 🟡: TIM2..TIM5 register map (RM0390 "TIM2 to TIM5
 *        registers"), pinned to the reference manual with _Static_assert.
 *
 * The same struct type describes all general-purpose timers. Offsets 0x30
 * (RCR) and 0x44 (BDTR) exist only on the advanced timers TIM1/TIM8, so
 * here they are reserved words. CMSIS instead uses ONE TIM_TypeDef for every
 * timer and simply names them: on TIM2 they read as 0.
 */
#ifndef MY_TIM_H
#define MY_TIM_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    volatile uint32_t CR1;        /**< 0x00 control 1 (CEN, UDIS, URS, OPM, DIR, ARPE) */
    volatile uint32_t CR2;        /**< 0x04 control 2                                   */
    volatile uint32_t SMCR;       /**< 0x08 slave mode control                          */
    volatile uint32_t DIER;       /**< 0x0C DMA/interrupt enable                        */
    volatile uint32_t SR;         /**< 0x10 status (rc_w0 flags, A0.2)                  */
    volatile uint32_t EGR;        /**< 0x14 event generation (w)                        */
    volatile uint32_t CCMR1;      /**< 0x18 capture/compare mode 1                      */
    volatile uint32_t CCMR2;      /**< 0x1C capture/compare mode 2                      */
    volatile uint32_t CCER;       /**< 0x20 capture/compare enable                      */
    volatile uint32_t CNT;        /**< 0x24 counter (32-bit on TIM2/TIM5)               */
    volatile uint32_t PSC;        /**< 0x28 prescaler                                   */
    volatile uint32_t ARR;        /**< 0x2C auto-reload                                 */
    uint32_t          RESERVED0;  /**< 0x30 (RCR on TIM1/TIM8 only)                     */
    volatile uint32_t CCR[4];     /**< 0x34..0x40 capture/compare 1..4                  */
    uint32_t          RESERVED1;  /**< 0x44 (BDTR on TIM1/TIM8 only)                    */
    volatile uint32_t DCR;        /**< 0x48 DMA control                                 */
    volatile uint32_t DMAR;       /**< 0x4C DMA address for full transfer               */
    volatile uint32_t OR;         /**< 0x50 option (TIM2: ITR1 remap, TIM5: TI4 remap)  */
} my_tim_t;

_Static_assert(offsetof(my_tim_t, SR)     == 0x10U, "TIM SR");
_Static_assert(offsetof(my_tim_t, CNT)    == 0x24U, "TIM CNT");
_Static_assert(offsetof(my_tim_t, PSC)    == 0x28U, "TIM PSC");
_Static_assert(offsetof(my_tim_t, ARR)    == 0x2CU, "TIM ARR");
_Static_assert(offsetof(my_tim_t, CCR)    == 0x34U, "TIM CCR1");
_Static_assert(offsetof(my_tim_t, CCR[3]) == 0x40U, "TIM CCR4");
_Static_assert(offsetof(my_tim_t, DCR)    == 0x48U, "TIM DCR");
_Static_assert(offsetof(my_tim_t, OR)     == 0x50U, "TIM OR");
_Static_assert(sizeof(my_tim_t)           == 0x54U, "TIM block size");

#define MY_TIM2_BASE   (0x40000000UL)   /**< APB1, first peripheral of all */
#define MY_TIM3_BASE   (0x40000400UL)
#define MY_TIM4_BASE   (0x40000800UL)
#define MY_TIM5_BASE   (0x40000C00UL)

#define MY_TIM2        ((my_tim_t *)MY_TIM2_BASE)
#define MY_TIM5        ((my_tim_t *)MY_TIM5_BASE)

_Static_assert(MY_TIM2_BASE + offsetof(my_tim_t, CCR[1]) == 0x40000038UL, "TIM2->CCR2 address");

#endif /* MY_TIM_H */
