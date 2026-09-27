import React, { useEffect, useMemo, useState } from "react";
import { Helmet } from "react-helmet";
import { Link } from "react-router-dom";
import { motion } from "framer-motion";
import Icon from "../../components/AppIcon";
import SiteShell from "../../components/SiteShell";
import BitRow, { toBits } from "../lessons/animations/BitRow";
import { availableLessonIds } from "../lessons/lessonLoader";
import { LAST_LESSON_KEY } from "../lessons/LessonPage";
import { allLessons, curriculum, deliveryOrder, findCurriculumItem } from "../../content/curriculum";

const COUNTER_START = 250;
const COUNTER_PERIOD_MS = 700;

/** Hero animation: a uint8_t counter that visibly wraps 255 → 0 (the A0.1 idea). */
const HeroCounter = () => {
  const [value, setValue] = useState(COUNTER_START);

  useEffect(() => {
    const id = window.setInterval(() => setValue((v) => (v + 1) & 0xff), COUNTER_PERIOD_MS);
    return () => window.clearInterval(id);
  }, []);

  const wrapped = value < COUNTER_START && value < 8;
  return (
    <div className="rounded-2xl border border-slate-800 bg-slate-950/80 p-4">
      <div className="mb-3 flex items-center justify-between font-mono text-xs">
        <span className="text-slate-400">uint8_t counter++;</span>
        <motion.span key={value} initial={{ y: -4 }} animate={{ y: 0 }} className={`font-bold ${wrapped ? "text-amber-300" : "text-cyan-200"}`}>
          = {value} {wrapped && "(wrapped!)"}
        </motion.span>
      </div>
      <div className="flex justify-center overflow-x-auto">
        <BitRow bits={toBits(value, 8)} maxWidth={8} idPrefix="hero" tone={(i, bit) => (bit ? (wrapped ? "promoted" : "focus") : "zero")} />
      </div>
    </div>
  );
};

const LEVELS = [
  { emoji: "🟢", title: "Beginner", subtitle: "What & why", text: "Plain language, real-world analogies, every acronym defined. Nothing assumed beyond basic C.", ring: "ring-emerald-400/30", tone: "text-emerald-300" },
  { emoji: "🟡", title: "Intermediate", subtitle: "How to use it", text: "Correct configuration, HAL and register-level code, and how to read the reference manual section.", ring: "ring-amber-400/30", tone: "text-amber-300" },
  { emoji: "🔴", title: "Advanced", subtitle: "Under the hood", text: "Instructions, timing, errata, compiler behaviour, and what actually goes wrong in shipped products.", ring: "ring-rose-400/30", tone: "text-rose-300" },
];

const FEATURES = [
  { icon: "Clapperboard", title: "Step-through animations", text: "Watch bits promote, flip, wrap and sign-extend. Every animation is tied to a real bug." },
  { icon: "FileCode2", title: "Compiled lab code", text: "HAL and register-level versions, built with the STM32CubeIDE toolchain under strict warnings." },
  { icon: "Bug", title: "Real mistakes", text: "Symptom, cause, how to diagnose it, how to fix it. At least five per lesson." },
  { icon: "ListChecks", title: "Quiz & graded exercises", text: "Instant feedback, then 🟢/🟡/🔴 challenges with hidden solutions." },
  { icon: "Microscope", title: "Verified facts", text: "Types, instructions and warnings checked against the real compiler and ST headers." },
  { icon: "StickyNote", title: "One-screen cheat sheet", text: "The rules, registers and formulas you need at the bench." },
];

const KIT = [
  { name: "NUCLEO-F446RE", note: "Cortex-M4F, 180 MHz, on-board ST-LINK" },
  { name: "Logic analyzer (8 ch)", note: "UART, SPI, I2C, PWM timing" },
  { name: "BME280 breakout", note: "I2C sensor lab (A10)" },
  { name: "W25Q64 SPI flash", note: "SPI lab and data logger (A9, B16)" },
  { name: "2× SN65HVD230", note: "CAN transceivers (A16, B16)" },
  { name: "DC motor + encoder + TB6612FNG", note: "Timers and control capstone (A7, B16)" },
];

