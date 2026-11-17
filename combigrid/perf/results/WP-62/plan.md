# WP-62 — Dimension-adaptive combination technique: plan

Date: 2026-11-17 · Base commit 14c1e3d16 · Evidence: prototypes in `proto/`, traces in `raw/`,
sources in [literature.md](literature.md), log in [progress.md](progress.md).
Timings: judged build, 16 threads unless stated, laptop on AC, `calibrate` 1.004 / 1.000 / 0.999 /
1.001. Evaluation counts and errors do not depend on the machine.

## 1. Recommendation in short

1. Implement the **Gerstner–Griebel scheme with a profit indicator and bulk marking** as a new
   driver class with **its own incremental structures** (sparse multi-index set, lazily extended
   1D rule table, value store). The existing grids and operators stay as they are; the driver
   hands its result over as a generation instruction that owns the index set and coefficients.
2. Reuse function values **by index** (one value block per multi-index = the points that are
   new in every dimension) for nested nodes; keep per-tensor-grid results for non-nested nodes.
3. Offer the new **predicted-profit ("lazy") selection** as an option, not as the default: it
   needs 1.3–5x fewer evaluations on the test functions, but it cannot see interactions that
   the lower-order increments do not announce.
4. Offer SGMK-style dimension buffering only as an explicit option for d ≫ 100.
5. Close three library gaps first: nestedness test for custom growth functions, the standard
   Clenshaw–Curtis growth 1, 3, 5, 9, …, and parallel regions gated by work.

Decisions for the owner: §5 (D1–D9). Work packages: §6.

## 2. Compared approaches and measured results

Prototype: `proto/adaptive_proto.hpp` (C++, against the library's node families, quadrature
rules and `SourceFunc`). Validation: on the final index set the adaptive estimate equals
`quadrature(f, SparseGrid)` and `quadrature(f, DeltasSparseGrid)` (relative difference ≤ 2e-14,
10 configurations), the incremental coefficients equal `computeCTCoeffs` (0 mismatches), and the
value store evaluates exactly the DSG's number of points.

Test set: Genz families (oscillatory, product peak, Gaussian, corner peak with exponent 2,
continuous), the thesis UQ integrand, coefficients a_k = 4 k⁻² (`alg2`), 4·2^{−(k−1)} (`geo`),
5 strong + 100x weaker rest (`eff5`); closed-form integrals. Clenshaw–Curtis nodes, doubling
growth, boundary offset 0 for d ≤ 5 and 1 for d ≥ 10 (with offset 0 the coarsest tensor grid has
3^d points). **N(ε)** = evaluations after which the relative error stays ≤ ε.

Methods: `level` complete CT · `qo` a-priori sets from 1D probes (product of per-dimension
profits, Nobile–Tamellini–Tempone style) · `gg` Gerstner–Griebel with |Δ_ℓ| · `ggp` the same with
|Δ_ℓ| / new points · `ggp-buf3` + SGMK buffer of 3 · `lazy` new, see §3.2.

### 2.1 Evaluations for a given accuracy (ratio to `ggp`, geometric mean over 4 functions)

