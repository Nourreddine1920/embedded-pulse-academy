/**
 * @file    core_regs.h
 * @brief   A1.2: a software model of the Cortex-M special registers.
 *
 * Pure logic, no hardware: the condition-flag arithmetic of xPSR, the field
 * layout of xPSR and CONTROL, and the rule that PRIMASK, FAULTMASK and BASEPRI
 * use to decide whether an interrupt may run. The same code is unit-tested on
 * a PC and then compared with the real core in regs_hw.c.
 */
#ifndef CORE_REGS_H
#define CORE_REGS_H

#include <stdbool.h>
#include <stdint.h>

/* ---- Condition flags (APSR, xPSR bits 31:28) ----------------------------- */

/** @brief The four condition flags set by ADDS, SUBS, CMP, ... */
typedef struct
{
    bool n;   /**< negative: bit 31 of the result                         */
    bool z;   /**< zero: the result is 0                                  */
    bool c;   /**< carry: unsigned carry out (ADD); NOT borrow (SUB, CMP) */
    bool v;   /**< overflow: signed result does not fit in 32 bits        */
} Flags;

/** @brief Flags after `ADDS a, b`. */
Flags Flags_Add(uint32_t a, uint32_t b);

/** @brief Flags after `SUBS a, b` (and `CMP a, b`). C is set when a >= b unsigned. */
Flags Flags_Sub(uint32_t a, uint32_t b);

/* ---- xPSR and CONTROL fields ---------------------------------------------- */

/** @brief xPSR split into its fields (DUI 0553, "Program Status Register"). */
typedef struct
{
    bool     n, z, c, v;     /**< [31:28] condition flags                    */
    bool     q;              /**< [27]    sticky saturation (DSP instructions) */
    uint8_t  ge;             /**< [19:16] SIMD greater-or-equal flags        */
    uint8_t  it;             /**< IT block state: [26:25] and [15:10]        */
    bool     t;              /**< [24]    Thumb state: must always be 1      */
    uint16_t isr;            /**< [8:0]   exception number, 0 in thread mode */
} XpsrFields;

/** @brief Decodes an xPSR value. */
void Xpsr_Decode(uint32_t xpsr, XpsrFields *out);

/** @brief "Thread mode", "NMI", "HardFault", "SysTick", "IRQ6", ... */
const char *Exception_Name(uint16_t isr, char *scratch, unsigned scratchSize);

/** @brief CONTROL split into its fields. */
typedef struct
{
    bool nPriv;   /**< bit 0: 1 = thread mode is unprivileged               */
    bool spSel;   /**< bit 1: 1 = thread mode uses PSP, 0 = MSP             */
    bool fpca;    /**< bit 2: floating-point context is active (FPU parts)  */
} ControlFields;

/** @brief Decodes a CONTROL value. */
void Control_Decode(uint32_t control, ControlFields *out);

/* ---- Interrupt masking ----------------------------------------------------- */

/** @brief The three mask registers. */
typedef struct
{
    bool    primask;     /**< 1: all configurable exceptions are blocked             */
    bool    faultmask;   /**< 1: everything except NMI is blocked (even HardFault)   */
    uint8_t basepri;     /**< 0: off. Otherwise priorities numerically >= it are blocked */
} MaskState;

/** @brief The priority field of an exception, left-aligned in 8 bits (F446: 4 bits implemented). */
uint8_t Priority_Encode(uint8_t level, uint8_t implementedBits);

/** @brief The level (0 = most urgent) that a priority byte stands for. */
uint8_t Priority_Level(uint8_t priorityByte, uint8_t implementedBits);

/** @brief What BASEPRI reads back after writing @p value: unimplemented low bits read as 0. */
uint8_t Basepri_ReadBack(uint8_t value, uint8_t implementedBits);

/**
 * @brief true if a configurable exception with this priority byte is blocked.
 *        BASEPRI is compared as the hardware stores it: unimplemented low bits are 0.
 */
bool Irq_IsMasked(const MaskState *mask, uint8_t implementedBits, uint8_t priorityByte);

/** @brief Bit n of the result is set if priority level n is blocked (levels 0..15). */
uint16_t Irq_MaskedLevels(const MaskState *mask, uint8_t implementedBits);

#endif /* CORE_REGS_H */