const EmbeddedAcademy = () => {
  const [lastLessonId, setLastLessonId] = useState(null);

  useEffect(() => {
    try {
      setLastLessonId(window.localStorage.getItem(LAST_LESSON_KEY));
    } catch (e) {
      /* storage unavailable: no resume card */
    }
  }, []);

  const published = useMemo(() => deliveryOrder.filter((l) => availableLessonIds.has(l.id)), []);
  const firstLesson = published[0] || deliveryOrder[0];
  const nextUp = deliveryOrder.find((l) => !availableLessonIds.has(l.id));
  const resume = lastLessonId && availableLessonIds.has(lastLessonId) ? findCurriculumItem(lastLessonId) : null;
  const moduleCount = curriculum.reduce((n, part) => n + part.modules.length, 0);
  const totalHours = curriculum.reduce((n, part) => n + part.modules.reduce((h, m) => h + m.hours, 0), 0);

  return (
    <SiteShell>
      <Helmet>
        <title>Embedded Pulse Academy | STM32 & FreeRTOS from scratch to production</title>
        <meta name="description" content="STM32 and FreeRTOS lessons at three levels, with animations, compiled lab code, quizzes and exercises." />
      </Helmet>

      {/* ---------------------------------------------------------------- Hero */}
      <section className="relative overflow-hidden border-b border-slate-800">
        <div className="pointer-events-none absolute inset-0 bg-[radial-gradient(ellipse_at_top_right,rgba(34,211,238,0.18),transparent_55%),radial-gradient(ellipse_at_bottom_left,rgba(16,185,129,0.10),transparent_50%)]" />
        <div className="relative mx-auto grid max-w-7xl items-center gap-12 px-4 py-16 sm:px-6 md:py-24 lg:grid-cols-[1.15fr_0.85fr] lg:px-8">
          <motion.div initial={{ opacity: 0, y: 18 }} animate={{ opacity: 1, y: 0 }} transition={{ duration: 0.5 }}>
            <p className="inline-flex items-center gap-2 rounded-full border border-cyan-400/30 bg-cyan-400/10 px-3 py-1.5 text-xs font-semibold uppercase tracking-[0.16em] text-cyan-200">
              <Icon name="Sparkles" size={13} /> New curriculum · {published.length} of {allLessons.length} lessons live
            </p>
            <h1 className="mt-6 text-4xl font-black leading-tight tracking-tight text-white md:text-6xl">
              STM32 &amp; FreeRTOS,
              <br />
              <span className="bg-gradient-to-r from-cyan-300 via-sky-300 to-emerald-300 bg-clip-text text-transparent">down to the last bit.</span>
            </h1>
            <p className="mt-6 max-w-2xl text-lg leading-8 text-slate-300">
              A complete path from embedded C to production RTOS firmware. Every concept is explained three ways, for beginners, working engineers and senior developers. Every lesson includes
              animations, compiled lab code and real debugging stories.
            </p>
            <div className="mt-8 flex flex-wrap gap-3">
              <Link to={`/lessons/${resume ? resume.id : firstLesson.id}`} className="inline-flex items-center gap-2 rounded-xl bg-cyan-400 px-5 py-3 font-semibold text-slate-950 transition hover:bg-cyan-300">
                <Icon name="PlayCircle" size={18} />
                {resume ? `Continue ${resume.id}` : `Start with ${firstLesson.id}`}
              </Link>
              <Link to="/lessons" className="inline-flex items-center gap-2 rounded-xl border border-slate-600 bg-slate-900/70 px-5 py-3 font-semibold text-white transition hover:border-slate-400">
                <Icon name="Map" size={18} /> View the curriculum
              </Link>
            </div>
            <dl className="mt-10 grid max-w-xl grid-cols-3 gap-3">
              {[
                [allLessons.length, "Lessons"],
                [moduleCount, "Modules"],
                [`~${totalHours} h`, "Hands-on"],
              ].map(([value, label]) => (
                <div key={label} className="rounded-xl bg-slate-900/60 p-4 ring-1 ring-slate-800">
                  <dt className="text-xs uppercase tracking-[0.14em] text-slate-400">{label}</dt>
                  <dd className="mt-1 text-2xl font-bold text-white">{value}</dd>
                </div>
              ))}
            </dl>
          </motion.div>

          <motion.div initial={{ opacity: 0, x: 18 }} animate={{ opacity: 1, x: 0 }} transition={{ duration: 0.5, delay: 0.1 }} className="space-y-4">
            <div className="rounded-3xl border border-slate-800 bg-slate-900/70 p-5 shadow-2xl">
              <p className="text-xs font-semibold uppercase tracking-[0.16em] text-slate-400">{resume ? "Pick up where you left off" : "Your first lesson"}</p>
              <Link to={`/lessons/${(resume || firstLesson).id}`} className="group mt-2 block">
                <p className="font-mono text-sm text-cyan-400">{(resume || firstLesson).id}</p>
                <h2 className="text-2xl font-bold text-white group-hover:text-cyan-200">{(resume || firstLesson).title}</h2>
                <p className="mt-2 text-sm leading-6 text-slate-400">{(resume || firstLesson).summary}</p>
              </Link>
              <div className="mt-4">
                <HeroCounter />
              </div>
              <p className="mt-3 text-xs text-slate-500">Live: an 8-bit counter wrapping from 255 to 0. Why that matters is the subject of lesson A0.1.</p>
            </div>
            {nextUp && (
              <div className="flex items-center gap-3 rounded-2xl border border-dashed border-slate-700 px-4 py-3 text-sm text-slate-400">
                <Icon name="Hourglass" size={16} className="text-slate-500" />
                Coming next: <span className="font-mono text-slate-300">{nextUp.id}</span> {nextUp.title}
              </div>
            )}
          </motion.div>
        </div>
      </section>

      {/* ------------------------------------------------- Published lessons */}
      <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
        <div className="flex flex-wrap items-end justify-between gap-4">
          <div>
            <p className="text-xs font-semibold uppercase tracking-[0.18em] text-cyan-300">Available now</p>
            <h2 className="mt-2 text-3xl font-black text-white">Published lessons</h2>
          </div>
          <Link to="/lessons" className="text-sm font-semibold text-cyan-300 hover:text-cyan-200">
            All {allLessons.length} lessons →
          </Link>
        </div>
        <div className="mt-8 grid gap-5 md:grid-cols-2 xl:grid-cols-3">
          {published.map((lesson, i) => (
            <motion.div key={lesson.id} initial={{ opacity: 0, y: 14 }} animate={{ opacity: 1, y: 0 }} transition={{ delay: 0.2 + i * 0.06 }}>
              <Link to={`/lessons/${lesson.id}`} className="group flex h-full flex-col rounded-2xl border border-slate-800 bg-slate-900/60 p-6 transition hover:-translate-y-0.5 hover:border-cyan-400/50">
                <div className="flex items-center justify-between">
                  <span className="rounded-md bg-cyan-400/10 px-2 py-0.5 font-mono text-xs font-bold text-cyan-300">{lesson.id}</span>
                  <span className="text-xs text-slate-500">Module {lesson.moduleId} · {lesson.level}</span>
                </div>
                <h3 className="mt-4 text-xl font-bold text-white group-hover:text-cyan-200">{lesson.title}</h3>
                <p className="mt-2 flex-1 text-sm leading-6 text-slate-400">{lesson.summary}</p>
                <span className="mt-5 inline-flex items-center gap-1.5 text-sm font-semibold text-cyan-300">
                  Open lesson <Icon name="ArrowRight" size={15} className="transition group-hover:translate-x-1" />
                </span>
              </Link>
            </motion.div>
          ))}
        </div>
      </section>

      {/* -------------------------------------------------- How lessons work */}
      <section className="border-y border-slate-800 bg-slate-900/30">
        <div className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
          <p className="text-xs font-semibold uppercase tracking-[0.18em] text-cyan-300">How every lesson works</p>
          <h2 className="mt-2 max-w-3xl text-3xl font-black text-white">One lesson, three depths. Beginners and senior engineers both learn something.</h2>
          <div className="mt-8 grid gap-5 md:grid-cols-3">
            {LEVELS.map((level) => (
              <div key={level.title} className={`rounded-2xl bg-slate-950/70 p-6 ring-1 ${level.ring}`}>
                <p className="text-2xl">{level.emoji}</p>
                <h3 className={`mt-3 text-lg font-bold ${level.tone}`}>{level.title}</h3>
                <p className="text-xs uppercase tracking-[0.14em] text-slate-500">{level.subtitle}</p>
                <p className="mt-3 text-sm leading-6 text-slate-300">{level.text}</p>
              </div>
            ))}
          </div>
          <div className="mt-10 grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
            {FEATURES.map((f) => (
              <div key={f.title} className="flex gap-4 rounded-2xl border border-slate-800 bg-slate-950/50 p-5">
                <span className="flex h-10 w-10 shrink-0 items-center justify-center rounded-xl bg-cyan-400/10 text-cyan-300">
                  <Icon name={f.icon} size={19} />
                </span>
                <div>
                  <h3 className="font-semibold text-white">{f.title}</h3>
                  <p className="mt-1 text-sm leading-6 text-slate-400">{f.text}</p>
                </div>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* ------------------------------------------------------ The two parts */}
      <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
        <p className="text-xs font-semibold uppercase tracking-[0.18em] text-cyan-300">The path</p>
        <h2 className="mt-2 text-3xl font-black text-white">Two parts, {moduleCount} modules</h2>
        <div className="mt-8 grid gap-6 lg:grid-cols-2">
          {curriculum.map((part) => {
            const lessons = part.modules.flatMap((m) => m.lessons);
            const done = lessons.filter((l) => availableLessonIds.has(l.id)).length;
            return (
              <div key={part.part} className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6">
                <div className="flex items-start justify-between gap-4">
                  <div>
                    <p className="font-mono text-sm text-cyan-400">Part {part.part}</p>
                    <h3 className="text-2xl font-bold text-white">{part.partTitle}</h3>
                  </div>
                  <span className="rounded-full bg-slate-800 px-3 py-1 text-xs text-slate-300">
                    {done}/{lessons.length} live
                  </span>
                </div>
                <div className="mt-5 grid gap-2 sm:grid-cols-2">
                  {part.modules.map((module) => {
                    const live = module.lessons.filter((l) => availableLessonIds.has(l.id)).length;
                    return (
                      <Link
                        key={module.id}
                        to={`/lessons#${module.id}`}
                        className={`flex items-center gap-3 rounded-xl border px-3 py-2.5 text-sm transition ${live ? "border-cyan-400/40 bg-cyan-500/5 text-white hover:bg-cyan-500/10" : "border-slate-800 text-slate-400 hover:border-slate-600 hover:text-slate-200"}`}
                      >
                        <span className={`w-9 shrink-0 font-mono text-xs font-bold ${live ? "text-cyan-300" : "text-slate-500"}`}>{module.id}</span>
                        <span className="flex-1 leading-5">{module.title}</span>
                        {live > 0 && <span className="rounded-full bg-cyan-400/15 px-2 py-0.5 text-[10px] font-bold text-cyan-200">{live} live</span>}
                      </Link>
                    );
                  })}
                </div>
              </div>
            );
          })}
        </div>
      </section>

      {/* -------------------------------------------------------- Bench kit */}
      <section className="border-t border-slate-800 bg-slate-900/30">
        <div className="mx-auto grid max-w-7xl gap-10 px-4 py-16 sm:px-6 lg:grid-cols-[0.8fr_1.2fr] lg:px-8">
          <div>
            <p className="text-xs font-semibold uppercase tracking-[0.18em] text-cyan-300">Your bench</p>
            <h2 className="mt-2 text-3xl font-black text-white">One board to start. A small kit to finish.</h2>
            <p className="mt-4 leading-7 text-slate-400">
              Part A's first modules need only the Nucleo board and a USB cable. The extra parts are cheap, well documented breakouts, and each is introduced in the module that uses it.
            </p>
          </div>
          <ul className="grid gap-3 sm:grid-cols-2">
            {KIT.map((item, i) => (
              <li key={item.name} className="flex items-start gap-3 rounded-2xl border border-slate-800 bg-slate-950/60 p-4">
                <span className={`mt-0.5 flex h-7 w-7 shrink-0 items-center justify-center rounded-lg text-xs font-bold ${i === 0 ? "bg-cyan-400 text-slate-950" : "bg-slate-800 text-slate-300"}`}>{i + 1}</span>
                <span>
                  <span className="block font-semibold text-white">{item.name}</span>
                  <span className="text-sm text-slate-400">{item.note}</span>
                </span>
              </li>
            ))}
          </ul>
        </div>
      </section>
    </SiteShell>
  );
};

export default EmbeddedAcademy;
