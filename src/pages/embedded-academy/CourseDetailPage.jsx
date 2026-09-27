import React from "react";
import { Link, Navigate, useParams } from "react-router-dom";
import Icon from "../../components/AppIcon";
import { learningPathDetails } from "./data/platformContent";

const CourseDetailPage = () => {
  const { slug } = useParams();
  const currentPath = typeof window !== "undefined" ? window.location.pathname : "";
  const fallbackSlug = currentPath.includes("freertos") ? "freertos-path" : "stm32-path";
  const normalizedSlug =
    slug === "stm32" ? "stm32-path" : slug === "freertos" ? "freertos-path" : slug ?? fallbackSlug;
  const course = learningPathDetails[normalizedSlug] ?? learningPathDetails["stm32-path"];

  if (!learningPathDetails[normalizedSlug]) {
    return <Navigate to="/" replace />;
  }

  return (
    <div className="min-h-screen bg-slate-950 text-slate-100">
      <header className="sticky top-0 z-30 border-b border-slate-800 bg-slate-950/85 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-4 py-4 sm:px-6 lg:px-8">
          <Link to="/" className="inline-flex items-center gap-2 text-sm font-semibold uppercase tracking-[0.18em] text-cyan-300">
            <Icon name="Cpu" size={16} />
            Embedded Pulse Academy
          </Link>

          <nav className="hidden items-center gap-3 md:flex">
            <Link to="/" className="rounded-full border border-slate-700 px-3 py-1.5 text-sm text-slate-200 transition hover:border-slate-500">Home</Link>
            <Link to="/courses/stm32-path" className="rounded-full bg-cyan-400 px-4 py-2 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300">STM32 path</Link>
            <Link to="/courses/freertos-path" className="rounded-full border border-slate-700 px-4 py-2 text-sm font-semibold text-white transition hover:border-slate-500">FreeRTOS path</Link>
          </nav>
        </div>
      </header>

      <main>
        <section className="border-b border-slate-800 bg-gradient-to-br from-slate-950 via-slate-900 to-cyan-950/60">
          <div className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
            <div className="mb-6 inline-flex items-center gap-2 rounded-full border border-cyan-400/30 bg-cyan-400/10 px-3 py-1.5 text-[11px] font-semibold uppercase tracking-[0.18em] text-cyan-200">
              <Icon name="Sparkles" size={14} />
              {course.eyebrow}
            </div>

            <div className="grid items-start gap-10 lg:grid-cols-[1.1fr_0.9fr]">
              <div>
                <h1 className="max-w-3xl text-4xl font-black tracking-tight text-white md:text-6xl">
                  {course.title}
                </h1>
                <p className="mt-6 max-w-2xl text-lg leading-8 text-slate-300">
                  {course.summary}
                </p>

                <div className="mt-8 flex flex-wrap gap-3">
                  <Link to="/" className="rounded-xl bg-cyan-400 px-5 py-3 font-semibold text-slate-950 transition hover:bg-cyan-300">
                    Back to academy
                  </Link>
                  <a href="#curriculum" className="rounded-xl border border-slate-600 bg-slate-900 px-5 py-3 font-semibold text-white transition hover:border-slate-500">
                    Explore curriculum
                  </a>
                </div>
              </div>

              <div className="rounded-3xl border border-slate-700 bg-slate-900/70 p-6 shadow-2xl">
                <div className="mb-5 flex items-center justify-between">
                  <div>
                    <p className="text-xs uppercase tracking-[0.14em] text-slate-400">Learning profile</p>
                    <h2 className="mt-1 text-2xl font-bold text-white">{course.level}</h2>
                  </div>
                  <Icon name="TrendingUp" size={28} className="text-cyan-300" />
                </div>

                <div className="grid gap-4 sm:grid-cols-3">
                  <div className="rounded-2xl border border-slate-700 bg-slate-950/60 p-4">
                    <div className="text-xs uppercase tracking-[0.14em] text-slate-400">Duration</div>
                    <div className="mt-2 text-xl font-bold text-white">{course.duration}</div>
                  </div>
                  <div className="rounded-2xl border border-slate-700 bg-slate-950/60 p-4">
                    <div className="text-xs uppercase tracking-[0.14em] text-slate-400">Format</div>
                    <div className="mt-2 text-xl font-bold text-white">Project</div>
                  </div>
                  <div className="rounded-2xl border border-slate-700 bg-slate-950/60 p-4">
                    <div className="text-xs uppercase tracking-[0.14em] text-slate-400">Focus</div>
                    <div className="mt-2 text-xl font-bold text-white">{course.focus}</div>
                  </div>
                </div>

                <div className="mt-5">
                  <p className="mb-3 text-xs uppercase tracking-[0.16em] text-slate-400">Audience</p>
                  <div className="flex flex-wrap gap-2">
                    {course.audience.map((item) => (
                      <span key={item} className="rounded-full border border-slate-700 bg-slate-950 px-2.5 py-1 text-xs font-medium text-slate-200">
                        {item}
                      </span>
                    ))}
                  </div>
                </div>
              </div>
            </div>
          </div>
        </section>

        <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
          <div className="grid gap-8 lg:grid-cols-[1.1fr_0.9fr]">
            <div className="space-y-8">
              <div className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6 shadow-lg">
                <div className="mb-5 flex items-center gap-3">
                  <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-cyan-400/15 text-cyan-300">
                    <Icon name="Compass" size={20} />
                  </div>
                  <h2 className="text-2xl font-bold text-white">Overview</h2>
                </div>
                <p className="text-base leading-7 text-slate-300">{course.overview}</p>
              </div>

              <div className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6 shadow-lg">
                <h3 className="mb-5 text-2xl font-bold text-white">Skills you will build</h3>
                <div className="grid gap-4 md:grid-cols-2">
                  {course.outcomes.map((item) => (
                    <div key={item} className="rounded-2xl border border-slate-700 bg-slate-950/60 p-4">
                      <div className="mb-2 flex h-8 w-8 items-center justify-center rounded-full bg-cyan-400/15 text-cyan-300">
                        <Icon name="CheckCircle2" size={16} />
                      </div>
                      <p className="text-sm leading-6 text-slate-200">{item}</p>
                    </div>
                  ))}
                </div>
              </div>
            </div>

            <div className="space-y-8">
              <div className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6 shadow-lg">
                <h3 className="mb-5 text-2xl font-bold text-white">Prerequisites</h3>
                <ul className="space-y-3 text-slate-300">
                  {course.prerequisites.map((item) => (
                    <li key={item} className="flex items-start gap-3 rounded-2xl border border-slate-700 bg-slate-950/60 p-3">
                      <span className="mt-1 h-2 w-2 rounded-full bg-cyan-400" />
                      <span>{item}</span>
                    </li>
                  ))}
                </ul>
              </div>

              <div className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6 shadow-lg">
                <h3 className="mb-5 text-2xl font-bold text-white">Toolchain</h3>
                <div className="flex flex-wrap gap-2">
                  {course.tools.map((tool) => (
                    <span key={tool} className="rounded-full border border-cyan-400/30 bg-cyan-500/10 px-3 py-1.5 text-sm font-medium text-cyan-200">
                      {tool}
                    </span>
                  ))}
                </div>
              </div>
            </div>
          </div>
        </section>

        <section id="curriculum" className="border-y border-slate-800 bg-slate-900/60">
          <div className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
            <div className="mb-10">
              <p className="text-sm font-semibold uppercase tracking-[0.18em] text-cyan-300">Curriculum</p>
              <h2 className="mt-2 text-3xl font-bold text-white">Structured learning modules</h2>
            </div>

            <div className="space-y-6">
              {course.modules.map((module, index) => {
                const moduleSlug = module.slug || module.title.toLowerCase().replace(/[^a-z0-9]+/g, "-").replace(/(^-|-$)/g, "");

                return (
                  <div key={module.title} className="rounded-3xl border border-slate-800 bg-slate-950/70 p-6 shadow-lg">
                    <div className="mb-4 flex flex-col gap-3 md:flex-row md:items-center md:justify-between">
                      <div className="flex items-center gap-3">
                        <div className="flex h-10 w-10 items-center justify-center rounded-full bg-cyan-400/15 font-bold text-cyan-200">
                          {index + 1}
                        </div>
                        <div>
                          <h3 className="text-xl font-bold text-white">{module.title}</h3>
                          <p className="text-sm text-slate-400">{module.focus}</p>
                        </div>
                      </div>
                      <div className="flex items-center gap-3">
                        <span className="rounded-full border border-slate-700 bg-slate-900 px-3 py-1 text-xs font-semibold uppercase tracking-[0.16em] text-slate-200">
                          {module.duration}
                        </span>
                        <Link
                          to={`/courses/${normalizedSlug}/${moduleSlug}`}
                          className="rounded-full bg-cyan-400 px-3 py-1.5 text-xs font-semibold uppercase tracking-[0.12em] text-slate-950 hover:bg-cyan-300"
                        >
                          Open module
                        </Link>
                      </div>
                    </div>

                    <div className="grid gap-4 md:grid-cols-2">
                      <div>
                        <p className="mb-3 text-sm font-semibold uppercase tracking-[0.14em] text-cyan-300">Lessons</p>
                        <ul className="space-y-2 text-slate-300">
                          {module.lessons.map((lesson) => (
                            <li key={lesson} className="flex items-start gap-3 text-sm leading-6">
                              <span className="mt-2 h-2 w-2 rounded-full bg-cyan-400" />
                              <span>{lesson}</span>
                            </li>
                          ))}
                        </ul>
                      </div>

                      <div className="rounded-2xl border border-slate-700 bg-slate-900 p-4">
                        <p className="mb-2 text-sm font-semibold uppercase tracking-[0.14em] text-cyan-300">Lab objective</p>
                        <p className="text-sm leading-6 text-slate-300">{module.lab}</p>
                      </div>
                    </div>
                  </div>
                );
              })}
            </div>
          </div>
        </section>

        <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
          <div className="grid gap-8 lg:grid-cols-[1.1fr_0.9fr]">
            <div className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6 shadow-lg">
              <div className="mb-5 flex items-center gap-3">
                <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-cyan-400/15 text-cyan-300">
                  <Icon name="FolderKanban" size={20} />
                </div>
                <h3 className="text-2xl font-bold text-white">Project pipeline</h3>
              </div>

              <div className="space-y-4">
                {course.projects.map((project) => (
                  <div key={project.title} className="rounded-2xl border border-slate-700 bg-slate-950/70 p-4">
                    <div className="mb-2 flex items-center justify-between gap-3">
                      <h4 className="text-lg font-semibold text-white">{project.title}</h4>
                      <span className="rounded-full border border-cyan-400/30 bg-cyan-500/10 px-2.5 py-1 text-[11px] font-semibold uppercase tracking-[0.12em] text-cyan-200">
                        {project.level}
                      </span>
                    </div>
                    <p className="text-sm leading-6 text-slate-300">{project.summary}</p>
                    <div className="mt-3 flex flex-wrap gap-2">
                      {project.deliverables.map((deliverable) => (
                        <span key={deliverable} className="rounded-full border border-slate-700 bg-slate-900 px-2.5 py-1 text-[11px] font-medium text-slate-200">
                          {deliverable}
                        </span>
                      ))}
                    </div>
                  </div>
                ))}
              </div>
            </div>

            <div className="rounded-3xl border border-slate-800 bg-slate-900/60 p-6 shadow-lg">
              <h3 className="mb-5 text-2xl font-bold text-white">Assessment path</h3>
              <div className="space-y-4">
                {course.assessment.map((item) => (
                  <div key={item} className="flex items-start gap-3 rounded-2xl border border-slate-700 bg-slate-950/70 p-4 text-sm text-slate-300">
                    <span className="mt-1 flex h-6 w-6 items-center justify-center rounded-full bg-cyan-400/15 text-cyan-200">
                      <Icon name="ArrowRight" size={12} />
                    </span>
                    <span>{item}</span>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </section>
      </main>

      <footer className="border-t border-slate-800 bg-slate-950">
        <div className="mx-auto grid max-w-7xl gap-8 px-4 py-12 sm:px-6 lg:grid-cols-4 lg:px-8">
          <div>
            <div className="mb-4 inline-flex items-center gap-2 text-sm font-semibold uppercase tracking-[0.18em] text-cyan-300">
              <Icon name="Cpu" size={16} />
              Embedded Pulse
            </div>
            <p className="text-sm leading-7 text-slate-400">Modern embedded systems learning for engineers, builders, and firmware teams.</p>
          </div>

          <div>
            <h3 className="mb-4 text-sm font-semibold uppercase tracking-[0.16em] text-slate-200">Tracks</h3>
            <ul className="space-y-3 text-sm text-slate-400">
              <li><Link to="/courses/stm32-path">STM32 path</Link></li>
              <li><Link to="/courses/freertos-path">FreeRTOS path</Link></li>
            </ul>
          </div>

          <div>
            <h3 className="mb-4 text-sm font-semibold uppercase tracking-[0.16em] text-slate-200">Focus</h3>
            <ul className="space-y-3 text-sm text-slate-400">
              <li>Embedded C</li>
              <li>Peripherals</li>
              <li>RTOS design</li>
              <li>System debugging</li>
            </ul>
          </div>

          <div>
            <h3 className="mb-4 text-sm font-semibold uppercase tracking-[0.16em] text-slate-200">Contact</h3>
            <ul className="space-y-3 text-sm text-slate-400">
              <li>Professional training</li>
              <li>Hands-on labs</li>
              <li>Career-ready projects</li>
            </ul>
          </div>
        </div>
      </footer>
    </div>
  );
};

export default CourseDetailPage;
