# WP-45 — `DeltasSparseGrid` global interpolation operator (global weights) + judged cases

Status: PROPOSED · Date: 2026-11-12 · Base commit: 6138b8b9e (tree had the master's PLAN/STATUS edits)
Recommendation: accept, and apply the drafted judged cases. The new DSG `interpolate` is **3.4x (d2) – 8.5x (d7)** faster than the `SparseGrid` operator on the 6 nested e2e problems (16 t; 5.15 → 0.96 s), within 1.04–1.19x of evaluating the distinct points alone. cc results agree with the SG operator within 1.5e-14. Results are bitwise independent of the thread count. Judged suite unchanged (e2e 1.001x); one placement shift: cheb1 d5 0.98.
Machine: `perf.py calibrate` e2e 1.001 / 1.001, micro 0.995 / 0.997 (burst / sustained; before the A/Bs 1.002 / 1.002, 0.997 / 0.997)

## Change
- **Design (owner decision in session, option (B) of two):** global weights instead of a per-TG gather. I(x) = Σ_p W_p(x) f(p), with W_p = Σ_{ℓ ≥ m(p)} c_ℓ Π_k L_{k,ℓ_k,p_k}(x_k). These are the DSG quadrature's global weights with the 1D quadrature weights replaced by 1D basis values at x. Its row loop (`CTCoeffTrie`, projected sub-tries, `processDeltaTensorGrid`) is therefore reused unchanged: one pass, every grid point evaluated once, no value store, no per-TG collapse. Option (A) (gather, WP-36 prototype) measured 2.0–7.2x slower in the same run.
- **New public virtual** `InterpolationMethod::basisValues(pos, nodes, span out)` (owner-approved): the weights of the values in the 1D interpolant.
  - Default: n scalar interpolations with unit vectors (O(n²); custom methods).
  - Overrides: both barycentric formulas, one pass (factors / their sum). The optimized one is vectorizable.
  - Trivial cases mirror `hasTrivialResult` via the new protected `hasTrivialBasisValues`.
  - Not SWIG-wrapped (`InterpolationMethod` isn't).
- **Per grid**, `misc::DSGGlobalInterpIndex` in the `OperatorDataCache` (first call, thread-safe): `CTCoeffTrie`; per (dim, level) the regular nodes (as the SG operator generates them), method and delta order (`dsg_quadrature::mapNodeIdx`); rows and delta TGs sorted by size.
- **Per call** (`tools::dsg_global_interpolation::evaluateInterpolant`), one parallel region:
  - basis values per row, largest first, then a barrier;
  - `processDeltaTensorGrid` per delta TG, largest first, `dynamic`;
  - the delta-TG sums are stored and added in grid order, so the result is deterministic (see Reference check).
  - Serial without a region below `constants::global_interpolation::DSG_PARALLEL_MIN_GRID_POINTS` = 128 (crossover measured).
- **Overloads:** `interpolate(SourceFunc | VectorSourceFunc [, span], point, const DeltasSparseGrid&)`, C++ only, not in the umbrella header (WP-50). The Doxygen states the evaluation set and order, the numerics and the memory.
- Nested nodes only: cheb1 → the DSG constructor throws (documented).
- Files:
  - new `operators/global_interpolation/deltas_sparse_grid_global_interpolation.{hpp,cpp}`, `tools/operators/global_interpolation/deltas_sparse_grid_global_interpolation_tools.{hpp,cpp}`, `miscellaneous/data_contexts/deltas_sparse_grid_global_interpolation_index.{hpp,cpp}` (702 lines, ~half Doxygen);
  - changed `interpolation_method.{hpp,cpp}`, `(optimized_)barycentric_formula.{hpp,cpp}`, `constants.hpp`, `delta_quadrature_context.hpp` (doc only);
  - benchmarks `perf_micro.cpp` (+12 cases), `perf_interp_explore.cpp` (global "dsg api" column);
  - docs CLAUDE.md §2, `claude/module-reference.md`, `claude/python-bindings.md`.

## Gain vs. cost
- Gain: judged suite 0 by design. DSG vs. SG operator, all query points, 16 t (`explore-t16.md`, 7 reps):
  - cc: d10 1.980 → 0.392 s (**5.05x**), d7 1.264 → 0.149 s (**8.48x**), d5 1.443 → 0.172 s (**8.38x**), d3 0.241 → 0.042 s (**5.71x**), d2 0.174 → 0.051 s (**3.40x**).
  - equi d3 L5: 51.3 → 6.6 ms (**7.77x**).
  - 1 t, 4 points: 4.65 / 8.20 / 7.94 / 4.93 / 3.07x. Micro (1 point, 1 t): 3.1–7.8x; vector m = 4: 6.81x (d5) / 2.81x (d2).
  - Evaluations per call ÷ 4.21 / 7.48 / 7.79 / 4.86 / 2.75.
- Memory:
  - SG / judged cases unchanged (e2e geomean B/A 0.999, 0 flags).
  - Per call: basis values (Σ nodes over all levels, e.g. 33k doubles at cc d2 L12), the quadrature's per-thread scratch, and nTG·m doubles of sums. No value store: drafted d10 case peaks at 12.3 vs. 24.7 MiB for SG.
  - Per grid: the index (nodes of every level, trie).
- API: the agreed overloads plus `basisValues` (owner's decision). Python rebuilt with `SG_PYTHON=1`, all 10 examples ✓.
- Maintainability: ≈ 350 code lines; the hot loop is the DSG quadrature's (no new kernel).

## Tests
- Pinning (green on unchanged code, debug ×3): `interpolation_pinning/Global_Interpolation_Golden_Values`, `vector_global_interpolation/*`, `{equidistant,clenshaw_curtis}_node_interpolation/*`, barycentric-formula suites, `multi_output_interpolation_method/*`, `quadratureDelta_*`, `vector_delta_quadrature`.
- New `dsg_global_interpolation/*` (8 cases):
  - DSG == SG within a Lebesgue-scaled round-off bound: cc/equi, b0/b1/b2, unit/shifted, d1–4 complete + anisotropic MIVec, d5, d11, d12, cc d2 L9, d1 L7; points interior, on nodes, on the boundary, mixed.
  - Each grid point evaluated exactly once and nothing else (counting function vs. SG).
  - Vector == scalar per output == SG vector; serial path == parallel path **bitwise**; wrong size throws.
  - Concurrent first calls share one index; index rows match the quadrature layout (rows are permutations, basis values sum to 1); single-point grid.
- New `basis_values/*` (3): overrides == default probing == Σ L_i f_i (n = 1…33, on nodes, extrapolation); default probes the scalar interpolant; trivial cases.
- Results: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ (twice) · ASan ✓. No flaky recurrence this session.

## Reference check
- `perf.py refcheck`: OK, 120/120 at 1 and 16 threads (`refcheck.txt`).
- Drafted judged cases (worktree): 132/132 OK; DSG records bitwise equal at 1 and 16 threads.
- Numerics (`numerics-sg-vs-dsg.md`, `judged-cases.md`):
  - cc: equal to the SG operator within 1.5e-14.
  - High-degree equidistant (d1 L6, d2 L5, d3 L5, Lebesgue up to ~1e16): both operators are round-off-dominated (errors 0.2–0.7). The DSG is sometimes worse (d2 L5 max 0.70 vs. 0.32 = WARN-level vs. the SG reference), sometimes better (e2e equi d3 L5 sum 0.78 vs. 1.30).
  - The first version's reduction made these cases thread-count dependent (≤ 0.37) → fixed by the ordered combination.

## Benchmarks (A = WP-45-before, B = WP-45-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `micro.md`, 37 cases, sum 1.025x; dsg cases exist in B only):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interp/sg cc d8 L2 / d5 L5 / d3 L8 / d2 L12, equi d3 L5 (controls) | 155.8 / 74.19 / 14.78 / 16.64 / 3.25 ms | 156.1 / 74.07 / 14.77 / 16.65 / 3.26 ms | 1.000 / 1.002 / 1.002 / 0.999 / 1.000 | all noise |
| **interp/dsg** (same configs; vs. sg in B) | – | 35.09 / 10.85 / 3.14 / 5.30 / 0.417 ms | **4.45 / 6.83 / 4.70 / 3.14 / 7.82** | – |
| **interp/dsg_vec vs. sg_vec**, m = 4, d5 L5 / d2 L12 | – | 38.48 vs. 262.2 / 19.67 vs. 55.29 ms | **6.81 / 2.81** | – |
| micro/dsg/* (14 quadrature controls) | | | 0.987–1.10 | all noise |
| interp/global_tg/equi d3 L5; barycentric_1d/generic n9 | 119.1 µs; 824 ns | 120.7 µs; 832 ns | **0.985; 0.988** | SLOWER: placement (below) |

L2/L3 e2e (16 t, `e2e.md`, `^(interp\|interplin\|quad/dsg)/`, 25 cases, 5 rounds): 24 within noise. **`interp/sg/cheb1/b0/d5/L4/p64` 0.977 [0.964, 0.990]**; 8 rounds (`e2e-generic-r8.md`): 0.980 [0.971, 0.988] (336 → 343 ms), cheb1 d3 0.999, equi d3 0.992 (noise). Code placement:
- the hot functions of the generic formula (`interpolate`, `evaluateFormula`, `barycentricFormulaWeights`, `hasTrivialResult`) are byte-identical in size; only their 64-byte offsets moved;
- op-cache misses 38M → 101M at equal instructions (micro controls).
- WP-07 replaces this path.

Sum of medians: A = 28.206 s, B = 28.187 s (1.001x) · Peak memory: geomean B/A 0.999, 0 MEM±, floors 10.6 / 10.5 MiB.

Judged-case draft (`judged-cases.{md,patch}`): 6 e2e `interp/dsg/...` cases (nested-node problems of `interp/sg`; cheb1 has none) and 12 reference cases (120 → 132). The patch generalizes WP-47's `addLinearInterpolationDSG` to `addInterpolationDSG(linear, …)`. **+0.82 s per suite pass.** Apply, then `perf.py protect --authorized-by-owner`.

## Attempts not kept (`variants.md`)
- Option (A) per-TG gather: owner chose (B); WP-36's prototype is 2.0–7.2x slower than (B) in the same run.
- Unordered reduction over delta TGs (v1): thread-count dependent results in the Lebesgue regime → ordered sums.
- Gate 512 → 128 (crossover 65–113 points). Serial basis values (v1–v2): up to 55 % of a call (equi), replaced by the in-region pass.
- All later steps were kept: in-region basis values; largest-first delta TGs (1.11–1.38x on the parallel cases); vectorized optimized `basisValues` (≈ 2 % at d2 only, minimal).

## Profile shift / follow-ups
- Critical paths now:
  - cc d2 L12: the largest delta TG (12,288 points ≈ 0.5 ms of 0.82 ms). Split the rows of large delta TGs across threads in `forEachGPWithGlobalWeight` (shared with the DSG quadrature; WP-19-like).
  - equi: the largest row's O(n²) log weights (≈ 85 of 108 µs) → WP-07.
- Largest-first scheduling paid here, unlike in the earlier DSG quadrature test: re-check for `quad/dsg` at low d.
- WP-49 (interplin global slot weights) can follow this pattern. WP-09 (multi-point): one evaluation pass for all points, W_p(x_q) per point. WP-50: the overloads bind like the DSG quadrature (float / DataVector).
- Refinement:
  - helpful: index and trie rebuild in O(closure + nodes), operator-independent cache keys.
  - blocking: global weights form no per-TG / per-MI sums (error indicators need the SG operator or per-TG sums), and the index is immutable.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add `--keep combigrid/benchmarks/perf/perf_micro.cpp --keep combigrid/benchmarks/perf/perf_interp_explore.cpp` to keep the benchmarks)
