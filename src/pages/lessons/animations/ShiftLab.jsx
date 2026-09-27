import React, { useState } from "react";
import Icon from "../../../components/AppIcon";
import BitRow, { BitRuler, toBits } from "./BitRow";
import ScrollRight from "./ScrollRight";

const PROMOTED_WIDTH = 16; // low 16 bits of the 32-bit int are enough to show everything

/** Interactive: << and >> on int8_t / uint8_t, after promotion to int. */
const ShiftLab = () => {
  const [signed, setSigned] = useState(true);
  const [value, setValue] = useState(-100);
  const [amount, setAmount] = useState(2);
  const [dir, setDir] = useState("right");

  const min = signed ? -128 : 0;
  const max = signed ? 127 : 255;
  const v = Math.max(min, Math.min(max, value));
  const result = dir === "left" ? v << amount : v >> amount; // JS int32 == GCC int for these ranges
  const divided = dir === "right" ? Math.trunc(v / 2 ** amount) : v * 2 ** amount;
  const differs = dir === "right" && divided !== result;

  const promotedBits = toBits(v, PROMOTED_WIDTH);
  const resultBits = toBits(result, PROMOTED_WIDTH);
  const fillTone = (i, b, idx) => {
    if (dir === "left" && idx < amount) return "promoted";
    if (dir === "right" && idx >= PROMOTED_WIDTH - amount) return "promoted";
    return b ? "base" : "zero";
  };

  const switchType = (s) => {
    setSigned(s);
    setValue(s ? -100 : 200);
  };

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex items-center gap-2 border-b border-slate-800 bg-slate-900/80 px-4 py-3 text-sm font-semibold text-cyan-200">
        <Icon name="MoveHorizontal" size={16} />
        Interactive · Shifts after promotion
      </div>
      <div className="space-y-4 p-4">
        <div className="flex flex-wrap items-center gap-3 text-xs">
          <div className="flex overflow-hidden rounded-lg border border-slate-700">
            {[true, false].map((s) => (
              <button key={String(s)} type="button" onClick={() => switchType(s)} className={`px-3 py-1.5 font-mono font-bold ${signed === s ? "bg-cyan-400 text-slate-950" : "text-slate-300"}`}>
                {s ? "int8_t" : "uint8_t"}
              </button>
            ))}
          </div>
          <div className="flex overflow-hidden rounded-lg border border-slate-700">
            {["left", "right"].map((d) => (
              <button key={d} type="button" onClick={() => setDir(d)} className={`px-3 py-1.5 font-mono font-bold ${dir === d ? "bg-cyan-400 text-slate-950" : "text-slate-300"}`}>
                {d === "left" ? "x << n" : "x >> n"}
              </button>
            ))}
          </div>
          <label className="flex items-center gap-2 font-mono text-slate-300">
            x = <input type="range" min={min} max={max} value={v} onChange={(e) => setValue(Number(e.target.value))} className="w-28 accent-cyan-400" aria-label="Value" />
            <span className="w-10 text-cyan-200">{v}</span>
          </label>
          <label className="flex items-center gap-2 font-mono text-slate-300">
            n = <input type="range" min="0" max="7" value={amount} onChange={(e) => setAmount(Number(e.target.value))} className="w-20 accent-cyan-400" aria-label="Shift amount" />
            <span className="w-4 text-cyan-200">{amount}</span>
          </label>
        </div>

        <ScrollRight className="space-y-2 pb-1">
          <BitRuler maxWidth={PROMOTED_WIDTH} />
          <BitRow bits={toBits(v, 8)} maxWidth={PROMOTED_WIDTH} label={`x (${signed ? "int8_t" : "uint8_t"})`} caption={String(v)} idPrefix="x" tone={(i, b) => (signed && i === 0 ? (b ? "sign" : "zero") : b ? "base" : "zero")} />
          <BitRow bits={promotedBits} maxWidth={PROMOTED_WIDTH} label="promoted (int)" caption="bits 15..0" idPrefix="p" tone={(i, b, idx) => (idx >= 8 ? (b ? "sign" : "promoted") : b ? "base" : "zero")} />
          <BitRow bits={resultBits} maxWidth={PROMOTED_WIDTH} label={dir === "left" ? `x << ${amount}` : `x >> ${amount}`} caption={String(result)} idPrefix="r" tone={fillTone} />
        </ScrollRight>

        <div className="grid gap-3 text-xs leading-5 text-slate-300 sm:grid-cols-2">
          <p className="rounded-lg bg-slate-900 p-3">
            {dir === "left" ? (
              <>Zeros enter on the right (amber). Because x was promoted to a 32-bit <code>int</code>, nothing falls off the top: 8-bit values never overflow here. They would only truncate if you stored the result back into 8 bits.</>
            ) : signed && v < 0 ? (
              <>The promoted value is negative, so GCC fills with <b>copies of the sign bit</b> (an arithmetic shift, <code>ASRS</code>). The C standard makes this implementation-defined.</>
            ) : (
              <>A non-negative value shifts in <b>zeros</b> (a logical shift, <code>LSRS</code>). This is always well defined.</>
            )}
          </p>
          <p className={`rounded-lg p-3 ${differs ? "bg-rose-500/10 text-rose-200" : "bg-slate-900"}`}>
            {dir === "right" ? (
              <>
                x / {2 ** amount} = <b>{divided}</b> but x &gt;&gt; {amount} = <b>{result}</b>.{" "}
                {differs ? "Different! Division truncates toward zero; the shift rounds toward −∞. GCC adds a fix-up before ASRS when you write /." : "Same here."}
              </>
            ) : (
              <>
                x × {2 ** amount} = <b>{divided}</b>. A left shift by n multiplies by 2ⁿ, but for negative values it is undefined behaviour in C. Shift unsigned values only.
              </>
            )}
          </p>
        </div>
      </div>
    </figure>
  );
};

export default ShiftLab;
