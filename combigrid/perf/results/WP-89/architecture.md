# WP-89 — Dimension-adaptive refinement: architecture

Status: **approved by the owner (2026-11-26)** · Date: 2026-11-26 · Base: HEAD 765738082.
Inputs: the owner's vision and answers ([progress.md](progress.md) §3, §3b), WP-62's evidence
([plan](../WP-62/plan.md), [literature](../WP-62/literature.md)), the current code. Marks:
**[owner]** = decided by the owner in this session, **[proposal]** = my choice, part of this
approval.

## 1. Summary

- The user builds a `SparseGrid` any way (e.g. `CompleteSGGenInstr`) and hands it to a
  `DimensionAdaptiveRefinement` object together with a **refinement target** (an operator with its
  arguments, incl. the source function) and `RefinementOptions`. `refine()` grows **the same grid
  object** until a stopping criterion holds; afterwards the grid is an ordinary `SparseGrid`
  (every operator works on it). The object can be kept: further `refine()` calls (also with changed
  options, e.g. another indicator) continue where the last one stopped, reusing all per-TG
  estimates. `step()` performs one decision for demonstrations. **[owner]**
- Algorithm: Gerstner–Griebel (old / active sets; the result contains every evaluated index),
  increments Δ_ℓ by inclusion–exclusion over per-TG results of the target, indicators **gain**
  (‖Δ_ℓ‖), **cost** (grid points of the TG), **profit** (gain / cost), batches of indices with
  priority ≥ θ · best. **[owner]**
- Parallelism: an engine without barriers — workers take TG evaluation tasks from a queue; the
  worker that completes a task performs the management. Two decision modes, both implemented and
  measured, the default decided with the owner on numbers **[owner]**: `Synchronized`
  (deterministic decisions, idle workers evaluate ahead speculatively; index set bitwise
  thread-count independent) and `Asynchronous` (decisions on whatever has finished;
  timing-dependent).
- The grid no longer stores a copy of its generation instruction: new class `SparseGridSetup`
  (domain, node families, growth functions, boundary offset); `SGGenInstr` derives from it;
  grids store a copy; `getGenInstr()` → `getSetup()`. **[owner]**
- No function values are stored by the refinement; reuse across TGs only through a
  `CachedSourceFunc` passed by the user. **[owner]**

## 2. Decisions

| # | Topic | Decision |
|---|---|---|
| D-A | operator hook | per-TG kernel with f inside (`RefinementTarget`), the existing kernels wrapped **[owner]** |
| D-B | state between calls | in the `DimensionAdaptiveRefinement` object (user-owned); the grid holds no extra state; a new object on a refined grid restarts (active = maximal indices, all TGs re-estimated); a version counter in the grid detects changes behind the object's back → resync **[owner]** |
| D-C | value reuse, cost | no value store (only `CachedSourceFunc`); cost = all grid points of the TG **[owner]** |
| D-D | parallelism | barrier-free engine, modes `Synchronized` / `Asynchronous`, both measured, default decided with the owner; batches θ = 0.5, cap 64 (fixed, thread-count independent) **[owner]**; parallel kernels inside a TG → follow-up **[owner]** |
| D-D2 | look-ahead (predicted indicators of unevaluated TGs, WP-62 "lazy") | follow-up; the design keeps room for it (a candidate state "predicted", priority from a prediction) **[owner]** |
| D-E | grid ↔ instruction | `SparseGridSetup`, `SGGenInstr : public SparseGridSetup`, `getSetup()` **[owner]** |
| D-F | grid attributes, order | grid fully usable only after `refine()` / `step()`; per attribute the cheapest correct update (§5.4) **[owner]**; storage order **canonical** (the order `MIVecSGGenInstr` gives the same index set) **[proposal]** |
| D-G / D-J | DSG | refine `DeltasSparseGrid` directly in the long run; this session: `SparseGrid`; the engine works on `SparseGridBase` internals so that only the hand-over differs **[owner]** |
| D-H | indicators, stopping | gain = max-norm of Δ_ℓ, cost = TG points, profit = gain / cost; criteria checked continuously; once one holds, unstarted tasks are dropped and started ones finish (§5.3) **[owner]** |
| D-I | targets | public abstract `RefinementTarget`; built-in quadrature, global interpolation at P points, linear interpolation at P points (scalar + vector); end-of-call hook once per refinement call **[owner]** |

