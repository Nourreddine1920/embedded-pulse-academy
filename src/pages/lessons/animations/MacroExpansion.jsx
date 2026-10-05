import React from "react";
import { motion } from "framer-motion";
import Stepper, { Verdict } from "./Stepper";

/*
 * A0.6: what the preprocessor really hands to the compiler.
 * Each case: the macro, the call as written, the expansion token by token,
 * and the result (values from the A0.6 hygiene bench, verified on a PC and
 * in the Cortex-M4 disassembly).
 */
const cases = [
  {
    label: "SQUARE(x + 1U)",
    define: "#define SQUARE(x)  x * x",
    call: "SQUARE(x + 1U)      /* x = 3 */",
    tokens: [["x + 1U", "arg"], [" * ", "op"], ["x + 1U", "arg"]],
    reading: "x + (1U * x) + 1U",
    result: "3 + 3 + 1 = 7",
    want: "16",
    ok: false,
    caption: "The preprocessor pastes text, it does not evaluate. The argument x + 1U is dropped into x * x, and * binds tighter than +, so the multiplication grabs only 1U and x. Result 7, not 16.",
  },
  {
    label: "Parenthesise the parameters",
    define: "#define SQUARE(x)  ((x) * (x))",
    call: "SQUARE(x + 1U)",
    tokens: [["((", "op"], ["x + 1U", "arg"], [") * (", "op"], ["x + 1U", "arg"], ["))", "op"]],
    reading: "(x + 1U) * (x + 1U)",
    result: "4 * 4 = 16",
    want: "16",
    ok: true,
    caption: "Rule for every function-like macro: wrap every use of a parameter in parentheses AND wrap the whole body. MISRA C:2012 Rule 20.7 requires the first. This fixes precedence, but not the next two problems.",
  },
  {
    label: "10U * DOUBLE(x)",
    define: "#define DOUBLE(x)  (x) + (x)",
    call: "10U * DOUBLE(x)     /* x = 3 */",
    tokens: [["10U * ", "ctx"], ["(x)", "arg"], [" + ", "op"], ["(x)", "arg"]],
    reading: "(10U * x) + x",
    result: "30 + 3 = 33",
    want: "60",
    ok: false,
    caption: "The parameters are parenthesised, but the body isn't. The caller's 10U * sticks to the first (x) only. The expansion interacts with the tokens around the call, which a function never does.",
  },
  {
    label: "MAX(adc_read(), 1000U)",
    define: "#define MAX(a, b)  ((a) > (b) ? (a) : (b))",
    call: "MAX(adc_read(), 1000U)",
    tokens: [["((", "op"], ["adc_read()", "call"], [") > (1000U) ? (", "op"], ["adc_read()", "call"], [") : (1000U))", "op"]],
    reading: "adc_read() is written twice, so it runs twice",
    result: "1st read 1650 > 1000, so read AGAIN: 2400",
    want: "1650",
    ok: false,
    calls: 2,
    caption: "Perfect parentheses, still wrong. The argument text appears twice in the body, so the function is called twice: once to compare, once to return. The result is a different sample, and one sample is lost. The Cortex-M4 code has a BL adc_read and a second B.W adc_read.",
  },
  {
    label: "static inline max_u32()",
    define: "static inline uint32_t max_u32(uint32_t a, uint32_t b) { return (a > b) ? a : b; }",
    call: "max_u32(adc_read(), 1000U)",
    tokens: [["a = adc_read()", "call"], ["; b = 1000U; ", "op"], ["(a > b) ? a : b", "arg"]],
    reading: "arguments are evaluated once, then bound to typed parameters",
    result: "1650 (one read)",
    want: "1650",
    ok: true,
    calls: 1,
    caption: "A function evaluates each argument exactly once, converts it to the parameter type (with -Wconversion checking that), and has normal precedence. At -Os and -O2, GCC inlines it: the code is CMP, IT CC, MOVCC, byte for byte the same as the macro for plain arguments.",
  },
  {
    label: "if (fault) LED_PULSE();",
    define: "#define LED_PULSE()  led_on(); led_off()",
    call: "if (fault) LED_PULSE();",
    tokens: [["if (fault) ", "ctx"], ["led_on();", "arg"], [" led_off();", "bad"]],
    reading: "if (fault) led_on();   led_off();   /* always runs */",
    result: "led_off() called with fault == false",
    want: "0 calls",
    ok: false,
    caption: "Two statements, but if only guards the first. GCC -Wall catches this one (-Wmultistatement-macros). Wrapping it in plain braces { … } breaks if/else instead: the ; after the macro ends the if, and the else has no if.",
  },
  {
    label: "do { … } while (0)",
    define: "#define LED_PULSE()  do { led_on(); led_off(); } while (0)",
    call: "if (fault) LED_PULSE(); else log_ok();",
    tokens: [["if (fault) ", "ctx"], ["do { led_on(); led_off(); } while (0)", "arg"], ["; else log_ok();", "ctx"]],
    reading: "one statement that needs its semicolon",
    result: "0 calls when fault == false; if/else still works",
    want: "0 calls",
    ok: true,
    caption: "The do/while(0) idiom makes the body a single statement that swallows the caller's semicolon, so it behaves like a function call everywhere. It costs nothing: the loop runs exactly once and the compiler emits no branch. Better still: make it a static inline function.",
  },
];

