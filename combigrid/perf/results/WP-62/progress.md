# WP-62 progress log — dimension-adaptive refinement (exploration)

Session start: 2026-11-17 (base commit 14c1e3d16, branch `optimizations`).

## 1. Current state and next steps

- **State** (final, 2026-11-17): session complete. Deliverables: `plan.md`, `literature.md`, this
  log, `proto/`, `raw/`, summary `../../summaries/WP-62-dimension-adaptive-exploration.md`.
- **Next**: the owner decides D1–D9 (plan §5); the master turns plan §6 into work packages.
- A resuming session has nothing left to do except what the owner asks for.

## 2. Findings so far

### Environment (step 0)
- `perf.py verify`: 40 protected files unchanged.
- `git status --short` at start: ` M combigrid/perf/PLAN.md`, ` M combigrid/perf/STATUS.md`
  (the master's uncommitted WP-62 entries; not touched by this session) + the owner's untracked
  `compile*.sh`. No source change in the tree. Note: PLAN.md says "runs on a clean tree (after
  WP-49 is committed)" — WP-49 is neither committed nor in the tree; no build / benchmark process
  was running at the start, so the session proceeds.
- `perf.py envcheck`: **FAIL — not on AC power** (`ac=0`), CPU 5 % busy (VS Code). `calibrate`
  refuses to run. Consequence: until the laptop is on AC, every timing is exploratory
  (`--no-envcheck`) and labelled as such; evaluation counts and errors do not depend on it.

### Literature (question 1) — done
- Written up with sources and verified / recalled marks: `literature.md`.
- Key facts: GG = old / active sets, result includes the active set, safeguard
  `g = max{w |Δ|/|Δ_1|, (1 − w) n_1/n_k}`; NTT / SGMK work model = new points (nested) or whole
  tensor grid (non-nested); SGMK buffering assumes the variables sorted by importance and adds
  variable N_curr + 1 when a buffered one is activated; Tasmanian refines in batches
  (`min_growth`) from fitted anisotropic weights and offers candidate lists for parallel loading.

### Prototype validation (2026-11-17)
- `proto/run_quad check=1`: the adaptive estimate equals `quadrature(f, SparseGrid)` and
  `quadrature(f, DeltasSparseGrid)` on the final index set (rel. diff ≤ 2e-14 in 10 configs: cc /
  equi, b0 / b1, doubling / linear / custom growth, GG / lazy / buffered / a-priori), the
  incremental coefficients equal the library's (0 mismatches), the delta-value store evaluates
  exactly the DSG's number of points (e.g. d3 b0: 1937 = 1937; the SG operator 6449).
- Library limits found:
  - `getFirstTypeChebyshevNodeGenFunc()->getQuadRule()` throws `not_implemented_exception`:
    cheb1 has no quadrature; the non-nested quadrature tests use equidistant + linear growth.
  - A custom growth function (standard CC growth 1, 3, 5, 9, … = `f(0) = 1, f(ℓ) = 2^ℓ − 1` with
    boundary offset 1) works with `SparseGrid`, but `NodeGenFunc::isNested` throws "Unknown
    level-to-grid-point-count function" → no `DeltasSparseGrid`. The built-in doubling growth
    with offset 1 jumps 1 → 5 → 9 points (4 new points per first refinement of a dimension
    instead of 2): relevant for high d (d × 4 instead of d × 2 evaluations to look at every
    dimension once).
  - With boundary offset 0 the level-0 tensor grid has 3^d points: d ≳ 15 needs offset ≥ 1.

