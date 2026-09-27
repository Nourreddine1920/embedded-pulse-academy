import React from "react";
import { motion } from "framer-motion";
import BitRow, { toBits } from "./BitRow";
import Stepper, { Verdict } from "./Stepper";

/* Each step: which instruction is active in each lane, and the values shown. */
const MAIN = ["LDR  r3, [GPIOA_ODR]", "ORR  r3, r3, #0x20", "STR  r3, [GPIOA_ODR]"];
const ISR = ["LDR  r2, [GPIOA_ODR]", "ORR  r2, r2, #0x40", "STR  r2, [GPIOA_ODR]", "BX   lr"];
const FIX = ["MOVS r2, #0x20", "STR  r2, [GPIOA_BSRR]"];

const frames = [
  { main: -1, isr: -1, odr: 0x00, r3: null, irq: false },
  { main: 0, isr: -1, odr: 0x00, r3: 0x00, irq: false },
  { main: 1, isr: -1, odr: 0x00, r3: 0x20, irq: false },
  { main: 1, isr: 2, odr: 0x40, r3: 0x20, irq: true },
  { main: 2, isr: -1, odr: 0x20, r3: 0x20, irq: false, lost: true },
  { main: -1, isr: -1, odr: 0x60, r3: null, irq: false, fix: true },
];

const steps = [
  { label: "The setup", caption: "main() wants to turn on PA5 (bit 5). A timer interrupt turns on PA6 (bit 6) of the same port. Both use ODR |= mask." },
  { label: "main: LDR", caption: "main reads ODR into register r3: 0x00." },
  { label: "main: ORR", caption: "main sets bit 5 in its private copy: r3 = 0x20. ODR itself hasn't been written yet." },
  { label: "Interrupt!", caption: "The timer IRQ fires between ORR and STR. The ISR does its own read-modify-write: ODR = 0x00 | 0x40 = 0x40. PA6 turns on, and the ISR returns." },
  { label: "main: STR", caption: "main resumes and stores its stale copy r3 = 0x20. Bit 6 is overwritten with 0: the ISR's write is lost, and PA6 turns off without anyone asking." },
  { label: "The fix: BSRR", caption: "Writing 1 << 5 to BSRR sets PA5 in a single store, and the hardware leaves all other pins alone. Whenever the ISR runs, both bits survive: ODR = 0x60." },
];

const Lane = ({ title, lines, active, tone }) => (
  <div className="min-w-[210px] flex-1 rounded-xl border border-slate-800 bg-slate-900/60 p-3">
    <p className={`mb-2 text-xs font-bold uppercase tracking-[0.14em] ${tone}`}>{title}</p>
    <ol className="space-y-1 font-mono text-xs">
      {lines.map((line, i) => (
        <li key={line} className={`rounded px-2 py-1 transition ${i === active ? "bg-cyan-400/20 text-cyan-100 ring-1 ring-cyan-400/60" : i < active ? "text-slate-500" : "text-slate-400"}`}>
          {line}
        </li>
      ))}
    </ol>
  </div>
);

const RmwRace = () => (
  <Stepper title="Animation · The read-modify-write race" steps={steps}>
    {(step) => {
      const f = frames[step];
      return (
        <div className="max-w-[640px] space-y-4">
          <div className="flex flex-wrap gap-3">
            {f.fix ? (
              <Lane title="main (fixed)" lines={FIX} active={1} tone="text-emerald-300" />
            ) : (
              <Lane title="main()" lines={MAIN} active={f.main} tone="text-cyan-300" />
            )}
            <motion.div animate={{ opacity: f.irq ? 1 : 0.45, scale: f.irq ? 1 : 0.98 }} className="flex-1">
              <Lane title={f.irq ? "TIM ISR ⚡ running" : "TIM ISR"} lines={ISR} active={f.isr} tone={f.irq ? "text-amber-300" : "text-slate-500"} />
            </motion.div>
          </div>
          <div className="space-y-2">
            <BitRow
              bits={toBits(f.odr, 8)}
              maxWidth={8}
              label="GPIOA->ODR[7:0]"
              caption={`0x${f.odr.toString(16).toUpperCase().padStart(2, "0")}`}
              idPrefix="odr"
              tone={(i, b, idx) => (f.lost && idx === 6 ? "bad" : idx === 5 && b ? "focus" : idx === 6 && b ? "promoted" : b ? "base" : "zero")}
            />
            {f.r3 !== null && (
              <BitRow bits={toBits(f.r3, 8)} maxWidth={8} label="r3 (main's copy)" caption={`0x${f.r3.toString(16).toUpperCase().padStart(2, "0")}`} idPrefix="r3" tone={(i, b) => (b ? "focus" : "zero")} />
            )}
          </div>
          {f.lost && <Verdict ok={false}>PA6 lost: the ISR's write was overwritten</Verdict>}
          {f.fix && <Verdict ok>PA5 and PA6 both on</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default RmwRace;
