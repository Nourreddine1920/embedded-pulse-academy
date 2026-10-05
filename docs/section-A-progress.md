# Section A progress ledger (bare-metal)

One line per finished lesson: id, lesson lines, validator, ARM check, host run, animations.
Helper scripts (outside the repo): `arm.sh <labId> [files]` compiles Core/Src/*.c with a stub main.h.

- A0.5 done: lesson 727 lines; validator 0 errors; ARM check OK (main, main_reg, +LAB_UNALIGNED_DEMO 1/2, solutions vs CMSIS); host run real; anims struct-overlay, padding-layout, bitfield-rmw (pre-existing, text fixed LDRH/STRH).
- A0.6 done: lesson 739 lines; validator 0 errors; ARM check OK (lab + misra/size/solutions); host run real; anims macro-expansion, static-lifetime, linkage-map (pre-existing). No MISRA checker available: findings by reading, stated in lesson.
- A1.1 done: lesson ~715 lines; validator 0 errors; ARM OK (lab incl. FPU off, solutions, size/core_compare for 6 cores); host real run (simulated regs); anims core-family-explorer, compile-by-core (new).
- A1.2 done: lesson ~703 lines; validator 0 errors; ARM OK (lab + size/aapcs + solutions); host run real (model + 100k cross-check); anims register-file, irq-mask-levels (new). Board-run values labeled expected.
- Session stopped by user after A1.2. Resume at A1.3.
