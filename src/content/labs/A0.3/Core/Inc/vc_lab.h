/**
 * @file    vc_lab.h
 * @brief   A0.3 lab: what the optimizer may remove, why volatile is not
 *          atomic, and where const data really lives.
 *
 * Hardware-independent: the caller injects a console, a free-running cycle
 * counter and a "fast tick" interrupt source, so the same file runs on the
 * STM32 (HAL or register-level main) and on a PC (a thread plays the ISR).
 */
#ifndef VC_LAB_H
#define VC_LAB_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Writes a NUL-terminated string to the lab console. */
typedef void (*VcLab_WriteFn)(const char *text);

/** @brief Starts (true) or stops (false) the fast periodic interrupt. */
typedef void (*VcLab_TickFn)(bool enable);

/** @brief Names the memory region that holds @p address ("Flash", "SRAM"...). */
typedef const char *(*VcLab_RegionFn)(const void *address);

/** @brief Everything the lab needs from the platform. */
typedef struct
{
    VcLab_WriteFn            write;         /**< Console output (required).          */
    const volatile uint32_t *cycleCounter;  /**< Free-running counter, e.g. DWT_CYCCNT. */
    uint32_t                 cyclesPerMs;   /**< Counter ticks per millisecond.      */
    VcLab_TickFn             fastTick;      /**< Starts/stops the fast interrupt.    */
    VcLab_RegionFn           regionOf;      /**< Optional: NULL prints "?".          */
    const volatile uint32_t *cpuid;         /**< Optional: SCB->CPUID, or NULL.      */
} VcLab_Platform;

/**
 * @brief  The fast interrupt's work. Call it from the timer ISR (target) or
 *         from the "ISR" thread (PC). It sets the test flags and increments
 *         the shared counters.
 */
void VcLab_TickIsr(void);

/**
 * @brief  Runs every test and prints a report.
 * @param  platform  Platform hooks (write, cycleCounter and fastTick required).
 * @retval true if every "fixed" variant behaved correctly.
 */
bool VcLab_Run(const VcLab_Platform *platform);

#endif /* VC_LAB_H */
