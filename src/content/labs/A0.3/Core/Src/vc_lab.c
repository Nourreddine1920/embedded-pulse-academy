/**
 * @file    vc_lab.c
 * @brief   A0.3 lab: volatile and const, measured.
 *
 * T0  Build and CPU report (const volatile read of a read-only register)
 * T1  A flag set by an interrupt: plain bool vs volatile bool
 * T2  A busy-wait delay loop: plain vs volatile counter
 * T3  volatile is not atomic: counter++ from main and from an ISR
 * T4  Where const data lives: Flash (.rodata) or SRAM (.data/.bss/stack)
 *
 * The "buggy" variants are deliberately buggy. Their results depend on the
 * optimisation level: that is the lesson. Only the "fixed" variants decide
 * PASS/FAIL.
 */
#include "vc_lab.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* Configuration                                                             */
/* ------------------------------------------------------------------------- */

#define LAB_LINE_LEN        (128U)
#define FLAG_TIMEOUT_MS     (50U)       /**< T1: give the ISR 50 ms.          */
#define SPIN_LOOPS          (100000UL)  /**< T2: iterations of the delay loop. */
#define RACE_LOOPS          (200000UL)  /**< T3: increments done by main().    */
#define TABLE_LEN           (16U)
#define NAME_COUNT          (3U)

#define NOINLINE            __attribute__((noinline))

/* ------------------------------------------------------------------------- */
/* State shared with the ISR                                                 */
/* ------------------------------------------------------------------------- */

static bool              s_flagPlain;       /**< BUG: written by the ISR, not volatile. */
static volatile bool     s_flagVolatile;    /**< Fixed: every read goes to memory.      */
static volatile uint32_t s_isrCount;        /**< Only the ISR writes it.                */
static volatile uint32_t s_sharedVolatile;  /**< ++ from main AND ISR: still racy.      */
static atomic_uint       s_sharedAtomic;    /**< ++ from main AND ISR: LDREX/STREX.     */

void VcLab_TickIsr(void)
{
    s_flagPlain    = true;
    s_flagVolatile = true;
    s_isrCount++;
    s_sharedVolatile++;
    atomic_fetch_add_explicit(&s_sharedAtomic, 1U, memory_order_relaxed);
}

/* ------------------------------------------------------------------------- */
/* Data for T4. Each one lands in a different section.                       */
/* ------------------------------------------------------------------------- */

static const uint32_t k_table[TABLE_LEN] = {          /* .rodata -> Flash       */
    0x00000000UL, 0x1DB71064UL, 0x3B6E20C8UL, 0x26D930ACUL,
    0x76DC4190UL, 0x6B6B51F4UL, 0x4DB26158UL, 0x5005713CUL,
    0xEDB88320UL, 0xF00F9344UL, 0xD6D6A3E8UL, 0xCB61B38CUL,
    0x9B64C2B0UL, 0x86D3D2D4UL, 0xA00AE278UL, 0xBDBDF21CUL
};
static uint32_t s_table[TABLE_LEN] = { 1UL };         /* .data -> SRAM + Flash  */
static uint32_t s_zeroTable[TABLE_LEN];               /* .bss  -> SRAM          */

static const char *s_names[NAME_COUNT]             = { "idle", "run", "fault" };  /* array in .data  */
static const char *const k_names[NAME_COUNT]       = { "idle", "run", "fault" };  /* array in .rodata */

/* ------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* ------------------------------------------------------------------------- */

static const VcLab_Platform *s_pf;

static void lab_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

static void lab_printf(const char *fmt, ...)
{
    char line[LAB_LINE_LEN];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof line, fmt, args);
    va_end(args);
    s_pf->write(line);
}

static uint32_t now_cycles(void)
{
    return *s_pf->cycleCounter;
}

static const char *region(const void *address)
{
    return (s_pf->regionOf != NULL) ? s_pf->regionOf(address) : "?";
}

static const char *verdict(bool ok)
{
    return ok ? "PASS" : "FAIL";
}

/* ------------------------------------------------------------------------- */
/* T0: build and CPU report                                                  */
/* ------------------------------------------------------------------------- */

