/**
 * @file    layout_report.c
 * @brief   A0.5: layout report (sizeof / offsetof / padding / bit-fields /
 *          wire-frame decoding). Same code on the board and on a PC.
 */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "layout_report.h"
#include "regmap.h"
#include "sensor_frame.h"

#define REPORT_LINE_MAX   (128U)

static Report_WriteFn s_write;

void Report_Init(Report_WriteFn write)
{
    s_write = write;
}

void Report_Printf(const char *fmt, ...)
{
    char    line[REPORT_LINE_MAX];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    if (s_write != NULL)
    {
        s_write(line);
    }
}

/* ------------------------------------------------------------------------- */
/* Register maps                                                             */
/* ------------------------------------------------------------------------- */

typedef struct
{
    const char *name;
    uint32_t    offset;
    uint32_t    size;
} member_row_t;

/** One table row from the type itself: nothing is typed in twice. */
#define ROW(type, member) \
    { #member, (uint32_t)offsetof(type, member), (uint32_t)sizeof(((type *)0)->member) }

#define HOLE(type, member, index) \
    { "(hole)", (uint32_t)(offsetof(type, member) + ((index) * sizeof(uint32_t))), 4U }

static void print_rows(const char *title, uint32_t type_size, uint32_t base,
                       const member_row_t *rows, size_t count)
{
    Report_Printf("\r\n--- %s: sizeof = %lu (0x%02lX) ---\r\n", title,
                  (unsigned long)type_size, (unsigned long)type_size);
    Report_Printf("member      offset  size  address\r\n");
    for (size_t i = 0U; i < count; i++)
    {
        Report_Printf("%-10s  0x%02lX    %lu     0x%08lX\r\n", rows[i].name,
                      (unsigned long)rows[i].offset, (unsigned long)rows[i].size,
                      (unsigned long)(base + rows[i].offset));
    }
}

void Report_RegisterMaps(void)
{
    static const member_row_t gpio[] = {
        ROW(my_gpio_t, MODER), ROW(my_gpio_t, OTYPER), ROW(my_gpio_t, OSPEEDR),
        ROW(my_gpio_t, PUPDR), ROW(my_gpio_t, IDR),    ROW(my_gpio_t, ODR),
        ROW(my_gpio_t, BSRR),  ROW(my_gpio_t, LCKR),   ROW(my_gpio_t, AFR[0]),
        ROW(my_gpio_t, AFR[1]),
    };
    static const member_row_t usart[] = {
        ROW(my_usart_t, SR),  ROW(my_usart_t, DR),  ROW(my_usart_t, BRR),
        ROW(my_usart_t, CR1), ROW(my_usart_t, CR2), ROW(my_usart_t, CR3),
        ROW(my_usart_t, GTPR),
    };
    /* RCC up to APB2ENR: enough to see the first four holes. */
    static const member_row_t rcc[] = {
        ROW(my_rcc_t, AHB2RSTR), ROW(my_rcc_t, AHB3RSTR), HOLE(my_rcc_t, RESERVED0, 0U),
        ROW(my_rcc_t, APB1RSTR), ROW(my_rcc_t, APB2RSTR), HOLE(my_rcc_t, RESERVED1, 0U),
        HOLE(my_rcc_t, RESERVED1, 1U), ROW(my_rcc_t, AHB1ENR), ROW(my_rcc_t, AHB2ENR),
        ROW(my_rcc_t, AHB3ENR), HOLE(my_rcc_t, RESERVED2, 0U), ROW(my_rcc_t, APB1ENR),
        ROW(my_rcc_t, APB2ENR),
    };

    print_rows("my_gpio_t @ GPIOA", (uint32_t)sizeof(my_gpio_t), MY_GPIOA_BASE,
               gpio, sizeof gpio / sizeof gpio[0]);
    print_rows("my_usart_t @ USART2", (uint32_t)sizeof(my_usart_t), MY_USART2_BASE,
               usart, sizeof usart / sizeof usart[0]);
    print_rows("my_rcc_t @ RCC (0x14..0x44)", (uint32_t)sizeof(my_rcc_t), MY_RCC_BASE,
               rcc, sizeof rcc / sizeof rcc[0]);
    Report_Printf("\r\nGPIO ports are 0x%03lX apart, my_gpio_t is 0x%02lX bytes: the rest of each slot is reserved\r\n",
                  (unsigned long)((MY_GPIOC_BASE - MY_GPIOA_BASE) / 2UL),
                  (unsigned long)sizeof(my_gpio_t));
}

/* ------------------------------------------------------------------------- */
/* Padding                                                                   */
/* ------------------------------------------------------------------------- */

struct order_abc { uint8_t a; uint32_t b; uint16_t c; };
struct order_bca { uint32_t b; uint16_t c; uint8_t a; };
struct __attribute__((packed)) packed_abc { uint8_t a; uint32_t b; uint16_t c; };
struct with_u64  { uint8_t a; uint64_t b; };

/** The wire frame's members WITHOUT packed: what a naive driver writes. */
typedef struct
{
    uint8_t  sync;
    uint8_t  seq;
    int16_t  temp_cdeg;
    uint16_t humidity_cpct;
    uint32_t timestamp_ms;    /* the compiler moves this to offset 8 */
    uint8_t  status;
    uint8_t  checksum;
} naive_frame_t;

#define PAD_ROW(label, type, total_members)                                           \
    Report_Printf("%-31s %6lu  %2lu  %2lu  %2lu  %7lu\r\n", label,                     \
                  (unsigned long)sizeof(type), (unsigned long)offsetof(type, a),      \
                  (unsigned long)offsetof(type, b), (unsigned long)offsetof(type, c), \
                  (unsigned long)(sizeof(type) - (total_members)))

void Report_Padding(void)
{
    Report_Printf("\r\n--- Padding: same members, different layouts ---\r\n");
    Report_Printf("%-31s %6s  %2s  %2s  %2s  %7s\r\n", "struct", "sizeof", "a", "b", "c", "padding");
    PAD_ROW("{ u8 a; u32 b; u16 c; }", struct order_abc, 7U);
    PAD_ROW("{ u32 b; u16 c; u8 a; }", struct order_bca, 7U);
    PAD_ROW("packed { u8 a; u32 b; u16 c; }", struct packed_abc, 7U);
    Report_Printf("%-31s %6lu  %2lu  %2lu  %2s  %7lu\r\n", "{ u8 a; u64 b; }",
                  (unsigned long)sizeof(struct with_u64), (unsigned long)offsetof(struct with_u64, a),
                  (unsigned long)offsetof(struct with_u64, b), "-",
                  (unsigned long)(sizeof(struct with_u64) - 9U));
    Report_Printf("_Alignof: u16 %lu, u32 %lu, u64 %lu, double %lu, packed_abc %lu\r\n",
                  (unsigned long)_Alignof(uint16_t), (unsigned long)_Alignof(uint32_t),
                  (unsigned long)_Alignof(uint64_t), (unsigned long)_Alignof(double),
                  (unsigned long)_Alignof(struct packed_abc));
    Report_Printf("sensor frame: naive struct %lu bytes (timestamp @ %lu), packed %lu bytes (timestamp @ %lu)\r\n",
                  (unsigned long)sizeof(naive_frame_t), (unsigned long)offsetof(naive_frame_t, timestamp_ms),
                  (unsigned long)sizeof(sensor_frame_wire_t),
                  (unsigned long)offsetof(sensor_frame_wire_t, timestamp_ms));
}

/* ------------------------------------------------------------------------- */
/* Bit-fields                                                                */
/* ------------------------------------------------------------------------- */

/** USART_CR1 (RM0390) as bit-fields. For comparison only: CMSIS uses masks. */
typedef struct
{
    uint32_t sbk    : 1;   /* bit 0  */
    uint32_t rwu    : 1;
    uint32_t re     : 1;
    uint32_t te     : 1;   /* bit 3  */
    uint32_t idleie : 1;
    uint32_t rxneie : 1;
    uint32_t tcie   : 1;
    uint32_t txeie  : 1;
    uint32_t peie   : 1;
    uint32_t ps     : 1;
    uint32_t pce    : 1;
    uint32_t wake   : 1;
    uint32_t m      : 1;
    uint32_t ue     : 1;   /* bit 13 */
    uint32_t        : 1;   /* bit 14 reserved */
    uint32_t over8  : 1;   /* bit 15 */
    uint32_t        : 16;  /* bits 31:16 reserved */
} usart_cr1_bits_t;

typedef union
{
    uint32_t         w;
    usart_cr1_bits_t b;
} usart_cr1_t;

_Static_assert(sizeof(usart_cr1_t) == 4U, "CR1 view must be one word");

#define CR1_TE   (1UL << 3)    /* = CMSIS USART_CR1_TE */
#define CR1_UE   (1UL << 13)   /* = CMSIS USART_CR1_UE */

/** Mixed declared types: layout rules differ between compilers/ABIs. */
struct mixed_types { uint8_t a : 4; uint16_t b : 8; };
enum two_values { VALUE_A, VALUE_B };

void Report_Bitfields(void)
{
    usart_cr1_t cr1 = { .w = 0U };

    cr1.b.te = 1U;
    cr1.b.ue = 1U;

    Report_Printf("\r\n--- Bit-fields: USART_CR1 through a union ---\r\n");
    Report_Printf("cr1.b.te = 1; cr1.b.ue = 1;  ->  cr1.w = 0x%08lX\r\n", (unsigned long)cr1.w);
    Report_Printf("CR1_TE | CR1_UE (masks)      ->          0x%08lX  %s\r\n",
                  (unsigned long)(CR1_TE | CR1_UE),
                  (cr1.w == (CR1_TE | CR1_UE)) ? "(same on this compiler)" : "(DIFFERENT!)");
    Report_Printf("byte 0 of cr1 in memory = 0x%02X, byte 1 = 0x%02X\r\n",
                  (unsigned)((const uint8_t *)&cr1)[0], (unsigned)((const uint8_t *)&cr1)[1]);

    Report_Printf("\r\n--- Implementation-defined sizes on this compiler ---\r\n");
    Report_Printf("sizeof(struct { uint8_t a:4; uint16_t b:8; }) = %lu\r\n",
                  (unsigned long)sizeof(struct mixed_types));
    Report_Printf("sizeof(enum { VALUE_A, VALUE_B })             = %lu\r\n",
                  (unsigned long)sizeof(enum two_values));
}

/* ------------------------------------------------------------------------- */
/* Frames                                                                    */
/* ------------------------------------------------------------------------- */

static bool same_reading(const sensor_reading_t *x, const sensor_reading_t *y)
{
    return (x->seq == y->seq) && (x->temp_cdeg == y->temp_cdeg) &&
           (x->humidity_cpct == y->humidity_cpct) &&
           (x->timestamp_ms == y->timestamp_ms) && (x->status == y->status);
}

static void print_reading(const char *label, bool ok, const sensor_reading_t *r)
{
    if (!ok)
    {
        Report_Printf("%-8s  rejected (sync/checksum)\r\n", label);
        return;
    }
    Report_Printf("%-8s %4u %10d %9u   0x%08lX    0x%02X  id=%u\r\n", label,
                  (unsigned)r->seq, (int)r->temp_cdeg, (unsigned)r->humidity_cpct,
                  (unsigned long)r->timestamp_ms, (unsigned)r->status,
                  (unsigned)((r->status & SENSOR_STATUS_ID_MSK) >> SENSOR_STATUS_ID_POS));
}

uint32_t Report_Frames(void)
{
    /* 8-aligned buffer; the frame starts at rx + 1, as it would after a
     * 1-byte header in a UART ring buffer. So the timestamp sits at rx + 7. */
    _Alignas(8) uint8_t rx[1U + sizeof(naive_frame_t)];
    uint8_t *const      frame = &rx[1];
    const sensor_reading_t sent = {
        .seq = 7U, .temp_cdeg = -1234, .humidity_cpct = 4567U,
        .timestamp_ms = 0x12345678UL,
        .status = (uint8_t)(SENSOR_STATUS_VALID | (3U << SENSOR_STATUS_ID_POS)),
    };
    sensor_reading_t byBytes = {0}, byMemcpy = {0}, byUnion = {0};
    uint32_t         failures = 0U;

    memset(rx, 0xEE, sizeof rx);
    SensorFrame_Encode(&sent, frame);

    Report_Printf("\r\n--- Sensor frame at rx+1 (timestamp at rx+7: not 4-byte aligned) ---\r\n");
    Report_Printf("bytes:");
    for (uint32_t i = 0U; i < SENSOR_FRAME_SIZE; i++)
    {
        Report_Printf(" %02X", (unsigned)frame[i]);
    }
    Report_Printf("\r\n%-8s %4s %10s %9s %13s %7s\r\n", "decoder", "seq", "temp_cdeg", "hum_cpct",
                  "timestamp_ms", "status");

    const bool okB = SensorFrame_DecodeBytes(frame, &byBytes);
    const bool okM = SensorFrame_DecodeMemcpy(frame, &byMemcpy);
    const bool okU = SensorFrame_DecodeUnion(frame, &byUnion);
    print_reading("bytes", okB, &byBytes);
    print_reading("memcpy", okM, &byMemcpy);
    print_reading("union", okU, &byUnion);

    const bool agree = okB && okM && okU && same_reading(&byBytes, &sent) &&
                       same_reading(&byMemcpy, &sent) && same_reading(&byUnion, &sent);
    Report_Printf("all three decoders match the sent values: %s\r\n", agree ? "yes" : "NO");
    failures += agree ? 0U : 1U;

    /* The naive (padded) struct reads the timestamp from wire bytes 8..11. */
    naive_frame_t naive;
    memcpy(&naive, frame, sizeof naive);
    Report_Printf("naive padded struct: timestamp_ms = 0x%08lX (expected 0x%08lX), status = 0x%02X\r\n",
                  (unsigned long)naive.timestamp_ms, (unsigned long)sent.timestamp_ms,
                  (unsigned)naive.status);
    failures += (naive.timestamp_ms != sent.timestamp_ms) ? 0U : 1U;   /* must be wrong */

    /* One flipped bit in the humidity must be caught by the checksum. */
    frame[4] ^= 0x01U;
    const bool corruptAccepted = SensorFrame_DecodeBytes(frame, &byBytes);
    Report_Printf("flip bit 0 of byte 4 -> checksum %s\r\n", corruptAccepted ? "MISSED it" : "rejects the frame");
    failures += corruptAccepted ? 1U : 0U;

    return failures;
}
