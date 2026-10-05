/**
 * @file    sensor_frame.h
 * @brief   A0.5: a 12-byte sensor frame on the wire, and three ways to
 *          decode it (byte shifts, memcpy into a packed struct, union).
 *
 * Wire format (little-endian, no padding, as sent by the sensor):
 *
 *   offset  size  field
 *   0       1     sync       0xA5
 *   1       1     seq        frame counter
 *   2       2     temp       int16, 0.01 degC
 *   4       2     humidity   uint16, 0.01 %RH
 *   6       4     timestamp  uint32, ms          <- NOT 4-byte aligned
 *   10      1     status     bit 0 VALID, bit 1 HEATER, bits 7:4 sensor id
 *   11      1     checksum   XOR of bytes 0..10
 */
#ifndef SENSOR_FRAME_H
#define SENSOR_FRAME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SENSOR_FRAME_SIZE        (12U)
#define SENSOR_FRAME_SYNC        (0xA5U)

#define SENSOR_STATUS_VALID      (0x01U)
#define SENSOR_STATUS_HEATER     (0x02U)
#define SENSOR_STATUS_ID_POS     (4U)
#define SENSOR_STATUS_ID_MSK     (0xFU << SENSOR_STATUS_ID_POS)

/** @brief Decoded reading: a normal (padded, aligned) struct for the app. */
typedef struct
{
    uint8_t  seq;
    int16_t  temp_cdeg;       /**< -1234 = -12.34 degC  */
    uint16_t humidity_cpct;   /**< 4567  = 45.67 %RH    */
    uint32_t timestamp_ms;
    uint8_t  status;
} sensor_reading_t;

/** @brief The frame exactly as on the wire. packed = no padding, align 1. */
typedef struct __attribute__((packed))
{
    uint8_t  sync;
    uint8_t  seq;
    int16_t  temp_cdeg;
    uint16_t humidity_cpct;
    uint32_t timestamp_ms;
    uint8_t  status;
    uint8_t  checksum;
} sensor_frame_wire_t;

_Static_assert(sizeof(sensor_frame_wire_t) == SENSOR_FRAME_SIZE, "wire frame must be 12 bytes");
_Static_assert(offsetof(sensor_frame_wire_t, timestamp_ms) == 6U, "timestamp at wire offset 6");

/** @brief The same 12 bytes seen as raw bytes or as fields (C11 type punning). */
typedef union
{
    uint8_t             raw[SENSOR_FRAME_SIZE];
    sensor_frame_wire_t f;
} sensor_frame_u;

/** @brief XOR of @p len bytes. */
uint8_t SensorFrame_Checksum(const uint8_t *buf, size_t len);

/** @brief Serialises @p in into buf[0..11] (portable, byte by byte). */
void SensorFrame_Encode(const sensor_reading_t *in, uint8_t buf[SENSOR_FRAME_SIZE]);

/**
 * @brief Portable decoder: assembles every field from bytes with shifts.
 *        Works on any CPU, any endianness, any buffer alignment.
 * @return false if sync or checksum is wrong.
 */
bool SensorFrame_DecodeBytes(const uint8_t buf[SENSOR_FRAME_SIZE], sensor_reading_t *out);

/** @brief memcpy into the packed struct. Correct only on little-endian CPUs. */
bool SensorFrame_DecodeMemcpy(const uint8_t buf[SENSOR_FRAME_SIZE], sensor_reading_t *out);

/** @brief Copy into a union, read the struct member. Little-endian only. */
bool SensorFrame_DecodeUnion(const uint8_t buf[SENSOR_FRAME_SIZE], sensor_reading_t *out);

#endif /* SENSOR_FRAME_H */