static void test_build_report(void)
{
#if defined(__OPTIMIZE__)
    const char *opt = "optimised (-O1 or higher)";
#else
    const char *opt = "not optimised (-O0)";
#endif

    lab_printf("[T0] Build: %s\r\n", opt);

    if (s_pf->cpuid != NULL)
    {
        /* A read-only register: const (we may not write) + volatile (it is hardware). */
        const uint32_t id = *s_pf->cpuid;

        lab_printf("     CPUID = 0x%08" PRIX32 " (implementer 0x%02" PRIX32 ", part 0x%03" PRIX32
                   ", r%" PRIu32 "p%" PRIu32 ")\r\n",
                   id, id >> 24, (id >> 4) & 0xFFFU, (id >> 20) & 0xFU, id & 0xFU);
    }
}

/* ------------------------------------------------------------------------- */
/* T1: a flag set by an interrupt                                            */
/* ------------------------------------------------------------------------- */

/** @brief BUG: s_flagPlain is not volatile, so the compiler may read it once. */
static NOINLINE bool wait_flag_plain(uint32_t timeoutCycles)
{
    const uint32_t start = now_cycles();

    while (!s_flagPlain)
    {
        if ((now_cycles() - start) >= timeoutCycles)
        {
            return false;
        }
    }
    return true;
}

/** @brief Fixed: s_flagVolatile is re-read from memory on every iteration. */
static NOINLINE bool wait_flag_volatile(uint32_t timeoutCycles)
{
    const uint32_t start = now_cycles();

    while (!s_flagVolatile)
    {
        if ((now_cycles() - start) >= timeoutCycles)
        {
            return false;
        }
    }
    return true;
}

static bool test_isr_flag(void)
{
    const uint32_t timeout = FLAG_TIMEOUT_MS * s_pf->cyclesPerMs;

    s_flagPlain    = false;
    s_flagVolatile = false;
    s_isrCount     = 0U;

    s_pf->fastTick(true);
    const bool plainSeen    = wait_flag_plain(timeout);
    const bool volatileSeen = wait_flag_volatile(timeout);
    s_pf->fastTick(false);

    lab_printf("[T1] ISR flag    plain=%s volatile=%s (ISR ran %" PRIu32 " times)\r\n",
               plainSeen ? "seen" : "TIMEOUT", volatileSeen ? "seen" : "TIMEOUT", s_isrCount);
    lab_printf("      -> %s\r\n", verdict(volatileSeen));
    return volatileSeen;
}

/* ------------------------------------------------------------------------- */
/* T2: a busy-wait delay loop                                                */
/* ------------------------------------------------------------------------- */

/** @brief BUG: the loop has no observable effect, so -O1+ deletes it. */
static NOINLINE void spin_plain(uint32_t loops)
{
    for (uint32_t i = 0U; i < loops; i++)
    {
    }
}

/** @brief Kept, but each iteration is a load and a store to the stack. */
static NOINLINE void spin_volatile(uint32_t loops)
{
    for (volatile uint32_t i = 0U; i < loops; i++)
    {
    }
}

static bool test_delay_loop(void)
{
    uint32_t start = now_cycles();
    spin_plain(SPIN_LOOPS);
    const uint32_t plainCycles = now_cycles() - start;

    start = now_cycles();
    spin_volatile(SPIN_LOOPS);
    const uint32_t volatileCycles = now_cycles() - start;

    /* The volatile loop must take at least one counter tick per 16 iterations
     * on any machine: it cannot be removed. */
    const bool ok = volatileCycles >= (SPIN_LOOPS / 16UL);

    lab_printf("[T2] delay loop  plain=%" PRIu32 " cycles, volatile=%" PRIu32 " cycles (%lu loops)\r\n",
               plainCycles, volatileCycles, (unsigned long)SPIN_LOOPS);
    lab_printf("      -> %s\r\n", verdict(ok));
    return ok;
}

/* ------------------------------------------------------------------------- */
/* T3: volatile is not atomic                                                */
/* ------------------------------------------------------------------------- */

/** @brief BUG: LDR, ADD, STR. An ISR between the LDR and the STR is lost. */
static NOINLINE void count_volatile(uint32_t loops)
{
    for (uint32_t i = 0U; i < loops; i++)
    {
        s_sharedVolatile++;
    }
}

