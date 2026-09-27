import React, { useMemo, useState } from "react";
import { Helmet } from "react-helmet";
import { useNavigate } from "react-router-dom";
import Icon from "../../components/AppIcon";
import CourseCard from "./components/CourseCard";
import CodeBlock from "./components/CodeBlock";
import QuizCard from "./components/QuizCard";
import TaskTimeline from "./components/TaskTimeline";
import {
  academyMeta,
  platformRoadmaps,
  microcontrollerCatalog,
  freertosModules,
  freertosProjectCatalog,
  glossaryTerms,
  sampleLessons,
} from "./data/platformContent";

const EmbeddedAcademy = () => {
  const navigate = useNavigate();
  const [selectedPath, setSelectedPath] = useState("stm32-path");
  const [selectedPlatform, setSelectedPlatform] = useState("stm32");
  const [search, setSearch] = useState("");
  const [theme, setTheme] = useState("dark");

  const currentRoadmap = useMemo(
    () => platformRoadmaps.find((item) => item.id === selectedPath) ?? platformRoadmaps[0],
    [selectedPath]
  );

  const filteredCatalog = useMemo(() => {
    const term = search.toLowerCase();
    return microcontrollerCatalog.filter((item) => {
      return [item.family, item.focus, item.description].join(" ").toLowerCase().includes(term);
    });
  }, [search]);

  const selectedLesson = selectedPath === "freertos-path" ? sampleLessons.freeRtos : sampleLessons.stm32;

  return (
    <div className={theme === "dark" ? "bg-slate-950 text-slate-100" : "bg-slate-50 text-slate-900"}>
      <Helmet>
        <title>{academyMeta.name} | Embedded Systems Learning Platform</title>
        <meta name="description" content={academyMeta.description} />
      </Helmet>

      <header className="sticky top-0 z-30 border-b border-slate-800/80 bg-slate-950/85 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-4 py-4 sm:px-6 lg:px-8">
          <div className="inline-flex items-center gap-2 text-sm font-semibold uppercase tracking-[0.18em] text-cyan-300">
            <Icon name="Cpu" size={16} />
            Embedded Pulse
          </div>

          <nav className="hidden items-center gap-3 md:flex">
            <button type="button" onClick={() => setSelectedPath("stm32-path")} className="rounded-full border border-slate-700 px-3 py-1.5 text-sm text-slate-200 hover:border-slate-500">STM32</button>
            <button type="button" onClick={() => setSelectedPath("freertos-path")} className="rounded-full border border-slate-700 px-3 py-1.5 text-sm text-slate-200 hover:border-slate-500">FreeRTOS</button>
            <button type="button" className="rounded-full bg-cyan-400 px-4 py-2 text-sm font-semibold text-slate-950 hover:bg-cyan-300">Start Learning</button>
          </nav>

          <button
            type="button"
            onClick={() => {
              const nextTheme = theme === "dark" ? "light" : "dark";
              setTheme(nextTheme);
              localStorage.setItem("academy-theme", nextTheme);
            }}
            className="rounded-full border border-slate-700 bg-slate-900 px-3 py-1.5 text-xs font-medium text-slate-200 transition hover:border-slate-500"
          >
            {theme === "dark" ? "Light mode" : "Dark mode"}
          </button>
        </div>
      </header>

      <main className="pt-4">
        <section className="border-b border-slate-800 bg-gradient-to-br from-slate-950 via-slate-900 to-brand-primary/80">
          <div className="mx-auto max-w-7xl px-4 py-20 sm:px-6 lg:px-8">
            <div className="mb-8 flex items-center justify-between">
              <div className="inline-flex items-center gap-2 rounded-full border border-cyan-400/30 bg-cyan-400/10 px-4 py-2 text-xs font-semibold uppercase tracking-[0.18em] text-cyan-200">
                <Icon name="Cpu" size={14} />
                {academyMeta.name}
              </div>
              <button
                type="button"
                onClick={() => {
                  const nextTheme = theme === "dark" ? "light" : "dark";
                  setTheme(nextTheme);
                  localStorage.setItem("academy-theme", nextTheme);
                }}
                className="rounded-full border border-slate-700 bg-slate-900 px-3 py-1.5 text-xs font-medium text-slate-200 transition hover:border-slate-500"
              >
                {theme === "dark" ? "Light mode" : "Dark mode"}
              </button>
            </div>

            <div className="grid items-center gap-10 lg:grid-cols-[1.2fr_0.8fr]">
              <div>
                <h1 className="max-w-3xl text-4xl font-black tracking-tight text-white md:text-6xl">
                  Learn embedded systems with a real-world engineering approach.
                </h1>
                <p className="mt-6 max-w-2xl text-lg leading-8 text-slate-300">
                  {academyMeta.tagline} Explore verified STM32 content, deep RTOS fundamentals, and practical firmware workflows built from the Industry Insights Hub resources.
                </p>

                <div className="mt-8 flex flex-wrap gap-3">
                  <button onClick={() => navigate(`/courses/${selectedPath}`)} className="rounded-xl bg-cyan-400 px-5 py-3 font-semibold text-slate-950 transition hover:bg-cyan-300">
                    Start learning
                  </button>
                  <button onClick={() => navigate(`/courses/${selectedPath}`)} className="rounded-xl border border-slate-600 bg-slate-900 px-5 py-3 font-semibold text-white transition hover:border-slate-500">
                    View curriculum
                  </button>
                </div>

                <div className="mt-8 grid max-w-xl grid-cols-3 gap-4 text-left">
                  <div className="rounded-xl bg-slate-900/50 p-4 ring-1 ring-slate-700">
                    <div className="text-2xl font-bold text-white">6</div>
                    <div className="mt-1 text-xs uppercase tracking-[0.14em] text-slate-300">Platforms</div>
                  </div>
                  <div className="rounded-xl bg-slate-900/50 p-4 ring-1 ring-slate-700">
                    <div className="text-2xl font-bold text-white">8</div>
                    <div className="mt-1 text-xs uppercase tracking-[0.14em] text-slate-300">RTOS modules</div>
                  </div>
                  <div className="rounded-xl bg-slate-900/50 p-4 ring-1 ring-slate-700">
                    <div className="text-2xl font-bold text-white">24h+</div>
                    <div className="mt-1 text-xs uppercase tracking-[0.14em] text-slate-300">Hands-on learning</div>
                  </div>
                </div>
              </div>

              <div className="rounded-3xl border border-slate-700 bg-slate-900/60 p-6 shadow-2xl">
                <div className="mb-5 flex items-center justify-between">
                  <div>
                    <p className="text-xs uppercase tracking-[0.16em] text-slate-400">Learning roadmap</p>
                    <h2 className="mt-1 text-2xl font-bold text-white">{currentRoadmap.title}</h2>
                  </div>
                  <Icon name="TrendingUp" size={28} className="text-cyan-300" />
                </div>

                <div className="space-y-3">
                  {currentRoadmap.modules.map((module, index) => (
                    <div key={module} className="flex items-center gap-3 rounded-xl border border-slate-700 bg-slate-950/50 p-3">
                      <div className="flex h-8 w-8 items-center justify-center rounded-full bg-cyan-400/20 text-xs font-bold text-cyan-200">
                        {index + 1}
                      </div>
                      <span className="text-sm text-slate-200">{module}</span>
                    </div>
                  ))}
                </div>
              </div>
            </div>
          </div>
        </section>

        <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
          <div className="mb-8 flex flex-col gap-6 md:flex-row md:items-end md:justify-between">
            <div>
              <p className="text-sm font-semibold uppercase tracking-[0.18em] text-brand-secondary">Learning paths</p>
              <h2 className="mt-2 text-3xl font-bold text-text-primary">Choose your embedded journey</h2>
            </div>
            <div className="flex items-center gap-3 rounded-2xl border border-border bg-white px-4 py-3 shadow-sm">
              <Icon name="Search" size={16} className="text-text-secondary" />
              <input
                value={search}
                onChange={(event) => setSearch(event.target.value)}
                placeholder="Search courses and concepts"
                className="w-full bg-transparent text-sm text-text-primary placeholder:text-text-muted focus:outline-none md:w-72"
              />
            </div>
          </div>

          <div className="mb-8 flex flex-wrap gap-3">
            {platformRoadmaps.map((route) => (
              <button
                key={route.id}
                type="button"
                onClick={() => {
                  setSelectedPath(route.id);
                  navigate(`/courses/${route.id}`);
                }}
                className={`rounded-full px-4 py-2 text-sm font-semibold transition ${
                  selectedPath === route.id
                    ? "bg-brand-primary text-white shadow-lg"
                    : "border border-border bg-white text-text-primary hover:bg-brand-surface"
                }`}
              >
                {route.title}
              </button>
            ))}
          </div>

          <div className="grid gap-6 md:grid-cols-2 xl:grid-cols-3">
            {filteredCatalog.map((platform) => (
              <CourseCard
                key={platform.id}
                title={platform.family}
                description={platform.description}
                difficulty={platform.level.includes("Advanced") ? "Advanced" : platform.level.includes("Intermediate") ? "Intermediate" : "Beginner"}
                level={platform.level}
                duration={platform.learningHours}
                tags={[platform.focus]}
                accent={platform.id === "stm32" ? "brand-primary" : platform.id === "esp32" ? "brand-secondary" : "brand-accent"}
                onSelect={() => {
                  setSelectedPlatform(platform.id);
                  if (platform.id === "stm32") {
                    navigate("/courses/stm32-path");
                  }
                }}
              />
            ))}
          </div>
        </section>

        <section className="border-y border-slate-200 bg-slate-50">
          <div className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
            <div className="mb-8 flex items-center justify-between">
              <div>
                <p className="text-sm font-semibold uppercase tracking-[0.18em] text-brand-secondary">Platform focus</p>
                <h2 className="mt-2 text-3xl font-bold text-text-primary">{selectedPlatform.toUpperCase()} learning center</h2>
              </div>
              <div className="rounded-full bg-white px-4 py-2 text-sm text-text-secondary shadow-sm ring-1 ring-slate-200">
                Verified source material from the Industry Insights Hub
              </div>
            </div>

            <div className="grid gap-8 lg:grid-cols-[1.1fr_0.9fr]">
              <div className="space-y-6">
                <div className="rounded-3xl border border-border bg-white p-6 shadow-sm">
                  <h3 className="mb-3 text-2xl font-bold text-text-primary">Overview and learning flow</h3>
                  <p className="mb-6 text-base leading-7 text-text-secondary">
                    The embedded learning content is organized into introduction, setup, architecture, peripheral development, advanced firmware design, and practical projects. Each platform follows a progression from first principles to real-world system implementation.
                  </p>
                  <div className="grid gap-4 md:grid-cols-3">
                    {[
                      { label: "Fundamentals", detail: "Architecture and toolchain" },
                      { label: "Peripheral skills", detail: "GPIO, UART, SPI, ADC, PWM" },
                      { label: "Systems design", detail: "RTOS, DMA, debugging, optimization" },
                    ].map((item) => (
                      <div key={item.label} className="rounded-2xl border border-border bg-brand-surface p-4">
                        <div className="mb-2 text-xs uppercase tracking-[0.14em] text-text-secondary">{item.label}</div>
                        <div className="text-sm font-semibold text-text-primary">{item.detail}</div>
                      </div>
                    ))}
                  </div>
                </div>

                <div className="rounded-3xl border border-border bg-white p-6 shadow-sm">
                  <h3 className="mb-4 text-xl font-bold text-text-primary">Lesson example</h3>
                  <CodeBlock code={selectedLesson.code} language="c" />
                </div>
              </div>

              <div className="space-y-6">
                <TaskTimeline tasks={["Startup", "Core tasks", "Communication", "Timing", "Optimization"]} activeIndex={2} />

                <div className="rounded-3xl border border-border bg-white p-6 shadow-sm">
                  <h3 className="mb-4 text-xl font-bold text-text-primary">Quick assessment</h3>
                  <QuizCard
                    question="Which design pattern is most suitable for sharing sensor values between tasks?"
                    options={["Polling loop", "Queue", "Hard-coded delay", "Global variable only"]}
                    correctAnswer="Queue"
                    explanation="A queue is the safest and most scalable way to pass data between tasks while keeping timing and synchronization predictable."
                  />
                </div>
              </div>
            </div>
          </div>
        </section>

        <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
          <div className="mb-8">
            <p className="text-sm font-semibold uppercase tracking-[0.18em] text-brand-secondary">FreeRTOS Academy</p>
            <h2 className="mt-2 text-3xl font-bold text-text-primary">Real-time systems learning, from fundamentals to advanced firmware</h2>
          </div>

          <div className="grid gap-6 lg:grid-cols-[1.3fr_0.7fr]">
            <div className="space-y-4">
              {freertosModules.map((module) => (
                <div key={module.id} className="rounded-2xl border border-border bg-white p-5 shadow-sm">
                  <div className="flex items-center justify-between gap-3">
                    <h3 className="text-lg font-bold text-text-primary">{module.title}</h3>
                    <span className="rounded-full bg-brand-surface px-2.5 py-1 text-[11px] font-semibold uppercase tracking-[0.12em] text-brand-primary">
                      module
                    </span>
                  </div>
                  <ul className="mt-4 space-y-2">
                    {module.lessons.map((lesson) => (
                      <li key={lesson} className="flex items-start gap-3 text-sm text-text-secondary">
                        <span className="mt-1 h-2 w-2 rounded-full bg-cyan-500" />
                        <span>{lesson}</span>
                      </li>
                    ))}
                  </ul>
                </div>
              ))}
            </div>

            <div className="space-y-6">
              <div className="rounded-3xl border border-border bg-slate-900 p-6 text-white shadow-xl">
                <h3 className="text-xl font-bold">Interactive task scheduler</h3>
                <p className="mt-2 text-sm text-slate-300">Watch task execution, priority, and preemption as each time slice advances.</p>
                <div className="mt-6 space-y-3">
                  {["Task A (Priority 3)", "Task B (Priority 2)", "Task C (Priority 1)", "ISR service"].map((item, index) => (
                    <div key={item} className="flex items-center justify-between rounded-xl border border-slate-700 bg-slate-800 px-3 py-2 text-sm">
                      <span>{item}</span>
                      <span className={`h-3 w-3 rounded-full ${index === 0 ? "bg-emerald-400" : index === 3 ? "bg-amber-400" : "bg-sky-400"}`} />
                    </div>
                  ))}
                </div>
              </div>

              <div className="rounded-3xl border border-border bg-white p-6 shadow-sm">
                <h3 className="mb-4 text-xl font-bold text-text-primary">Real-world project path</h3>
                <div className="space-y-3">
                  {freertosProjectCatalog.map((project) => (
                    <div key={project.title} className="rounded-2xl border border-border bg-brand-surface p-4">
                      <div className="mb-2 flex items-center justify-between gap-3">
                        <span className="font-semibold text-text-primary">{project.title}</span>
                        <span className="text-xs font-medium uppercase tracking-[0.12em] text-brand-secondary">{project.difficulty}</span>
                      </div>
                      <p className="text-sm text-text-secondary">{project.summary}</p>
                      <div className="mt-3 text-xs uppercase tracking-[0.12em] text-text-muted">{project.duration}</div>
                    </div>
                  ))}
                </div>
              </div>
            </div>
          </div>
        </section>

        <section className="mx-auto max-w-7xl px-4 py-16 sm:px-6 lg:px-8">
          <div className="grid gap-8 lg:grid-cols-[1.1fr_0.9fr]">
            <div className="rounded-3xl border border-border bg-white p-6 shadow-sm">
              <div className="mb-5 flex items-center justify-between">
                <h3 className="text-2xl font-bold text-text-primary">Technical glossary</h3>
                <Icon name="BookText" size={22} className="text-brand-primary" />
              </div>
              <div className="grid gap-3 md:grid-cols-2">
                {glossaryTerms.map((item) => (
                  <div key={item.term} className="rounded-2xl border border-border bg-brand-surface p-4">
                    <div className="mb-2 text-sm font-bold uppercase tracking-[0.12em] text-brand-primary">{item.term}</div>
                    <p className="text-sm leading-6 text-text-secondary">{item.description}</p>
                  </div>
                ))}
              </div>
            </div>

            <div className="rounded-3xl border border-border bg-white p-6 shadow-sm">
              <h3 className="mb-5 text-2xl font-bold text-text-primary">Learning progression</h3>
              <div className="space-y-4">
                {[
                  { title: "Foundations", detail: "Toolchain setup, clocks, GPIO, debugging basics." },
                  { title: "Communication & control", detail: "UART, SPI, I2C, ADC, PWM, DMA, interrupts." },
                  { title: "System design", detail: "Scheduling, synchronization, low-power behavior, RTOS design." },
                  { title: "Advanced engineering", detail: "Bootloaders, optimization, complex multi-task firmware and monitoring." },
                ].map((step, index) => (
                  <div key={step.title} className="flex gap-4">
                    <div className="flex h-9 w-9 items-center justify-center rounded-full bg-brand-primary text-xs font-bold text-white">
                      {index + 1}
                    </div>
                    <div>
                      <div className="font-semibold text-text-primary">{step.title}</div>
                      <div className="text-sm text-text-secondary">{step.detail}</div>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </section>

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
                <li><button type="button" onClick={() => navigate("/courses/stm32-path")} className="hover:text-white">STM32 path</button></li>
                <li><button type="button" onClick={() => navigate("/courses/freertos-path")} className="hover:text-white">FreeRTOS path</button></li>
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
      </main>
    </div>
  );
};

export default EmbeddedAcademy;
