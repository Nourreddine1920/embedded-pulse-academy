/**
 * @file    host_main.c
 * @brief   A0.4: runs the register-probe code on a PC.
 *
 * Address arithmetic and the drivers are tested against fake registers.
 * Nothing here dereferences a real STM32 address: on a PC that would be a
 * segmentation fault, and it is the one thing a host build can never test.
 */
#include <stdio.h>

#include "devinfo.h"
#include "mmio_selftest.h"
#include "probe.h"

static void host_write(const char *text)
{
    fputs(text, stdout);
}

int main(void)
{
    Probe_Init(host_write);

    Probe_Printf("=== A0.4 Register probe (host) ===\r\n");
    const uint32_t failures = MmioSelfTest_Run();

    Probe_Printf("\r\n--- Address map ---\r\n");
    Probe_AddressTable();

    Probe_Printf("\r\n--- Pointer arithmetic ---\r\n");
    Probe_PointerArithmetic();

    /* The same DevInfo_Read() the board uses, pointed at fake memory. */
    const uint32_t cpuid  = 0x410FC241UL;
    const uint32_t idcode = 0x10006421UL;
    const uint32_t uid[3] = { 0x00400024UL, 0x3437510DUL, 0x31383932UL };
    const uint16_t flash  = 512U;
    const DevInfo_Sources fakeSources = { .cpuid = &cpuid, .idcode = &idcode, .uid = uid, .flashKiB = &flash };
    DevInfo info;

    DevInfo_Read(&fakeSources, &info);
    Probe_Printf("\r\n--- Device identification (simulated values) ---\r\n");
    Probe_PrintDevInfo(&info);

    return (failures == 0U) ? 0 : 1;
}