| d, decay | ε | level | qo | gg | ggp-buf3 | lazy | lazy + 10 % exploration |
|---|---|---|---|---|---|---|---|
| 10 alg2 | 1e-4 / 1e-6 | 9.6 / 8.5 | 0.48 / 0.56 | 1.02 / 0.75 | 1.00 / 1.00 | 0.54 / 0.55 | 0.54 / 0.57 |
| 10 geo | 1e-4 / 1e-6 | 42 / 10 (1e-6: 1 of 4 reached) | 0.81 / 0.72 | 1.08 / 0.79 | 1.00 / 1.00 | 0.71 / 0.76 | 0.81 / 0.82 |
| 20 alg2 | 1e-4 / 1e-6 | 21 / not reached | 0.26 / 0.65 | 0.40 / 1.03 | 1.00 / 1.00 | 0.21 / 0.53 | 0.25 / 0.59 |
| 50 alg2 | 1e-4 / 1e-6 | not reached | 0.33 / 0.43 | 0.41 / 1.20 | 1.16 / 1.00 | 0.20 / 0.38 | 0.21 / 0.45 |
| 100 alg2 | 1e-4 / 1e-6 | not reached | 0.41 / 0.14 | 0.42 / 0.61 | 1.16 / 1.03 | 0.24 / 0.19 | 0.27 / 0.22 |
| 100 geo | 1e-4 / 1e-6 | not reached | 0.88 / 0.68 | 1.09 / 0.83 | 0.85 / 0.98 | 0.75 / 0.77 | 0.80 / 0.83 |
| 2 (offset 0, 6 functions) | 1e-6 / 1e-8 | 0.94 / 1.54 | 2.4 / 1.9 | 1.12 / 1.00 | – | 0.65 / 0.79 | – |
| 5 (offset 0, 6 functions) | 1e-6 / 1e-8 | 2.5 / 3.8 | 2.1 / 1.4 | 1.05 / 1.05 | – | 0.63 / 0.72 | – |

Budget 3e5 evaluations ("not reached" = not within it). Example d = 100, Gaussian: the complete
CT of level 2 costs 80 001 points for an error of 4.0e-3; `ggp` reaches 7.5e-8 and `lazy` 1.6e-8
with 2e5 points. Full tables: `python3 proto/analyze.py highd ratio 1e-4,1e-6 ggp` (also `lowd`).

Further results (details and numbers in [progress.md](progress.md) §2):

| Question | Result |
|---|---|
| Isotropic function (UQ integrand, d = 5) | adaptivity does not lose: N(1e-6) `ggp` 4473, `lazy` 4401, `level` 12 033 |
| Profit (|Δ| / work) vs. plain |Δ| | 0.40–1.20, no clear winner; profit is the literature default (SGMK, NTT) |
| Reuse of values (new points only vs. whole tensor grids) | 2.0–2.8x fewer evaluations for the same accuracy |
| Non-nested nodes (equidistant, linear growth; work = whole tensor grid) | works: d = 10 error 1.3e-5 … 1.5e-4 at 3e5 evaluations (level sets 1.1e-2) |
| Standard CC growth 1, 3, 5, 9 vs. built-in 1, 5, 9, 17 (offset 1), d = 100 | error at 400 / 1e4 / 1e5 evaluations 1.0e-3 / 9.5e-6 / 1.8e-8 vs. 1.4 / 5.2e-5 / 8.7e-7 |
| Vector-valued (3 outputs, norm = max of the relative increments) | costs what the hardest output costs alone (29 025 vs. 31 289 at d = 10); 0.54–0.59x of three scalar runs |
| Bulk marking (accept everything ≥ θ·best, at most B) | θ = 0.5, B = 64: 16–31x fewer rounds at −1 … +12 % evaluations; θ = 0.1 up to +50 %; fixed batches (θ = 0) up to 7.6x at small budgets |
| Error estimate η = sum of the active indicators | stopping at η ≤ tol gives error ≤ tol in 71–100 % of the GG runs, worst 23·tol; lazy worst 30–110·tol |
| Interpolation indicators (NumPy model, d = 2, 4, 8) | surplus on the new points, indicator points and the quadrature indicator stay within ≈ 5x; surplus / work is best or near-best (d = 8: 3.8e-5 vs. 1.9e-4 for the quadrature indicator) |

### 2.2 High dimension: buffering and robustness

Error at small budgets, Gaussian with `alg2` coefficients (`python3 proto/analyze.py perm budget …`):

