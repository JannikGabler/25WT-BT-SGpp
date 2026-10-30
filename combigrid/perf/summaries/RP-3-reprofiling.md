# RP-3 — Re-profiling after WP-03 (cumulative progress, peak memory, re-ranking)

Status: PROPOSED · Date: 2026-10-30 · Base commit: 0c6ecfc4b
Recommendation: accept (measurement only). Keep the owner's fixed order and the ranks behind it:
- every runtime estimate stands (shares unchanged since RP-2);
- MI-set generation is finished as a runtime topic;
- WP-10 has new decision data (cold first call +63…+130 ms per process).
Machine: `perf.py calibrate` ratios e2e 1.002 burst / 1.001 sustained, micro 1.032 / 1.034 (1.000 = as at baseline time)

## Change
- None (measurement). Snapshot `rp-3` = HEAD (in-tree `perf.py build normal` of 0c6ecfc4b; "dirty" only because of the owner's untracked `compile*.sh`). Since rp-2 the production code changed by WP-03 and by 0273aa1d7 (`SourceFunc` lost its unused cache member).
- Measurements:
  - `ab e2e baseline rp-3` (full suite, incl. peak memory) and a direct `ab e2e rp-2 rp-3` of interp + interplin;
  - `run micro` of the hot-spot cases (without `micro/cache/`, `micro/vec/`);
  - `perf.py profile` of 21 e2e cases (≥ 1 per group, high d first), split by inline chain (RP-2 method);
  - a stage split of the mivec generation (scratch program);
  - an OpenBLAS cold-start diagnostic (`OPENBLAS_NUM_THREADS=1`, `--warmups 0`, `strace`).
- Files: `ANALYSIS.md` (section "Re-profiling RP-3" + contents line). `results/RP-3/`:
  - reports: `e2e.md/.json`, `e2e-rp-2-vs-rp-3-interp.md/.json`, `micro-run.json`;
  - derived data: `groups.md`, `profile-splits.md`, `mivec-generation-split.md`, `openblas-cold-start.md`, `openblas-diag-raw.json`;
  - `profiles/`: 21 top lists + 21 splits;
  - scripts: `profile_split.py`, `groups.py`, `openblas_diag.py`, `mivec_gen_split.cpp`.

## Gain vs. cost
- Gain, cumulative baseline → rp-3 (full suite, 16 threads):
  - **e2e 69.92 → 38.45 s (1.818x, 31.47 s saved)**; d ≥ 7: 59.76 → 31.90 s (1.87x).
  - Verdicts: 37 faster, 1 within noise (`interp/sg/equi/b0/d3/L5/p64`), **0 SLOWER**.
  - Since RP-2 (43.65 s): 1.135x, all of it mivec (6.35 → 1.26 s, 5.05x). The other groups are within 0.3 %, except interplin 1.023x (session drift: direct A/B all noise).

| group | baseline → rp-3 | speedup | rp-2 → rp-3 |
|---|---|---|---|
| quad/sg | 22.28 → 9.20 s | 2.42x | 0.997x |
| quad/dsg | 26.70 → 17.75 s | 1.50x | 1.000x |
| interp | 6.60 → 5.70 s | 1.16x | 1.006x |
| interplin | 5.02 → 4.55 s | 1.10x | 1.023x |
| mivec | 9.32 → 1.26 s | 7.42x | 5.05x |

- Memory, peak RSS geomean B/A 1.014 (rp-2 1.017):
  - 8 `MEM+`, as in RP-2: sg d2 (WP-01 scratch) equi 364 → 620 MiB, cc 805 → 1064 MiB; 6 dsg b0 (WP-02 slot arrays) +1.1…+5.1 MiB.
  - 3 `MEM−`: dsg b1 d20 331 → 218 MiB, d15 234 → 150 MiB, mivec d10 17.7 → 17.1 MiB.
  - rp-2 → rp-3 per case geomean 0.995 (cross-session).
- API / functionality: none. Maintainability / readability: n/a.

## Tests
- n/a (no code changed). `perf.py build normal` + `perf.py test` passed before the snapshot. `perf.py verify` OK at start and end.

## Reference check
- n/a (no code changed).

## Benchmarks (A = baseline fed2cdb4e, B = rp-3 0c6ecfc4b; speedup = A/B)
L1 micro (1 thread, `run micro` absolute; RP-2 values from ANALYSIS.md; micro calibrate 1.032 vs. 0.994 at RP-2):

| case | rp-2 | rp-3 |
|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 26.12 ns/point (1.20x SG kernel) | 26.50 ns/point (1.21x SG kernel 21.98) |
| micro/dsg/prepare/equi/b1/d20/L6 | 65.7 ms | 63.9 ms |
| micro/interp/global_tg/cc/b0/d5/L6 | 107.7 ns/point | 108.7 ns/point |
| micro/interplin/tg/equi/b1/d8/L5 | 169.3 ns/corner | 167.8 ns/corner |
| micro/mivec/ct_coeffs/d10/S12/n1000 | 15.6 ms | 15.8 ms |

L3 e2e (16 threads, `total`, the longest rp-3 cases; full table `e2e.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.965 s | 6.785 s | 1.023 | [1.012, 1.035] |
| quad/dsg/equi-exp/b0/d10/L7 | 5.653 s | 5.396 s | 1.048 | [1.044, 1.052] |
| interplin/sg/equi/b1/d10/L5/p32 | 2.695 s | 2.468 s | 1.091 | [1.069, 1.114] |
| quad/sg/equi-sin/b0/d7/L7 | 3.525 s | 2.418 s | 1.458 | [1.446, 1.470] |
| interp/sg/cc/b0/d10/L2/p8 | 2.231 s | 1.990 s | 1.124 | [1.116, 1.133] |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 2.271 s | 742.6 ms | 3.052 | [2.894, 3.220] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 3.249 s | 483.6 ms | 6.744 | [6.381, 7.127] |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 1.914 s | 3.08 ms | 595.2 | [550.5, 643.5] |

Sum of medians (38 cases): A = 69.922 s, B = 38.451 s, saved = 31.471 s; foreign load ≤ 0.3 % · Full reports: `combigrid/perf/results/RP-3/`

Peak memory (flagged and largest):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 804.7 MiB | 1063.6 MiB | 1.322 | MEM+ |
| quad/sg/equi-exp/b0/d2/L20 | 364.1 MiB | 619.7 MiB | 1.702 | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 330.9 MiB | 217.5 MiB | 0.657 | MEM− |
| quad/dsg/equi-exp/b1/d15/L7 | 234.0 MiB | 150.2 MiB | 0.642 | MEM− |
| quad/dsg/cc-uq/b0/d10/L7 | 37.2 MiB | 42.3 MiB | 1.135 | MEM+ |

Geomean B/A = 1.014 · process floor A = 10.3 MiB, B = 10.3 MiB

## Attempts not kept
- None. The RP-2 profile method was reproduced exactly: DSG b0 d10 14.5 / 11.6 / 11.6 / 5.4 % vs. RP-2 14.4 / 11.6 / 11.8 / 5.3 %. `MI::operator[]` and `std::fill_n` frames count for the helper that inlines them.

## Answers to the RP-3 questions (16-thread profiles; RP-2 values in brackets)
- **mivec, split of the 1.26 s**: operator 1.241 s (98.9 %), generation 13.8 ms (`mivec-generation-split.md`).
  - Generation stages at d10 (7.6 ms): closure 0.74 ms (10 %), **CT coefficients 4.82 ms (64 %)**, filter + partition 1.38 ms (18 %), tensor-grid materialization 0.61 ms (8 %). At d15 / d20 the CT coefficients are 70 / 71 %.
  - The CT coefficients scale only 1.7–3.8x from 1 to 16 threads.
  - Total 0.036 % of the suite → no WP.
  - Operator = SG quadrature: d5 / d10 integrand 76.9 / 70.2 %, `processTensorGrid` 15.2 / 11.8 %.
- **OpenBLAS start-up of the short d15/d20 processes (WP-10)** (`openblas-cold-start.md`):
  - The pool does not touch the judged `total` (0.97–1.02 with `OPENBLAS_NUM_THREADS=1`).
  - It adds **+63…+130 ms to the first operator call** of every process: mivec d20 135.7 vs. 6.2 ms (22x), interp d3 169 vs. 54 ms, dsg b0 d10 +67 ms. Process wall +56…+135 ms; loading alone costs +2–3 ms.
  - Mechanism: the 15 pool threads busy-wait after their start, and OpenMP barrier wake-ups then complete only every 4-ms scheduler tick.
  - The d15/d20 profiles (libgomp 47–51 %, OpenBLAS 29–40 %) show exactly this warm-up spinning.
- **quad/dsg, quad/sg, interp, interplin: shares unchanged** (≈ 1 point; no LTO shift this time):
  - **WP-04 confirmed at 1.5–2.5 s**: b0 d7/d10 row loop 5.55 s [5.53], per-row trie work 3.30 s [3.3]; micro 26.50 vs. 21.98 ns/point (1.21x [1.20]) → parity ≤ 2.5 s.
  - **WP-17 confirmed (≈ 1–1.5 s)**: dispatch DSG b0 0.92 s [0.9], b1 0.13 s [0.13], quad/sg ≈ 0.45–0.55 s [≈ 0.4] (0.29 s in the profiled cases), mivec 0.11 s.
  - **WP-06 confirmed (0.6–0.7 s, d ≥ 7 ≈ 0.4 s)**: `interpolateFirstDim` 19.1 / 17.8 / 20.0 % (cc d10 / d7 / d5), incl. `getGridPoint` 5.6–8.5 %; with barycentric 0.73 s at d ≥ 7.
  - **WP-08 confirmed (≈ 0.5 s, d ≥ 7 ≈ 0.45 s)**: corner loop 28.1 / 25.8 % (d10 / d8) = 1.09 s.
  - **WP-05 confirmed (0.5–0.8 s, d ≤ 3)**: CC d2 weights 24.8 % + kernel 23.1 %; micro CC 64.3 vs. equi 15.6 ns/point.
- **WP-29 "before" picture** (full table: ANALYSIS.md / `profile-splits.md`):

| kernel | top-case shares |
|---|---|
| (a) SG row loop | 25.2 / 18.6 / 6.3 / 12.7 % (sg equi d10, cc d10, sin d7, b1 d20); mivec 15.2 / 11.8 % |
| (b) DSG row loop | 43.2 / 33.1 % (b0 d10 equi / cc), 43.3 / 33.8 % (d7); b1 d20 / d15 21.1 / 24.9 % + builder 13.1 / 9.0 % |
| (c) `interpolateFirstDim` + barycentric | 22.6 / 22.6 / 26.0 % (cc d10 / d7 / d5); cheb1 d5: generic weights 24.8 % + allocations 7.2 % |
| (c) corner loop | 28.1 / 25.8 % (d10 / d8) |

  Dispatch inside these kernels: 5–10 % (SG, DSG), ≤ 2.5 % (interp, interplin).
- **WP-32 / WP-33**: no new evidence; ranks confirmed.
  - `libsgppbase.so` ≤ 0.06 % at d ≥ 7 (WP-23: ≤ 0.09 %).
  - With base's 64-byte `operator new`, interp/interplin show no shift (direct A/B all noise), so WP-32 ≈ 0 in the judged build. WP-33's memory lever (−5 % geomean) is unchanged.
- Unchanged-code shift: `interp/sg/cheb1/b0/d5/L4/p64` 0.980x rp-2 → rp-3 (−4.5 ms; libgomp 21.3 vs. 18.9 %). Placement / LTO band, negligible.

## Profile shift / follow-ups
Ranking (expected e2e saving at rp-3, suite 38.45 s). Positions 1–7 are fixed by the owner (2026-10-30) and only annotated:
1. RP-3 (this).
2. WP-29a–d: feature; scalar path ≈ 0. Kernel shares above: largest vector gain in DSG b0 (row loop 33–43 %).
3. WP-26 → 4. WP-28 → 5. WP-27: features; judged suite 0 by construction.
6. **WP-04**: 1.5–2.5 s, b0 d7/d10 (row loop 5.55 s, trie work 3.30 s); memory ≈ −3 MiB per b0 case.
7. **WP-17**: ≈ 1–1.5 s (dispatch ≈ 1.6 s over DSG + SG + mivec); interface.

Remaining runtime WPs (d ≥ 7 first; ties: quadrature > global interp > linear interp > generation). The order is unchanged from PLAN.md:
8. WP-09: decision data, gates WP-06/WP-08.
9. WP-06: 0.6–0.7 s (d ≥ 7 ≈ 0.4 s).
10. WP-08: ≈ 0.5 s (d ≥ 7 ≈ 0.45 s). Ties with WP-06 at d ≥ 7 within precision; global interpolation first by the tie-break.
11. WP-21: 0.1–0.2 s (b1 d15/d20; builder 0.27 s).
12. WP-18: runtime 0.03–0.07 s + memory.
13. WP-05: 0.5–0.8 s at d ≤ 3, largest memory lever; the owner may move it up.
14. WP-07: ≈ 0.2 s. 15. WP-19: 0.1–0.3 s.
16. **WP-10**: judged suite nil. New: cold first call −63…−130 ms per process (up to 22x for ms pipelines). Owner decision: is the cold-call latency of one-shot processes worth the build change?
17. WP-32: ≈ 0. 18. WP-33: memory, needs decision. 19. WP-31: nil.

New findings (no new WP):
- mivec generation: CT coefficients are 64–71 % of the generation and scale only 1.7–3.8x; 8 ms on the suite. Relevant later for dimension-adaptive refinement (incremental CT-coefficient update).
- `MIVecSGGenInstr` filter + partition is 1.38 ms at d10, more than the closure. Not worth a WP.

Methodology:
- `results/RP-3/profile_split.py` reproduces the RP-2 inline-chain split; reusable.
- The OpenBLAS share in profiles of short processes is warm-up spinning, not measured time.

## Revert
Nothing to revert in code. To discard: `git restore combigrid/perf/ANALYSIS.md` and delete `combigrid/perf/results/RP-3/` and this summary (`.work/snapshots/rp-3` is git-ignored).
