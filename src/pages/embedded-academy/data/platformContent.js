export const academyMeta = {
  name: "Embedded Pulse Academy",
  tagline: "Embedded systems learning for engineers, students, and firmware developers.",
  description:
    "A structured learning platform built from the verified microcontroller and embedded resources already present in the Industry Insights Hub, expanded with professional teaching flows for STM32, FreeRTOS, and practical embedded engineering.",
};

export const platformRoadmaps = [
  {
    id: "stm32-path",
    title: "STM32 Learning Path",
    modules: [
      "STM32 Fundamentals",
      "Peripheral Programming",
      "Timers, DMA & Interrupts",
      "RTOS & System Architecture",
      "Advanced Debugging & Optimization",
    ],
  },
  {
    id: "freertos-path",
    title: "FreeRTOS Academy",
    modules: [
      "RTOS Foundations",
      "Tasks & Scheduling",
      "Queues, Semaphores & Mutexes",
      "Memory & Heap Design",
      "Interrupt Integration",
      "Real-world STM32 Projects",
    ],
  },
];

export const microcontrollerCatalog = [
  {
    id: "stm32",
    family: "STM32",
    focus: "ARM Cortex-M, HAL driver development, RTOS and peripheral programming",
    level: "Beginner to Advanced",
    learningHours: "28h",
    description:
      "Professional-grade ARM Cortex-M microcontrollers from STMicroelectronics with rich peripheral sets, excellent documentation, strong HAL support, and broad industrial use.",
  },
  {
    id: "esp32",
    family: "ESP32",
    focus: "IoT, Wi-Fi, BLE, MQTT, low-power embedded applications",
    level: "Beginner to Intermediate",
    learningHours: "20h",
    description:
      "Dual-core ESP32 devices are ideal for connected devices, sensor hubs, cloud telemetry, and low-power wireless systems.",
  },
  {
    id: "arduino",
    family: "Arduino",
    focus: "Rapid prototyping, sensors, interaction design, education",
    level: "Beginner",
    learningHours: "14h",
    description:
      "A beginner-friendly platform for fast prototyping, teaching electronics fundamentals, and building interactive embedded projects.",
  },
  {
    id: "rp2040",
    family: "Raspberry Pi Pico / RP2040",
    focus: "PIO, dual-core processing, USB and custom protocols",
    level: "Intermediate to Advanced",
    learningHours: "18h",
    description:
      "The RP2040 platform offers a compact, powerful dual-core MCU with flexible programmable input/output, USB, and custom hardware interfaces.",
  },
  {
    id: "atmega328p",
    family: "ATmega328P",
    focus: "AVR fundamentals, timers, interrupts, and bare-metal engineering",
    level: "Beginner to Intermediate",
    learningHours: "16h",
    description:
      "A classic 8-bit AVR part that teaches low-level register programming and real-time control with a minimal footprint.",
  },
  {
    id: "pic",
    family: "PIC Microcontrollers",
    focus: "Industrial control, peripheral-driven design, MCLP ecosystem",
    level: "Intermediate",
    learningHours: "12h",
    description:
      "PICs remain relevant in industrial automation and cost-sensitive embedded products, particularly when paired with Microchip tools and documentation.",
  },
];

