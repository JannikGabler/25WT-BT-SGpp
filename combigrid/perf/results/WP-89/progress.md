# WP-89 progress log — dimension-adaptive CT (architecture with the owner, then implementation)

Session start: 2026-11-26 (HEAD 765738082, branch `optimizations`).

## 1. Current state and next steps

- **Session complete (2026-11-26)**: M1–M5 implemented and verified; summary
  `summaries/WP-89-dimension-adaptive-ct.md` (status PROPOSED).
- Verification: debug `--repeat 3` ✓, ASan ✓, normal `--repeat 3` ✓, refcheck WARN (known case only,
  identical before), bitwise 137 / 146 (9 CC cases 1–2 ulp: Eigen FFT under LTO, `bits/README.md`),
  TSan (`tsan/summary.txt`: only fork / join pairs), Python scratch build ✓.
- Measurements: `micro.md` (1.001x), `e2e.md` (suite 1.000x, memory 1.005), `adaptive/table.md`,
  `micro_adaptive.txt`; calibrate 1.004 / 1.000 / 1.000 / 1.002.
- **Open for the owner**: default schedule (recommendation: `Synchronized`, window 0); acceptance.
- **Follow-ups** (summary): bookkeeping at high d, parallel kernels inside a TG, async decision rule,
  DSG refinement, Python bindings + examples, predicted indicators, buffering, nested-aware cost,
  multi-point interpolation kernel, CC weight bit stability.

## 2. Environment / tree at start

- `git status --short`: ` M combigrid/perf/PLAN.md`, ` M combigrid/perf/STATUS.md` (the master's
  uncommitted WP-89 entries; not touched), the owner's untracked `compile*.sh`. No source change.
- `perf.py verify` ✓ (40 protected files), `envcheck` ✓ (busy 0.4 %, boost 0, AC). `calibrate`:
  deferred to the implementation phase (before the "before" snapshot).

## 3. Owner's vision (Phase 2, 2026-11-26, verbatim in substance)

- Not single-shot: the user can always refine a sparse grid further, possibly with changed
  refinement arguments.
- Integrated into the code base, not a separate thing: the result is a sparse grid usable like any
  non-adaptively created one; preferably a `SparseGrid`, as from an `SGGenInstr` today.
- Supports any operator (current and future ones); node families and growth functions supported
  the same way as now.
- Draft: the user creates an initial sparse grid any way (e.g. `CompleteSGGenInstr`) → hands it
  with refinement arguments (termination criterion, error indicator, maximum time, …) to some
  dimension-adaptive machinery (e.g. an operation) → the machinery refines the given grid → the
  grid is used as usual (operators) → optionally another round with adjusted arguments.
- State: temporary state lives in the machinery during the refinement; persistent information
  for later refinements is open — possibly stored in the grid itself (e.g. in the cross-call
  operator data cache or something like it).
- Driving: a whole iterative refinement process in one call, and step by step.
- TG ordering: not settled; a future WP chooses the best internal order for all use cases (plus
  external orders if needed). This WP may use an existing order (index order, the external
  largest-first order) or add a new external one, and notes its wishes for that WP in the summary.
- Scope of this WP's implementation: the core dimension-adaptive refinement incl. parallelism and
  at least three "error" indicators: gain, cost, profit = gain / cost.
- Planned later (not required here): more indicators, advanced concepts such as dimension
  buffering (potentially).
- Note: each sparse grid holds a private copy of the `SGGenInstr` that built it; planned rework:
  no copy, only an internal struct with what the grid needs after creation.

### Follow-up answers (Phase 2)
1. Refinement changes the given `SparseGrid` object itself (preferred).
2. "Any operator" means both: refined grids work with every operator, and any operator can guide
   the refinement — preferably out of the box (generic hook), as far as possible.
3. DSGs must be obtainable from a refinement somehow; how is open; may be a later WP.
4. Operator switch: re-evaluating everything is fine. Indicator switch: re-computation is fine, but
   the expensive part (per-TG operator estimates) should be reusable if possible.
5. The grid need not be usable during refinement.
6. gain = size of Δ_ℓ of the target operator, cost = number of function evaluations, profit =
   gain / cost (confirmed).
7. This WP may decouple the sparse grid from its `SGGenInstr`.
8. Python bindings and examples: follow-up.

## 3b. Decisions (Phase 3)

