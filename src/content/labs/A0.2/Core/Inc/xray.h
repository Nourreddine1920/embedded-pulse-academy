/**
 * @file    xray.h
 * @brief   A0.2: prints 32-bit registers in binary and marks changed bits.
 *
 * Hardware-independent: output goes through an injected write function.
 */
#ifndef XRAY_H
#define XRAY_H

#include <stdint.h>

/** @brief Writes a NUL-terminated string to the console. */
typedef void (*Xray_WriteFn)(const char *text);

/** @brief Selects the console used by every Xray_* function. */
void Xray_Init(Xray_WriteFn write);

/** @brief Prints a bit-index ruler aligned with Xray_Show / Xray_Diff. */
void Xray_Ruler(void);

/** @brief Prints one value: name, hex and grouped binary. */
void Xray_Show(const char *name, uint32_t value);

/**
 * @brief Prints @p before and @p after and a '^' under every changed bit,
 *        followed by the list of changed bit numbers.
 */
void Xray_Diff(const char *name, uint32_t before, uint32_t after);

/** @brief printf-style helper through the same console. */
void Xray_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif /* XRAY_H */
