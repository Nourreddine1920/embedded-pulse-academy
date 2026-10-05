/**
 * @file    aapcs.c
 * @brief   A1.2: what the register-use rules of the ARM procedure call standard
 *          (AAPCS) look like in machine code.
 *
 *   arm-none-eabi-gcc -std=c11 -mcpu=cortex-m4 -mthumb -mfloat-abi=hard
 *       -mfpu=fpv4-sp-d16 -O2 -c aapcs.c && arm-none-eabi-objdump -d aapcs.o
 *
 * Read: where each argument arrives, where the result goes, and which
 * registers a function must save before it may use them.
 */
#include <stdint.h>

uint32_t external_work(uint32_t x);          /* a function the compiler cannot see into */

/** Five integer arguments: r0-r3 carry the first four, the fifth is on the stack. */
uint32_t sum5(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
    return a + b + c + d + e;
}

/** A 64-bit result comes back in r1:r0 (r0 = low word). */
uint64_t widen_mul(uint32_t a, uint32_t b)
{
    return (uint64_t)a * b;
}

/** A float argument and result use s0 on the hard-float ABI (-mfloat-abi=hard). */
float half(float x)
{
    return x * 0.5f;
}

/**
 * Needs to keep values alive across a call to external_work(). r0-r3 and r12
 * may be destroyed by the callee, so the values go in r4-r8, which the
 * function must therefore save (push) and restore (pop), together with lr.
 */
uint32_t keeps_across_call(uint32_t a, uint32_t b, uint32_t c)
{
    const uint32_t x = external_work(a);
    const uint32_t y = external_work(b);
    const uint32_t z = external_work(c);

    return x + y + z + a + b + c;
}

/** A leaf function that needs no saved registers: no push, no pop. */
uint32_t leaf(uint32_t a, uint32_t b)
{
    return (a << 3) + b;
}
