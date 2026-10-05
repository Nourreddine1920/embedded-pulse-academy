import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine } from "./Stepper";

/* Real arm-none-eabi-gcc 10.3 -O2 output (lab A1.1, size/core_compare.c), disassembled with objdump. */
const CORES = [
  { id: "m0", name: "Cortex-M0 / M0+", flags: "-mcpu=cortex-m0" },
  { id: "m3", name: "Cortex-M3", flags: "-mcpu=cortex-m3" },
  { id: "m4", name: "Cortex-M4", flags: "-mcpu=cortex-m4 (soft float)" },
  { id: "m4f", name: "Cortex-M4F", flags: "-mfpu=fpv4-sp-d16 -mfloat-abi=hard" },
  { id: "m7d", name: "Cortex-M7 (DP FPU)", flags: "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard" },
];

const FUNCS = [
  { code: "int32_t div_i32(int32_t a, int32_t b) { return a / b; }", out: { m0: ["bl __aeabi_idiv", "lib"], m3: ["sdiv r0, r0, r1", "hw"], m4: ["sdiv r0, r0, r1", "hw"], m4f: ["sdiv r0, r0, r1", "hw"], m7d: ["sdiv r0, r0, r1", "hw"] } },
  { code: "uint64_t mul_u64(uint32_t a, uint32_t b) { return (uint64_t)a * b; }", out: { m0: ["bl __aeabi_lmul", "lib"], m3: ["umull r0, r1, r0, r1", "hw"], m4: ["umull r0, r1, r0, r1", "hw"], m4f: ["umull r0, r1, r0, r1", "hw"], m7d: ["umull r0, r1, r0, r1", "hw"] } },
  { code: "uint32_t leading_zeros(uint32_t x) { … __builtin_clz(x) … }", out: { m0: ["bl __clzsi2", "lib"], m3: ["clz r0, r0", "hw"], m4: ["clz r0, r0", "hw"], m4f: ["clz r0, r0", "hw"], m7d: ["clz r0, r0", "hw"] } },
  { code: "int32_t sat16(int32_t x) { clamp to -32768…32767 }", out: { m0: ["10 instr., compare + branch", "lib"], m3: ["ssat r0, #16, r0", "hw"], m4: ["ssat r0, #16, r0", "hw"], m4f: ["ssat r0, #16, r0", "hw"], m7d: ["ssat r0, #16, r0", "hw"] } },
  { code: "float add_f32(float a, float b) { return a + b; }", out: { m0: ["bl __aeabi_fadd", "lib"], m3: ["bl __aeabi_fadd", "lib"], m4: ["bl __aeabi_fadd", "lib"], m4f: ["vadd.f32 s0, s0, s1", "hw"], m7d: ["vadd.f32 s0, s0, s1", "hw"] } },
  { code: "double add_f64(double a, double b) { return a + b; }", out: { m0: ["bl __aeabi_dadd", "lib"], m3: ["bl __aeabi_dadd", "lib"], m4: ["bl __aeabi_dadd", "lib"], m4f: ["bl __aeabi_dadd", "lib"], m7d: ["vadd.f64 d0, d0, d1", "hw"] } },
];

const steps = [
  { label: "One source, five builds", caption: "The same six C functions compiled for five targets. The compiler may only use instructions the core has. Where one is missing it calls a library routine (libgcc) that does the job in software." },
  { label: "Division", caption: "ARMv6-M (M0, M0+) has no divide instruction, so '/' becomes a call to __aeabi_idiv. ARMv7-M cores and later have SDIV: one instruction, 2 to 12 cycles depending on the operands (Arm TRM)." },
  { label: "64-bit multiply", caption: "UMULL produces a 64-bit product from two 32-bit operands in one instruction on ARMv7-M. On the M0 the compiler calls __aeabi_lmul, which builds the product from several 16 x 16 multiplies." },
  { label: "Count leading zeros", caption: "CLZ is a single instruction from the M3 up. The M0 calls __clzsi2. This is what makes normalization and priority-encoding loops cheap on bigger cores." },
  { label: "Saturation", caption: "A clamp written as two ifs compiles to a single SSAT on ARMv7-M. The M0 keeps the compare-and-branch sequence. Saturating arithmetic is one reason the M3 and up are the usual choice for signal processing." },
  { label: "Single-precision float", caption: "Only the M4F (FPv4-SP) and the M7 do float add in hardware: one VADD.F32. The M3, and an M4 built without the FPU flags, call __aeabi_fadd: a library call that runs many instructions per add, and a larger program." },
  { label: "Double precision", caption: "The F446's FPU is single precision only: double arithmetic still goes to the library on the M4F. Only a core with an FPv5 double-precision FPU (some Cortex-M7 parts) has VADD.F64. In practice: use float on the F4, and write 1.0f, not 1.0." },
];

const TONE = {
  hw: "border-emerald-400/70 bg-emerald-500/15 text-emerald-200",
  lib: "border-amber-400/70 bg-amber-500/15 text-amber-200",
  off: "border-slate-800 bg-slate-950 text-slate-700",
};

const CompileByCore = () => (
  <Stepper title="Animation · The same C source on five cores" steps={steps}>
    {(step) => {
      const shown = step === 0 ? 0 : step;
      const focus = step === 0 ? -1 : step - 1;
      return (
        <div className="max-w-[760px] space-y-3">
          <CodeLine parts={[[focus >= 0 ? FUNCS[focus].code : "arm-none-eabi-gcc -mcpu=… -mthumb -O2 -c core_compare.c", true]]} />
          <div className="grid grid-cols-[repeat(5,minmax(120px,1fr))] gap-2">
            {CORES.map((c) => (
              <div key={c.id} className="rounded-md border border-slate-700 bg-slate-900 px-2 py-1">
                <p className="text-xs font-semibold text-slate-100">{c.name}</p>
                <p className="font-mono text-[9px] leading-3 text-slate-500">{c.flags}</p>
              </div>
            ))}
            {FUNCS.map((f, row) =>
              CORES.map((c) => {
                const visible = row < shown;
                const [text, kind] = f.out[c.id];
                return (
                  <motion.div
                    key={`${row}-${c.id}`}
                    animate={{ opacity: visible ? (row === focus || step === 0 ? 1 : 0.55) : 0.25 }}
                    className={`rounded-md border px-2 py-1 font-mono text-[11px] leading-4 ${visible ? TONE[kind] : TONE.off} ${row === focus ? "ring-1 ring-cyan-300/70" : ""}`}
                  >
                    {visible ? text : "·"}
                  </motion.div>
                );
              })
            )}
          </div>
          <p className="text-[11px] text-slate-500">
            <span className="text-emerald-300">green</span>: one instruction · <span className="text-amber-300">amber</span>: library call or an inline software sequence
          </p>
        </div>
      );
    }}
  </Stepper>
);

export default CompileByCore;
