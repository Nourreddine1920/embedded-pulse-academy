import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* main's loop body as compiled by GCC 10.3 -O2 (see the lab's count_volatile / count_atomic). */
const MAIN_V = ["LDR   r3, [r1]      ; s_count", "ADD   r3, r3, #1", "STR   r3, [r1]"];
const MAIN_A = ["LDREX r1, [r2]      ; s_count", "ADDS  r1, #1", "STREX r0, r1, [r2]", "CMP   r0, #0", "BNE   retry"];
const ISR = ["LDR   r3, [s_count]", "ADD   r3, #1", "STR   r3, [s_count]", "BX    lr"];

const frames = [
  { main: MAIN_V, active: -1, isr: -1, ram: 41, reg: null, expect: 41 },
  { main: MAIN_V, active: 0, isr: -1, ram: 41, reg: 41, expect: 41 },
  { main: MAIN_V, active: 0, isr: 2, ram: 42, reg: 41, expect: 42, irq: true },
  { main: MAIN_V, active: 1, isr: -1, ram: 42, reg: 42, expect: 42 },
  { main: MAIN_V, active: 2, isr: -1, ram: 42, reg: 42, expect: 43, lost: true },
  { main: MAIN_A, active: 0, isr: -1, ram: 41, reg: 41, expect: 41, tag: "armed" },
  { main: MAIN_A, active: 0, isr: 2, ram: 42, reg: 41, expect: 42, irq: true, tag: "cleared" },
  { main: MAIN_A, active: 2, isr: -1, ram: 42, reg: 42, expect: 43, tag: "cleared", strex: "r0 = 1: failed, nothing written" },
  { main: MAIN_A, active: 2, isr: -1, ram: 43, reg: 43, expect: 43, tag: "armed", strex: "retry: LDREX 42, ADDS, STREX ok", ok: true },
];

const steps = [
  { label: "The setup", caption: "volatile uint32_t s_count = 41. main() does s_count++ in a loop, and a timer ISR also does s_count++. After one of each we expect 43." },
  { label: "main: LDR", caption: "volatile guarantees a real load: main reads 41 into r3. So far so good." },
  { label: "Interrupt!", caption: "The timer IRQ fires between main's LDR and STR. The ISR loads 41, adds 1 and stores 42. RAM is correct for now." },
  { label: "main: ADD", caption: "main resumes with its stale copy: r3 = 41 + 1 = 42. volatile made the load happen; it cannot make the three instructions indivisible." },
  { label: "main: STR", caption: "main stores 42 over the ISR's 42. Two increments ran, the counter moved by one. The lab measures hundreds of these per second at 20 kHz." },
  { label: "Fix: LDREX", caption: "atomic_fetch_add compiles to LDREX/STREX. LDREX loads 41 and arms the core's exclusive monitor on that address." },
  { label: "Interrupt!", caption: "Same interrupt, same place. The ISR stores 42. On Cortex-M, taking an exception clears the exclusive monitor, so main's reservation is gone." },
  { label: "STREX fails", caption: "STREX only stores if the monitor is still armed. It isn't, so nothing is written and r0 = 1. The CMP/BNE loops back." },
  { label: "Retry wins", caption: "The retry loads 42, adds 1 and STREX succeeds: 43. No interrupt was disabled, and the ISR's increment survived." },
];

const Lane = ({ title, lines, active, tone }) => (
  <div className="min-w-[240px] flex-1 rounded-xl border border-slate-800 bg-slate-900/60 p-3">
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

const Box = ({ label, value, tone }) => (
  <div className="flex flex-col items-center gap-1">
    <span className="font-mono text-[11px] text-slate-400">{label}</span>
    <motion.span key={`${label}-${value}`} initial={{ scale: 0.8 }} animate={{ scale: 1 }} className={`inline-flex h-9 min-w-[3.5rem] items-center justify-center rounded-lg border px-2 font-mono text-sm font-bold ${tone}`}>
      {value === null ? "–" : value}
    </motion.span>
  </div>
);

const NEUTRAL = "border-slate-700 bg-slate-900 text-slate-300";

const VolatileNotAtomic = () => (
  <Stepper title="Animation · volatile is not atomic" steps={steps}>
    {(step) => {
      const f = frames[step];
      const atomic = f.main === MAIN_A;
      return (
        <div className="max-w-[680px] space-y-4">
          <CodeLine parts={[[atomic ? "atomic_fetch_add_explicit(&s_count, 1U, memory_order_relaxed);" : "s_count++;   /* volatile uint32_t */", true]]} />
          <div className="flex flex-wrap gap-3">
            <Lane title={atomic ? "main() · atomic" : "main() · volatile"} lines={f.main} active={f.active} tone={atomic ? "text-emerald-300" : "text-cyan-300"} />
            <motion.div animate={{ opacity: f.irq ? 1 : 0.45 }} className="min-w-[190px]">
              <Lane title={f.irq ? "TIM ISR ⚡ running" : "TIM ISR"} lines={ISR} active={f.isr} tone={f.irq ? "text-amber-300" : "text-slate-500"} />
            </motion.div>
          </div>
          <div className="flex flex-wrap items-end gap-4">
            <Box label="s_count (RAM)" value={f.ram} tone={f.lost ? "border-rose-400/80 bg-rose-500/20 text-rose-200" : f.ok ? "border-emerald-400/70 bg-emerald-500/15 text-emerald-200" : NEUTRAL} />
            <Box label="main's register" value={f.reg} tone={NEUTRAL} />
            <Box label="expected" value={f.expect} tone="border-slate-800 bg-slate-950 text-slate-400" />
            {f.tag && <Box label="exclusive monitor" value={f.tag} tone={f.tag === "armed" ? "border-cyan-400/70 bg-cyan-500/15 text-cyan-100" : "border-amber-400/70 bg-amber-500/15 text-amber-200"} />}
          </div>
          {f.strex && <p className="font-mono text-xs text-slate-300">STREX → {f.strex}</p>}
          {f.lost && <Verdict ok={false}>One increment lost: 42, expected 43</Verdict>}
          {f.ok && <Verdict ok>43: both increments counted</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default VolatileNotAtomic;
