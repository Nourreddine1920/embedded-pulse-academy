import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* core_cm4.h (CMSIS 5) definitions, C (not C++) variants. */
const ROWS = [
  { q: "__I  / __IM", exp: "volatile const", read: "ok", write: "error" },
  { q: "__O  / __OM", exp: "volatile", read: "silent", write: "ok" },
  { q: "__IO / __IOM", exp: "volatile", read: "ok", write: "ok" },
];

const CELL = {
  ok: ["compiles, real access", "bg-emerald-500/15 text-emerald-200 ring-emerald-400/50"],
  error: ["compile error", "bg-rose-500/20 text-rose-200 ring-rose-400/60"],
  silent: ["compiles! (not checked)", "bg-amber-500/15 text-amber-200 ring-amber-400/60"],
  hidden: ["?", "bg-slate-800 text-slate-500 ring-slate-700"],
};

const frames = [
  { code: "#define __I  volatile const    /* core_cm4.h */", show: 0, focus: -1 },
  { code: "uint32_t id = SCB->CPUID;      /* __IM uint32_t CPUID; */", show: 0, focus: 0, col: "read" },
  { code: "SCB->CPUID = 0U;", show: 1, focus: 0, col: "write", msg: "error: assignment of read-only member 'CPUID'" },
  { code: "#define __O  volatile          /* no 'write-only' in C */", show: 1, focus: 1, col: "read" },
  { code: "GPIOA->ODR |= 0x20U;           /* __IO uint32_t ODR; */", show: 2, focus: 2, col: "both" },
  { code: "GPIOA->IDR = 0U;   /* stm32f446xx.h: __IO uint32_t IDR; */", show: 3, focus: 2, col: "write", st: true },
  { code: "const volatile uint32_t *idcode = (const volatile uint32_t *)0xE0042000UL;", show: 3, focus: 0, col: "read", own: true },
];

const steps = [
  { label: "Three macros", caption: "CMSIS spells register permissions with three macros. They are not new keywords: the preprocessor replaces them with plain C qualifiers. __IM/__OM/__IOM are the same thing for struct members." },
  { label: "__I: read", caption: "__I expands to volatile const. Reading is a normal volatile load. SCB->CPUID in core_cm4.h is declared __IM, so the compiler knows it is read-only." },
  { label: "__I: write", caption: "Writing a const object is a constraint violation, so the compiler refuses: real GCC message shown. This is the only permission C can actually enforce." },
  { label: "__O: write-only?", caption: "C has no 'write-only' qualifier, so __O is just volatile. Reading a write-only register compiles without a warning. On the STM32 it returns 0 (BSRR) or garbage: the macro is documentation, nothing more." },
  { label: "__IO: read/write", caption: "__IO is volatile: reads and writes both allowed, and every one is performed. That's what GPIOA->ODR |= … needs: one LDR, one STR." },
  { label: "What ST actually declares", caption: "Surprise: stm32f446xx.h declares almost every register __IO, even read-only ones like GPIO IDR and DBGMCU IDCODE. GPIOA->IDR = 0 compiles and simply does nothing. The RM's access column, not the header, is the truth." },
  { label: "Enforce it yourself", caption: "In your own drivers, hand out read-only views as pointers to const volatile. The lab's DevInfo_Sources does exactly that: any accidental write through them is a compile error." },
];

const Cell = ({ kind }) => {
  const [text, cls] = CELL[kind];
  return (
    <motion.span key={kind} initial={{ scale: 0.7, opacity: 0 }} animate={{ scale: 1, opacity: 1 }} className={`inline-block rounded px-2 py-1 font-mono text-[11px] ring-1 ${cls}`}>
      {text}
    </motion.span>
  );
};

const IoQualifiers = () => (
  <Stepper title="Animation · __I, __O, __IO: what the compiler checks and what it doesn't" steps={steps}>
    {(step) => {
      const f = frames[step];
      return (
        <div className="max-w-[680px] space-y-3">
          <CodeLine parts={[[f.code, true]]} />
          <table className="w-full border-separate border-spacing-y-1.5 text-left text-xs">
            <thead>
              <tr className="text-slate-500">
                <th className="pr-3 font-semibold">macro</th>
                <th className="pr-3 font-semibold">expands to</th>
                <th className="pr-3 font-semibold">read</th>
                <th className="font-semibold">write</th>
              </tr>
            </thead>
            <tbody>
              {ROWS.map((r, i) => {
                const visible = i < f.show;
                const focused = i === f.focus;
                return (
                  <motion.tr key={r.q} animate={{ opacity: visible || focused ? 1 : 0.35 }}>
                    <td className={`pr-3 font-mono ${focused ? "text-cyan-200" : "text-slate-300"}`}>{r.q}</td>
                    <td className="pr-3 font-mono text-slate-400">{r.exp}</td>
                    <td className="pr-3">
                      <Cell kind={visible || (focused && (f.col === "read" || f.col === "both")) ? r.read : "hidden"} />
                    </td>
                    <td>
                      <Cell kind={visible || (focused && (f.col === "write" || f.col === "both")) ? r.write : "hidden"} />
                    </td>
                  </motion.tr>
                );
              })}
            </tbody>
          </table>
          {f.msg && <p className="font-mono text-xs text-rose-300">{f.msg}</p>}
          {f.st && <Verdict ok={false}>Compiles; the write is ignored by the hardware</Verdict>}
          {f.own && <Verdict ok>Read-only by type: writes will not compile</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default IoQualifiers;
