/**
 * @file  gpio_drv.c
 * @brief Exercise A0.2 🟡: minimal register-level GPIO driver built on bits.h.
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "bits.h"

/** @brief Pin configuration, values exactly as encoded in RM0390. */
typedef struct
{
    uint32_t mode;    /**< MODER:   0 input, 1 output, 2 AF, 3 analog     */
    uint32_t otype;   /**< OTYPER:  0 push-pull, 1 open-drain              */
    uint32_t speed;   /**< OSPEEDR: 0 low, 1 medium, 2 high, 3 very high   */
    uint32_t pull;    /**< PUPDR:   0 none, 1 pull-up, 2 pull-down         */
    uint32_t af;      /**< AFRL/AFRH: 0..15, only used when mode == 2      */
} GpioPinConfig;

#define GPIO_PINS_PER_AFR   (8U)
#define GPIO_AFR_WIDTH      (4U)
#define GPIO_FIELD2_WIDTH   (2U)

/**
 * @brief Configures one pin, touching only that pin's fields.
 * @note  Each register is a separate read-modify-write: call this during
 *        initialisation, not from code that races with ISRs on the same port.
 */
void gpio_config_pin(GPIO_TypeDef *port, uint32_t pin, const GpioPinConfig *cfg)
{
    const uint32_t pos2   = pin * GPIO_FIELD2_WIDTH;
    const uint32_t mask2  = FIELD_MASK(GPIO_FIELD2_WIDTH, pos2);
    const uint32_t afrIdx = pin / GPIO_PINS_PER_AFR;              /* 0 = AFRL, 1 = AFRH */
    const uint32_t afrPos = (pin % GPIO_PINS_PER_AFR) * GPIO_AFR_WIDTH;

    /* Set the alternate function BEFORE switching the mode to AF, so the
     * pin never briefly drives a wrong peripheral signal. */
    port->AFR[afrIdx] = field_set(port->AFR[afrIdx], FIELD_MASK(GPIO_AFR_WIDTH, afrPos), afrPos, cfg->af);
    port->OTYPER  = field_set(port->OTYPER, BIT(pin), pin, cfg->otype);
    port->OSPEEDR = field_set(port->OSPEEDR, mask2, pos2, cfg->speed);
    port->PUPDR   = field_set(port->PUPDR, mask2, pos2, cfg->pull);
    port->MODER   = field_set(port->MODER, mask2, pos2, cfg->mode);
}

/** @brief Atomic pin write: one store to BSRR. */
void gpio_write(GPIO_TypeDef *port, uint32_t pin, bool high)
{
    port->BSRR = high ? BIT(pin) : BIT(pin + 16U);
}

/** @brief Toggle that only affects @p pin, HAL-style (reads ODR, writes BSRR). */
void gpio_toggle(GPIO_TypeDef *port, uint32_t pin)
{
    const uint32_t odr = port->ODR;
    port->BSRR = ((odr & BIT(pin)) << 16U) | (~odr & BIT(pin));
}

/** @brief Returns the input level of @p pin. */
bool gpio_read(const GPIO_TypeDef *port, uint32_t pin)
{
    return bits_any_set(port->IDR, BIT(pin));
}
