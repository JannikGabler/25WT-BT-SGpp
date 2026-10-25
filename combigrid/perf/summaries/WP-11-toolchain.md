# WP-11 — Toolchain measurement: GCC flags vs. Clang ThinLTO vs. Clang full LTO

Status: PROPOSED · Date: 2026-10-25 · Base commit: 308ee46e6
Recommendation: owner decision needed. Add `-fno-semantic-interposition` (gcc-nosi) to the judged configuration and stay with GCC. It saves 2.4 % over the suite with no slower group, and results are bitwise identical in 100/104 reference cases. Clang ThinLTO gains 3.4 % in sum, but quadrature gets slower, 14 of 38 cases are SLOWER and refcheck shows a WARN.
Machine: `perf.py calibrate` ratios e2e 1.003 burst / 1.002 sustained, micro 1.041 / 1.042 (1.000 = as at baseline time)

## Change
- Measurement only: no module source, SConstruct, site_scons or compileNormal.sh change. Source = HEAD for all variants. Variants = the judged configuration plus:
  - **gcc-nosi**: `-fno-semantic-interposition` (CPPFLAGS + LINKFLAGS).
  - **gcc-nosi-bsym**: gcc-nosi + `-fno-plt` + `-Wl,-Bsymbolic-functions`.
  - **clang-thin** / **clang-full**: `COMPILER=clang CC=clang CXX=clang++ LINK=clang++`, `-flto=thin` / `-flto`, `-fuse-ld=lld -O3` at link.
  - **clang-thin-nosi** (optional): clang-thin + `-fno-semantic-interposition`.
- Two configuration notes for Clang:
  - Explicit `CC/CXX/LINK`: SConstruct runs its configure checks with `$CXX` before `configureClangCompiler` switches to clang++. With g++, `-flto=thin` fails the Eigen header check.
  - `-O3` at link: otherwise lld runs the LTO backend at O2, while GCC keeps the compile-time `-O3`.
  - SConstruct's Clang path also differs from GCC's: `-Weverything` with exclusions, and no `-fno-strict-aliasing`/`-funroll-loops`.
- New wrapper `combigrid/perf/scripts/toolchain.py`. It imports perf.py (unmodified) for `run_scons`, the stamp format and `copy_build`. Its commands are `build`, `snapshot`, `timing`, `warnings`, `overview` and `info`. A/B, refcheck, tests and profiles use perf.py itself.
  - Variant stamps carry the variant name (only `gcc` is `normal`), so `perf.py snapshot`/`current` refuse variant builds.
  - Each snapshot's `meta.json` records the variant, the SCons arguments, the full compile/link lines, `readelf -p .comment` and `ldd`. All snapshots were verified this way.
  - `buildVars.out` is only written by SCons, never read, so no flag leakage. It was still backed up and restored.
- OpenMP runtime used: GCC builds load `libgomp.so.1`; Clang builds load `/usr/lib/x86_64-linux-gnu/libomp.so.5` (libomp5 22.1.2). BLAS is the same OpenBLAS (pthreads) for all.
- Files: `combigrid/perf/scripts/toolchain.py` (new), this summary, `combigrid/perf/results/WP-11/`

## Gain vs. cost
- Gain, gcc-nosi: e2e 54.20 → 52.91 s (1.024x, 1.28 s saved). By group: quad/sg 1.017x, quad/dsg 1.018x, interp 1.065x, interplin 1.000x, mivec 1.043x; the d ≥ 7 cases 1.018x. 25 cases faster, 1 SLOWER (quad/sg/cc-uq/b0/d10/L4, 0.990x). Micro geomean 1.062x.
  - Mechanism: the 23 `SourceFunc::evaluate@plt` calls are gone, PLT calls drop 2090 → 1709, local aliases rise 7 → 85.
  - Minimal but consistent. As expected, ≲ 2 % on quadrature and more on interp.
- Clang ThinLTO: 54.03 → 52.25 s (1.034x), but the gain is uneven:
  - mivec 1.38x (generation 1.5x: downwards closure, CT coefficients); interp 1.10x, interplin 1.07x.
  - Slower: quad/sg 0.964x and quad/dsg 0.980x. quad/dsg is the largest group (26.7 s); e.g. b1/d20 0.951x, equi-sin/d7 0.916x.
  - 14 of 38 cases SLOWER.
  - Full LTO is worse (1.018x): quadrature 0.94–0.95x (cc-uq/d10 0.83x), only mivec gains more (1.51x).
