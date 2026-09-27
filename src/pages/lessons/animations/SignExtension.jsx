import React from "react";
import BitRow, { BitRuler, toBits } from "./BitRow";
import Stepper, { CodeLine, Verdict } from "./Stepper";

const MSB = 0xff;
const LSB = 0x38;
const RAW = 0xff38;           // 65336 as int
const SIGN_EXTENDED = 0xffffff38; // -200 as int32

const steps = [
  { label: "Bytes from the sensor", caption: "A temperature sensor sends a signed 16-bit value as two bytes: MSB 0xFF, LSB 0x38. The datasheet says the value is −200 (−2.00 °C at 0.01 °C/LSB)." },
  { label: "msb << 8", caption: "msb is promoted to int (32 bits) and shifted left by 8, giving 0x0000FF00. Bit 15 is 1, but in a 32-bit int the sign bit is bit 31, and that's 0." },
  { label: "| lsb", caption: "OR in the low byte: 0x0000FF38. As a 32-bit int this is +65 336, not −200. That's the bug: the 16-bit sign bit (pink) is now just an ordinary bit." },
  { label: "Cast to int16_t", caption: "(int16_t) keeps the low 16 bits and treats bit 15 as the sign bit. 0xFF38 read as two's complement is −200. (GCC defines this out-of-range conversion as modulo 2¹⁶.)" },
  { label: "Sign extension", caption: "Assigning the int16_t to an int32_t sign-extends: bit 15 is copied into bits 16–31 (the SXTH instruction). Result: 0xFFFFFF38 = −200. Correct." },
];

const SignExtension = () => (
  <Stepper title="Animation · Rebuilding a signed sensor value" steps={steps}>
    {(step) => (
      <div className="space-y-2">
        <CodeLine
          parts={[
            ["int32_t t = ", false],
            [step >= 3 ? "(int16_t)" : "", step >= 3],
            ["((msb << 8)", step === 1],
            [" | lsb)", step === 2],
            [";", false],
          ]}
        />
        <BitRuler />
        {step === 0 && (
          <>
            <BitRow bits={toBits(MSB, 8)} label="msb (uint8_t)" caption="0xFF" idPrefix="m" />
            <BitRow bits={toBits(LSB, 8)} label="lsb (uint8_t)" caption="0x38" idPrefix="l" />
          </>
        )}
        {step === 1 && (
          <BitRow
            bits={toBits(MSB << 8, 32)}
            label="msb << 8 (int)"
            caption="0x0000FF00"
            idPrefix="r"
            tone={(i, bit, idx) => (idx === 15 ? "sign" : idx === 31 ? "focus" : bit ? "base" : "zero")}
          />
        )}
        {step === 2 && (
          <>
            <BitRow bits={toBits(RAW, 32)} label="… | lsb (int)" caption="0x0000FF38 = +65 336" idPrefix="r" tone={(i, bit, idx) => (idx === 15 ? "sign" : idx === 31 ? "bad" : bit ? "base" : "zero")} />
            <div className="pt-2 sm:pl-[124px] sticky left-0 w-max">
              <Verdict ok={false}>t = 65336 (expected −200)</Verdict>
            </div>
          </>
        )}
        {step === 3 && (
          <BitRow bits={toBits(RAW, 16)} label="(int16_t)" caption="0xFF38 = −200" idPrefix="r" tone={(i, bit, idx) => (idx === 15 ? "sign" : bit ? "base" : "zero")} />
        )}
        {step === 4 && (
          <>
            <BitRow bits={toBits(SIGN_EXTENDED, 32)} label="int32_t t" caption="0xFFFFFF38 = −200" idPrefix="r" tone={(i, bit, idx) => (idx > 15 ? "sign" : idx === 15 ? "sign" : bit ? "base" : "zero")} />
            <div className="pt-2 sm:pl-[124px] sticky left-0 w-max">
              <Verdict ok>t = −200</Verdict>
            </div>
          </>
        )}
      </div>
    )}
  </Stepper>
);

export default SignExtension;
