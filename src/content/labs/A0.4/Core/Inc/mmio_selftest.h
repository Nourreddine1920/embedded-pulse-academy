/**
 * @file    mmio_selftest.h
 * @brief   A0.4: tests led_drv and devinfo against fake registers
 *          (runs on the PC and on the STM32, touches no hardware).
 */
#ifndef MMIO_SELFTEST_H
#define MMIO_SELFTEST_H

#include <stdint.h>

/**
 * @brief  Runs every check and prints failures through Probe_Printf().
 * @retval Number of failed checks (0 = all passed).
 */
uint32_t MmioSelfTest_Run(void);

#endif /* MMIO_SELFTEST_H */
