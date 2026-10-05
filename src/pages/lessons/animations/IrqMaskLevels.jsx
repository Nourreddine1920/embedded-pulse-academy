import React, { useState } from "react";
import { motion } from "framer-motion";
import Icon from "../../../components/AppIcon";

/*
 * PRIMASK / FAULTMASK / BASEPRI on a part that implements 4 priority bits
 * (STM32F446: __NVIC_PRIO_BITS = 4). Same rule as Irq_IsMasked() in the lab:
 *   PRIMASK or FAULTMASK set -> every configurable exception is blocked
 *   BASEPRI != 0             -> priority byte >= BASEPRI is blocked
 * Level n is stored left-aligned: byte = n << 4. BASEPRI keeps only 4 bits.
 */
const BITS = 4;
const LEVELS = Array.from({ length: 1 << BITS }, (_, i) => i);

const IrqMaskLevels = () => {
  const [primask, setPrimask] = useState(false);
  const [faultmask, setFaultmask] = useState(false);
  const [basepriLevel, setBasepriLevel] = useState(0); // 0 = off, else level 1..15
  const basepri = basepriLevel << (8 - BITS);

  const blocked = (level) => primask || faultmask || (basepri !== 0 && level << (8 - BITS) >= basepri);
  const runCount = LEVELS.filter((l) => !blocked(l)).length;

  const chip = (on, label, set) => (
    <button
      type="button"
      aria-pressed={on}
      onClick={() => set(!on)}
      className={`rounded-full border px-3 py-1 text-xs font-semibold transition ${on ? "border-rose-400 bg-rose-500/80 text-white" : "border-slate-600 bg-slate-900 text-slate-300 hover:border-cyan-400/60"}`}
    >
      {label} = {on ? 1 : 0}
    </button>
  );

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-slate-800 bg-slate-900/80 px-4 py-3">
        <div className="flex items-center gap-2 text-sm font-semibold text-cyan-200">
          <Icon name="Clapperboard" size={16} />
          Animation · Which interrupt priorities can run?
        </div>
        <button
          type="button"
          onClick={() => {
            setPrimask(false);
            setFaultmask(false);
            setBasepriLevel(0);
          }}
          className="rounded-md px-2 py-1 text-xs text-slate-400 hover:bg-slate-800 hover:text-white"
        >
          Reset
        </button>
      </div>

      <div className="space-y-4 px-4 py-5">
        <div className="flex flex-wrap items-center gap-3">
          {chip(primask, "PRIMASK", setPrimask)}
          {chip(faultmask, "FAULTMASK", setFaultmask)}
          <label className="flex items-center gap-2 text-xs text-slate-300">
            BASEPRI level
            <input type="range" min={0} max={15} value={basepriLevel} onChange={(e) => setBasepriLevel(Number(e.target.value))} className="accent-cyan-400" />
            <span className="w-24 font-mono text-cyan-200">
              {basepriLevel === 0 ? "off (0x00)" : `0x${basepri.toString(16).toUpperCase().padStart(2, "0")}`}
            </span>
          </label>
        </div>

        <div className="overflow-x-auto">
          <div className="grid min-w-[560px] grid-cols-16 gap-1" style={{ gridTemplateColumns: "repeat(16, minmax(0, 1fr))" }}>
            {LEVELS.map((l) => {
              const b = blocked(l);
              return (
                <motion.div
                  key={l}
                  animate={{ opacity: b ? 0.45 : 1 }}
                  className={`rounded-md border px-1 py-2 text-center font-mono ${b ? "border-rose-500/60 bg-rose-500/15 text-rose-200" : "border-emerald-400/70 bg-emerald-500/15 text-emerald-200"}`}
                >
                  <p className="text-sm font-bold">{l}</p>
                  <p className="text-[9px] opacity-80">0x{(l << (8 - BITS)).toString(16).toUpperCase().padStart(2, "0")}</p>
                  <p className="text-[10px]">{b ? "blocked" : "runs"}</p>
                </motion.div>
              );
            })}
          </div>
          <div className="mt-1 flex min-w-[560px] justify-between font-mono text-[10px] text-slate-500">
            <span>level 0: most urgent</span>
            <span>level 15: least urgent</span>
          </div>
        </div>

        <p className="text-sm leading-6 text-slate-300">
          {primask || faultmask
            ? "PRIMASK or FAULTMASK blocks every configurable priority level. Only NMI (and HardFault, unless FAULTMASK is set) is still served."
            : basepriLevel === 0
              ? "No mask: every level can preempt thread mode. A lower number always wins."
              : `BASEPRI = 0x${basepri.toString(16).toUpperCase()} blocks levels ${basepriLevel} to 15 (priority byte >= 0x${basepri.toString(16).toUpperCase()}). ${runCount} levels still run. This is how an RTOS keeps its critical sections open to the most urgent interrupts.`}
        </p>
        <p className="text-[11px] leading-5 text-slate-500">
          The F446 implements 4 priority bits, so level n is stored as n &lt;&lt; 4 and BASEPRI ignores its low 4 bits (0x55 acts as 0x50).
        </p>
      </div>
    </figure>
  );
};

export default IrqMaskLevels;
