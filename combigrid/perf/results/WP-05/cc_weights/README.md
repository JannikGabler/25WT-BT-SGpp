# Clenshaw-Curtis weights: bitwise analysis (WP-05)

Harness sources in this directory (built by hand against `lib/`; `old_cc.cpp` = the HEAD version
of `clenshaw_curtis_quadrature_rule.cpp` with the namespace renamed to `quadrature_rules_old`).

1. Old vs. new source, each compiled in its own TU into one program (`cmp.cpp`), n = 2^(l+1)+1,
   l = 0..20: **0 differing weights** with `-O3 -march=native` and with
   `-O3 -march=native -ffp-contract=off`. The rewrite performs the same operations.
2. Debug build (OPT=0): `quadrature_weights/Clenshaw_Curtis_Weights_Bitwise` hash
   0xd91eb5c907cf0c7d before and after (17 node counts, 2..32769).
3. Optimized library (LTO) before vs. after (`dump.cpp` against the snapshots' `lib/`): weights
   differ by at most 4.4e-16 * (1/m) in absolute terms (~2 ulp of a typical weight; up to 2^18
   ulp in the tiny boundary weights, which are computed as 1 - s with cancellation). Test hash
   of the optimized build: 0x0e83fe7e2827deb3 (before) -> 0x2cd0a4276a5d4ad7 (after).
   n=2^21+1: before 455 ms, after 356 ms (cold, one call).
4. Reproducibility (`dumpsrc.cpp`, new source): -O0 == -O2 -ffp-contract=off == -O3 -march=native
   -ffp-contract=off == the same + -flto=auto, **bitwise** for all n. With contraction (default
   -O3 -march=native) the weights differ from -O0 by <= 6.7e-16 * (1/m). FMA contraction (in the
   inlined kissfft butterflies) is the only source of the build-dependent bits; compiling this one
   TU without contraction would make the CC weights build-independent (a numerical change of
   <= 6.7e-16/m vs. today's optimized build; not applied).
5. Fallbacks: Armadillo backend (-DUSE_ARMADILLO) within 1.2e-15/m of the Eigen weights (l <= 20);
   naive path (no FFT library) unchanged code, compiles and runs.
