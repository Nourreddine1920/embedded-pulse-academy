/**
 * @file    int_lab.h
 * @brief   A0.1 lab: fixed-width types and integer-promotion pitfalls.
 *
 * The lab is hardware-independent: the caller injects an output function
 * and (optionally) a cycle counter, so the exact same file runs on the
 * STM32 (HAL or register-level main) and on a PC for comparison.
 */
#ifndef INT_LAB_H
#define INT_LAB_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Writes a NUL-terminated string to the lab console. */
typedef void (*IntLab_WriteFn)(const char *text);

/** @brief Returns a free-running cycle count (may wrap). */
typedef uint32_t (*IntLab_CycleFn)(void);

/**
 * @brief  Runs every lab test and prints a report.
 * @param  write   Console output function (must not be NULL).
 * @param  cycles  Cycle counter, or NULL to skip the timing test.
 * @retval true if every "fixed" variant produced the expected result.
 */
bool IntLab_Run(IntLab_WriteFn write, IntLab_CycleFn cycles);

#endif /* INT_LAB_H */
