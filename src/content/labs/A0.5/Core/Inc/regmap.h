/**
 * @file    regmap.h
 * @brief   A0.5: hand-written register maps for GPIO, USART, RCC and EXTI
 *          (STM32F446, RM0390), checked at compile time.
 *
 * These are *our own* versions of the CMSIS GPIO_TypeDef, USART_TypeDef,
 * RCC_TypeDef and EXTI_TypeDef. The header needs no vendor file, so it
 * builds on a PC too. Every register offset is pinned to the reference
 * manual with a _Static_assert: if a member is missing, mistyped or in the
 * wrong order, the build fails instead of the board.
 *
 * regmap_check.h (target only) additionally compares every offset with the
 * vendor header.
 */
#ifndef REGMAP_H
#define REGMAP_H

#include <stddef.h>
#include <stdint.h>

/* Access qualifiers: same meaning as CMSIS __IO / __I / __O (lesson A0.4).  */
#define MY_RW   volatile          /**< read/write                            */
#define MY_RO   volatile const    /**< read-only: writing is a compile error */
#define MY_WO   volatile          /**< write-only: reads return 0            */

/* ------------------------------------------------------------------------- */
/* GPIO: RM0390 "GPIO registers", ports A..H, 0x400 apart                    */
/* ------------------------------------------------------------------------- */
typedef struct
{
    MY_RW uint32_t MODER;     /**< 0x00 mode, 2 bits per pin                  */
    MY_RW uint32_t OTYPER;    /**< 0x04 output type, 1 bit per pin            */
    MY_RW uint32_t OSPEEDR;   /**< 0x08 output speed, 2 bits per pin          */
    MY_RW uint32_t PUPDR;     /**< 0x0C pull-up/pull-down, 2 bits per pin     */
    MY_RO uint32_t IDR;       /**< 0x10 input data (r)                        */
    MY_RW uint32_t ODR;       /**< 0x14 output data                           */
    MY_WO uint32_t BSRR;      /**< 0x18 bit set/reset (w)                     */
    MY_RW uint32_t LCKR;      /**< 0x1C configuration lock                    */
    MY_RW uint32_t AFR[2];    /**< 0x20 AFRL (pins 0-7), 0x24 AFRH (pins 8-15) */
} my_gpio_t;

_Static_assert(offsetof(my_gpio_t, MODER)   == 0x00U, "GPIO MODER offset");
_Static_assert(offsetof(my_gpio_t, OTYPER)  == 0x04U, "GPIO OTYPER offset");
_Static_assert(offsetof(my_gpio_t, OSPEEDR) == 0x08U, "GPIO OSPEEDR offset");
_Static_assert(offsetof(my_gpio_t, PUPDR)   == 0x0CU, "GPIO PUPDR offset");
_Static_assert(offsetof(my_gpio_t, IDR)     == 0x10U, "GPIO IDR offset");
_Static_assert(offsetof(my_gpio_t, ODR)     == 0x14U, "GPIO ODR offset");
_Static_assert(offsetof(my_gpio_t, BSRR)    == 0x18U, "GPIO BSRR offset");
_Static_assert(offsetof(my_gpio_t, LCKR)    == 0x1CU, "GPIO LCKR offset");
_Static_assert(offsetof(my_gpio_t, AFR)     == 0x20U, "GPIO AFRL offset");
_Static_assert(sizeof(my_gpio_t)            == 0x28U, "GPIO block size");

/* ------------------------------------------------------------------------- */
/* USART: RM0390 "USART registers" (USART1/2/3/6, UART4/5)                   */
/* ------------------------------------------------------------------------- */
typedef struct
{
    MY_RW uint32_t SR;        /**< 0x00 status (TC, RXNE are rc_w0, A0.2)     */
    MY_RW uint32_t DR;        /**< 0x04 data (bits 8:0)                        */
    MY_RW uint32_t BRR;       /**< 0x08 baud rate: mantissa 15:4, fraction 3:0 */
    MY_RW uint32_t CR1;       /**< 0x0C control 1 (UE, M, TE, RE, ...)         */
    MY_RW uint32_t CR2;       /**< 0x10 control 2                              */
    MY_RW uint32_t CR3;       /**< 0x14 control 3                              */
    MY_RW uint32_t GTPR;      /**< 0x18 guard time and prescaler               */
} my_usart_t;

