/**
 * Embedded Pulse Academy curriculum map (approved 2026-09-27).
 * Lesson bodies live in src/content/lessons/<id>.md; a lesson without a
 * file is shown as "coming soon". `deliveryOrder` is the writing order.
 */

const L = (id, title, level, prereqs, summary) => ({ id, title, level, prereqs, summary });

export const curriculum = [
  {
    part: "A",
    partTitle: "STM32 bare-metal",
    modules: [
      {
        id: "A0", title: "Embedded C foundations", level: "B–A", hours: 4,
        lessons: [
          L("A0.1", "Fixed-width types & integer promotion", "B–I", [], "stdint.h, type sizes on ARM, integer promotion and the usual arithmetic conversions"),
          L("A0.2", "Bitwise operations & masks", "B", ["A0.1"], "Set/clear/toggle/test, multi-bit field insert and extract, read-modify-write"),
          L("A0.3", "volatile and const", "B–A", ["A0.2"], "What the optimizer may remove, why volatile is not atomic, const data in Flash"),
          L("A0.4", "Pointers to hardware registers", "B–I", ["A0.3"], "Memory-mapped I/O, casting addresses, CMSIS __IO/__I/__O"),
          L("A0.5", "Structs, unions & bit-fields as register maps", "I–A", ["A0.4"], "CMSIS peripheral structs, padding and reserved words, bit-field portability"),
          L("A0.6", "Embedded C hygiene", "I–A", ["A0.5"], "static, inline vs macros, static_assert, a first look at MISRA-C"),
        ],
      },
      {
        id: "A1", title: "ARM Cortex-M architecture", level: "B–A", hours: 4,
        lessons: [
          L("A1.1", "Cortex-M family & the STM32 portfolio", "B", ["A0"], "M0/M0+/M3/M4/M7/M33 and which STM32 series use them"),
          L("A1.2", "Core registers", "I–A", ["A1.1"], "R0–R12, SP, LR, PC, xPSR, PRIMASK/FAULTMASK/BASEPRI, CONTROL"),
          L("A1.3", "Modes, privilege, MSP/PSP", "I–A", ["A1.2"], "Thread vs Handler mode, CONTROL.nPRIV/SPSEL, why an RTOS uses the PSP"),
          L("A1.4", "Memory map & bus matrix", "I–A", ["A1.1"], "Code/SRAM/peripheral regions, AHB/APB, bus-matrix masters, bit-banding"),
          L("A1.5", "Harvard vs von Neumann & the pipeline", "I–A", ["A1.4"], "I-Code/D-Code/System buses, 3-stage pipeline, ART accelerator"),
          L("A1.6", "The FPU", "I–A", ["A1.2"], "FPv4-SP, CPACR enable, lazy stacking, float ABI flags"),
        ],
      },
      {
        id: "A2", title: "Boot process", level: "I–A", hours: 4,
        lessons: [
          L("A2.1", "Reset sequence & boot modes", "I", ["A1.4"], "BOOT0, system bootloader, initial MSP and PC fetch"),
          L("A2.2", "The vector table", "I–A", ["A2.1"], "Layout, weak aliases to Default_Handler, VTOR relocation"),
          L("A2.3", "Startup file, line by line", "I–A", ["A2.2"], ".data copy, .bss zeroing, SystemInit, __libc_init_array, main"),
          L("A2.4", "The linker script", "A", ["A2.3"], "MEMORY/SECTIONS, LMA vs VMA, _estack, heap/stack, KEEP"),
          L("A2.5", "Lab: zero-HAL blink", "A", ["A2.4", "A5.2"], "Own startup, linker script and Makefile, no vendor code"),
        ],
      },
      {
        id: "A3", title: "Toolchain & debugging", level: "B–A", hours: 4,
        lessons: [
          L("A3.1", "CubeMX → project", "B", [], ".ioc, generated layout, USER CODE regions, safe regeneration"),
          L("A3.2", "The build process", "B–I", ["A3.1"], "Compile → link, reading the .map file, text/data/bss"),
          L("A3.3", "Flashing & SWD", "B–I", ["A3.1"], "ST-LINK, SWDIO/SWCLK/NRST, connect-under-reset, recovery"),
          L("A3.4", "Debugger fluency", "I", ["A3.3"], "FPB breakpoints, DWT watchpoints, live expressions, SFR view"),
          L("A3.5", "SWV/ITM printf & the DWT cycle counter", "I–A", ["A3.4", "A1.2"], "SWO, ITM stimulus ports, measuring cycles"),
          L("A3.6", "First look at faults", "I–A", ["A3.4"], "Catching a HardFault, reading the stacked PC/LR"),
        ],
      },
      {
        id: "A4", title: "Clock system (RCC)", level: "B–A", hours: 4,
        lessons: [
          L("A4.1", "Clock sources", "B", ["A1.4"], "HSI, HSE (8 MHz MCO on Nucleo), LSI, LSE and their accuracy"),
          L("A4.2", "PLL math", "I–A", ["A4.1"], "PLLM/N/P/Q/R, VCO limits, 8 MHz → 180 MHz, over-drive"),
          L("A4.3", "Bus prescalers & the clock tree", "I", ["A4.2"], "AHB/APB limits, timer clock ×2 rule"),
          L("A4.4", "Flash wait states & voltage scaling", "I–A", ["A4.2"], "Latency table, PWR VOS, programming order"),
          L("A4.5", "Peripheral clock gating", "I–A", ["A4.3", "A0.4"], "RCC enable registers, post-enable delay"),
          L("A4.6", "Clock Security System & MCO", "A", ["A4.4"], "CSS/NMI on HSE failure, MCO on a scope, register-level 180 MHz"),
        ],
      },
      {
        id: "A5", title: "GPIO", level: "B–A", hours: 4,
        lessons: [
          L("A5.1", "The GPIO electrical model", "B–I", ["A0.2"], "Modes, push-pull vs open-drain, pulls, speed, FT pins"),
          L("A5.2", "GPIO registers", "I", ["A5.1", "A4.5"], "MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR"),
          L("A5.3", "ODR vs BSRR: atomicity", "I–A", ["A5.2"], "Read-modify-write race with an ISR"),
          L("A5.4", "Alternate functions & pin muxing", "I", ["A5.2"], "Datasheet AF table, conflicts, CubeMX pinout"),
          L("A5.5", "Inputs & debouncing", "B–I", ["A5.2"], "Bounce on a scope, RC filters, software debouncers"),
          L("A5.6", "Lab: register-level GPIO driver", "I–A", ["A5.3"], "Clean driver API with a HAL comparison"),
        ],
      },
      {
        id: "A6", title: "Interrupts & NVIC", level: "B–A", hours: 5,
        lessons: [
          L("A6.1", "Polling vs interrupts", "B", ["A2.2"], "Exceptions vs IRQs, pending and active"),
          L("A6.2", "NVIC priorities", "I–A", ["A6.1"], "4 priority bits, PRIGROUP, preemption vs sub-priority"),
          L("A6.3", "EXTI", "I", ["A6.1", "A5.2"], "SYSCFG_EXTICR, edges, pending clear, shared vectors"),
          L("A6.4", "Exception entry & exit", "A", ["A6.2", "A1.3", "A1.6"], "Stacking, latency, tail-chaining, late arrival, EXC_RETURN"),
          L("A6.5", "ISR design rules & shared data", "I–A", ["A6.4", "A0.3"], "Atomicity, critical sections, flag-clearing race"),
        ],
      },
      {
        id: "A7", title: "Timers", level: "B–A", hours: 7,
        lessons: [
          L("A7.1", "SysTick & the HAL timebase", "B–I", ["A6.2"], "24-bit down-counter, HAL_IncTick, HAL_Delay in ISRs"),
          L("A7.2", "Timer anatomy", "B–I", ["A4.3"], "PSC/ARR/CNT, update event, preload, frequency math"),
          L("A7.3", "PWM", "I", ["A7.2", "A5.4"], "CCRx, modes 1/2, edge vs center aligned, resolution"),
          L("A7.4", "Input capture", "I–A", ["A7.2", "A6"], "Period/duty, PWM-input mode, filters, overflow"),
          L("A7.5", "Output compare & one-pulse mode", "I–A", ["A7.3"], "Precise pulse generation"),
          L("A7.6", "Encoder mode", "I–A", ["A7.4"], "Quadrature decoding, x2/x4, 16-bit wrap"),
          L("A7.7", "Advanced timers (TIM1/TIM8)", "A", ["A7.3"], "Complementary outputs, dead-time, break, MOE"),
        ],
      },
      {
        id: "A8", title: "UART/USART", level: "B–A", hours: 5,
        lessons: [
          L("A8.1", "UART framing & electrical levels", "B", ["A5.4"], "Frame format, TTL/RS-232/RS-485, Nucleo VCP"),
          L("A8.2", "Baud-rate generation & error", "I", ["A8.1", "A4.3"], "USARTDIV, OVER8, error budget"),
          L("A8.3", "Polling vs interrupt vs DMA", "I", ["A8.2", "A6"], "CPU cost and latency, status flags"),
          L("A8.4", "Ring buffers", "I–A", ["A8.3", "A6.5"], "Lock-free SPSC, power-of-two sizing"),
          L("A8.5", "IDLE-line + DMA reception", "A", ["A8.4", "A12.2"], "Variable-length frames, circular DMA"),
          L("A8.6", "printf retargeting", "B–I", ["A8.3"], "_write, newlib-nano, float printf cost"),
        ],
      },
      {
        id: "A9", title: "SPI", level: "B–A", hours: 4,
        lessons: [
          L("A9.1", "The SPI protocol", "B–I", ["A5.4"], "Signals, CPOL/CPHA modes 0–3"),
          L("A9.2", "STM32 SPI configuration", "I–A", ["A9.1", "A4.3"], "Prescaler, DFF, NSS management, flag sequence"),
          L("A9.3", "Duplex modes & the disable procedure", "A", ["A9.2"], "BIDIMODE, RX-only, shutdown sequence"),
          L("A9.4", "SPI + DMA", "I–A", ["A9.2", "A12.2"], "Paired streams, chip-select timing"),
          L("A9.5", "Lab: W25Q64 SPI NOR flash", "I–A", ["A9.4"], "JEDEC ID, page program, erase, status polling"),
        ],
      },
      {
        id: "A10", title: "I2C", level: "B–A", hours: 5,
        lessons: [
          L("A10.1", "I2C at bit level", "B–I", ["A5.1"], "START/STOP, ACK/NACK, addressing, pull-up sizing"),
          L("A10.2", "The F4 I2C peripheral", "I–A", ["A10.1", "A4.3"], "CCR/TRISE, event sequence, v1 vs v2 IP"),
          L("A10.3", "Repeated start & register reads", "I", ["A10.2"], "HAL_I2C_Mem_Read"),
          L("A10.4", "Clock stretching & error handling", "A", ["A10.3"], "BERR/ARLO/AF/OVR, timeouts, retries"),
          L("A10.5", "Bus-lock recovery", "A", ["A10.4"], "9-clock recovery, SWRST, BUSY-flag erratum"),
          L("A10.6", "Lab: BME280 sensor", "I–A", ["A10.3"], "ID check, calibration, compensated readings"),
        ],
      },
      {
        id: "A11", title: "ADC & DAC", level: "B–A", hours: 6,
        lessons: [
          L("A11.1", "Sampling theory", "B", ["A0"], "Nyquist, aliasing, quantization, ENOB"),
          L("A11.2", "F4 ADC architecture", "I", ["A11.1", "A4.3"], "SAR, ADCCLK, sampling time, conversion time"),
          L("A11.3", "Sampling time vs source impedance", "A", ["A11.2"], "R_AIN formula, worked example"),
          L("A11.4", "Conversion modes", "I–A", ["A11.2", "A7.2"], "Scan, continuous, injected, timer triggers"),
          L("A11.5", "ADC + DMA, internal channels & accuracy", "A", ["A11.4", "A12.2"], "VREFINT, temperature, software oversampling"),
          L("A11.6", "DAC", "I", ["A11.2", "A12.2"], "Buffered output, DMA waveforms"),
        ],
      },
      {
        id: "A12", title: "DMA", level: "B–A", hours: 4,
        lessons: [
          L("A12.1", "What DMA does", "B–I", ["A1.4"], "Streams, channels, request mapping"),
          L("A12.2", "Configuring a stream", "I", ["A12.1"], "Direction, widths, circular, FIFO/burst"),
          L("A12.3", "Double buffering", "I–A", ["A12.2"], "DBM, half/complete callbacks"),
          L("A12.4", "Priorities, arbitration & errors", "A", ["A12.3"], "Contention, TEIF/FEIF/DMEIF"),
          L("A12.5", "Cache coherence (F7/H7)", "A", ["A12.2"], "Clean/invalidate, MPU regions, alignment"),
        ],
      },
      {
        id: "A13", title: "Low-power modes", level: "I–A", hours: 4,
        lessons: [
          L("A13.1", "Power budget & measurement", "B–I", ["A5"], "IDD jumper, averaging duty-cycled current"),
          L("A13.2", "Sleep mode", "I", ["A13.1", "A6"], "WFI/WFE, SLEEPONEXIT"),
          L("A13.3", "Stop mode", "I–A", ["A13.2", "A4"], "Regulator modes, EXTI wake, clock restore"),
          L("A13.4", "Standby mode", "I–A", ["A13.3"], "WKUP, backup domain, reset flags"),
          L("A13.5", "RTC", "I", ["A4.1"], "Calendar, alarms, wake-up timer"),
          L("A13.6", "Debugging low-power firmware", "A", ["A13.3"], "DBGMCU_CR, measurement artifacts"),
        ],
      },
      {
        id: "A14", title: "Flash, option bytes, bootloader & CRC", level: "I–A", hours: 5,
        lessons: [
          L("A14.1", "F446 Flash organization", "I", ["A2.4"], "Sectors, erase granularity, PSIZE"),
          L("A14.2", "Programming Flash from firmware", "I–A", ["A14.1"], "Unlock, program, EEPROM emulation"),
          L("A14.3", "Option bytes", "I–A", ["A14.2"], "RDP levels, WRP, BOR level"),
          L("A14.4", "Custom bootloader & IAP", "A", ["A14.2", "A2.2"], "Memory split, jump sequence"),
          L("A14.5", "Image integrity & CRC", "A", ["A14.4"], "F4 CRC unit, A/B slots, rollback"),
        ],
      },
      {
        id: "A15", title: "Reliability", level: "I–A", hours: 5,
        lessons: [
          L("A15.1", "IWDG vs WWDG", "I", ["A7.2"], "Timeout math, windows, debug freeze"),
          L("A15.2", "Fault exceptions decoded", "A", ["A6.4", "A3.6"], "CFSR/HFSR/MMFAR/BFAR"),
          L("A15.3", "Production fault handler", "A", ["A15.2"], "Stacked-frame capture, post-mortem"),
          L("A15.4", "Stack overflow detection", "A", ["A15.2", "A2.4"], "Painting, MPU guard, -fstack-usage"),
          L("A15.5", "Defensive firmware design", "I–A", ["A15.1"], "BOR, reset causes, safe states"),
        ],
      },
      {
        id: "A16", title: "Industrial buses", level: "I–A", hours: 5,
        lessons: [
          L("A16.1", "CAN fundamentals", "B–I", ["A8.1"], "Arbitration, frames, stuffing, error states"),
          L("A16.2", "bxCAN on F446", "I–A", ["A16.1", "A6"], "Bit timing, filters, mailboxes, loopback"),
          L("A16.3", "FDCAN (G4/H7)", "A", ["A16.2"], "CAN FD, dual bit rate, message RAM"),
          L("A16.4", "USB CDC intro", "I", ["A4.2"], "OTG_FS, 48 MHz clock, CDC class"),
        ],
      },
      {
        id: "A17", title: "HAL vs LL vs registers", level: "I–A", hours: 2,
        lessons: [
          L("A17.1", "The three layers", "I", ["A5", "A8"], "What each abstracts, portability, HAL lock"),
          L("A17.2", "Measured comparison", "I–A", ["A17.1", "A3.5"], "Flash size and cycle counts"),
          L("A17.3", "Mixing layers & a decision guide", "A", ["A17.2"], "Safe combinations, production choices"),
        ],
      },
    ],
  },
  {
    part: "B",
    partTitle: "FreeRTOS",
    modules: [
      { id: "B0", title: "Why an RTOS", level: "B–I", hours: 2, lessons: [
        L("B0.1", "Super-loop limits", "B", ["A6"], "Foreground/background and latency coupling"),
        L("B0.2", "Real-time definitions", "B–I", ["B0.1"], "Hard/soft/firm, deadline, jitter, WCET"),
        L("B0.3", "When not to use an RTOS", "I–A", ["B0.2"], "Alternatives compared"),
      ] },
      { id: "B1", title: "Architecture & Cortex-M port", level: "I–A", hours: 4, lessons: [
        L("B1.1", "Source tree tour", "B–I", ["B0"], "Kernel files, port layer, heaps"),
        L("B1.2", "FreeRTOSConfig.h, option by option", "I–A", ["B1.1"], "Every key option and its cost"),
        L("B1.3", "The Cortex-M port", "A", ["B1.1", "A6.4", "A1.3"], "SysTick, PendSV, SVC"),
        L("B1.4", "CubeMX integration & the HAL timebase", "I", ["B1.2", "A7.1"], "TIM6 timebase, CMSIS-RTOS v2"),
      ] },
      { id: "B2", title: "Tasks", level: "B–A", hours: 4, lessons: [
        L("B2.1", "Creating tasks", "B", ["B1.4"], "Dynamic vs static creation"),
        L("B2.2", "Task states", "B–I", ["B2.1"], "State diagram and transitions"),
        L("B2.3", "TCB & ready lists", "A", ["B2.2"], "Kernel data structures"),
        L("B2.4", "Stack sizing methodology", "I–A", ["B2.1", "A15.4"], "Worst case, high-water mark, FPU cost"),
        L("B2.5", "Idle task, hooks & deletion", "I", ["B2.2"], "Hooks and cleanup"),
      ] },
      { id: "B3", title: "Scheduler", level: "B–A", hours: 4, lessons: [
        L("B3.1", "Scheduling policies", "B–I", ["B2.2"], "Preemptive, time slicing, cooperative"),
        L("B3.2", "Context switch, step by step", "A", ["B3.1", "B1.3"], "Stacking, PendSV, EXC_RETURN"),
        L("B3.3", "vTaskDelay vs vTaskDelayUntil", "I", ["B3.1"], "Drift vs fixed period"),
        L("B3.4", "Starvation & measuring the scheduler", "I–A", ["B3.2", "A3.5"], "Priority mistakes, switch time"),
      ] },
      { id: "B4", title: "Queues", level: "B–A", hours: 4, lessons: [
        L("B4.1", "Copy semantics", "B–I", ["B2"], "Copy vs pointer, ownership"),
        L("B4.2", "Blocking & timeouts", "I", ["B4.1"], "portMAX_DELAY, full/empty"),
        L("B4.3", "Queue sets", "I–A", ["B4.2"], "Waiting on several objects"),
        L("B4.4", "Queue design patterns", "I–A", ["B4.2"], "Command queue, mailbox, pools"),
        L("B4.5", "Queue internals", "A", ["B4.2"], "Storage and waiting lists"),
      ] },
      { id: "B5", title: "Semaphores & mutexes", level: "B–A", hours: 4, lessons: [
        L("B5.1", "Binary & counting semaphores", "B–I", ["B4"], "Signaling vs counting"),
        L("B5.2", "Mutex vs binary semaphore", "I", ["B5.1"], "Ownership and inheritance"),
        L("B5.3", "Priority inversion", "I–A", ["B5.2"], "Mars Pathfinder, inheritance limits"),
        L("B5.4", "Recursive mutexes", "I", ["B5.2"], "When they hide design smells"),
        L("B5.5", "Deadlock", "I–A", ["B5.3"], "Coffman conditions, lock ordering"),
      ] },
      { id: "B6", title: "Event groups", level: "I–A", hours: 2, lessons: [
        L("B6.1", "Event bits", "I", ["B5"], "Wait any/all, clear-on-exit"),
        L("B6.2", "Rendezvous with xEventGroupSync", "I–A", ["B6.1"], "Barriers"),
        L("B6.3", "ISR interaction & determinism", "A", ["B6.1", "B9.1"], "Deferred set-bits via the daemon"),
      ] },
      { id: "B7", title: "Task notifications", level: "I–A", hours: 3, lessons: [
        L("B7.1", "As a binary/counting semaphore", "I", ["B5.1"], "ulTaskNotifyTake"),
        L("B7.2", "As a mailbox or event bits", "I", ["B7.1"], "eSetValueWithOverwrite, eSetBits"),
        L("B7.3", "Notification arrays", "I–A", ["B7.2"], "Indexed notifications"),
        L("B7.4", "Measured performance comparison", "A", ["B7.1", "A3.5"], "DWT benchmark, limitations"),
      ] },
      { id: "B8", title: "Stream & message buffers", level: "I–A", hours: 3, lessons: [
        L("B8.1", "Stream buffers", "I", ["B7"], "Single writer/reader, trigger level"),
        L("B8.2", "Message buffers", "I", ["B8.1"], "Length-prefixed messages"),
        L("B8.3", "ISR → task byte streams", "I–A", ["B8.1", "A8.5"], "UART DMA into a stream buffer"),
        L("B8.4", "Core-to-core transfer", "A", ["B8.2"], "sbSEND_COMPLETED, dual-core AMP"),
      ] },
      { id: "B9", title: "Software timers", level: "I–A", hours: 2, lessons: [
        L("B9.1", "Timer daemon & command queue", "I", ["B4"], "How timer calls become commands"),
        L("B9.2", "One-shot vs auto-reload", "I", ["B9.1"], "Callback rules"),
        L("B9.3", "Pitfalls", "I–A", ["B9.2"], "Blocking callbacks, queue full, accuracy"),
      ] },
      { id: "B10", title: "Interrupts with FreeRTOS", level: "I–A", hours: 4, lessons: [
        L("B10.1", "FromISR APIs", "I", ["B4", "A6"], "Why separate APIs exist"),
        L("B10.2", "configMAX_SYSCALL_INTERRUPT_PRIORITY", "I–A", ["B10.1", "A6.2"], "NVIC numbering, the #1 crash"),
        L("B10.3", "Deferred interrupt processing", "I", ["B10.1", "B7"], "ISR → task patterns"),
        L("B10.4", "portYIELD_FROM_ISR", "I–A", ["B10.3"], "xHigherPriorityTaskWoken"),
        L("B10.5", "Zero-latency interrupts", "A", ["B10.2"], "Above the threshold"),
      ] },
      { id: "B11", title: "Memory management", level: "I–A", hours: 3, lessons: [
        L("B11.1", "heap_1 … heap_5 compared", "I", ["B2"], "Algorithms and determinism"),
        L("B11.2", "Fully static systems", "I–A", ["B11.1"], "Static allocation callbacks"),
        L("B11.3", "Fragmentation demo", "I–A", ["B11.1"], "Provoking and measuring it"),
        L("B11.4", "malloc-failed hook & newlib", "A", ["B11.1"], "Hook design, newlib thread safety"),
      ] },
      { id: "B12", title: "Resource management", level: "I–A", hours: 3, lessons: [
        L("B12.1", "Critical sections", "I–A", ["B10.2"], "BASEPRI masking, nesting"),
        L("B12.2", "Scheduler suspension", "I", ["B12.1"], "vTaskSuspendAll"),
        L("B12.3", "Gatekeeper task pattern", "I", ["B4"], "One owner per peripheral"),
        L("B12.4", "Reentrancy & HAL", "I–A", ["B12.3", "A17.1"], "Wrapping non-thread-safe drivers"),
      ] },
      { id: "B13", title: "Debugging & analysis", level: "I–A", hours: 4, lessons: [
        L("B13.1", "Stack overflow checking", "I", ["B2.4"], "Methods 1 and 2"),
        L("B13.2", "configASSERT strategy", "I", ["B1.2"], "Debug vs production"),
        L("B13.3", "Run-time stats", "I–A", ["B13.1", "A7.2"], "vTaskGetRunTimeStats"),
        L("B13.4", "High-water marks & thread-aware debugging", "I", ["B13.1"], "CubeIDE RTOS views"),
        L("B13.5", "SystemView / Tracealyzer", "A", ["B13.3"], "Recording and reading traces"),
      ] },
      { id: "B14", title: "Low power", level: "A", hours: 3, lessons: [
        L("B14.1", "Idle-hook sleep", "I", ["B2.5", "A13.2"], "WFI in the idle hook"),
        L("B14.2", "Tickless idle", "A", ["B14.1"], "configUSE_TICKLESS_IDLE"),
        L("B14.3", "Tickless + Stop mode", "A", ["B14.2", "A13.3", "A13.5"], "RTC wake-up timer as tick source"),
      ] },
      { id: "B15", title: "Architecture & design patterns", level: "I–A", hours: 4, lessons: [
        L("B15.1", "Task decomposition", "I", ["B4", "B10"], "Split by timing, not by feature"),
        L("B15.2", "Priority assignment", "I–A", ["B15.1"], "Rate-monotonic analysis"),
        L("B15.3", "Active objects & message-driven design", "A", ["B15.1"], "Event queue per object"),
        L("B15.4", "State machines + RTOS", "I–A", ["B15.3"], "Event-driven FSMs in tasks"),
        L("B15.5", "Layered firmware", "A", ["B15.3"], "BSP/driver/service/app, host testing"),
      ] },
      { id: "B16", title: "Capstones", level: "A", hours: 30, lessons: [
        L("B16.1", "Multi-sensor data logger", "A", ["A8", "A9", "A10", "A12", "B13"], "I2C + SPI + UART + DMA"),
        L("B16.2", "DC motor speed control", "A", ["A7", "B3", "B10", "B15.2"], "PWM + encoder + PID task"),
        L("B16.3", "CAN node", "A", ["A16.2", "B12"], "Two-node network, bus-off recovery"),
        L("B16.4", "Production hardening review", "A", ["A15", "B13"], "Budgets, WCET, release checklist"),
      ] },
    ],
  },
];

/** Flat list of every lesson with its module attached. */
export const allLessons = curriculum.flatMap((part) =>
  part.modules.flatMap((module) =>
    module.lessons.map((lesson) => ({ ...lesson, moduleId: module.id, moduleTitle: module.title, part: part.part }))
  )
);

/** Writing/reading order agreed for the course (DMA moved before UART). */
const moduleOrder = [
  "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A12", "A8", "A9", "A10", "A11",
  "A13", "A14", "A15", "A16", "A17",
  "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12", "B13", "B14", "B15", "B16",
];

export const deliveryOrder = moduleOrder.flatMap((moduleId) =>
  allLessons.filter((lesson) => lesson.moduleId === moduleId)
);

/** Finds a lesson or a module by id ("A0.1" or "A0"). */
export const findCurriculumItem = (id) =>
  allLessons.find((lesson) => lesson.id === id) ||
  curriculum.flatMap((part) => part.modules).find((module) => module.id === id);
