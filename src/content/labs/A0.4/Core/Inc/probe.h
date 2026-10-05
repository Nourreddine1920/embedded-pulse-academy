/**
 * @file    probe.h
 * @brief   A0.4: console output for the register probe (hardware-independent).
 *
 * Output goes through an injected write function, so the same report runs
 * over USART2 on the board and over stdout on a PC.
 */
#ifndef PROBE_H
#define PROBE_H

#include <stdint.h>

#include "devinfo.h"

/** @brief Writes a NUL-terminated string to the console. */
typedef void (*Probe_WriteFn)(const char *text);

/** @brief Selects the console used by every Probe_* function. */
void Probe_Init(Probe_WriteFn write);

/** @brief printf-style helper through the same console. */
void Probe_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/** @brief Prints every lab register as base + offset = address. */
void Probe_AddressTable(void);

/** @brief Shows that p + n moves n * sizeof(*p) bytes. No memory is accessed. */
void Probe_PointerArithmetic(void);

/** @brief Prints a decoded DevInfo. */
void Probe_PrintDevInfo(const DevInfo *info);

#endif /* PROBE_H */
