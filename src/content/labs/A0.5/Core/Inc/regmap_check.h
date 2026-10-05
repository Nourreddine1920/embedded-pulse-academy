/**
 * @file    regmap_check.h
 * @brief   A0.5 (target only): proves at compile time that our register maps
 *          in regmap.h match ST's CMSIS device header byte for byte.
 *
 * Include it in exactly one .c file of the firmware project, after
 * stm32f4xx.h. Nothing here generates code: a mismatch stops the build.
 */
#ifndef REGMAP_CHECK_H
#define REGMAP_CHECK_H

#include <stddef.h>
#include "stm32f4xx.h"
#include "regmap.h"

/** Same offset in our struct and in the vendor struct. */
#define REGMAP_SAME(mine, cmsis, member)                                   \
    _Static_assert(offsetof(mine, member) == offsetof(cmsis, member),      \
                   #mine "." #member " differs from " #cmsis)

REGMAP_SAME(my_gpio_t, GPIO_TypeDef, MODER);
REGMAP_SAME(my_gpio_t, GPIO_TypeDef, IDR);
REGMAP_SAME(my_gpio_t, GPIO_TypeDef, BSRR);
REGMAP_SAME(my_gpio_t, GPIO_TypeDef, LCKR);
REGMAP_SAME(my_gpio_t, GPIO_TypeDef, AFR);
_Static_assert(sizeof(my_gpio_t) == sizeof(GPIO_TypeDef), "GPIO size");

REGMAP_SAME(my_usart_t, USART_TypeDef, SR);
REGMAP_SAME(my_usart_t, USART_TypeDef, BRR);
REGMAP_SAME(my_usart_t, USART_TypeDef, CR1);
REGMAP_SAME(my_usart_t, USART_TypeDef, GTPR);
_Static_assert(sizeof(my_usart_t) == sizeof(USART_TypeDef), "USART size");

REGMAP_SAME(my_rcc_t, RCC_TypeDef, APB1RSTR);
REGMAP_SAME(my_rcc_t, RCC_TypeDef, AHB1ENR);
REGMAP_SAME(my_rcc_t, RCC_TypeDef, APB1ENR);
REGMAP_SAME(my_rcc_t, RCC_TypeDef, BDCR);
REGMAP_SAME(my_rcc_t, RCC_TypeDef, SSCGR);
REGMAP_SAME(my_rcc_t, RCC_TypeDef, DCKCFGR2);
_Static_assert(sizeof(my_rcc_t) == sizeof(RCC_TypeDef), "RCC size");

REGMAP_SAME(my_exti_t, EXTI_TypeDef, PR);
_Static_assert(sizeof(my_exti_t) == sizeof(EXTI_TypeDef), "EXTI size");

/* Base addresses: ours must be the vendor's. (Integer compare, no deref.) */
_Static_assert(MY_GPIOA_BASE  == GPIOA_BASE,  "GPIOA base");
_Static_assert(MY_GPIOC_BASE  == GPIOC_BASE,  "GPIOC base");
_Static_assert(MY_RCC_BASE    == RCC_BASE,    "RCC base");
_Static_assert(MY_USART2_BASE == USART2_BASE, "USART2 base");
_Static_assert(MY_EXTI_BASE   == EXTI_BASE,   "EXTI base");

#endif /* REGMAP_CHECK_H */
