/**
 * @file    bits.h
 * @brief   A0.2: portable bit and bit-field helpers for 32-bit registers.
 *
 * Every helper works on a uint32_t *value* and returns the new value, so it
 * can be unit-tested on a PC. Applying it to a hardware register is a
 * separate, explicit step (see reg_modify()), which keeps the one
 * read-modify-write visible in the code.
 *
 * Masks are always unsigned long (UL) so shifting into bit 31 is defined
 * (lesson A0.1).
 */
#ifndef BITS_H
#define BITS_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Single-bit mask: BIT(5) == 0x00000020. n must be 0..31. */
#define BIT(n)                  (1UL << (n))

/**
 * @brief Mask of @p width ones starting at bit @p pos.
 *        FIELD_MASK(2, 10) == 0x00000C00 (the MODER field of pin 5).
 *        The width == 32 case is handled because 1UL << 32 is undefined.
 */
#define FIELD_MASK(width, pos)  ((((width) >= 32U) ? 0xFFFFFFFFUL : ((1UL << (width)) - 1UL)) << (pos))

/* ---- Whole-bit operations (value in, value out) ----------------------- */

static inline uint32_t bits_set(uint32_t value, uint32_t mask)    { return value | mask; }
static inline uint32_t bits_clear(uint32_t value, uint32_t mask)  { return value & ~mask; }
static inline uint32_t bits_toggle(uint32_t value, uint32_t mask) { return value ^ mask; }

/** @brief true if ALL bits of @p mask are 1 in @p value. */
static inline bool bits_all_set(uint32_t value, uint32_t mask)    { return (value & mask) == mask; }

/** @brief true if AT LEAST ONE bit of @p mask is 1 in @p value. */
static inline bool bits_any_set(uint32_t value, uint32_t mask)    { return (value & mask) != 0U; }

/* ---- Multi-bit fields ------------------------------------------------- */

/** @brief Extracts the field at @p mask / @p pos, right-aligned. */
static inline uint32_t field_get(uint32_t value, uint32_t mask, uint32_t pos)
{
    return (value & mask) >> pos;
}

/**
 * @brief Returns @p value with the field replaced by @p field.
 *        Extra high bits in @p field are discarded by the final "& mask",
 *        so a too-large field value can never corrupt a neighbour field.
 */
static inline uint32_t field_set(uint32_t value, uint32_t mask, uint32_t pos, uint32_t field)
{
    return (value & ~mask) | ((field << pos) & mask);
}

/**
 * @brief Extracts a two's complement field of @p width bits (1..31) and
 *        sign-extends it. No implementation-defined shifts or casts.
 */
static inline int32_t field_get_signed(uint32_t value, uint32_t pos, uint32_t width)
{
    const uint32_t raw  = (value >> pos) & ((1UL << width) - 1UL);
    const uint32_t sign = 1UL << (width - 1U);
    return (int32_t)(raw ^ sign) - (int32_t)sign;   /* (raw - 2^(w-1)) in range */
}

/* ---- Bit tricks -------------------------------------------------------- */

/** @brief Number of 1 bits (Kernighan: each pass clears the lowest 1). */
static inline uint32_t bits_count(uint32_t value)
{
    uint32_t count = 0U;
    while (value != 0U)
    {
        value &= value - 1U;
        count++;
    }
    return count;
}

/** @brief Index of the lowest 1 bit, or 32 if @p value is 0. */
static inline uint32_t bits_lowest_index(uint32_t value)
{
    return (value == 0U) ? 32U : (uint32_t)__builtin_ctz(value);  /* RBIT + CLZ on Cortex-M3/M4 */
}

/** @brief Isolates the lowest 1 bit: 0b0110_1000 -> 0b0000_1000. */
static inline uint32_t bits_lowest(uint32_t value)
{
    return value & (0U - value);
}

/** @brief true for 1, 2, 4, 8, ... (exactly one bit set). */
static inline bool bits_is_pow2(uint32_t value)
{
    return (value != 0U) && ((value & (value - 1U)) == 0U);
}

/* ---- Applying to hardware --------------------------------------------- */

/**
 * @brief Read-modify-write of a peripheral register: clear @p clear_mask,
 *        then set @p set_mask, with a single read and a single write.
 * @note  NOT atomic: an interrupt between the read and the write that
 *        changes the same register will have its change overwritten.
 *        Use set/reset registers (BSRR) or a critical section (A6.5).
 */
static inline void reg_modify(volatile uint32_t *reg, uint32_t clear_mask, uint32_t set_mask)
{
    *reg = (*reg & ~clear_mask) | set_mask;
}

#endif /* BITS_H */
