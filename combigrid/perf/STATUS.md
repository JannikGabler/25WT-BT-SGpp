# Optimization status — combigrid (master file)

Last processed: WP-48 (2026-11-24; accepted, commit pending) · Next free WP number: 87 ·
Last re-profiling: RP-6 (2026-11-18) · Finished WPs, results, archive: [HISTORY.md](HISTORY.md) ·
Dropped-earlier list, measurement notes: [LESSONS.md](LESSONS.md)

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
  - **Feedback (11-18, generalized 11-24)**: sessions are encouraged to report anything that looks
    wrong, unrealistic or obsolete (unrealistic gains, untracked realistic cases, plan entries, rules,
    decisions, code, docs, tooling) in the summary or directly (note in every prompt).
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
| 1 | WP-77 | TG-loop load balance: largest-first TG order (SG quad / mivec, DSG quad, SG interp) + SG interplin schedule (static has L2 locality, WP-48); optional TG split variant (RP-6: suite 0.25–0.4 s, `expensive/` 1.1–1.6 s; absorbs WP-59); bitwise neutral after WP-61 / WP-48 | ready (next; prompt on hold, owner 11-24) |
| 2 | WP-35 | Fragile tests + audit (test-only): seeded `equidistant_nodes/*`, timing-dependent `concurrent_operator_calls_on_one_function`, FMA-dependent cache test, Eigen / Armadillo-dependent `grid_storage_pinning` (owner 11-24: moved up) | ready (after WP-77) |
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
| 13 | WP-85 | Operators' OpenMP regions: generic capture-and-rethrow (user `evaluate` throws, `bad_alloc`, user node families → catchable instead of `std::terminate`; agreed 11-22) | ready (low; after WP-77, same TG loops as WP-48) |
| 14 | WP-54 | TODO inventory of combigrid (measurement) | ready (low) |
| 15 | WP-55 | Code-hygiene exploration: verify the backlog (PLAN.md end), propose clean-up WPs | ready (low; after WP-54) |
| 16 | WP-37 | pysgpp `DataVector` iteration / bounds check (base) | needs decision |
| 17 | WP-30 | Vector-valued: small-m optimizations | planned |
| 18 | WP-34 | Python source functions: GIL contention at 16 t (36x) | planned (owner places it) |
| 19 | WP-39 | Disk tier: append-only log write path + parallel load | planned (owner places it) |
| 20 | WP-41 | Adaptive caching: disk rule weighted by active threads | planned (late) |
| 21 | WP-42 | Adaptive caching: "keep" hint / non-myopic decisions | planned (late) |
| 22 | WP-78 | Caching: `CachedSourceFunc` overhead at 1 µs × 16 t (WP-76: cached interplin/sg d8 efficiency 0.20) | planned (owner places it; master: size it soon, cached cases now count) |
| 23 | WP-44 | DSG row loop light: single-pass row weights + inline `compRowWeights` (RP-6: 0.2–0.3 s at b0 d7/d10 ≈ 1.5–2 % of quad/dsg; S) | planned (owner places it; master: ≈ rank 5) |
| 24 | WP-07 | Generic barycentric weights cache (≈ 0.003 s judged; may merge into WP-09) | ready (low) |
| 25 | WP-86 | SG interpolations: per-call overhead of tiny grids (nTG = 1 on the calling thread, thread-local per-thread scratch; WP-48 follow-ups; tiny grids untracked) | planned (low) |
| last | WP-43 | Final clean-up: phase references out of code docs + phase leftovers | ready (always last) |

## Next action

