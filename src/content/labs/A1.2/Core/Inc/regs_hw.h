/**
 * @file    regs_hw.h
 * @brief   A1.2 (target only): reads the real core registers and runs the
 *          interrupt-masking experiment on the real NVIC.
 */
#ifndef REGS_HW_H
#define REGS_HW_H

#include <stdbool.h>
#include <stdint.h>

#include "core_regs.h"

/** @brief The special registers, as read by MRS in the current context. */
typedef struct
{
    uint32_t msp;
    uint32_t psp;
    uint32_t control;
    uint32_t primask;
    uint32_t basepri;
    uint32_t faultmask;
    uint32_t xpsr;
    uint32_t ipsr;
} RegsSnapshot;

/** @brief Result of one run of the masking experiment. */
typedef struct
{
    bool     predictedRun;        /**< the model: should the handler run at once?         */
    bool     ranWhileMasked;      /**< hardware: did the handler run with the mask applied? */
    bool     ranAfterUnmask;      /**< hardware: did it run once the mask was removed?     */
    uint32_t handlerIpsr;         /**< IPSR as seen inside the handler (22 for EXTI0)      */
    uint32_t basepriReadBack;     /**< BASEPRI after writing the requested value           */
} IrqExperiment;

/** @brief Reads MSP, PSP, CONTROL, PRIMASK, BASEPRI, FAULTMASK, xPSR and IPSR. */
void RegsHw_Snapshot(RegsSnapshot *out);

/**
 * @brief Pends EXTI0 (IRQ6) in software at priority @p level, with the masks of
 *        @p mask applied, and reports whether the handler ran.
 * @note  Needs EXTI0_IRQHandler from regs_hw.c; do not enable EXTI line 0 elsewhere.
 */
void RegsHw_IrqExperiment(const MaskState *mask, uint8_t level, IrqExperiment *out);

/**
 * @brief Prints the register snapshot (thread mode), then runs the masking
 *        experiment for a fixed list of cases and compares it with the model.
 * @return number of cases where the hardware disagreed with the model.
 */
uint32_t RegsHw_PrintReport(void);

#endif /* REGS_HW_H */
