# RP-4 — Re-profiling after WP-08 / WP-06 / WP-52 (cumulative progress, re-ranking, placement check)

Status: PROPOSED · Date: 2026-11-13 · Base commit: 6ac45a05d
Recommendation: accept (measurement only). Results:
- interp/sg and interplin/sg are at the integrand floor;
- WP-07 has no judged target left;
- WP-49 is confirmed, and the serial interplin/dsg d2 should be folded into it;
- the `sumOverRows` alignment gives no WP.
Machine: `perf.py calibrate` ratios e2e 1.001 burst / 1.000 sustained, micro 1.003 / 0.999 (1.000 = as at baseline time)

## Change
- None in the source. Snapshot `rp-4` = HEAD (in-tree `perf.py build normal`; tests pass).
- Experiment: `rp-4-sor-a64` (`[[gnu::aligned(64)]]` on `weighted_sum::sumOverRows`). The patch is kept in `results/RP-4/sumoverrows-aligned64.patch` and reverted in the tree; the rebuilt lib is byte-identical to rp-4.
- Diagnostic snapshot `rp-4-binWP45`: rp-4 libraries with the `WP-45-after` `perf_e2e`.
- Measurements:
  - `ab e2e baseline rp-4` (full suite);
  - direct `ab e2e rp-3 rp-4` on `^(quad|mivec)/`;
  - hybrid A/B (benchmark binary);
  - `run micro` of the 93 classic cases;
  - 31 profiles with inline-chain splits;
  - thread scan of the low-d cases;
  - placement check (micro, e2e with 8 rounds, op-cache counters).
- Files: `ANALYSIS.md` (section "Re-profiling RP-4" + contents line). `results/RP-4/`:
  - reports: `e2e`, `e2e-rp-3-vs-rp-4-quad-mivec`, `e2e-binary-placement`, `placement-{micro,e2e}` (`.md` + `.json`), `placement-opcache.md`, `micro-run.{md,json}`;
  - `thread-scan/`;
  - derived: `groups.md`, `profile-splits.md`, `profiles/`;
  - scripts: `groups.py`, `run_profiles.sh`, `summarize_profiles.py`, `opcache_stat.py`;
  - the patch.

## Gain vs. cost
- Cumulative, baseline → rp-4 (38 common cases, 16 t): **69.89 → 36.16 s (1.933x, 33.74 s saved)**.
  - d ≥ 7: 59.80 → 30.36 s (1.97x).
  - 37 faster, 1 within noise (`interplin/sg/.../d2`), **0 SLOWER**.
  - Since rp-3 (38.45 s): **1.063x**, all of it from WP-06 / WP-08.
  - Full suite with the 10 new DSG interpolation cases: **38.17 s**.

| group | baseline → rp-4 | speedup | rp-3 → rp-4 | share of the suite |
|---|---|---|---|---|
| quad/sg | 22.28 → 9.24 s | 2.41x | 0.996x | 24.2 % |
| quad/dsg | 26.72 → 17.83 s | 1.50x | 0.995x | 46.7 % (b0 40.3) |
| interp/sg | 6.60 → 4.24 s | 1.56x | **1.345x** | 11.1 % |
| interplin/sg | 4.97 → 3.52 s | 1.41x | **1.293x** | 9.2 % |
| mivec | 9.33 → 1.34 s | 6.97x | 0.940x | 3.5 % |
| interp/dsg (new) | 0.82 s | – | – | 2.1 % |
| interplin/dsg (new) | 1.20 s | – | – | 3.1 % |

- Memory: peak RSS geomean B/A 1.014 (rp-3 1.014); 12 `MEM+`, 4 `MEM−`.
  - As at RP-3: sg d2 equi 364 → 620 MiB, cc 799 → 1079 MiB; dsg b0 +1.8…+6.1 MiB; dsg b1 d20 / d15 −113 / −81 MiB.
  - WP-06: interp cc d10 23.7 → 12.2 MiB, d7 13.0 → 12.2 MiB (`MEM−`), cc d2 +1.2 MiB.
  - New small `MEM+` (+0.6…+2.7 MiB) with the process floor at 10.2 → 10.4 MiB.
- API / functionality: none. Maintainability: n/a.

