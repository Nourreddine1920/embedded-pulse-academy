import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine } from "./Stepper";

/* Sections as placed by the STM32CubeIDE linker script (STM32F446RETX_FLASH.ld). */
const FLASH = [
  { id: "vec", name: ".isr_vector", note: "vector table" },
  { id: "text", name: ".text", note: "code" },
  { id: "rodata", name: ".rodata", note: "const data, string literals" },
  { id: "dataimg", name: ".data image", note: "initial values, copied at reset" },
];
const SRAM = [
  { id: "data", name: ".data", note: "initialised variables" },
  { id: "bss", name: ".bss", note: "zeroed at reset" },
  { id: "heap", name: "heap", note: "malloc (avoid)" },
  { id: "stack", name: "stack", note: "locals, grows down" },
];

const cases = [
  { code: "static const uint32_t k_table[16] = { … };", hit: ["rodata"], flash: 64, ram: 0, label: "static const array", caption: "A const object with static storage and a constant initialiser goes to .rodata, which the linker script places in Flash. It costs 64 bytes of Flash and zero RAM, and the startup code never touches it." },
  { code: "static uint32_t s_table[16] = { 1 };", hit: ["data", "dataimg"], flash: 64, ram: 64, label: "initialised, not const", caption: "Not const, so it must be writable: .data in SRAM. Its initial values also live in Flash, and the reset handler copies them into RAM before main(). It costs both." },
  { code: "static uint32_t s_zero[16];", hit: ["bss"], flash: 0, ram: 64, label: "zero-initialised", caption: "No initialiser (or all zeros): .bss. The reset handler clears it with a loop, so there's no Flash image. It costs RAM only." },
  { code: "static const char *s_names[] = { \"idle\", \"run\" };", hit: ["data", "dataimg", "rodata"], flash: 17, ram: 8, label: "pointer array: the classic trap", caption: "const applies to the chars, not to the pointers. The strings are in .rodata, but the array of 2 pointers is writable, so it lands in .data: RAM plus a Flash copy. Tables of strings in firmware are full of this." },
  { code: "static const char *const k_names[] = { \"idle\", \"run\" };", hit: ["rodata"], flash: 17, ram: 0, label: "const pointers to const chars", caption: "The second const makes the pointers themselves read-only. Now the whole table is .rodata: zero RAM. Read declarations right to left: k_names is an array of const pointers to const char." },
  { code: "void f(void) { const uint32_t lut[4] = { 1, 2, 3, 4 }; … }", hit: ["stack", "rodata"], flash: 16, ram: 16, label: "const LOCAL", caption: "A const local is still an automatic variable: it's created on the stack on every call, copied from a template in .rodata (the lab measures it at an SRAM address). const only means 'this code won't write it'." },
  { code: "void f(void) { static const uint32_t lut[4] = { 1, 2, 3, 4 }; … }", hit: ["rodata"], flash: 16, ram: 0, label: "static const local", caption: "Add static and the table exists once, in .rodata, with no copy and no stack use. This is what you want for lookup tables inside functions." },
  { code: "const volatile uint32_t *const idr = &GPIOA->IDR;", hit: [], flash: 0, ram: 0, label: "const volatile: a read-only register", caption: "GPIOA->IDR is at 0x4002 0010, a peripheral address outside both regions. const = our code may not write it, volatile = the hardware changes it. CMSIS spells this __I (or __IM). The final const makes the pointer itself fixed, so the compiler can fold it into the code." },
];

const steps = cases.map(({ label, caption }) => ({ label, caption }));

const Region = ({ title, range, rows, hit }) => (
  <div className="min-w-[230px] flex-1 rounded-xl border border-slate-800 bg-slate-900/60 p-3">
    <p className="text-xs font-bold uppercase tracking-[0.14em] text-cyan-300">{title}</p>
    <p className="mb-2 font-mono text-[11px] text-slate-500">{range}</p>
    <div className="space-y-1.5">
      {rows.map((r) => {
        const on = hit.includes(r.id);
        return (
          <motion.div
            key={r.id}
            animate={{ scale: on ? 1.02 : 1 }}
            className={`flex items-center justify-between rounded-lg border px-2.5 py-1.5 font-mono text-xs transition ${on ? "border-amber-400/70 bg-amber-500/15 text-amber-100" : "border-slate-800 bg-slate-950 text-slate-500"}`}
          >
            <span className="font-bold">{r.name}</span>
            <span className="ml-3 text-[10px]">{r.note}</span>
          </motion.div>
        );
      })}
    </div>
  </div>
);

const ConstPlacement = () => (
  <Stepper title="Animation · Where does it live? Flash or SRAM" steps={steps}>
    {(step) => {
      const c = cases[step];
      return (
        <div className="max-w-[680px] space-y-4">
          <CodeLine parts={[[c.code, true]]} />
          <div className="flex flex-wrap gap-3">
            <Region title="Flash · 512 KB" range="0x0800 0000 – 0x0807 FFFF" rows={FLASH} hit={c.hit} />
            <Region title="SRAM · 128 KB" range="0x2000 0000 – 0x2001 FFFF" rows={SRAM} hit={c.hit} />
          </div>
          <div className="flex gap-4 font-mono text-xs">
            <span className={c.ram ? "text-rose-300" : "text-emerald-300"}>RAM: {c.ram} B</span>
            <span className="text-slate-300">Flash: {c.flash} B</span>
            {c.hit.length === 0 && <span className="text-amber-300">peripheral space (0x4000 0000+)</span>}
          </div>
        </div>
      );
    }}
  </Stepper>
);

export default ConstPlacement;
