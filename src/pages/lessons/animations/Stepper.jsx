import React, { useEffect, useState } from "react";
import { AnimatePresence, motion } from "framer-motion";
import Icon from "../../../components/AppIcon";
import ScrollRight from "./ScrollRight";

const AUTOPLAY_MS = 2600;

/**
 * Step-through player for lesson animations.
 * @param title  heading of the widget
 * @param steps  [{ label, caption }] (caption may be a React node)
 * @param children (stepIndex) => visual for that step
 */
const Stepper = ({ title, steps, children }) => {
  const [step, setStep] = useState(0);
  const [playing, setPlaying] = useState(false);
  const last = steps.length - 1;

  useEffect(() => {
    if (!playing) return undefined;
    if (step >= last) {
      setPlaying(false);
      return undefined;
    }
    const id = window.setTimeout(() => setStep((s) => Math.min(s + 1, last)), AUTOPLAY_MS);
    return () => window.clearTimeout(id);
  }, [playing, step, last]);

  const go = (next) => {
    setPlaying(false);
    setStep(Math.max(0, Math.min(last, next)));
  };

  const togglePlay = () => {
    if (step >= last) setStep(0);
    setPlaying((p) => !p);
  };

  return (
    <figure className="not-prose my-8 overflow-hidden rounded-2xl border border-cyan-500/25 bg-slate-950 shadow-[0_0_40px_-20px_rgba(34,211,238,0.5)]">
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-slate-800 bg-slate-900/80 px-4 py-3">
        <div className="flex items-center gap-2 text-sm font-semibold text-cyan-200">
          <Icon name="Clapperboard" size={16} />
          {title}
        </div>
        <div className="flex items-center gap-1.5">
          <button type="button" onClick={() => go(0)} className="rounded-md p-1.5 text-slate-400 hover:bg-slate-800 hover:text-white" aria-label="Restart">
            <Icon name="RotateCcw" size={15} />
          </button>
          <button type="button" onClick={() => go(step - 1)} disabled={step === 0} className="rounded-md p-1.5 text-slate-300 hover:bg-slate-800 disabled:opacity-30" aria-label="Previous step">
            <Icon name="ChevronLeft" size={17} />
          </button>
          <button type="button" onClick={togglePlay} className="inline-flex items-center gap-1.5 rounded-md bg-cyan-400 px-3 py-1.5 text-xs font-bold text-slate-950 hover:bg-cyan-300">
            <Icon name={playing ? "Pause" : "Play"} size={13} />
            {playing ? "Pause" : step >= last ? "Replay" : "Play"}
          </button>
          <button type="button" onClick={() => go(step + 1)} disabled={step === last} className="rounded-md p-1.5 text-slate-300 hover:bg-slate-800 disabled:opacity-30" aria-label="Next step">
            <Icon name="ChevronRight" size={17} />
          </button>
        </div>
      </div>

      <ScrollRight className="px-4 py-6">
        <div className="w-max min-w-full">{children(step)}</div>
      </ScrollRight>

      <div className="border-t border-slate-800 bg-slate-900/60 px-4 py-3">
        <div className="mb-2 flex gap-1.5">
          {steps.map((s, i) => (
            <button
              key={s.label}
              type="button"
              onClick={() => go(i)}
              className={`h-1.5 flex-1 rounded-full transition ${i <= step ? "bg-cyan-400" : "bg-slate-700 hover:bg-slate-600"}`}
              aria-label={`Step ${i + 1}: ${s.label}`}
            />
          ))}
        </div>
        <AnimatePresence mode="wait">
          <motion.div key={step} initial={{ opacity: 0, y: 6 }} animate={{ opacity: 1, y: 0 }} exit={{ opacity: 0, y: -6 }} transition={{ duration: 0.25 }}>
            <p className="text-xs font-semibold uppercase tracking-[0.14em] text-cyan-300">
              Step {step + 1}/{steps.length} · {steps[step].label}
            </p>
            <p className="mt-1 text-sm leading-6 text-slate-300">{steps[step].caption}</p>
          </motion.div>
        </AnimatePresence>
      </div>
    </figure>
  );
};

/** A line of C where one fragment is highlighted for the current step. */
export const CodeLine = ({ parts }) => (
  <div className="sticky left-0 z-10 mb-5 inline-block max-w-[calc(100vw-6rem)] rounded-lg border border-slate-800 bg-slate-900 px-3 py-2 font-mono text-sm sm:max-w-none">
    {parts.map(([text, active], i) => (
      <span key={i} className={active ? "rounded bg-cyan-400/20 px-0.5 text-cyan-200 ring-1 ring-cyan-400/50" : "text-slate-400"}>
        {text}
      </span>
    ))}
  </div>
);

/** Big verdict chip used at the end of comparisons. */
export const Verdict = ({ ok, children }) => (
  <motion.span
    initial={{ scale: 0.6, opacity: 0 }}
    animate={{ scale: 1, opacity: 1 }}
    transition={{ type: "spring", stiffness: 260, damping: 16 }}
    className={`inline-flex items-center gap-2 rounded-full px-3 py-1 font-mono text-sm font-bold ${ok ? "bg-emerald-500/15 text-emerald-300 ring-1 ring-emerald-400/50" : "bg-rose-500/15 text-rose-300 ring-1 ring-rose-400/50"}`}
  >
    {children}
  </motion.span>
);

export default Stepper;
