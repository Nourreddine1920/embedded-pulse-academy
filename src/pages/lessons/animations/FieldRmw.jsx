import React from "react";
import BitRow, { BitRuler, hex, toBits } from "./BitRow";
import Stepper, { CodeLine, Verdict } from "./Stepper";

const BEFORE = 0xa8000ca0; // PA15/14/13 = AF (debug), PA5 = analog (11), PA3/PA2 = AF
const FIELD = 0x00000c00; // GPIO_MODER_MODER5_Msk
const NOT_FIELD = ~FIELD >>> 0;
const CLEARED = (BEFORE & NOT_FIELD) >>> 0;
const SETMASK = 0x00000400; // 1UL << GPIO_MODER_MODER5_Pos
const AFTER = (CLEARED | SETMASK) >>> 0;
const OR_ONLY = (BEFORE | SETMASK) >>> 0;
const OVERWRITE = SETMASK;

const inField = (idx) => idx === 10 || idx === 11;
const debugBits = (idx) => idx >= 26;

const steps = [
  { label: "Read", caption: "MODIFY_REG starts with one read of GPIOA->MODER. Two bits per pin: PA5's field is bits 11:10, currently 11 (analog mode). Bits 31:26 are PA15–PA13 in AF mode, which is the SWD debug connection." },
  { label: "Build ~mask", caption: "CLEARMASK = GPIO_MODER_MODER5_Msk = 0x00000C00. Inverted, it is all ones except the two field bits." },
  { label: "AND: clear the field", caption: "value & ~mask forces bits 11:10 to 00 and leaves the other 30 bits exactly as they were." },
  { label: "OR: insert the new value", caption: "| (1UL << 10) writes 01 (general-purpose output) into the now-empty field." },
  { label: "Write back", caption: "One store puts the result back. Only PA5 changed mode. This is the correct way to change a multi-bit field." },
  { label: "Bug 1: OR only", caption: "MODER |= (1UL << 10) can only set bits, never clear them: 11 | 01 = 11. PA5 stays in analog mode and the LED never lights. The field must be cleared first." },
  { label: "Bug 2: plain write", caption: "MODER = 0x400 sets PA5 correctly but writes 0 to every other field. PA13/PA14 (SWDIO/SWCLK) become inputs and the debugger disconnects instantly. Recovery: connect under reset (A3.3)." },
];

const FieldRmw = () => (
  <Stepper title="Animation · Changing one 2-bit field safely (MODIFY_REG)" steps={steps}>
    {(step) => (
      <div className="space-y-2">
        <CodeLine
          parts={
            step === 5
              ? [["GPIOA->MODER |= (1UL << 10);", true]]
              : step === 6
              ? [["GPIOA->MODER = 0x400UL;", true]]
              : [
                  ["MODIFY_REG(GPIOA->MODER, ", false],
                  ["GPIO_MODER_MODER5_Msk", step === 1 || step === 2],
                  [", ", false],
                  ["1UL << GPIO_MODER_MODER5_Pos", step === 3],
                  [");", false],
                ]
          }
        />
        <BitRuler />
        {step <= 4 && <BitRow bits={toBits(BEFORE, 32)} label="MODER (read)" caption={hex(BEFORE, 32)} idPrefix="r" tone={(i, b, idx) => (inField(idx) ? "focus" : debugBits(idx) && b ? "sign" : b ? "base" : "zero")} />}
        {step >= 1 && step <= 2 && <BitRow bits={toBits(NOT_FIELD, 32)} label="~mask" caption={hex(NOT_FIELD, 32)} idPrefix="m" tone={(i, b, idx) => (inField(idx) ? "promoted" : "dim")} />}
        {step >= 2 && step <= 4 && <BitRow bits={toBits(CLEARED, 32)} label="& ~mask" caption={hex(CLEARED, 32)} idPrefix="c" tone={(i, b, idx) => (inField(idx) ? "promoted" : b ? "base" : "zero")} />}
        {step >= 3 && step <= 4 && <BitRow bits={toBits(AFTER, 32)} label="| 1UL<<10" caption={hex(AFTER, 32)} idPrefix="a" tone={(i, b, idx) => (inField(idx) ? "good" : b ? "base" : "zero")} />}
        {step === 4 && (
          <div className="sticky left-0 w-max pt-2 sm:pl-[124px]">
            <Verdict ok>PA5 = 01 (output), debug pins untouched</Verdict>
          </div>
        )}
        {step === 5 && (
          <>
            <BitRow bits={toBits(BEFORE, 32)} label="MODER" caption={hex(BEFORE, 32)} idPrefix="r" tone={(i, b, idx) => (inField(idx) ? "focus" : b ? "base" : "zero")} />
            <BitRow bits={toBits(OR_ONLY, 32)} label="after |=" caption={hex(OR_ONLY, 32)} idPrefix="a" tone={(i, b, idx) => (inField(idx) ? "bad" : b ? "base" : "zero")} />
            <div className="sticky left-0 w-max pt-2 sm:pl-[124px]">
              <Verdict ok={false}>field = 11, still analog</Verdict>
            </div>
          </>
        )}
        {step === 6 && (
          <>
            <BitRow bits={toBits(BEFORE, 32)} label="MODER" caption={hex(BEFORE, 32)} idPrefix="r" tone={(i, b, idx) => (debugBits(idx) && b ? "sign" : b ? "base" : "zero")} />
            <BitRow bits={toBits(OVERWRITE, 32)} label="after =" caption={hex(OVERWRITE, 32)} idPrefix="a" tone={(i, b, idx) => ((BEFORE >>> idx) & 1 && !b ? "bad" : inField(idx) && b ? "good" : b ? "base" : "zero")} />
            <div className="sticky left-0 w-max pt-2 sm:pl-[124px]">
              <Verdict ok={false}>SWD pins lost: debugger disconnects</Verdict>
            </div>
          </>
        )}
      </div>
    )}
  </Stepper>
);

export default FieldRmw;