_Static_assert(offsetof(my_usart_t, SR)   == 0x00U, "USART SR offset");
_Static_assert(offsetof(my_usart_t, DR)   == 0x04U, "USART DR offset");
_Static_assert(offsetof(my_usart_t, BRR)  == 0x08U, "USART BRR offset");
_Static_assert(offsetof(my_usart_t, CR1)  == 0x0CU, "USART CR1 offset");
_Static_assert(offsetof(my_usart_t, GTPR) == 0x18U, "USART GTPR offset");
_Static_assert(sizeof(my_usart_t)         == 0x1CU, "USART block size");

/* ------------------------------------------------------------------------- */
/* RCC: RM0390 "RCC register map". The holes are real: the RM lists no       */
/* register there, so the struct must reserve the space.                     */
/* ------------------------------------------------------------------------- */
typedef struct
{
    MY_RW uint32_t CR;            /**< 0x00 clock control                     */
    MY_RW uint32_t PLLCFGR;       /**< 0x04 main PLL configuration            */
    MY_RW uint32_t CFGR;          /**< 0x08 clock configuration               */
    MY_RW uint32_t CIR;           /**< 0x0C clock interrupt                   */
    MY_RW uint32_t AHB1RSTR;      /**< 0x10                                   */
    MY_RW uint32_t AHB2RSTR;      /**< 0x14                                   */
    MY_RW uint32_t AHB3RSTR;      /**< 0x18                                   */
    uint32_t       RESERVED0;     /**< 0x1C (hole)                            */
    MY_RW uint32_t APB1RSTR;      /**< 0x20                                   */
    MY_RW uint32_t APB2RSTR;      /**< 0x24                                   */
    uint32_t       RESERVED1[2];  /**< 0x28-0x2C (hole)                       */
    MY_RW uint32_t AHB1ENR;       /**< 0x30 GPIOx clocks                      */
    MY_RW uint32_t AHB2ENR;       /**< 0x34                                   */
    MY_RW uint32_t AHB3ENR;       /**< 0x38                                   */
    uint32_t       RESERVED2;     /**< 0x3C (hole)                            */
    MY_RW uint32_t APB1ENR;       /**< 0x40 USART2, TIM2..7, I2C, ...         */
    MY_RW uint32_t APB2ENR;       /**< 0x44 USART1/6, SPI1, SYSCFG, ...       */
    uint32_t       RESERVED3[2];  /**< 0x48-0x4C (hole)                       */
    MY_RW uint32_t AHB1LPENR;     /**< 0x50                                   */
    MY_RW uint32_t AHB2LPENR;     /**< 0x54                                   */
    MY_RW uint32_t AHB3LPENR;     /**< 0x58                                   */
    uint32_t       RESERVED4;     /**< 0x5C (hole)                            */
    MY_RW uint32_t APB1LPENR;     /**< 0x60                                   */
    MY_RW uint32_t APB2LPENR;     /**< 0x64                                   */
    uint32_t       RESERVED5[2];  /**< 0x68-0x6C (hole)                       */
    MY_RW uint32_t BDCR;          /**< 0x70 backup domain control             */
    MY_RW uint32_t CSR;           /**< 0x74 control/status (reset flags)      */
    uint32_t       RESERVED6[2];  /**< 0x78-0x7C (hole)                       */
    MY_RW uint32_t SSCGR;         /**< 0x80 spread spectrum                   */
    MY_RW uint32_t PLLI2SCFGR;    /**< 0x84                                   */
    MY_RW uint32_t PLLSAICFGR;    /**< 0x88                                   */
    MY_RW uint32_t DCKCFGR;       /**< 0x8C dedicated clocks 1                */
    MY_RW uint32_t CKGATENR;      /**< 0x90 clock gating                      */
    MY_RW uint32_t DCKCFGR2;      /**< 0x94 dedicated clocks 2                */
} my_rcc_t;