## Tests
- n/a (no code change). `perf.py test` (normal) passed before the snapshot. `perf.py verify` OK at the start and the end.

## Reference check
- n/a (no code change).

## Benchmarks (A = baseline fed2cdb4e, B = rp-4 6ac45a05d; speedup = A/B)
L1 micro (1 t, `run micro`, rp-4; RP-3 in brackets):

| case | rp-4 |
|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 vs. sol/interp_tg_integrand | 86.98 vs. 84.84 ns/point (1.025x of the floor) [108.7] |
| micro/interp/global_tg/cc/b0/d10/L2 · cheb1/b0/d5/L4 | 225.6 (1.041x floor) · 91.9 ns/point (1.076x) [cheb1 194.4] |
| micro/interplin/tg/equi/b1/d8/L5 vs. sol/interplin_tg_integrand | 141.7 vs. 141.4 ns/corner [167.8] |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 · quad/process_tg/equi/b0/d10/L4 | 26.46 · 21.96 ns/point [26.50 · 21.98] |
| micro/quad/process_tg/cc/b0/d2/L16 vs. equi | 63.4 vs. 15.2 ns/point [64.3 vs. 15.6] |
| micro/interplin/dsg/equi/b1/d10/L5 · d2/L16 vs. sg | 46.2 vs. 202.0 · **69.2 vs. 66.8** ns/point |

