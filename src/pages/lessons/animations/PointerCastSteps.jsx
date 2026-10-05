import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

const FULL = [
  ["*", 3],
  ["(", 1],
  ["volatile ", 2],
  ["uint32_t *", 1],
  [")", 1],
  ["0x40020018UL", 0],
  [" = 0x20UL;", 4],
];

/* Real GCC 10.3 -O2 output (arm-none-eabi, -mcpu=cortex-m4 -mthumb). */
const ASM_VOLATILE = [
  "ldr   r3, [pc, #4]    ; r3 = 0x40020000 (literal pool)",
  "movs  r2, #32         ; r2 = 0x20",
  "str   r2, [r3, #24]   ; store to 0x40020000 + 24 = BSRR",
  ".word 0x40020000",
];
const ASM_TWO_PLAIN = [
  "ldr   r3, [pc, #8]    ; 0x40020000",
  "mov.w r2, #0x200000   ; only the SECOND value",
  "str   r2, [r3, #24]   ; one store: the first write vanished",
];
const ASM_TWO_VOLATILE = [
  "ldr   r3, [pc, #12]   ; 0x40020000",
  "movs  r1, #32",
  "mov.w r2, #0x200000",
  "str   r1, [r3, #24]   ; write 1: BS5",
  "str   r2, [r3, #24]   ; write 2: BR5",
];

const steps = [
  { label: "Just a number", caption: "0x40020018UL is an integer: an unsigned long that happens to equal the address of GPIOA_BSRR in RM0390. Nothing about it says 'memory' yet." },
  { label: "Cast to a pointer", caption: "(uint32_t *) turns the integer into a pointer to a 32-bit word at that address. The conversion is implementation-defined in ISO C; GCC for ARM keeps the bits unchanged. Still no memory access." },
  { label: "Add volatile", caption: "volatile uint32_t * says: every access through this pointer must really happen, exactly as written, in order, with the full 32-bit width. The compiler may not cache, merge or drop it (lesson A0.3)." },
  { label: "Dereference", caption: "The leading * turns the pointer into an lvalue: the register itself. You can now read it or assign to it, like a variable that lives at 0x40020018." },
  { label: "Store → bus write", caption: "The assignment compiles to one STR. The core puts address 0x40020018, data 0x00000020 and size 'word' on the AHB bus; GPIOA decodes offset 0x18 as BSRR and sets PA5. LD2 turns on." },
  { label: "Without volatile", caption: "Two plain stores to the same address: GCC keeps only the last one, because for ordinary memory the first is dead. For BSRR that deletes a real pin change. This is real -O2 output." },
  { label: "With volatile", caption: "The same two writes through a volatile pointer: two STRs, in order. Exactly what the hardware needs. This is why CMSIS declares every register volatile (__IO)." },
];

const Bus = ({ show }) => (
  <motion.div animate={{ opacity: show ? 1 : 0.25 }} className="flex flex-wrap items-center gap-3 rounded-xl border border-slate-800 bg-slate-900/60 p-3 font-mono text-xs">
    <span className="text-slate-400">AHB</span>
    <span className="rounded bg-cyan-500/15 px-2 py-1 text-cyan-200 ring-1 ring-cyan-400/50">HADDR 0x40020018</span>
    <span className="rounded bg-amber-500/15 px-2 py-1 text-amber-200 ring-1 ring-amber-400/50">HWDATA 0x00000020</span>
    <span className="rounded bg-emerald-500/15 px-2 py-1 text-emerald-200 ring-1 ring-emerald-400/50">HSIZE word</span>
    <span className="text-slate-500">→ GPIOA BSRR →</span>
    <motion.span
      animate={{ backgroundColor: show ? "rgba(52,211,153,0.9)" : "rgba(51,65,85,1)", boxShadow: show ? "0 0 18px rgba(52,211,153,0.8)" : "none" }}
      className="inline-block h-4 w-4 rounded-full"
    />
    <span className="text-slate-400">LD2 (PA5)</span>
  </motion.div>
);

const Asm = ({ lines }) => (
  <ol className="space-y-1 rounded-xl border border-slate-800 bg-slate-900/60 p-3 font-mono text-xs text-slate-300">
    {lines.map((l, i) => (
      <motion.li key={l} initial={{ opacity: 0, x: -8 }} animate={{ opacity: 1, x: 0 }} transition={{ delay: i * 0.12 }}>
        {l}
      </motion.li>
    ))}
  </ol>
);

const Kind = ({ step }) => {
  const kinds = [
    ["type", "unsigned long", "a value"],
    ["type", "uint32_t *", "an address, no access"],
    ["type", "volatile uint32_t *", "an address, every access kept"],
    ["type", "volatile uint32_t (lvalue)", "the register itself"],
  ];
  const k = kinds[Math.min(step, 3)];
  return (
    <div className="flex flex-wrap items-center gap-3 font-mono text-xs">
      <span className="text-slate-500">{k[0]}:</span>
      <motion.span key={k[1]} initial={{ scale: 0.8, opacity: 0 }} animate={{ scale: 1, opacity: 1 }} className="rounded bg-cyan-400/15 px-2 py-1 text-cyan-100 ring-1 ring-cyan-400/50">
        {k[1]}
      </motion.span>
      <span className="text-slate-400">{k[2]}</span>
    </div>
  );
};

const PointerCastSteps = () => (
  <Stepper title="Animation · Building *(volatile uint32_t *)0x40020018 = 0x20 piece by piece" steps={steps}>
    {(step) => {
      if (step === 5 || step === 6) {
        const vol = step === 6;
        return (
          <div className="max-w-[640px] space-y-3">
            <CodeLine
              parts={[
                [vol ? "REG32(0x40020018UL) = 0x20UL;  REG32(0x40020018UL) = 0x200000UL;" : "*(uint32_t *)0x40020018UL = 0x20UL;  *(uint32_t *)0x40020018UL = 0x200000UL;", true],
              ]}
            />
            <Asm lines={vol ? ASM_TWO_VOLATILE : ASM_TWO_PLAIN} />
            {vol ? <Verdict ok>Both bus writes happen, in order</Verdict> : <Verdict ok={false}>First write optimized away</Verdict>}
          </div>
        );
      }
      return (
        <div className="max-w-[640px] space-y-3">
          <CodeLine parts={FULL.filter(([, s]) => s <= Math.max(step, 0) || (step >= 4)).map(([t, s]) => [t, s === step || (step === 4 && s === 4)])} />
          {step <= 3 ? <Kind step={step} /> : <Asm lines={ASM_VOLATILE} />}
          <Bus show={step === 4} />
        </div>
      );
    }}
  </Stepper>
);

export default PointerCastSteps;
