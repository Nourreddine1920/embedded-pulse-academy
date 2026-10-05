/**
 * @file    core_compare.c
 * @brief   A1.1: the same C source compiled for different Cortex-M cores.
 *
 * Nothing here runs: compile it with -S (or -c and objdump) once per core and
 * compare the instructions. The compiler may only use what the core has, so the
 * output shows what each core lacks:
 *
 *   for c in cortex-m0 cortex-m0plus cortex-m3 cortex-m4 cortex-m7 cortex-m33; do
 *       arm-none-eabi-gcc -std=c11 -mcpu=$c -mthumb -O2 -S -o $c.s core_compare.c
 *   done
 *
 * Add -mfpu=fpv4-sp-d16 -mfloat-abi=hard for the M4F, and
 * -mfpu=fpv5-d16 -mfloat-abi=hard for an M7 with a double-precision FPU.
 */
#include <stdint.h>

/** 32-bit signed division: SDIV, or a call into libgcc? */
int32_t div_i32(int32_t a, int32_t b)
{
    return a / b;
}

/** 32 x 32 -> 64-bit multiply: UMULL, or a library call? */
uint64_t mul_u64(uint32_t a, uint32_t b)
{
    return (uint64_t)a * b;
}

/** Clamp to int16 range: SSAT on cores that have it. */
int32_t sat16(int32_t x)
{
    if (x > 32767)
    {
        return 32767;
    }
    if (x < -32768)
    {
        return -32768;
    }
    return x;
}

/** Count leading zeros: CLZ, or a library call? */
uint32_t leading_zeros(uint32_t x)
{
    return (x == 0U) ? 32U : (uint32_t)__builtin_clz(x);
}

/** Single-precision add: a VADD.F32 with an FPU, a library call without. */
float add_f32(float a, float b)
{
    return a + b;
}

/** Double-precision add: only an FPv5 double-precision FPU does this in hardware. */
double add_f64(double a, double b)
{
    return a + b;
}
