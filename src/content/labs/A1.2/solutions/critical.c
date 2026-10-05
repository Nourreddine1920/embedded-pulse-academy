/**
 * @file    critical.c
 * @brief   A1.2 exercises 2 and 3: nested critical sections, on a SIMULATED core.
 *
 * CoreModel stands for the two registers a critical section touches. On the
 * STM32 the same code uses __get_PRIMASK()/__disable_irq()/__set_PRIMASK() and
 * __set_BASEPRI()/__set_BASEPRI_MAX(). Build the host test with
 *     gcc -std=c11 -Wall -Wextra -DCRITICAL_TEST critical.c -o critical
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
    bool    primask;    /**< PRIMASK: 1 = interrupts blocked          */
    uint8_t basepri;    /**< BASEPRI: 0 = off, else blocks >= value   */
} CoreModel;

/* ---- Exercise 2: PRIMASK, naive and correct ------------------------------- */

/** BAD: assumes interrupts were enabled on entry and forces them on at exit. */
void Naive_Enter(CoreModel *m)
{
    m->primask = true;                 /* __disable_irq() */
}

void Naive_Exit(CoreModel *m)
{
    m->primask = false;                /* __enable_irq(): wrong if we were already inside a critical section */
}

/** GOOD: remember what PRIMASK was, and put exactly that back. */
uint32_t Saved_Enter(CoreModel *m)
{
    const uint32_t saved = m->primask ? 1U : 0U;   /* __get_PRIMASK() */

    m->primask = true;                              /* __disable_irq() */
    return saved;
}

void Saved_Exit(CoreModel *m, uint32_t saved)
{
    m->primask = (saved != 0U);                     /* __set_PRIMASK(saved) */
}

/* ---- Exercise 3: BASEPRI, and why it needs the _MAX form ------------------- */

/** @brief MSR BASEPRI_MAX: writes only if the value is more restrictive (non-zero and lower), or BASEPRI is 0. */
static void basepri_max(CoreModel *m, uint8_t value)
{
    if ((value != 0U) && ((m->basepri == 0U) || (value < m->basepri)))
    {
        m->basepri = value;
    }
}

/** GOOD: raises the mask to @p value, never lowers it, and returns the old value. */
uint8_t Basepri_Enter(CoreModel *m, uint8_t value)
{
    const uint8_t saved = m->basepri;     /* __get_BASEPRI() */

    basepri_max(m, value);                /* __set_BASEPRI_MAX(value) */
    return saved;
}

void Basepri_Exit(CoreModel *m, uint8_t saved)
{
    m->basepri = saved;                   /* __set_BASEPRI(saved) */
}

/** BAD: plain write on entry (can LOWER the mask) and a clear on exit. */
void BasepriNaive_Enter(CoreModel *m, uint8_t value)
{
    m->basepri = value;                   /* __set_BASEPRI(value) */
}

void BasepriNaive_Exit(CoreModel *m)
{
    m->basepri = 0U;                      /* __set_BASEPRI(0) */
}

#ifdef CRITICAL_TEST
int main(void)
{
    CoreModel m = { false, 0U };
    unsigned  failed = 0U;

    /* Exercise 2: outer section, inner section, then check the mask is still on. */
    Naive_Enter(&m);                       /* outer */
    Naive_Enter(&m);                       /* inner */
    Naive_Exit(&m);                        /* inner exit */
    printf("naive PRIMASK nesting : interrupts blocked after inner exit = %s  (want yes)\n", m.primask ? "yes" : "NO");
    failed += m.primask ? 0U : 1U;         /* the naive version is EXPECTED to fail: counted below */
    const unsigned naiveFailed = failed;
    failed = 0U;

    m.primask = false;
    const uint32_t outer = Saved_Enter(&m);
    const uint32_t inner = Saved_Enter(&m);

    Saved_Exit(&m, inner);
    printf("saved PRIMASK nesting : interrupts blocked after inner exit = %s  (want yes)\n", m.primask ? "yes" : "NO");
    failed += m.primask ? 0U : 1U;
    Saved_Exit(&m, outer);
    printf("saved PRIMASK nesting : interrupts blocked after outer exit = %s  (want no)\n", m.primask ? "YES" : "no");
    failed += m.primask ? 1U : 0U;

    /* Exercise 3: outer level 5 (0x50), inner level 2 (0x20), then back. */
    m = (CoreModel){ false, 0U };
    const uint8_t o = Basepri_Enter(&m, 0x50U);
    const uint8_t i = Basepri_Enter(&m, 0x20U);

    printf("BASEPRI_MAX nesting   : outer 0x50, inner 0x20 -> 0x%02X (want 0x20)\n", (unsigned)m.basepri);
    failed += (m.basepri == 0x20U) ? 0U : 1U;
    Basepri_Exit(&m, i);
    printf("BASEPRI_MAX nesting   : after inner exit        -> 0x%02X (want 0x50)\n", (unsigned)m.basepri);
    failed += (m.basepri == 0x50U) ? 0U : 1U;
    (void)Basepri_Enter(&m, 0x80U);        /* a LESS restrictive request must not lower the mask */
    printf("BASEPRI_MAX nesting   : inner asks for 0x80     -> 0x%02X (want 0x50, not lowered)\n", (unsigned)m.basepri);
    failed += (m.basepri == 0x50U) ? 0U : 1U;
    Basepri_Exit(&m, o);

    m = (CoreModel){ false, 0U };
    BasepriNaive_Enter(&m, 0x20U);         /* outer, very restrictive */
    BasepriNaive_Enter(&m, 0x80U);         /* inner, less restrictive: lowers the mask */
    printf("naive BASEPRI         : outer 0x20, inner 0x80  -> 0x%02X (outer protection lost)\n", (unsigned)m.basepri);
    BasepriNaive_Exit(&m);
    printf("naive BASEPRI         : after inner exit        -> 0x%02X (outer section now unprotected)\n", (unsigned)m.basepri);

    printf("correct versions failed: %u, naive PRIMASK version failed: %u (expected 1)\n", failed, naiveFailed);
    return (failed == 0U && naiveFailed == 1U) ? 0 : 1;
}
#endif
