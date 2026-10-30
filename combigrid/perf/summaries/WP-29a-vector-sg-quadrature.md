# WP-29a — Vector-valued source functions, design T, step (a): `VectorSourceFunc`, value types, SG quadrature + mivec

Status: PROPOSED · Date: 2026-10-30 · Base commit: e1257538d
Recommendation: accept — the agreed design-T feature, scalar path unchanged (e2e 1.002x, 0 SLOWER, memory 1.002, refcheck OK); vector runs 1.37–2.68x faster than m scalar runs at d10 / b1 d20 for m = 2–32
Machine: `perf.py calibrate` ratios e2e 1.001 / 0.999, micro 1.000 / 1.002 (burst / sustained)

## Change
- New `VectorSourceFunc(std::function<void(const DataVector&, std::span<double>)>, nOutputs)` (copy + move ctor like `SourceFunc`; `nOutputs()`, `evaluate(point, out)`, `evaluateNormalizedInPlace(point, area, out)`; defined in the `.cpp` like `SourceFunc`, LTO-inlined: no call to either `evaluate` left in `libsgppcombigrid.so`). No per-evaluation check (WP-25): `out.size()` is an `assert`; `nOutputs = 0` throws `base::data_exception`.
- Value types `tools::ScalarValues` (`Acc = double`) / `tools::VectorValues` (`Acc = double*`, runtime m; values + accumulator slots in a caller buffer sized by `bufferSize(m, nAccs)`), header-inline views taken by value, operator-independent (`zeroAcc(slot)`, `evaluate`, `addScaled`, `store`).
- SG kernel `sumOverRows` written once as a template over the value type (WP-01 locals rule kept); `processTensorGrid(context, SourceFunc|VectorSourceFunc, tg, scratch[, out])`. `misc::QuadratureContext` no longer holds the source function (design T); `QuadratureScratch(nDim, maxSum, nOutputs = 0)` holds the per-thread `valueBuffer` and the slot constants. Public overloads `quadrature(const VectorSourceFunc&, const SparseGrid&, std::span<double>)` (throws `base::data_exception` on a wrong size) and `base::DataVector quadrature(const VectorSourceFunc&, const SparseGrid&)`; per-thread m-vector results, combined in a critical section. mivec runs through the same code.
- Files: new `functions/source_functions/vector_source_function.{hpp,cpp}`, `tools/operators/value_types/{scalar,vector}_values.hpp`; changed `operators/quadrature/quadrature.{hpp,cpp}`, `miscellaneous/data_contexts/quadrature_context.hpp`, `miscellaneous/data_scratches/quadrature_scratch.hpp`, `sgpp_combigrid.hpp` (+ `vector_source_function.hpp`), `build/pysgpp/combigrid.i` (`%ignore` of both vector overloads), `perf_micro.cpp` (call sites + new cases), `perf_vector_proto.hpp` (`Setup::context()` without function), `quadrature_test.cpp` (2 call sites).

