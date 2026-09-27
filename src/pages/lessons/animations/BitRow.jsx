import React from "react";
import { motion } from "framer-motion";

/** Tone → cell classes. Kept in one place so every animation matches. */
const TONES = {
  base: "border-slate-600 bg-slate-800 text-slate-100",
  zero: "border-slate-700 bg-slate-900 text-slate-500",
  promoted: "border-amber-400/70 bg-amber-500/15 text-amber-200",
  sign: "border-fuchsia-400/70 bg-fuchsia-500/15 text-fuchsia-200",
  good: "border-emerald-400/70 bg-emerald-500/15 text-emerald-200",
  bad: "border-rose-400/80 bg-rose-500/20 text-rose-200",
  dim: "border-slate-800 bg-slate-950 text-slate-700",
  focus: "border-cyan-400/80 bg-cyan-500/15 text-cyan-100",
};

/** Converts a BigInt/number to an MSB-first array of 0/1 of the given width. */
export const toBits = (value, width) => {
  const mask = (1n << BigInt(width)) - 1n;
  const v = BigInt(value) & mask;
  return Array.from({ length: width }, (_, i) => Number((v >> BigInt(width - 1 - i)) & 1n));
};

/** Reads an MSB-first bit array back into an unsigned BigInt. */
export const fromBits = (bits) => bits.reduce((acc, b) => (acc << 1n) | BigInt(b), 0n);

/** Interprets a bit array as two's complement. */
export const toSigned = (bits) => {
  const u = fromBits(bits);
  return bits[0] ? u - (1n << BigInt(bits.length)) : u;
};

export const hex = (value, width) =>
  "0x" + (BigInt(value) & ((1n << BigInt(width)) - 1n)).toString(16).toUpperCase().padStart(width / 4, "0");

/**
 * One row of bits, MSB on the left, grouped in nibbles.
 * @param bits      MSB-first array of 0/1
 * @param tone      (index, bit) => key of TONES (index 0 = MSB)
 * @param label     text on the left
 * @param caption   text on the right (value / hex)
 * @param align     "right" keeps the LSB column fixed when widths change
 */
const BitRow = ({ bits, tone, label, caption, maxWidth = 32, align = "right", idPrefix = "b", onBitClick }) => {
  const width = bits.length;
  const pad = align === "right" ? maxWidth - width : 0;

  return (
    <div className="flex min-w-max items-center gap-3">
      {(label || caption) && (
        <div className="sticky left-0 z-10 w-24 shrink-0 bg-slate-950 py-0.5 pr-1 text-right leading-tight sm:w-28">
          {label && <div className="font-mono text-xs text-slate-400">{label}</div>}
          {caption && <div className="mt-0.5 font-mono text-[11px] font-semibold text-slate-200">{caption}</div>}
        </div>
      )}
      <div className="flex items-center">
        {Array.from({ length: pad }, (_, i) => (
          <span key={`pad-${i}`} className={`h-6 w-[15px] sm:h-7 sm:w-4 ${(i + 1) % 4 === 0 ? "mr-1.5" : "mr-0.5"}`} />
        ))}
        {bits.map((bit, i) => {
          const column = pad + i; // stable column index -> stable animation key
          const bitIndex = width - 1 - i;
          const t = tone ? tone(i, bit, bitIndex) : bit ? "base" : "zero";
          const Cell = onBitClick ? motion.button : motion.span;
          return (
            <Cell
              key={`${idPrefix}-${column}`}
              {...(onBitClick ? { type: "button", onClick: () => onBitClick(bitIndex), whileTap: { scale: 0.85 }, "aria-label": `Toggle bit ${bitIndex}` } : {})}
              layout
              initial={{ opacity: 0, y: -8 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.45, ease: "easeOut" }}
              title={`bit ${bitIndex}`}
              className={`flex h-6 w-[15px] items-center justify-center rounded border font-mono text-[10px] font-semibold sm:h-7 sm:w-4 sm:text-[11px] ${onBitClick ? "cursor-pointer hover:border-cyan-300" : ""} ${TONES[t] || TONES.base} ${(column + 1) % 4 === 0 ? "mr-1.5" : "mr-0.5"}`}
            >
              <motion.span key={`${column}-${bit}`} initial={{ scale: 0.3 }} animate={{ scale: 1 }} transition={{ duration: 0.3 }}>
                {bit}
              </motion.span>
            </Cell>
          );
        })}
      </div>
    </div>
  );
};

/** Bit-index ruler aligned with BitRow (31 … 0). */
export const BitRuler = ({ maxWidth = 32, labelWidth = true }) => (
  <div className="flex min-w-max items-center gap-3">
    {labelWidth && <div className="sticky left-0 z-10 w-24 shrink-0 self-stretch bg-slate-950 sm:w-28" />}
    <div className="flex">
      {Array.from({ length: maxWidth }, (_, i) => {
        const bitIndex = maxWidth - 1 - i;
        const show = bitIndex % 8 === 7 || bitIndex === 0;
        return (
          <span
            key={i}
            className={`w-[15px] text-center font-mono text-[9px] text-slate-500 sm:w-4 ${(i + 1) % 4 === 0 ? "mr-1.5" : "mr-0.5"}`}
          >
            {show ? bitIndex : ""}
          </span>
        );
      })}
    </div>
  </div>
);

export default BitRow;