_Static_assert(offsetof(my_rcc_t, APB1RSTR) == 0x20U, "RCC APB1RSTR offset");
_Static_assert(offsetof(my_rcc_t, AHB1ENR)  == 0x30U, "RCC AHB1ENR offset");
_Static_assert(offsetof(my_rcc_t, APB1ENR)  == 0x40U, "RCC APB1ENR offset");
_Static_assert(offsetof(my_rcc_t, APB2ENR)  == 0x44U, "RCC APB2ENR offset");
_Static_assert(offsetof(my_rcc_t, BDCR)     == 0x70U, "RCC BDCR offset");
_Static_assert(offsetof(my_rcc_t, CSR)      == 0x74U, "RCC CSR offset");
_Static_assert(offsetof(my_rcc_t, SSCGR)    == 0x80U, "RCC SSCGR offset");
_Static_assert(offsetof(my_rcc_t, DCKCFGR2) == 0x94U, "RCC DCKCFGR2 offset");
_Static_assert(sizeof(my_rcc_t)             == 0x98U, "RCC block size");

/* ------------------------------------------------------------------------- */
/* EXTI: RM0390 "EXTI registers"                                             */
/* ------------------------------------------------------------------------- */
typedef struct
{
    MY_RW uint32_t IMR;       /**< 0x00 interrupt mask                        */
    MY_RW uint32_t EMR;       /**< 0x04 event mask                            */
    MY_RW uint32_t RTSR;      /**< 0x08 rising trigger selection              */
    MY_RW uint32_t FTSR;      /**< 0x0C falling trigger selection             */
    MY_RW uint32_t SWIER;     /**< 0x10 software interrupt event              */
    MY_RW uint32_t PR;        /**< 0x14 pending (rc_w1: write 1 to clear)     */
} my_exti_t;

_Static_assert(offsetof(my_exti_t, PR) == 0x14U, "EXTI PR offset");
_Static_assert(sizeof(my_exti_t)       == 0x18U, "EXTI block size");

/* ------------------------------------------------------------------------- */
/* Base addresses (RM0390 "Memory map") and the overlay pointers             */
/* ------------------------------------------------------------------------- */
#define MY_GPIOA_BASE    (0x40020000UL)   /**< AHB1                          */
#define MY_GPIOC_BASE    (0x40020800UL)   /**< = GPIOA + 2 * 0x400           */
#define MY_RCC_BASE      (0x40023800UL)   /**< AHB1                          */
#define MY_USART2_BASE   (0x40004400UL)   /**< APB1                          */
#define MY_EXTI_BASE     (0x40013C00UL)   /**< APB2                          */

#define MY_GPIOA         ((my_gpio_t *)MY_GPIOA_BASE)
#define MY_GPIOC         ((my_gpio_t *)MY_GPIOC_BASE)
#define MY_RCC           ((my_rcc_t *)MY_RCC_BASE)
#define MY_USART2        ((my_usart_t *)MY_USART2_BASE)
#define MY_EXTI          ((my_exti_t *)MY_EXTI_BASE)

/**
 * @brief Absolute address of a register without dereferencing anything:
 *        MY_REG_ADDR(MY_USART2_BASE, my_usart_t, BRR) == 0x40004408.
 *        Pure integer arithmetic, so it is a constant expression.
 */
#define MY_REG_ADDR(base, type, member)  ((uint32_t)(base) + (uint32_t)offsetof(type, member))

_Static_assert(MY_REG_ADDR(MY_GPIOA_BASE, my_gpio_t, BSRR)   == 0x40020018UL, "GPIOA->BSRR address");
_Static_assert(MY_REG_ADDR(MY_USART2_BASE, my_usart_t, BRR)  == 0x40004408UL, "USART2->BRR address");
_Static_assert(MY_REG_ADDR(MY_RCC_BASE, my_rcc_t, AHB1ENR)   == 0x40023830UL, "RCC->AHB1ENR address");
_Static_assert(MY_REG_ADDR(MY_EXTI_BASE, my_exti_t, PR)      == 0x40013C14UL, "EXTI->PR address");

#endif /* REGMAP_H */