## Gain vs. cost
- Gain (feature): scalar path within noise: e2e `^quad/sg/|^mivec/` 10.482 → 10.457 s (1.002x, 0 SLOWER, 1 faster = placement, not claimed). Vector (1 thread, production kernels, `vector-gain.md`; gain = m scalar runs / one vector run, m = 1/2/4/8/32): **d10 L4 0.89 / 1.37 / 1.74 / 2.23 / 2.68**; **b1 d20 (all TGs) 0.93 / 1.45 / 1.71 / 2.15 / 2.48**; **d2 L16 0.66 / 0.78 / 1.02 / 1.18 / 1.29** (WP-24 rt in the same run: 0.92/1.39/1.71/2.15/2.56, 0.93/1.44/1.66/2.06/2.38, 0.68/0.81/1.01/1.18/1.38). m = 1 and d2 L16 m = 2 are slower than scalar runs (memory accumulators + m-loops per point, as in WP-24): a single output belongs in `SourceFunc`.
- Memory: geomean B/A 1.002; 1 MEM+ `quad/sg/equi-exp/b0/d7/L7` 14.3 → 14.8 MiB (1.033; in the discarded first run 14.4 → 14.5, no flag; the scalar path allocates nothing new: empty `valueBuffer`) → noise. Process floor 10.1 → 10.3 MiB.
- API / functionality: agreed design T only (owner 2026-10-30): `VectorSourceFunc` + 2 `quadrature` overloads; `SourceFunc`, existing public signatures and scalar results unchanged. Internal: `QuadratureContext` field removed, `quadrature_operator::processTensorGrid` takes the function; micro cases `micro/quad/process_tg|process_all_tgs/*` adapted (call only, same meaning). Python: unchanged (`%ignore`; fast check + full `SG_PYTHON=1` build, only SWIG warning 389, all 8 `examples/python` run, `pysgpp` has no `VectorSourceFunc`).
- Maintainability: +~320 lines production code incl. docs (2 value types 174, `VectorSourceFunc` 144, quadrature +~90); one kernel for both value types; the slot scheme serves WP-29b/c unchanged.

## Tests
- Pinning tests (passed 3× on the unchanged code, debug): `quadrature_test` (incl. `quadrature_operator_tensor_grids/*`, `multiindex_vector_sg_gen_instr`), `source_function_test` (WP-25), MI-set / generation suites (full suite).
- New: `vector_source_function_test.cpp` (5 cases: copy/move ctor + `evaluate`, m = 1, copies call copies, `evaluateNormalizedInPlace`, m = 0 throws); `vector_quadrature_test.cpp` (`Outputs_Equal_Scalar_Quadrature`: equi / cc / mixed, doubling + linear growth, b0/b1, d = 1–4, unit + shifted domain, complete L0 (single-point) / L3 + MIVec, m = 1/3, both overloads; per TG: same points in the same order as the scalar run and |Δ| ≤ 16 eps·|TG sum|; operator: ≤ 16 eps·vol·Σ|c|·TG sums; `Zero_Point_Tensor_Grid`; `Wrong_Result_Size_Is_Rejected`). First-kind Chebyshev nodes have no quadrature rule (not testable). `evaluate`'s out length: assert only (no per-evaluation check), not testable without aborting.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ (final) · ASan ✓ · new suites 200× in a row at 16 threads ✓. Earlier, 2 of 16 normal `perf.py test` runs failed one time-seeded test each: one not captured, the other the pre-existing `equidistant_nodes/sin_func_random_cube_3D` (seed 1793379453: expected −1.7e-4, 1e-12 relative tolerance; `WP-29a-before` fails that seed 200/200 too) → unrelated, see follow-ups.

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads (`refcheck.txt`). Bitwise (1 thread, `reference-bitwise.txt`): 100/104 identical; 4 `cc`/`uq` cases (2 SG, 2 **unchanged DSG**) differ by 1–8 ulp because LTO recompiled the unchanged Clenshaw–Curtis FFT weights (n ≥ 65: up to 16 ulp in some weights; identical sources give identical weights, every library change flipped them, also WP-24/25-after).

## Benchmarks (A = WP-29a-before, B = WP-29a-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`; sum 244.99 → 243.50 ms, 1.006x):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/process_tg/equi/b0/d10/L4 | 13.96 ms | 14.00 ms | 0.998 | [0.986, 1.010] |
| quad/process_tg/equi/b0/d2/L16 | 6.20 ms | 6.24 ms | 0.993 | [0.984, 1.002] |
| quad/process_tg/cc/b0/d2/L16 | 25.19 ms | 25.47 ms | 0.988 | [0.978, 0.998] SLOWER |
| quad/process_all_tgs/equi/b1/d20/L4 | 110.91 ms | 109.03 ms | 1.018 | [1.013, 1.023] faster |
| quad/process_all_tgs/cc/b1/d10/L5 | 52.67 ms | 52.65 ms | 1.009 | [0.983, 1.035] |