| # | Decision | Owner's answer (2026-11-26) |
|---|---|---|
| D-A | Operator hook | **(a) per-TG kernel with f inside** (integrates with the existing kernels, less restrictive). Owner's ideas against idle threads: (1) add TGs in batches, (2) kernels with a flag that toggles serial / parallel execution inside a TG. |
| D-B | State between rounds | **(a) a refinement object kept by the user** (active set, per-TG estimates, options); grid holds no extra state (full set = closure of its levels); new object restarts with A = maximal indices; version counter detects foreign changes. |
| D-C | Function-value reuse | **(a) only through the user's `CachedSourceFunc`**; the machinery holds no large memory (no value store). Cost indicator: **all grid points of the TG** (no nested / non-nested distinction for now); nested-aware work model → follow-up. |
| D-D | Parallelism | Owner's idea: an orchestrator without barriers (threads pick up TGs; finished threads do the management). Options: (A) synchronous batches, (B) asynchronous with timing-dependent decisions, (C) asynchronous evaluation + deterministic decisions + speculative evaluation ahead. **Owner: determinism is nice to have, not a hard requirement; implement / test both (B) and (C), decide together with numbers; (C) preferred if it costs nothing significant.** Batches (θ, fixed cap) as the decision rule; kernel-level parallel flag (owner's idea 2) → follow-up with low-d evidence. |
| D-D2 | "Look-ahead" | Owner's meaning: **indicators extrapolated to unevaluated TGs** (WP-62's predicted / "lazy" selection: 0.19–0.84x of GG's evaluations, blind to hidden interactions) — not necessary now, a potential optimization → follow-up; the architecture keeps room for it (predicted candidates, priority from a prediction). |
| D-E | Decoupling grid / `SGGenInstr` | **(a)** new class **`SparseGridSetup`** (domain, node families, growth functions, boundary offset; getters / setters); `SGGenInstr` derives from it; grids store a copy; `getGenInstr()` → `getSetup()`; existing Python bindings kept working. |
| D-F | Grid after refine / step, TG storage order | Owner: grid must be fully usable only after the refinement; attributes need not be valid during it; **choose the best update strategy per attribute** (not necessarily rebuild everything at the end). `refine()` must not be built around `step()`; `step()` exists for examples / demonstrations (share code where it helps). Storage order: canonical (proposal in architecture.md). |
| D-H | Indicators, stopping | gain = max-norm of Δ, cost = TG points, profit = gain / cost; criteria as proposed. Owner: criteria are checked; once one holds, the asynchronous core trims the tasks to those needed to reach a state that allows the refinement to end. |
| D-I | Targets | public abstract target + quadrature / global interp / linear interp (scalar + vector) — OK. Owner: the end-of-call hook once per refinement call, not per TG. |
| D-J | Grid types | `SparseGrid` now, direct DSG refinement next / follow-up — OK. |
| D-G | DSG | long run: **refine DSGs directly** (some operators are faster on them); a `DeltasSparseGrid(const SparseGrid&)` constructor only if really beneficial — not the intended way. |

## 4. Study notes: discrepancies WP-62 plan vs. current code (verified in the code)

1. **1D quadrature rules**: `NodeGenFunc::getQuadRule(n)` is gone (commented out); only the pure
   virtual `getQuadRule(n, includeBoundary)` exists (WP-83). The WP-62 prototype calls
   `getQuadRule(n)` (would not compile now). cheb1 has `FejerFirstQuadRule` (WP-83; plan §3.4 says
   "no rule"). Equi / CC ignore `includeBoundary` ("TODO" in both .cpp): levels below an offset ≥ 2
   use non-interpolatory weights (WP-84 open).
2. **Nestedness / index maps**: `isNested`, `getNodeIdxMapBetweenLvls`,
   `getNodeIdxMapFromDeltaTGLvlToRegularLvl`, `genDeltaNodesWithoutBoundary` still compare the
   growth-function pointer with `doublingLvl2GPCntFunction` / `linearLvl2GPCntFunction` (equi, CC;
   cheb1 analogous) → custom growth throws (unchanged since WP-62).
3. **Growth semantics**: `Lvl2GPCntFunc` returns the number of *inner* nodes; boundary nodes are
   added for `lvl >= boundaryLevelOffset`. The standard CC growth 1, 3, 5, 9, … is
   `f(0) = 1, f(ℓ) = 2^ℓ − 1` **only with offset 1**; with offset 0 it gives 3, 3, 5, 9 (levels 0
   and 1 equal), with offset 2 it gives 1, 1, 5, 9. The Doxygen of `doublingLvl2GPCntFunction`
   is wrong (says 2^{ℓ−1} + 1; code 2^{ℓ+1} − 1 inner).
4. **Work gate**: WP-48's small-call gate was refuted (static schedule's L2 locality); plan §3.4
   counted on sharing it. The driver needs its own work-dependent parallel regions (WP-62: 16 t
   slower than 1 t for cheap integrands, d10 0.062 → 0.144 s).
5. **Ordered compensated combination** (WP-61 / 48): every operator writes per-(delta-)TG values to
   TG-indexed slots and combines them serially with `tools::numerics::compensatedDotProduct(s)` /
   `CompensatedSum` → bitwise thread-count independent. The adaptive estimate / indicators should
   follow the same rule (sum in index order, compensated).
6. **TG processing order** (WP-77): per grid, `computeMetaValues` → `getTGProcessingOrder()`
   (O(nTG) counting sort). A grid built anew from a handed-over instruction recomputes it.
7. **Cross-call operator data cache** (WP-73 / 82): per grid object; module reference already says
   "adaptive refinement builds a new grid per step, so its caches start empty; nothing to
   invalidate" — valid only if grids are not grown in place.
8. **Textbook operators** (WP-79): SG only, per-TG `(c_ℓ, Op_ℓ f)` formed but not returned;
   usable as test oracles for hand-over results.
9. **MIVecSGGenInstr**: stores a reference; closure in colexicographic order, `computeCTCoeffs`,
   reduced variant drops zero coefficients (colex order kept), full variant partitions the
   zero-coefficient MIs to the **front** (`partitionForExtensiveSG`) → the DSG's TG order is
   "zeros first, then colex". The TG storage order of a handed-over grid is decided by the
   instruction.
10. **SG quadrature** computes per-TG estimates internally (`tools::quadrature::estimateTensorGrids`,
    TG-indexed) and returns only the combination.
11. **Tests**: golden values with tolerances, never stored bit patterns (WP-35); portable
    (CLAUDE.md §4, owner 2026-11-26).
