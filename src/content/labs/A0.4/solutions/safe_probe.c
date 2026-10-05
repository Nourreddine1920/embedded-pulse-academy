/**
 * @file  safe_probe.c
 * @brief Exercise A0.4 🔴: read an address that might not exist, without
 *        crashing, using the Cortex-M3/M4 BFHFNMIGN mechanism.
 *
 * Normally a load from an unmapped address raises a precise BusFault, which
 * escalates to HardFault when BusFault is not enabled. With FAULTMASK set,
 * the CPU runs at priority -1; if CCR.BFHFNMIGN is also set, data bus faults
 * at that priority are ignored and only recorded in BFSR (part of CFSR).
 * ARMv7-M ARM, "Configuration and Control Register, CCR".
 */
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"

#define CFSR_BFSR_ALL   (0x0000FF00UL)   /**< BusFault status byte, rc_w1 bits */

/**
 * @brief  Reads the 32-bit word at @p addr.
 * @retval true and *out = value if the bus answered, false on a bus error.
 */
bool probe_read32(uintptr_t addr, uint32_t *out)
{
    const uint32_t faultmask = __get_FAULTMASK();
    uint32_t       value;
    bool           ok;

    SCB->CFSR = CFSR_BFSR_ALL;               /* clear old BusFault flags (write 1) */
    __disable_fault_irq();                   /* FAULTMASK = 1: priority -1         */
    SCB->CCR |= SCB_CCR_BFHFNMIGN_Msk;
    __DSB();
    __ISB();

    value = *(volatile const uint32_t *)addr;   /* may fault: ignored and recorded */
    __DSB();

    ok = (SCB->CFSR & (SCB_CFSR_PRECISERR_Msk | SCB_CFSR_IMPRECISERR_Msk)) == 0U;

    SCB->CCR &= ~SCB_CCR_BFHFNMIGN_Msk;
    SCB->CFSR = CFSR_BFSR_ALL;
    __DSB();
    __ISB();
    __set_FAULTMASK(faultmask);

    if (ok)
    {
        *out = value;
    }
    return ok;
}

/* Usage (NUCLEO-F446RE):
 *   uint32_t v;
 *   probe_read32(0x40020000UL, &v);   true,  v = GPIOA->MODER
 *   probe_read32(0xE0042000UL, &v);   true,  v = DBGMCU->IDCODE
 *   probe_read32(0x40008000UL, &v);   APB1 ends at 0x40007FFF: expect false
 */