cc d2 L16 is dominated by the CC weights (WP-05): instructions −0.9 %, cycles +0.9 %, op-cache misses unchanged (`perf stat`); `micro/quadrule/cc_weights/n1025` 0.993 SLOWER (`micro-cc-weights.md`) → LTO recompile of unchanged code, not the kernel.

L2/L3 e2e (16 threads, `e2e.md`, 17 cases; a first run with a foreign-load WARN was discarded: 1.005x, 0 SLOWER):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/equi-sin/b0/d7/L7 | total | 2.419 s | 2.412 s | 1.004 | [0.996, 1.012] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.317 s | 1.304 s | 1.009 | [1.000, 1.018] |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.199 s | 1.208 s | 0.992 | [0.948, 1.038] |
| quad/sg/equi-exp/b0/d10/L4 | operator | 932.6 ms | 926.2 ms | 1.001 | [0.977, 1.025] |
| quad/sg/cc-uq/b0/d7/L7 | total | 703.3 ms | 692.9 ms | 1.020 | [1.008, 1.033] |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 741.0 ms | 768.8 ms | 0.987 | [0.936, 1.041] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 492.6 ms | 493.0 ms | 0.996 | [0.913, 1.087] |

Sum of medians (total, 17 cases): A = 10.482 s, B = 10.457 s, saved = 0.025 s (1.002x, within noise) · Reports: `combigrid/perf/results/WP-29a/`

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/equi-exp/b0/d7/L7 | 14.3 MiB | 14.8 MiB | 1.033 | MEM+ (noise, see above) |
| quad/sg/cc-uq/b0/d2/L20 | 1081.0 MiB | 1064.4 MiB | 0.985 | ~ |

Geomean B/A = 1.002 · process floor A = 10.1, B = 10.3 MiB

## Attempts not kept
- none (one variant, as the WP-24 prototype; no placement shift in the SG row loop, so no alignment attribute).

## Profile shift / follow-ups
- Owner's files now partly outdated: CLAUDE.md:144 (§6 "Not implemented: … vector-valued source functions") and combigrid/claude/module-reference.md:154 ("No vector-valued source functions (planned: WP-29 …)"): SG quadrature + mivec exist now; DSG / interpolation after WP-29b/c. Also CLAUDE.md §2 ("User functions are wrapped in `SourceFunc`") and python-bindings.md "Deliberately not exposed" (vector overloads until WP-29d).
- WP-29b: drop `sourceFunc` from `DeltaQuadratureContext` the same way, slots in `DeltaQuadratureScratch`. WP-29c: value types gain `evaluateNormalizedInPlace` / interpolation. WP-29d: remove the two `%ignore`s, `%ignore` the `std::function` ctors, `PyObject*` ctor, exception registry keyed by `VectorSourceFunc*`; Python gets the `DataVector` overload.
- WP-30: vector path at small m (m = 1: 0.66–0.93x; d2 L16 m = 2: 0.78x): memory accumulators + m-loop per point; WP-24 rowbuf was ≤ 5 % better at m = 2–8.
- Reproducibility: the CC FFT weights change bitwise with every library change (LTO); pinning would need explicit FMA/unfused code there (WP-05 touches it).
- Pre-existing flaky test `equidistant_nodes/sin_func_random_cube_3D`: relative tolerance on a near-zero expected value (b ≈ −a); an absolute floor would fix it (test-only WP).
- Strict aliasing: the touched code's hoisting stays necessary (locals across the opaque `evaluate`; `lower`/`width` are doubles like the stored nodes) → nothing to simplify.
- Dimension-adaptive refinement: not blocked; per-TG `processTensorGrid(…, VectorSourceFunc, …, out)` yields m values per TG (indicators per output); the vector run evaluates the same points (bitwise, same order) as the scalar run → same cache keys; a context without the function serves several functions / steps.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add
`--keep combigrid/tests/operators/quadrature/vector_quadrature_test.cpp` etc. only together with the feature)