| Case | Budget | ggp | ggp-buf3 | lazy | lazy-buf3 |
|---|---|---|---|---|---|
| d = 1000, important dimensions first | 300 / 3000 / 3e4 | 1.4 / 1.4 / 9.8e-6 | 1.9e-5 / 3.2e-4 / 1.4e-5 | 1.4 / 1.4 / 3.7e-8 | 5.5e-4 / 2.9e-5 / 2.1e-7 |
| d = 100, important dimensions first | 300 / 3000 / 3e4 | 1.4 / 2.6e-4 / 9.8e-6 | 2.0e-5 / 3.2e-4 / 1.4e-5 | 1.4 / 3.5e-5 / 2.4e-7 | 5.5e-4 / 2.8e-5 / 2.4e-7 |
| d = 100, important dimensions **last** | 300 / 3000 / 3e4 | 1.0 / 1.2e-4 / 6.2e-6 | **1.0 / 1.0 / 1.0** | 1.0 / 1.4e-5 / 1.7e-7 | 1.0 / 1.4e-5 / 1.7e-7 |
| d = 1000, important dimensions **last** | 300 / 3000 / 3e4 | 0.5 / 0.5 / 2.7e-7 | **0.5 / 0.5 / 0.5** | 0.5 / 0.5 / 6.0e-8 | 0.5 / 0.5 / 6.0e-8 |

- Buffering pays only while the budget is below a few evaluations per dimension (here 4·d).
- With a wrong order, buffered GG never reaches the important dimensions within the budget
  (every newly explored dimension is paired with all activated ones) and reports an error
  estimate of 8e-7 at a true error of 1.0. The lazy selection does not have this problem,
  because pairs that are only predicted cost nothing.

Hidden interaction (`inter`: Gaussian + k(x_p)·k(x_q), k vanishing at the level-0 node, p and q
the two least important dimensions; d = 50):

| Method | N(1e-4) | Remark |
|---|---|---|
| `ggp` / `gg` | 26 425 / 35 633 | finds it once both axes are accepted |
| `lazy` | never (error 0.22 at 3e5) | error estimate 2.2e-8 at the end: a silent failure |
| `lazy` + exploration share 0.25 / 0.10 | 75 153 / 155 321 | 1225 pairs × 16 points must be looked at |
| `qo` (a-priori) | never | no verification at all |

So the methods form a scale from "verify every admissible neighbour" (GG) to "verify nothing"
(a-priori); the lazy selection with an exploration share sits in between and makes the price of
robustness explicit.

### 2.3 Cost of the bookkeeping

| Variant | Measured |
|---|---|
| Incremental (prototype), GG, 1 thread, 1e6 evaluations | 10.5 / 28 / 47.5 ns per evaluation at d = 10 / 50 / 100 (≈ 3 µs per multi-index at d = 100) |
| Incremental, lazy | 21 / 115 / 192 ns per evaluation (unoptimized prediction) |
| Share of the wall time, GG: cheap integrand (34–156 ns) / 1 µs / 1 ms | 17–20 % / 1.5–3.6 % / < 0.02 % |
| Rebuild `SparseGrid(MIVecSGGenInstr)`, d = 10, 2109 indices | 1.5 ms (16 threads) / 2.5 ms (1) |
| Rebuild, d = 100, 15 950 indices | 73.7 / 513 ms, of which `computeCTCoeffs` 36.9 / 470 ms |
| One rebuild per accepted index over that d = 100 run (3084 steps) | ≈ 114 s / ≈ 790 s, against 0.0475 s incremental |
| The same with bulk marking (38 steps) | ≈ 1.4 s (16 threads) |
| Per-candidate loop with today's public API (single-index instruction + grid + `quadrature`) | 147–171 µs per index at d = 10; 303–369 µs at d = 100 (≈ 10 µs per point) |
| Values through `CachedSourceFunc` instead of the index store, 1 thread | + ≈ 170 ns (d = 10) / ≈ 480 ns (d = 100) per looked-up point; index store 9 / 73 ns per covered point |

