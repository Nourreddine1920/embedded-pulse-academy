import React, { useState } from "react";
import { AnimatePresence, motion } from "framer-motion";
import Icon from "../../../components/AppIcon";

/** Renders `code` spans inside quiz text. */
const inline = (text) =>
  String(text)
    .split(/(`[^`]+`)/g)
    .map((part, i) =>
      part.startsWith("`") && part.endsWith("`") ? (
        <code key={i} className="rounded bg-slate-800 px-1.5 py-0.5 font-mono text-[0.85em] text-cyan-200">
          {part.slice(1, -1)}
        </code>
      ) : (
        part
      )
    );

/**
 * Multiple-choice quiz from a ```quiz fenced JSON block:
 * [{ "q": "...", "options": ["..."], "answer": 1, "why": "..." }]
 */
const QuizBlock = ({ source }) => {
  let questions = [];
  try {
    questions = JSON.parse(source);
  } catch (e) {
    return <pre className="text-rose-300">Quiz JSON error: {e.message}</pre>;
  }
  return <Quiz questions={questions} />;
};

const Quiz = ({ questions }) => {
  const [picked, setPicked] = useState({});
  const answered = Object.keys(picked).length;
  const score = questions.filter((q, i) => picked[i] === q.answer).length;

  return (
    <div className="not-prose my-6 space-y-5">
      {questions.map((q, qi) => {
        const choice = picked[qi];
        const done = choice !== undefined;
        return (
          <div key={qi} className="rounded-2xl border border-slate-800 bg-slate-900/70 p-5">
            <p className="mb-4 text-[15px] font-semibold leading-7 text-white">
              <span className="mr-2 text-cyan-300">Q{qi + 1}.</span>
              {inline(q.q)}
            </p>
            <div className="space-y-2">
              {q.options.map((opt, oi) => {
                const isAnswer = oi === q.answer;
                let cls = "border-slate-700 bg-slate-950/60 text-slate-200 hover:border-slate-500";
                if (done && isAnswer) cls = "border-emerald-400/60 bg-emerald-500/10 text-emerald-100";
                else if (done && oi === choice) cls = "border-rose-400/60 bg-rose-500/10 text-rose-100";
                else if (done) cls = "border-slate-800 bg-slate-950/40 text-slate-500";
                return (
                  <button
                    key={oi}
                    type="button"
                    disabled={done}
                    onClick={() => setPicked((p) => ({ ...p, [qi]: oi }))}
                    className={`flex w-full items-start gap-3 rounded-xl border px-4 py-3 text-left text-sm leading-6 transition ${cls}`}
                  >
                    <span className="mt-0.5 font-sans text-xs font-bold text-slate-500">{String.fromCharCode(65 + oi)}</span>
                    <span className="flex-1">{inline(opt)}</span>
                    {done && isAnswer && <Icon name="CheckCircle2" size={18} className="shrink-0 text-emerald-300" />}
                    {done && oi === choice && !isAnswer && <Icon name="XCircle" size={18} className="shrink-0 text-rose-300" />}
                  </button>
                );
              })}
            </div>
            <AnimatePresence>
              {done && (
                <motion.p
                  initial={{ opacity: 0, height: 0 }}
                  animate={{ opacity: 1, height: "auto" }}
                  className={`mt-4 rounded-xl px-4 py-3 text-sm leading-6 ${choice === q.answer ? "bg-emerald-500/10 text-emerald-100" : "bg-rose-500/10 text-rose-100"}`}
                >
                  <b>{choice === q.answer ? "Correct. " : "Not quite. "}</b>
                  {inline(q.why)}
                </motion.p>
              )}
            </AnimatePresence>
          </div>
        );
      })}
      <div className="flex items-center justify-between rounded-2xl border border-cyan-500/30 bg-cyan-500/5 px-5 py-3 text-sm">
        <span className="text-slate-300">
          Score: <b className="text-cyan-200">{score}</b> / {questions.length} {answered < questions.length && <span className="text-slate-500">({questions.length - answered} left)</span>}
        </span>
        {answered > 0 && (
          <button type="button" onClick={() => setPicked({})} className="inline-flex items-center gap-1.5 text-xs text-slate-400 hover:text-white">
            <Icon name="RotateCcw" size={13} /> Retry
          </button>
        )}
      </div>
    </div>
  );
};

export default QuizBlock;
