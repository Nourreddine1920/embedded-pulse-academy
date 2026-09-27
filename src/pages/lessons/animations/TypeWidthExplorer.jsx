import React, { useMemo, useState } from "react";
import { motion } from "framer-motion";
import Icon from "../../../components/AppIcon";
import ScrollRight from "./ScrollRight";
import BitRow, { BitRuler, hex, toBits, toSigned, fromBits } from "./BitRow";

const TYPES = [
  { name: "uint8_t", width: 8, signed: false },
  { name: "int8_t", width: 8, signed: true },
  { name: "uint16_t", width: 16, signed: false },
  { name: "int16_t", width: 16, signed: true },
  { name: "uint32_t", width: 32, signed: false },
  { name: "int32_t", width: 32, signed: true },
];

const PRESETS = ["200", "300", "-1", "-200", "65536", "0x80000000"];

const parse = (text) => {
  const t = text.trim().replace(/_/g, "");
  if (/^-?0x[0-9a-f]+$/i.test(t)) {
    const neg = t.startsWith("-");
    const v = BigInt(neg ? t.slice(1) : t);
    return neg ? -v : v;
  }
  if (/^-?\d+$/.test(t)) return BigInt(t);
  return null;
};

/** Interactive: store one value in every fixed-width type and see what survives. */
const TypeWidthExplorer = () => {
  const [text, setText] = useState("300");
  const value = parse(text);

  const rows = useMemo(() => {
    if (value === null) return [];
    return TYPES.map((t) => {
      const bits = toBits(value, t.width);
      const stored = t.signed ? toSigned(bits) : fromBits(bits);
      return { ...t, bits, stored, fits: stored === value };
    });
  }, [value]);

  const u32Bytes = value === null ? [] : toBits(value, 32).reduce((acc, bit, i) => {
    const byte = Math.floor(i / 8);
    acc[byte] = (acc[byte] << 1) | bit;
    return acc;
  }, [0, 0, 0, 0]).reverse(); // little-endian: LSB byte at the lowest address

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex flex-wrap items-center gap-3 border-b border-slate-800 bg-slate-900/80 px-4 py-3">
        <div className="flex items-center gap-2 text-sm font-semibold text-cyan-200">
          <Icon name="Binary" size={16} />
          Interactive · One value, six types
        </div>
      </div>

      <div className="space-y-4 p-4">
        <div className="flex flex-wrap items-center gap-2">
          <label htmlFor="twe-input" className="font-mono text-sm text-slate-300">value =</label>
          <input
            id="twe-input"
            value={text}
            onChange={(e) => setText(e.target.value)}
            spellCheck="false"
            className={`w-40 rounded-lg border bg-slate-900 px-3 py-1.5 font-mono text-sm text-white outline-none focus:ring-2 focus:ring-cyan-400 ${value === null ? "border-rose-500" : "border-slate-700"}`}
          />
          {PRESETS.map((p) => (
            <button key={p} type="button" onClick={() => setText(p)} className={`rounded-full border px-2.5 py-1 font-mono text-xs transition ${text === p ? "border-cyan-400 bg-cyan-400/15 text-cyan-200" : "border-slate-700 text-slate-400 hover:border-slate-500 hover:text-slate-200"}`}>
              {p}
            </button>
          ))}
        </div>

        {value === null ? (
          <p className="text-sm text-rose-300">Type a decimal (e.g. -200) or hex (e.g. 0xFF38) integer.</p>
        ) : (
          <ScrollRight className="space-y-2 pb-2">
            <BitRuler />
            {rows.map((r) => (
              <BitRow
                key={r.name}
                bits={r.bits}
                label={r.name}
                caption={
                  <motion.span key={`${r.name}-${r.stored}`} initial={{ opacity: 0, x: -6 }} animate={{ opacity: 1, x: 0 }} className={r.fits ? "text-emerald-300" : "text-rose-300"}>
                    {r.fits ? "✓ " : "✗ → "}
                    {r.stored.toString()}
                  </motion.span>
                }
                idPrefix={r.name}
                tone={(i, bit) => (r.signed && i === 0 ? (bit ? "sign" : "zero") : bit ? (r.fits ? "base" : "bad") : "zero")}
              />
            ))}
          </ScrollRight>
        )}

        {value !== null && (
          <div className="rounded-xl border border-slate-800 bg-slate-900/60 p-3">
            <p className="mb-2 text-xs text-slate-400">
              How the <code className="text-cyan-300">uint32_t</code> ({hex(value, 32)}) sits in SRAM. Cortex-M is <b>little-endian</b>, so the least significant byte is at the lowest address:
            </p>
            <div className="flex flex-wrap gap-2">
              {u32Bytes.map((b, i) => (
                <motion.div key={`${i}-${b}`} initial={{ y: -6, opacity: 0 }} animate={{ y: 0, opacity: 1 }} transition={{ delay: i * 0.06 }} className="rounded-lg border border-slate-700 bg-slate-950 px-3 py-2 text-center">
                  <div className="font-mono text-[10px] text-slate-500">0x2000000{i}</div>
                  <div className="font-mono text-sm font-bold text-slate-100">{b.toString(16).toUpperCase().padStart(2, "0")}</div>
                </motion.div>
              ))}
            </div>
          </div>
        )}
        <p className="text-xs leading-5 text-slate-400">
          Converting to an <b>unsigned</b> type is always defined: the value is reduced modulo 2<sup>N</sup>. Converting an out-of-range value to a <b>signed</b> type
          is implementation-defined; GCC also reduces modulo 2<sup>N</sup>, which is what you see here. The pink cell is the sign bit.
        </p>
      </div>
    </figure>
  );
};

export default TypeWidthExplorer;
