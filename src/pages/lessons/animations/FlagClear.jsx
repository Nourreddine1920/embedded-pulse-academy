import React from "react";
import BitRow, { toBits } from "./BitRow";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* USART_SR low byte: bit 7 TXE (r), bit 6 TC (rc_w0), bit 5 RXNE (rc_w0). */
const TXE = 7;
const TC = 6;
const RXNE = 5;

const usartFrames = [
  { sr: 0xc0, r3: null },
  { sr: 0xc0, r3: 0xc0 },
  { sr: 0xe0, r3: 0xc0 },
  { sr: 0xe0, r3: 0x80 },
  { sr: 0x80, r3: 0x80, lost: true },
  { sr: 0xa0, write: 0xbf, fixed: true },
];

const steps = [
  { label: "USART_SR", caption: "TXE (bit 7) and TC (bit 6) are 1. TC and RXNE are 'rc_w0' bits: writing 0 clears them, writing 1 does nothing. We want to clear TC." },
  { label: "LDR", caption: "SR &= ~USART_SR_TC compiles to LDR, BIC, STR. First the read: r3 = 0xC0." },
  { label: "A byte arrives", caption: "Between the read and the write, the receiver finishes a byte and the hardware sets RXNE (bit 5). Our copy in r3 doesn't know." },
  { label: "BIC", caption: "Clear bit 6 in the copy: r3 = 0x80. Bit 5 in the copy is 0 because it was 0 when we read it." },
  { label: "STR: the damage", caption: "Writing 0x80 back writes 0 to TC (cleared, as intended) and 0 to RXNE, which clears it too. The received byte's flag is gone: the byte is never read, and it's overwritten by the next one." },
  { label: "Fix: SR = ~TC", caption: "Write 1 everywhere except TC. Ones are ignored by rc_w0 bits and by read-only bits, so only TC clears and RXNE survives. This is exactly what __HAL_UART_CLEAR_FLAG does." },
  { label: "EXTI_PR (rc_w1)", caption: "The opposite convention: EXTI pending bits clear when you write 1. Lines 13 and 0 are both pending. EXTI->PR |= EXTI_PR_PR13 reads 0x2001 and writes 0x2001 back, which clears BOTH, and the line-0 interrupt is silently lost." },
  { label: "Fix: PR = PR13", caption: "Write only the bit you want to clear. Zeros are ignored by rc_w1 bits, so line 0 stays pending. This is what __HAL_GPIO_EXTI_CLEAR_IT does." },
];

const srTone = (lost) => (i, b, idx) => {
  if (lost && idx === RXNE) return "bad";
  if (idx === TC) return b ? "focus" : "good";
  if (idx === RXNE && b) return "promoted";
  if (idx === TXE && b) return "base";
  return "dim";
};

const FlagClear = () => (
  <Stepper title="Animation · Clearing status flags without losing events" steps={steps}>
    {(step) => {
      if (step <= 5) {
        const f = usartFrames[step];
        return (
          <div className="space-y-2">
            <CodeLine parts={[[step === 5 ? "USART2->SR = ~USART_SR_TC;" : "USART2->SR &= ~USART_SR_TC;", true]]} />
            <p className="font-mono text-[11px] text-slate-500">bit 7 = TXE (r) · bit 6 = TC (rc_w0) · bit 5 = RXNE (rc_w0)</p>
            <BitRow bits={toBits(f.sr, 8)} maxWidth={8} label="USART2->SR" caption={`0x${f.sr.toString(16).toUpperCase()}`} idPrefix="sr" tone={srTone(f.lost)} />
            {f.r3 !== null && f.r3 !== undefined && <BitRow bits={toBits(f.r3, 8)} maxWidth={8} label="r3 (copy)" caption={`0x${f.r3.toString(16).toUpperCase()}`} idPrefix="r3" tone={(i, b) => (b ? "focus" : "zero")} />}
            {f.write && <BitRow bits={toBits(f.write, 8)} maxWidth={8} label="value written" caption="0xBF" idPrefix="w" tone={(i, b, idx) => (idx === TC ? "good" : "dim")} />}
            {f.lost && <Verdict ok={false}>RXNE cleared: received byte lost</Verdict>}
            {f.fixed && <Verdict ok>Only TC cleared, RXNE kept</Verdict>}
          </div>
        );
      }
      const before = 0x2001;
      const after = step === 6 ? 0x0000 : 0x0001;
      return (
        <div className="space-y-2">
          <CodeLine parts={[[step === 6 ? "EXTI->PR |= EXTI_PR_PR13;" : "EXTI->PR = EXTI_PR_PR13;", true]]} />
          <BitRow bits={toBits(before, 16)} maxWidth={16} label="PR before" caption="0x2001" idPrefix="pb" tone={(i, b, idx) => (idx === 13 ? "focus" : idx === 0 ? "promoted" : b ? "base" : "dim")} />
          <BitRow bits={toBits(step === 6 ? 0x2001 : 0x2000, 16)} maxWidth={16} label="value written" caption={step === 6 ? "0x2001" : "0x2000"} idPrefix="pw" tone={(i, b, idx) => (idx === 0 && b ? "bad" : idx === 13 ? "good" : "dim")} />
          <BitRow bits={toBits(after, 16)} maxWidth={16} label="PR after" caption={`0x${after.toString(16).toUpperCase().padStart(4, "0")}`} idPrefix="pa" tone={(i, b, idx) => (idx === 0 ? (b ? "good" : "bad") : b ? "base" : "dim")} />
          {step === 6 ? <Verdict ok={false}>Line 0 interrupt lost</Verdict> : <Verdict ok>Line 13 cleared, line 0 still pending</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default FlagClear;