### Quadrature comparison (raw traces: `raw/<group>.jsonl.gz`, tables: `proto/analyze.py`)
Setup: CC nodes, doubling growth, boundary offset 1 (d ≥ 10) / 0 (d ≤ 5), Genz families with
a_k = 4 k⁻² (`alg2`), 4·2^{−(k−1)} (`geo`), 5 strong + weak rest (`eff5`); metric N(ε) =
evaluations after which the relative error stays ≤ ε. Methods: `level` complete CT, `qo` a-priori
product profits from 1D probes, `gg` GG with |Δ|, `ggp` GG with |Δ| / new points, `ggp-buf3` SGMK
buffer 3, `lazy` = new: candidates get a predicted Δ (product extrapolation from evaluated
backward neighbours) and are evaluated only when the prediction tops the queue.
- **Adaptive vs. complete CT**: `level` needs 8.5–42x the evaluations of `ggp` at d = 10 (N(1e-4),
  N(1e-6)), ≥ 20x at d = 20, and does not reach 1e-4 within 3e5 evaluations at d ≥ 50 (level 2
  of d = 100 already costs 80 001 points for err 4e-3; `ggp` reaches 7.5e-8 with 2e5).
- **lazy vs. ggp** (ratio of N, geometric mean over 4 functions): alg2 d10 0.54–0.55, d20
  0.21–0.53, d50 0.20–0.38, d100 0.19–0.24; geo 0.71–0.77 at every d; eff5 0.23–0.94. Low d (b0):
  0.63–0.84. The three predictor variants (geo-mean / max / min over the pairs) give the same.
- **profit vs. plain |Δ|** (`gg` / `ggp`): 0.40–1.20, no clear winner.
- **a-priori `qo`**: 0.14–0.88 of `ggp` at d ≥ 10 for these (near-)product functions, but 2–5x
  worse at d ≤ 5 with offset 0 (its 1D probes are expensive there) — and blind, see below.
- **Buffering** (`perm` group, error at small budgets, gauss alg2): d = 1000, budget 300 / 1000 /
  3000: `ggp` 1.4 / 1.4 / 1.4 (cannot even look at every dimension: 4000 points), `ggp-buf3`
  1.9e-5 / 1.0e-3 / 3.2e-4, `lazy-buf3` 5.5e-4 / 2.0e-4 / 2.9e-5. At 3e4: 9.8e-6, 1.4e-5, 2.1e-7.
  **Reversed importance** (important dimensions last): `ggp-buf3` / `ggp-buf10` stay at error
  1.0 (d = 100) and 0.5 (d = 1000) for the whole budget of 3e4 with an error estimate of 8e-7
  (each newly explored dimension is paired with all activated ones: 16 k points per step);
  `lazy-buf3` = `lazy` (1.7e-7 at d = 100), because unevaluated pairs cost nothing.
- **Hidden interaction** (`inter`: gauss + k(x_p) k(x_q), invisible on the axes, p, q the two
  least important dimensions): d = 50, N(1e-4): `ggp` 26 425, `gg` 35 633, `lazy` and `qo` **never**
  (error 0.22 at 3e5 evaluations with a final error estimate of 2.2e-8), `lazy` with exploration share 0.25 /
  0.10: 75 153 / 155 321. d = 10: all find it (≤ 7 889).
- **Bulk marking** (accept all candidates ≥ θ·best, ≤ B per step): θ = 0.5, B = 64: rounds 592 →
  38 (d10), 1187 → 38 (d50) at −1 … +12 % evaluations for N(1e-6); θ = 0.1: up to +50 %;
  θ = 0 (always 64): N(1e-4) up to 7.6x.
- **Reuse of values** (`store` group): delta-value store (new points only) vs. whole tensor
  grids without reuse: N(1e-4) / N(1e-6) 2.0–2.8x fewer evaluations (d10 gauss 18 737 vs.
  44 825; d50 corner 115 577 vs. 279 270).
- **Non-nested** (equidistant, linear growth; work = whole TG): at ≈ 3e5 evaluations d2 4e-6
  (level sets 2e-4), d10 1.3e-5 … 1.5e-4 (level sets 1.1e-2). Works, but far behind doubling.
- **Standard CC growth** 1, 3, 5, 9 (custom growth function, offset 1) vs. built-in 1, 5, 9, 17:
  d = 100 `ggp` error at 400 / 1e4 / 1e5 evaluations 1.0e-3 / 9.5e-6 / 1.8e-8 vs. 1.4 / 5.2e-5 /
  8.7e-7 (5–48x lower error).
