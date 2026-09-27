import React from "react";
import { Link, Navigate, useParams } from "react-router-dom";
import Icon from "../../components/AppIcon";
import { learningPathDetails } from "./data/platformContent";

const CourseModulePage = () => {
  const { pathSlug, moduleSlug } = useParams();
  const course = learningPathDetails[pathSlug] ?? learningPathDetails["stm32-path"];
  const normalizedModuleSlug = moduleSlug || "";
  const module =
    course.modules.find((item) => {
      const generatedSlug = (item.slug || item.title).toLowerCase().replace(/[^a-z0-9]+/g, "-").replace(/(^-|-$)/g, "");
      return generatedSlug === normalizedModuleSlug;
    }) || course.modules[0];

  if (!learningPathDetails[pathSlug] || !module) {
    return <Navigate to="/" replace />;
  }

  const moduleObjectives = Array.isArray(module.objectives) && module.objectives.length ? module.objectives : module.lessons.slice(0, 4);
  const moduleSkills = Array.isArray(module.skills) && module.skills.length ? module.skills : ["Embedded design", "Debugging", "System thinking", "Validation"];
  const moduleDeliverables = Array.isArray(module.deliverables) && module.deliverables.length ? module.deliverables : [module.lab, "Structured notes", "Practical verification"];
  const moduleTakeaway = module.takeaway || `This module helps you understand ${module.title.toLowerCase()} and apply it in a real embedded project with measurable engineering outcomes.`;

  return (
    <div className="min-h-screen bg-slate-950 text-slate-100">
      <header className="sticky top-0 z-30 border-b border-slate-800 bg-slate-950/85 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-4 py-4 sm:px-6 lg:px-8">
          <Link to="/" className="inline-flex items-center gap-2 text-sm font-semibold uppercase tracking-[0.18em] text-cyan-300">
            <Icon name="Cpu" size={16} />
            Embedded Pulse Academy
          </Link>

          <nav className="hidden items-center gap-3 md:flex">
            <Link to="/" className="rounded-full border border-slate-700 px-3 py-1.5 text-sm text-slate-200 hover:border-slate-500">Home</Link>
            <Link to={`/courses/${pathSlug}`} className="rounded-full bg-cyan-400 px-4 py-2 text-sm font-semibold text-slate-950 hover:bg-cyan-300">Path overview</Link>
          </nav>
        </div>
      </header>

      <main className="mx-auto grid max-w-7xl gap-8 px-4 py-10 sm:px-6 lg:grid-cols-[280px_minmax(0,1fr)] lg:px-8">
        <aside className="rounded-3xl border border-slate-800 bg-slate-900/70 p-5 shadow-xl">
          <p className="mb-4 text-xs font-semibold uppercase tracking-[0.18em] text-cyan-300">{course.eyebrow}</p>
          <h2 className="text-2xl font-bold text-white">{course.title}</h2>
          <div className="mt-5 space-y-2">
            {course.modules.map((item) => (
              <Link
                key={item.slug}
                to={`/courses/${pathSlug}/${item.slug}`}
                className={`flex items-center justify-between rounded-2xl border px-3 py-3 text-sm transition ${
                  item.slug === module.slug
                    ? "border-cyan-400/50 bg-cyan-500/10 text-cyan-100"
                    : "border-slate-700 bg-slate-950/60 text-slate-300 hover:border-slate-500"
                }`}
              >
                <span>{item.title}</span>
                <Icon name="ArrowRight" size={14} />
              </Link>
            ))}
          </div>
        </aside>

        <article className="space-y-8">
          <section className="rounded-3xl border border-slate-800 bg-gradient-to-br from-slate-900 via-slate-900 to-cyan-950/40 p-8 shadow-2xl">
            <div className="mb-4 inline-flex items-center gap-2 rounded-full border border-cyan-400/30 bg-cyan-500/10 px-3 py-1.5 text-[11px] font-semibold uppercase tracking-[0.18em] text-cyan-200">
              <Icon name="Layers3" size={14} />
              Module focus
            </div>
            <h1 className="text-4xl font-black tracking-tight text-white">{module.title}</h1>
            <p className="mt-4 max-w-3xl text-lg leading-8 text-slate-300">{module.description}</p>

            <div className="mt-6 flex flex-wrap gap-3 text-sm">
              <span className="rounded-full border border-slate-700 bg-slate-950/60 px-3 py-1.5 text-slate-200">{module.duration}</span>
              <span className="rounded-full border border-slate-700 bg-slate-950/60 px-3 py-1.5 text-slate-200">{module.focus}</span>
            </div>
          </section>

          <section className="grid gap-8 lg:grid-cols-2">
            <div className="rounded-3xl border border-slate-800 bg-slate-900/70 p-6 shadow-lg">
              <h2 className="mb-4 text-2xl font-bold text-white">What you will learn</h2>
              <ul className="space-y-3">
                {moduleObjectives.map((item) => (
                  <li key={item} className="flex items-start gap-3 rounded-2xl border border-slate-700 bg-slate-950/60 p-3 text-sm leading-6 text-slate-200">
                    <span className="mt-1 h-2.5 w-2.5 rounded-full bg-cyan-400" />
                    <span>{item}</span>
                  </li>
                ))}
              </ul>
            </div>

            <div className="rounded-3xl border border-slate-800 bg-slate-900/70 p-6 shadow-lg">
              <h2 className="mb-4 text-2xl font-bold text-white">Skills to master</h2>
              <div className="flex flex-wrap gap-2">
                {moduleSkills.map((skill) => (
                  <span key={skill} className="rounded-full border border-cyan-400/30 bg-cyan-500/10 px-3 py-1.5 text-sm text-cyan-100">{skill}</span>
                ))}
              </div>
            </div>
          </section>

          <section className="rounded-3xl border border-slate-800 bg-slate-900/70 p-6 shadow-lg">
            <h2 className="mb-5 text-2xl font-bold text-white">Module breakdown</h2>
            <div className="space-y-4">
              {module.lessons.map((lesson, index) => (
                <div key={lesson} className="rounded-2xl border border-slate-700 bg-slate-950/60 p-4">
                  <div className="mb-2 flex items-center gap-3">
                    <div className="flex h-8 w-8 items-center justify-center rounded-full bg-cyan-500/10 text-xs font-bold text-cyan-200">{index + 1}</div>
                    <h3 className="text-lg font-semibold text-white">{lesson}</h3>
                  </div>
                </div>
              ))}
            </div>
          </section>

          <section className="grid gap-8 lg:grid-cols-2">
            <div className="rounded-3xl border border-slate-800 bg-slate-900/70 p-6 shadow-lg">
              <h2 className="mb-4 text-2xl font-bold text-white">Practical lab</h2>
              <p className="text-base leading-7 text-slate-300">{module.lab}</p>
            </div>

            <div className="rounded-3xl border border-slate-800 bg-slate-900/70 p-6 shadow-lg">
              <h2 className="mb-4 text-2xl font-bold text-white">Deliverables</h2>
              <ul className="space-y-2 text-slate-300">
                {moduleDeliverables.map((item) => (
                  <li key={item} className="flex items-start gap-3 text-sm leading-6">
                    <span className="mt-2 h-2 w-2 rounded-full bg-cyan-400" />
                    <span>{item}</span>
                  </li>
                ))}
              </ul>
            </div>
          </section>

          <section className="rounded-3xl border border-slate-800 bg-slate-900/70 p-6 shadow-lg">
            <h2 className="mb-5 text-2xl font-bold text-white">Key takeaway</h2>
            <p className="text-base leading-7 text-slate-300">{moduleTakeaway}</p>
          </section>
        </article>
      </main>
    </div>
  );
};

export default CourseModulePage;