**WP-77** (largest-first TG order; SG interplin schedule; TG split optional) is next — **prompt on
hold** (owner 11-24: pause before issuing it). Since WP-48 every operator combines TG-indexed slots in
order → WP-77 must stay bitwise neutral at 1 and 16 t; SG interplin's static schedule keeps each
thread's TG rows in its L2 across calls (≈ 2x on interplin/sg d2) → keep static there unless the A/B
says otherwise. Then **WP-35** (flaky tests). Owner commits "Accept WP-48" (code, summary,
`results/WP-48/`, STATUS / PLAN / HISTORY / LESSONS) before WP-77 starts. Adaptive WPs 63–72 wait for
the owner (D1–D9 + placement); WP-84 / WP-80 / WP-81 wait for the owner's placement. Hygiene findings →
**Code-hygiene backlog** (end of PLAN.md) → WP-54 → WP-55 → clean-up WPs. Numerics findings → **Numerics backlog**
(end of PLAN.md; owner 11-24: later numerics-optimization phase).

## Open decisions for the owner

- **WP-62 D1–D9** (plan `results/WP-62/plan.md` §5; recommendations in PLAN.md WP-62) and the
  placement of WP-63 … 72 — owner decides later (11-17).
- WP-09 (after its summary): adopt the proposed interface or not.
- Placement: WP-80 (grid evaluation), WP-81 (visualization), WP-84 (equi / CC rules below an offset ≥ 2;
  correctness, relevant for adaptive offsets). Optional: protected `quad/sg/cheb1-*` reference case (none yet).
- CC weights with `-ffp-contract=off` (bitwise build-independent weights; ≤ 6.7e-16/m): **not now** (11-15).
- Placement of WP-54 / WP-55 (hygiene; may move up).
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
- Case realism (WP-48): tiny SG interpolation grids (nTG = 1, e.g. d1; µs calls) are not in the suite;
  their overhead is WP-86 (low). Owner: add a case / move WP-86 up, or leave it.
- Decided 11-24: `cpp_concepts.hpp` keeps its four concepts in one header; redundant SWIG `%extend
  VectorSourceFunc::evaluate` (warning 509) → backlog L.

## Re-profiling

Last: RP-6 (2026-11-18; rp-6 = 37a84e1af; accepted). Accepted optimizations since: **0**. WP-79
(feature, 1.002x), WP-83 (bug fix + feature, 0.996x = noise), WP-61 (functional, 0.998x) and WP-48 (functional, 1.000x;
largest case interplin/sg d2 1.117x, no group ≥ 1.3x, no hot-spot shift) do not count. Rule: features /
refactorings / measurement WPs count only if a group changes ≥ 1.3x or a hot-spot ranking shifts. The `interp/dsg`, `interplin/dsg` and `expensive/`
cases have no baseline values (reported absolutely; `expensive/` history in
`results/WP-76/history/`, rp-6 values in `results/RP-6/e2e-expensive-rp-5-vs-rp-6.md`).

## Log (newest first; older entries in HISTORY.md)

- 2026-11-24 Owner: new **Numerics backlog** (PLAN.md end) for a later numerics-optimization phase;
  item A: DSG interpolations combine with plain ordered sums (not compensated; WP-48 scope, unmeasured).
- 2026-11-24 Owner: "Do not retry" → **"Dropped earlier"** (context, not prohibitions; sessions read the
  summary first; lines carry their context); prompt note "Case realism" → general **feedback** note.
  Skill `perf-master` (template, steps 2 / 5), LESSONS.md, PLAN.md header, README.md updated.
- 2026-11-24 WP-48 processed: **accepted** by the owner (commit pending). All operators bitwise
  thread-count independent (0 of 146 reference cases vary at 1/2/3/16 t, before 53); judged 1.000x,
  `expensive/` 1.000x, interplin/sg d2 L16 1.117x; memory 1.003 (0 flags); refcheck = known WARN;
  `grid_storage_pinning` DSG hashes re-pinned. Small-call gate refuted, thread-local TG buffer dropped
  (2 "Dropped earlier" lines); measurement note on thread-count independence updated (+ micro at 16 t).
  Ranks −1 (WP-77 → 1, WP-35 → 2); WP-77 entry gets WP-48's interplin locality finding; new WP-86
  (tiny-grid overhead, rank 25, low). No RP due. WP-77 prompt **on hold** (owner: pause).