L3 e2e (16 t, `total`; full table `e2e.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.964 s | 6.848 s | 1.018 | [1.012, 1.024] |
| quad/dsg/equi-exp/b0/d10/L7 | 5.685 s | 5.368 s | 1.056 | [1.047, 1.065] |
| quad/sg/equi-sin/b0/d7/L7 | 3.524 s | 2.404 s | 1.468 | [1.450, 1.486] |
| interplin/sg/equi/b1/d10/L5/p32 | 2.687 s | 1.850 s | 1.454 | [1.424, 1.484] |
| interp/sg/cc/b0/d10/L2/p8 | 2.235 s | 1.627 s | 1.376 | [1.364, 1.387] |
| interplin/sg/equi/b1/d8/L5/p256 | 1.671 s | 1.200 s | 1.407 | [1.366, 1.450] |
| quad/dsg/equi-exp/b1/d20/L6 | 6.759 s | 1.214 s | 5.571 | [5.551, 5.592] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 3.324 s | 482.2 ms | 6.899 | [6.714, 7.089] |

Sum of medians (38 common cases): A = 69.894 s, B = 36.157 s, saved = 33.737 s; foreign load ≤ 0.3 % · Full reports: `combigrid/perf/results/RP-4/`

Peak memory (flagged, largest):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 363.8 MiB | 620.0 MiB | 1.704 | MEM+ |
| quad/sg/cc-uq/b0/d2/L20 | 799.1 MiB | 1078.7 MiB | 1.350 | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 331.1 MiB | 217.6 MiB | 0.657 | MEM− |
| interp/sg/cc/b0/d10/L2/p8 | 23.7 MiB | 12.2 MiB | 0.514 | MEM− |
| quad/dsg/cc-uq/b0/d10/L7 | 37.0 MiB | 43.1 MiB | 1.163 | MEM+ |

Geomean B/A = 1.014 · process floor A = 10.2 MiB, B = 10.4 MiB

## Attempts not kept
- `[[gnu::aligned(64)]]` on `sumOverRows` (owner's placement check). Both instantiations moved to offset 0, `.text` +176 B, benchmark binaries identical.
  - e2e `^(interp|quad)/sg/` (8 rounds): sum **0.994x**. interp/sg is all noise (`cc/b0/d10/L2/p8` 0.997 [0.978, 1.016]); **5 quad/sg cases SLOWER** (0.983–0.995).
  - micro: sum 1.000x. `interp/sg/cc/b0/d8/L2` 0.995 (not recovered); `quad/process_tg` d10 0.987 / 0.993 SLOWER.
  - op-cache misses at identical instructions: e2e interp −11 %, quad +2.4 % / −1.9 %; micro +15 % / +21 %.
  - Verdict: not real for interp, a cost for quad → **no WP**, reverted.

## Answers to the RP-4 questions
- **Suite and groups**: shown above. quad/dsg and quad/sg shares are unchanged within ≈ 1 point:
  - DSG b0 d7/d10: row loop 5.50 s [5.55], trie work 3.21 s [3.30], dispatch 0.98 s;
  - b1: builder 0.28 s;
  - SG `sumOverRows` (now its own symbol) 22.7 / 16.3 / 5.6 / 14.0 %.
- **"Unchanged" cases slower since rp-3** (dsg equi b0 d3–d7 0.96x, mivec d5 0.93x): a placement shift of the integrand in the protected `perf_e2e` (`expSum` at line offset 16 → 48 since the judged-case commit). With the rp-4 libraries and the older binary, d5 1.036x, d7 1.022x and mivec d5 1.063x come back. This is not combigrid code. Cross-snapshot ratios on `exp` cases carry ±2–6 %.
- **interp/sg is at the integrand floor**:
  - integrand 86.9–88.9 % at cc d5–d10;
  - the operator kernel is 0.19 s over all 8 cases, libgomp 0.23 s;
  - micro 1.03–1.08x of the integrand-only loop.
- **interplin/sg is at the floor**: kernel 0.24 s, micro 1.00–1.02x. No operator-side lever ≥ 0.2 s in either: only fewer evaluations help (DSG overloads, WP-09).
- **interplin/dsg**: gather + mark 0.40 s (d10 0.16, d8 0.15, d5 0.08 s). d2 is serial: 0.180 s at 1 t vs. 0.183 s at 16 t, so DSG is 1.66x slower than SG d2.
- **interp/dsg**: row loop 0.07 s; low d is region-bound (libgomp 39–57 %).

## Profile shift / follow-ups
Re-ranked expected e2e savings (rp-4 suite 38.17 s; d ≥ 7 first):
1. **WP-49** (DSG interplin global slot weights): **0.15–0.25 s** (gather + mark 0.40 s; d ≥ 8: 0.31 s). Proposed extension: the serial d2 call (0.184 vs. SG 0.111 s → ≈ 0.07 s; per-call fixed cost ≈ 11 µs; or WP-48's small-call gate).
2. **WP-21** (DSG b1 slack break + sub-trie reuse): 0.1–0.2 s at d15/d20 (builder 0.28 s); unchanged.
3. **WP-44** (DSG row loop light): 0.2–0.3 s at b0 d7/d10 (trie work 3.21 s); unchanged, minimal per case (≈ 1.5–2 % of quad/dsg b0); the owner places it. The row loop (5.5 s, 14 % of the suite) stays the largest operator pool.
4. **WP-05** (SG quad low d): **0.5–0.7 s** at d ≤ 3. cc d2: CC weights 0.31 s + page faults / zero fill 0.33 s; equi d2 0.09 s. Still the largest memory lever (620 / 1079 MiB). The owner may move it up (largest absolute gain).
5. **WP-19** (SG load balancing at d2, after WP-05): **0.1–0.2 s**, narrowed from 0.1–0.3 s. 1 → 16 t: cc d2 3.95x, equi d2 5.26x; libgomp + OpenBLAS 0.18 s.
6. **WP-51** (DSG low-d row split): ≤ 0.02–0.04 s (dsg equi d3 6.4x, interp/dsg cc d2 6.5x scaling).
7. **WP-07** (generic barycentric weights): **≈ 0.003 s judged** (cheb1 d5 1.1 % of 117 ms; dsg equi d3 ≈ 13 % of 6.5 ms) → demote to the end or merge into WP-09. Its value is for high-level generic-family users only.
8. WP-32 / WP-33 / WP-31 / WP-48 (low) unchanged. WP-48 gets a concrete target if WP-49 does not take the interplin/dsg d2 case.

Not a WP:
- the `sumOverRows` alignment (above);
- the benchmark-binary placement (protected file). Owner's option: align the integrands in `perf_problems.hpp`, so that future judged-case edits do not shift cheap-integrand cases by ±2–6 %; the baseline would then need re-measuring.

## Revert
Nothing to revert in code (the alignment patch is already reverted). To discard: `git restore combigrid/perf/ANALYSIS.md`, delete `combigrid/perf/results/RP-4/` and this summary (`.work/snapshots/rp-4*` are git-ignored).
