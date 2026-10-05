/**
 * @file    storage_classes.c
 * @brief   A0.6: the same 1 KB table in .bss, .data or .rodata.
 *
 *   arm-none-eabi-gcc ... -Os -DTABLE_KIND=1 -c storage_classes.c && arm-none-eabi-size storage_classes.o
 *   TABLE_KIND 1: static, zero-initialised   -> .bss    (RAM only, zeroed by startup)
 *   TABLE_KIND 2: static, initialised        -> .data   (RAM + the same bytes in Flash, copied at boot)
 *   TABLE_KIND 3: static const, initialised  -> .rodata (Flash only, counted in "text")
 */
#include <stdint.h>

#ifndef TABLE_KIND
#define TABLE_KIND 1
#endif

#define TABLE_LEN (256U)

#if TABLE_KIND == 1
static uint32_t s_table[TABLE_LEN];
#elif TABLE_KIND == 2
static uint32_t s_table[TABLE_LEN] = { 1U, 2U, 3U };
#else
static const uint32_t s_table[TABLE_LEN] = { 1U, 2U, 3U };
#endif

uint32_t table_read(uint32_t i)
{
    return s_table[i % TABLE_LEN];
}

#if TABLE_KIND != 3
/* Without a write, GCC sees that a static table is never modified and may
 * place it in .rodata even without const. The write keeps it in RAM. */
void table_write(uint32_t i, uint32_t value)
{
    s_table[i % TABLE_LEN] = value;
}
#endif
