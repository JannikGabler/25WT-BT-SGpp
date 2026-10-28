# WP-14 — `-fno-strict-aliasing`: measure its cost and restrict its scope

Status: PROPOSED; **accepted by the owner on 2026-10-28**, including the datadriven fix · Date: 2026-10-28 · Base commit: a16733d65 (SCons part saved by the owner in checkpoint 50d552f00)
Recommendation: accept. The change is functionally neutral in all modules and bindings, e2e is 0.988x (−0.65 s), and RP-1, WP-02 and WP-03 then run under the final flags. On the owner's decision, the type-punning sites in datadriven were replaced by `std::bit_cast`.
Machine: `perf.py calibrate` ratios e2e 1.034 burst / 1.029 sustained, micro 0.998 / 1.000 (Part A on 2026-10-26: 1.004/1.000, 0.998/0.998)

## Change
- `site_scons/SGppConfigure.py: configureGNUCompiler` no longer adds `-fno-strict-aliasing` globally. The comment states the current reasoning. The WP-13 block is unchanged.
- `jsgpp/SConscript`: `javaEnv` gets the flag, for every compiler. This also fixes the Clang path, which never added it. SWIG 4.4 `java.swg` still puns jlong ↔ pointer, and the scan finds 8237 level-3 warnings in `jsgpp_wrap.cc`.
- `pyEnv` does not get the flag: 0 level-3 warnings. Level 2 flags only the `void**` idiom, which is well-defined for GCC (`void*` aliases every pointer type).
- datadriven (owner's decision 2026-10-28): all 8 pointer-cast punning sites now use `std::bit_cast` (5 files).
  - `OperationMultiEvalStreaming_{mult,multTranspose}Impl.cpp`: SSE3 and AVX abs mask.
  - `OperationMultipleEvalSubspaceCombined.hpp`: 2× AVX abs mask.
  - `OperationMultiEvalModMaskStreaming_{mult,multTranspose}Impl.cpp`: scalar fallback masking.
  - The union reads in `SubspaceCombined.hpp` are unchanged; GCC and Clang document union punning as supported.
- Unchanged: `-funroll-loops`, the Intel path, compileNormal.sh, perf.py, the baseline.
- Docs: README "Profiling and tools" now covers WP-13's flag and the strict-aliasing status. CLAUDE.md §4: strict aliasing is on, type punning is UB.
- Tools: `toolchain.py` variant `gcc-sa` + `scripts/wrappers/g++-sa`. The flag must come last on the line, and appended `CPPFLAGS` precede configure's flags. `results/WP-14/`: `fullcheck.py`, `sascan.py`, `tysan_classify.py`, `java/JsgppSmoke.java`, `checks.md` (all raw results).
- Files: `site_scons/SGppConfigure.py`, `jsgpp/SConscript`, `CLAUDE.md`, `combigrid/perf/README.md`, `combigrid/perf/scripts/toolchain.py`, `combigrid/perf/scripts/wrappers/g++-sa`, `combigrid/perf/results/WP-14/`. Not mine: the owner's `combigrid/build/jsgpp/combigrid.i` edit, needed for `SG_JAVA=1` with combigrid.

## Gain vs. cost
- Gain: e2e 52.77 → 53.42 s, **0.988x, −0.65 s**; 12 faster / 11 SLOWER / 15 noise; d ≥ 7: 0.980x.
  - By group: interp 1.097x, interplin 1.101x, quad/sg 0.995x, quad/dsg 0.970x, mivec 0.886x (generation 0.864x).
  - Part A (`WP-14-sa`, a day earlier): 0.987x with the same pattern; the code is bitwise identical to `WP-14-after`.
  - Micro: geomean 1.033x (13/3/14). `sourcefunc/eval_normalized_in_place` 1.49x, `interp/global_tg/cc` 1.16x, `gen/complete_mis` 1.13x, `mivec/downwards_closure` 0.94–0.96x.
- Why mixed: TBAA helps the operators as hypothesised (sizes and bounds stay in registers). Two hot loops lose:
  - `genMIVecDownwardsClosure`: instructions +3.9 %, cycles +23 % (IPC 2.07 → 1.75; same loop structure, different addressing/scheduling).
  - The dsg trie functions: uniformly −5 %.
  - These are exactly WP-03's and WP-02's rewrite targets.
  - Strict aliasing *given* WP-13 does help where both inline, but the sum is still negative.
- API / functionality: none.
  - 6 CC quadrature reference values move by 1–9 ulp. The cause is FMA placement in Eigen's kissfft: with `-ffp-contract=off`, the weights are bitwise identical with and without the flag.
  - The level-2 scan found real punning in **datadriven**: AVX abs masks read an `int64_t` through `double*`. GCC 15 folded them correctly, but it was UB.
  - The same pattern was also in branches not compiled with AVX2. In the scalar fallback of ModMask it even gave **level-3** warnings (= `-Wall`), so a build without SSE3 and with `-Werror` would have broken.
  - Fixed with `std::bit_cast` (C++20; the default `CPP_STD` is 23). The judged build (base + combigrid) is unaffected.
- Maintainability: matches GCC's default and the Clang path. The flag sits only where SWIG needs it.

## Tests
- Pinning tests: none needed for the flags (no module source change). The complete project is the regression check.
- datadriven fix: build OK (11 TUs, 0 warnings, cpplint OK). Strict-aliasing warnings are 0 at levels 3/2, also in the SSE3 and fallback branches (before: 2 at level 3, 6 at level 2).
  - Boost 135/135 with the same 854763 assertions, including `TestStreaming{,ModMask}Mult{,Transpose}`. Python tests: 21 OK.
  - `results/WP-14/datadriven/ddprobe.cpp` covers Streaming, ModMask and SubspaceCombined. SubspaceCombined has no CPU test (it needs `USE_OCL`). All 8568 results are **bitwise identical** before vs. after.
- Results: debug `test --repeat 3` ✓ · normal ✓ (before / sa / after, 3/3 each) · ASan n.a.
- Complete project: `SG_ALL=1 SG_PYTHON=1 SG_JAVA=1`, OPT=1 + judged flags, `.venv` with numpy first on PATH. HEAD = a16733d65 + the combigrid.i edit (worktree) vs. the working tree. Pass/fail is **identical** everywhere:
  - Boost 816/816 in both, with identical deterministic assertion counts (optimization 277725 as at WP-13).
  - C++ examples: 56/56; the same 2 pre-existing failures (thesis comparison templates require 2 arguments).
  - Python examples: 29/29.
  - Python tests: base 7, datadriven 21, pde 0.
  - Java: tutorial, refinement, optimization (directors) and `JsgppSmoke` have identical output. `example_learnerSGDE.java` fails to compile in both (pre-existing API drift).
- Flag placement: only the `jsgpp_wrap.cc` CU (DWARF 1/1; 0 in pysgpp, base 381, combigrid 59, datadriven 265 CUs). No new warnings.
- Scans: level 3 (= `-Wall`): 0 in all 1134 TUs; level 2: the 4 datadriven sites.
- TySan (diagnostic): no class of reports indicates punning in SG++ code (see Attempts).

## Reference check
- `perf.py refcheck`: **OK**, 104 OK at 1 and 16 threads (`refcheck.txt`, identical to `refcheck-sa.txt`). Bitwise, 6 CC cases change by 1–9 ulp (FMA, see above). Errors move both ways (9.99e-16 → 0, 2.0739e-13 → 2.0750e-13).

## Benchmarks (A = WP-14-before, B = WP-14-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, 30 cases; selection):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/sourcefunc/eval_normalized_in_place/d10 | 55.76 us | 37.60 us | 1.492 | [1.463, 1.522] |
| micro/interp/global_tg/cc/b0/d5/L6 | 1.29 ms | 1.12 ms | 1.159 | [1.117, 1.202] |
| micro/gen/complete_mis_with_coeffs/d20/L6 | 51.00 ms | 44.97 ms | 1.131 | [1.121, 1.140] |
| micro/interplin/tg/equi/b1/d8/L5 | 46.65 us | 42.48 us | 1.096 | [1.084, 1.108] |
| micro/mivec/downwards_closure/d10/S10/n300 | 594.77 ms | 622.31 ms | 0.956 | [0.952, 0.960] |
| micro/mivec/downwards_closure/d20/S6/n100 | 17.75 ms | 18.85 ms | 0.943 | [0.932, 0.954] |

L2/L3 e2e (16 threads; largest and notable cases):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.950 s | 7.020 s | 0.990 | [0.984, 0.996] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.669 s | 7.029 s | 0.949 | [0.945, 0.952] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.521 s | 5.659 s | 0.976 | [0.972, 0.979] |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.412 s | 2.412 s | 0.988 | [0.955, 1.022] |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.170 s | 1.993 s | 1.086 | [1.068, 1.104] |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.621 s | 1.450 s | 1.118 | [1.111, 1.125] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.685 s | 2.453 s | 1.095 | [1.073, 1.118] |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.677 s | 2.070 s | 0.810 | [0.805, 0.815] |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.876 s | 2.072 s | 0.902 | [0.888, 0.916] |

Sum of medians (all 38 cases): A = 52.771 s, B = 53.424 s, saved = −0.654 s (0.988x). Rounds took 221 s each; foreign load ≤ 0.7 %. Reports: `e2e`, `micro` (official), `e2e-sa`, `micro-sa` (Part A).

## Attempts not kept
- `gcc-sa` (Part A measurement variant): its code is identical to the final state, so nothing was dropped.
- TySan (Clang 21, experimental), diagnostic only, ~3 h instead of ~1 h. The shared-library build works.
  - Problems:
    - OpenBLAS × shadow memory needed > 12 GB per binary. That **crashed the PC on 2026-10-26**; the fix is `OPENBLAS_NUM_THREADS=1` plus a cgroup cap.
    - A runtime crash in `isAliasingLegal` aborted 21 of 74 combigrid suites.
    - base and optimization were stopped at the 6 GB cap; datadriven and pde were not run.
  - All report classes are known false positives: vptr, stack/OpenMP loop bounds, arrays in class objects (mt19937), layout.

## Profile shift / follow-ups
- WP-03: `genMIVecDownwardsClosure` is 11–19 % slower under strict aliasing (IPC drop). Measure the rewrite under the final flags.
- WP-02: the dsg trie functions are ~5 % slower.
- Pre-existing: `example_learnerSGDE.java` does not compile; the thesis comparison templates fail without arguments; numpy is only in `.venv`.

## Revert
Flags: `git checkout a16733d65 -- site_scons/SGppConfigure.py jsgpp/SConscript CLAUDE.md combigrid/perf/README.md` (optionally also `toolchain.py` and the `g++-sa` wrapper). This part is already in commit 50d552f00, so `perf.py reject --apply` (which reverts against HEAD) would not revert it. datadriven fix (uncommitted): `git checkout -- datadriven/src/sgpp/datadriven/operation/hash`. Revert the datadriven fix only together with the flags: with strict aliasing on, the original fallback code warns at level 3 under `-Wall`.
