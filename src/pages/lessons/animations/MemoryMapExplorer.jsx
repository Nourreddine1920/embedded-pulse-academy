import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* STM32F446 memory map (RM0390 "Memory map", Cortex-M4 architectural regions). */
const REGIONS = [
  { id: "ppb", name: "Cortex-M4 internal (PPB)", range: "0xE000 0000 – 0xE00F FFFF", tone: "fuchsia" },
  { id: "fmc", name: "FMC / QUADSPI (external memory)", range: "0x6000 0000 – 0xDFFF FFFF", tone: "slate" },
  { id: "periph", name: "Peripherals", range: "0x4000 0000 – 0x5FFF FFFF", tone: "cyan" },
  { id: "sram", name: "SRAM (128 KiB)", range: "0x2000 0000 – 0x2001 FFFF", tone: "emerald" },
  { id: "code", name: "Code: Flash · System memory · OTP", range: "0x0000 0000 – 0x1FFF FFFF", tone: "amber" },
];

const TONE = {
  fuchsia: "border-fuchsia-400/70 bg-fuchsia-500/15 text-fuchsia-100",
  slate: "border-slate-600 bg-slate-800/70 text-slate-300",
  cyan: "border-cyan-400/70 bg-cyan-500/15 text-cyan-100",
  emerald: "border-emerald-400/70 bg-emerald-500/15 text-emerald-100",
  amber: "border-amber-400/70 bg-amber-500/15 text-amber-100",
};

/* What the zoom panel shows at each step. */
const frames = [
  { region: null, code: "/* 2^32 bytes: 0x00000000 … 0xFFFFFFFF */", rows: [] },
  {
    region: "code",
    code: "const uint32_t table[] = {…};   /* lives at 0x0800xxxx */",
    rows: [
      ["0x0800 0000", "Main Flash, 512 KiB (your program)"],
      ["0x1FFF 0000", "System memory: ST bootloader"],
      ["0x1FFF 7A10", "Unique device ID, 96 bits"],
      ["0x1FFF 7A22", "Flash size in KiB (16-bit)"],
    ],
  },
  {
    region: "sram",
    code: "uint32_t counter;   /* &counter == 0x2000xxxx */",
    rows: [
      ["0x2000 0000", "SRAM1, 112 KiB: .data, .bss, heap"],
      ["0x2001 C000", "SRAM2, 16 KiB"],
      ["0x2001 FFFF", "Top of RAM: initial stack pointer grows down from here"],
    ],
  },
  {
    region: "periph",
    code: "#define PERIPH_BASE 0x40000000UL",
    rows: [
      ["0x4000 0000", "APB1: TIM2…7, USART2 (0x4000 4400), I2C, DAC"],
      ["0x4001 0000", "APB2: TIM1/8, USART1/6, ADC, SPI1, EXTI"],
      ["0x4002 0000", "AHB1: GPIOA…H, CRC, RCC (0x4002 3800), DMA"],
      ["0x5000 0000", "AHB2: USB OTG FS, DCMI"],
    ],
  },
  {
    region: "periph",
    code: "#define GPIOA_BASE (AHB1PERIPH_BASE + 0x0000UL)   /* 0x40020000 */",
    rows: [
      ["+0x00", "MODER   mode, 2 bits per pin"],
      ["+0x10", "IDR     input levels"],
      ["+0x14", "ODR     output latch"],
      ["+0x18", "BSRR    set/reset (write-only)"],
      ["+0x20", "AFR[0]  alternate functions 0–7"],
    ],
    focus: 2,
  },
  {
    region: "periph",
    code: "*(volatile uint32_t *)0x40020014UL   /* GPIOA->ODR */",
    decode: true,
  },
  {
    region: "ppb",
    code: "SCB->CPUID   DBGMCU->IDCODE",
    rows: [
      ["0xE000 E010", "SysTick"],
      ["0xE000 E100", "NVIC"],
      ["0xE000 ED00", "SCB: CPUID = 0x410FC241 (Cortex-M4 r0p1)"],
      ["0xE004 2000", "DBGMCU_IDCODE: DEV_ID 0x421 = STM32F446"],
    ],
  },
];

