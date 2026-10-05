/**
 * @file    float_fix.c
 * @brief   A1.1 exercise 3: two accidental double-precision operations on a
 *          Cortex-M4F, and their fixes.
 *
 * Build for the STM32F446 and read the disassembly (arm-none-eabi-objdump -d):
 *   arm-none-eabi-gcc -std=c11 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
 *       -mthumb -O2 -Wall -Wextra -Wconversion -Wdouble-promotion -c float_fix.c
 *
 * The F446's FPU is single precision. A double operand sends the whole
 * expression to libgcc (__aeabi_f2d, __aeabi_dmul, __aeabi_d2f, sqrt).
 *
 * Without the pragma below, GCC 10.3 reports both BAD functions:
 *   -Wdouble-promotion: implicit conversion from 'float' to 'double' ...
 *   -Wfloat-conversion: conversion from 'double' to 'float' may change value
 * The pragma only exists so this file builds with 0 warnings. Never silence
 * these in real code: they are the warnings that find the bug.
 */
#include <math.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wdouble-promotion"

/** BAD: 0.1 is a double constant, so y is converted up, multiplied in software, converted back. */
float scale_bad(float y)
{
    return y * 0.1;
}

/** BAD: sqrt() is the double-precision function: f2d, call sqrt, d2f. */
float root_bad(float y)
{
    return sqrt(y);
}

#pragma GCC diagnostic pop

/** GOOD: 0.1f is a float constant, so the multiply is one VMUL.F32. */
float scale_good(float y)
{
    return y * 0.1f;
}

/** GOOD: sqrtf() is the single-precision function: one VSQRT.F32 (plus the errno path). */
float root_good(float y)
{
    return sqrtf(y);
}
