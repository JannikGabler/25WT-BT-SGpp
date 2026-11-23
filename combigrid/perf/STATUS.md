# Optimization status — combigrid (master file)

Last processed: WP-83 (2026-11-22; accepted, commit pending) · Next free WP number: 86 ·
Last re-profiling: RP-6 (2026-11-18) · Finished WPs, results, archive: [HISTORY.md](HISTORY.md) ·
Do-not-retry list, measurement notes: [LESSONS.md](LESSONS.md)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), GCC 15.2,
  16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (2.0 GHz), profile
  `performance`, AC, idle; nosi (WP-13), strict aliasing on (WP-14; off only for the SWIG Java wrapper).
  Toolchain variants (Clang 21 + libomp + lld): `perf/scripts/toolchain.py`.
- **Baseline**: setup commit **fed2cdb4e**, snapshot `baseline` (if cleaned: `perf.py snapshot
  baseline --commit fed2cdb4e`), built with its own flags (flag changes count as progress); never
  re-recorded. Latest re-profiling snapshot **`rp-6` = 37a84e1af** (**2.014x**, memory 0.716; suite of 48
  cases 36.33 s, `expensive/` 17.77 s; others in HISTORY.md).
- **Memory** (WP-15): e2e reports give each process's peak RSS (floor ≈ 10.2–10.5 MiB); `MEM±` =
  |B/A − 1| > 3 % without overlap of the rounds.
- **Machine**: a reboot re-enables boost (`envcheck` fails → ask the owner). Full e2e A/B ≈ 25–30 min.
  More noise notes: LESSONS.md.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed (labelled, weighed
  against API / maintainability / readability); functional / interface / flag changes only after
  consulting the owner; errors stay in the same order of magnitude (refcheck OK/WARN/FAIL); high
  dimensions first; STATUS.md may exceed ~120 lines slightly. Dated decisions:
  - Memory (10-28): runtime ranks; peak memory reported with every change; reduction = secondary goal.
  - Prompt notes (PLAN.md header): strict aliasing (10-28, code-changing WPs), dimension-adaptive
    refinement (10-29, API WPs), Python `.venv/` (10-30, every prompt).
  - Vector-valued (10-30): design T; component k == scalar k within round-off. Doc updates by the sessions.
  - Dimension-adaptive refinement (11-17): exploration WP-62 (Fable 5.2, autonomous) with few
    functional restrictions (prototypes may change API / behavior); implementation WPs from its plan.
  - Expensive group (WP-76, 11-17): `expensive/` (c1us / c100us FMA-chain integrands, `-ramp`,
    `-cached` unranked) has its own sum line; **ranking unchanged** (cheap-suite savings), its effects
    are reported separately; no baseline values (history in `results/WP-76/history/`).
  - TG split (11-18): WP-77 implements the largest-first order; splitting TGs above the balanced
    share is measured as an optional second variant, the owner decides on its summary.
  - **Ranking (11-18)**: judgement-based, no weighting formula (skill `perf-master` step 4; inputs
    table + principles in PLAN.md): gains on every case type (cheap, `expensive/`, cached) count, no
    type is prioritized in general; confidence, effort, lasting complexity, dependencies /
    interactions, memory, sessions' recommendations; high d = mild preference; owner overrides.
    Ranks change only with new evidence (logged reason). Replaces "cheap-suite seconds, d ≥ 7 first,
    operator weights" and the ranking part of the expensive-group decision (11-17).
  - **Case realism (11-18)**: sessions may report at any time gains that only pay for unrealistic
    cases (e.g. very high levels) or realistic cases the suite does not track (note in every prompt).
  - Standard CT (11-17): operators on a plain `SparseGrid` implement the standard CT — optimizable,
    but their core stays; duplicate evaluations are intended (no more than the standard CT needs);
    the DSG variants are the performance alternative and may deviate.
  - Caching (10-30 … 11-02): design W, bitwise user-domain keys, sharded table, stop inserting at
    the limit; API / disk / adaptive details decided in-session; default `storageSpaceLimit` 32 GiB;
    public headers free of Boost / xxHash / LMDB; `USE_LMDB` as `USE_GSL` / `USE_ZLIB`.