- `-fno-plt -Bsymbolic-functions` adds nothing: 1.015x, worse than gcc-nosi alone (the GOT-indirect calls into libm/base/libgomp do not pay). `-fno-semantic-interposition` under Clang: 1.033x vs. 1.034x (no effect).
- Costs and risks of gcc-nosi:
  - One flag. The owner changes the judged configuration (compileNormal.sh, perf.py `BUILD_CONFIGS`) and re-records the baseline. Optionally it goes into `configureGNUCompiler` for all users (affects all modules).
  - Interposing SG++'s exported functions (`LD_PRELOAD`) no longer works.
  - No API change, no new warnings, no build-time cost. Results: 100/104 reference cases bitwise identical (1 thread); 4 uq cases differ by ≤ 1.1e-15 relative.
  - Python: see "Tests".
  - Side effect: the "make hot helpers `inline`" workaround (-fPIC memory) becomes unnecessary for future WPs.
- Costs and risks of Clang:
  - 612 unique warnings (GCC: 7) from `-Weverything`: 431 `-Wunsafe-buffer-usage`, 82 `-Wnewline-eof`, ... In combigrid/src: 181, of which 170 are `-Wunsafe-buffer-usage*`/`-Wnewline-eof`, 9 `-Wold-style-cast`, 2 as with GCC.
  - Needs the SConstruct workarounds above, and a runtime libomp (22.1) newer than the compiler (21.1).
  - CI builds with GCC.
  - Results change at round-off level in 58/104 cases, with one refcheck WARN.

## Tests
- No source change, so no pinning tests. Normal-build Boost tests `--repeat 3` pass for every variant: gcc, gcc-nosi, gcc-nosi-bsym, clang-thin, clang-thin-nosi, clang-full.
- Python (`SG_PYTHON=1 RUN_PYTHON_TESTS=0`, flags of the variant, `--tests skip`): builds succeed for clang-thin (381 s incl. switch; `_pysgpp_swig.so` loads libomp) and gcc-nosi (240 s). All 8 `combigrid/examples/python/*.py` exit 0 with both.
- End state: `perf.py build normal`, `test --repeat 3` ✓, refcheck OK (`results/WP-11/refcheck.txt`). `lib/libsgppcombigrid.so` is byte-identical to snapshot `WP-11-gcc`. `buildVars.out` is byte-identical to the start.

## Reference check
- gcc, gcc-nosi, gcc-nosi-bsym: OK (104 OK at 1 and 16 threads).
- clang-thin, clang-thin-nosi, clang-full: **WARN**, 103 OK + 1 WARN at both thread counts: `interp/sg/equi/b0/d2/L5/cos-shifted`, value 3, error 1.6e-6 → 4.2e-6 (×2.57).
  - Cause: in `BarycentricFormula::evaluateFormula`, Clang contracts `numerator += factor * values[i]` into an FMA (9 `vfmadd`), while GCC vectorizes the factors and keeps mul + in-order add (1 FMA).
  - Verified with a probe compiling only that TU: Clang `-ffp-contract=off` reproduces GCC bit for bit. GCC with or without `-ffp-contract=off` gives identical bits, and Clang `-ffp-contract=fast` equals the Clang default. Nodes and weights are bitwise equal.
  - The case is pathological: 65 equidistant nodes, Lebesgue constant ~1e16, reference errors up to 0.32. Errors move both ways (value 0 10x better, value 5 0.85x).
  - Not a defect, but every FMA-placement change trips this case. Possible tolerance override: owner-only.
- Reports: `results/WP-11/refcheck-*.txt`.

## Benchmarks (A = WP-11-gcc, B = variant; speedup = A/B, > 1 is faster)
Overview (e2e, 16 threads, 38 cases, 5 rounds; micro 30 cases, 1 thread; verdicts faster/SLOWER/noise):

