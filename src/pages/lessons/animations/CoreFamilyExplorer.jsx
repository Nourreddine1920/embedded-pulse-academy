import React, { useState } from "react";
import { motion } from "framer-motion";
import Icon from "../../../components/AppIcon";

/*
 * Which STM32 series use which Cortex-M core, filtered by what you need.
 * Core capabilities: Arm Technical Reference Manuals. Series data: ST product
 * pages (maximum CPU frequency of the family; individual parts are lower).
 * The FPU / DP flags describe the series as shipped, not the core's options.
 *   fpu: 0 none, 1 single precision, 2 single + double ("some" parts: 1.5)
 */
const NEEDS = [
  { id: "div", label: "Hardware divide" },
  { id: "dsp", label: "DSP / SIMD" },
  { id: "fpu", label: "FPU (single)" },
  { id: "dp", label: "FPU (double)" },
  { id: "tz", label: "TrustZone" },
  { id: "bb", label: "Bit-banding" },
];

const SERIES = [
  { s: "F0", core: "M0", mhz: 48, div: 0, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 0 },
  { s: "C0", core: "M0+", mhz: 48, div: 0, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 0 },
  { s: "L0", core: "M0+", mhz: 32, div: 0, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 0 },
  { s: "G0", core: "M0+", mhz: 64, div: 0, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 0 },
  { s: "F1", core: "M3", mhz: 72, div: 1, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 1 },
  { s: "F2", core: "M3", mhz: 120, div: 1, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 1 },
  { s: "L1", core: "M3", mhz: 32, div: 1, dsp: 0, fpu: 0, dp: 0, tz: 0, bb: 1 },
  { s: "F3", core: "M4F", mhz: 72, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 0, bb: 1 },
  { s: "F4", core: "M4F", mhz: 180, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 0, bb: 1, me: true },
  { s: "G4", core: "M4F", mhz: 170, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 0, bb: 1 },
  { s: "L4", core: "M4F", mhz: 80, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 0, bb: 1 },
  { s: "F7", core: "M7", mhz: 216, div: 1, dsp: 1, fpu: 1, dp: 0.5, tz: 0, bb: 0 },
  { s: "H7", core: "M7", mhz: 480, div: 1, dsp: 1, fpu: 1, dp: 1, tz: 0, bb: 0 },
  { s: "L5", core: "M33", mhz: 110, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 1, bb: 0 },
  { s: "U5", core: "M33", mhz: 160, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 1, bb: 0 },
  { s: "H5", core: "M33", mhz: 250, div: 1, dsp: 1, fpu: 1, dp: 0, tz: 1, bb: 0 },
];

const CORE_TONE = {
  M0: "border-slate-500 bg-slate-800 text-slate-200",
  "M0+": "border-slate-500 bg-slate-800 text-slate-200",
  M3: "border-sky-400/70 bg-sky-500/15 text-sky-100",
  M4F: "border-cyan-400/80 bg-cyan-500/15 text-cyan-100",
  M7: "border-fuchsia-400/70 bg-fuchsia-500/15 text-fuchsia-100",
  M33: "border-emerald-400/70 bg-emerald-500/15 text-emerald-100",
};

const matches = (row, wanted) => wanted.every((id) => row[id] > 0);

const CoreFamilyExplorer = () => {
  const [wanted, setWanted] = useState([]);
  const toggle = (id) => setWanted((w) => (w.includes(id) ? w.filter((x) => x !== id) : [...w, id]));
  const shown = SERIES.filter((r) => matches(r, wanted));

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-slate-800 bg-slate-900/80 px-4 py-3">
        <div className="flex items-center gap-2 text-sm font-semibold text-cyan-200">
          <Icon name="Clapperboard" size={16} />
          Animation · Which STM32 series has the core you need?
        </div>
        <button type="button" onClick={() => setWanted([])} className="rounded-md px-2 py-1 text-xs text-slate-400 hover:bg-slate-800 hover:text-white">
          Clear filters
        </button>
      </div>

      <div className="space-y-4 px-4 py-5">
        <div className="flex flex-wrap gap-2">
          {NEEDS.map((n) => {
            const on = wanted.includes(n.id);
            return (
              <button
                key={n.id}
                type="button"
                aria-pressed={on}
                onClick={() => toggle(n.id)}
                className={`rounded-full border px-3 py-1 text-xs font-semibold transition ${on ? "border-cyan-400 bg-cyan-400 text-slate-950" : "border-slate-600 bg-slate-900 text-slate-300 hover:border-cyan-400/60"}`}
              >
                {n.label}
              </button>
            );
          })}
        </div>

        <div className="grid grid-cols-2 gap-2 sm:grid-cols-4">
          {SERIES.map((r) => {
            const ok = matches(r, wanted);
            return (
              <motion.div
                key={r.s}
                animate={{ opacity: ok ? 1 : 0.22, scale: ok && wanted.length > 0 ? 1.02 : 1 }}
                transition={{ duration: 0.25 }}
                className={`rounded-lg border px-3 py-2 ${CORE_TONE[r.core]} ${r.me ? "ring-2 ring-amber-300/80" : ""}`}
              >
                <p className="flex items-baseline justify-between font-mono text-sm font-bold">
                  <span>STM32{r.s}</span>
                  <span className="text-[10px] font-normal opacity-80">{r.mhz} MHz max</span>
                </p>
                <p className="text-xs font-semibold">Cortex-{r.core}</p>
                <p className="mt-1 font-mono text-[10px] opacity-80">
                  {r.dp === 0.5 ? "FPU: SP, DP on some parts" : r.dp === 1 ? "FPU: SP + DP" : r.fpu ? "FPU: SP" : "no FPU"}
                  {r.tz ? " · TrustZone" : ""}
                </p>
              </motion.div>
            );
          })}
        </div>

        <p className="text-sm leading-6 text-slate-300">
          {wanted.length === 0
            ? "Pick requirements to dim the series that can't meet them. The amber ring marks the F4, the family of the NUCLEO-F446RE."
            : shown.length === 0
              ? "No series in this list has all of those."
              : `${shown.length} of ${SERIES.length} series match: ${shown.map((r) => r.s).join(", ")}.`}
        </p>
        {wanted.includes("dp") && (
          <p className="text-xs leading-5 text-amber-200">Double-precision FPU is optional per part even inside a series (and the F7 varies by part number): always check the datasheet of the exact device.</p>
        )}
        <p className="text-[11px] leading-5 text-slate-500">
          Core capabilities from the Arm TRMs; maximum frequency is the family's top part and most devices are slower. Verify against ST's product selector before choosing a part.
        </p>
      </div>
    </figure>
  );
};

export default CoreFamilyExplorer;
