/**
 * @file    host_main.c
 * @brief   A0.5: runs the layout report on a PC. Compare its output with the
 *          board's: the register maps and frames must match; the
 *          "implementation-defined" lines may not.
 */
#include <stdio.h>

#include "layout_report.h"

static void host_write(const char *text)
{
    fputs(text, stdout);
}

int main(void)
{
    Report_Init(host_write);
    Report_Printf("=== A0.5 Register maps & layouts (host) ===\r\n");

    Report_RegisterMaps();
    Report_Padding();
    Report_Bitfields();
    const uint32_t failures = Report_Frames();

    Report_Printf("\r\nframe checks failed: %lu\r\n", (unsigned long)failures);
    return (failures == 0U) ? 0 : 1;
}