const steps = [
  { label: "One address space", caption: "A Cortex-M4 sees a single 4 GB address space. Flash, RAM, peripheral registers and the core's own control registers are all just address ranges in it. There are no special I/O instructions: a register is read with LDR and written with STR, exactly like a variable." },
  { label: "Code region", caption: "Flash starts at 0x0800 0000 (and is aliased at 0 when booting from Flash). Just below 0x2000 0000 sits ST's system memory, which also holds factory data: the unique ID at 0x1FFF 7A10 and the flash size at 0x1FFF 7A22." },
  { label: "SRAM", caption: "Your variables live here. The compiler owns this region: it may keep a variable in a CPU register, reorder or merge accesses, because ordinary memory has no side effects." },
  { label: "Peripherals", caption: "From 0x4000 0000 the addresses are not memory at all. Each one is wired to a register inside a peripheral. Reading or writing them makes hardware do something: that's memory-mapped I/O." },
  { label: "Zoom: GPIOA", caption: "Each peripheral gets a block of addresses. GPIOA's block starts at 0x4002 0000 and holds ten 32-bit registers at fixed offsets. The reference manual lists them as offsets from the block's base." },
  { label: "Decode 0x40020014", caption: "An absolute register address is simply base + offset. The bus matrix routes 0x4002 xxxx to AHB1, the AHB1 decoder selects the GPIOA slot, and the low bits select ODR inside it." },
  { label: "Private peripheral bus", caption: "0xE000 0000 upwards belongs to the Cortex-M4 itself (ARM defines it, identical on every M4): SysTick, NVIC, SCB. ST's debug unit DBGMCU sits at 0xE004 2000. The lab reads CPUID and IDCODE from here." },
];

const Decode = () => {
  const parts = [
    { hex: "4", label: "peripheral region", sub: "0x4000 0000", cls: "text-cyan-200 ring-cyan-400/60 bg-cyan-500/15" },
    { hex: "002", label: "AHB1 bus", sub: "+0x0002 0000", cls: "text-amber-200 ring-amber-400/60 bg-amber-500/15" },
    { hex: "00", label: "GPIOA slot", sub: "+0x0000", cls: "text-emerald-200 ring-emerald-400/60 bg-emerald-500/15" },
    { hex: "14", label: "ODR offset", sub: "+0x14", cls: "text-fuchsia-200 ring-fuchsia-400/60 bg-fuchsia-500/15" },
  ];
  return (
    <div className="space-y-4">
      <div className="flex items-end gap-1 font-mono text-2xl font-bold">
        <span className="text-slate-500">0x</span>
        {parts.map((p, i) => (
          <motion.div key={p.label} initial={{ opacity: 0, y: -10 }} animate={{ opacity: 1, y: 0 }} transition={{ delay: i * 0.25 }} className="flex flex-col items-center">
            <span className={`rounded px-1.5 ring-1 ${p.cls}`}>{p.hex}</span>
            <span className="mt-1 text-[10px] font-normal text-slate-400">{p.label}</span>
            <span className="text-[10px] font-normal text-slate-500">{p.sub}</span>
          </motion.div>
        ))}
      </div>
      <p className="font-mono text-xs text-slate-300">0x4000 0000 + 0x0002 0000 + 0x0000 + 0x14 = 0x4002 0014</p>
      <Verdict ok>GPIOA_BASE + GPIO_ODR offset = GPIOA-&gt;ODR</Verdict>
    </div>
  );
};

const MemoryMapExplorer = () => (
  <Stepper title="Animation · The STM32F446 memory map, from 4 GB down to one register" steps={steps}>
    {(step) => {
      const f = frames[step];
      return (
        <div className="max-w-[760px]">
          <CodeLine parts={[[f.code, true]]} />
          <div className="flex flex-wrap gap-4">
            <div className="w-[250px] shrink-0 space-y-1.5">
              <p className="font-mono text-[10px] text-slate-500">0xFFFF FFFF</p>
              {REGIONS.map((r) => {
                const active = f.region === r.id || f.region === null;
                return (
                  <motion.div
                    key={r.id}
                    animate={{ opacity: active ? 1 : 0.3, scale: f.region === r.id ? 1.03 : 1 }}
                    transition={{ duration: 0.35 }}
                    className={`rounded-lg border px-3 py-2 ${TONE[r.tone]}`}
                  >
                    <p className="text-xs font-semibold">{r.name}</p>
                    <p className="font-mono text-[10px] opacity-80">{r.range}</p>
                  </motion.div>
                );
              })}
              <p className="font-mono text-[10px] text-slate-500">0x0000 0000</p>
            </div>
            <div className="min-w-[280px] flex-1 rounded-xl border border-slate-800 bg-slate-900/60 p-4">
              {f.decode ? (
                <Decode />
              ) : f.rows.length === 0 ? (
                <p className="text-sm leading-6 text-slate-400">
                  Every box on the left is a range of addresses. The CPU issues an address on the bus; the bus matrix decides which memory or peripheral answers.
                </p>
              ) : (
                <ol className="space-y-1.5">
                  {f.rows.map(([addr, what], i) => (
                    <motion.li
                      key={addr}
                      initial={{ opacity: 0, x: 12 }}
                      animate={{ opacity: 1, x: 0 }}
                      transition={{ delay: i * 0.12 }}
                      className={`flex gap-3 rounded px-2 py-1 font-mono text-xs ${f.focus === i ? "bg-cyan-400/20 text-cyan-100 ring-1 ring-cyan-400/60" : "text-slate-300"}`}
                    >
                      <span className="w-24 shrink-0 text-slate-400">{addr}</span>
                      <span>{what}</span>
                    </motion.li>
                  ))}
                </ol>
              )}
            </div>
          </div>
        </div>
      );
    }}
  </Stepper>
);

export default MemoryMapExplorer;
