/**
 * @file    console.h
 * @brief   A0.6: minimal printf-style console through an injected write function.
 *
 * Hardware-independent: the board passes a UART writer, the PC passes fputs.
 */
#ifndef CONSOLE_H
#define CONSOLE_H

/** @brief Writes a NUL-terminated string to the console. */
typedef void (*Console_WriteFn)(const char *text);

/** @brief Selects the output used by Console_Printf(). */
void Console_Init(Console_WriteFn write);

/** @brief printf-style output, at most CONSOLE_LINE_LEN - 1 characters per call. */
void Console_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif /* CONSOLE_H */
