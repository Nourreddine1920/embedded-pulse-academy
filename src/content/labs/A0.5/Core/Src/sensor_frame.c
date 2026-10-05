/**
 * @file    sensor_frame.c
 * @brief   A0.5: encode/decode the 12-byte sensor frame three ways.
 */
#include <string.h>
#include "sensor_frame.h"

/* Byte offsets on the wire (see the table in sensor_frame.h). */
#define OFF_SYNC      (0U)
#define OFF_SEQ       (1U)
#define OFF_TEMP      (2U)
#define OFF_HUM       (4U)
#define OFF_TS        (6U)
#define OFF_STATUS    (10U)
#define OFF_CHECKSUM  (11U)

uint8_t SensorFrame_Checksum(const uint8_t *buf, size_t len)
{
    uint8_t x = 0U;

    for (size_t i = 0U; i < len; i++)
    {
        x ^= buf[i];
    }
    return x;
}

static void put_u16le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xFFU);
    p[1] = (uint8_t)(v >> 8);
}

static void put_u32le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFFU);
    p[1] = (uint8_t)((v >> 8) & 0xFFU);
    p[2] = (uint8_t)((v >> 16) & 0xFFU);
    p[3] = (uint8_t)(v >> 24);
}

static uint16_t get_u16le(const uint8_t *p)
{
    return (uint16_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8));
}

static uint32_t get_u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/** 16-bit two's complement -> int16_t without implementation-defined casts (A0.2). */
static int16_t s16_from_u16(uint16_t raw)
{
    return (int16_t)((int32_t)(raw ^ 0x8000U) - 0x8000);
}

void SensorFrame_Encode(const sensor_reading_t *in, uint8_t buf[SENSOR_FRAME_SIZE])
{
    buf[OFF_SYNC] = SENSOR_FRAME_SYNC;
    buf[OFF_SEQ]  = in->seq;
    put_u16le(&buf[OFF_TEMP], (uint16_t)in->temp_cdeg);   /* modulo 2^16: defined */
    put_u16le(&buf[OFF_HUM], in->humidity_cpct);
    put_u32le(&buf[OFF_TS], in->timestamp_ms);
    buf[OFF_STATUS]   = in->status;
    buf[OFF_CHECKSUM] = SensorFrame_Checksum(buf, OFF_CHECKSUM);
}

static bool frame_ok(const uint8_t buf[SENSOR_FRAME_SIZE])
{
    return (buf[OFF_SYNC] == SENSOR_FRAME_SYNC) &&
           (SensorFrame_Checksum(buf, OFF_CHECKSUM) == buf[OFF_CHECKSUM]);
}

bool SensorFrame_DecodeBytes(const uint8_t buf[SENSOR_FRAME_SIZE], sensor_reading_t *out)
{
    if (!frame_ok(buf))
    {
        return false;
    }
    out->seq           = buf[OFF_SEQ];
    out->temp_cdeg     = s16_from_u16(get_u16le(&buf[OFF_TEMP]));
    out->humidity_cpct = get_u16le(&buf[OFF_HUM]);
    out->timestamp_ms  = get_u32le(&buf[OFF_TS]);
    out->status        = buf[OFF_STATUS];
    return true;
}

/* Copies the packed struct field by field into the aligned app struct.
 * Reading w->timestamp_ms is fine: the compiler knows it is packed and emits
 * an access that tolerates misalignment. Taking &w->timestamp_ms is not. */
static void from_wire(const sensor_frame_wire_t *w, sensor_reading_t *out)
{
    out->seq           = w->seq;
    out->temp_cdeg     = w->temp_cdeg;
    out->humidity_cpct = w->humidity_cpct;
    out->timestamp_ms  = w->timestamp_ms;
    out->status        = w->status;
}

bool SensorFrame_DecodeMemcpy(const uint8_t buf[SENSOR_FRAME_SIZE], sensor_reading_t *out)
{
    sensor_frame_wire_t w;

    if (!frame_ok(buf))
    {
        return false;
    }
    memcpy(&w, buf, sizeof w);          /* the only defined way to reinterpret bytes */
    from_wire(&w, out);
    return true;
}

bool SensorFrame_DecodeUnion(const uint8_t buf[SENSOR_FRAME_SIZE], sensor_reading_t *out)
{
    sensor_frame_u u;

    if (!frame_ok(buf))
    {
        return false;
    }
    /* In a driver the UART ISR writes u.raw[i] directly as bytes arrive;
     * here we copy from buf to keep the three decoders comparable.            */
    memcpy(u.raw, buf, SENSOR_FRAME_SIZE);   /* write one member ...            */
    from_wire(&u.f, out);                    /* ... read another: defined in C11 */
    return true;
}
