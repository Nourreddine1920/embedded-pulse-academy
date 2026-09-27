import React from "react";
import { motion } from "framer-motion";
import BitRow, { toBits } from "./BitRow";
import Stepper, { CodeLine, Verdict } from "./Stepper";

const steps = [
  { label: "Two variables", caption: "t is a signed 32-bit −1. u is an unsigned 32-bit 1. Any human says −1 < 1." },
  {
    label: "Bit pattern of −1",
    caption: "In two's complement, −1 is all ones: 0xFFFFFFFF. The bits don't record whether the value is signed. The type does.",
  },
  {
    label: "Usual arithmetic conversions",
    caption:
      "int32_t and uint32_t have the same rank, and one of them is unsigned, so the signed operand is converted to uint32_t. The bits stay the same, but they now mean 4 294 967 295.",
  },
  { label: "Compare", caption: "The CPU compares 4 294 967 295 < 1 with an unsigned compare (CMP + CS/CC condition codes). Result: false." },
  {
    label: "The fix",
    caption: "Handle the negative case first: (t < 0) || ((uint32_t)t < u). Or keep both operands the same signedness by design. -Wsign-compare (in -Wextra) catches this.",
  },
];

/** Horizontal number line; `pos` in [0,1]. */
const NumberLine = ({ pos, label, tone }) => (
  <div className="relative mx-2 mt-10 h-14 min-w-[280px]">
    <div className="absolute left-0 right-0 top-6 h-0.5 bg-slate-600" />
    <span className="absolute left-0 top-9 -translate-x-1/2 font-mono text-[10px] text-slate-400">0</span>
    <span className="absolute left-[2%] top-9 font-mono text-[10px] text-cyan-300">1 (u)</span>
    <span className="absolute right-0 top-9 translate-x-1/4 font-mono text-[10px] text-slate-400">4 294 967 295</span>
    <motion.div className="absolute top-0" animate={{ left: `${pos * 100}%` }} transition={{ type: "spring", stiffness: 60, damping: 14 }}>
      <div className={`-translate-x-1/2 whitespace-nowrap rounded-md px-2 py-0.5 font-mono text-xs font-bold ${tone}`}>{label}</div>
      <div className="mx-auto h-3 w-0.5 -translate-x-1/2 bg-current" />
    </motion.div>
  </div>
);

const SignedUnsignedCompare = () => (
  <Stepper title="Animation · Why (−1 < 1u) is false" steps={steps}>
    {(step) => {
      const allOnes = toBits(0xffffffff, 32);
      return (
        <div className="space-y-3">
          <CodeLine
            parts={[
              ["int32_t t = -1;  uint32_t u = 1;  ", step <= 1],
              ["if (", false],
              [step === 4 ? "(t < 0) || ((uint32_t)t < u)" : "t < u", step >= 2],
              [")", false],
            ]}
          />
          {step >= 1 && (
            <BitRow
              bits={allOnes}
              label={step >= 2 ? "t as uint32_t" : "t  (int32_t)"}
              caption={step >= 2 ? "= 4 294 967 295" : "= −1"}
              tone={() => (step >= 2 ? "bad" : "sign")}
              idPrefix="t"
            />
          )}
          <BitRow bits={toBits(1, 32)} label="u  (uint32_t)" caption="= 1" idPrefix="u" tone={(i, b) => (b ? "focus" : "zero")} />
          <div className="sticky left-0 max-w-[calc(100vw-4rem)] overflow-hidden pt-2 sm:max-w-none">
            {step < 2 ? (
              <div className="font-mono text-xs text-slate-400">Signed view: t = −1 sits left of 0, so t &lt; u, as you'd expect.</div>
            ) : (
              <NumberLine pos={step === 4 ? 0 : 1} label={step === 4 ? "t < 0 → handled first" : "t → 4 294 967 295"} tone={step === 4 ? "bg-emerald-500/20 text-emerald-300" : "bg-rose-500/20 text-rose-300"} />
            )}
          </div>
          {step === 3 && <div className="sticky left-0 w-max"><Verdict ok={false}>t &lt; u → false</Verdict></div>}
          {step === 4 && <div className="sticky left-0 w-max"><Verdict ok>(t &lt; 0) || ((uint32_t)t &lt; u) → true</Verdict></div>}
        </div>
      );
    }}
  </Stepper>
);

export default SignedUnsignedCompare;