## 3. Public API (C++)

All in `namespace sgpp::combigrid`, in the umbrella header. **[proposal]** Python bindings are a
follow-up **[owner]**: the new headers are not `%include`d in `combigrid.i` until then (the `.i`
lists headers explicitly), so the umbrella header temporarily holds public API without a binding.

```cpp
// grids/sparse_grid_setup.hpp — what a grid needs after its creation
class SparseGridSetup {
 public:
  explicit SparseGridSetup(size_t nDim);
  // today's SGGenInstr getters / setters / helpers, moved unchanged:
  // nDim, get/setBoundaryLevelOffset, get/setDomain(ForDim), get/setNodeGenFunc(s)(ForDim),
  // get/setLvl2GPCntFunc(s)(ForDim), getVolumeOfDomain, getUniqueNodeGenFuncs,
  // areNodesOfAllDimsNested, resize
};
class SGGenInstr : public SparseGridSetup { /* gen*MIVec*, clone — unchanged */ };
// SparseGridBase: const SparseGridSetup& getSetup() const noexcept;  (replaces getGenInstr())

// refinement/refinement_indicator.hpp
enum class RefinementIndicator { Gain, Cost, Profit };
// refinement/refinement_schedule.hpp
enum class RefinementSchedule { Synchronized, Asynchronous };
// refinement/refinement_stop_reason.hpp
enum class RefinementStopReason { None, Tolerance, RelativeTolerance, GridPointBudget,
                                  TensorGridLimit, TimeLimit, NoCandidates, StepDone };

// refinement/refinement_options.hpp — plain data; 0 / max / inf = criterion off
struct RefinementOptions {
  RefinementIndicator indicator = RefinementIndicator::Profit;
  double absoluteTolerance = 0.0;            // stop when the error estimate eta <= it
  double relativeTolerance = 0.0;            // stop when eta <= it * max-norm of the estimate
  size_t maxGridPoints = SIZE_MAX;           // evaluation budget of one call (sum of TG points), never overshot
  size_t maxTensorGrids = SIZE_MAX;          // size of the index set (incl. zero-coefficient TGs)
  double maxSeconds = infinity;              // wall time of one call
  LvlType maxLevel = 30;                     // per dimension; larger candidates are not admissible
  double batchThreshold = 0.5;               // theta: accept every active index with priority >= theta * best
  size_t maxBatchSize = 64;                  // fixed (not tied to the thread count)
  RefinementSchedule schedule = RefinementSchedule::Synchronized;  // default after the measurement
  size_t speculationWindow = 0;              // Synchronized: TGs evaluated ahead (0 = off; default after the measurement)
};

// refinement/refinement_statistics.hpp
struct RefinementStatistics {
  size_t nDecisions, nEvaluatedTensorGrids, nEvaluatedGridPoints;   // of the call
  size_t nSpeculativeTensorGrids, nUnusedSpeculativeTensorGrids;
  size_t nOld, nActive, nTensorGrids;                               // state after the call
  double errorEstimate;                                             // eta = sum of the active gains
  RefinementStopReason stopReason;
  double seconds, workerIdleSeconds;                                // idle: sum over the workers
};

// refinement/targets/refinement_target.hpp — "an operator applied to one tensor grid"
class RefinementTarget {
 public:
  virtual ~RefinementTarget() = default;
  virtual size_t nValues() const = 0;                       // k values per tensor grid
  /// Serial, once per refinement call before any estimate: per-call data (e.g. 1D weights are
  /// then computed lazily per row), scratch for nWorkers workers.
  virtual void beginRefinementCall(const SparseGridSetup& setup, const NodeTable& unitNodeTable,
                                   size_t nWorkers) = 0;
  /// The k values of one tensor grid (Op_l f); called concurrently by distinct workers.
  virtual void estimate(const TensorGridView& domainTG, const TensorGridView& unitTG,
                        size_t worker, std::span<double> out) = 0;
  /// Maps a combination of TG values (estimate, increment) to the target's result; default:
  /// identity (quadrature: times the domain volume).
  virtual void finishCombination(std::span<double> values) const {}
  /// Serial, once at the end of a refinement call (calls the source function's
  /// finishOperatorCall()).
  virtual void endRefinementCall() = 0;
};
// built-in targets (refinement/targets/*), each with a SourceFunc and a VectorSourceFunc
// constructor (k = m resp. P * m), wrapping the existing per-TG kernels:
class QuadratureRefinementTarget;           // (const SourceFunc&) / (const VectorSourceFunc&)
class GlobalInterpolationRefinementTarget;  // (f, const base::DataMatrix& points)  points on the domain
class LinearInterpolationRefinementTarget;  // (f, const base::DataMatrix& points)

// refinement/dimension_adaptive_refinement.hpp
class DimensionAdaptiveRefinement {
 public:
  /// Keeps references to grid and target (both must outlive the object).
  DimensionAdaptiveRefinement(SparseGrid& grid, RefinementTarget& target,
                              const RefinementOptions& options = {});
  RefinementStatistics refine();   // until a criterion holds (at least one limit must be set, else throws)
  RefinementStatistics step();     // one decision: accept a batch, evaluate its candidates
  void setOptions(const RefinementOptions& options);
  const RefinementOptions& getOptions() const noexcept;
  std::span<const double> getEstimate() const noexcept;  // target's CT result on the index set (k values)
  double getErrorEstimate() const noexcept;               // eta
};
/// One-shot convenience (operation style): a temporary refinement object.
RefinementStatistics refine(SparseGrid& grid, RefinementTarget& target,
                            const RefinementOptions& options);
```

