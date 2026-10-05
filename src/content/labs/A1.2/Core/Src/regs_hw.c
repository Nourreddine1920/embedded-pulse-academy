/**
 * @file    regs_hw.c
 * @brief   A1.2: real core registers and the masking experiment (STM32F446, CMSIS).
 */
#include "regs_hw.h"

#include "stm32f4xx.h"
#include "regs_report.h"

#define LAB_IRQ           EXTI0_IRQn          /* IRQ6 = exception number 22, free on the Nucleo */
#define LAB_PRIO_BITS     (__NVIC_PRIO_BITS)  /* 4 on the STM32F446 */

static volatile uint32_t s_handlerRuns;
static volatile uint32_t s_handlerIpsr;

/** Runs when EXTI0 is pended by software (STIR / NVIC_SetPendingIRQ). The EXTI itself is not used. */
void EXTI0_IRQHandler(void)
{
    s_handlerIpsr = __get_IPSR();     /* the exception number: 16 + IRQ number = 22 */
    s_handlerRuns++;
}

void RegsHw_Snapshot(RegsSnapshot *out)
{
    out->msp       = __get_MSP();
    out->psp       = __get_PSP();
    out->control   = __get_CONTROL();
    out->primask   = __get_PRIMASK();
    out->basepri   = __get_BASEPRI();
    out->faultmask = __get_FAULTMASK();
    out->xpsr      = __get_xPSR();
    out->ipsr      = __get_IPSR();
}

void RegsHw_IrqExperiment(const MaskState *mask, uint8_t level, IrqExperiment *out)
{
    const uint8_t prioByte = Priority_Encode(level, (uint8_t)LAB_PRIO_BITS);
    const uint32_t before  = s_handlerRuns;

    out->predictedRun = !Irq_IsMasked(mask, (uint8_t)LAB_PRIO_BITS, prioByte);

    NVIC_SetPriority(LAB_IRQ, level);          /* CMSIS shifts the level into the top bits */
    NVIC_ClearPendingIRQ(LAB_IRQ);
    NVIC_EnableIRQ(LAB_IRQ);

    /* Apply the masks. BASEPRI is written as the left-aligned priority byte. */
    __set_BASEPRI(mask->basepri);
    out->basepriReadBack = __get_BASEPRI();
    if (mask->primask)
    {
        __disable_irq();
    }
    if (mask->faultmask)
    {
        __disable_fault_irq();
    }

    NVIC_SetPendingIRQ(LAB_IRQ);
    __DSB();
    __ISB();
    out->ranWhileMasked = (s_handlerRuns != before);

    /* Remove the masks, then give the pending interrupt a chance to run. */
    if (mask->faultmask)
    {
        __enable_fault_irq();
    }
    if (mask->primask)
    {
        __enable_irq();
    }
    __set_BASEPRI(0U);
    __DSB();
    __ISB();
    out->ranAfterUnmask = (s_handlerRuns != before);
    out->handlerIpsr    = s_handlerIpsr;

    NVIC_DisableIRQ(LAB_IRQ);
    NVIC_ClearPendingIRQ(LAB_IRQ);
}

uint32_t RegsHw_PrintReport(void)
{
    static const struct
    {
        const char *name;
        MaskState   mask;
        uint8_t     level;
    } cases[] = {
        { "none           ", { false, false, 0x00U },  5U },
        { "BASEPRI = 0x50 ", { false, false, 0x50U },  4U },
        { "BASEPRI = 0x50 ", { false, false, 0x50U },  5U },
        { "BASEPRI = 0x50 ", { false, false, 0x50U },  9U },
        { "BASEPRI = 0x55 ", { false, false, 0x55U },  5U },
        { "BASEPRI = 0xF0 ", { false, false, 0xF0U }, 14U },
        { "BASEPRI = 0xF0 ", { false, false, 0xF0U }, 15U },
        { "PRIMASK = 1    ", { true, false, 0x00U },   0U },
        { "FAULTMASK = 1  ", { false, true, 0x00U },   0U },
    };
    RegsSnapshot snap;
    uint32_t     mismatches = 0U;

    RegsHw_Snapshot(&snap);
    Regs_Printf("\r\n--- Special registers in thread mode (main) ---\r\n");
    Regs_Printf("MSP = 0x%08lX  PSP = 0x%08lX\r\n", (unsigned long)snap.msp, (unsigned long)snap.psp);
    Regs_PrintControl(snap.control);
    Regs_Printf("PRIMASK = %lu  BASEPRI = 0x%02lX  FAULTMASK = %lu\r\n", (unsigned long)snap.primask,
                (unsigned long)snap.basepri, (unsigned long)snap.faultmask);
    Regs_PrintXpsr("thread:", snap.xpsr);

    Regs_Printf("\r\n--- Masking experiment: EXTI0 (IRQ6) pended in software ---\r\n");
    Regs_Printf("mask             level  model   ran while masked  ran after unmask  read-back  match\r\n");
    for (unsigned i = 0U; i < (sizeof cases / sizeof cases[0]); i++)
    {
        IrqExperiment e;

        RegsHw_IrqExperiment(&cases[i].mask, cases[i].level, &e);
        const bool match = (e.predictedRun == e.ranWhileMasked) && e.ranAfterUnmask;

        mismatches += match ? 0U : 1U;
        Regs_Printf("%s %5u  %-7s %-16s  %-16s  0x%02lX       %s\r\n", cases[i].name, (unsigned)cases[i].level,
                    e.predictedRun ? "runs" : "blocked", e.ranWhileMasked ? "yes" : "no",
                    e.ranAfterUnmask ? "yes" : "no", (unsigned long)e.basepriReadBack, match ? "yes" : "NO");
    }

    IrqExperiment last;

    RegsHw_IrqExperiment(&cases[0].mask, cases[0].level, &last);
    Regs_Printf("IPSR inside the handler = %lu (16 + IRQ6 = 22)\r\n", (unsigned long)last.handlerIpsr);
    return mismatches;
}
