import React, { useState } from "react";
import { motion } from "framer-motion";
import Icon from "../../../components/AppIcon";
import BitRow, { BitRuler, hex, toBits } from "./BitRow";
import ScrollRight from "./ScrollRight";

const OPS = {
  set: { label: "SET", code: (n) => `REG |= (1UL << ${n});`, apply: (r, m) => (r | m) >>> 0, maskRow: (m) => m, maskLabel: "1UL << n" },
  clear: { label: "CLEAR", code: (n) => `REG &= ~(1UL << ${n});`, apply: (r, m) => (r & ~m) >>> 0, maskRow: (m) => ~m >>> 0, maskLabel: "~(1UL << n)" },
  toggle: { label: "TOGGLE", code: (n) => `REG ^= (1UL << ${n});`, apply: (r, m) => (r ^ m) >>> 0, maskRow: (m) => m, maskLabel: "1UL << n" },
  test: { label: "TEST", code: (n) => `bool on = (REG & (1UL << ${n})) != 0U;`, apply: (r, m) => (r & m) >>> 0, maskRow: (m) => m, maskLabel: "1UL << n" },
};

const START_VALUE = 0x000000a0;

/** Interactive: pick an operation and a bit, click bits to edit the register. */
const BitOpsPlayground = () => {
  const [reg, setReg] = useState(START_VALUE);
  const [bit, setBit] = useState(5);
  const [op, setOp] = useState("set");

  const cfg = OPS[op];
  const mask = (2 ** bit) >>> 0; // 1UL << bit without JS's signed 32-bit shift
  const maskRow = cfg.maskRow(mask);
  const result = cfg.apply(reg, mask);
  const isTest = op === "test";
  const changed = (reg ^ result) >>> 0;

  const toggleBit = (index) => setReg((r) => (r ^ (2 ** index)) >>> 0);

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-slate-800 bg-slate-900/80 px-4 py-3">
        <div className="flex items-center gap-2 text-sm font-semibold text-cyan-200">
          <Icon name="ToggleRight" size={16} />
          Interactive · Set, clear, toggle, test
        </div>
        <button type="button" onClick={() => setReg(START_VALUE)} className="inline-flex items-center gap-1 rounded-md px-2 py-1 text-xs text-slate-400 hover:bg-slate-800 hover:text-white">
          <Icon name="RotateCcw" size={13} /> Reset
        </button>
      </div>

      <div className="space-y-4 p-4">
        <div className="flex flex-wrap items-center gap-2">
          {Object.entries(OPS).map(([key, o]) => (
            <button
              key={key}
              type="button"
              onClick={() => setOp(key)}
              className={`rounded-lg px-3 py-1.5 font-mono text-xs font-bold transition ${op === key ? "bg-cyan-400 text-slate-950" : "border border-slate-700 text-slate-300 hover:border-slate-500"}`}
            >
              {o.label}
            </button>
          ))}
          <label className="ml-auto flex items-center gap-2 font-mono text-xs text-slate-300">
            n =
            <input type="range" min="0" max="31" value={bit} onChange={(e) => setBit(Number(e.target.value))} className="w-32 accent-cyan-400" aria-label="Bit number" />
            <span className="w-6 text-cyan-200">{bit}</span>
          </label>
        </div>

        <div className="rounded-lg border border-slate-800 bg-slate-900 px-3 py-2 font-mono text-sm text-cyan-200">{cfg.code(bit)}</div>

        <ScrollRight className="space-y-2 pb-1">
          <BitRuler />
          <BitRow
            bits={toBits(reg, 32)}
            label="REG (click!)"
            caption={hex(reg, 32)}
            idPrefix="reg"
            onBitClick={toggleBit}
            tone={(i, b, idx) => (idx === bit ? "focus" : b ? "base" : "zero")}
          />
          <BitRow bits={toBits(maskRow, 32)} label={cfg.maskLabel} caption={hex(maskRow, 32)} idPrefix="mask" tone={(i, b, idx) => (idx === bit ? "promoted" : b ? "base" : "dim")} />
          <BitRow
            bits={toBits(result, 32)}
            label={isTest ? "REG & mask" : "result"}
            caption={hex(result, 32)}
            idPrefix="res"
            tone={(i, b, idx) => (isTest ? (idx === bit ? (b ? "good" : "bad") : "dim") : (changed >>> idx) & 1 ? "good" : b ? "base" : "zero")}
          />
        </ScrollRight>

        <div className="flex flex-wrap items-center gap-3">
          {isTest ? (
            <motion.span key={`${reg}-${bit}`} initial={{ scale: 0.8 }} animate={{ scale: 1 }} className={`rounded-full px-3 py-1 font-mono text-sm font-bold ${result ? "bg-emerald-500/15 text-emerald-300" : "bg-rose-500/15 text-rose-300"}`}>
              bit {bit} is {result ? "1 → true" : "0 → false"}
            </motion.span>
          ) : (
            <>
              <button type="button" onClick={() => setReg(result)} className="inline-flex items-center gap-1.5 rounded-lg bg-emerald-400 px-3 py-1.5 text-xs font-bold text-slate-950 hover:bg-emerald-300">
                <Icon name="Check" size={13} /> Write result to REG
              </button>
              <span className="text-xs text-slate-400">
                {changed ? `Only bit ${bit} changes. Every other bit is preserved.` : `Bit ${bit} already had that value: nothing changes.`}
              </span>
            </>
          )}
        </div>
        <p className="text-xs leading-5 text-slate-400">
          Every one of these compiles to <b>three</b> instructions on a volatile register: <code className="text-cyan-300">LDR</code>, then{" "}
          <code className="text-cyan-300">ORR / BIC / EOR / TST</code>, then <code className="text-cyan-300">STR</code>. That read-modify-write is not atomic. Keep reading.
        </p>
      </div>
    </figure>
  );
};

export default BitOpsPlayground;
