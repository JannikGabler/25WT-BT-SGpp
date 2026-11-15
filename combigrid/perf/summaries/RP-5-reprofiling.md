# RP-5 — Re-profiling after WP-18 / WP-05 (cumulative progress, sg d2 critical path, re-ranking)

Status: PROPOSED · Date: 2026-11-15 · Base commit: 3261399ec
Recommendation: accept (measurement only). Results:
- **merge WP-19 into WP-56**: the largest single lever now, 0.3–0.4 s on `quad/sg/cc-uq/b0/d2/L20`;
- WP-05's DSG d10 placement loss is gone at HEAD (no WP);
- WP-49 / WP-21 confirmed;
- WP-57 gets a judged memory target;
- one new small memory WP.

Machine: `perf.py calibrate` ratios e2e 1.019 burst / 0.999 sustained, micro 0.998 / 1.002 (1.000 = as at baseline time)

## Change
- None in the source. Snapshot `rp-5` = HEAD (in-tree `perf.py build normal`; `perf.py test` passed).
- Measurements:
  - `ab e2e baseline rp-5` and `ab e2e rp-4 rp-5` (full suite each);
  - an 8-round A/B `WP-05-before` vs. `rp-5` on DSG b0 d10;
  - `ab micro rp-4 rp-5` (49 hot-spot cases);
  - 33 profiles with inline splits;
  - op-cache counters of 4 snapshots;
  - per-thread timelines of sg d2 L20 (perf samples);
  - a standalone 1-thread CC-FFT variant benchmark (no library change);
  - heaptrack of 5 peak cases (capped at 8 GB).
- No hybrid snapshot: the rp-4 `perf_e2e` does not run on rp-5 libraries (`SparseGrid` iteration removed by WP-18). The integrands keep their 64-byte line offsets in rp-4 and rp-5, so RP-4's binary effect does not apply.
- Files: `ANALYSIS.md` (section "Re-profiling RP-5" + contents line), `results/RP-5/` (reports, `timeline/`, `heap/`, `profiles/`, scripts; raw data in `.work/profiles/RP-5/`).

## Gain vs. cost
- Cumulative baseline → rp-5 (38 common cases, 16 t): **69.93 → 35.45 s (1.973x)**. d ≥ 7: 59.76 → 30.29 s (1.97x). 37 faster, 1 noise (interplin/sg d2), 0 SLOWER.
- Suite (48 cases): **37.44 s**. Direct rp-4 → rp-5: 38.12 → 37.50 s (**1.017x**, −0.62 s).

| group | baseline → rp-5 | rp-4 → rp-5 (direct) | share of rp-5 |
|---|---|---|---|
| quad/sg | 22.29 → 8.62 s (2.59x) | **1.069x** (cc d2 1.560, equi d2 1.310) | 23.0 % |
| quad/dsg | 26.74 → 17.83 s (1.50x) | 0.999x (b1 1.05–1.07; b0 equi d7 / d10 0.986 / 0.989 SLOWER) | 47.6 % (b0 41.5) |
| interp/sg | 6.59 → 4.22 s (1.56x) | 1.005x | 11.3 % |
| interplin/sg | 5.01 → 3.51 s (1.43x) | 0.998x | 9.4 % |
| mivec | 9.30 → 1.27 s (7.33x) | 1.017x | 3.4 % |
| interp/dsg | 0.83 s | 0.986x (d5 / d7 0.982 / 0.987 SLOWER) | 2.2 % |
| interplin/dsg | 1.17 s | 1.014x | 3.1 % |

