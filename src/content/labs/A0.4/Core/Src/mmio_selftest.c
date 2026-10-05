/**
 * @file    mmio_selftest.c
 * @brief   A0.4: unit tests with a fake register block.
 *
 * A GPIO port is ten consecutive 32-bit registers. An array of ten
 * uint32_t has exactly the same layout, so the driver cannot tell the
 * difference: it only ever computes base + offset and dereferences.
 */
#include "mmio_selftest.h"

#include <stdbool.h>
#include <string.h>

#include "devinfo.h"
#include "led_drv.h"
#include "mmio.h"
#include "probe.h"

/* Word indices into the fake port (byte offset / 4). */
#define W_MODER    (GPIO_MODER_OFS / 4U)
#define W_OTYPER   (GPIO_OTYPER_OFS / 4U)
#define W_ODR      (GPIO_ODR_OFS / 4U)
#define W_BSRR     (GPIO_BSRR_OFS / 4U)

#define MODER_RESET_GPIOA   (0xA8000000UL)   /**< RM0390: PA13/14/15 in AF (SWD) */

static uint32_t s_checks;
static uint32_t s_failures;

static void check(bool ok, const char *expr, int line)
{
    s_checks++;
    if (!ok)
    {
        s_failures++;
        Probe_Printf("  FAIL (line %d): %s\r\n", line, expr);
    }
}

#define CHECK(expr)  check((expr), #expr, __LINE__)

static void test_pointer_arithmetic(void)
{
    uint32_t                 block[GPIO_BLOCK_WORDS];
    volatile uint32_t *const base = block;

    /* +1 on a uint32_t pointer is +4 bytes; reg_at() takes RM byte offsets. */
    CHECK(reg_at(base, GPIO_ODR_OFS) == &base[5]);
    CHECK((uintptr_t)reg_at(base, GPIO_BSRR_OFS) - (uintptr_t)base == 0x18U);
    CHECK((uintptr_t)(base + 1) - (uintptr_t)base == sizeof(uint32_t));
}

static void test_led_driver(void)
{
    uint32_t fake[GPIO_BLOCK_WORDS];
    LedDrv   led;

    (void)memset(fake, 0, sizeof fake);
    fake[W_MODER]  = MODER_RESET_GPIOA | 0x000000A0UL;   /* + PA2/PA3 in AF (UART) */
    fake[W_OTYPER] = 0x00000020UL;                       /* pretend PA5 was open-drain */

    LedDrv_Init(&led, fake, 5U);
    CHECK(fake[W_MODER] == 0xA80004A0UL);                /* only bits 11:10 changed */
    CHECK(fake[W_OTYPER] == 0x00000000UL);               /* push-pull */

    LedDrv_On(&led);
    CHECK(fake[W_BSRR] == 0x00000020UL);                 /* BS5 */
    LedDrv_Off(&led);
    CHECK(fake[W_BSRR] == 0x00200000UL);                 /* BR5 = bit 21 */

    /* A fake has no hardware behind it: ODR does not follow BSRR. The test
     * sets ODR itself to play the part of the GPIO peripheral. */
    fake[W_ODR] = 0x00000020UL;
    CHECK(LedDrv_IsOn(&led));
    LedDrv_Toggle(&led);
    CHECK(fake[W_BSRR] == 0x00200000UL);                 /* was on  -> reset */

    fake[W_ODR] = 0x00000000UL;
    CHECK(!LedDrv_IsOn(&led));
    LedDrv_Toggle(&led);
    CHECK(fake[W_BSRR] == 0x00000020UL);                 /* was off -> set */
}

static void test_devinfo(void)
{
    /* Values an STM32F446RE revision A reports (UID is per chip: made up here). */
    const uint32_t fakeCpuid     = 0x410FC241UL;
    const uint32_t fakeIdcode    = 0x10006421UL;
    const uint32_t fakeUid[3]    = { 0x00400024UL, 0x3437510DUL, 0x31383932UL };
    const uint16_t fakeFlashSize = 512U;

    const DevInfo_Sources src = {
        .cpuid = &fakeCpuid, .idcode = &fakeIdcode, .uid = fakeUid, .flashKiB = &fakeFlashSize,
    };
    DevInfo info;

    DevInfo_Read(&src, &info);
    CHECK(info.devId == 0x421U);
    CHECK(info.revId == 0x1000U);
    CHECK(info.partNo == 0xC24U);
    CHECK((info.variant == 0U) && (info.revision == 1U));
    CHECK(info.uid[1] == 0x3437510DUL);
    CHECK(info.flashKiB == 512U);
    CHECK(strcmp(DevInfo_DeviceName(info.devId), "STM32F446xx") == 0);
    CHECK(strcmp(DevInfo_CoreName(info.partNo), "Cortex-M4") == 0);
}

uint32_t MmioSelfTest_Run(void)
{
    s_checks   = 0U;
    s_failures = 0U;

    test_pointer_arithmetic();
    test_led_driver();
    test_devinfo();

    Probe_Printf("mmio self-test: %lu/%lu checks passed\r\n",
                 (unsigned long)(s_checks - s_failures), (unsigned long)s_checks);
    return s_failures;
}
