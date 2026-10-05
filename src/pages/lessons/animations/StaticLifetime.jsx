import React from "react";
import { AnimatePresence, motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/*
 * A0.6: automatic vs static local lifetime, then the reentrancy cost of static.
 * Left: the stack (frames come and go). Right: .bss/.data (objects live forever).
 */
const frames = [
  { code: "uint32_t calls = 0U;   vs   static uint32_t calls = 0U;", stack: [], statics: [{ name: "count_calls_static::calls", value: "0", note: ".bss, zeroed by startup before main()" }], out: [] },
  { code: "a[0] = count_calls_auto();", stack: [{ name: "calls", value: "0 → 1", fn: "count_calls_auto" }], statics: [{ name: "count_calls_static::calls", value: "0" }], out: ["auto: 1"] },
  { code: "a[1] = count_calls_auto();", stack: [{ name: "calls", value: "0 → 1", fn: "count_calls_auto" }], statics: [{ name: "count_calls_static::calls", value: "0" }], out: ["auto: 1 1"] },
  { code: "s[0] = count_calls_static();", stack: [], statics: [{ name: "count_calls_static::calls", value: "0 → 1", hot: true }], out: ["auto: 1 1", "static: 1"] },
  { code: "s[1] = count_calls_static();  s[2] = count_calls_static();", stack: [], statics: [{ name: "count_calls_static::calls", value: "1 → 2 → 3", hot: true }], out: ["auto: 1 1 1", "static: 1 2 3"], ok: true },
  { code: "printf(\"%s %s\", hex_static(0xCAFE), hex_static(0xBEEF));", stack: [], statics: [{ name: "hex_static::text[11]", value: "\"0xCAFE\"", hot: true, note: "first call fills the ONE buffer" }], out: ["pointer #1 → hex_static::text"] },
  { code: "printf(\"%s %s\", hex_static(0xCAFE), hex_static(0xBEEF));", stack: [], statics: [{ name: "hex_static::text[11]", value: "\"0xBEEF\"", hot: true, bad: true, note: "second call overwrites it" }], out: ["pointer #1 → hex_static::text", "pointer #2 → hex_static::text", "printed: 0xBEEF 0xBEEF"], ok: false },
  { code: "hex_to(first, sizeof first, 0xCAFE), hex_to(second, sizeof second, 0xBEEF)", stack: [{ name: "first[11]", value: "\"0xCAFE\"", fn: "caller" }, { name: "second[11]", value: "\"0xBEEF\"", fn: "caller" }], statics: [], out: ["printed: 0xCAFE 0xBEEF"], ok: true },
];

const steps = [
  { label: "Two counters", caption: "Same code, one keyword different. The automatic calls lives in the stack frame of each call. The static calls is one object for the whole run, placed in .bss (zero-initialised) and cleared once by the startup code, before main() runs." },
  { label: "auto: call 1", caption: "Each call pushes a fresh frame. calls is created, set to 0, incremented to 1, returned, and destroyed when the frame is popped." },
  { label: "auto: call 2", caption: "A brand-new calls, set to 0 again. An automatic variable remembers nothing between calls: the result is 1 every time." },
  { label: "static: call 1", caption: "The static calls isn't on the stack. The '= 0U' is NOT executed on each call: it's the initial value, applied once at boot. The call only increments it: 1." },
  { label: "static: calls 2, 3", caption: "The value persists: 2, then 3. That's what a function-local static is for: edge detectors, 'first call' flags, counters. The name is still visible only inside the function." },
  { label: "The price: one buffer", caption: "hex_static() formats into a static char buffer and returns a pointer to it. The first call writes \"0xCAFE\" into that single buffer and returns its address." },
  { label: "Second call overwrites", caption: "The second call writes \"0xBEEF\" into the SAME buffer and returns the same address. printf receives two identical pointers. On Cortex-M4 (left-to-right) it prints 0xBEEF twice; on the PC build it prints 0xCAFE twice: the argument order is unspecified. An ISR calling it would corrupt main's string the same way." },
  { label: "Fix: caller's buffer", caption: "Let the caller provide the storage (hex_to(buf, size, v)). Each buffer is automatic, in the caller's frame, so every call and every context has its own. The function is now reentrant: safe from ISRs and from two RTOS tasks." },
];

const Cell = ({ item, tone }) => (
  <motion.div
    layout
    initial={{ opacity: 0, x: tone === "stack" ? -16 : 16 }}
    animate={{ opacity: 1, x: 0 }}
    exit={{ opacity: 0, scale: 0.9 }}
    transition={{ duration: 0.35 }}
    className={`rounded-lg border px-3 py-2 font-mono text-xs ${item.bad ? "border-rose-400/60 bg-rose-500/10" : item.hot ? "border-cyan-400/60 bg-cyan-500/10" : "border-slate-700 bg-slate-900"}`}
  >
    {item.fn && <p className="text-[10px] uppercase tracking-wider text-slate-500">frame: {item.fn}()</p>}
    <p className="text-slate-200">
      {item.name} = <span className={item.bad ? "text-rose-200" : "text-cyan-200"}>{item.value}</span>
    </p>
    {item.note && <p className="mt-0.5 text-[10px] text-slate-500">{item.note}</p>}
  </motion.div>
);

const Column = ({ title, sub, items, tone }) => (
  <div className="min-w-[230px] flex-1 rounded-xl border border-slate-800 bg-slate-900/50 p-3">
    <p className="text-xs font-bold uppercase tracking-[0.14em] text-cyan-300">{title}</p>
    <p className="mb-2 font-mono text-[11px] text-slate-500">{sub}</p>
    <div className="min-h-[64px] space-y-1.5">
      <AnimatePresence mode="popLayout">
        {items.length === 0 ? (
          <motion.p key="empty" initial={{ opacity: 0 }} animate={{ opacity: 1 }} exit={{ opacity: 0 }} className="font-mono text-[11px] italic text-slate-600">
            (nothing)
          </motion.p>
        ) : (
          items.map((it) => <Cell key={`${it.name}-${it.value}`} item={it} tone={tone} />)
        )}
      </AnimatePresence>
    </div>
  </div>
);

const StaticLifetime = () => (
  <Stepper title="Animation · static: lives forever, shared by every caller" steps={steps}>
    {(step) => {
      const f = frames[step];
      return (
        <div className="max-w-[720px] space-y-4">
          <CodeLine parts={[[f.code, true]]} />
          <div className="flex flex-wrap gap-3">
            <Column title="Stack (automatic)" sub="created per call, gone on return" items={f.stack} tone="stack" />
            <Column title=".bss / .data (static)" sub="one object, lives from reset to power-off" items={f.statics} tone="static" />
          </div>
          <div className="rounded-lg border border-slate-800 bg-black/40 px-3 py-2 font-mono text-xs text-emerald-200">
            {f.out.length === 0 ? <span className="text-slate-600">console</span> : f.out.map((l) => <p key={l}>{l}</p>)}
          </div>
          {f.ok === true && <Verdict ok>{step === 4 ? "static keeps its value between calls" : "reentrant: each caller owns its buffer"}</Verdict>}
          {f.ok === false && <Verdict ok={false}>not reentrant: both arguments point at one buffer</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default StaticLifetime;
