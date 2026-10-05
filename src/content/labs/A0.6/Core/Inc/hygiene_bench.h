/**
 * @file    hygiene_bench.h
 * @brief   A0.6: runs the legacy macros and their fixes side by side.
 *
 * Hardware-independent: prints through console.h, runs on the board and on a PC.
 */
#ifndef HYGIENE_BENCH_H
#define HYGIENE_BENCH_H

#include <stdint.h>

/**
 * @brief Runs every demo and prints the results.
 * @return Number of FIXED versions that gave a wrong result (0 expected).
 *         The legacy versions are expected to be wrong and are not counted.
 */
uint32_t HygieneBench_Run(void);

#endif /* HYGIENE_BENCH_H */