/** @brief Fixed: LDREX, ADD, STREX, retried if anything intervened. */
static NOINLINE void count_atomic(uint32_t loops)
{
    for (uint32_t i = 0U; i < loops; i++)
    {
        atomic_fetch_add_explicit(&s_sharedAtomic, 1U, memory_order_relaxed);
    }
}

static bool test_not_atomic(void)
{
    /* Phase 1: volatile counter */
    s_isrCount       = 0U;
    s_sharedVolatile = 0U;
    s_pf->fastTick(true);
    uint32_t start = now_cycles();
    count_volatile(RACE_LOOPS);
    const uint32_t volatileCycles = now_cycles() - start;
    s_pf->fastTick(false);
    const uint32_t volatileExpected = RACE_LOOPS + s_isrCount;
    const uint32_t volatileLost     = volatileExpected - s_sharedVolatile;

    /* Phase 2: atomic counter */
    s_isrCount = 0U;
    atomic_store_explicit(&s_sharedAtomic, 0U, memory_order_relaxed);
    s_pf->fastTick(true);
    start = now_cycles();
    count_atomic(RACE_LOOPS);
    const uint32_t atomicCycles = now_cycles() - start;
    s_pf->fastTick(false);
    const uint32_t atomicExpected = RACE_LOOPS + s_isrCount;
    const uint32_t atomicLost     = atomicExpected - (uint32_t)atomic_load(&s_sharedAtomic);

    lab_printf("[T3] volatile++  expected=%" PRIu32 " lost=%" PRIu32 " (%" PRIu32 " cycles)\r\n",
               volatileExpected, volatileLost, volatileCycles);
    lab_printf("     atomic++    expected=%" PRIu32 " lost=%" PRIu32 " (%" PRIu32 " cycles)\r\n",
               atomicExpected, atomicLost, atomicCycles);
    lab_printf("      -> %s\r\n", verdict(atomicLost == 0U));
    return atomicLost == 0U;
}

/* ------------------------------------------------------------------------- */
/* T4: where const data lives                                                */
/* ------------------------------------------------------------------------- */

static void show_object(const char *what, const void *address)
{
    lab_printf("     %-30s %p  %s\r\n", what, address, region(address));
}

static bool is_region(const void *address, const char *name)
{
    return strcmp(region(address), name) == 0;
}

static NOINLINE bool test_const_placement(void)
{
    const uint32_t localConst[4] = { 1UL, 2UL, 3UL, 4UL };   /* a const LOCAL: stack */

    s_zeroTable[0] = s_table[0];                             /* keep both referenced */

    lab_printf("[T4] Where does it live?\r\n");
    show_object("static const uint32_t k_table", k_table);
    show_object("static uint32_t s_table = {1}", s_table);
    show_object("static uint32_t s_zeroTable", s_zeroTable);
    show_object("\"idle\" (string literal)", k_names[0]);
    show_object("const char *s_names[]", (const void *)s_names);
    show_object("const char *const k_names[]", k_names);
    show_object("const uint32_t localConst[]", localConst);
    show_object("function test_const_placement", (const void *)(uintptr_t)&test_const_placement);

    if (s_pf->regionOf == NULL)
    {
        lab_printf("      -> (no memory map on this platform, not checked)\r\n");
        return true;
    }

    const bool ok = is_region(k_table, "Flash") && is_region(k_names, "Flash") &&
                    is_region(k_names[0], "Flash") && is_region((const void *)s_names, "SRAM") &&
                    is_region(localConst, "SRAM");

    lab_printf("      -> %s\r\n", verdict(ok));
    return ok;
}

/* ------------------------------------------------------------------------- */

bool VcLab_Run(const VcLab_Platform *platform)
{
    s_pf = platform;

    lab_printf("\r\n=== A0.3 volatile & const lab ===\r\n");
    test_build_report();

    bool ok = true;
    ok = test_isr_flag() && ok;
    ok = test_delay_loop() && ok;
    ok = test_not_atomic() && ok;
    ok = test_const_placement() && ok;

    lab_printf("=== %s ===\r\n", ok ? "ALL PASS" : "SOME TESTS FAILED");
    return ok;
}
