/**
 * @file    led_drv.c
 * @brief   A0.4: GPIO output driver built on base + offset pointers.
 */
#include "led_drv.h"

#include "mmio.h"

#define MODER_FIELD_MASK   (0x3UL)   /**< 2 bits per pin                */
#define MODER_OUTPUT       (0x1UL)   /**< 01 = general-purpose output   */
#define BSRR_RESET_SHIFT   (16U)     /**< bits 31:16 reset pins 15:0    */

void LedDrv_Init(LedDrv *led, volatile uint32_t *port, uint32_t pin)
{
    volatile uint32_t *const moder  = reg_at(port, GPIO_MODER_OFS);
    volatile uint32_t *const otyper = reg_at(port, GPIO_OTYPER_OFS);
    const uint32_t           pos    = pin * 2U;

    led->port = port;
    led->pin  = pin;

    /* One read and one write per register: the field change is visible. */
    *moder  = (*moder & ~(MODER_FIELD_MASK << pos)) | (MODER_OUTPUT << pos);
    *otyper = *otyper & ~(1UL << pin);
}

void LedDrv_On(const LedDrv *led)
{
    *reg_at(led->port, GPIO_BSRR_OFS) = 1UL << led->pin;
}

void LedDrv_Off(const LedDrv *led)
{
    *reg_at(led->port, GPIO_BSRR_OFS) = 1UL << (led->pin + BSRR_RESET_SHIFT);
}

bool LedDrv_IsOn(const LedDrv *led)
{
    return (*reg_at(led->port, GPIO_ODR_OFS) & (1UL << led->pin)) != 0U;
}

void LedDrv_Toggle(const LedDrv *led)
{
    if (LedDrv_IsOn(led))
    {
        LedDrv_Off(led);
    }
    else
    {
        LedDrv_On(led);
    }
}
