import React, { useEffect, useMemo, useState } from "react";
import { Link, useParams } from "react-router-dom";
import { Helmet } from "react-helmet";
import { motion, useScroll, useSpring } from "framer-motion";
import Icon from "../../components/AppIcon";
import Shell from "../../components/SiteShell";
import MarkdownLesson, { slugify } from "./components/MarkdownLesson";
import { availableLessonIds, loadLesson } from "./lessonLoader";
import { deliveryOrder, findCurriculumItem } from "../../content/curriculum";

/** Renders `code` spans in short frontmatter text. */
const inlineCode = (text) =>
  String(text)
    .split(/(`[^`]+`)/g)
    .map((part, i) =>
      part.startsWith("`") && part.endsWith("`") ? (
        <code key={i} className="rounded-md bg-slate-800 px-1.5 py-0.5 font-mono text-[0.85em] text-cyan-200">
          {part.slice(1, -1)}
        </code>
      ) : (
        part
      )
    );

/** localStorage key read by the home page "continue" card. */
export const LAST_LESSON_KEY = "epa-last-lesson";

const asList = (value) => (Array.isArray(value) ? value : value ? [value] : []);

/** Collects "## Heading" lines outside fenced blocks for the table of contents. */
const extractToc = (body) => {
  const toc = [];
  let fence = null;
  body.split(/\r?\n/).forEach((line) => {
    const f = /^(`{3,}|~{3,})/.exec(line.trim());
    if (f) {
      if (!fence) fence = f[1];
      else if (line.trim().startsWith(fence)) fence = null;
      return;
    }
    if (!fence && line.startsWith("## ")) toc.push({ text: line.slice(3).trim(), id: slugify(line.slice(3).trim()) });
  });
  return toc;
};

const LessonChip = ({ id, fallback }) => {
  const item = findCurriculumItem(id);
  const available = availableLessonIds.has(id);
  const label = item ? `${id} · ${item.title}` : fallback || id;
  return available ? (
    <Link to={`/lessons/${id}`} className="rounded-full border border-cyan-400/40 bg-cyan-500/10 px-3 py-1 text-xs text-cyan-100 hover:border-cyan-300">
      {label}
    </Link>
  ) : (
    <span className="rounded-full border border-slate-700 bg-slate-900 px-3 py-1 text-xs text-slate-400" title="Coming soon">
      {label}
    </span>
  );
};