| config | e2e Σ medians | Σ A/B | quad/sg | quad/dsg | interp | interplin | mivec | d ≥ 7 | verdicts | micro geomean | refcheck | tests | clean build | warnings |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| gcc (A) | 54.0–54.2 s | – | – | – | – | – | – | – | – | – | OK | ✓ | 291 s | 7 |
| gcc-nosi | 54.20 → 52.91 s | **1.024** | 1.017 | 1.018 | 1.065 | 1.000 | 1.043 | 1.018 | 25/1/12 | 1.062 | OK | ✓ | (270 s)¹ | 7 |
| gcc-nosi-bsym | 54.11 → 53.34 s | 1.015 | 1.008 | 1.007 | 1.048 | 0.992 | 1.042 | 1.009 | 23/3/12 | 1.055 | OK | ✓ | (271 s)¹ | 7 |
| clang-thin | 54.03 → 52.25 s | **1.034** | 0.964 | 0.980 | 1.096 | 1.074 | 1.378 | 1.034 | 19/14/5 | 1.068 | WARN | ✓ | 352 s | 612 |
| clang-thin-nosi | 54.07 → 52.34 s | 1.033 | 0.959 | 0.980 | 1.093 | 1.079 | 1.379 | 1.035 | 17/15/6 | 1.071 | WARN | ✓ | (314 s)¹ | 612 |
| clang-full | 54.07 → 53.11 s | 1.018 | 0.938 | 0.948 | 1.099 | 1.066 | 1.510 | 1.016 | 15/15/8 | 1.075 | WARN | ✓ | 444 s | 612 |

¹ full rebuild after a configuration switch, without `scons -c`, i.e. without re-running cpplint.

Per configuration (Σ of medians of `total`; group sums A → B, A/B; notable cases):
- gcc-nosi: quad/sg 9.37 → 9.21 s, quad/dsg 26.77 → 26.31 s, interp 6.65 → 6.24 s, interplin 5.02 → 5.03 s, mivec 6.39 → 6.12 s. Best: interp/cc/d2/L12 1.225x, d3/L9 1.176x, interp d7/d10 1.052x/1.041x, dsg b0/d7 1.041x. Worst: cc-uq/d10 0.990x (SLOWER), interplin d8 0.972x (noise). Micro: opt_cc barycentric 1.85–2.04x, gen/sparse_grid b1/d20 1.12x.
- gcc-nosi-bsym: quad/sg 9.32 → 9.25 s, quad/dsg 26.76 → 26.58 s, interp 6.64 → 6.34 s, interplin 5.02 → 5.06 s, mivec 6.37 → 6.12 s. SLOWER: dsg b1/d20 0.986x, equi-sin/d7 0.984x; micro downwards_closure 0.955x/0.960x.
- clang-thin: quad/sg 9.36 → 9.71 s, quad/dsg 26.71 → 27.26 s, interp 6.63 → 6.05 s, interplin 4.97 → 4.63 s, mivec 6.35 → 4.61 s. Best: mivec d20/d15/d10 1.60x/1.47x/1.29x, interp/cc/d2 1.255x, interp d10 1.117x, interplin d10/d8 1.10x/1.09x. Worst: equi-exp/d2/L20 0.832x, equi-sin/d7 0.916x, dsg b1/d20 0.951x. Micro 0.912x (dsg/prepare b1/d20) … 1.76x (opt_cc).
- clang-thin-nosi: quad/sg 9.43 → 9.83 s, quad/dsg 26.70 → 27.24 s, interp 6.62 → 6.06 s, interplin 4.97 → 4.60 s, mivec 6.35 → 4.61 s; same pattern as clang-thin.
- clang-full: quad/sg 9.36 → 9.98 s, quad/dsg 26.73 → 28.20 s, interp 6.64 → 6.04 s, interplin 4.98 → 4.67 s, mivec 6.36 → 4.21 s. Best: mivec d20 1.89x; worst: cc-uq/d10 0.828x, dsg cc-uq/d10 0.874x.

Build times (wall clock, -j16, idle; `timing`: `scons -c` + build, then a comment line inserted into / removed from the file, each rebuild timed; 2 samples agree within 1 %; no-op build 8 s):

