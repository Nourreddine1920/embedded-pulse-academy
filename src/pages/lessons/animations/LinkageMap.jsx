import React from "react";
import { motion } from "framer-motion";
import Stepper, { Verdict } from "./Stepper";

/*
 * A0.6: linkage as the linker sees it. Symbol tables are the real
 * `arm-none-eabi-nm` letters: uppercase = external (global), lowercase =
 * internal (static), U = undefined here, needed from another object file.
 */
const EC = [
  { l: "T", n: "EventCounter_Record" },
  { l: "T", n: "EventCounter_Get" },
  { l: "R", n: "g_eventCounterVersion" },
  { l: "r", n: "k_names" },
  { l: "d", n: "s_bootMarker" },
  { l: "b", n: "s_counts" },
];
const MAIN = [
  { l: "T", n: "main" },
  { l: "t", n: "button_pressed_edge" },
  { l: "U", n: "EventCounter_Record" },
  { l: "U", n: "EventCounter_Get" },
  { l: "U", n: "g_eventCounterVersion" },
];

const frames = [
  { left: [], right: [], leftName: "event_counter.c", rightName: "main_reg.c" },
  { left: EC, right: [], leftName: "event_counter.o", rightName: "main_reg.c", hl: ["r", "d", "b"] },
  { left: EC, right: MAIN, leftName: "event_counter.o", rightName: "main_reg.o", hl: ["U"] },
  { left: EC, right: MAIN, leftName: "event_counter.o", rightName: "main_reg.o", link: true },
  { left: [{ l: "b", n: "s_count" }, { l: "t", n: "helper" }], right: [{ l: "b", n: "s_count" }, { l: "t", n: "helper" }], leftName: "uart.o", rightName: "spi.o", verdict: { ok: true, text: "two private s_count: no clash" } },
  { left: [{ l: "B", n: "g_errors" }], right: [{ l: "B", n: "g_errors" }], leftName: "uart.o", rightName: "spi.o", clash: "g_errors", verdict: { ok: false, text: "multiple definition of `g_errors'" } },
  { left: [{ l: "B", n: "g_errors" }], right: [{ l: "U", n: "g_errors" }], leftName: "errors.o", rightName: "spi.o", link: true, verdict: { ok: true, text: "one definition, many declarations" } },
];

const steps = [
  { label: "Two translation units", caption: "Each .c file, plus everything it #includes, is compiled on its own into an object file. The compiler never sees the other .c files. The linker joins the objects by symbol NAME, and only the names with external linkage take part." },
  { label: "event_counter.o", caption: "arm-none-eabi-nm shows the symbol table. T/R: external function and const data, visible to every other file. Lowercase r/d/b: file-scope static objects. They have internal linkage: the linker keeps them private to this object file." },
  { label: "main_reg.o", caption: "main_reg.c calls EventCounter_Record(), which it only knows from the prototype in the header, so the symbol is U (undefined here). button_pressed_edge is static: t, private (at -O0; at -Os it's inlined into main and disappears). Nothing in main_reg.o can even name s_counts." },
  { label: "Link", caption: "The linker resolves every U against exactly one external definition. The private symbols (lowercase) are never matched against anything. That is why a static helper can't be called by mistake from another module, and why it can't collide." },
  { label: "static: no collisions", caption: "uart.c and spi.c both have static uint32_t s_count and a static helper(). Two different objects, two different functions, no conflict. Without static they'd be two external definitions of the same name." },
  { label: "Definition in a header", caption: "A header contains uint32_t g_errors; and two .c files include it. That's a DEFINITION in both objects: two B symbols with one name. Since GCC 10 (-fno-common is the default), the link fails: multiple definition of `g_errors'. Older GCCs silently merged them as 'common' symbols." },
  { label: "extern in the header", caption: "The header says extern uint32_t g_errors; (a declaration: 'it exists somewhere'). Exactly one .c file defines it. The other objects get a U, which the linker resolves to the one B. One definition, declared everywhere through the header." },
];

const LETTER_TONE = (l, hl) => {
  const external = l === l.toUpperCase();
  if (hl && hl.includes(l)) return "bg-cyan-400/20 text-cyan-100 ring-1 ring-cyan-400/60";
  if (l === "U") return "bg-amber-500/15 text-amber-200";
  return external ? "bg-slate-800 text-slate-100" : "bg-slate-900 text-slate-400";
};

const ObjectFile = ({ name, symbols, hl, clash, link, side }) => (
  <div className="min-w-[230px] flex-1 rounded-xl border border-slate-800 bg-slate-900/60 p-3">
    <p className="mb-2 font-mono text-xs font-bold text-cyan-300">{name}</p>
    {symbols.length === 0 ? (
      <p className="font-mono text-[11px] italic text-slate-600">source only: not compiled yet</p>
    ) : (
      <ul className="space-y-1">
        {symbols.map((s, i) => {
          const bad = clash && s.n === clash;
          const linked = link && (s.l === "U" || (side === "left" && s.l === s.l.toUpperCase()));
          return (
            <motion.li
              key={`${name}-${s.l}-${s.n}`}
              initial={{ opacity: 0, x: side === "left" ? -12 : 12 }}
              animate={{ opacity: 1, x: 0 }}
              transition={{ delay: i * 0.08 }}
              className={`flex items-center gap-2 rounded px-2 py-1 font-mono text-xs ${bad ? "bg-rose-500/20 ring-1 ring-rose-400/60" : linked ? "bg-emerald-500/10 ring-1 ring-emerald-400/40" : ""}`}
            >
              <span className={`inline-flex h-5 w-5 items-center justify-center rounded text-[11px] font-bold ${LETTER_TONE(s.l, hl)}`}>{s.l}</span>
              <span className={s.l === s.l.toUpperCase() ? "text-slate-100" : "text-slate-400"}>{s.n}</span>
              {s.l !== "U" && s.l === s.l.toLowerCase() && <span className="ml-auto text-[10px] text-slate-600">private</span>}
            </motion.li>
          );
        })}
      </ul>
    )}
  </div>
);

const LinkageMap = () => (
  <Stepper title="Animation · Linkage: what the linker can see" steps={steps}>
    {(step) => {
      const f = frames[step];
      return (
        <div className="max-w-[720px] space-y-4">
          <div className="flex flex-wrap items-stretch gap-3">
            <ObjectFile name={f.leftName} symbols={f.left} hl={f.hl} clash={f.clash} link={f.link} side="left" />
            <div className="flex min-w-[60px] flex-col items-center justify-center font-mono text-[11px] text-slate-500">
              <motion.span animate={{ opacity: f.link || f.clash ? 1 : 0.3 }} className={f.clash ? "text-rose-300" : f.link ? "text-emerald-300" : ""}>
                {f.clash ? "✕ ld" : "⇄ ld"}
              </motion.span>
            </div>
            <ObjectFile name={f.rightName} symbols={f.right} hl={f.hl} clash={f.clash} link={f.link} side="right" />
          </div>
          <p className="font-mono text-[11px] text-slate-500">
            nm letters: T/t code · R/r const data · D/d initialised data · B/b zeroed data · U undefined (needed from elsewhere) · UPPER = external, lower = static
          </p>
          {f.verdict && <Verdict ok={f.verdict.ok}>{f.verdict.text}</Verdict>}
          {step === 3 && <Verdict ok>3 U symbols resolved, 0 private symbols exported</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default LinkageMap;
