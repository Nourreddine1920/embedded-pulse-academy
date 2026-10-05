import React from "react";
import BitRow, { BitRuler, hex, toBits } from "./BitRow";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* All instruction sequences below are real arm-none-eabi-gcc 10.3 -O2 output. */
const MODER_BEFORE = 0xa80000a0;
const MODER_AFTER = 0xa80004a0;

const steps = [
  { label: "Looks like one assignment", caption: "GPIOA's MODER described as sixteen 2-bit bit-fields. 'MODER_BF->m5 = 1;' reads like a single write to two bits. The hardware has no 2-bit accesses, so what does the compiler emit?" },
  { label: "LDR: read the whole word", caption: "Because the bit-field is declared volatile uint32_t, GCC (AAPCS rule, -fstrict-volatile-bitfields) reads the whole 32-bit register into r2." },
  { label: "BFI: insert into the copy", caption: "BFI r2, r1, #10, #2 replaces bits 11:10 of the copy with 01. The other 30 bits are whatever was read a moment ago." },
  { label: "STR: write the whole word back", caption: "The result is exactly MODIFY_REG: LDR, modify, STR. It's a read-modify-write hidden behind '=', with the same race window as A0.2's 'ODR |='. Two bit-field lines are two separate RMWs." },
  { label: "On an rc_w1 register: disaster", caption: "EXTI_PR as bit-fields: 'PR_BF->pr13 = 1;' compiles to LDRH, ORR #0x2000, STRH (a 16-bit read-modify-write). Lines 13 and 0 were pending; the read returns 0x2001 and writing 0x2001 back clears BOTH. The line-0 interrupt is lost." },
  { label: "Mask: one plain store", caption: "EXTI->PR = EXTI_PR_PR13; is MOV + STR: no read at all. Only bit 13 is written as 1, so only line 13 clears. This is why CMSIS never describes registers with bit-fields." },
  { label: "The access width follows the type", caption: "Declare the field as 'volatile uint8_t en:1' and GCC emits LDRB/STRB: an 8-bit bus access. GPIO accepts that, but RM0390 says USART registers take only 16- or 32-bit accesses. The declared type silently picks the bus width." },
  { label: "Bit order is implementation-defined", caption: "C11 §6.7.2.1: the order of bit-fields within a unit is implementation-defined. GCC on little-endian ARM allocates from bit 0 up, so 'ue' (the 14th field) lands on bit 13. A big-endian ABI allocates from the top, and the same declaration puts 'ue' on bit 18. Masks mean the same thing everywhere." },
];

const PR_TONE = (i, b, idx) => (idx === 13 ? "focus" : idx === 0 && b ? "promoted" : b ? "base" : "dim");

const BitfieldRmw = () => (
  <Stepper title="Animation · What a bit-field assignment really does" steps={steps}>
    {(step) => {
      if (step <= 3) {
        const asm = ["", "ldr   r2, [r3, #0]", "bfi   r2, r1, #10, #2", "str   r2, [r3, #0]"][step];
        const r2 = step === 1 ? MODER_BEFORE : MODER_AFTER;
        return (
          <div className="space-y-2">
            <CodeLine parts={[["MODER_BF->m5 = 1U;", step === 0], [asm ? `   →   ${asm}` : "", true]]} />
            <BitRuler />
            <BitRow bits={toBits(step === 3 ? MODER_AFTER : MODER_BEFORE, 32)} label="GPIOA->MODER" caption={hex(step === 3 ? MODER_AFTER : MODER_BEFORE, 32)} idPrefix="m" tone={(i, b, idx) => (idx === 10 || idx === 11 ? (step === 3 ? "good" : "focus") : b ? "base" : "zero")} />
            {step >= 1 && <BitRow bits={toBits(r2, 32)} label="r2 (copy)" caption={hex(r2, 32)} idPrefix="r" tone={(i, b, idx) => (idx === 10 || idx === 11 ? (step >= 2 ? "good" : "focus") : step >= 1 ? (b ? "promoted" : "zero") : "dim")} />}
            {step === 3 && <Verdict ok={false}>3 instructions, not atomic: a hidden read-modify-write</Verdict>}
          </div>
        );
      }
      if (step <= 5) {
        const bad = step === 4;
        return (
          <div className="space-y-2">
            <CodeLine parts={[[bad ? "PR_BF->pr13 = 1U;   →   ldrh / orr #0x2000 / strh" : "EXTI->PR = EXTI_PR_PR13;   →   mov #0x2000 / str", true]]} />
            <BitRow bits={toBits(0x2001, 16)} maxWidth={16} label="PR before" caption="0x2001" idPrefix="pb" tone={PR_TONE} />
            <BitRow bits={toBits(bad ? 0x2001 : 0x2000, 16)} maxWidth={16} label="value written" caption={bad ? "0x2001" : "0x2000"} idPrefix="pw" tone={(i, b, idx) => (idx === 0 && b ? "bad" : idx === 13 ? "good" : "dim")} />
            <BitRow bits={toBits(bad ? 0x0000 : 0x0001, 16)} maxWidth={16} label="PR after" caption={bad ? "0x0000" : "0x0001"} idPrefix="pa" tone={(i, b, idx) => (idx === 0 ? (b ? "good" : "bad") : "dim")} />
            {bad ? <Verdict ok={false}>Line 0 cleared without being handled</Verdict> : <Verdict ok>Only line 13 cleared</Verdict>}
          </div>
        );
      }
      if (step === 6) {
        return (
          <div className="space-y-3">
            <CodeLine parts={[["typedef struct { volatile uint8_t en:1; volatile uint8_t mode:3; } ctl_t;", true]]} />
            <div className="grid max-w-xl grid-cols-2 gap-3 font-mono text-xs">
              <div className="rounded-lg border border-slate-700 bg-slate-900 p-3 text-slate-300">
                <div className="mb-1 text-[10px] uppercase tracking-wider text-slate-500">declared uint32_t</div>
                ldr &nbsp;r2, [r3]<br />bfi &nbsp;r2, r1, #10, #2<br />str &nbsp;r2, [r3]
                <div className="mt-2 text-emerald-300">32-bit bus access</div>
              </div>
              <div className="rounded-lg border border-rose-400/50 bg-rose-500/10 p-3 text-slate-300">
                <div className="mb-1 text-[10px] uppercase tracking-wider text-slate-500">declared uint8_t</div>
                ldrb r3, [r2]<br />orr.w r3, r3, #1<br />strb r3, [r2]
                <div className="mt-2 text-rose-300">8-bit bus access</div>
              </div>
            </div>
            <Verdict ok={false}>USART_xx: half-word or word access only</Verdict>
          </div>
        );
      }
      const lsb = 1 << 13;
      const msb = 1 << 18;
      return (
        <div className="space-y-2">
          <CodeLine parts={[["struct { uint32_t sbk:1, rwu:1, re:1, te:1, …, m:1, ue:1, …; } cr1;  cr1.ue = 1;", true]]} />
          <BitRuler />
          <BitRow bits={toBits(lsb, 32)} label="GCC, ARM LE" caption={hex(lsb, 32)} idPrefix="le" tone={(i, b) => (b ? "good" : "dim")} />
          <BitRow bits={toBits(msb, 32)} label="big-endian ABI" caption={hex(msb, 32)} idPrefix="be" tone={(i, b) => (b ? "bad" : "dim")} />
          <Verdict ok>USART_CR1_UE = 1UL &lt;&lt; 13, on every compiler</Verdict>
        </div>
      );
    }}
  </Stepper>
);

export default BitfieldRmw;
