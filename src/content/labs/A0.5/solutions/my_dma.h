/**
 * @file  my_dma.h
 * @brief Exercise A0.5 🔴: the DMA controller as ONE struct with a nested
 *        array of 8 stream structs (RM0390 "DMA register map").
 *
 * CMSIS splits this into DMA_TypeDef (4 flag registers) and eight separate
 * DMA_Stream_TypeDef pointers (DMA2_Stream7 = DMA2_BASE + 0xB8). Nesting
 * gives the same addresses and lets you index streams: MY_DMA2->S[n].CR.
 */
#ifndef MY_DMA_H
#define MY_DMA_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    volatile uint32_t CR;     /**< +0x00 configuration (EN, CHSEL, DIR, ...) */
    volatile uint32_t NDTR;   /**< +0x04 number of data items                */
    volatile uint32_t PAR;    /**< +0x08 peripheral address                  */
    volatile uint32_t M0AR;   /**< +0x0C memory 0 address                    */
    volatile uint32_t M1AR;   /**< +0x10 memory 1 address (double buffer)    */
    volatile uint32_t FCR;    /**< +0x14 FIFO control                        */
} my_dma_stream_t;

typedef struct
{
    volatile const uint32_t LISR;   /**< 0x00 flags, streams 0..3 (r)            */
    volatile const uint32_t HISR;   /**< 0x04 flags, streams 4..7 (r)            */
    volatile uint32_t       LIFCR;  /**< 0x08 flag clear, streams 0..3 (w, 1 clears) */
    volatile uint32_t       HIFCR;  /**< 0x0C flag clear, streams 4..7           */
    my_dma_stream_t         S[8];   /**< 0x10 + 0x18 * n                         */
} my_dma_t;

_Static_assert(sizeof(my_dma_stream_t)   == 0x18U, "stream stride is 0x18");
_Static_assert(offsetof(my_dma_t, S)     == 0x10U, "stream 0 at 0x10");
_Static_assert(offsetof(my_dma_t, S[7])  == 0xB8U, "stream 7 at 0xB8");
_Static_assert(offsetof(my_dma_t, S[7].M0AR) == 0xC4U, "stream 7 M0AR");
_Static_assert(sizeof(my_dma_t)          == 0xD0U, "DMA block size");

#define MY_DMA1_BASE   (0x40026000UL)
#define MY_DMA2_BASE   (0x40026400UL)
#define MY_DMA2        ((my_dma_t *)MY_DMA2_BASE)

_Static_assert(MY_DMA2_BASE + offsetof(my_dma_t, S[7].CR) == 0x400264B8UL, "DMA2_Stream7->CR");

/*
 * Flags are NOT at a regular stride: each stream owns 6 bits (FEIF, -, DMEIF,
 * TEIF, HTIF, TCIF) at positions 0, 6, 16, 22 of LISR (streams 0-3) and of
 * HISR (streams 4-7). A bit-field struct cannot express "same layout, other
 * register, irregular gaps" cleanly; a shift table can.
 */
#define MY_DMA_FLAG_FEIF   (1UL << 0)
#define MY_DMA_FLAG_DMEIF  (1UL << 2)
#define MY_DMA_FLAG_TEIF   (1UL << 3)
#define MY_DMA_FLAG_HTIF   (1UL << 4)
#define MY_DMA_FLAG_TCIF   (1UL << 5)
#define MY_DMA_FLAGS_ALL   (0x3DUL)

static inline uint32_t my_dma_flag_shift(uint32_t stream)
{
    static const uint8_t shift[4] = { 0U, 6U, 16U, 22U };
    return shift[stream & 3U];
}

/** @brief The 6 flag bits of @p stream, right-aligned (one read). */
static inline uint32_t my_dma_flags(const my_dma_t *dma, uint32_t stream)
{
    const uint32_t isr = (stream < 4U) ? dma->LISR : dma->HISR;
    return (isr >> my_dma_flag_shift(stream)) & MY_DMA_FLAGS_ALL;
}

/** @brief Clears @p flags of @p stream: one plain write, 1 clears, 0 ignored. */
static inline void my_dma_clear(my_dma_t *dma, uint32_t stream, uint32_t flags)
{
    const uint32_t value = (flags & MY_DMA_FLAGS_ALL) << my_dma_flag_shift(stream);

    if (stream < 4U)
    {
        dma->LIFCR = value;
    }
    else
    {
        dma->HIFCR = value;
    }
}

#endif /* MY_DMA_H */