- **Vector-valued** (3 outputs gauss, osc, corner; norm = max over outputs of |Δ_o| / |Δ_o(0)|):
  N(1e-6) for all outputs 29 025 (d10) / 110 865 (d50) = the hardest output alone (31 289 /
  115 577) and 0.54x / 0.59x of the three separate runs.
- **Isotropic** (thesis UQ integrand, d5 b0): `ggp` N(1e-6) 4473, `level` 12 033, `lazy` 4401:
  adaptivity does not lose where there is no anisotropy.

### Timings (judged build, AC, calibrate OK; raw: `raw/timing/*.txt`)
- Incremental bookkeeping, cheap integrand (34 / 82 / 156 ns at d = 10 / 50 / 100), 1 thread,
  1e6 evaluations: GG 10.5 / 28 / 47.5 ns per evaluation (17–20 % of the wall time; 3 µs per
  multi-index at d = 100), lazy 21 / 115 / 192 ns (26–42 %). With a 1 µs integrand: GG 1.5 %
  (d10) / 3.6 % (d50), lazy 3.6 % / 12 %; at 1 ms < 0.02 %.
- Threads, 1 µs integrand, 3e5 evaluations: GG one index per step 0.316 → 0.299 s (1 → 16
  threads, 1.06x); with bulk marking θ = 0.5, B = 64: 0.0463 s (6.8x), B = 1024: 0.0416 s
  (7.6x); d50 0.341 → 0.0443 s (7.7x). Lazy with bulk: 5.0–6.5x (d10), 3.2–4.7x (d50; the
  serial prediction is 30–40 % then). Cheap integrand: 16 threads are **slower** (d10 0.062 →
  0.144 s): the parallel regions must be gated by estimated work.
- Rebuild per step with today's classes (`bench_structures`, sets of a GG run):
  `SparseGrid(MIVecSGGenInstr)` for 2109 MIs (d10): 1.5 ms at 16 threads / 2.5 ms at 1;
  15 950 MIs (d100): 73.7 / 513 ms, of which `computeCTCoeffs` 36.9 / 470 ms (50 % / 92 %).
  One rebuild per accepted index over the d = 100 run (3084 steps): ≈ 114 s (16 threads) /
  ≈ 790 s (1) against 0.0475 s incremental bookkeeping; with bulk marking (38 steps) ≈ 1.4 s.
- Per-candidate loop with the public API (single-MI instruction + `SparseGrid` +
  `quadrature`): 147–171 µs per multi-index at d = 10 (1963 points each), 303–369 µs at
  d = 100 (31 points each: ≈ 10 µs per point) against ≈ 3 µs per multi-index incremental.
- DSG on the final d = 100 set (1 M points): construction 75.6 ms, first quadrature 104 ms at 16
  threads (SG quadrature of its 2.2 M points: 26.7 ms).

### Value reuse: by index vs. source-function cache (`raw/timing/value_reuse.txt`, 1 thread)
- GG run with 4e6 charged evaluations, cheap integrand (34 / 156 ns at d = 10 / 100):
  - delta-value store (reuse by index): 0.19 s for 3.99 M function calls covering 21.5 M
    tensor-grid points at d = 10 (9 ns per covered point); d = 100: 0.75 s, 10.3 M points (73 ns
    per covered point, 187 ns per function call of which 156 ns are the function).
  - whole tensor grids through `CachedSourceFunc`: d = 10 0.79 s for 4.0 M points (0.93 M calls,
    198 ns per point), d = 100 2.57 s (1.65 M calls, 641 ns per point); without cache 0.11 /
    0.66 s (28 / 165 ns per point).
  - → the cache adds ≈ 170 ns (d10) / ≈ 480 ns (d100) per looked-up point and pays only for
    functions ≳ 0.3–1 µs (as WP-25); the index store is 20x / 9x cheaper per covered point and
    needs 8 bytes per distinct point (32 MB for 4 M points).

### Error estimate as stopping criterion (all traces of `lowd` / `highd`)
- η = sum of the indicators of the active set (relative to the first increment). Error / η over
  all trace points: GG-profit median 0.28 (high d) / 0.15 (low d), 90 % quantile 8 / 1.4, max 46 /
  7; lazy median 0.9 / 0.74, max 72 / 200.
