# WP-89 — Dimension-adaptive CT: architecture with the owner, then implementation

Status: PROPOSED · Date: 2026-11-26 · Base commit: 765738082
Recommendation: owner decision needed — accept the feature (architecture approved, milestones M1–M5 implemented and tested, judged suite neutral); choose the default schedule from the numbers below.
Machine: `perf.py calibrate` ratios e2e 1.004 / 1.000, micro 1.000 / 1.002 (measurement time; 1.000 / 0.999 / 0.997 / 0.998 at session start)

## Change
- Architecture settled with the owner in the session (`results/WP-89/architecture.md`, decisions D-A … D-J with the owner's answers, §10 implementation notes): `DimensionAdaptiveRefinement` grows a `SparseGrid` **in place** (Gerstner–Griebel, θ-batches), guided by any per-tensor-grid operator (`RefinementTarget`; built-in quadrature, global / linear interpolation at points, scalar + vector), indicators gain / cost / profit, criteria tolerance (abs / rel), grid-point budget (never exceeded), index-set limit, time, max level; `refine()` / `step()` / `setOptions()`; the object keeps its state (no TG evaluated twice; restart on a foreign grid change via a storage version).
- Engine without barriers (one OpenMP region per call, management on whichever worker holds the lock): `Synchronized` (bitwise thread-count independent; optional `speculationWindow` = evaluation ahead) and `Asynchronous` (timing-dependent). Exceptions of the source function / management are captured and rethrown after the grid is handed back.
- Existing code: `SparseGridSetup` (new base of `SGGenInstr`; grids store a copy, `getGenInstr()` → `getSetup()`; Python updated: `getSetup` returns a copy, no `_genInstr` keep-alive); `QuadWeightTable` from (node table, setup), rules from the growth function; per-TG quadrature kernels take the weight table; `node_table_generation::{genEmptyNodeTable, genUnitRow, genDomainRow}`; `SparseGridBase` storage version + friend accessor.
- Files: new `grids/sparse_grid_setup.*`, `refinement/` (8 headers / 4 .cpp + `targets/`), `miscellaneous/refinement/` (index set, state, index state, node storage, engine, storage access), `tools/refinement/` (index-set, decision, hand-over tools); 48 tracked files changed (+650 / −665 lines, mostly the rename); new code 3445 lines, tests 995 lines; `benchmarks/perf/perf_adaptive.cpp` (new), `perf_micro.cpp` (+`micro/adaptive/*`, `QuadWeightTable` / kernel calls adapted); docs CLAUDE.md §2 / §6, module-reference, python-bindings.md.

## Gain vs. cost
- Gain: feature. Synchronized refinement (deterministic) at 16 threads: d10 / d50 with a 1 µs integrand 7.2x / 7.0x over 1 thread (budget 3e5, rel. error 5.8e-9 / 2.5e-7), d100 cheap 2.3x, low d with 100 µs integrands only 1.8x / 2.3x (idle 86–89 %: few large TGs). Asynchronous: up to 6.6x faster than synchronized at d3, 1.66x at d10, but much worse grids per budget (d10 rel. error 2.0e-3 vs. 5.8e-9; d5 1.2e-3 vs. 1.7e-8). Evaluation ahead (window 32): identical grids, +3 % at d ≥ 10, +30–46 % at low d, but up to +88 % evaluations there.
- Judged suite: neutral — suite sum 35.915 → 35.918 s (1.000x), `expensive/` 1.000x, cached 0.998x; 2 SLOWER: `quad/sg/equi-exp/b1/d20/L5` operator 0.983 (+3.4 ms, cheap `exp` case; the same kernel in `micro/quad/process_all_tgs/equi/b1/d20/L4` is 1.000 → code placement band of RP-4) and `quad/sg/cc-uq/b0/d2/L20` operator 0.951 (CI [0.912, 0.991]; `total` within noise 0.963 [0.917, 1.012]; CC weight micro cases all 1.00; bimodal case, RSS spread 7–10 %).
- Memory: geomean B/A 1.005, 0 `MEM+` / `MEM-`; largest `quad/dsg/equi-exp/b0/d7/L10` 28.4 → 29.4 MiB (within its spread); process floor 10.3 → 10.5 MiB (the larger library). The refinement keeps k doubles + a few words per multi-index; no function values.
- API / functionality: new public API (agreed: architecture approved 2026-11-26); `getGenInstr()` removed in favour of `getSetup()` (owner: decouple the grid from its instruction); grids no longer need the instruction / its `LvlMIVec` after construction; refinement not yet in Python.
- Maintainability / readability: +3.4 k lines of new, separate code; the judged code changes are mechanical (rename, kernel signature, row helpers).

## Tests
- Pinning tests (passed on the unchanged code, debug `--repeat 3`): the existing suite (grid construction / storage, DSG, quadrature / interpolation kernels, thread-count independence, Python examples).
- New / extended: `tests/miscellaneous/refinement/refinement_index_set_test.cpp` (6), `tests/tools/refinement/refinement_index_set_tools_test.cpp` (5: coefficients == `computeCTCoeffs` on random admissible sequences d = 1 … 50, colex order == closure order), `tests/refinement/dimension_adaptive_refinement_test.cpp` (14: refined grid == `MIVecSGGenInstr` grid of its set bitwise for 6 configurations incl. d = 1, offsets 0 / 1 / 2, domains, equi / CC / cheb1, linear / doubling; estimates == operators bitwise; thread counts 1 / 2 / 3 / 16 (with and without evaluation ahead); budget / limits / tolerances / time; reuse and restart; steps; empty and 1D grids; exceptions; async; anisotropy; accuracy d5 error 1.1e-11); rewritten `Stores_Setup_Of_*` in the SG / DSG construction tests.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ · TSan (`results/WP-89/tsan/`): 445 reports, all fork / join pairs of the uninstrumented libgomp, 0 between two accesses of one parallel region · Python (scratch build): 12 examples + `python_checks.py` ✓.

## Reference check
- `perf.py refcheck` (1 and 16 threads): WARN — only the known `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34, identical in `WP-89-before`, since WP-06).
- Bitwise (`results/WP-89/bits/`): 137 / 146 cases identical; 9 CC quadratures (SG and DSG) differ by 1–2 ulp at both thread counts: the CC weights' last bits change because Eigen's header-only kiss FFT is compiled differently under LTO (inlining / FMA contraction; CC rule instructions identical) — see `bits/README.md`.

## Benchmarks (A = WP-89-before, B = WP-89-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `results/WP-89/micro.md`, the classic cases without cache / expensive / vector): 163 within noise, 9 faster, 15 SLOWER, all within ±1–5 % (code placement; untouched code such as `dsg/sub_tries` −4.6 %, `interplin_dsg/build` −2 %); sum of medians 16.823 → 16.811 s (1.001x). Attributable: `crosscall/quad_sg/build/equi/b1/d20/L4` 5.61 → 5.96 µs (0.941; the weight table takes each row's rule via the growth function: +0.35 µs per SG quadrature call).

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| micro/quad/operator/cc/b1/d10/L5 | call | 44.02 ms | 44.18 ms | 0.996 | [0.992, 1.000] |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 11.84 ms | 11.70 ms | 1.008 | [0.999, 1.018] |
| micro/gen/sg/cc/b0/d2/L20 | call | 136.13 ms | 137.49 ms | 0.991 | [0.988, 0.995] |
| micro/quadrule/cc_weights/n1048577 | call | 67.75 ms | 67.73 ms | 0.994 | [0.981, 1.006] |

L2/L3 e2e (16 threads, full suite incl. `expensive/`; `results/WP-89/e2e.md`): see Gain; all other cases within noise.

Sum of medians (total): suite A = 35.915 s, B = 35.918 s (1.000x) · `expensive/` 15.681 → 15.682 s · cached 1.002 → 1.004 s · foreign load 0.2 % per round.

Peak memory: geomean B/A = 1.005, 0 flags · process floor A = 10.3 MiB, B = 10.5 MiB.

Feature benchmarks (`perf_adaptive`, in-tree normal build = `WP-89-after`, 16 threads, 5 samples; `results/WP-89/adaptive/table.md`):

| case (CC, offset 1, Gaussian a_k = 4 k⁻², budget) | sync 16 t | vs 1 t | sync-w32 vs sync | async vs sync | rel. error sync / async |
|---|---|---|---|---|---|
| d10 c1us 3e5 | 41.8 ms | 7.23x | 1.03x (+14 % evaluations) | 1.66x | 5.8e-9 / 2.0e-3 |
| d50 c1us 3e5 | 51.1 ms | 6.97x | 1.03x (+3 %) | 1.24x | 2.5e-7 / 7.1e-6 |
| d100 c0 1e6 | 164 ms | 2.28x | 1.00x (+3 %) | 1.04x | 4.2e-8 / 2.6e-7 |
| d3 c100us 2e4 | 1.089 s | 1.82x | 1.46x (+88 %) | 6.62x | 1.4e-15 / 5.0e-11 |
| d5 c100us 2e4 | 0.836 s | 2.27x | 1.30x (+70 %) | 4.36x | 1.7e-8 / 1.2e-3 |

L1 feature cases (1 thread, `results/WP-89/micro_adaptive.txt`): a whole refinement costs 57 ns per evaluated point at d10 vs. 36 ns for the SG quadrature of the refined grid (≈ 21 ns bookkeeping; WP-62 prototype 10.5 ns) and 585 vs. 268 ns at d100 (≈ 317 ns; prototype 47.5 ns). `step()` on a d100 grid of 1e5 points 23.9 ms (incl. one decision's evaluations) vs. rebuilding the grid from its index set 91.7 ms.

## Attempts not kept
- none (variants of the schedule are options, measured above).

## Profile shift / follow-ups
- **Owner decision — default schedule**: recommendation `Synchronized`, `speculationWindow` 0 (deterministic, best grids per budget); `Asynchronous` stays an option documented as timing-dependent and weaker per budget; evaluation ahead only for low d and expensive functions, better bounded by grid points than by a TG count (follow-up).
- **Re-split of WP-63 … 72**: WP-64 (core structures), WP-65 (driver, now operator-generic), WP-66 (non-nested + vector), WP-67 (interpolation via indicator points), WP-72 (resume: the refinement object) → **done** here; WP-68 (Python bindings + examples) → follow-up (docs done); WP-69 (predicted / lazy indicators), WP-70 (dimension buffering), WP-63 (generic nestedness + standard CC growth 1, 3, 5, 9, 5–48x lower error at d = 100 in WP-62) → follow-ups; WP-71 (operators on stored values) and the value store → dropped (owner: no function values in the machinery).
- New follow-ups: (1) **bookkeeping at high d** (d100 ≈ 317 ns per point; profile: colex sort with O(d) compares at the hand-over 8.5 %, page faults of the growing state ≈ 10 %, index lookups ≈ 7 %, O(d) cost per candidate via the growth function 2 %, O(d) kernel setup per tiny TG): sort keys / incremental order, reserved state, incremental costs, sparse levels; (2) **parallel kernels inside one TG** (owner's idea 2; low d idles 86–89 %); (3) **async decision rule** (decides after almost every result; e.g. wait for a share of the candidates); (4) direct `DeltasSparseGrid` refinement; (5) nested-aware cost model; (6) multi-point interpolation kernel for the interpolation targets (P points evaluate a TG P times without cache); (7) more indicators / norms, GG work safeguard.
- **CC weights' last bits depend on unrelated code** (Eigen kiss FFT under LTO, `bits/README.md`) → numerics backlog.
- **TG ordering** (for the ordering WP): a refined grid stores its TGs in colexicographic order (the order `MIVecSGGenInstr` gives the same set), recomputed once per call by a sort; processing order recomputed per call (no incremental merge needed). Wishes: storage order as a deterministic function of the index set (a sort key), cheap to recompute; the same order for complete and MIVec grids of the same set (today they differ); the processing order derived at construction / hand-over; a DSG's full-set order by the same key. The colex sort is visible at d100 (8.5 % of a refinement) → a cheaper key helps both.
- Tooling: `pkill -f` patterns match the agent's own shell (careful in sessions); TSan with OpenMP needs the fork/join classification (`results/WP-89/tsan/classify.py`).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply --keep combigrid/perf/PLAN.md --keep combigrid/perf/STATUS.md` (keeps `summaries/` and `results/`; the master's PLAN / STATUS edits predate the session).