- **Refcheck WARN case** (owner 10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session ("Accept/Reject WP-NN: …": code, summary,
  `results/WP-NN/`, STATUS/PLAN); the tree is clean before the next work session.

## Work packages (open, ranked; details in PLAN.md; finished ones in HISTORY.md)

| Rank | ID | Title | Status |
|---|---|---|---|
| – | WP-63 … 72 | Dimension-adaptive CT implementation (A1 nestedness + std CC growth, A2 core, A3 quad driver, A4 non-nested / vector, A5 interp, A6 Python, B1 lazy, B2 buffering, C1 stored values, C2 resume) | needs decision (D1–D9) · **unranked** (owner places them) |
| – | WP-84 | Quadrature rules for inner-only nodes: equi / CC below an offset ≥ 2 override `getQuadRule(n, includeBoundary)` (CC → Fejér-2; equi: session proposes; results at b ≥ 2 change, agreed 11-22) | ready · **unranked** (owner places it) |
| – | WP-80 | Grid-evaluation operator: `f` once per distinct grid point (cache warm-up, testing; name / result / dedup in-session; owner's request) | ready · **unranked** (owner places it) |
| – | WP-81 | Explanation / visualization foundations: exploration + requirements with the owner in-session (info helpers, strings, plots) → implementation WPs | ready · **unranked** (owner places it) |
| 1 | WP-61 | SG quadrature: bitwise thread-count-independent results (per-TG sums in TG order); **before WP-77** (its reordering then stays bitwise, one re-pinning); after WP-83 (same driver) | **in progress** (prompt 11-22; functional: agreed 11-15) |
| 2 | WP-77 | TG-loop load balance: largest-first TG order (SG quad / mivec, DSG quad, SG interp) + SG interplin dynamic; optional TG split variant (RP-6: suite 0.25–0.4 s, `expensive/` 1.1–1.6 s; absorbs WP-59) | ready (after WP-61) |
| 3 | WP-74 | DSG lin. interp.: fused last transform + vectorized fiber transform + 1–4-leaf evaluation chunks (RP-6: 0.05–0.08 s = 6–10 % of interplin/dsg; `expensive/` d5 −45 %) | ready (low) |
| 4 | WP-21 | DSG b1: slack break in the projection DFS + sub-trie reuse (RP-6: 0.1–0.15 s, b1 d15 / d20 only) | ready |
| 5 | WP-51 | DSG row loop at low d: split large delta TGs (≤ 0.04 s; order part → WP-77) | ready (low; measure first) |
| 6 | WP-57 | Per-thread scratch without zero-filled max-TG buffers (dsg b0 d3 L16 ≈ −19 % RSS) | ready (low; memory; session with WP-60) |
| 7 | WP-60 | DSG quadrature: share 1D weight rows via the unit node table (≤ 0.02 s) | ready (low) |
| 8 | WP-58 | Domain node table shares the unit table's rows on [0, 1] (−33.5 MB on sg d2 L20) | ready (low; memory) |
| 9 | WP-09 | Multi-point interpolation API (proposal: gains vs. complexity) | ready |
| 10 | WP-32 | Cache-line-align per-thread scratch / shared query data | ready (≈ 0 judged) |
| 11 | WP-33 | Base's global `operator new` replacement | needs decision (after WP-32) |
| 12 | WP-31 | MI-set: O(n·d) `isMIVecDownwardsClosed` | ready (low) |
| 13 | WP-48 | Operator drivers: `std::fma`-pinned CT combination + small-call gate (interplin/sg d2 ≈ 20 ms) | ready (low) |
| 14 | WP-85 | Operators' OpenMP regions: generic capture-and-rethrow (user `evaluate` throws, `bad_alloc`, user node families → catchable instead of `std::terminate`; agreed 11-22) | ready (low; after WP-77, same TG loops) |
| 15 | WP-35 | Fragile tests + audit (test-only) | ready (low; may move up) |
| 16 | WP-54 | TODO inventory of combigrid (measurement) | ready (low) |
| 17 | WP-55 | Code-hygiene exploration: verify the backlog (PLAN.md end), propose clean-up WPs | ready (low; after WP-54) |
| 18 | WP-37 | pysgpp `DataVector` iteration / bounds check (base) | needs decision |
| 19 | WP-30 | Vector-valued: small-m optimizations | planned |
| 20 | WP-34 | Python source functions: GIL contention at 16 t (36x) | planned (owner places it) |
| 21 | WP-39 | Disk tier: append-only log write path + parallel load | planned (owner places it) |
| 22 | WP-41 | Adaptive caching: disk rule weighted by active threads | planned (late) |
| 23 | WP-42 | Adaptive caching: "keep" hint / non-myopic decisions | planned (late) |
| 24 | WP-78 | Caching: `CachedSourceFunc` overhead at 1 µs × 16 t (WP-76: cached interplin/sg d8 efficiency 0.20) | planned (owner places it; master: size it soon, cached cases now count) |
| 25 | WP-44 | DSG row loop light: single-pass row weights + inline `compRowWeights` (RP-6: 0.2–0.3 s at b0 d7/d10 ≈ 1.5–2 % of quad/dsg; S) | planned (owner places it; master: ≈ rank 5) |
| 26 | WP-07 | Generic barycentric weights cache (≈ 0.003 s judged; may merge into WP-09) | ready (low) |
| last | WP-43 | Final clean-up: phase references out of code docs + phase leftovers | ready (always last) |

## Next action

**WP-61 in progress** (prompt 11-22; rank 1: bitwise thread-count-independent SG quadrature, makes
WP-77's reordering bitwise neutral). Owner commits "Accept WP-83" (checkpoint 18346c71d + the owner's
staged rework; STATUS / PLAN / HISTORY / LESSONS) before WP-61 starts. Next master start: process
WP-61, then **WP-77** (largest-first TG order; TG split as optional variant). Adaptive WPs 63–72 wait
for the owner (D1–D9 + placement); WP-84 / WP-80 / WP-81 wait for the owner's placement. Hygiene
findings → **Code-hygiene backlog** (end of PLAN.md) → WP-54 → WP-55 → clean-up WPs.

## Open decisions for the owner

- **WP-62 D1–D9** (plan `results/WP-62/plan.md` §5; recommendations in PLAN.md WP-62) and the
  placement of WP-63 … 72 — owner decides later (11-17).
- WP-09 (after its summary): adopt the proposed interface or not.
- Placement: WP-80 (grid evaluation), WP-81 (visualization), WP-84 (equi / CC rules below an offset ≥ 2;
  correctness, relevant for adaptive offsets). Optional: protected `quad/sg/cheb1-*` reference case (none yet).
- CC weights with `-ffp-contract=off` (bitwise build-independent weights; ≤ 6.7e-16/m): **not now** (11-15).
- Placement of WP-54 / WP-55 (hygiene; may move up); WP-35 up (time-seeded test hit in several sessions).
- WP-33 (after WP-32): drop / restrict / align large allocations only / keep base's `operator new`
  (WP-23: memory −5 %, generation 1.1–1.4x; datadriven relies on it).
- WP-04 `TrieNode` 24 → 16 B (optional; `results/WP-04/*.patch`): −3 MiB per b0 case for b0 0.982x.
- WP-37 (base's Python `DataVector` iteration, segfault today; base API change): placement. WP-34: placement + mechanism (Python callables 1 t 0.78 vs. 16 t 25.1 µs ≈ 32x; e.g. serial
  operator or `OMP_NUM_THREADS=1` hint). WP-39: placement. WP-44: placement (largest remaining DSG lever;
  RP-6: 0.2–0.3 s, S; master: ≈ rank 5). WP-78 (cache overhead 1 µs × 16 t): placement (master: size it soon).
- Adaptive as the default `CachePolicy` (not planned): cheap functions 0.57–0.91x of plain instead
  of 0.07–0.30x; expensive pay ≈ 3–7 ns per hit.
- WP-06 option (B) (only if asked): fix dimensions with unit-vector basis values (on-node queries).
- Owner files outdated (backlog E): CLAUDE.md §5 banner claim; ANALYSIS.md WP-29c symbol names;
  SG++ install docs (optional Boost / xxHash, LMDB, CI `liblmdb-dev`).

## Re-profiling

Last: RP-6 (2026-11-18; rp-6 = 37a84e1af; accepted). Accepted optimizations since: **0**. WP-79
(feature, 1.002x) and WP-83 (bug fix + feature, 0.996x = noise) do not count. Rule: features /
refactorings / measurement WPs count only if a group changes ≥ 1.3x or a hot-spot ranking shifts. The `interp/dsg`, `interplin/dsg` and `expensive/`
cases have no baseline values (reported absolutely; `expensive/` history in
`results/WP-76/history/`, rp-6 values in `results/RP-6/e2e-expensive-rp-5-vs-rp-6.md`).

## Log (newest first; older entries in HISTORY.md)

- 2026-11-22 WP-83 summary processed: **accepted** by the owner incl. the owner's staged rework
  (checkpoint 18346c71d; commit pending): cheb1 SG quadrature (`FejerFirstQuadRule`, interpolatory with
  boundary nodes; owner in-session: additive virtual `getQuadRule(n, includeBoundary)`), rules obtained
  serially (`std::terminate` → exception). Judged 0.996x (noise), `expensive/` 0.999x, memory 1.003
  (0 flags), refcheck = known WARN, existing results bitwise. Owner: follow-ups → new **WP-84** (equi /
  CC rules below an offset ≥ 2, functional agreed; owner places it) and **WP-85** (generic
  capture-and-rethrow in the operators' OpenMP regions; rank 14, after WP-77; ranks ≥ 14 +1). Rule cost
  at levels ≥ 15: not planned. 1 "Do not retry" line, 1 measurement note. WP-61 prompt issued.
