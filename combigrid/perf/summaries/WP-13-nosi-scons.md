# WP-13 — `-fno-semantic-interposition` in site_scons (GCC, OPT=1)

Status: PROPOSED · Date: 2026-10-26 · Base commit: 9ab8f642d
Recommendation: accept. The judged build's code is bitwise identical to WP-11 `gcc-nosi`: e2e 1.023x (1.24 s saved), no SLOWER e2e case, refcheck OK. Boost test outcomes are identical in all 8 modules. There is no API change.
Machine: `perf.py calibrate` ratios e2e 1.004 burst / 1.000 sustained, micro 0.997 / 1.000 (1.000 = as at baseline time)

## Change
- `site_scons/SGppConfigure.py: configureGNUCompiler` (covers gnu, openmpi and mpich) appends `-fno-semantic-interposition` to `CPPFLAGS` and `LINKFLAGS`. Conditions:
  - `OPT=1`, GCC ≥ 5 (the flag exists since GCC 5; the same version-check pattern as the color flag, so no extra configure test), not cygwin/win32 (as `-fPIC`).
  - It uses `AppendUnique`, so a user who still passes the flag in `CPPFLAGS` does not get it twice.
  - The flag is placed after `-fno-strict-aliasing -funroll-loops`; the comment explains why.
- CLAUDE.md §4: the sentence on what `OPT=1` adds now names the flag.
- Unchanged: compileNormal.sh, `perf.py`, the baseline, the Clang/Intel paths, all module sources.
- Files: `site_scons/SGppConfigure.py` (+10 lines), `CLAUDE.md`, this summary, `combigrid/perf/results/WP-13/` (`micro`, `e2e`, `refcheck.txt`, `checks.md` = raw outputs of all checks below)

## Gain vs. cost
- Gain: e2e (38 cases, 16 threads) 54.03 → 52.79 s, **1.023x, 1.24 s saved**. 26 faster / 0 SLOWER / 12 noise.
  - By group: quad/sg 1.015x, quad/dsg 1.016x, interp 1.062x, interplin 1.007x, mivec 1.041x; d ≥ 7 1.017x.
  - Micro: geomean 1.059x; 14 faster / 1 SLOWER (`mivec/ct_coeffs` 0.993x) / 15 noise.
  - As expected, this equals WP-11 `gcc-nosi` (1.024x, 25/1/12, micro 1.062x).
  - The gain is minimal but consistent. It is the cheapest change so far: one flag, no source change.
- Mechanism, libsgppcombigrid.so:
  - PLT call/jmp sites 2139 → 1739 (all `@plt` call/jmp lines counted; WP-11 used a different filter: 2090 → 1709).
  - `.localalias` 7 → 85; `SourceFunc::evaluate@plt` 10 → 0.
  - All 8 libs: `JUMP_SLOT` relocations 3662 → 1113, symbolic `R_X86_64_64` 8575 → 4717. Vtable slots now bind to the library's own definitions.
  - Library size +1.5 % (inlining).
- API / functionality: none for combigrid. This is the build configuration change agreed with the owner (2026-10-25).
  - Interposing exported SG++ functions (`LD_PRELOAD`) no longer affects calls within the defining library.
  - No SG++ library defines a strong exported *function* that another one also defines. The 94 duplicates are data: 26 typeinfo, 66 typeinfo names, `std::piecewise_construct` and `_Sp_make_shared_tag::_S_ti()::__tag`. Their relocations are unchanged (still symbolic, so still unified by ld.so; datadriven 162 → 161).
  - Two vague-linkage RTTI names (`_ZTS…DataTransformationBuilder`, `_ZTS…_Sp_counted_ptr_inplace<BlacsProcessGrid>`) are no longer exported by datadriven. Nothing references them.
- Maintainability: 10 lines in one place, no code change.
  - Every working-tree "before" snapshot now includes the flag. The `baseline` snapshot (fed2cdb4e with its own site_scons) does not, so cumulative gains vs. baseline include this ~2.3 %.
  - `buildVars.out` now records the flag (written, never read).

## Tests
- Pinning tests: none needed (no module source change). The combigrid Boost suite serves as regression test.
- Results: debug `test --repeat 3` ✓ · normal ✓ (before and after, 3/3 each) · ASan n.a.
- Flag placement:
  - On all 1136 compile and 77 link lines of the GNU `OPT=1` all-module build, on the judged build (compile and `-shared` link lines) and on `_pysgpp_swig.so`.
  - Absent with `OPT=0`: 0 of 540 compile_commands.json entries, 0 lines in the debug build log.
- All modules (SG_ALL=1 SG_PYTHON=0 + judged flags; before = HEAD, after = change): 816/816 test cases pass both times (base 190, combigrid 390, datadriven 135, misc 1, optimization 45, pde 48, quadrature 2, solver 5).
  - No module failed to build. Optional deps at their defaults: GSL not found, no MPI/ScaLAPACK, OpenCL, CGAL, UMFPACK, Gmm++. Java/MATLAB not built.
  - Only log difference that is not run-to-run noise: optimization `TestIterativeGridGenerators` has 20093 → 20100 assertions, deterministically. A probe running the test's loop shows `IterativeGridGeneratorLinearSurplus` ends with 199 instead of 198 points (limit N = 200) on 7 boundary grid types. Presumably round-off from the new inlining flips the surplus ranking in the last refinement step. Not investigated further (outside combigrid).
  - combigrid assertion counts and datadriven "observed error" lines differ between any two runs (clock/`random_device` seeded).
