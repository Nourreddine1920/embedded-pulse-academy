import React from "react";
import { motion } from "framer-motion";
import BitRow, { BitRuler, hex, toBits } from "./BitRow";
import Stepper, { CodeLine } from "./Stepper";

/* Cortex-M4 programmer's model (DUI 0553 "Core registers") and AAPCS roles. */
const REGS = [
  ["R0", "arg 1 / result"],
  ["R1", "arg 2 / result hi"],
  ["R2", "arg 3"],
  ["R3", "arg 4"],
  ["R4", "callee-saved"],
  ["R5", "callee-saved"],
  ["R6", "callee-saved"],
  ["R7", "callee-saved"],
  ["R8", "callee-saved"],
  ["R9", "callee-saved"],
  ["R10", "callee-saved"],
  ["R11", "callee-saved"],
  ["R12", "scratch (IP)"],
  ["SP", "MSP or PSP"],
  ["LR", "return address"],
  ["PC", "next instruction"],
];

const TONE = {
  dim: "border-slate-800 bg-slate-950 text-slate-600",
  base: "border-slate-700 bg-slate-900 text-slate-300",
  low: "border-cyan-400/70 bg-cyan-500/15 text-cyan-100",
  high: "border-sky-400/60 bg-sky-500/10 text-sky-100",
  scratch: "border-amber-400/70 bg-amber-500/15 text-amber-100",
  saved: "border-emerald-400/70 bg-emerald-500/15 text-emerald-100",
  sp: "border-fuchsia-400/70 bg-fuchsia-500/15 text-fuchsia-100",
  lr: "border-violet-400/70 bg-violet-500/15 text-violet-100",
  pc: "border-rose-400/70 bg-rose-500/15 text-rose-100",
};

const steps = [
  { label: "Sixteen registers", caption: "The Cortex-M register file: R0 to R12 are general purpose, R13 is the stack pointer, R14 the link register, R15 the program counter. Everything the CPU computes passes through them." },
  { label: "Low and high", caption: "R0 to R7 (low) can be used by every instruction, including the compact 16-bit Thumb ones. R8 to R12 (high) are reachable by most 32-bit instructions but few 16-bit ones, which is why compilers prefer the low registers." },
  { label: "AAPCS: who may destroy what", caption: "By the ARM calling standard, R0 to R3 and R12 are scratch: a called function may overwrite them. R4 to R11 are callee-saved: if a function uses them it must push them first and restore them before returning. That is the 'stmdb sp!, {r4-r8, lr}' at the top of functions." },
  { label: "SP: two stacks", caption: "R13 is really two registers, MSP (main) and PSP (process). Only one is visible as SP at a time. Exceptions always run on MSP. Thread code uses MSP or PSP according to CONTROL.SPSEL: an RTOS gives each task its own PSP (A1.3)." },
  { label: "LR: the way back", caption: "A BL (call) writes the return address into LR. In an exception handler LR holds a special EXC_RETURN value instead, and returning to it tells the core to unstack (A6.4). Bit 0 of a return address is always 1 (Thumb)." },
  { label: "PC: always Thumb", caption: "R15 points at the next instruction. Reads show the instruction address plus 4, and bit 0 reads as 0. Writing an address with bit 0 clear would switch to the ARM state, which Cortex-M doesn't have: UsageFault (INVSTATE). Every vector table entry therefore has bit 0 set." },
  { label: "xPSR: flags, IT, exception number", caption: "Three views of one register: APSR (N Z C V Q and GE flags), EPSR (the Thumb bit T, and IT/ICI state) and IPSR (the number of the running exception, 0 in thread mode)." },
  { label: "Mask registers and CONTROL", caption: "Outside the register file proper are PRIMASK, FAULTMASK, BASEPRI (interrupt masks) and CONTROL (privilege level and stack choice). They're reached only with MRS/MSR, or the CMSIS __get_/__set_ functions." },
];

const XPSR = 0x61000016; // Z C set, Thumb bit, IPSR = 22 (IRQ6)

const toneFor = (name, step) => {
  if (step === 0) return "base";
  if (step === 1) return /^R([0-7])$/.test(name) ? "low" : /^R(8|9|1[0-2])$/.test(name) ? "high" : "dim";
  if (step === 2) return /^R[0-3]$|^R12$/.test(name) ? "scratch" : /^R([4-9]|1[01])$/.test(name) ? "saved" : "dim";
  if (step === 3) return name === "SP" ? "sp" : "dim";
  if (step === 4) return name === "LR" ? "lr" : "dim";
  if (step === 5) return name === "PC" ? "pc" : "dim";
  return "dim";
};

