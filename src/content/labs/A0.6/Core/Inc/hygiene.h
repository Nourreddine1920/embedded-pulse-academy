/**
 * @file    hygiene.h
 * @brief   A0.6 "AFTER": the same helpers as legacy_macros.h, done properly.
 *
 *  - static inline functions instead of function-like macros:
 *    arguments evaluated exactly once, typed parameters, normal precedence,
 *    and at -O1 and above the same machine code as the macro.
 *  - do { } while (0) for the rare macro that must stay a macro.
 *  - static_assert on every build-time assumption (configuration values,
 *    wire-format layouts). A false assumption stops the build instead of
 *    shipping.
 *
 * Header discipline: include guard, declarations and static inline
 * definitions only. No object is DEFINED here (see event_counter.h for extern).
 */
#ifndef HYGIENE_H
#define HYGIENE_H

#include <assert.h>    /* C11: static_assert is a macro for _Static_assert */
#include <stddef.h>    /* offsetof */
#include <stdint.h>

/* ---- 1. static inline instead of function-like macros ------------------ */

static inline uint32_t square_u32(uint32_t x)             { return x * x; }
static inline uint32_t double_u32(uint32_t x)             { return x + x; }
static inline uint32_t max_u32(uint32_t a, uint32_t b)    { return (a > b) ? a : b; }
static inline int32_t  max_i32(int32_t a, int32_t b)      { return (a > b) ? a : b; }

/* ---- 2. Multi-statement code -------------------------------------------- */

/** @brief Board hooks (the bench counts calls; a board would drive a pin). */
void led_on(void);
void led_off(void);

/** Preferred: a function. It is a single statement wherever it is called. */
static inline void led_pulse(void)
{
    led_on();
    led_off();
}

/** If it must be a macro: do { } while (0) makes it one statement that
 *  needs its trailing semicolon, so it works under if/else without braces. */
#define LED_PULSE_SAFE()   do { led_on(); led_off(); } while (0)

/* ---- 3. Configuration checked at build time ----------------------------- */

#define LOG_BUFFER_SIZE    (64U)
static_assert((LOG_BUFFER_SIZE & (LOG_BUFFER_SIZE - 1U)) == 0U,
              "LOG_BUFFER_SIZE must be a power of two (indices are masked, A8.4)");

#define APB1_CLOCK_HZ      (16000000UL)     /**< HSI, APB1 /1 (A0.1 clock setup) */
#define CONSOLE_BAUD       (115200UL)
/** USART_BRR for 16x oversampling, rounded to nearest: 16 MHz / 115200 = 138.9 -> 139. */
#define USART_BRR_VALUE    ((APB1_CLOCK_HZ + (CONSOLE_BAUD / 2UL)) / CONSOLE_BAUD)
static_assert((USART_BRR_VALUE >= 16UL) && (USART_BRR_VALUE <= 0xFFFFUL),
              "USART_BRR out of range: check APB1_CLOCK_HZ and CONSOLE_BAUD");

/* ---- 4. Layouts checked at build time (A0.5) ----------------------------- */

/** Header of a frame sent over UART. Its layout IS the protocol. */
typedef struct
{
    uint8_t  id;          /* byte 0     */
    uint8_t  length;      /* byte 1     */
    uint16_t crc;         /* bytes 2..3 */
    uint32_t timestamp;   /* bytes 4..7 */
} FrameHeader;

static_assert(sizeof(FrameHeader) == 8U, "FrameHeader must be 8 bytes on the wire");
static_assert(offsetof(FrameHeader, crc) == 2U, "FrameHeader.crc must be at byte 2");
static_assert(offsetof(FrameHeader, timestamp) == 4U, "FrameHeader.timestamp must be at byte 4");

#endif /* HYGIENE_H */