const steps = cases.map(({ label, caption }) => ({ label, caption }));

const TOKEN_TONE = {
  arg: "bg-cyan-400/15 text-cyan-100 ring-1 ring-cyan-400/50",
  op: "text-slate-400",
  ctx: "bg-amber-400/15 text-amber-100 ring-1 ring-amber-400/50",
  call: "bg-fuchsia-400/15 text-fuchsia-100 ring-1 ring-fuchsia-400/50",
  bad: "bg-rose-500/20 text-rose-100 ring-1 ring-rose-400/60",
};

const Row = ({ label, children }) => (
  <div className="flex items-start gap-3">
    <span className="w-24 shrink-0 pt-1 text-right font-mono text-[11px] uppercase tracking-wider text-slate-500">{label}</span>
    <div className="min-w-0 flex-1">{children}</div>
  </div>
);

const MacroExpansion = () => (
  <Stepper title="Animation · What the preprocessor really pastes" steps={steps}>
    {(step) => {
      const c = cases[step];
      return (
        <div className="max-w-[720px] space-y-3">
          <Row label="definition">
            <code className="block rounded-lg border border-slate-800 bg-slate-900 px-3 py-2 font-mono text-xs text-emerald-200">{c.define}</code>
          </Row>
          <Row label="you write">
            <code className="block rounded-lg border border-slate-800 bg-slate-900 px-3 py-2 font-mono text-sm text-slate-100">{c.call}</code>
          </Row>
          <Row label="compiler sees">
            <div className="flex flex-wrap items-center rounded-lg border border-slate-800 bg-slate-950 px-3 py-2 font-mono text-sm">
              {c.tokens.map(([text, tone], i) => (
                <motion.span
                  key={`${step}-${i}`}
                  initial={{ opacity: 0, y: -10 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ delay: i * 0.18, duration: 0.3 }}
                  className={`whitespace-pre rounded px-0.5 ${TOKEN_TONE[tone]}`}
                >
                  {text}
                </motion.span>
              ))}
            </div>
          </Row>
          <Row label="which means">
            <p className="pt-1 font-mono text-xs text-slate-300">{c.reading}</p>
          </Row>
          <Row label="result">
            <div className="flex flex-wrap items-center gap-3 pt-0.5">
              <span className="font-mono text-sm text-slate-100">{c.result}</span>
              <span className="font-mono text-xs text-slate-500">want {c.want}</span>
              {c.calls !== undefined && (
                <span className="flex items-center gap-1 font-mono text-xs text-slate-400">
                  adc_read() calls:
                  {Array.from({ length: c.calls }, (_, i) => (
                    <motion.span
                      key={`${step}-call-${i}`}
                      initial={{ scale: 0 }}
                      animate={{ scale: 1 }}
                      transition={{ delay: 0.5 + i * 0.35 }}
                      className={`inline-flex h-5 w-5 items-center justify-center rounded-full text-[10px] font-bold ${c.calls > 1 ? "bg-rose-500/25 text-rose-200" : "bg-emerald-500/25 text-emerald-200"}`}
                    >
                      {i + 1}
                    </motion.span>
                  ))}
                </span>
              )}
            </div>
          </Row>
          <div className="pl-0 sm:pl-[108px]">
            <Verdict ok={c.ok}>{c.ok ? "correct" : "bug"}</Verdict>
          </div>
        </div>
      );
    }}
  </Stepper>
);

export default MacroExpansion;