const RegisterFile = () => (
  <Stepper title="Animation · The Cortex-M register file" steps={steps}>
    {(step) => (
      <div className="max-w-[720px] space-y-4">
        <CodeLine
          parts={[
            [
              [
                "/* sixteen 32-bit registers, R0 to R15 */",
                "movs r0, #1 ;  add.w r9, r9, #1",
                "push {r4-r8, lr}  ...  pop {r4-r8, pc}",
                "mrs r0, msp ;  mrs r0, psp ;  msr psp, r0",
                "bl func   ->   lr = return address (bit 0 = 1)",
                "bx lr   ->   pc = lr",
                "mrs r0, xpsr",
                "mrs r0, primask ;  mrs r0, basepri ;  mrs r0, control",
              ][step],
              true,
            ],
          ]}
        />
        {step < 6 && (
          <div className="grid grid-cols-4 gap-2 sm:grid-cols-8">
            {REGS.map(([name, role]) => {
              const t = toneFor(name, step);
              return (
                <motion.div key={name} animate={{ opacity: t === "dim" ? 0.35 : 1, scale: t === "dim" || t === "base" ? 1 : 1.04 }} className={`rounded-lg border px-2 py-1.5 ${TONE[t]}`}>
                  <p className="font-mono text-xs font-bold">{name}</p>
                  <p className="font-mono text-[9px] leading-3 opacity-80">{step === 0 ? "32 bits" : role}</p>
                </motion.div>
              );
            })}
          </div>
        )}
        {step === 3 && (
          <div className="flex flex-wrap gap-2 font-mono text-xs">
            <span className="rounded border border-fuchsia-400/70 bg-fuchsia-500/15 px-2 py-1 text-fuchsia-100">MSP: reset, handlers, kernel</span>
            <span className="rounded border border-fuchsia-400/40 bg-slate-900 px-2 py-1 text-fuchsia-200">PSP: per-task (RTOS)</span>
            <span className="rounded border border-slate-600 bg-slate-900 px-2 py-1 text-slate-300">CONTROL.SPSEL = 1 selects PSP in thread mode</span>
          </div>
        )}
        {step === 6 && (
          <div className="space-y-2">
            <BitRuler />
            <BitRow
              bits={toBits(XPSR, 32)}
              label="xPSR"
              caption={hex(XPSR, 32)}
              idPrefix="x"
              tone={(i, b, idx) => (idx >= 28 ? (b ? "good" : "zero") : idx === 27 ? "promoted" : idx === 24 ? "sign" : idx <= 8 ? (b ? "focus" : "zero") : b ? "base" : "zero")}
            />
            <p className="font-mono text-xs leading-5 text-slate-300">
              <span className="text-emerald-300">N Z C V</span> [31:28] · <span className="text-amber-300">Q</span> [27] · <span className="text-fuchsia-300">T</span> [24] · <span className="text-cyan-300">ISR</span> [8:0]. The value above: Z and C set, Thumb, exception number 22 = IRQ6.
            </p>
          </div>
        )}
        {step === 7 && (
          <div className="grid gap-2 sm:grid-cols-3">
            {[
              ["PRIMASK", "1 bit", "1 = block every configurable exception"],
              ["FAULTMASK", "1 bit", "1 = block everything except NMI"],
              ["BASEPRI", "8 bits (top 4 used)", "block priorities numerically >= this value"],
              ["CONTROL.nPRIV", "bit 0", "1 = thread mode unprivileged"],
              ["CONTROL.SPSEL", "bit 1", "1 = thread mode uses PSP"],
              ["CONTROL.FPCA", "bit 2", "1 = floating-point context active"],
            ].map(([n, w, d]) => (
              <div key={n} className="rounded-lg border border-slate-700 bg-slate-900 px-3 py-2">
                <p className="font-mono text-xs font-bold text-cyan-200">
                  {n} <span className="font-normal text-slate-500">· {w}</span>
                </p>
                <p className="text-[11px] leading-4 text-slate-300">{d}</p>
              </div>
            ))}
          </div>
        )}
      </div>
    )}
  </Stepper>
);

export default RegisterFile;