- Stopping at η ≤ tol: the error at that point is ≤ tol in 71–100 % of the GG runs (median 0.15–
  0.45·tol, worst 23·tol); lazy 58–91 % (worst 30·tol at high d, 110·tol at low d).
  → as in the GG paper ("up to a constant ... up to 10"): a tolerance is met up to a factor.

### Interpolation indicators (`proto/interp_indicators.py`, raw: `raw/interp.jsonl.gz`, `raw/interp.txt`)
- NumPy model (CC, doubling, GG loop), error at 1000 fixed-seed validation points, indicators:
  |∫Δ_ℓ f| (quadrature), max |Δ_ℓ f| on the new points (surplus, SGMK `Linf`), max |Δ_ℓ f| on 200
  fixed random indicator points; each plain and per new point.
- All six stay within a factor ≈ 5 of each other. d = 8 at 6000 points, max error: gauss
  quadrature indicator 1.9e-4 / 9.8e-5 (per work), surplus 7.5e-5 / 3.8e-5, indicator points
  3.9e-5 / 5.4e-5; prodpeak 1.9e-4 vs. 6.0e-5 / 3.8e-5 vs. 4.5e-5; corner and osc equal (≤ 1.4x).
  d = 2 and d = 4: same final errors (≤ 2x), differences only at small budgets.
- → interpolation: surplus on the new points per work as default (needs no extra function
  values); the quadrature indicator is a usable fallback (≤ 5x larger error at equal points).

## 3. Prototype files

All in `combigrid/perf/results/WP-62/proto/` (no tracked source file changed):
- `adaptive_proto.hpp` — the driver (sparse MI set + hash index, 1D rule table with hierarchical
  difference weights, delta-value store or per-TG result store, incremental coefficients, GG /
  lazy selection, buffering, bulk marking, a-priori ordered sets). **working**.
- `test_functions.hpp` — Genz families (osc, prodpeak, gauss, cont, corner with fixed exponent),
  thesis UQ integrand, coefficient decays, exact integrals. **working**.
- `run_quad.cpp` — one run per call (`key=value`), JSON trace, `check=1` hand-over test.
  **working**. `build.sh` builds into `proto/bin/` against the in-tree `lib/`.
- `campaign.py` (runs the groups → `raw/`), `analyze.py` (tables), `summarize.py`. **working**.
- `bench_structures.cpp` — rebuild costs of the library classes along a run. **working**.
- `interp_indicators.py` — NumPy model of the interpolation indicators (run with
  `.venv/bin/python`). **working**.
- `proto/bin/` (binaries) is ignored by `proto/.gitignore`.

## 4. Provisional recommendations / open questions

- Default algorithm: GG with profit indicator, bulk marking (θ ≈ 0.5), incremental structures,
  delta-value store for nested nodes (per-TG results for non-nested ones).
- Lazy prediction as an opt-in selection policy (1.3–5x fewer evaluations for product-like
  functions, but blind to interactions the axes do not show; an exploration share bounds that).
- SGMK buffering only as an explicit option with the ordering caveat; with lazy prediction the
  buffer is not needed for correctness and its cost of a wrong order disappears.
- Library gaps to close first: generic nestedness test (custom growth), standard CC growth,
  parallel regions gated by work.

## 5. Tree state (end of the session)

- Changed tracked files by this session: **none**. `git status --short`: ` M PLAN.md`,
  ` M STATUS.md` (the master's edits from before the session), `?? results/WP-62/`, the new
  summary, the owner's `compile*.sh`.
- Build: `perf.py build debug` ✓ (193 s), `perf.py test --repeat 3` ✓ (3 × "No errors
  detected"), `perf.py verify` ✓ (40 protected files unchanged). The tree is left in the
  **debug** build: before repeating prototype timings run `perf.py build normal` and
  `bash combigrid/perf/results/WP-62/proto/build.sh`.
- The timings of this log used the in-tree normal build left by the WP-56 session (build stamp
  d426bddcf + the WP-56 changes) and the prototype built with `-O3 -march=native`.
