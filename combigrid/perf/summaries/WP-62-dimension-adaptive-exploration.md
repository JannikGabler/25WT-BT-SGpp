# WP-62 — Dimension-adaptive refinement: exploration + implementation plan

Status: PROPOSED · Date: 2026-11-17 · Base commit: 14c1e3d16
Recommendation: owner decision needed — the plan is decision-ready (9 decisions, 10 work packages); no library source was changed.
Machine: `perf.py calibrate` ratios e2e 1.004 / 1.000, micro 0.999 / 1.001 (the session started on battery, `envcheck` FAIL; all timings below were taken after the laptop was on AC and `envcheck` passed)

## Change
- Exploration only: a C++ prototype of an adaptive quadrature driver against the library (`results/WP-62/proto/`), 706 comparison runs, timing runs, a NumPy model for interpolation indicators, a literature review.
- Deliverables: [plan](../results/WP-62/plan.md) (approaches, design, API, decisions D1–D9, WP split A1–C2), [literature](../results/WP-62/literature.md), [progress log](../results/WP-62/progress.md).
- Files: only new files under `combigrid/perf/results/WP-62/` and this summary. Tracked files changed: none (`PLAN.md` / `STATUS.md` carry the master's uncommitted WP-62 entries from before the session).

## Gain vs. cost
- Gain: decision data. Adaptive (Gerstner–Griebel with profit, `ggp`) vs. the complete CT: 8.5–42x fewer evaluations at d = 10; at d = 100 the complete CT of level 2 costs 80 001 points for error 4.0e-3, `ggp` reaches 7.5e-8 with 2e5. New "lazy" selection (predicted increments, evaluated only when they top the queue): 0.19–0.84x of `ggp`'s evaluations, but blind to interactions the axes do not show.
- Memory: judged suite unchanged. The recommended value store needs 8 bytes per distinct point and output (32 MB for 4 M points).
- API / functionality: none made. Proposed: new `AdaptiveConfig` / `AdaptiveCombination` + an instruction that owns its index set; existing grids and operators unchanged.
- Maintainability / readability: the plan keeps the adaptive code separate (no growable `SparseGrid`); one prerequisite touches existing code (generic nestedness, standard CC growth).

## Tests
- Pinning tests: n.a. (no source change).
- Prototype validation (`run_quad check=1`, 10 configurations): adaptive estimate == `quadrature` on the handed-over `SparseGrid` and `DeltasSparseGrid` (rel. diff ≤ 2e-14), incremental coefficients == `computeCTCoeffs` (0 mismatches), evaluations == DSG points; exact integrals of all test functions checked at d = 2 (≤ 1e-14, `cont` 1e-9).
- Results: debug `test --repeat 3` ✓ · normal n.a. (timings used the in-tree normal build left by the WP-56 session: build stamp d426bddcf + the WP-56 changes, not rebuilt) · ASan n.a. · `perf.py verify` ✓ at start and end.

## Reference check
- `perf.py refcheck`: not run — no code of the judged suite changed.

## Measurements (instead of A/B benchmarks; raw data `results/WP-62/raw/`)
Evaluations for an accuracy, ratio to `ggp` (CC, doubling, a_k = 4 k⁻², geometric mean over 4 Genz functions, ε = 1e-4 / 1e-6):

| d | complete CT | a-priori (1D probes) | SGMK buffer 3 | lazy |
|---|---|---|---|---|
| 10 | 9.6 / 8.5 | 0.48 / 0.56 | 1.00 / 1.00 | 0.54 / 0.55 |
| 50 | not reached in 3e5 | 0.33 / 0.43 | 1.16 / 1.00 | 0.20 / 0.38 |
| 100 | not reached in 3e5 | 0.41 / 0.14 | 1.16 / 1.03 | 0.24 / 0.19 |

Robustness and high d:

| case | result |
|---|---|
| buffer, d = 1000, budget 300 | error 1.9e-5 (unbuffered 1.4: cannot look at all dimensions) |
| buffer, important dimensions last, d = 100, budget 3e4 | error 1.0 with error estimate 8e-7 (unbuffered 6.2e-6, lazy 1.7e-7) |
| hidden pair interaction, d = 50 | `ggp` N(1e-4) = 26 425; lazy and a-priori never (error 0.22, estimate 2.2e-8); lazy + 25 % exploration 75 153 |
| value reuse (new points only) | 2.0–2.8x fewer evaluations than whole tensor grids |
| standard CC growth 1, 3, 5, 9 vs. built-in 1, 5, 9, 17, d = 100 | 5–48x lower error at 1e4 / 1e5 evaluations |
| vector-valued, 3 outputs | cost of the hardest output alone; 0.54–0.59x of three scalar runs |
| stopping at estimate ≤ tol | error ≤ tol in 71–100 % of the GG runs, worst 23·tol |

Timings (judged build, AC):

| measurement | value |
|---|---|
| incremental bookkeeping, GG, 1 thread | 10.5 / 28 / 47.5 ns per evaluation at d = 10 / 50 / 100 (17–20 % with a 34–156 ns integrand, 1.5–3.6 % at 1 µs) |
| rebuild `SparseGrid(MIVecSGGenInstr)`, d = 100, 15 950 indices | 73.7 ms at 16 threads / 513 ms at 1 (`computeCTCoeffs` 36.9 / 470 ms) |
| rebuild per accepted index over that run | ≈ 114 s / ≈ 790 s vs. 0.0475 s incremental |
| public-API loop per candidate | 147–171 µs (d = 10), 303–369 µs (d = 100) per index |
| 16 threads, 1 µs integrand, 3e5 evaluations | one index per step 1.06x; bulk marking (θ = 0.5, ≤ 64) 6.8x (d = 10), 7.7x (d = 50) |
| `CachedSourceFunc` instead of the index store | + ≈ 170 / 480 ns per looked-up point (d = 10 / 100) vs. 9 / 73 ns |

## Attempts not kept
- Nothing was put into the tree. Variants measured and not recommended as defaults: plain |Δ| vs. profit (0.40–1.20, no winner); GG work safeguard w = 0.999 (4–6x the evaluations on the interaction case); fixed batches without threshold (up to 7.6x at small budgets); the three prediction variants (geo-mean / max / min: identical results).

## Profile shift / follow-ups
- Library gaps found: `NodeGenFunc::isNested` throws for custom growth functions (no DSG for standard CC growth); cheb1 has no quadrature rule; offset 0 makes the coarsest grid 3^d points; 16 threads are slower than one for cheap integrands in short parallel regions (work gate, cf. WP-48).
- Not prototyped: interpolation driver against the library (NumPy model only), vector kernels, time-based stopping, per-dimension node families / domains.
- Open decisions for the owner: plan §5 (default selection, indicator, value reuse, separate driver vs. growable grid, bulk defaults, buffering, prerequisite WP, tolerance semantics, interpolation indicator). WP split: plan §6 (A1 → A2 → A3 → A6 → A4 → A5 → B1 → B2 → C1 / C2).

## Prototype files (keep / revert)
- `combigrid/perf/results/WP-62/proto/*` (`adaptive_proto.hpp`, `test_functions.hpp`, `run_quad.cpp`, `bench_structures.cpp`, `interp_indicators.py`, `campaign.py`, `analyze.py`, `summarize.py`, `build.sh`): **keep** as evidence (untracked result files; `proto/bin/` is git-ignored). The plan does not depend on them. Rebuild with `perf.py build normal` + `bash proto/build.sh` before repeating timings (the tree is left in the debug build).

## Revert
Nothing to revert in the source tree. `python3 combigrid/perf/scripts/perf.py reject --apply` keeps `summaries/` and `results/` and would also revert the master's `PLAN.md` / `STATUS.md` edits (add `--keep combigrid/perf/PLAN.md --keep combigrid/perf/STATUS.md`).