const LessonPage = () => {
  const { lessonId } = useParams();
  const [lesson, setLesson] = useState(undefined);
  const [activeId, setActiveId] = useState("");
  const { scrollYProgress } = useScroll();
  const progress = useSpring(scrollYProgress, { stiffness: 120, damping: 30 });

  useEffect(() => {
    let cancelled = false;
    setLesson(undefined);
    loadLesson(lessonId).then((l) => {
      if (cancelled) return;
      setLesson(l);
      if (l) {
        try {
          window.localStorage.setItem(LAST_LESSON_KEY, lessonId);
        } catch (e) {
          /* storage unavailable (private mode): resume link is optional */
        }
      }
    });
    window.scrollTo(0, 0);
    return () => {
      cancelled = true;
    };
  }, [lessonId]);

  const toc = useMemo(() => (lesson ? extractToc(lesson.body) : []), [lesson]);

  /* Highlight the section currently in view. */
  useEffect(() => {
    if (!toc.length) return undefined;
    const observer = new IntersectionObserver(
      (entries) => {
        const visible = entries.filter((e) => e.isIntersecting).sort((a, b) => a.boundingClientRect.top - b.boundingClientRect.top);
        if (visible[0]) setActiveId(visible[0].target.id);
      },
      { rootMargin: "-80px 0px -70% 0px" }
    );
    const t = window.setTimeout(() => toc.forEach(({ id }) => document.getElementById(id) && observer.observe(document.getElementById(id))), 300);
    return () => {
      window.clearTimeout(t);
      observer.disconnect();
    };
  }, [toc]);

  const orderIndex = deliveryOrder.findIndex((l) => l.id === lessonId);
  const prev = orderIndex > 0 ? deliveryOrder[orderIndex - 1] : null;
  const next = orderIndex >= 0 && orderIndex < deliveryOrder.length - 1 ? deliveryOrder[orderIndex + 1] : null;
  const curriculumItem = findCurriculumItem(lessonId);

  if (lesson === null) {
    return (
      <Shell>
        <div className="mx-auto max-w-2xl px-4 py-24 text-center">
          <p className="text-sm uppercase tracking-[0.18em] text-cyan-300">{lessonId}</p>
          <h1 className="mt-3 text-3xl font-black text-white">{curriculumItem?.title || "Lesson not found"}</h1>
          <p className="mt-4 text-slate-400">This lesson is not written yet.</p>
          <Link to="/lessons" className="mt-8 inline-flex rounded-full bg-cyan-400 px-5 py-2 text-sm font-semibold text-slate-950">Back to the curriculum</Link>
        </div>
      </Shell>
    );
  }

  if (!lesson) {
    return (
      <Shell>
        <div className="mx-auto max-w-4xl space-y-4 px-4 py-16">
          {[0, 1, 2].map((i) => <div key={i} className="h-24 animate-pulse rounded-2xl bg-slate-900" />)}
        </div>
      </Shell>
    );
  }

  const { meta, body } = lesson;

  return (
    <Shell>
      <Helmet>
        <title>{`${meta.id} ${meta.title} | Embedded Pulse Academy`}</title>
      </Helmet>
      <motion.div style={{ scaleX: progress }} className="fixed left-0 right-0 top-0 z-50 h-1 origin-left bg-gradient-to-r from-cyan-400 via-sky-400 to-emerald-400" />

      <div className="mx-auto grid max-w-7xl gap-10 px-4 py-10 sm:px-6 lg:grid-cols-[240px_minmax(0,1fr)] lg:px-8">
        <aside className="hidden lg:block">
          <nav className="sticky top-24 max-h-[calc(100vh-8rem)] overflow-y-auto pr-2">
            <p className="mb-3 text-xs font-semibold uppercase tracking-[0.18em] text-slate-500">On this page</p>
            <ol className="space-y-1 border-l border-slate-800">
              {toc.map((item) => (
                <li key={item.id}>
                  <a
                    href={`#${item.id}`}
                    className={`-ml-px block border-l-2 py-1.5 pl-4 text-[13px] leading-5 transition ${activeId === item.id ? "border-cyan-400 font-semibold text-cyan-200" : "border-transparent text-slate-400 hover:border-slate-500 hover:text-slate-200"}`}
                  >
                    {item.text}
                  </a>
                </li>
              ))}
            </ol>
          </nav>
        </aside>

        <article className="min-w-0 max-w-[820px]">
          <motion.header initial={{ opacity: 0, y: 16 }} animate={{ opacity: 1, y: 0 }} transition={{ duration: 0.5 }} className="rounded-3xl border border-slate-800 bg-gradient-to-br from-slate-900 via-slate-900 to-cyan-950/50 p-6 shadow-2xl sm:p-8">
            <div className="flex flex-wrap items-center gap-2 text-xs">
              <Link to="/lessons" className="font-semibold uppercase tracking-[0.18em] text-cyan-300 hover:text-cyan-200">
                {curriculumItem ? `Module ${curriculumItem.moduleId} · ${curriculumItem.moduleTitle}` : "Curriculum"}
              </Link>
            </div>
            <h1 className="mt-3 text-3xl font-black tracking-tight text-white sm:text-4xl">
              <span className="mr-3 text-cyan-400">{meta.id}</span>
              {meta.title}
            </h1>
            {meta.summary && <p className="mt-4 text-lg leading-8 text-slate-300">{inlineCode(meta.summary)}</p>}

            <dl className="mt-6 grid gap-3 text-sm sm:grid-cols-3">
              {[
                ["Level", meta.level, "Gauge"],
                ["Estimated time", meta.time, "Clock"],
                ["Reference MCU", meta.mcu, "Cpu"],
              ].map(([label, value, icon]) => (
                <div key={label} className="rounded-2xl border border-slate-800 bg-slate-950/60 p-3">
                  <dt className="flex items-center gap-1.5 text-[11px] uppercase tracking-[0.14em] text-slate-500">
                    <Icon name={icon} size={12} />
                    {label}
                  </dt>
                  <dd className="mt-1 font-semibold text-slate-100">{value || "—"}</dd>
                </div>
              ))}
            </dl>

            <div className="mt-5 space-y-3 text-sm">
              <div className="flex flex-wrap items-center gap-2">
                <span className="w-28 text-xs uppercase tracking-[0.12em] text-slate-500">Prerequisites</span>
                {asList(meta.prereqs).length ? asList(meta.prereqs).map((p) => <LessonChip key={p} id={p} />) : <span className="text-slate-300">{meta.prereqsText || "Basic C"}</span>}
              </div>
              <div className="flex flex-wrap items-center gap-2">
                <span className="w-28 text-xs uppercase tracking-[0.12em] text-slate-500">Hardware</span>
                {asList(meta.hardware).map((h) => (
                  <span key={h} className="rounded-full border border-slate-700 bg-slate-950/60 px-3 py-1 text-xs text-slate-200">{h}</span>
                ))}
              </div>
            </div>
          </motion.header>

          <div className="lesson-body">
            <MarkdownLesson source={body} />
          </div>

          <section className="mt-16 grid gap-4 rounded-3xl border border-slate-800 bg-slate-900/60 p-6 sm:grid-cols-2">
            <div>
              <p className="mb-3 flex items-center gap-2 text-xs font-semibold uppercase tracking-[0.16em] text-slate-400">
                <Icon name="ArrowDownToLine" size={14} /> This lesson depends on
              </p>
              <div className="flex flex-wrap gap-2">{asList(meta.dependsOn).length ? asList(meta.dependsOn).map((d) => <LessonChip key={d} id={d} />) : <span className="text-sm text-slate-400">Nothing but basic C.</span>}</div>
            </div>
            <div>
              <p className="mb-3 flex items-center gap-2 text-xs font-semibold uppercase tracking-[0.16em] text-slate-400">
                <Icon name="ArrowUpFromLine" size={14} /> Lessons that build on it
              </p>
              <div className="flex flex-wrap gap-2">{asList(meta.buildsOn).map((d) => <LessonChip key={d} id={d} />)}</div>
            </div>
          </section>

          <nav className="mt-8 grid gap-4 sm:grid-cols-2">
            {prev ? (
              <Link to={`/lessons/${prev.id}`} className="group rounded-2xl border border-slate-800 bg-slate-900/60 p-5 hover:border-slate-600">
                <span className="flex items-center gap-1 text-xs text-slate-500"><Icon name="ArrowLeft" size={13} /> Previous</span>
                <span className="mt-1 block font-semibold text-slate-100 group-hover:text-cyan-200">{prev.id} · {prev.title}</span>
              </Link>
            ) : <span />}
            {next && (
              <Link to={`/lessons/${next.id}`} className="group rounded-2xl border border-slate-800 bg-slate-900/60 p-5 text-right hover:border-slate-600">
                <span className="flex items-center justify-end gap-1 text-xs text-slate-500">Next <Icon name="ArrowRight" size={13} /></span>
                <span className="mt-1 block font-semibold text-slate-100 group-hover:text-cyan-200">{next.id} · {next.title}</span>
                {!availableLessonIds.has(next.id) && <span className="text-xs text-slate-500">coming soon</span>}
              </Link>
            )}
          </nav>
        </article>
      </div>
    </Shell>
  );
};

export default LessonPage;
