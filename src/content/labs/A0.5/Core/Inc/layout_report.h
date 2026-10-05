/**
 * @file    layout_report.h
 * @brief   A0.5: prints sizeof/offsetof tables, padding, bit-field and
 *          frame-decoding results, so you can compare compilers and targets.
 *
 * Hardware-independent: output goes through an injected write function,
 * as in the A0.2 X-ray.
 */
#ifndef LAYOUT_REPORT_H
#define LAYOUT_REPORT_H

#include <stdint.h>

/** @brief Writes a NUL-terminated string to the console. */
typedef void (*Report_WriteFn)(const char *text);

/** @brief Selects the console used by every Report_* function. */
void Report_Init(Report_WriteFn write);

/** @brief printf-style helper through the same console. */
void Report_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/** @brief Offsets, sizes and absolute addresses of my_gpio_t / my_usart_t / my_rcc_t. */
void Report_RegisterMaps(void);

/** @brief The same members in three orders, packed, and with a 64-bit member. */
void Report_Padding(void);

/** @brief USART CR1 as a bit-field union vs masks; implementation-defined sizes. */
void Report_Bitfields(void);

/**
 * @brief Decodes a frame stored at an odd address three ways, then shows
 *        what an unpacked struct and a corrupted byte do.
 * @return number of failed checks (0 = all good).
 */
uint32_t Report_Frames(void);

#endif /* LAYOUT_REPORT_H */