export const freertosModules = [
  {
    id: "intro",
    title: "Module 1: Introduction to RTOS",
    lessons: [
      "What is an RTOS and why it is used",
      "Bare-metal vs RTOS-based development",
      "Real-time systems and deterministic behavior",
      "Hard, soft, and firm real-time systems",
      "FreeRTOS architecture and main components",
    ],
  },
  {
    id: "tasks",
    title: "Module 2: Tasks and Scheduling",
    lessons: [
      "Task creation and deletion",
      "Task states and transitions",
      "Priority and preemption",
      "Cooperative vs preemptive scheduling",
      "Context switching and stack management",
      "Idle and timer tasks",
      "Scheduling examples",
    ],
  },
  {
    id: "comms",
    title: "Module 3: Inter-task Communication",
    lessons: [
      "Queues and queue management",
      "Semaphores and counting semaphores",
      "Mutexes and recursive mutexes",
      "Direct-to-task notifications",
      "Event groups",
      "Stream and message buffers",
      "Choosing the right communication mechanism",
    ],
  },
  {
    id: "memory",
    title: "Module 4: Memory Management",
    lessons: [
      "FreeRTOS heap management",
      "heap_1, heap_2, heap_3, heap_4, heap_5",
      "Static vs dynamic allocation",
      "Stack overflow detection",
      "Fragmentation and optimization",
    ],
  },
  {
    id: "interrupts",
    title: "Module 5: Interrupts and Hardware Integration",
    lessons: [
      "ISRs and task-safe APIs",
      "Deferred interrupt processing",
      "Hardware timer integration",
      "DMA integration",
      "Interrupt-to-task communication",
      "Practical STM32 examples",
    ],
  },
  {
    id: "advanced",
    title: "Module 6: Advanced FreeRTOS",
    lessons: [
      "Priority inversion and inheritance",
      "Deadlocks and race conditions",
      "Timing analysis and scheduling",
      "Tickless idle and low-power design",
      "Software timers",
      "Runtime statistics and monitoring",
      "Debugging and performance optimization",
    ],
  },
  {
    id: "stm32-integration",
    title: "Module 7: FreeRTOS on STM32",
    lessons: [
      "STM32CubeIDE setup",
      "FreeRTOSConfig.h configuration",
      "CMSIS-RTOS2 vs native APIs",
      "STM32 HAL integration",
      "Multi-task firmware design",
      "UART monitoring tasks",
      "DMA + interrupt integration",
      "Practical STM32 projects with code",
    ],
  },
  {
    id: "projects",
    title: "Module 8: Real-world Projects",
    lessons: [
      "Multitasking LED controller",
      "UART command-line interface",
      "Sensor acquisition using queues and DMA",
      "Real-time data logger",
      "Multitasking IoT monitoring system",
      "Low-power sensor monitoring",
    ],
  },
];

export const freertosProjectCatalog = [
  {
    title: "Multitasking LED Controller",
    summary: "A simple scheduler example that manages periodic blinking, event-driven LED states, and task priorities.",
    difficulty: "Beginner",
    duration: "2h",
  },
  {
    title: "UART Command-Line Interface",
    summary: "Build a terminal-driven interface using multiple tasks for parsing, logging, and command execution.",
    difficulty: "Intermediate",
    duration: "3h",
  },
  {
    title: "Sensor Acquisition with Queues and DMA",
    summary: "Collect ADC data efficiently, distribute it between tasks, and process it without blocking the system.",
    difficulty: "Intermediate",
    duration: "4h",
  },
  {
    title: "Real-time Data Logger",
    summary: "Combine timers, queues, and UART logging into a robust embedded telemetry workflow.",
    difficulty: "Intermediate",
    duration: "4h",
  },
  {
    title: "Multitasking IoT Monitoring System",
    summary: "Create a coordinated application for sensor acquisition, alarm handling, and reporting with FreeRTOS tasks.",
    difficulty: "Advanced",
    duration: "5h",
  },
  {
    title: "Low-power Sensor Monitoring",
    summary: "Combine task scheduling, tickless idle, and event-driven design to develop energy-efficient firmware.",
    difficulty: "Advanced",
    duration: "5h",
  },
];

export const glossaryTerms = [
  { term: "RTOS", description: "Real-Time Operating System used to schedule tasks deterministically in embedded systems." },
  { term: "Priority inversion", description: "A condition where a lower-priority task holds a resource needed by a higher-priority task." },
  { term: "ISR", description: "Interrupt Service Routine, a short handler that executes in response to hardware or software interrupts." },
  { term: "Queue", description: "A data structure used for inter-task communication and buffering of messages." },
  { term: "Semaphore", description: "A synchronization primitive used to control access to shared resources or signal events." },
  { term: "Mutex", description: "A mutual exclusion primitive that protects shared data and prevents concurrent access." },
  { term: "DMA", description: "Direct Memory Access allows high-speed transfers without CPU intervention." },
  { term: "Tickless idle", description: "Low-power mode that reduces idle tick overhead while preserving RTOS scheduling." },
];