Example:

```cpp
CompleteSGGenInstr genInstr(1, d);  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
genInstr.setBoundaryLevelOffset(1);
SparseGrid sg(genInstr);
CachedSourceFunc f(func);                                     // optional: reuse values across TGs
QuadratureRefinementTarget target(f);
RefinementOptions options;  options.maxGridPoints = 100000;  options.absoluteTolerance = 1e-8;
DimensionAdaptiveRefinement refinement(sg, target, options);
refinement.refine();
double q = refinement.getEstimate()[0];                       // == quadrature(f, sg) (same bits, one build)
double v = interpolate(f, x, sg);                              // sg is an ordinary grid again
options.indicator = RefinementIndicator::Gain;  refinement.setOptions(options);
refinement.refine();                                           // continues; no TG is re-estimated
```

## 4. Algorithm

Index set I (downwards closed) with states per index: **old** (accepted: forward neighbours
examined), **active** (evaluated, in I, candidate for acceptance), **pending** (admissible, task
queued or running, not yet in I), **speculative** (evaluated ahead, not yet requested; not in I).
After a call the grid is I = old ∪ active (every evaluated index is used, as in Gerstner–Griebel).

- Start (new object, or the grid's version changed): I = closure of the grid's levels (= all TGs
  incl. zero coefficients: maximal indices have coefficient 1), coefficients by
  `computeCTCoeffs`, active = maximal indices, old = the rest, every TG of I is estimated
  (counted in the call's budget). Later calls reuse everything.
- Commit of an evaluated index ℓ: per-TG values Q_ℓ (k doubles) stored;
  Δ_ℓ = Σ_{z ∈ {0,1}^{supp ℓ}} (−1)^{|z|} Q_{ℓ−z} (compensated, fixed z order; every ℓ − z is in I
  because ℓ is admissible); gain = ‖finishCombination(Δ_ℓ)‖_∞, cost = points of TG ℓ, priority
  from the indicator (gain, 1 / cost, gain / cost; ties by canonical index order); coefficients
  c_{ℓ−z} += (−1)^{|z|}; η += gain.
- Decision: the batch = active indices with priority ≥ θ · best (in priority order, at most
  `maxBatchSize`; cut where the new candidates no longer fit the budget); each moves to old
  (η −= gain); every forward neighbour ℓ + e_k with level ≤ `maxLevel` whose backward neighbours
  are all old becomes pending (a task).
- The estimate (k values) = `finishCombination(Σ_ℓ c_ℓ Q_ℓ)`, combined in canonical order with
  `tools::numerics::compensatedDotProducts` — the same computation as the SG operators, so
  `getEstimate()` equals the operator on the refined grid bitwise (within one build) for the
  quadrature target.

## 5. Engine and data structures (internal)

### 5.1 Engine **[owner: barrier-free; proposal: details]**

One OpenMP parallel region per `refine()` / `step()` call, every thread a worker. One mutex guards
the management state and the task queue (tasks are whole TGs: coarse). Worker loop: take a task
(pending first, then speculative), evaluate it outside the lock (`target.estimate`), lock, record
it, perform the management that is due (commits, decisions, stopping checks), unlock. Idle
workers wait on a condition variable. Cheap tasks (high d, cheap f) are taken in chunks sized by
the measured cost per point, so the lock is not taken per microsecond-task; below a work gate the
call runs on one worker (16 threads were slower than 1 for cheap integrands in WP-62).

- `Synchronized`: decision n + 1 happens only once every pending task of decision n is evaluated;
  the decision's results are committed in canonical order → index set, estimate and η are bitwise
  independent of the thread count and the timing (except a `maxSeconds` stop). Idle workers
  evaluate **speculative** tasks (up to `speculationWindow`): forward neighbours of the indices
  that the next decisions will most likely accept (top of the active priority order) whose other
  backward neighbours are old or about to be accepted. A speculative result is used when the
  decision process requests the index (no wait); unused ones stay in the object for later calls.
  Window 0 = plain synchronous batches.
- `Asynchronous`: each result is committed when it arrives; a decision is made whenever fewer than
  2 × workers tasks are queued or running. Maximum utilization, no speculation; the index set
  depends on timing (1 worker: deterministic).
- Exceptions of the source function inside workers are captured (first one), the call stops as
  below, the grid is finalized with the committed results, then the exception is rethrown (no
  `std::terminate`, cf. WP-85).

### 5.2 Data structures (`miscellaneous/refinement/`, namespace `misc`)

| Structure | Content | Notes |
|---|---|---|
| `RefinementIndexSet` | append-only dense level rows (`LvlType` × d, what `TensorGridView` needs), open-addressing hash index with an additive per-(dim, level) hash (neighbour hashes in O(1)), per index: state, coefficient, offset of its values, gain, cost, priority | O(1) neighbour lookups; equality check O(d) only on hash matches |
| per-TG values | k doubles per evaluated index (chunked, stable addresses) | the expensive part the owner wants reused; 8·k bytes per TG |
| task queue | pending (priority order) and speculative tasks | under the management mutex |
| node rows | the grid's unit and domain `NodeTable`s, borrowed for the call in a **fixed-capacity layout** (`maxLevel` + 1 rows per dimension group, empty until generated) | rows never move while workers read them; new rows generated by the management before a task that needs them is published |

### 5.3 Stopping **[owner]**

Criteria are checked by the management after every commit / decision (time also before a task is
started). Once one holds: no new decisions, unstarted pending and speculative tasks are dropped,
started tasks finish; finished pending tasks are committed (they are admissible, so I stays
downwards closed). The budget is reserved when a task is started, so it is never overshot.
`refine()` throws `std::invalid_argument` if no limit is set (tolerances, budget, TG limit, time
all off). `step()` stops after one decision.

### 5.4 Hand-over to the grid (per attribute) **[owner: best strategy per attribute]**

| Attribute | Strategy |
|---|---|
| node tables (unit, domain) | moved out of the grid at the start (O(rows)), grown in the fixed-capacity layout, moved back compact at the end; existing rows are never regenerated |
| levels, coefficients | coefficients maintained incrementally; at the end the nonzero-coefficient indices are written in canonical order (sort, O(nTG log nTG · d)) |
| max TG size / sum, processing order (WP-77) | recomputed at the end (`computeMetaValues`, O(nTG)) |
| cross-call operator data cache | cleared at the start (its data refers to the old storage) |
| version counter (new, internal) | incremented at the end; a refinement object compares it at its next call |
| views / iterators into the grid | invalid after a call (like `std::vector` iterators after a reallocation) |

Access to the protected storage: a friend accessor in `tools::refinement` (no public setter).

## 6. Changes to existing code (judged code marked ⚖)

| Change | Where | Check |
|---|---|---|
| `SparseGridSetup` extracted; `SGGenInstr` derives from it; grids store a `SparseGridSetup` in `SparseGridBase` instead of an `SGGenInstr` clone; `getGenInstr()` → `getSetup()` | `grids/`, `sparse_grid_generation_instructions/`, every operator driver / kernel that read the instruction ⚖, tests, `perf_micro.cpp`, Python `.i` (rename + keep-alive, typemaps of the growth-function setters move to the base) | refcheck, `perf_reference` bitwise diff, `ab e2e` full suite (header layout change also shifts code placement in `perf_e2e`), Python checks (`SG_PYTHON=1`, examples) |
| `QuadWeightTable` / `QuadratureContext` independent of `SparseGrid` (a unit node table + setup; rules from the growth function, so rows not generated yet work) | `miscellaneous/data_contexts/` ⚖ | as above |
| per-TG kernels take `const SparseGridSetup&` instead of `const SGGenInstr&` | `tools/operators/*` ⚖ | as above |
| `SparseGridBase`: version counter, friend accessor for the hand-over | `grids/` ⚖ | as above |
| CLAUDE.md §6 "Not implemented: dimension-adaptive refinement", module reference ("Known limitations", cross-call cache note "nothing to invalidate", key classes) | docs | — |
| Generation instructions, `NodeGenFunc`, growth functions, DSG, operators' public signatures | **unchanged** | — |

## 7. Milestones (re-split of WP-63 … WP-72)

| M | Scope | Tests (first) | Benchmarks | Risk |
|---|---|---|---|---|
| **M1** | `SparseGridSetup` decoupling (§6 rows 1–3), Python bindings kept working | existing grid / operator / Python tests pass unchanged (renamed calls); `getSetup()` equals the instruction's setup; grids independent of the instruction after construction | refcheck + bitwise diff + `ab e2e` full suite (neutral expected) | low; wide but mechanical |
| **M2** | `RefinementIndexSet`, incremental coefficients, increments, admissibility / forward neighbours, canonical order, start from a grid (closure, maximal indices) | coefficients == `computeCTCoeffs` on random admissible sequences (d = 1 … 50); closure / maximal indices; canonical order == `MIVecSGGenInstr`'s order; Δ by inclusion–exclusion on known product functions | `micro/adaptive/{insert,neighbours,coeffs}` (vs. WP-62: rebuild 73.7 ms / step at d = 100) | medium (kernel of everything) |
| **M3** | fixed-capacity node rows, hand-over to the grid (§5.4), `RefinementTarget` + 3 built-in targets (scalar + vector) | target values == the kernels on the same TG; hand-over: refined grid == grid built by `MIVecSGGenInstr` from the same set (levels, coefficients, node tables, operator results bitwise in one build); d = 1, offsets 0 / 1, non-unit domains | `micro/adaptive/handover` | medium (touches grid storage) |
| **M4** | engine `Synchronized` without speculation (= batches), decisions, indicators gain / cost / profit, all stopping criteria, `refine()` / `step()` / free `refine`, statistics | estimate == `quadrature` / `interpolate` / `interpolateLinear` / textbook operators on the refined grid; index set and estimate bitwise equal at 1 / 2 / 3 / 16 threads; budget never overshot; `step()` sequence == `refine()` with θ-batches; later call continues without re-estimation (evaluation count); restart on a foreign grid change; exceptions of f rethrown, grid valid; d = 1; empty / size-1 start grids | `perf_adaptive.cpp` (new, unprotected): Genz-type functions d = 10 / 50 / 100 and `perf::expensive` c1us / c100us (WP-76), evaluations / error / wall time at 1 and 16 threads, idle share | medium |
| **M5** | `Asynchronous` mode, speculation in `Synchronized`; measurement of both → owner decides the default | async: valid grid, estimate consistent, 1 worker == synchronized; speculation does not change the index set (bitwise) | `perf_adaptive`: B vs. C vs. plain batches, 16 threads, idle share, unused speculative TGs | medium–high (concurrency; TSan run) |

Follow-up WPs (not in this session):

| Follow-up | From | Note |
|---|---|---|
| Direct `DeltasSparseGrid` refinement | D-G / D-J | hand-over writes delta rows + full set; engine unchanged |
| Python bindings + examples (C++ and Python) of the refinement | owner 8 (old WP-68) | `%include` the new headers; targets with Python callables run serially (GIL, WP-34) |
| Predicted indicators ("look-ahead", WP-62 lazy) + exploration share | D-D2 (old WP-69) | combines with speculation (prediction picks what to evaluate ahead) |
| Parallel kernels inside a TG (flag) | D-D idea 2 | evidence from `perf_adaptive` at low d first |
| Nested-aware cost model (new points) | D-C | needs generic nestedness |
| Generic nestedness + standard CC growth 1, 3, 5, 9 | old WP-63 | independent; 5–48x lower error at d = 100 (WP-62); growth function counts inner nodes, so standard CC growth only means 1, 3, 5, 9 with offset 1 |
| Multi-point interpolation kernel for the interpolation targets (evaluate a TG once for P points) | D-I | WP-09 shape; until then a `CachedSourceFunc` avoids the P-fold evaluations |
| More indicators / norms (per-output scaling, L2, user norm), GG work safeguard | owner, WP-62 D2 | the indicator is computed once per committed index: an enum switch or a callback costs nothing |
| Dimension buffering | owner ("potentially"), old WP-70 | — |
| Dropped: index value store (old WP-64 part, WP-71 "operators on stored values") | D-C | the refinement holds no function values |
| Absorbed: non-nested + vector values (old WP-66), interpolation target (old WP-67 except the nested surplus indicator, which needs stored values), resume (old WP-72: the refinement object + in-place grid resume by design) | — | — |

**Implementation scope of this session [proposal]**: M1 → M2 → M3 → M4, then M5 with the
measurement for your decision on the default schedule. `progress.md` is updated after every
milestone; whatever is not finished becomes a follow-up WP in the summary.

## 8. Implications for the tensor-grid order

- A refined grid stores its TGs in **canonical order** (the order `MIVecSGGenInstr` gives the same
  index set), recomputed once per refinement call (sort) — independent of history, schedule,
  timing and thread count; "same index set ⇒ same operator bits".
- The processing order (largest first, WP-77) is recomputed once per call; with per-call
  hand-over no incremental merge is needed.
- The refinement's internal set uses insertion order + hash index; the canonical order is needed
  only at the hand-over.
- Wishes for the ordering WP: (1) the storage order is a deterministic function of the index set
  (a sort key), cheap to recompute (a sort per refinement call is fine); (2) `CompleteSGGenInstr`
  and `MIVecSGGenInstr` grids of the same set should get the same order (today they differ); (3)
  the processing order stays derived from the storage order at construction / hand-over; (4) a
  DSG's full-set order follows the same key.

## 9. Risks

- Concurrency of the engine (management under one lock, rows published before tasks, exception
  capture): TSan run (`results/WP-28/tsan.sh` pattern), ASan, `--repeat 3`.
- M1 changes a header used by the judged binaries: code placement can shift cheap `exp` cases by
  ±2–6 % (RP-4 note); check with the bitwise diff + full `ab e2e`.
- Thread-count independence holds for `Synchronized` only; `Asynchronous` is documented as
  timing-dependent.
- `maxSeconds` stops are timing-dependent by nature.

## 10. Implementation notes (where the implementation refines the approved design)

- `QuadWeightTable` keeps a delegating `QuadWeightTable(const SparseGrid&)` (the SG operator, tests,
  `perf_micro`) beside the new `(const NodeTable&, const SparseGridSetup&)`; the per-TG quadrature
  kernels (`processTensorGrid`, `prepareTensorGrid`, `getWeights`) take the weight table instead
  of the `QuadratureContext`. Row rules come from the growth function's node count (equal to the
  row size of generated rows), so rows generated during a call get their rule too.
- Speculative evaluation (§5.1): the decision waits only for its needed tasks; every candidate's
  cost counts against the budget when a decision accepts it, also if it was evaluated ahead (so the
  budget does not depend on timing); up to `speculationWindow` tensor grids may be evaluated
  beyond the budget. Speculative multi-indices enter the index set when they are queued, so the
  index order (tie-break, commit order) differs from window 0 → results differ in round-off, but
  stay independent of the thread count.
- `getEstimate()` / `getErrorEstimate()` are NaN while the grid has unevaluated tensor grids (a
  call that stopped during the restart evaluation, e.g. budget or time).
- Exceptions of the management (e.g. of a user target's `finishCombination`) are captured inside
  the parallel region like those of the source function.
- A call that changes nothing (no commit, no decision) moves the node rows back into the grid's
  own tables: neither the TG order nor the storage version changes.
- `step()` always runs synchronized and needs no stopping criterion.
