/**
 * @file    config_checks.h
 * @brief   A0.6 exercise 2: build-time checks for a UART logger configuration.
 *
 * Include this header once, from the file that uses the configuration.
 * Every check below turns a field bug into a compile error.
 */
#ifndef CONFIG_CHECKS_H
#define CONFIG_CHECKS_H

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/* ---- The configuration under test ---------------------------------------- */
#define LOG_RING_SIZE      (256U)           /* bytes, indices wrap with & (size - 1) */
#define LOG_LINE_MAX       (80U)
#define LOG_APB1_HZ        (16000000UL)
#define LOG_BAUD           (115200UL)
#define LOG_FLUSH_MS       (20U)

typedef enum { LOG_LEVEL_ERROR = 0, LOG_LEVEL_WARN, LOG_LEVEL_INFO, LOG_LEVEL_COUNT } LogLevel;

typedef struct
{
    uint8_t  level;          /* LogLevel, stored in one byte */
    uint8_t  length;         /* 0..LOG_LINE_MAX              */
    uint16_t sequence;
    uint32_t timestamp_ms;
} LogRecordHeader;

extern const char *const g_logLevelNames[];   /* defined in the logger's .c file */
#define LOG_LEVEL_NAMES_LEN  (3U)             /* number of entries in that table */

/* 1. Ring size: a power of two, and small enough for a uint16_t index. */
static_assert((LOG_RING_SIZE != 0U) && ((LOG_RING_SIZE & (LOG_RING_SIZE - 1U)) == 0U),
              "LOG_RING_SIZE must be a non-zero power of two");
static_assert(LOG_RING_SIZE <= 65536U, "LOG_RING_SIZE must fit a uint16_t index");

/* 2. One line must fit in the ring, and its length in the uint8_t length field. */
static_assert(LOG_LINE_MAX < LOG_RING_SIZE, "a log line must fit in the ring");
static_assert(LOG_LINE_MAX <= UINT8_MAX, "LogRecordHeader.length is a uint8_t");

/* 3. The baud-rate divisor fits USART_BRR (16 bits) and is not below 16. */
#define LOG_BRR  ((LOG_APB1_HZ + (LOG_BAUD / 2UL)) / LOG_BAUD)
static_assert((LOG_BRR >= 16UL) && (LOG_BRR <= 0xFFFFUL), "USART_BRR out of range");

/* 4. The ring can absorb LOG_FLUSH_MS of output at full baud (10 bits per byte). */
static_assert(((LOG_BAUD / 10UL) * LOG_FLUSH_MS / 1000UL) <= LOG_RING_SIZE,
              "ring too small for LOG_FLUSH_MS of traffic");

/* 5. Wire layout of the record header (A0.5). */
static_assert(sizeof(LogRecordHeader) == 8U, "LogRecordHeader must be 8 bytes");
static_assert(offsetof(LogRecordHeader, timestamp_ms) == 4U, "timestamp at byte 4");

/* 6. The level fits its one-byte field, and the name table matches the enum. */
static_assert((unsigned)LOG_LEVEL_COUNT <= (unsigned)UINT8_MAX + 1U, "LogLevel must fit in uint8_t");
static_assert(LOG_LEVEL_NAMES_LEN == (unsigned)LOG_LEVEL_COUNT, "one name per LogLevel");

/* 7. Platform assumptions the logger relies on. */
static_assert(CHAR_BIT == 8, "8-bit bytes");
static_assert(sizeof(uint32_t) == 4U, "uint32_t is 4 bytes");

#endif /* CONFIG_CHECKS_H */
