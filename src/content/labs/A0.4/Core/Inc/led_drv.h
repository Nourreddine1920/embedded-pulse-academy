/**
 * @file    led_drv.h
 * @brief   A0.4: a one-pin GPIO output driver that never names an address.
 *
 * The driver receives the base address of a GPIO port as a pointer and
 * reaches every register as base + offset. On the STM32 the caller passes
 * (volatile uint32_t *)0x40020000; in a unit test it passes an ordinary
 * array that stands in for the ten GPIO registers. Same code, both places.
 */
#ifndef LED_DRV_H
#define LED_DRV_H

#include <stdbool.h>
#include <stdint.h>

/** @brief One output pin: which GPIO block, which pin (0..15). */
typedef struct
{
    volatile uint32_t *port;   /**< GPIOx base address (MODER, offset 0x00) */
    uint32_t           pin;    /**< 0..15                                   */
} LedDrv;

/**
 * @brief Binds @p led to @p port / @p pin and makes the pin a push-pull
 *        output (MODER field = 01, OTYPER bit = 0). Other pins untouched.
 * @note  The GPIO clock (RCC_AHB1ENR) must already be on.
 */
void LedDrv_Init(LedDrv *led, volatile uint32_t *port, uint32_t pin);

/** @brief Drives the pin high with one write to BSRR (atomic). */
void LedDrv_On(const LedDrv *led);

/** @brief Drives the pin low with one write to BSRR (atomic). */
void LedDrv_Off(const LedDrv *led);

/** @brief Reads ODR and writes the opposite level through BSRR. */
void LedDrv_Toggle(const LedDrv *led);

/** @brief true if the output latch (ODR) of the pin is 1. */
bool LedDrv_IsOn(const LedDrv *led);

#endif /* LED_DRV_H */
