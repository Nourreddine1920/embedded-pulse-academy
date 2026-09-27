import React, { useEffect } from "react";
import { Link, useLocation } from "react-router-dom";
import { Helmet } from "react-helmet";
import { motion } from "framer-motion";
import Icon from "../../components/AppIcon";
import Shell from "../../components/SiteShell";
import { availableLessonIds } from "./lessonLoader";
import { allLessons, curriculum } from "../../content/curriculum";

const LEVEL_TONE = { B: "text-emerald-300", I: "text-amber-300", A: "text-rose-300" };

const LevelBadge = ({ level }) => (
  <span className="font-mono text-[11px] font-bold">
    {level.split("–").map((part, i) => (
      <React.Fragment key={part + i}>
        {i > 0 && <span className="text-slate-600">–</span>}
        <span className={LEVEL_TONE[part] || "text-slate-300"}>{part}</span>
      </React.Fragment>
    ))}
  </span>
);

/** Full curriculum map with links to the lessons that are written. */
const LessonsIndexPage = () => {
  const written = allLessons.filter((l) => availableLessonIds.has(l.id)).length;
  const { hash } = useLocation();

  /* React Router does not scroll to #A7-style anchors by itself. */
  useEffect(() => {
    if (!hash) return undefined;
    const id = window.setTimeout(() => document.getElementById(decodeURIComponent(hash.slice(1)))?.scrollIntoView({ behavior: "smooth", block: "start" }), 150);
    return () => window.clearTimeout(id);
  }, [hash]);

  return (
    <Shell>
      <Helmet>
        <title>Curriculum | Embedded Pulse Academy</title>
      </Helmet>
      <main className="mx-auto max-w-7xl px-4 py-12 sm:px-6 lg:px-8">
        <p className="text-xs font-semibold uppercase tracking-[0.18em] text-cyan-300">STM32 + FreeRTOS · NUCLEO-F446RE</p>
        <h1 className="mt-3 text-4xl font-black tracking-tight text-white">Curriculum map</h1>
        <p className="mt-4 max-w-3xl text-lg leading-8 text-slate-300">
          {allLessons.length} lessons across two parts, from embedded C to production FreeRTOS systems. Every lesson explains each concept at three levels:
          <span className="text-emerald-300"> 🟢 Beginner</span>, <span className="text-amber-300">🟡 Intermediate</span> and <span className="text-rose-300">🔴 Advanced</span>.
        </p>
        <div className="mt-6 max-w-md">
          <div className="flex justify-between text-xs text-slate-400">
            <span>Lessons published</span>
            <span>{written} / {allLessons.length}</span>
          </div>
          <div className="mt-1.5 h-2 overflow-hidden rounded-full bg-slate-800">
            <motion.div initial={{ width: 0 }} animate={{ width: `${(written / allLessons.length) * 100}%` }} transition={{ duration: 1 }} className="h-full rounded-full bg-cyan-400" />
          </div>
        </div>

        {curriculum.map((part) => (
          <section key={part.part} className="mt-14">
            <h2 className="text-2xl font-black text-white">
              Part {part.part} · {part.partTitle}
            </h2>
            <div className="mt-6 grid gap-5 md:grid-cols-2 xl:grid-cols-3">
              {part.modules.map((module, mi) => (
                <motion.div
                  key={module.id}
                  id={module.id}
                  initial={{ opacity: 0, y: 14 }}
                  whileInView={{ opacity: 1, y: 0 }}
                  viewport={{ once: true, margin: "-40px" }}
                  transition={{ duration: 0.35, delay: (mi % 3) * 0.05 }}
                  className="scroll-mt-24 rounded-2xl border border-slate-800 bg-slate-900/60 p-5"
                >
                  <div className="flex items-start justify-between gap-3">
                    <div>
                      <p className="font-mono text-xs font-bold text-cyan-400">{module.id}</p>
                      <h3 className="text-lg font-bold text-white">{module.title}</h3>
                    </div>
                    <div className="text-right text-xs text-slate-500">
                      <LevelBadge level={module.level} />
                      <div>~{module.hours} h</div>
                    </div>
                  </div>
                  <ol className="mt-4 space-y-1">
                    {module.lessons.map((lesson) => {
                      const available = availableLessonIds.has(lesson.id);
                      const inner = (
                        <>
                          <span className={`w-12 shrink-0 font-mono text-xs ${available ? "text-cyan-300" : "text-slate-600"}`}>{lesson.id}</span>
                          <span className="flex-1">{lesson.title}</span>
                          {available ? <Icon name="ArrowRight" size={14} className="mt-0.5 shrink-0 text-cyan-300" /> : <LevelBadge level={lesson.level} />}
                        </>
                      );
                      return (
                        <li key={lesson.id}>
                          {available ? (
                            <Link to={`/lessons/${lesson.id}`} title={lesson.summary} className="flex items-start gap-2 rounded-lg bg-cyan-500/10 px-2 py-1.5 text-sm text-white ring-1 ring-cyan-400/30 hover:bg-cyan-500/20">
                              {inner}
                            </Link>
                          ) : (
                            <div title={`${lesson.summary} (coming soon)`} className="flex items-start gap-2 px-2 py-1.5 text-sm text-slate-400">
                              {inner}
                            </div>
                          )}
                        </li>
                      );
                    })}
                  </ol>
                </motion.div>
              ))}
            </div>
          </section>
        ))}
      </main>
    </Shell>
  );
};

export default LessonsIndexPage;
