/**
 * @file    spsc_ring.h
 * @brief   A0.3 exercise 🟡: lock-free single-producer / single-consumer byte
 *          ring, e.g. a UART RX ISR (producer) feeding main() (consumer).
 *
 * Rules that make it safe without disabling interrupts:
 *  - exactly ONE context writes head (the ISR), exactly ONE writes tail (main);
 *  - head and tail are 32-bit and aligned, so each load/store is single-copy
 *    atomic on Cortex-M (ARMv7-M ARM, A3.5.3);
 *  - the data byte is written BEFORE head is published (release), and read
 *    AFTER head is observed (acquire). GCC 10.3 implements acquire/release on
 *    Cortex-M4 with a DMB next to the load/store (verified). On a single-core
 *    M4 without a cache only the compiler ordering is strictly needed, but the
 *    DMB costs a few cycles and keeps the code correct on multi-core parts.
 */
#ifndef SPSC_RING_H
#define SPSC_RING_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#define SPSC_RING_SIZE   (64U)                              /**< Power of two. */
#define SPSC_RING_MASK   (SPSC_RING_SIZE - 1U)

_Static_assert((SPSC_RING_SIZE & SPSC_RING_MASK) == 0U, "size must be a power of two");

typedef struct
{
    uint8_t     data[SPSC_RING_SIZE];
    atomic_uint head;       /**< Next slot to write. Written only by the producer. */
    atomic_uint tail;       /**< Next slot to read.  Written only by the consumer. */
} SpscRing;

/** @brief Producer side (ISR). Returns false and drops the byte if full. */
static inline bool spsc_put(SpscRing *r, uint8_t byte)
{
    const unsigned head = atomic_load_explicit(&r->head, memory_order_relaxed);   /* our own */
    const unsigned tail = atomic_load_explicit(&r->tail, memory_order_acquire);

    if ((head - tail) >= SPSC_RING_SIZE)
    {
        return false;                                       /* full */
    }
    r->data[head & SPSC_RING_MASK] = byte;                  /* 1. write the data ... */
    atomic_store_explicit(&r->head, head + 1U, memory_order_release);   /* 2. ... then publish */
    return true;
}

/** @brief Consumer side (main). Returns false if empty. */
static inline bool spsc_get(SpscRing *r, uint8_t *byte)
{
    const unsigned tail = atomic_load_explicit(&r->tail, memory_order_relaxed);   /* our own */
    const unsigned head = atomic_load_explicit(&r->head, memory_order_acquire);

    if (head == tail)
    {
        return false;                                       /* empty */
    }
    *byte = r->data[tail & SPSC_RING_MASK];                 /* 1. read the data ...  */
    atomic_store_explicit(&r->tail, tail + 1U, memory_order_release);   /* 2. ... then free it */
    return true;
}

#endif /* SPSC_RING_H */