- **Memory**: peak RSS geomean baseline → rp-5 **0.724** (RP-4: 1.014), 26 `MEM−`, 4 `MEM+` (+0.4–0.5 MiB, process floor 10.0 → 10.2 MiB).
  - sg cc d2 L20 806 → 309 MiB, equi d2 L20 364 → 124 MiB;
  - dsg b1 d20 332 → 83 MiB, d15 231 → 52 MiB;
  - dsg b0 −18…−30 %.
  - rp-4 → rp-5: 0.764, 27 `MEM−`, 1 `MEM+` (interplin/dsg d2 +0.9 MiB, WP-18's eager unit table).
- API / functionality: none. Maintainability: n/a.

## Tests
- n/a (no code change). `perf.py test` (normal) passed before the snapshot. `perf.py verify` OK at the start and the end.

## Reference check
- n/a (no code change).

## Benchmarks (A = baseline fed2cdb4e / rp-4 6ac45a05d, B = rp-5 3261399ec; speedup = A/B)
L1 micro (1 t, A = rp-4; sum 1.025x):

| case | rp-4 | rp-5 | speedup | 95% CI |
|---|---|---|---|---|
| quad/process_tg/cc/b0/d2/L16 | 24.82 ms | 5.92 ms | 4.199 | [4.172, 4.226] |
| quadrule/cc_weights/n32769 | 4.37 ms | 3.18 ms | 1.375 | [1.370, 1.381] |
| dsg/prepare/equi/b1/d20/L6 | 112.5 ms | 97.8 ms | 1.151 | [1.141, 1.162] |
| quad/process_tg/equi/b0/d10/L4 | 13.89 ms | 14.14 ms | 0.981 | [0.970, 0.993] |
| dsg/process_delta_tgs/equi/b0/d10/L7/s256 | 223.4 ms | 228.1 ms | 0.974 | [0.947, 1.002] |
| quad/operator/cc/b0/d2/L16 · equi (new) | – | 122.2 · 101.7 ms | – | – |

L3 e2e (16 t, `total`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 (A = baseline) | 6.973 s | 6.894 s | 1.011 | [1.005, 1.017] |
| quad/dsg/equi-exp/b0/d10/L7 (A = rp-4) | 5.363 s | 5.440 s | 0.989 | [0.982, 0.996] |
| quad/sg/cc-uq/b0/d2/L20 (A = rp-4) | 1.206 s | 782.1 ms | 1.560 | [1.470, 1.655] |
| quad/dsg/equi-exp/b1/d20/L6 (A = rp-4) | 1.216 s | 1.138 s | 1.067 | [1.063, 1.071] |
| quad/dsg/cc-uq/b0/d10/L7 (A = WP-05-before, 8 rounds) | 6.890 s | 6.888 s | 1.001 | [0.997, 1.005] |

Sum of medians (38 common cases, A = baseline): A = 69.927 s, B = 35.446 s, saved = 34.481 s · Full reports: `combigrid/perf/results/RP-5/`

Peak memory (largest, baseline → rp-5):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 806.5 MiB | 309.1 MiB | 0.383 | MEM- |
| quad/sg/equi-exp/b0/d2/L20 | 364.0 MiB | 124.4 MiB | 0.342 | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 332.0 MiB | 82.8 MiB | 0.249 | MEM- |
| quad/dsg/equi-exp/b0/d3/L16 | 62.3 MiB | 43.9 MiB | 0.705 | MEM- |

Geomean B/A = 0.724 · process floor A = 10.0 MiB, B = 10.2 MiB

## Attempts not kept
- None. The CC-FFT variant benchmark is standalone: `results/RP-5/cc_fft_variants.cpp`, built in the scratchpad; no library build.

## Answers to the RP-5 questions
1. **Suite and groups**: shown above.
   - quad/dsg b0 d7/d10 = 15.0 s (40 % of the suite): integrand 8.6 s, row loop **5.72 s** [RP-4: 5.50]. The row loop splits into `updateCoeffSums` 1.88, `compRowWeights` 1.57, evaluate + addScaled 0.97 and row / advance / dispatch 1.30 s.
   - quad/dsg b1: builder 0.245 s, row loop 0.56 s.
   - quad/sg: integrand-bound above d2 (72–92 %); `sumOverRows` 0.22 + 0.22 + 0.14 s; `process_tg` d10 is below the speed-of-light loop.
2. **sg cc d2 L20 critical path** (per-thread timeline, measured call, operator 725 ms in the sampled run):
   - Threads are idle or spinning 59 % of the time. Balanced work would take ≈ 0.30 s.
   - 13 threads finish after ≈ 250 ms.
   - The critical thread first runs a TG (185 ms), then the largest row (375 ms incl. ≈ 25 % page faults), then its own TG (130 ms).
   - A second thread waits ≈ 260 ms in `call_once` for that row.
   - libgomp 11 % = barrier spin. OpenBLAS 9 % = its pool spinning ≈ 110 ms in the warm-up call only (outside `total`).
   - FFT n = 2^21+1 at 1 t: 339 ms today, **120 ms** as a real FFT of length m (b is non-zero at even indices only; round-off, max |Δ| 4.2e-22 at w ≈ 4.8e-7), 89 ms with a reused plan.
   - Sizing: (a) alone −0.2…−0.25 s; (c) largest-row TGs first alone −0.15…−0.2 s; **(a) + (c) −0.3…−0.35 s**; (b) not needed.
   - WP-19 (TG row split) then removes a ≈ 50–70 ms tail per d2 case (equi d2: 310 ms operator vs. balanced 0.24 s).
   - **Merge WP-19 into WP-56.**
3. **WP-05's dsg cc d10 0.978 is gone at HEAD**: WP-05-before vs. rp-5 1.001 [0.997, 1.005] (8 rounds).
   - Op-cache misses: 3.17e8 (WP-05-before), 3.78e9 (WP-05-after), **3.12e8** (rp-5).
   - In WP-05-after they sat in `forEachGPWithGlobalWeight` 31 %, `uqIntegrand` 29 % and libm `exp` 28 %.
   - The hot functions kept their 64-byte alignment; only their 4-KiB offsets moved (0x85700 → 0x85ec0 → 0x85f00). It was a set conflict across library, benchmark binary and libm; the +64 B since then removed it. Not controllable → **no WP**.
   - The remaining rp-4 → rp-5 loss on dsg b0 equi d7/d10 (0.986 / 0.989) is not op-cache (misses equal; cycles +1.25 % at instructions +0.13 %). It is the same cost WP-18 measured and accepted (domain-table reads + `rowNodes` copy).
4. **Re-sizing**: see the follow-ups below.

## Profile shift / follow-ups
Re-ranked expected e2e savings (rp-5 suite 37.44 s):
1. **WP-56 + WP-19 (merged)**: **0.3–0.4 s** on sg cc d2 L20, + ≈ 0.05 s on equi d2 from the WP-19 part. Memory: FFT temporaries −30 MB at cc d2.
   - (a) real FFT of length m (2.8x). This changes the weights at round-off: the golden hash in `quadrature_weights_test.cpp` must be re-pinned → owner decision.
   - (c) TGs with the largest rows first (largest-first order, dynamic schedule).
   - Then a row split only if a tail ≥ 50 ms remains.
2. **WP-44** (DSG row loop light): 0.2–0.3 s, unchanged. Row loop 5.72 s, `compRowWeights` 1.57 s. Minimal per case; the owner places it.
3. **WP-49**: 0.15–0.25 s confirmed (gather + mark 0.39 s: d10 0.165, d8 0.136, d5 0.077 s), plus the serial d2 ≈ 0.07 s (0.178 vs. SG 0.110 s; libgomp + OpenBLAS 64 %).
4. **WP-21**: **0.1–0.15 s**, narrowed from 0.1–0.2 s (builder 0.245 s [0.28]: d20 0.146, d15 0.099 s).
5. **WP-51**: ≤ 0.04 s, unchanged (dsg equi d3 libgomp 23 %, interp/dsg cc d2 40 %).
6. **WP-57**: runtime ≈ 0, but a judged memory target. `DeltaWeightedSumScratch` takes 8.4 MB (16 × 512 KiB) of the 22 MB heap on dsg b0 d3 L16 (RSS 44 MiB) → ≈ −19 % RSS there.
7. **New, WP-58 (memory, low)**: the domain node table shares the unit table's rows where the mapping is the identity ([0, 1]: `fma(x, 1, 0)` = x bitwise).
   - It saves 33.5 MB on both sg d2 L20 cases: equi ≈ −30 % RSS, whose 101 MB heap is unit + domain table + weight rows; cc ≈ −11 %.
   - Generation −(a few) ms.
   - Risk: ownership / lifetime of shared rows (views); the cache keys use domain bits (unchanged values).
8. Not a WP: WP-05's placement effect (gone) and the dsg b0 0.986–0.989 (WP-18's accepted cost, row loop placement sensitive).
- No new runtime lever ≥ 0.2 s besides 1; interp/sg and interplin/sg stay at the integrand floor.
- Hygiene (known, WP-38 / WP-55): `clenshaw_curtis_quadrature_rule.{hpp,cpp}` and `clenshaw_curtis_quadrature_rule_getter.cpp` lack the copyright banner; cpplint warns in every build.

## Revert
Nothing to revert in code. To discard: `git restore combigrid/perf/ANALYSIS.md`, delete `combigrid/perf/results/RP-5/` and this summary (`.work/snapshots/rp-5`, `.work/profiles/RP-5` are git-ignored).
