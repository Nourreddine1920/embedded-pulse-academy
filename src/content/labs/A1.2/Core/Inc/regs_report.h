/**
 * @file    regs_report.h
 * @brief   A1.2: console output, self-test and tables for the register model.
 *
 * Hardware-independent: output goes through an injected write function.
 */
#ifndef REGS_REPORT_H
#define REGS_REPORT_H

#include <stdint.h>

#include "core_regs.h"

/** @brief Writes a NUL-terminated string to the console. */
typedef void (*Regs_WriteFn)(const char *text);

/** @brief Selects the console used by every Regs_* function. */
void Regs_Init(Regs_WriteFn write);

/** @brief printf-style helper through the same console. */
void Regs_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/** @brief Model checks. @return number of failed checks. */
uint32_t Regs_SelfTest(void);

/** @brief ADDS/SUBS flag table for a fixed list of operand pairs. */
void Regs_PrintFlagTable(void);

/** @brief Which priority levels each mask configuration blocks (4 implemented bits). */
void Regs_PrintMaskTable(void);

/** @brief The AAPCS roles of R0-R15 and the special registers. */
void Regs_PrintRegisterRoles(void);

/** @brief Prints xPSR and CONTROL values decoded into fields. */
void Regs_PrintXpsr(const char *label, uint32_t xpsr);
void Regs_PrintControl(uint32_t control);

#endif /* REGS_REPORT_H */
