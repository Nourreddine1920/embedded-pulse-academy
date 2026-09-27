# Embedded Pulse Academy

A learning platform that teaches **STM32 microcontrollers** and **FreeRTOS** from embedded C basics to production firmware.

Every lesson explains each concept at three depths:

- 🟢 **Beginner**: what and why, with analogies
- 🟡 **Intermediate**: how to use it, with HAL and register-level code
- 🔴 **Advanced**: under the hood and in production

Each lesson also includes step-through animations, compiled lab code, a quiz, graded exercises and a cheat sheet.

- **Reference board:** NUCLEO-F446RE (STM32F446RE, Cortex-M4F, 180 MHz)
- **Toolchain:** STM32CubeIDE / CubeMX, `arm-none-eabi-gcc`, ST-LINK
- **RTOS:** native FreeRTOS API (v10.x / v11.x), with CMSIS-RTOS v2 notes

## Curriculum

| Part | Content | Modules |
|---|---|---|
| **A: STM32 bare-metal** | Embedded C, Cortex-M, boot, clocks, GPIO, NVIC, timers, UART/SPI/I2C, ADC/DAC, DMA, low power, bootloaders, reliability, CAN/USB, HAL vs LL | A0 – A17 |
| **B: FreeRTOS** | Tasks, scheduler, queues, semaphores, notifications, buffers, timers, ISRs, memory, debugging, tickless idle, design patterns, capstones | B0 – B16 |

The full map (lesson IDs, levels and prerequisites) lives in [`src/content/curriculum.js`](src/content/curriculum.js) and is rendered at `/lessons`.

## Getting started

Requirements: Node.js 18 or newer.

```bash
npm install
npm start          # dev server (Vite)
npm run build      # production build in dist/
npm run serve      # preview the production build
```

Routes:

| Route | Page |
|---|---|
| `/` | Home |
| `/lessons` | Curriculum map |
| `/lessons/<id>` | A lesson, e.g. `/lessons/A0.1` |

## Project structure

```text
src/
├── components/              SiteShell (header/footer), AppIcon
├── content/
│   ├── curriculum.js        Curriculum map (single source of truth)
│   ├── lessons/<id>.md      Lesson text (Markdown + frontmatter)
│   └── labs/<id>/           Lab sources shown in the lesson
│       ├── Core/Inc, Core/Src   main.c (HAL / CubeMX) and main_reg.c (register-level)
│       ├── host/            Same tests built for the PC
│       └── solutions/       Exercise solutions
├── pages/
│   ├── embedded-academy/    Home page (plus the legacy course pages)
│   └── lessons/
│       ├── LessonPage.jsx, LessonsIndexPage.jsx, lessonLoader.js
│       ├── components/      MarkdownLesson, CodeView, Mermaid, QuizBlock
│       └── animations/      Lesson animations + registry (index.js)
└── styles/
```

## Writing a lesson

Create `src/content/lessons/<id>.md`. It shows up in the curriculum and gets its own route automatically.

```markdown
---
id: A0.2
title: Bitwise operations & masks
summary: One-sentence summary (inline `code` allowed).
level: Beginner → Intermediate (with 🔴 depth)
time: ≈ 2 h 30 min
mcu: STM32F446RE · Cortex-M4F
prereqs: [A0.1]
hardware: [NUCLEO-F446RE, USB Mini-B cable]
dependsOn: [A0.1]
buildsOn: [A0.3, A5.2]
---
```

Fenced blocks with special languages become widgets:

| Fence | Renders |
|---|---|
| ` ```layer beginner ` / `intermediate` / `advanced` | Coloured level panel (use 4 backticks when it contains code) |
| ` ```callout tip title="…" ` (also `note`, `warning`, `danger`, `analogy`, `production`) | Callout box |
| ` ```anim <key> ` | Animation from `src/pages/lessons/animations/index.js` |
| ` ```mermaid caption="…" ` | Mermaid diagram (loaded on demand) |
| ` ```quiz ` | Quiz from JSON: `[{ "q", "options", "answer", "why" }]` |
| ` ```solution title="…" ` | Collapsible exercise solution |
| ` ```c file=A0.1/Core/Src/main.c ` | Lab source loaded from `src/content/labs/` |

Code shown in lessons is pulled from `src/content/labs/`, so the page always shows the exact file that was compiled. Lab code is checked with the STM32CubeIDE toolchain (GCC 10.3, `-Wall -Wextra -Wconversion -Wsign-conversion`, at `-O0` and `-O2`) before a lesson is published.

## Commit conventions

[Conventional Commits](https://www.conventionalcommits.org/): `feat(scope): …`, `fix(scope): …`, `docs: …`, `chore: …`. Lesson content uses `content(<lesson id>): …`, for example `content(A0.2): add bitwise operations lesson`.
