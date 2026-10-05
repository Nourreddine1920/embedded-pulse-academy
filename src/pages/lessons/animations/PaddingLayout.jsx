import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* Byte-by-byte layouts, verified with arm-none-eabi-gcc 10.3 (and MinGW gcc 12). */
const MEMBER_TONE = {
  a: "border-amber-400/70 bg-amber-500/15 text-amber-200",
  b: "border-cyan-400/80 bg-cyan-500/15 text-cyan-100",
  c: "border-emerald-400/70 bg-emerald-500/15 text-emerald-200",
  pad: "border-dashed border-slate-600 bg-slate-900 text-slate-500",
  empty: "border-slate-800 bg-slate-950 text-slate-800",
  bad: "border-rose-400/80 bg-rose-500/20 text-rose-200",
  reg: "border-slate-600 bg-slate-800 text-slate-100",
};

const frames = [
  { code: "struct s { uint8_t a; uint32_t b; uint16_t c; };", bytes: ["a"], size: null },
  { code: "struct s { uint8_t a; uint32_t b; uint16_t c; };", bytes: ["a", "pad", "pad", "pad", "b", "b", "b", "b"], size: null },
  { code: "struct s { uint8_t a; uint32_t b; uint16_t c; };   /* sizeof = 12 */", bytes: ["a", "pad", "pad", "pad", "b", "b", "b", "b", "c", "c", "pad", "pad"], size: 12 },
  { code: "struct s { uint32_t b; uint16_t c; uint8_t a; };   /* sizeof = 8 */", bytes: ["b", "b", "b", "b", "c", "c", "a", "pad"], size: 8 },
  { code: "struct __attribute__((packed)) s { uint8_t a; uint32_t b; uint16_t c; };   /* 7 */", bytes: ["a", "b", "b", "b", "b", "c", "c"], size: 7, misaligned: true },
];

const RCC_ROWS = [
  ["0x18", "AHB3RSTR", "reg"],
  ["0x1C", "RESERVED0", "pad"],
  ["0x20", "APB1RSTR", "reg"],
  ["0x24", "APB2RSTR", "reg"],
  ["0x28", "RESERVED1[0]", "pad"],
  ["0x2C", "RESERVED1[1]", "pad"],
  ["0x30", "AHB1ENR", "reg"],
];

const steps = [
  { label: "First member at offset 0", caption: "C guarantees that the first member is at offset 0 and that members appear in declaration order. 'a' takes byte 0." },
  { label: "Alignment inserts padding", caption: "On Cortex-M (AAPCS) a uint32_t must start at a multiple of 4. The compiler inserts 3 unnamed padding bytes so 'b' starts at offset 4." },
  { label: "Tail padding", caption: "'c' (2-byte aligned) goes at 8. Then 2 more padding bytes, so sizeof is 12, a multiple of the struct's alignment (4). In an array s[2], the second element's 'b' must be aligned too. Five of the 12 bytes are padding." },
  { label: "Reorder: largest first", caption: "Same three members, sorted by size: b at 0, c at 4, a at 6, one byte of tail padding. sizeof drops from 12 to 8, without any compiler extension." },
  { label: "packed: no padding, no alignment", caption: "__attribute__((packed)) removes padding and sets the struct's alignment to 1. It matches a wire format byte for byte, but 'b' now starts at offset 1: it straddles two words. The compiler must use unaligned-tolerant accesses (on the M4, a plain LDR usually is), and a pointer to 'b' is dangerous." },
  { label: "Register maps: holes are explicit", caption: "A peripheral struct is all uint32_t, so the compiler never pads it. Gaps in the memory map (RCC 0x1C, 0x28–0x2C, …) are declared as RESERVED members. Forgetting one shifts every later register, just like the struct-overlay bug." },
];

const Byte = ({ kind, i, label }) => (
  <motion.div
    layout
    initial={{ opacity: 0, scale: 0.7 }}
    animate={{ opacity: 1, scale: 1 }}
    transition={{ duration: 0.3, delay: i * 0.03 }}
    className={`flex h-10 w-12 flex-col items-center justify-center rounded border font-mono text-[11px] font-semibold ${MEMBER_TONE[kind]}`}
  >
    <span>{label}</span>
    <span className="text-[9px] font-normal opacity-70">{i}</span>
  </motion.div>
);

const PaddingLayout = () => (
  <Stepper title="Animation · Where padding comes from" steps={steps}>
    {(step) => {
      if (step === 5) {
        return (
          <div className="space-y-2">
            <CodeLine parts={[["uint32_t RESERVED0;   /* 0x1C: no register here */", true]]} />
            {RCC_ROWS.map(([off, name, kind], i) => (
              <motion.div key={off} initial={{ opacity: 0, x: -10 }} animate={{ opacity: 1, x: 0 }} transition={{ delay: i * 0.06 }} className="flex items-center gap-3">
                <span className="w-12 text-right font-mono text-xs text-slate-500">{off}</span>
                <div className={`w-64 rounded border px-2 py-1 font-mono text-xs ${MEMBER_TONE[kind]}`}>{kind === "reg" ? `__IO uint32_t ${name};` : `uint32_t ${name};`}</div>
              </motion.div>
            ))}
            <Verdict ok>offsetof(RCC_TypeDef, AHB1ENR) == 0x30</Verdict>
          </div>
        );
      }
      const f = frames[step];
      const total = 12;
      return (
        <div className="space-y-4">
          <CodeLine parts={[[f.code, true]]} />
          {[0, 1, 2].map((word) => (
            <div key={word} className="flex items-center gap-2">
              <span className="w-16 text-right font-mono text-[10px] text-slate-500">word {word}</span>
              {[0, 1, 2, 3].map((k) => {
                const i = word * 4 + k;
                const kind = i < f.bytes.length ? f.bytes[i] : "empty";
                const label = kind === "pad" ? "pad" : kind === "empty" ? "" : kind;
                const shown = f.misaligned && kind === "b" ? "bad" : kind;
                return i < total ? <Byte key={`${step}-${i}`} kind={shown} i={i} label={label} /> : null;
              })}
            </div>
          ))}
          <div className="flex flex-wrap items-center gap-3">
            {f.size !== null && <Verdict ok={f.size !== 12}>sizeof = {f.size}</Verdict>}
            {f.misaligned && <Verdict ok={false}>b at offset 1: unaligned</Verdict>}
          </div>
        </div>
      );
    }}
  </Stepper>
);

export default PaddingLayout;
