# RP-6 — Re-profiling after WP-56 / WP-49 (+ WP-73 / WP-82 state)

Status: PROPOSED · Date: 2026-11-18 · Base commit: 37a84e1af
Recommendation: accept (measurement only). Results:
- **New largest lever: largest-first TG order** in the TG loops (SG quadrature / mivec, DSG quadrature, SG global interpolation): ≈ 0.25–0.4 s on the suite, ≈ 1.1–1.6 s on `expensive/` → extend WP-77, absorb WP-51's order part, replace WP-59's guided → dynamic (predicted to lose);
- WP-74 grows by the evaluation-pass chunk granularity; WP-75 has no candidates (close);
- basis-value table for `interp/sg`: no WP;
- WP-73 / WP-82 are neutral in the judged suite.

Machine: `perf.py calibrate` ratios e2e 1.004 burst / 0.999 sustained, micro 1.004 / 1.002 (1.000 = as at baseline time)

## Change
- None in the source. Snapshot `rp-6` = HEAD 37a84e1af (in-tree `perf.py build normal`; `perf.py test --repeat 3` passed 3/3, the known flaky tests did not recur).
- Measurements:
  - `ab e2e baseline rp-6`, `ab e2e rp-5 rp-6` (full suites);
  - `ab e2e hist-rp-5 rp-6 --filter '^expensive/'` (rp-5 libraries + WP-76's history driver);
  - `ab micro rp-5 rp-6` (classic hot-spot filter);
  - 42 profiles with inline splits;
  - 2 per-thread timelines;
  - 1-thread runs of 4 `expensive/` cases.
- Probes (no library change):
  - TG sizes of the e2e grids + a makespan model (`tg_sizes.cpp`, `schedule_sim.py`);
  - DSG multilinear evaluation-pass balance (`chunk_balance.cpp`, internal headers);
  - cross-call cache sizes (`cache_bytes.cpp`).
- Files: `ANALYSIS.md` (section "Re-profiling RP-6" + contents line), `results/RP-6/` (reports, `profiles/`, probes, scripts; raw perf data in `.work/profiles/`).

## Gain vs. cost
- Cumulative baseline → rp-6 (38 common cases, 16 t): **69.89 → 34.70 s (2.014x)** [RP-5 1.973x]. d ≥ 7: 59.71 → 29.98 s (1.99x). 37 faster, 1 noise (interplin/sg d2), 0 SLOWER.
- Suite (48 cases): **36.33 s**. Direct rp-5 → rp-6: 37.44 → 36.33 s (**1.031x**, −1.11 s).
  - ≈ 0.30 s of this is the benchmark binary (WP-76's `perf_e2e.cpp` edit). WP-76's same-library A/A shows the same shifts: dsg equi b0 d3–d7, sg equi b0 d5 / d7, mivec d5.
  - ≈ 0.81 s is the libraries (WP-56 + WP-49).
- `expensive/` (21): 17.75 → 17.77 s (0.999x); `expensive/ cached` (4): 1.07 → 1.04 s (1.020x).

| group | baseline → rp-6 | rp-5 → rp-6 (direct) | share of rp-6 |
|---|---|---|---|
| quad/sg | 22.30 → 8.13 s (2.74x) | **1.059x** (cc d2 L20 1.783, equi b1 d10 1.210) | 22.4 % |
| quad/dsg | 26.74 → 17.70 s (1.51x) | 1.006x (equi b0 = binary placement; cc 0.998–1.003) | **48.7 %** (b0 42.4) |
| interp/sg | 6.60 → 4.23 s (1.56x) | 0.996x | 11.7 % |
| interplin/sg | 4.97 → 3.54 s (1.41x) | 0.990x | 9.7 % |
| mivec | 9.28 → 1.09 s (8.49x) | **1.190x** (d10 1.330, d5 1.132) | 3.0 % |
| interp/dsg | 0.83 s | 0.998x | 2.3 % |
| interplin/dsg | 0.79 s | **1.479x** (d10 / d8 / d5 1.59 / 1.58 / 1.51) | 2.2 % |

- **Memory**: peak RSS geomean baseline → rp-6 **0.716**, 25 `MEM−`, 6 `MEM+` (+0.4–1.2 MiB; process floor 10.2 → 10.5 MiB).
  - sg cc d2 L20 839 → **191 MiB** (rp-5 284); sg equi d2 L20 364 → 114 MiB.
  - dsg b1 d20 332 → 83 MiB.
  - rp-5 → rp-6: 0.999. `MEM+` interplin/dsg d10 15.3 → 15.9 MiB (WP-49 fibers).
- API / functionality: none. Maintainability: n/a.

## Tests
- n/a (no code change). `perf.py test --repeat 3` (normal, rp-6): all passed; `equidistant_nodes/sin_func_random_cube_3D` and `*_nodes/linear_func_unit_cube_1D` did not fail. `perf.py verify` OK at the start and at the end.

## Reference check
- n/a (no code change).

## Benchmarks (A = baseline fed2cdb4e / rp-5 3261399ec, B = rp-6 37a84e1af; speedup = A/B)
L1 micro (1 t, A = rp-5; common classic cases, sum 3.379 → 3.288 s, 1.028x):

| case | rp-5 | rp-6 | speedup | 95% CI |
|---|---|---|---|---|
| quadrule/cc_weights/n32769 | 3.18 ms | 1.40 ms | 2.259 | [2.214, 2.305] |
| quad/operator/cc/b0/d2/L16 | 122.4 ms | 112.4 ms | 1.091 | [1.068, 1.114] |
| interplin/dsg/equi/b1/d10/L5 | 142.5 ms | 88.5 ms | 1.612 | [1.590, 1.634] |
| dsg/process_delta_tgs/equi/b0/d10/L7/s256 | 228.5 ms | 223.7 ms | 1.024 | [1.013, 1.036] |
| tg/iterate_mutable/equi/b0/d10/L4 (untouched) | 7.74 ms | 7.89 ms | 0.980 | [0.970, 0.989] |

L3 e2e (16 t, `total`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 (A = baseline) | 6.993 s | 6.859 s | 1.021 | [1.010, 1.032] |
| quad/sg/cc-uq/b0/d2/L20 (A = rp-5) | 792.5 ms | 441.0 ms | 1.783 | [1.702, 1.867] |
| interplin/dsg/equi/b1/d10/L5/p32 (A = rp-5) | 429.5 ms | 271.8 ms | 1.588 | [1.574, 1.602] |
| expensive/mivec/sg/.../d15/S6/n300/c100us (A = hist-rp-5) | 2.060 s | 2.156 s | 0.955 | [0.952, 0.959] |
| expensive/mivec/sg/.../d20/S10/n300/c1us (A = hist-rp-5) | 751.3 ms | 666.2 ms | 1.128 | [1.102, 1.156] |

Sum of medians (38 common cases, A = baseline): A = 69.888 s, B = 34.699 s, saved = 35.189 s · Full reports: `combigrid/perf/results/RP-6/`

Peak memory (largest, baseline → rp-6):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 839.1 MiB | 191.1 MiB | 0.228 | MEM- |
| quad/sg/equi-exp/b0/d2/L20 | 363.9 MiB | 113.7 MiB | 0.312 | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 332.2 MiB | 82.9 MiB | 0.249 | MEM- |
| quad/dsg/equi-exp/b0/d3/L16 | 62.1 MiB | 44.3 MiB | 0.712 | MEM- |

Geomean B/A = 0.716 · process floor A = 10.2 MiB, B = 10.5 MiB

## Attempts not kept
- None (measurement only). One probe change was dropped: a corner-count cost model for SG `interplin`. That operator evaluates 2^d corners per TG, so its per-TG cost is uniform and the model would have been wrong.

## Answers to the RP-6 questions
1. **Suite and groups**: shown above.
   - quad/dsg b0 d7/d10 = 14.86 s (40.9 % of the suite): integrand 8.48 s, row loop **5.74 s** [5.72] (`updateCoeffSums` 1.87, `compRowWeights` 1.56, evaluate + addScaled 0.97, rest 1.34). b1 builder 0.241 s.
   - quad/sg is integrand-bound above d2 (73–92 %; `sumOverRows` 0.20 + 0.22 + 0.14 s). At d2 the CC weights fell to 2.7 % (kissfft); the rest of the overhead is the TG tail (see 6).
2. **interp/sg basis values**: ≤ 0.07 % at d ≥ 5 (0.00 % at d10 L2), 0.4 % at cc d2.
   - A shared per-call table would save ≤ 0.001 s → **no WP**.
   - WP-07 stays ≈ 0. WP-73's cache does not apply (SG interpolation keeps nothing by design).
3. **interplin/dsg after WP-49** (16 t; d5 / d8 / d10):
   - libgomp 28.9 / 18.3 / 12.1 % (35 / 43 / 33 ms); `transformFiber` 20.1 / 9.3 / 5.8 % (24 / 22 / 16 ms); evaluation pass 7 / 14 / 16 ms.
   - d2 is serial; libgomp + OpenBLAS 67 % there is pool spin.
   - New: the evaluation pass hands out 16-leaf chunks. Their efficiency is 0.865 / 0.901 / 0.913, and 1-leaf chunks would reach 0.97–0.99 (`chunk-balance.md`).
   - `expensive/interplin/dsg` d5 has 16 chunks for 16 threads, model 0.489 = measured 0.49.
   - **WP-75**: no byte-type stores left in hot loops. The `uint8_t` arrays of `DSGLinInterpIndex` are written only at index build.
4. **WP-73 / WP-82**: neutral. quad/dsg cc 0.998–1.003, b1 1.002 / 1.008, interp/dsg 0.979–1.000. The e2e pipelines build a new grid per run, so the cache never hits there.
   - Cache bytes after one default call: quad/dsg b0 d3 L16 8.0 MiB, d5–d10 0.9–1.2 MiB, b1 d15 9.4 MiB, d20 17.8 MiB; interp/dsg ≤ 0.75 MiB; interplin/dsg ≤ 2.0 MiB.
5. **Re-sizing**: see the follow-ups below.
   - The new lever ≥ 0.2 s is the largest-first TG order.
   - Model (`schedule-sim.md`): cost of a TG = its points. It reproduces `expensive/mivec/sg` c100us **exactly**: dynamic 2.157 s vs. measured 2.156 s; guided 2.060 s vs. rp-5 2.060 s.
6. **Expensive group**: suite 17.77 s, cached 1.04 s.
   - mivec/sg c100us (the 2.16 s case is d15 S6; d20 S10 is the c1us case): the largest TG holds 15,625 points = **1.72x the balanced share**. The timeline shows 15 threads idle after ≈ 0.75 s.
   - Largest-first order alone: 2.16 → 1.56 s. Only splitting that TG reaches the bound of 0.91 s.
   - mivec/dsg c100us: 0.88 → 0.55 s with largest-first.
   - WP-56's shift is guided → dynamic in TG order: c100us 0.955x (+4.6 %), c1us d20 1.128x, as the model predicts (guided / dynamic ratio 1.139).
   - 1-thread efficiencies: interplin/sg ramp 0.82 (its work is also 1.32x uniform), interp/sg ramp 0.81, mivec/sg ramp 0.42, interplin/dsg d5 0.49 (chunks).
7. **Memory**: see above. Largest remaining peaks: sg cc d2 L20 191 MiB, sg equi d2 L20 114, dsg b1 d20 83, cached interplin d8 59.5, dsg b1 d15 52, dsg b0 d3 44 MiB.

## Profile shift / follow-ups
Re-ranked expected savings (rp-6 suite 36.33 s; `expensive/` on its own line):
1. **WP-77 extended: largest-first TG order** (cost ∝ points; one per-grid size order, e.g. an O(nTG) counting sort by ⌊log2 nGP⌋ or a permutation computed at generation; reduction order changes; SG quadrature is not thread-count independent today, WP-61). Suite **≈ 0.25–0.4 s**:
   - mivec d10 / d5 ≈ 0.11 s;
   - DSG b0 d7–d3 ≈ 0.13 s (the d10 tail is measured at 0.75 % = 41 ms per call; this absorbs WP-51's order part);
   - sg d2 L20 ×2 ≈ 0.05–0.1 s;
   - quad/sg b0 ≤ 0.05 s;
   - interp/sg ≈ 0.03–0.08 s (the d7 / d5 / d3 / d2 gains minus d10 L2, where the model has guided 0.934 vs. largest-first 0.908).
   - `expensive/`: ≈ **1.1–1.6 s** (mivec sg c100us −0.6, dsg c100us −0.33, sg c1us −0.18, ramp similar).
   - Owner decision (new evidence for the dropped WP-19 slabs): splitting TGs above the balanced share saves another −0.65 s on `expensive/mivec/sg` c100us. In the cheap suite it applies only to mivec d15 S10 (largest TG 1.36x balanced) and sg d2.
2. **WP-44** (DSG row loop light): 0.2–0.3 s, unchanged (row loop 5.74 s). Owner places it.
3. **WP-21**: 0.1–0.15 s, unchanged (builder 0.241 s: d20 0.143, d15 0.098 s).
4. **WP-74 extended**: fused last transform (−1 of d + 1 barriers ≈ 14 ms), vectorized fiber transform (≈ −30 ms), and **evaluation chunks by reached slots / 1–4 leaves** (≈ −0.03 s on the suite, the expensive d5 case 0.345 → ≈ 0.19 s).
   - Total ≈ 0.05–0.08 s on the suite, ≈ −0.17 s on `expensive/`.
   - The fixed chunk order keeps results thread-count independent.
5. **WP-59 reframed**: plain guided → dynamic is predicted to **lose** on interp/sg (model d10 L2 0.934 → 0.830, d2 0.595 → 0.706, net ≈ −0.05…−0.1 s). Its interp part goes into 1.
   - What remains: the SG `interplin` TG loop `static` → chunked dynamic. ≈ 0 on the suite (uniform cost, efficiency 0.97 at c100us); `expensive/` ramp 1.29 → ≈ 1.06 s.
   - Low; it can share item 1's session.
6. **WP-51** (row split at low d): ≤ 0.04 s (dsg d3 libgomp 23 %, interp/dsg cc d2 39 %); the order part moves to 1.
7. **WP-57 / WP-58** (memory): unchanged targets.
   - WP-57: dsg b0 d3 L16 44 MiB, −8 MB.
   - WP-58: −33.5 MB on sg d2 L20 (114 / 191 MiB).
   - WP-82's default cache keeps ≤ 17.8 MiB per DSG grid after the call; WP-60's shared weight rows reduce it.
8. **WP-60**: ≤ 0.02 s (serial index build, every e2e call is a first call). **WP-48**: ≈ 0.02 s (interplin d2 SG 0.112 vs. DSG 0.155 s).
9. **WP-75: close** (no candidates).
- Measurement note: cross-snapshot ratios that span WP-76's binary carry ≈ 0.30 s / ±4–8 % on dsg / sg equi b0 and mivec d5. `results/WP-76/e2e-aa-old.md` is the reference.

## Revert
Nothing to revert in code. To discard: `git restore combigrid/perf/ANALYSIS.md`, delete `combigrid/perf/results/RP-6/` and this summary (`.work/snapshots/rp-6`, `.work/profiles/RP-6` are git-ignored).
