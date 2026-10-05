/**
 * @file    core_info.h
 * @brief   A1.1: decodes which Cortex-M core a chip contains, and what that
 *          core can do, from the ID and feature registers.
 *
 * Pure logic: takes raw register VALUES and returns decoded fields, so the same
 * code runs on the STM32 (values read from the core) and on a PC (values passed
 * in by a test).
 */
#ifndef CORE_INFO_H
#define CORE_INFO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** @brief SCB->CPUID split into its fields (DUI 0553, "CPUID Base Register"). */
typedef struct
{
    uint8_t  implementer;     /**< [31:24]  0x41 = Arm                       */
    uint8_t  variant;         /**< [23:20]  the "r" in r0p1                  */
    uint8_t  architecture;    /**< [19:16]  0xC = ARMv6-M, 0xF = see ID regs */
    uint16_t part;            /**< [15:4]   0xC24 = Cortex-M4                */
    uint8_t  revision;        /**< [3:0]    the "p" in r0p1                  */
} CoreId;

/** @brief Raw values of the registers the feature decoder needs. */
typedef struct
{
    uint32_t mvfr0;           /**< FPU: Media and VFP Feature Register 0, 0xE000EF40 */
    uint32_t isar0;           /**< ID_ISAR0, 0xE000ED60 (Divide_instrs in [27:24])   */
    uint32_t dwtCtrl;         /**< DWT->CTRL, 0xE0001000                              */
    uint32_t mpuType;         /**< MPU->TYPE, 0xE000ED90 (DREGION in [15:8])          */
    uint32_t cpacr;           /**< SCB->CPACR, 0xE000ED88 (CP10/CP11 in [23:20])      */
} FeatureRegs;

/** @brief What the registers say the core can do. */
typedef struct
{
    bool    fpuSingle;        /**< single-precision FPU present              */
    bool    fpuDouble;        /**< double-precision FPU present              */
    bool    fpuEnabled;       /**< CPACR gives full access to CP10 and CP11  */
    bool    hwDivide;         /**< SDIV / UDIV exist                         */
    bool    cycleCounter;     /**< DWT->CYCCNT exists (NOCYCCNT == 0)        */
    uint8_t dwtComparators;   /**< DWT->CTRL.NUMCOMP                         */
    uint8_t mpuRegions;       /**< MPU->TYPE.DREGION (0 = no MPU)            */
} Features;

/** @brief A row of the core comparison table (Arm TRMs; see the lesson). */
typedef struct
{
    uint16_t    part;         /**< CPUID part number                         */
    const char *name;         /**< "Cortex-M4"                               */
    const char *architecture; /**< "ARMv7E-M"                                */
    uint8_t     pipeline;     /**< pipeline stages                           */
    uint16_t    maxIrqs;      /**< external interrupt lines the core can have */
    bool        hwDivide;     /**< SDIV/UDIV in the instruction set          */
    bool        dsp;          /**< DSP / SIMD instructions (always, or optional on M33) */
    const char *fpu;          /**< "none", "opt. SP", "opt. SP/DP"           */
    bool        bitBand;      /**< bit-band alias regions                    */
    bool        trustZone;    /**< Security Extension                        */
} CoreSpec;

/** @brief Splits a CPUID value into its fields. */
void CoreInfo_ParseCpuid(uint32_t cpuid, CoreId *out);

/** @brief "Cortex-M4" for part 0xC24, ... or "unknown core". */
const char *CoreInfo_Name(uint16_t part);

/** @brief "ARMv6-M" or "ARMv7-M or ARMv8-M ..." for the CPUID architecture nibble. */
const char *CoreInfo_ArchitectureField(uint8_t architecture);

/** @brief Writes "Cortex-M4 r0p1" into buf (always NUL-terminated). */
void CoreInfo_Format(const CoreId *id, char *buf, size_t size);

/** @brief Decodes the feature registers. */
void CoreInfo_DecodeFeatures(const FeatureRegs *regs, Features *out);

/** @brief "none", "single precision" or "single + double precision". */
const char *CoreInfo_FpuName(const Features *features);

/** @brief The comparison-table row for a part number, or NULL. */
const CoreSpec *CoreInfo_Spec(uint16_t part);

/** @brief Number of rows in the comparison table. */
size_t CoreInfo_SpecCount(void);

/** @brief Row @p index of the comparison table (index < CoreInfo_SpecCount()). */
const CoreSpec *CoreInfo_SpecAt(size_t index);

#endif /* CORE_INFO_H */
