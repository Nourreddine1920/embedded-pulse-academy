import React from "react";
import { motion } from "framer-motion";
import Stepper, { CodeLine, Verdict } from "./Stepper";

/* GPIOx register map, RM0390 "GPIO registers". GPIOA base 0x4002 0000. */
const RM = [
  [0x00, "GPIOA_MODER"],
  [0x04, "GPIOA_OTYPER"],
  [0x08, "GPIOA_OSPEEDR"],
  [0x0c, "GPIOA_PUPDR"],
  [0x10, "GPIOA_IDR"],
  [0x14, "GPIOA_ODR"],
  [0x18, "GPIOA_BSRR"],
  [0x1c, "GPIOA_LCKR"],
  [0x20, "GPIOA_AFRL"],
  [0x24, "GPIOA_AFRH"],
];

const GOOD = ["MODER", "OTYPER", "OSPEEDR", "PUPDR", "IDR", "ODR", "BSRR", "LCKR", "AFR[0]", "AFR[1]"];
/* Same struct with LCKR forgotten: everything after BSRR slides down by 4. */
const BROKEN = ["MODER", "OTYPER", "OSPEEDR", "PUPDR", "IDR", "ODR", "BSRR", "AFR[0]", "AFR[1]", null];

const addr = (off) => `0x4002 00${off.toString(16).toUpperCase().padStart(2, "0")}`;
const offs = (off) => `+0x${off.toString(16).toUpperCase().padStart(2, "0")}`;

const steps = [
  {
    label: "The reference manual's view",
    caption: "RM0390 lists GPIOA's registers as fixed addresses: base 0x4002 0000 plus an offset. Ten 32-bit registers, back to back, 40 bytes in total.",
  },
  {
    label: "Cast the base to a struct pointer",
    caption: "#define GPIOA ((GPIO_TypeDef *) 0x40020000UL). No memory is allocated: the struct is a stencil laid over addresses that already exist. Its first member sits at the base.",
  },
  {
    label: "Members fall on consecutive words",
    caption: "Every member is a uint32_t, so each one starts 4 bytes after the previous one, with no padding. The member order is the register order, and offsetof() equals the RM offset.",
  },
  {
    label: "GPIOA->ODR = base + offsetof",
    caption: "The compiler turns GPIOA->ODR into 'register holding the base, plus 20': LDR r3, [r2, #20]. One base address in a register serves every member, which is why struct access is compact.",
  },
  {
    label: "Arrays of registers: AFR[2]",
    caption: "AFRL and AFRH have the same layout, so CMSIS declares them as an array: AFR[0] at 0x20 and AFR[1] at 0x24. The index is simply pin / 8.",
  },
  {
    label: "Bug: one member missing",
    caption: "Leave out LCKR and every later member slides down 4 bytes. AFR[0] now lands on LCKR, AFR[1] on AFRL. Setting PA9 to AF7 through AFR[1] actually writes AFRL (pins 0–7). The compiler has no way to know.",
  },
  {
    label: "Fix: pin the layout at compile time",
    caption: "_Static_assert(offsetof(my_gpio_t, AFR) == 0x20U, ...) turns the silent bug into a build error. CMSIS headers are generated from ST's register descriptions; hand-written maps need these checks.",
  },
];

const RowCell = ({ children, tone }) => {
  const TONES = {
    dim: "border-slate-800 bg-slate-950 text-slate-500",
    base: "border-slate-700 bg-slate-900 text-slate-200",
    focus: "border-cyan-400/80 bg-cyan-500/15 text-cyan-100",
    good: "border-emerald-400/70 bg-emerald-500/15 text-emerald-200",
    bad: "border-rose-400/80 bg-rose-500/20 text-rose-200",
  };
  return <div className={`rounded-md border px-2 py-1 font-mono text-xs transition-colors ${TONES[tone]}`}>{children}</div>;
};

const StructOverlay = () => (
  <Stepper title="Animation · Laying a struct over GPIOA" steps={steps}>
    {(step) => {
      const members = step >= 5 ? BROKEN : GOOD;
      const code =
        step === 0
          ? "/* RM0390: GPIOA registers at 0x40020000 + offset */"
          : step === 3
            ? "GPIOA->ODR = x;   /* LDR r2, =0x40020000 ; STR r3, [r2, #20] */"
            : step === 4
              ? "GPIOA->AFR[9 / 8] |= 7UL << ((9 % 8) * 4);   /* AFRH */"
              : step === 5
                ? "typedef struct { …BSRR; /* LCKR forgotten */ uint32_t AFR[2]; } my_gpio_t;"
                : step === 6
                  ? "_Static_assert(offsetof(my_gpio_t, AFR) == 0x20U, \"GPIO AFRL offset\");"
                  : "#define GPIOA ((GPIO_TypeDef *) 0x40020000UL)";

      const memberTone = (i, name) => {
        if (step < 1) return "dim";
        if (step === 1) return i === 0 ? "focus" : "dim";
        if (step === 3) return name === "ODR" ? "focus" : "base";
        if (step === 4) return name && name.startsWith("AFR") ? "focus" : "base";
        if (step >= 5) return i >= 7 ? "bad" : "base";
        return "good";
      };

      return (
        <div className="space-y-3">
          <CodeLine parts={[[code, true]]} />
          <div className="grid grid-cols-[auto_auto_auto_auto] items-center gap-x-3 gap-y-1.5">
            <div className="font-mono text-[10px] uppercase tracking-wider text-slate-500">address</div>
            <div className="font-mono text-[10px] uppercase tracking-wider text-slate-500">RM0390 register</div>
            <div className="font-mono text-[10px] uppercase tracking-wider text-slate-500">offset</div>
            <div className="font-mono text-[10px] uppercase tracking-wider text-slate-500">struct member</div>
            {RM.map(([off, name], i) => {
              const member = members[i];
              const tone = memberTone(i, member);
              return (
                <React.Fragment key={off}>
                  <RowCell tone={step === 3 && off === 0x14 ? "focus" : "base"}>{addr(off)}</RowCell>
                  <RowCell tone={step >= 5 && i >= 7 ? "good" : "base"}>{name}</RowCell>
                  <RowCell tone="dim">{offs(off)}</RowCell>
                  <motion.div key={`${step >= 5 ? "b" : "g"}-${i}`} initial={{ opacity: 0, x: -12 }} animate={{ opacity: step >= 1 ? 1 : 0.25, x: 0 }} transition={{ duration: 0.35, delay: step === 2 ? i * 0.06 : 0 }}>
                    <RowCell tone={member ? tone : "dim"}>
                      {member ? `GPIOA->${member}` : step >= 5 ? "(nothing)" : ""}
                      {member && step >= 5 && i >= 7 && <span className="ml-2 text-rose-300">≠ {name.replace("GPIOA_", "")}</span>}
                    </RowCell>
                  </motion.div>
                </React.Fragment>
              );
            })}
          </div>
          {step === 3 && <Verdict ok>0x40020000 + 0x14 = 0x40020014</Verdict>}
          {step === 5 && <Verdict ok={false}>AFR[1] writes AFRL: wrong pins reconfigured</Verdict>}
          {step === 6 && <Verdict ok>error: static assertion failed: "GPIO AFRL offset"</Verdict>}
        </div>
      );
    }}
  </Stepper>
);

export default StructOverlay;
