import React from "react";
import BitRow, { BitRuler, toBits } from "./BitRow";
import Stepper, { CodeLine, Verdict } from "./Stepper";

const A = 0x0f;
const NOT_A = 0xfffffff0; // ~ applied to the promoted int
const F0 = 0xf0;

const steps = [
  { label: "Declare", caption: "a is an 8-bit unsigned variable holding 0x0F (0000 1111). So far, nothing surprising." },
  {
    label: "Integer promotion",
    caption:
      "Before ~ is applied, C promotes a to int, because int (32 bits on ARM) can hold every uint8_t value. The 24 new upper bits are zeros (amber), because the value is unsigned.",
  },
  {
    label: "Apply ~",
    caption: "~ now flips ALL 32 bits, including the 24 zeros promotion just added. The result is the int 0xFFFFFFF0, which is -16.",
  },
  {
    label: "Compare",
    caption:
      "0xF0 is an int constant: 0x000000F0. The upper 24 bits differ (red), so the comparison is always false. GCC folds it to a constant 0 and warns under -Wextra.",
  },
  {
    label: "The fix",
    caption:
      "Cast back to the width you mean: (uint8_t)~a keeps only the low 8 bits (1111 0000). When promoted again for the comparison it becomes 0x000000F0, which matches.",
  },
];

const compareTone = (other) => (i, bit) => (bit === other[i] ? "good" : "bad");

const PromotionComplement = () => (
  <Stepper title="Animation · Why ~a == 0xF0 is false" steps={steps}>
    {(step) => {
      const code = [
        ["uint8_t a = 0x0F;  ", step === 0],
        ["if (", false],
        [step === 4 ? "(uint8_t)~a" : "~a", step >= 1 && step !== 3],
        [" == 0xF0", step === 3 || step === 4],
        [") { … }", false],
      ];
      const promoted = toBits(A, 32);
      const notA = toBits(NOT_A, 32);
      const f0 = toBits(F0, 32);

      return (
        <div className="space-y-2">
          <CodeLine parts={code} />
          <BitRuler />
          {step === 0 && <BitRow bits={toBits(A, 8)} label="a  (uint8_t)" caption="0x0F = 15" idPrefix="a" />}
          {step === 1 && (
            <BitRow bits={promoted} label="a → int" caption="0x0000000F" idPrefix="a" tone={(i, bit) => (i < 24 ? "promoted" : bit ? "base" : "zero")} />
          )}
          {step === 2 && (
            <BitRow bits={notA} label="~a  (int)" caption="0xFFFFFFF0 = −16" idPrefix="a" tone={(i) => (i < 24 ? "promoted" : "focus")} />
          )}
          {step === 3 && (
            <>
              <BitRow bits={notA} label="~a" caption="0xFFFFFFF0" idPrefix="a" tone={compareTone(f0)} />
              <BitRow bits={f0} label="0xF0  (int)" caption="0x000000F0" idPrefix="c" tone={compareTone(notA)} />
              <div className="pl-0 pt-3 sm:pl-[124px] sticky left-0 w-max">
                <Verdict ok={false}>~a == 0xF0 → false</Verdict>
              </div>
            </>
          )}
          {step === 4 && (
            <>
              <BitRow bits={toBits(0xf0, 8)} label="(uint8_t)~a" caption="0xF0" idPrefix="a" tone={() => "good"} />
              <BitRow bits={f0} label="0xF0  (int)" caption="0x000000F0" idPrefix="c" tone={(i, bit) => (i < 24 ? "zero" : bit ? "good" : "zero")} />
              <div className="pl-0 pt-3 sm:pl-[124px] sticky left-0 w-max">
                <Verdict ok>(uint8_t)~a == 0xF0 → true</Verdict>
              </div>
            </>
          )}
        </div>
      );
    }}
  </Stepper>
);

export default PromotionComplement;
