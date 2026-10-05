/**
 * @file    legacy_macros.h
 * @brief   A0.6 "BEFORE": function-like macros as found in many legacy code bases.
 *
 * The first three macros compile without a single warning under
 * -Wall -Wextra -Wconversion, and each one is wrong in a different way.
 * The fourth is caught under an if by GCC's -Wmultistatement-macros (part of
 * -Wall since GCC 8); the bench silences that one warning on that one line.
 * They exist only so the hygiene bench can show the bugs. Do not copy them.
 * The fixed versions live in hygiene.h.
 */
#ifndef LEGACY_MACROS_H
#define LEGACY_MACROS_H

/* Bug 1, precedence: the parameter is not parenthesised.
 * SQUARE(x + 1U) expands to  x + 1U * x + 1U                               */
#define SQUARE(x)        x * x

/* Bug 2, precedence: the whole expansion is not parenthesised.
 * 10U * DOUBLE(x) expands to  10U * (x) + (x)                              */
#define DOUBLE(x)        (x) + (x)

/* Bug 3, double evaluation: correct parentheses, but the winning argument
 * is evaluated TWICE. MAX(adc_read(), limit) may read the ADC twice.        */
#define MAX(a, b)        ((a) > (b) ? (a) : (b))

/* Bug 4, multi-statement macro without do { } while (0).
 * if (fault) LED_PULSE();  runs led_off() even when fault is false.         */
#define LED_PULSE()      led_on(); led_off()   /* led_on/led_off: hygiene.h */

#endif /* LEGACY_MACROS_H */
