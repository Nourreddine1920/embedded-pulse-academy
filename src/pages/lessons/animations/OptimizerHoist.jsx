import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* Listings match arm-none-eabi-gcc 10.3 output for: while (!s_flag) { } */
const LISTINGS = {
  O0: {
    title: "main() · -O0 (Debug)",
    lines: ["loop: LDR  r3, =s_flag", "      LDRB r3, [r3]      ; read RAM", "      CMP  r3, #0", "      BEQ  loop"],
  },
  O2: {
    title: "main() · -O2 (Release)",
    lines: ["      LDR  r3, =s_flag", "      LDRB r3, [r3]      ; read ONCE", "      CBNZ r3, done", "loop: B    loop          ; no load!", "done: BX   lr"],
  },
  VOL: {
    title: "main() · -O2 · volatile bool s_flag",
    lines: ["      LDR  r2, =s_flag", "loop: LDRB r3, [r2]      ; read EVERY time", "      CMP  r3, #0", "      BEQ  loop", "      BX   lr"],
  },
};
const ISR = ["MOVS r2, #1", "STRB r2, [s_flag]", "BX   lr"];

const frames = [
  { listing: "O0", active: -1, ram: 0, r3: null, isr: -1 },
  { listing: "O0", active: 1, ram: 0, r3: 0, isr: -1 },
  { listing: "O0", active: 3, ram: 1, r3: 0, isr: 1 },
  { listing: "O0", active: 1, ram: 1, r3: 1, isr: -1, ok: "Debug build: loop exits" },
  { listing: "O2", active: 1, ram: 0, r3: 0, isr: -1 },
  { listing: "O2", active: 3, ram: 1, r3: 0, isr: 1 },
  { listing: "O2", active: 3, ram: 1, r3: 0, isr: -1, bad: "Release build: spins forever" },
  { listing: "VOL", active: 1, ram: 1, r3: 1, isr: -1, ok: "volatile: re-read, loop exits" },
];

const steps = [
  { label: "The code", caption: "main() waits in while (!s_flag) { } for a flag that a timer interrupt sets to true. s_flag is a plain bool: nothing in main's loop writes it." },
  { label: "-O0: load", caption: "Without optimisation GCC translates the C literally: every iteration loads s_flag from RAM into r3 (LDRB) and compares it." },
  { label: "-O0: ISR runs", caption: "The interrupt stores 1 into s_flag in RAM. main's r3 still holds the old 0, but that's fine: the next iteration reloads it." },
  { label: "-O0: it works", caption: "The next LDRB reads 1, the loop exits. This is why the bug never shows up in the Debug build you single-step." },
  { label: "-O2: hoisted", caption: "The optimiser sees that nothing inside the loop can change s_flag (it doesn't know about interrupts), so it loads the flag ONCE, before the loop." },
  { label: "-O2: ISR runs", caption: "The ISR writes 1 to RAM exactly as before. But the loop no longer contains a load: it's a single B to itself." },
  { label: "-O2: forever", caption: "RAM says 1, r3 says 0, and nothing will ever reload it. The Release build hangs. This is legal: the C abstract machine has no interrupts." },
  { label: "Fix: volatile", caption: "volatile bool s_flag tells the compiler 'this object can change behind your back'. Every access in the source becomes a real load, so the LDRB is back inside the loop." },
];

const Lane = ({ title, lines, active, tone }) => (
  <div className="min-w-[250px] flex-1 rounded-xl border border-slate-800 bg-slate-900/60 p-3">
    <p className={`mb-2 text-xs font-bold uppercase tracking-[0.14em] ${tone}`}>{title}</p>
    <ol className="space-y-1 font-mono text-xs">
      {lines.map((line, i) => (
        <li key={line} className={`whitespace-pre rounded px-2 py-1 transition ${i === active ? "bg-cyan-400/20 text-cyan-100 ring-1 ring-cyan-400/60" : "text-slate-400"}`}>
          {line}
        </li>
      ))}
    </ol>
  </div>
);

const Cell = ({ label, value, tone }) => (
  <div className="flex items-center gap-3">
    <span className="w-28 text-right font-mono text-xs text-slate-400">{label}</span>
    <motion.span
      key={`${label}-${value}`}
      initial={{ scale: 0.8, opacity: 0.4 }}
      animate={{ scale: 1, opacity: 1 }}
      className={`inline-flex h-9 w-14 items-center justify-center rounded-lg border font-mono text-sm font-bold ${tone}`}
    >
      {value === null ? "–" : value}
    </motion.span>
  </div>
);

const OptimizerHoist = () => (
  <Stepper title="Animation · The optimiser hoists a load out of the loop" steps={steps}>
    {(step) => {
      const f = frames[step];
      const listing = LISTINGS[f.listing];
      const stale = f.r3 !== null && f.r3 !== f.ram;
      return (
        <div className="max-w-[680px] space-y-4">
          <CodeLine parts={[[f.listing === "VOL" ? "static volatile bool s_flag;  " : "static bool s_flag;  ", f.listing === "VOL"], ["while (!s_flag) { }", step > 0]]} />
          <div className="flex flex-wrap gap-3">
            <Lane title={listing.title} lines={listing.lines} active={f.active} tone={f.listing === "VOL" ? "text-emerald-300" : "text-cyan-300"} />
            <motion.div animate={{ opacity: f.isr >= 0 ? 1 : 0.45 }} className="min-w-[170px]">
              <Lane title={f.isr >= 0 ? "TIM ISR ⚡ running" : "TIM ISR"} lines={ISR} active={f.isr} tone={f.isr >= 0 ? "text-amber-300" : "text-slate-500"} />
            </motion.div>
          </div>
          <div className="space-y-2">
            <Cell label="s_flag (RAM)" value={f.ram} tone={f.ram ? "border-amber-400/70 bg-amber-500/15 text-amber-200" : "border-slate-700 bg-slate-900 text-slate-400"} />
            <Cell label="r3 (CPU)" value={f.r3} tone={stale ? "border-rose-400/80 bg-rose-500/20 text-rose-200" : f.r3 ? "border-emerald-400/70 bg-emerald-500/15 text-emerald-200" : "border-slate-700 bg-slate-900 text-slate-400"} />
          </div>
          {f.ok && <Verdict ok>{f.ok}</Verdict>}
          {f.bad && <Verdict ok={false}>{f.bad}</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default OptimizerHoist;
