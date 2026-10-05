/**
 * @file  button_drv.c
 * @brief Exercise A0.4 🟡: an input driver for B1 (PC13, active low) plus
 *        a host unit test that plays the part of the GPIO hardware.
 *
 * Host test:  gcc -std=c11 -Wall -Wextra -DBUTTON_DRV_HOST_TEST -ICore/Inc solutions/button_drv.c
 * On target:  ButtonDrv_Init(&b1, (volatile uint32_t *)LAB_GPIOC_BASE, 13U);
 */
#include <stdbool.h>
#include <stdint.h>

#include "mmio.h"

#define PUPDR_FIELD_MASK   (0x3UL)
#define PUPDR_PULL_UP      (0x1UL)
#define MODER_FIELD_MASK   (0x3UL)   /* input = 00 */

typedef struct
{
    const volatile uint32_t *idr;    /**< only IDR is needed after init: read-only view */
    uint32_t                 pin;
} ButtonDrv;

/** @brief PC13 -> input with pull-up; keeps a read-only pointer to IDR. */
void ButtonDrv_Init(ButtonDrv *btn, volatile uint32_t *port, uint32_t pin)
{
    volatile uint32_t *const moder = reg_at(port, GPIO_MODER_OFS);
    volatile uint32_t *const pupdr = reg_at(port, GPIO_PUPDR_OFS);
    const uint32_t           pos   = pin * 2U;

    *moder = *moder & ~(MODER_FIELD_MASK << pos);
    *pupdr = (*pupdr & ~(PUPDR_FIELD_MASK << pos)) | (PUPDR_PULL_UP << pos);

    btn->idr = reg_at(port, GPIO_IDR_OFS);   /* volatile -> const volatile: implicit */
    btn->pin = pin;
}

/** @brief B1 connects the pin to GND when pressed: pressed = bit clear. */
bool ButtonDrv_IsPressed(const ButtonDrv *btn)
{
    return (*btn->idr & (1UL << btn->pin)) == 0U;
}

#ifdef BUTTON_DRV_HOST_TEST
#include <stdio.h>

int main(void)
{
    uint32_t  fakeC[GPIO_BLOCK_WORDS] = { 0 };
    ButtonDrv b1;
    int       failures = 0;

    fakeC[GPIO_MODER_OFS / 4U] = 0xFFFFFFFFUL;        /* everything analog (worst case) */
    ButtonDrv_Init(&b1, fakeC, 13U);
    failures += (fakeC[GPIO_MODER_OFS / 4U] != 0xF3FFFFFFUL);   /* only bits 27:26 cleared */
    failures += (fakeC[GPIO_PUPDR_OFS / 4U] != 0x04000000UL);   /* bits 27:26 = 01         */

    fakeC[GPIO_IDR_OFS / 4U] = 1UL << 13;              /* released: pulled high */
    failures += ButtonDrv_IsPressed(&b1);
    fakeC[GPIO_IDR_OFS / 4U] = 0U;                     /* pressed: shorted to GND */
    failures += !ButtonDrv_IsPressed(&b1);

    printf("button_drv host test: %s\n", (failures == 0) ? "4/4 passed" : "FAILED");
    return failures;
}
#endif