Where the bookkeeping matters: with incremental structures never (≤ 20 % even for a 34 ns
integrand). A user loop on today's classes that rebuilds per accepted index is dominated by the
rebuild unless one evaluation costs ≳ 1 µs (d = 10) to ≳ 1 ms (d = 100).

Threads (1 µs integrand, 3e5 evaluations, d = 10 / 50): GG with one index per step 1.06x /
1.01x at 16 threads; with bulk marking θ = 0.5, B = 64: 6.8x / 7.7x (0.316 → 0.046 s, 0.341 →
0.044 s); lazy with bulk 5.0–6.5x / 3.2–4.7x. With the cheap integrand 16 threads are slower
than one (d = 10: 0.062 → 0.144 s): parallel regions must depend on the estimated work.

## 3. Recommended design

### 3.1 Algorithm

State: old set O (accepted), active set A (evaluated, not accepted), optional predicted set P
(admissible, not evaluated). The result always contains O ∪ A.

```
evaluate the root index; A = {root}
loop until a stopping criterion holds:
  take the batch B = candidates with priority ≥ θ · best (at most maxBulk, one kind)
  if B is predicted: evaluate B in parallel → A                      (lazy selection only)
  else: move B to O; N = admissible forward neighbours of B (all backward neighbours in O)
        verified selection: evaluate N in parallel → A
        lazy selection: predict Δ for each n ∈ N → P (no prediction available → evaluate)
```

- **Indicator** of an evaluated index: quadrature |Δ_ℓ Q|; interpolation max |Δ_ℓ f| over the new
  points (non-nested: over the points of the tensor grid ℓ); user-defined functional as a third
  kind. Vector-valued: norm over the outputs, default max_o |Δ_ℓ,o| / s_o with s_o = |first
  increment of output o| (alternatives: Euclidean, user weights).
- **Work** w_ℓ: new points Π (n(ℓ_k) − n(ℓ_k − 1)) when the values are reused, else the points of
  the tensor grid. Both come from the node table, not from a list of node families.
- **Priority**: profit |Δ_ℓ| / w_ℓ (or |Δ_ℓ|), optionally the GG safeguard
  max{w·profit, (1 − w)·n_root / n_ℓ}.
- **Prediction** (lazy): for a candidate j with ≥ 2 non-zero dimensions the geometric mean over
  the pairs (p, q) of |Δ_{j−e_p}|·|Δ_{j−e_q}| / |Δ_{j−e_p−e_q}| (exact for products of 1D
  functions); along an axis min(a, a² / b) from the last two increments; first-level axis indices
  are always evaluated. An **exploration share** x evaluates the cheapest unevaluated
  candidates until they amount to x of all evaluations. Diagnostics: number and size of
  "surprises" (true / predicted > 10).
- **Stopping**: error estimate η ≤ tolerance (sum of the active indicators, plus the predicted
  ones), evaluation budget (never overshot: a batch is cut), time budget, index / level limits,
  user callback per round.
- **Dimension buffering** (option): only the first b + (number of activated) dimensions are
  explored. The function is always evaluated in all d dimensions (level-0 node elsewhere), so
  SGMK's restriction on the function does not apply; the ordering assumption does.

### 3.2 Data structures (all new, internal: `misc::` / `tools::`)