export const sampleLessons = {
  freeRtos: {
    title: "FreeRTOS Scheduler Overview",
    code: `#include "FreeRTOS.h"
#include "task.h"

static void vTaskBlink(void *pvParameters) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    xTaskCreate(vTaskBlink, "BLINK", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}`,
  },
  stm32: {
    title: "STM32 GPIO and Clock Fundamentals",
    code: `GPIO_InitTypeDef gpio = {0};

__HAL_RCC_GPIOA_CLK_ENABLE();

gpio.Pin = GPIO_PIN_5;
gpio.Mode = GPIO_MODE_OUTPUT_PP;
gpio.Pull = GPIO_NOPULL;
gpio.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &gpio);`,
  },
};

export const learningPathDetails = {
  "stm32-path": {
    eyebrow: "STM32 path",
    title: "STM32 Embedded Systems Mastery",
    summary:
      "A complete pathway for learning how to design, program, debug, and optimize real STM32 firmware with a professional embedded engineering mindset.",
    level: "Beginner to Advanced",
    duration: "28h",
    focus: "Cortex-M, HAL, DMA, peripherals, debugging",
    audience: ["Students", "Embedded engineers", "Firmware developers", "IoT builders"],
    overview:
      "This track turns STM32 from a board into a system design discipline. It teaches the MCU architecture, clock and power fundamentals, GPIO and communication interfaces, timers, interrupts, low-level debugging, RTOS integration, and practical project execution. Each module focuses on a real capability you need in embedded product development.",
    outcomes: [
      "Configure STM32 clocks, pins, interrupts, and debug interfaces confidently",
      "Build low-level drivers for GPIO, UART, SPI, I2C, ADC, PWM, and timers",
      "Use DMA and interrupts to design efficient, responsive firmware",
      "Architect, debug, and optimize production-ready embedded applications",
      "Integrate RTOS concepts into STM32 projects while preserving reliability",
      "Deliver practical firmware projects from concept to validation",
    ],
    prerequisites: [
      "Basic C programming and function design",
      "Understanding of variables, loops, arrays, and pointers",
      "Willingness to work with hardware and debug tools",
      "Comfortable using a development board and IDE",
    ],
    tools: ["STM32CubeIDE", "STM32CubeMX", "ST-Link", "HAL Drivers", "Oscilloscope", "Logic Analyzer"],
    modules: [
      {
        slug: "stm32-foundations",
        title: "Module 1: STM32 Foundations",
        focus: "Architecture, board bring-up, and toolchain basics",
        duration: "3h",
        description: "Understand the STM32 architecture, board startup flow, and toolchain essentials required to create stable firmware from the first line of code.",
        objectives: [
          "Explain the Cortex-M architecture and STM32 internal blocks",
          "Set up an STM32 development environment and board configuration workflow",
          "Understand clocks, reset states, and GPIO fundamentals",
          "Use debugging tools to inspect code execution and peripheral state",
        ],
        skills: ["MCU architecture", "Clock setup", "GPIO debugging", "CubeIDE workflow"],
        lessons: [
          "STM32 family overview and Cortex-M architecture",
          "Clock tree, reset, boot sequence, and power domains",
          "GPIO and pin multiplexing fundamentals",
          "Project setup in STM32CubeIDE and CubeMX",
          "Debugging with breakpoints, watch variables, and serial output",
        ],
        lab: "Create a minimal LED blink project, verify the clock configuration, and observe startup behavior on hardware.",
        deliverables: ["Board bring-up checklist", "Clock review notes", "LED firmware test", "Debugging log"],
        takeaway: "The first module builds the habit of understanding the chip before writing application code so every later firmware feature is grounded in the actual hardware architecture.",
      },
      {
        slug: "peripheral-programming",
        title: "Module 2: Peripheral Programming",
        focus: "USART, SPI, I2C, ADC, and PWM control",
        duration: "4h",
        description: "Learn how to configure and use the fundamental external interfaces that make STM32 boards useful in real-world embedded systems.",
        objectives: [
          "Configure GPIO and special-function pins for input and output roles",
          "Implement digital and analog peripheral communication safely",
          "Read sensor and actuator signals using UART, SPI, I2C, ADC, and PWM",
          "Understand data flow from hardware layer to software decisions",
        ],
        skills: ["Peripheral configuration", "UART debugging", "ADC handling", "PWM generation"],
        lessons: [
          "GPIO configuration for input and output modes",
          "UART communication and serial protocol debugging",
          "SPI and I2C sensor interfacing patterns",
          "ADC conversion and calibration workflow",
          "PWM generation for motor and control applications",
        ],
        lab: "Read sensor data from a peripheral, process it in firmware, and output diagnostics over UART.",
        deliverables: ["Peripheral driver notes", "UART capture output", "ADC readings log", "PWM verification results"],
        takeaway: "Peripherals are the bridge between the MCU and the real world, and mastering them makes every practical project possible.",
      },
      {
        slug: "timers-and-interrupts",
        title: "Module 3: Timers and Interrupts",
        focus: "Scheduling, hardware timing, event-driven control",
        duration: "4h",
        description: "Master the timing primitives that give embedded firmware deterministic behavior and responsive event handling.",
        objectives: [
          "Use timers for periodic execution and measurement",
          "Deploy interrupts for efficient event-driven control",
          "Understand NVIC priority and response timing",
          "Build robust control loops with timing precision",
        ],
        skills: ["Timer configuration", "Interrupt design", "Event-driven logic", "Control timing"],
        lessons: [
          "Basic timer configuration and counting modes",
          "Input capture and output compare techniques",
          "Interrupt vector handling and NVIC priorities",
          "Debouncing, signal timing, and event triggers",
          "Precision timing for control loops and measurement",
        ],
        lab: "Build a precise periodic control loop with timer interrupts and measure timing accuracy on hardware.",
        deliverables: ["Timer configuration sheet", "Interrupt trace", "Timing benchmark results", "Debounce implementation"],
        takeaway: "Interrupt-driven firmware is the foundation of responsive embedded systems; the timing discipline you learn here is essential for stable control.",
      },
      {
        slug: "dma-and-data-flow",
        title: "Module 4: DMA and Data Flow",
        focus: "Efficient transfers and high-speed communication",
        duration: "4h",
        description: "Explore how DMA eliminates processor bottlenecks and improves the performance of high-speed data acquisition and logging systems.",
        objectives: [
          "Understand DMA transfer modes and memory interaction patterns",
          "Configure ADC and UART data paths using circular buffering",
          "Offload data movement from the CPU safely and predictably",
          "Build high-throughput firmware with lower latency and lower power draw",
        ],
        skills: ["DMA architecture", "Circular buffers", "High-speed logging", "CPU offloading"],
        lessons: [
          "DMA fundamentals and data transfer modes",
          "ADC-to-memory streaming and circular buffers",
          "UART DMA for logging and data acquisition",
          "DMA and interrupt coordination patterns",
          "Avoiding CPU bottlenecks in embedded firmware",
        ],
        lab: "Stream ADC data into memory and transmit the results without blocking the microcontroller.",
        deliverables: ["DMA transfer diagram", "Circular buffer design", "Signal inspection report", "Efficient logging flow"],
        takeaway: "DMA is the tool that turns a slow microcontroller into a high-performance signal-processing platform when used with disciplined data flow design.",
      },
      {
        slug: "rtos-and-system-architecture",
        title: "Module 5: RTOS and System Architecture",
        focus: "Scheduling, tasks, synchronization, and system design",
        duration: "5h",
        description: "Move beyond single-loop programming and design a proper multi-task architecture for reliable, scalable STM32 firmware.",
        objectives: [
          "Split a firmware project into meaningful tasks and responsibilities",
          "Use synchronization primitives to protect shared resources",
          "Reason about priorities, task blocking, and timing latency",
          "Design systems that stay deterministic under real operating conditions",
        ],
        skills: ["Task design", "RTOS synchronization", "Priority management", "System architecture"],
        lessons: [
          "Task design for sensor, control, and communication loops",
          "Queues, semaphores, and synchronization primitives",
          "Priority management and preemption basics",
          "Shared resource protection and deadlock awareness",
          "Designing deterministic embedded systems",
        ],
        lab: "Implement a multitasking firmware application that reads inputs, processes control logic, and reports results.",
        deliverables: ["Task architecture diagram", "Queue and semaphore design", "System timing notes", "RTOS validation checklist"],
        takeaway: "A strong system architecture makes firmware easier to debug, easier to extend, and far more reliable in production-like environments.",
      },
      {
        slug: "advanced-debugging-and-optimization",
        title: "Module 6: Advanced Debugging and Optimization",
        focus: "Performance tuning, inspection, and production readiness",
        duration: "4h",
        description: "Learn how to isolate faults, improve system quality, and make embedded firmware efficient, observable, and production-ready.",
        objectives: [
          "Diagnose runtime issues from logs, memory, timing, and signal observations",
          "Use watchdogs, fault handling, and runtime inspection to improve resilience",
          "Optimize code size, performance, and power usage without destabilizing the system",
          "Validate firmware quality with structured engineering checks",
        ],
        skills: ["Fault diagnosis", "Optimization", "Power tuning", "Validation workflows"],
        lessons: [
          "Fault analysis with watchdogs and fault handlers",
          "Memory footprint optimization and code profiling",
          "Low-power states and wake-up strategies",
          "Board bring-up and signal integrity checks",
          "Testing, validation, and firmware release practices",
        ],
        lab: "Diagnose a system issue, profile the firmware, and optimize the final design for stability and efficiency.",
        deliverables: ["Root cause analysis", "Optimization report", "Power observation notes", "Release checklist"],
        takeaway: "Professional embedded design is not just working firmware—it is stable firmware that can be measured, debugged, and improved under pressure.",
      },
    ],
    projects: [
      {
        title: "Smart Sensor Node",
        level: "Beginner",
        summary: "Create a system that reads temperature, humidity, and button events, then transmits the data over UART or BLE for monitoring.",
        deliverables: ["Schematics review", "Sensor driver", "UART log output", "Validation notes"],
      },
      {
        title: "Motor Speed Controller",
        level: "Intermediate",
        summary: "Use timers, ADC, and PID-style control logic to regulate motor speed and respond to changing conditions in real time.",
        deliverables: ["PWM generation", "ADC feedback loop", "Control algorithm", "Signal analysis"],
      },
      {
        title: "RTOS Industrial Monitor",
        level: "Advanced",
        summary: "Build a production-style firmware system with multiple tasks, communication buffers, alarms, and monitoring loops.",
        deliverables: ["Task architecture", "Queue sync design", "Alarm logic", "Debug report"],
      },
    ],
    assessment: [
      "Configure a complete STM32 project from scratch and explain the role of each peripheral block.",
      "Diagnose communication failures by checking wiring, clocks, interrupts, and DMA behavior.",
      "Design a multi-task architecture with safe inter-task communication and stable priorities.",
      "Produce a documented embedded project with verification steps and performance observations.",
    ],
  },
  "freertos-path": {
    eyebrow: "FreeRTOS path",
    title: "FreeRTOS Real-Time Systems Mastery",
    summary:
      "A detailed RTOS training path for engineers who want to master task scheduling, synchronization, memory design, hardware integration, and real-time embedded application development.",
    level: "Intermediate to Advanced",
    duration: "24h",
    focus: "Tasks, queues, semaphores, scheduling, STM32 + RTOS",
    audience: ["Firmware engineers", "Embedded developers", "System designers", "Students learning RTOS"],
    overview:
      "This learning track explains how FreeRTOS works under the hood and how to use it effectively in real embedded systems. From tasks and priorities to mutexes, memory allocation, and STM32 integration, each module builds the engineering thinking needed for dependable real-time applications.",
    outcomes: [
      "Understand the FreeRTOS scheduler, task states, and context switching flow",
      "Use queues, semaphores, mutexes, and notifications for safe task communication",
      "Manage memory strategies and detect heap issues in real-time systems",
      "Integrate FreeRTOS with STM32 peripherals and interrupt-driven logic",
      "Design low-power and deterministic embedded software architectures",
      "Debug scheduling issues, priority inversion, and race conditions effectively",
    ],
    prerequisites: [
      "C language fundamentals and embedded firmware basics",
      "Knowledge of interrupts and microcontroller timers",
      "Comfort with debugging, serial logs, and project structure",
      "Familiarity with an IDE such as STM32CubeIDE or VS Code",
    ],
    tools: ["FreeRTOS Kernel", "STM32CubeIDE", "STM32 HAL", "Trace tools", "Serial terminal", "Power analyzer"],
    modules: [
      {
        slug: "rtos-foundations",
        title: "Module 1: RTOS Foundations",
        focus: "Why RTOS matters and what makes a real-time system deterministic",
        duration: "3h",
        description: "Understand the core concepts behind real-time systems and why RTOS design becomes necessary when tasks must respond predictably under load.",
        objectives: [
          "Compare bare-metal and RTOS-driven embedded architecture",
          "Define real-time constraints and why determinism matters",
          "Learn the FreeRTOS kernel structure and scheduling model",
          "Know when an RTOS adds value and when a simple loop is enough",
        ],
        skills: ["RTOS basics", "Real-time thinking", "Kernel concepts", "System trade-offs"],
        lessons: [
          "Bare-metal versus RTOS-based design",
          "Hard, soft, and firm real-time system definitions",
          "FreeRTOS kernel components and lifecycle",
          "Scheduling principles and deterministic behavior",
          "When to use RTOS and when not to",
        ],
        lab: "Compare a polling approach against a task scheduler and explain the trade-offs in latency, responsiveness, and complexity.",
        deliverables: ["RTOS architecture notes", "Timing comparison sheet", "System decision memo", "Scheduler concept sketch"],
        takeaway: "Before writing an RTOS application, you need to define what must be deterministic and what can be deferred; that decision drives the entire architecture.",
      },
      {
        slug: "tasks-and-scheduling",
        title: "Module 2: Tasks and Scheduling",
        focus: "Lifecycle, states, priorities, and scheduling decisions",
        duration: "3h",
        description: "Learn how tasks are created, scheduled, and prioritized so your firmware can remain responsive and predictable.",
        objectives: [
          "Create and manage FreeRTOS tasks with proper priorities",
          "Understand task states and how tasks move through them",
          "Analyze preemption and blocking behavior in a scheduler",
          "Plan stack sizing and system behavior carefully",
        ],
        skills: ["Task scheduling", "Priority design", "System timing", "Stack planning"],
        lessons: [
          "Task creation, deletion, states, and priorities",
          "Preemptive scheduling and context switching",
          "Idle task and timer task behavior",
          "Stack sizing and task memory planning",
          "Priority assignment strategies in embedded systems",
        ],
        lab: "Create a multi-task design with different priorities and visualize task execution timing through logs.",
        deliverables: ["Task state diagram", "Priority plan", "Stack estimate", "Execution trace report"],
        takeaway: "Good task design is about more than creating code—it is about assigning responsibilities and timing in a deterministic system.",
      },
      {
        slug: "inter-task-communication",
        title: "Module 3: Inter-task Communication",
        focus: "Queues, semaphores, mutexes, and tasks synchronization",
        duration: "4h",
        description: "Understand the communication and synchronization primitives that safely pass information and protect shared resources between tasks.",
        objectives: [
          "Use queues and notifications to transfer data safely",
          "Apply semaphores and mutexes in real-world scenarios",
          "Design communication patterns for sensor processing, alarms, and user interfaces",
          "Avoid deadlocks and unstable timing caused by poor synchronization",
        ],
        skills: ["Queues", "Semaphores", "Mutexes", "Task synchronization"],
        lessons: [
          "Queues as message buffers and data transfer channels",
          "Counting semaphores and binary semaphores",
          "Mutexes and recursive mutex usage",
          "Direct notifications and event groups",
          "Choosing the right communication mechanism for each problem",
        ],
        lab: "Build a producer-consumer application where sensor data is queued, processed, and distributed to multiple tasks safely.",
        deliverables: ["Queue design", "Synchronization diagram", "Event flow notes", "Safety review"],
        takeaway: "The right synchronization primitive is the difference between a task framework that scales and one that becomes unstable under pressure.",
      },
      {
        slug: "memory-and-heap-design",
        title: "Module 4: Memory and Heap Design",
        focus: "Stack safety, heap management, and memory optimization",
        duration: "3h",
        description: "Design safer memory strategies so your real-time system remains stable, efficient, and under predictable resource limits.",
        objectives: [
          "Understand FreeRTOS heap implementations and resource constraints",
          "Decide between static and dynamic memory strategies",
          "Detect stack and heap issues before they affect runtime reliability",
          "Tune resource usage for low-power and embedded efficiency",
        ],
        skills: ["Heap management", "Stack planning", "Dynamic allocation", "Resource tuning"],
        lessons: [
          "Heap_1 to heap_5 overview",
          "Static versus dynamic allocation trade-offs",
          "Stack overflow detection and allocation planning",
          "Fragmentation and optimization strategies",
          "Memory profiling in constrained embedded systems",
        ],
        lab: "Audit a project for memory misuse, estimate stack requirements, and tune allocation for reliable runtime behavior.",
        deliverables: ["Memory budget table", "Heap selection rationale", "Stack analysis", "Optimization notes"],
        takeaway: "Memory design is critical in RTOS systems because the biggest faults often come from hidden resource pressure rather than obvious logic bugs.",
      },
      {
        slug: "interrupts-and-hardware-integration",
        title: "Module 5: Interrupts and Hardware Integration",
        focus: "Synchronizing hardware events with tasks and ISRs",
        duration: "4h",
        description: "Connect hardware interrupts to RTOS tasks without violating real-time constraints or causing unsafe shared-state behavior.",
        objectives: [
          "Separate short interrupt work from longer task logic",
          "Use deferred processing to keep ISRs short and safe",
          "Integrate timers, DMA, and communication pathways with RTOS tasks",
          "Create a clean hardware-to-software event pipeline",
        ],
        skills: ["ISR design", "Deferred processing", "DMA flow", "Hardware synchronization"],
        lessons: [
          "ISR characteristics and task-safe API usage",
          "Deferred processing with queues and notifications",
          "Timer-driven tasks and interrupt orchestration",
          "DMA-to-task integration and buffering",
          "Safe design patterns for interactive hardware",
        ],
        lab: "Handle sensor edge events and UART input using an interrupt-first design that hands work to RTOS tasks safely.",
        deliverables: ["ISR design doc", "Deferred processing flow", "Hardware event map", "Interrupt validation log"],
        takeaway: "The safest embedded patterns keep the ISR small, fast, and deterministic while the RTOS handles the non-critical processing work.",
      },
      {
        slug: "advanced-rtos-patterns",
        title: "Module 6: Advanced RTOS Patterns",
        focus: "Prevention of deadlocks, priority inversion, and unstable timing",
        duration: "3h",
        description: "Go beyond basic task usage and learn the design patterns that keep complex RTOS applications robust and maintainable.",
        objectives: [
          "Identify and avoid priority inversion and critical-section mistakes",
          "Diagnose unstable task behavior and deadlock scenarios",
          "Improve scheduling quality and reduce jitter in embedded systems",
          "Create resilient designs for production environments",
        ],
        skills: ["Priority inversion analysis", "Deadlock prevention", "Timing analysis", "Fault handling"],
        lessons: [
          "Priority inversion and priority inheritance",
          "Deadlock detection and avoidance patterns",
          "Race conditions and critical sections",
          "Timing analysis and performance metrics",
          "Software timers and tickless idle design",
        ],
        lab: "Analyze a stuck or unstable system, identify the root cause in task ordering and locking, and redesign the flow.",
        deliverables: ["Failure mode analysis", "Critical section review", "Stability checklist", "Timing optimization notes"],
        takeaway: "Advanced RTOS engineering is about preventing subtle failures before they appear under real load, not just making the code compile.",
      },
      {
        slug: "freertos-on-stm32",
        title: "Module 7: FreeRTOS on STM32",
        focus: "Real device integration using ST hardware and HAL",
        duration: "4h",
        description: "Apply FreeRTOS in real STM32 projects with proper configuration, device integration, and communication patterns.",
        objectives: [
          "Configure FreeRTOS on STM32 with the correct middleware and settings",
          "Integrate RTOS tasks with HAL and peripheral drivers",
          "Design multi-tasking around MCU hardware constraints",
          "Build maintainable firmware for device-level monitoring and control",
        ],
        skills: ["STM32 integration", "FreeRTOSConfig", "HAL task design", "Embedded monitoring"],
        lessons: [
          "STM32CubeIDE project setup with FreeRTOS",
          "FreeRTOSConfig.h configuration and memory tuning",
          "CMSIS-RTOS2 and native FreeRTOS API choices",
          "Task and queue integration with HAL drivers",
          "UART task monitoring, DMA, and logging strategies",
        ],
        lab: "Create a full STM32 application where a sensor task, control task, and debug task share resources without blocking each other.",
        deliverables: ["STM32 RTOS project", "Hardware integration notes", "Task communication map", "Monitoring design"],
        takeaway: "When RTOS concepts meet real hardware, the goal is not just working tasks—it is a system that scales, responds predictably, and remains debuggable.",
      },
      {
        slug: "production-rtos-projects",
        title: "Module 8: Production RTOS Projects",
        focus: "Complete embedded system design and validation",
        duration: "5h",
        description: "Bring everything together in realistic projects that mimic production-grade firmware development and testing.",
        objectives: [
          "Assemble a complete embedded application using multiple tasks and interfaces",
          "Design monitoring, control, and feedback systems in one architecture",
          "Validate reliability, data flow, and timing under realistic conditions",
          "Package the work as a professional firmware design exercise",
        ],
        skills: ["System integration", "Project validation", "Monitoring design", "Production thinking"],
        lessons: [
          "Multitasking LED and actuator controller",
          "Command-line interface using UART tasks",
          "Continuous sensor acquisition with queue safety",
          "Real-time data logger design",
          "Low-power monitoring and event-driven architecture",
        ],
        lab: "Implement a complete real-world RTOS project and validate mission-critical behavior with logs, timing checks, and stable execution.",
        deliverables: ["Project architecture", "Firmware validation plan", "System logs", "Production-ready summary"],
        takeaway: "Real-world embedded engineering is the ability to coordinate hardware, software, timing, and diagnostics into a single dependable system.",
      },
    ],
    projects: [
      {
        title: "UART Command Console",
        level: "Beginner",
        summary: "Build a command-driven system with separate tasks for parsing, execution, and user feedback using a real-time scheduler.",
        deliverables: ["Task model", "Queue protocol", "Terminal commands", "Validation logs"],
      },
      {
        title: "Sensor Fusion Node",
        level: "Intermediate",
        summary: "Collect readings from multiple sensors, guard shared resources, and publish processed values to a monitoring task.",
        deliverables: ["Semaphore design", "Queue processing", "Data validation", "Timing chart"],
      },
      {
        title: "Industrial Monitoring System",
        level: "Advanced",
        summary: "Develop a robust RTOS-based monitoring product with alarms, task isolation, interrupt handling, and system health reporting.",
        deliverables: ["Task architecture", "Heap sizing", "Error handling", "System test report"],
      },
    ],
    assessment: [
      "Explain how a FreeRTOS task transitions between states and how scheduling is affected by priority and blocking.",
      "Choose the correct synchronization primitives for queues, mutexes, and notification-driven workflows.",
      "Evaluate heap and stack decisions to prevent crashes, memory leaks, and blocked tasks.",
      "Build and validate a complete STM32 + FreeRTOS firmware project under realistic embedded constraints.",
    ],
  },
};