- Python (base + combigrid + SG_PYTHON=1, judged flags + change): build ✓ (136 s); all 8 `combigrid/examples/python/*.py` exit 0.
- Equivalence with snapshot `WP-11-gcc-nosi`:
  - The compile/link flags are the same apart from their order. The PLT and local-alias counts are the same.
  - All loadable sections and `.symtab` of all 5 snapshot files (both libs, perf_e2e/micro/reference) are bitwise identical.
  - Only `.debug_str`/`.debug_info` (DW_AT_producer with the reordered flags, same size) and the build-id differ.

## Reference check
- `perf.py refcheck`: **OK**, 104 OK at 1 and 16 threads (`results/WP-13/refcheck.txt`). No WARN, as for `gcc-nosi` (the code is identical).
- The e2e sanity line ("quad/sg/equi-exp/b0/d5/L10: 1.45e-13 → 8.25e-13") is 16-thread reduction-order noise. A's 5 results span 3.676551626908804…913351 and B's 908804…913806. Over all cases, B's worst-run error is within A's run-to-run spread.

## Benchmarks (A = WP-13-before, B = WP-13-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; 30 cases, 5 rounds; selection):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/interp/barycentric_1d/opt_cc/n65 | 401.2 ns | 196.9 ns | 2.040 | [2.032, 2.047] |
| micro/interp/barycentric_1d/opt_cc/n9 | 58.6 ns | 31.7 ns | 1.846 | [1.839, 1.853] |
| micro/gen/sparse_grid/cc/b1/d20/L5 | 103.56 ms | 93.37 ms | 1.108 | [1.098, 1.119] |
| micro/quad/process_tg/equi/b0/d2/L16 | 6.88 ms | 6.26 ms | 1.098 | [1.093, 1.102] |
| micro/gen/complete_mis_with_coeffs/d20/L6 | 52.68 ms | 51.31 ms | 1.030 | [1.020, 1.040] |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 111.19 ms | 108.99 ms | 1.019 | [1.014, 1.024] |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 68.49 ms | 69.47 ms | 0.976 | [0.948, 1.005] (noise) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | 16.11 ms | 16.24 ms | 0.993 | [0.989, 0.997] (SLOWER) |

L2/L3 e2e (16 threads, 38 cases, 5 rounds; largest cases and notable phases):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.978 s | 6.955 s | 1.004 | [0.998, 1.010] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.766 s | 6.678 s | 1.008 | [0.995, 1.022] |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.41 ms | 50.71 ms | 1.367 | [1.209, 1.545] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.660 s | 5.526 s | 1.024 | [1.021, 1.028] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.312 s | 1.317 s | 0.996 | [0.990, 1.002] |
| quad/sg/equi-exp/b0/d10/L4 | operator | 935.49 ms | 908.50 ms | 1.029 | [1.022, 1.035] |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.251 s | 2.173 s | 1.037 | [1.032, 1.043] |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.83 ms | 188.16 ms | 1.237 | [1.180, 1.297] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.694 s | 2.681 s | 1.001 | [0.976, 1.027] |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.784 s | 1.679 s | 1.057 | [1.039, 1.076] |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.898 s | 1.876 s | 1.014 | [1.005, 1.023] |

Sum of medians (all 38 cases): A = 54.032 s, B = 52.792 s, saved = 1.240 s (1.023x) · Rounds 222 s each, foreign load 0.2 % · Full reports: `combigrid/perf/results/WP-13/`

## Attempts not kept
- none (one variant: the agreed flag; `CheckFlag` not used, see Change).

## Profile shift / follow-ups
- Profiles: as in WP-11 `gcc-nosi` (same code). Top targets are unchanged: WP-02 `appendSubTrieChildren` (dsg), WP-03 closure/coefficients (mivec), interp `__cos_fma`.
- Since every working-tree build now has the flag, future WPs no longer need the "make hot helpers `inline`" workaround for `-fPIC` (user memory `combigrid-fpic-blocks-inlining`; update it if accepted).
- README.md "Profiling and tools" says exported functions go through the PLT without the flag. It is still true, but could mention that `OPT=1` GCC builds now have it.
- optimization's LinearSurplus grid generator is sensitive to round-off in its last refinement step (198 vs. 199 points). This is information for that module, nothing to do here.

## Revert
`git checkout -- site_scons/SGppConfigure.py CLAUDE.md`. Caution: `perf.py reject --apply` would also revert the master session's uncommitted edits of `combigrid/perf/PLAN.md` and `STATUS.md` (made at 23:38 during this session). Use `--keep combigrid/perf/PLAN.md --keep combigrid/perf/STATUS.md` with it.
