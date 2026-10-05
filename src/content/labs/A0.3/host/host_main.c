/**
 * @file    host_main.c
 * @brief   A0.3: runs the volatile & const lab on a PC.
 *
 * A second thread plays the hardware: it increments a free-running counter
 * (our "DWT_CYCCNT") and, while the fast tick is enabled, calls
 * VcLab_TickIsr() every TICK_PERIOD counts (our "timer interrupt").
 *
 * Unlike an ISR on a single-core MCU, this thread runs truly in parallel on
 * another core. The results are the same in kind, not in number.
 */
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <time.h>

#include "vc_lab.h"

#define TICK_PERIOD     (4096U)                 /**< Counts between "interrupts". */
#define CALIBRATE_MS    (100L)

static volatile uint32_t s_counter;             /**< The simulated cycle counter. */
static atomic_bool       s_tickEnabled;
static atomic_bool       s_quit;

static void *hardware_thread(void *arg)
{
    (void)arg;
    uint32_t sinceTick = 0U;

    while (!atomic_load(&s_quit))
    {
        s_counter++;
        if (++sinceTick >= TICK_PERIOD)
        {
            sinceTick = 0U;
            if (atomic_load(&s_tickEnabled))
            {
                VcLab_TickIsr();
            }
        }
    }
    return NULL;
}

static void host_write(const char *text)
{
    fputs(text, stdout);
}

static void host_fast_tick(bool enable)
{
    atomic_store(&s_tickEnabled, enable);
}

/** @brief Counter increments per millisecond, measured against clock(). */
static uint32_t calibrate(void)
{
    const clock_t  t0 = clock();
    const uint32_t c0 = s_counter;

    while ((clock() - t0) < (CALIBRATE_MS * CLOCKS_PER_SEC / 1000L))
    {
    }
    return (s_counter - c0) / (uint32_t)CALIBRATE_MS;
}

int main(void)
{
    pthread_t hw;

    if (pthread_create(&hw, NULL, hardware_thread, NULL) != 0)
    {
        return 2;
    }

    const VcLab_Platform platform = {
        .write        = host_write,
        .cycleCounter = &s_counter,
        .cyclesPerMs  = calibrate(),
        .fastTick     = host_fast_tick,
        .regionOf     = NULL,            /* a PC has no fixed Flash/SRAM map */
        .cpuid        = NULL,
    };

    const bool ok = VcLab_Run(&platform);

    atomic_store(&s_quit, true);
    (void)pthread_join(hw, NULL);
    return ok ? 0 : 1;
}
