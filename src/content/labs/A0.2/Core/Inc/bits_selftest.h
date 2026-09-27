/**
 * @file    bits_selftest.h
 * @brief   A0.2: self-test of bits.h (runs on the PC and on the STM32).
 */
#ifndef BITS_SELFTEST_H
#define BITS_SELFTEST_H

#include <stdint.h>

/**
 * @brief  Runs every check and prints failures through Xray_Printf().
 * @retval Number of failed checks (0 = all passed).
 */
uint32_t BitsSelfTest_Run(void);

#endif /* BITS_SELFTEST_H */