| Structure | Content | Measured / reason |
|---|---|---|
| Sparse multi-index set | per index the (dimension, level) pairs with level > 0, hash index (open addressing), state, increment, norm, work, coefficient | cost per index independent of d; forward neighbours O(#activated dimensions) |
| Incremental coefficients | adding ℓ changes c of ℓ − z, z ∈ {0, 1}^{nnz(ℓ)} | replaces `computeCTCoeffs` (92 % of the rebuild at d = 100, 1 thread) |
| Hierarchical 1D rule table | per (node family, growth, offset, interval) group and level: nodes, weights, the nodes new at the level, difference weights per origin level; extended lazily, rows never move | built by comparing node lists (works for any nested family and growth); non-nested → flagged |
| Value store (nested) | one block per index: values at its new points (× m outputs), chunked arena | Δ_ℓ = Σ_{m ≤ ℓ} ⟨⊗ difference weights, block m⟩; the blocks are exactly the DSG's delta tensor grids; 8·m bytes per point |
| Result store (non-nested) | per index the tensor-grid result(s) Q_ℓ | Δ_ℓ by inclusion–exclusion over 2^{nnz} stored results; no extra evaluations, since every index is evaluated once as a candidate |
| Candidate heap | (priority, index); a second heap by work for the exploration share | – |

`SparseGrid` / `NodeTable` (WP-18) are **not** made growable: the driver never needs a
`SparseGrid` while it runs. This avoids the stable-row / incremental `setStorage` work noted in
`results/WP-18/design-and-obsolete-code.md` and leaves the judged code untouched.

Zero-coefficient tensor grids: every index of O ∪ A has its increment (and block or result)
stored, so nothing has to be recomputed for the indicators; the hand-over drops zero
coefficients as today.

### 3.3 API sketch

C++ (public, umbrella header; names are proposals):

```cpp
struct AdaptiveConfig {                       // plain data
  double tolerance = 0.0;                     // on the error estimate, relative to the first increment
  size_t maxEvaluations = SIZE_MAX;           // never overshot
  double maxSeconds = infinity;
  size_t maxIndices = SIZE_MAX;
  LvlType maxLevel = 30;                      // per dimension
  AdaptiveIndicator indicator = AdaptiveIndicator::Quadrature;       // | InterpolationSurplus | Custom
  bool profit = true;                         // indicator / work
  double workSafeguard = 1.0;                 // w of Gerstner-Griebel
  AdaptiveSelection selection = AdaptiveSelection::Verified;         // | Predicted
  double explorationShare = 0.1;              // Predicted only
  double bulkThreshold = 0.5;                 // theta
  size_t maxBulk = 64;
  size_t dimensionBuffer = 0;                 // 0 = explore all dimensions
  AdaptiveOutputNorm outputNorm = AdaptiveOutputNorm::MaxRelative;   // | Euclidean | Weighted
  bool reuseValues = true;                    // index store (nested nodes)
};

class AdaptiveCombination {
 public:
  // Domain, node families, growth and boundary offset come from the instruction; its index set
  // (if any) is the start set.
  AdaptiveCombination(const SGGenInstr& setup, const AdaptiveConfig& config);

  AdaptiveStatistics refine(const SourceFunc& f);        // until a stopping criterion holds
  AdaptiveStatistics refine(const VectorSourceFunc& f);
  bool step(const SourceFunc& f);                        // one round; false = finished
  bool step(const VectorSourceFunc& f);
  void setStepCallback(std::function<bool(const AdaptiveStatistics&)> callback);  // false = stop

  double quadrature() const;                             // current estimate (vector: span / DataVector)
  AdaptiveStatistics statistics() const;                 // evaluations, indices (old / active /
                                                         // predicted), error estimate, rounds,
                                                         // surprises, stop reason, times
  std::unique_ptr<SGGenInstr> genInstr() const;          // owns the set + coefficients
  // -> SparseGrid sg(*adaptive.genInstr()); DeltasSparseGrid dsg(*adaptive.genInstr());
  //    all operators as today
};
```

- **No virtual call on a hot path**: the policies act once per multi-index (≈ 3 µs of
  bookkeeping), where an enum switch or one `std::function` call (custom indicator, callback) is
  free. The per-point loops are the two evaluation kernels (block evaluation, tensor-product
  weighted sum), templates over `ScalarValues` / `VectorValues` as in design T; plain
  `SourceFunc` stays branch-free (WP-25).
- **Generic instead of dispatch lists**: nested vs. non-nested, work, new nodes and difference
  weights are properties of the rule table, derived from the node lists.
- **Caching (design W)**: the driver takes `const SourceFunc&`, so a `CachedSourceFunc` works
  unchanged and shares its values with later operator calls (keys are the same domain points;
  the driver must map with `tools::mapToDomain`). The driver calls `finishOperatorCall()` once
  per round (persistent caches then lose at most one round).
- **Hand-over**: an instruction class that owns the `LvlMIVec` and the coefficients
  (`genReducedMIVecWithCoeffs` returns them without closure / `computeCTCoeffs`; no reference to
  a caller-owned vector, unlike `MIVecSGGenInstr`).

Python (pysgpp; same names):

```python
cfg = pysgpp.AdaptiveConfig(); cfg.tolerance = 1e-8; cfg.maxEvaluations = 100000
adaptive = pysgpp.AdaptiveCombination(genInstr, cfg)
while adaptive.step(f):                       # f: SourceFunc / VectorSourceFunc / cached
    print(adaptive.statistics().errorEstimate)
value = adaptive.quadrature()
sg = pysgpp.SparseGrid(adaptive.genInstr())   # then interpolate / quadrature as today
```

`step()` replaces a Python callback (no director, no GIL handling beyond the existing
`SourceFunc` one); `refine()` is bound too. Python callables stay serialized by the GIL, so the
gain there is the evaluation count.

### 3.4 Required changes to existing code

| Change | Where | Why |
|---|---|---|
| Nestedness and level-to-level index maps for any growth function | `NodeGenFunc::isNested`, `getNodeIdxMapBetweenLvls` (today: throw "Unknown level-to-grid-point-count function") | standard CC growth with `DeltasSparseGrid`; generic rule table |
| Standard CC growth `1, 3, 5, 9, …` as a library function | `level_to_grid_point_count_functions`, Python `%callback` | 5–48x lower error at d = 100; Python cannot define growth functions |
| Instruction that owns its index set and coefficients | `sparse_grid_generation_instructions/` | hand-over without recomputation and without the reference gotcha |
| Work-dependent parallel regions | new driver (and WP-48's small-call gate for the operators) | 16 threads slower than 1 for cheap integrands |
| Chebyshev 1st kind has no quadrature rule (`getQuadRule` throws) | unchanged; documented | cheb1 is interpolation-only: adaptive interpolation needs the non-nested path |
| Operators, `SparseGrid`, `DeltasSparseGrid`, `SourceFunc`, caches | **none** | the driver is separate |

## 4. Limits of this exploration

- The prototype covers quadrature with one shared 1D rule on [0, 1]^d; domains, per-dimension
  families and vector kernels were emulated or not exercised (vector: one scalar function per
  output). The interpolation indicators were measured in a NumPy model, not against the library.
- Test functions are smooth and (near-)product or ridge functions, where the product prediction
  is at its best; `inter` is the only adversarial case. Results for real models can differ.
- The lazy bookkeeping is unoptimized (allocations per prediction); its 3.2–4.7x thread scaling
  at d = 50 is limited by that serial part.
- Time-based stopping and work-gated parallelism are designed, not prototyped.
- `qo` probes each axis up to 12 levels, which is wasteful at offset 0 (its bad low-d rows).

## 5. Open decisions for the owner

| # | Decision | Options | Evidence | Recommendation |
|---|---|---|---|---|
| D1 | Default selection | (a) verified (GG) · (b) predicted (lazy) · (c) lazy with exploration share | lazy 0.19–0.84x of the evaluations; misses `inter` at d = 50 (error 0.22, estimate 2.2e-8); share 0.25 finds it at 2.8x GG's cost | (a) default, (c) as option with share 0.1; state the limitation in the docs |
| D2 | Default indicator weighting | (a) \|Δ\| / work · (b) \|Δ\| · (c) GG safeguard w < 1 | (a) vs. (b) 0.40–1.20; w = 0.999 on `inter`: 4–6x the evaluations of w = 1 at d = 10, 1e-4 not reached at d = 50 | (a); w as an option, default 1 |
| D3 | Value reuse | (a) index store · (b) only `CachedSourceFunc` · (c) none | (a) 9–73 ns per covered point, 8·m bytes per point; (b) + 170–480 ns per point, 8(d + m + 1) bytes; (c) 2.0–2.8x evaluations | (a) for nested nodes, switchable off (memory); caches keep working on top |
| D4 | Grid classes | (a) separate driver + hand-over · (b) growable `SparseGrid` / `NodeTable` · (c) user loop on today's classes | (c) 147–369 µs per index, rebuild 73.7 ms at d = 100; (b) touches judged code | (a) |
| D5 | Bulk marking default | θ and maxBulk | θ = 0.5, B = 64: 6.8–7.7x at 16 threads (1 µs), −1 … +12 % evaluations; θ = 0.1 up to +50 % | θ = 0.5, maxBulk = 4 × threads; θ = 1 gives the sequential algorithm |
| D6 | Dimension buffering | (a) not at all · (b) option, off by default · (c) on for large d | helps only below ≈ 4·d evaluations; wrong order → error 1.0 with estimate 8e-7 | (b), documented with the ordering caveat; recommend it together with the lazy selection |
| D7 | Standard CC growth + generic nestedness as a prerequisite WP | yes / later | 5–48x lower error at d = 100; today no DSG for custom growth | yes, first WP (small, useful without adaptivity) |
| D8 | Tolerance semantics | (a) raw estimate η ≤ tol · (b) safety factor (η ≤ tol / 10) · (c) no tolerance, budgets only | error ≤ tol in 71–100 % of the GG runs, worst 23·tol | (a) with the factor documented; η relative to the first increment per output |
| D9 | Interpolation indicator default | (a) surplus on new points / work · (b) quadrature indicator · (c) indicator points | within ≈ 5x; (a) best or near-best, no extra evaluations; (c) needed for non-nested nodes | (a) nested, (c) non-nested, (b) selectable |

## 6. Implementation work packages

Feature WPs: each with tests first, an example, and feature benchmarks; the judged suite must
stay neutral (refcheck + `ab e2e` only where judged code is touched: A1). Order = dependency
order.

| WP | Scope | Files (new unless noted) | Tests | Benchmarks | Depends on | Risk |
|---|---|---|---|---|---|---|
| **A1** Generic nestedness + standard CC growth | `isNested` / index maps from node lists instead of a known-function list; `standardLvl2GPCntFunction` (name open) incl. Python constant | `functions/level_to_grid_point_count_functions/*`, `functions/node_generation_functions/**` (changed), `Functions.i` | DSG == SG results for the new growth; unchanged structure hashes for doubling / linear | `refcheck` + `ab e2e '^(quad\|interp\|interplin)/dsg/'` neutral; micro `gen` cases | – | low; touches DSG generation |
| **A2** Core structures | sparse index set + hash, incremental coefficients, hierarchical rule table, value / result store | `miscellaneous/adaptive/*`, `tools/adaptive/*` | coefficients == `computeCTCoeffs` on random admissible sequences (d = 1 … 50); new nodes == `genDeltaNodes`; store layout == DSG delta grids; offsets 0 / 1; non-unit domains (`mapToDomain` bitwise) | `micro/adaptive/{insert,neighbours,coeffs,delta}` vs. the rebuild numbers of §2.3 | A1 | medium: the kernel of everything |
| **A3** Quadrature driver, scalar | GG selection, profit, bulk marking, budgets / tolerance, statistics, `step` / `refine`, owning instruction, work-gated OpenMP | `operators/adaptive/*` (public), `sparse_grid_generation_instructions/*` (new class), umbrella header | estimate == `quadrature` on the handed-over SG / DSG; evaluation count == DSG points; budget never overshot; d = 1; isotropic == complete CT sets; thread-count independent set | feature e2e in a new unprotected `perf_adaptive.cpp`: Genz d = 10 / 50 / 100, N(ε) and wall time at 1 / 16 threads, cheap / 1 µs | A2 | medium |
| **A4** Non-nested nodes + vector values | result store path; kernels as templates over `ScalarValues` / `VectorValues`; output norms | A2 / A3 files (changed) | equidistant linear growth; vector output k == scalar run of k; norms | `perf_adaptive` vector and non-nested cases | A3 | low–medium |
| **A5** Interpolation target | surplus indicator on the new points (via `InterpolationMethod::basisValues`), indicator points for non-nested nodes (cheb1) | `tools/adaptive/*`, driver (changed) | surplus == difference of the library interpolants; validation-point errors against the NumPy model | error vs. points at fixed-seed validation points | A3 (A4 for cheb1) | medium: cost of the surplus evaluation |
| **A6** Python bindings + docs | `AdaptiveConfig`, `AdaptiveCombination` (`step`, `refine`, statistics, `genInstr`), example, `python-bindings.md`, `module-reference.md`, CLAUDE.md | `combigrid.i` (+ new `.i`), examples | Python checks as WP-50 (results == C++, exceptions, cached functions make 0 extra calls) | evaluation counts with Python callables | A3 (A4, A5 as available) | low |
| **B1** Predicted (lazy) selection (if D1 = option) | prediction, exploration share, surprise diagnostics, allocation-free bookkeeping | driver (changed) | product functions: prediction exact; `inter` found with the share; share = 1 ⇒ verified behaviour | N(ε) ratios of §2.1 as regression numbers | A3 | medium: robustness expectations |
| **B2** Dimension buffering (if D6 = option) | explored / activated dimensions | driver (changed) | equals SGMK's order on a reference case; d = 1000 smoke test | small-budget errors of §2.2 | A3 | low |
| **C1** Operators on stored values (optional) | `DeltasSparseGrid` operators reading the driver's value blocks instead of calling f (a surrogate that is never re-evaluated) | DSG operator overloads | results == operators with a cached function | interpolation at many points without function calls | A3, WP-49 state | medium: API |
| **C2** Resume / persistence (optional) | serialize the adaptive state; continue a run | `miscellaneous/adaptive/*` | round trip | – | A3 | low |

Suggested order: A1 → A2 → A3 → A6 (first usable release in C++ and Python) → A4 → A5 → B1 → B2
→ C1 / C2.

## 7. Impact on the judged suite and on PLAN.md

- Judged suite: unchanged by A2–C2 (new code paths only). A1 changes DSG generation helpers: a
  neutrality check is part of it. The suite has no adaptive case; feature benchmarks live in an
  unprotected file, as for caching and vector values.
- WP-49, WP-21, WP-51, WP-57, WP-60 (DSG internals): independent; C1 should follow WP-49 because
  it reads the same DSG operator data.
- WP-48 (small-call work gate) and the driver's work-gated regions solve the same problem: share
  the threshold logic.
- WP-09 (multi-point interpolation): A5's surplus evaluates one tensor grid at many points — the
  same kernel shape; if WP-09 is accepted first, A5 uses it.
- WP-31 (`isMIVecDownwardsClosed` in O(n·d)): not needed by the driver (admissible insertion
  keeps the set closed).
- WP-34 (Python GIL contention): bulk marking makes it more visible (many threads wait for the
  GIL); the adaptive driver should evaluate Python callables serially.
- WP-42 (non-myopic adaptive caching): an adaptive run is the "values are reused later" case;
  the index store makes the cache unnecessary inside the driver.
- WP-18 follow-up "growable `NodeTable`": not needed with design D4 (a).
- CLAUDE.md §6 "Not implemented: dimension-adaptive refinement" and module-reference "Known
  limitations": updated by A3 / A6.
