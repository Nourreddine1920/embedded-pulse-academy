/**
 * @file    xray.c
 * @brief   A0.2: register X-ray, binary dumps with change markers.
 *
 * Output format (one line per value, nibbles separated by spaces):
 *
 *   GPIOA->MODER  before 0xA8000000  1010 1000 0000 0000 0000 0000 0000 0000
 *                 after  0xA8000400  1010 1000 0000 0000 0000 0100 0000 0000
 *                                                              ^
 *                 changed bits: 10
 */
#include "xray.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define XRAY_LINE_LEN      (128U)
#define XRAY_BITS          (32U)
#define XRAY_NIBBLE        (4U)
#define XRAY_NAME_WIDTH    "14"          /**< printf width of the name column */
/** Characters before the first binary digit: name(14)+" "+tag(6)+" "+hex(10)+"  ". */
#define XRAY_BIN_COLUMN    (14U + 1U + 6U + 1U + 10U + 2U)
/** Binary text: 32 digits + 7 separators + NUL. */
#define XRAY_BIN_LEN       (XRAY_BITS + (XRAY_BITS / XRAY_NIBBLE) - 1U + 1U)

static Xray_WriteFn s_write;

void Xray_Init(Xray_WriteFn write)
{
    s_write = write;
}

void Xray_Printf(const char *fmt, ...)
{
    char line[XRAY_LINE_LEN];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    s_write(line);
}

/**
 * @brief Renders bits 31..0 of @p value into @p out, or, if @p mark is true,
 *        renders '^' where @p value (used as a change mask) has a 1.
 */
static void render_bits(char out[XRAY_BIN_LEN], uint32_t value, bool mark)
{
    uint32_t pos = 0U;

    for (uint32_t i = 0U; i < XRAY_BITS; i++)
    {
        const uint32_t bitIndex = (XRAY_BITS - 1U) - i;
        const bool     isOne    = ((value >> bitIndex) & 1UL) != 0U;

        if ((i != 0U) && ((i % XRAY_NIBBLE) == 0U))
        {
            out[pos++] = ' ';
        }
        out[pos++] = mark ? (isOne ? '^' : ' ') : (isOne ? '1' : '0');
    }
    out[pos] = '\0';
}

void Xray_Ruler(void)
{
    /* Bit numbers above the first digit of each byte: 31, 23, 15, 7. */
    Xray_Printf("%*s31        23        15        7       0\r\n", (int)XRAY_BIN_COLUMN, "");
}

void Xray_Show(const char *name, uint32_t value)
{
    char bin[XRAY_BIN_LEN];

    render_bits(bin, value, false);
    Xray_Printf("%-" XRAY_NAME_WIDTH "s %-6s 0x%08" PRIX32 "  %s\r\n", name, "", value, bin);
}

void Xray_Diff(const char *name, uint32_t before, uint32_t after)
{
    char           bin[XRAY_BIN_LEN];
    const uint32_t changed = before ^ after;   /* XOR: 1 exactly where bits differ */

    render_bits(bin, before, false);
    Xray_Printf("%-" XRAY_NAME_WIDTH "s %-6s 0x%08" PRIX32 "  %s\r\n", name, "before", before, bin);
    render_bits(bin, after, false);
    Xray_Printf("%-" XRAY_NAME_WIDTH "s %-6s 0x%08" PRIX32 "  %s\r\n", "", "after", after, bin);

    if (changed == 0U)
    {
        Xray_Printf("%*s(no change)\r\n", (int)XRAY_BIN_COLUMN, "");
        return;
    }

    render_bits(bin, changed, true);
    Xray_Printf("%*s%s\r\n", (int)XRAY_BIN_COLUMN, "", bin);

    Xray_Printf("%*schanged bits:", (int)XRAY_BIN_COLUMN, "");
    for (uint32_t rest = changed; rest != 0U; rest &= rest - 1U)   /* visit each 1 bit */
    {
        Xray_Printf(" %u", (unsigned)__builtin_ctz(rest));
    }
    Xray_Printf("\r\n");
}