| config | clean | edit `quadrature.cpp` | edit `type_defs.hpp` |
|---|---|---|---|
| gcc (normal) | 291 s | 110 s | 137 s |
| clang-thin | 352 s | 76 s | 127 s |
| clang-thin + `--thinlto-cache-dir` | 337 s (cold) | 13 s | 121 s (63 s when reverting = warm cache) |
| clang-full | 444 s | 159 s | 194 s |
| gcc debug (OPT=0) | 192 s | 22 s | 52 s |
| clang debug (OPT=0) | 265 s | 22 s | 61 s |

A one-.cpp edit relinks 46 LTO targets (lib + tests + all examples), which dominates the optimized incremental builds. Reports: `results/WP-11/{e2e,micro}-<config>.{md,json}`, `build_times.jsonl`, `profiles.txt`. Rounds constant (221–224 s e2e, 46–51 s micro), foreign load ≤ 0.5 %.

## Attempts not kept
- Clang without explicit `CC/CXX/LINK`: `scons -c` failed in the configure check (g++ rejects `-flto=thin`). The chained command still snapshotted the previous gcc-nosi-bsym build as "WP-11-clang-thin". Its metadata showed the real variant; deleted and rebuilt correctly.
- clang-thin + `-ffp-contract=fast` (diagnostic only, refcheck): bitwise equal to clang-thin. This ruled out the contraction default and led to the FMA-placement probe above.
- clang-thin-nosi (optional variant): no gain over clang-thin (1.033x vs. 1.034x).
- The e2e A/B of clang-thin was interrupted once (the session's process exited) and re-run in full; the reported run is complete.
- **Side effect, repaired**: `scons -c` (the clean builds of `timing`) deletes all of `lib/` (`SConstruct:621`). That included the owner's other module libraries (datadriven, misc, optimization, pde, quadrature, solver; build of 2026-10-21) and the installed `lib/pysgpp`.
  - The six libraries were copied back from their module directories with timestamps, identical sizes.
  - `lib/pysgpp`: the original (2026-10-21, flags unknown) is lost. It was rebuilt from HEAD in the judged configuration + `SG_PYTHON=1`, and the examples run.
  - `toolchain.py` now preserves these `lib/` entries across its clean. The preserve step was checked; the restore path has not been exercised by a real `scons -c`.

## Profile shift / follow-ups
- Profiles (16 threads, self time; `results/WP-11/profiles.txt`; absolute cycles over 3 runs):
  - quad/dsg/equi-exp/b1/d20/L6: `appendSubTrieChildren` 64 % (gcc-nosi) / 66 % (clang-thin). In absolute terms Clang needs 11 % more cycles there (435 vs. 391 G) and 8 % fewer in `processDeltaTensorGrid`. OpenMP runtime: libgomp 0.5 %, libomp 3.5 %.
  - interp/sg/cc/b0/d10/L2/p8: `__cos_fma` 54–57 %. `interpolateFirstDim` needs 32 % fewer cycles with Clang (34 vs. 50 G). Runtime: libgomp 3.7 %, libomp 8.4 % (spin-waiting threads, not necessarily wall time).
- WP-03 (downwards closure): Clang is 1.4–1.9x faster on MIVec generation (micro downwards_closure 1.23–1.42x, complete_mis_with_coeffs 1.57x). Compare its code for the dominance/closure loops; a source-level change could give GCC the same.
- WP-06/WP-09: compare `interpolateFirstDim` code (Clang −32 % cycles). WP-02 (the target of `appendSubTrieChildren`) stays top: it is 64 % of the largest group with either compiler.
- Developer workflow (not judged): Clang ThinLTO with a cache rebuilds after a .cpp edit in 13 s instead of 110 s.
- After WP-02/WP-03 the Clang vs. GCC picture may change; `toolchain.py` makes a re-run cheap (build + snapshot per variant, then `perf.py ab`).
- STATUS D-2 (`-fno-semantic-interposition`): measured, see the recommendation.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`). It deletes `toolchain.py`; add `--keep combigrid/perf/scripts/toolchain.py` to keep the wrapper. Snapshots in `.work/` and the ThinLTO cache (`.work/thinlto-cache`, 238 MB) are git-ignored.
