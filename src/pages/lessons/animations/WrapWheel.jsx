import React, { useEffect, useRef, useState } from "react";
import { motion } from "framer-motion";
import Icon from "../../../components/AppIcon";

const MODULO = 65536;
const TIMEOUT = 10000;
const SIZE = 300;
const C = SIZE / 2;
const R = 100;
const PLAY_TARGET = 20000;
const PLAY_STEP = 120;

const point = (value, radius = R) => {
  const a = (value / MODULO) * 2 * Math.PI;
  return [C + radius * Math.sin(a), C - radius * Math.cos(a)];
};

const arcPath = (from, length) => {
  if (length <= 0) return "";
  const clamped = Math.min(length, MODULO - 1);
  const [x1, y1] = point(from);
  const [x2, y2] = point((from + clamped) % MODULO);
  const large = clamped > MODULO / 2 ? 1 : 0;
  return `M ${x1} ${y1} A ${R} ${R} 0 ${large} 1 ${x2} ${y2}`;
};

/** Interactive 16-bit timer wheel: shows why (uint16_t)(now - start) survives wrap-around. */
const WrapWheel = () => {
  const [start, setStart] = useState(60000);
  const [elapsed, setElapsed] = useState(3000);
  const [playing, setPlaying] = useState(false);
  const frame = useRef(0);

  useEffect(() => {
    if (!playing) return undefined;
    const tick = () => {
      setElapsed((e) => {
        if (e + PLAY_STEP >= PLAY_TARGET) {
          setPlaying(false);
          return PLAY_TARGET;
        }
        return e + PLAY_STEP;
      });
      frame.current = requestAnimationFrame(tick);
    };
    frame.current = requestAnimationFrame(tick);
    return () => cancelAnimationFrame(frame.current);
  }, [playing]);

  const now = (start + elapsed) % MODULO;
  const buggy = now - start; // what C computes in int after promotion
  const fixed = (now - start + MODULO) % MODULO; // (uint16_t)(now - start)
  const wrapped = now < start;
  const [sx, sy] = point(start);
  const [nx, ny] = point(now);

  const play = () => {
    setElapsed(0);
    setPlaying(true);
  };

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-slate-800 bg-slate-900/80 px-4 py-3">
        <div className="flex items-center gap-2 text-sm font-semibold text-cyan-200">
          <Icon name="Timer" size={16} />
          Interactive · A 16-bit timer counter wrapping around
        </div>
        <button type="button" onClick={play} className="inline-flex items-center gap-1.5 rounded-md bg-cyan-400 px-3 py-1.5 text-xs font-bold text-slate-950 hover:bg-cyan-300">
          <Icon name="Play" size={13} /> Run the clock
        </button>
      </div>

      <div className="grid gap-6 p-4 md:grid-cols-[300px_1fr]">
        <svg viewBox={`0 0 ${SIZE} ${SIZE}`} className="mx-auto w-full max-w-[300px]" role="img" aria-label={`Counter wheel: start ${start}, now ${now}`}>
          <circle cx={C} cy={C} r={R} fill="none" stroke="#1e293b" strokeWidth="14" />
          {[0, 16384, 32768, 49152].map((v) => {
            const [x, y] = point(v, R + 22);
            const [x1, y1] = point(v, R - 9);
            const [x2, y2] = point(v, R + 9);
            return (
              <g key={v}>
                <line x1={x1} y1={y1} x2={x2} y2={y2} stroke={v === 0 ? "#f43f5e" : "#475569"} strokeWidth={v === 0 ? 3 : 1.5} />
                <text x={x} y={y + 4} textAnchor="middle" className="fill-slate-400 font-mono" fontSize="11">{v}</text>
              </g>
            );
          })}
          <path d={arcPath(start, elapsed)} fill="none" stroke={wrapped ? "#fbbf24" : "#22d3ee"} strokeWidth="14" strokeLinecap="round" opacity="0.85" />
          <line x1={C} y1={C} x2={sx} y2={sy} stroke="#818cf8" strokeWidth="3" />
          <motion.line x1={C} y1={C} x2={nx} y2={ny} stroke="#22d3ee" strokeWidth="3" />
          <circle cx={sx} cy={sy} r="7" fill="#818cf8" />
          <circle cx={nx} cy={ny} r="7" fill="#22d3ee" />
          <circle cx={C} cy={C} r="4" fill="#94a3b8" />
          <text x={C} y={C + 30} textAnchor="middle" className="fill-slate-500" fontSize="10">wraps 65535 → 0 at the red tick</text>
        </svg>

        <div className="space-y-4 text-sm">
          <label className="block">
            <span className="mb-1 flex justify-between font-mono text-xs text-indigo-300">
              <span>start = {start}</span>
            </span>
            <input type="range" min="0" max="65535" value={start} onChange={(e) => setStart(Number(e.target.value))} className="w-full accent-indigo-400" />
          </label>
          <label className="block">
            <span className="mb-1 flex justify-between font-mono text-xs text-cyan-300">
              <span>real elapsed ticks = {elapsed}</span>
              <span>now = {now}</span>
            </span>
            <input type="range" min="0" max="65535" value={elapsed} onChange={(e) => { setPlaying(false); setElapsed(Number(e.target.value)); }} className="w-full accent-cyan-400" />
          </label>

          <div className="grid gap-3 sm:grid-cols-2">
            <div className={`rounded-xl border p-3 ${buggy >= TIMEOUT ? "border-slate-700" : wrapped ? "border-rose-500/60 bg-rose-500/10" : "border-slate-700"}`}>
              <p className="font-mono text-xs text-slate-400">int delta = now - start</p>
              <p className={`mt-1 font-mono text-2xl font-bold ${buggy < 0 ? "text-rose-300" : "text-slate-100"}`}>{buggy}</p>
              <p className="mt-1 text-xs text-slate-400">timeout {TIMEOUT} hit? <b className={buggy >= TIMEOUT ? "text-emerald-300" : "text-rose-300"}>{buggy >= TIMEOUT ? "yes" : "no"}</b></p>
            </div>
            <div className="rounded-xl border border-emerald-500/40 bg-emerald-500/5 p-3">
              <p className="font-mono text-xs text-slate-400">(uint16_t)(now - start)</p>
              <p className="mt-1 font-mono text-2xl font-bold text-emerald-300">{fixed}</p>
              <p className="mt-1 text-xs text-slate-400">timeout {TIMEOUT} hit? <b className={fixed >= TIMEOUT ? "text-emerald-300" : "text-slate-300"}>{fixed >= TIMEOUT ? "yes" : "no"}</b></p>
            </div>
          </div>

          <p className="rounded-lg bg-slate-900 p-3 text-xs leading-5 text-slate-300">
            {wrapped ? (
              <>
                <b className="text-amber-300">The counter wrapped.</b> Both operands were promoted to <code>int</code>, so the subtraction didn't wrap and came out
                negative. Casting the result back to <code>uint16_t</code> reduces it modulo 65 536, which gives the real elapsed time {fixed}.
              </>
            ) : (
              <>No wrap yet: both formulas agree. Press <b>Run the clock</b> or move <i>start</i> close to 65 535 and watch the int version go wrong.</>
            )}{" "}
            The fixed formula is correct for any interval up to 65 535 ticks.
          </p>
        </div>
      </div>
    </figure>
  );
};

export default WrapWheel;
