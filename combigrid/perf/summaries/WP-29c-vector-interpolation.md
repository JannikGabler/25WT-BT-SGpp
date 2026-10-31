# WP-29c — Vector-valued source functions, design T, step (c): global + linear interpolation

Status: PROPOSED · Date: 2026-10-31 · Base commit: a14f723a0
Recommendation: accept — the agreed design-T extension for `interpolate` / `interpolateLinear`, in the owner's refactored layout (kernels in `tools::`). Final tree (refactor) vs. before: e2e `^interp/|^interplin/` 0.999x (0 SLOWER, 1 faster), memory 1.004, refcheck OK. Vector run vs. m scalar runs at m = 32: 1.25–2.12x (interp), 1.21–1.59x (interplin); unchanged by the refactor
Machine: `perf.py calibrate` ratios e2e 1.001 / 0.999, micro 1.001 / 1.000 (burst / sustained)

## Change
- `InterpolationMethod`: new virtual `interpolate(pos, nodes, values /* n × m, node-major */, m, out)`. Its default applies the scalar formula per output (gathers the values). New protected multi-output `hasTrivialResult`. Both built-in formulas override it with "innerk" (factors once per node, then output by output with the scalar formula's operations). `BarycentricFormula` reuses the weights vector for the factors (no extra allocation). `OptBarycentricFormula` computes the factors in 64-node blocks into a stack array and accumulates the numerators in `out` (no allocation for any n). The scalar formulas are unchanged (same sizes and code).
- Global interpolation (owner's refactored layout, see "Owner refactoring"): `tools/operators/global_interpolation/global_interpolation_tools.{hpp,cpp}`, namespace `tools::global_interpolation`. Per-TG `processTensorGrid(SourceFunc, …)` / `processTensorGrid(VectorSourceFunc, …, span out)` call the kernel template `processTensorGridInternal<ValueType>`, which calls `interpolateFirstDim` / `interpolateLaterDim<ValueType>`; `getInterpolationCntPerDim` moved there too. The old per-TG `global_interpolation::interpolate` and the exported `interpolateFirstDim` / `interpolateLaterDim(SourceFunc, …)` are gone; the operator header only declares the public functions. New: public `interpolate(VectorSourceFunc, point, sg, span)` + `DataVector` variant (per-thread m-vectors, `critical`, `schedule(guided)` as the scalar driver).
- Linear interpolation: `tools/operators/linear_interpolation/linear_interpolation_tools.{hpp,cpp}`, namespace `tools::lin_interpolation`. It holds `processTensorGrid(SourceFunc | VectorSourceFunc, …)`, the template `sumOverCorners<ValueType>` (corner weight once for all m) and `computeNeighborGPWithWeight` (moved). `LinInterpScratch(nDim, nOutputs = 0)` + `valueBuffer` / `RESULT_ACC` / `N_ACCS` (empty for scalar). Public `interpolateLinear(VectorSourceFunc, …)` × 2 (`schedule(static)`). A wrong result size throws `std::invalid_argument`.
- Value types: `evaluateNormalizedInPlace(point, area)` (returns the values), `evaluateNormalizedInPlace(point, area, dst, idx)` and `interpolate(method, pos, nodes, values, dst, idx)`. The slot form makes the scalar instantiation the original `dst[idx] = f(…)` statement (see V2).
- Files: the 13 source files under `operators/{global,linear}_interpolation/`, new `tools/operators/{global,linear}_interpolation/*_tools.{hpp,cpp}` (owner), `miscellaneous/operators/value_types/{scalar,vector}_values.hpp` and `miscellaneous/data_scratches/linear_interpolation_scratch.hpp` (copyright banners added where they were missing); comment fixes (side task) in `quadrature_scratch.hpp`, `delta_quadrature_scratch.hpp`, `delta_quadrature_context.hpp`, `projected_sub_trie_builder.hpp`, `quadrature_tools.cpp`; `combigrid.i`; `perf_micro.cpp`, `perf_vector_proto.hpp` (call sites, owner); new tests `interpolation_pinning_test.cpp`, `vector_interpolation_test.cpp`; docs CLAUDE.md §2/§6, `module-reference.md` (3 places), `python-bindings.md` (1 row).

## Gain vs. cost
- Gain (feature): **scalar path within noise**: my version (after) e2e sum 10.235 → 10.266 s (0.997x; cc d10 0.987 [0.971, 1.004], with 8 rounds 0.994 [0.986, 1.002]); final tree (refactor) 10.258 → 10.263 s (0.999x). Vector (1 thread, `vector-gain.md`, gain = m scalar runs / one vector run, m = 1/2/4/8/32): interp **cheb1 d5 0.98/1.36/1.66/1.89/2.12**, cc d2 0.82/1.10/1.31/1.44/1.55, cc d5 0.89/1.04/1.15/1.19/1.25, **cc d10 0.93/1.07/1.25/1.38/1.55**; interplin d2 0.89/1.15/1.33/1.47/1.59, d8 0.95/1.06/1.11/1.16/1.21, **d10 1.01/1.00/1.20/1.24/1.44**. These match or beat the WP-24 innerk prototype in the same run (e.g. cc d10 m32 1.44, cheb1 2.04). m = 1 is 2–18 % slower than `SourceFunc` (worst: cc d2, whose largest TG does 8193 three-node interpolations; std::function + out-of-line multi-output `hasTrivialResult`) → WP-30.
- Memory: geomean B/A 1.003 (after) / 1.004 (refactor), 0 flags (largest: cc d10 24.5 → 25.0 MiB, inside the spread). Floor 10.3 / 10.1 MiB.
- API / functionality: the agreed design-T extension only. Internal: per-TG helpers moved from `global_interpolation::` / `linear_interpolation::` to `tools::global_interpolation::` / `tools::lin_interpolation::processTensorGrid` (micro call sites adapted, case names and meaning unchanged); `LinInterpScratch` ctor is `explicit` with an optional `nOutputs`. Python: unchanged in my version. Full `SG_PYTHON=1` build (only warning 389), all 8 `combigrid/examples/python` scripts OK, no new names in `pysgpp`. `InterpolationMethod` is not wrapped, so no directors are involved. **Not re-run on the refactor**; the operator headers now declare only the public functions, so the SWIG input shrinks.
- Maintainability: +941 / −266 lines of source incl. docs, banners and the new tools files (refactor). One kernel per operator for both value types; per-TG kernels in `tools::` as for the quadrature.

## Tests
- Pinning (passed 3× debug on the unchanged code): new `interpolation_pinning` (384 golden values of `interpolate` / `interpolateLinear`: equi/cc/cheb1, b0/b1, unit/shifted, d1–4, complete/MIVec, generic + on-node point; rel. 1e-12), plus the existing `interpolation_test`, barycentric tests, `vector_quadrature_test`, `vector_source_function_test`, `source_function_test`.
- New `vector_interpolation_test.cpp`: `vector_{global,linear}_interpolation/Outputs_Equal_Scalar_Interpolation` (the full matrix above × m = 1/3 × 2 points). Per TG, the same points in the same order; \|Δ\| ≤ 16 eps · Σ\|ℓ_i(x) f_i\| (explicit Lagrange basis) resp. 16 eps · interpolant of \|f\| (linear). Both overloads: ≤ 64 eps · Σ\|c\|·scale. Also `Wrong_Result_Size_Is_Rejected` (×2), `multi_output_interpolation_method/Default_Applies_Scalar_Formula_Per_Output` (external Lagrange subclass, bitwise) and `Built_In_Overrides_Equal_Default` (3 formulas, n = 1–17 incl. trivial paths, m = 1/3/4).
- Results, my version: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓. Refactor: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan not re-run. 20+ full-suite runs: `sin_func_random_cube_3D` (WP-35) did not fail. Test call sites follow the refactor (`tools::*::processTensorGrid`).

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads (`refcheck.txt`; V1/V2 and the refactor (`refcheck-refactor.txt`) also OK). There was no WARN, including `interp/sg/equi/b0/d2/L5/cos-shifted`: the scalar formulas are unchanged.

## Benchmarks (A = WP-29c-before, B = WP-29c-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`; sum 50.49 → 51.08 ms, dominated by cc d10):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interp/global_tg/cc/b0/d10/L2 | 47.03 ms | 47.62 ms | 0.988 | [0.980, 0.997] SLOWER (placement, see below) |
| interp/global_tg/cc/b0/d5/L6 | 1.12 ms | 1.12 ms | 0.917 | [0.725, 1.161] (V1/V2/V4: 0.999–1.001) |
| interp/global_tg/cheb1/b0/d5/L4 | 514.3 µs | 508.4 µs | 0.951 | [0.808, 1.118] |
| interplin/tg/equi/b1/d8/L5 | 42.60 µs | 42.48 µs | 1.003 | [0.997, 1.008] |
| interplin/tg/equi/b1/d2/L16 | 206.3 ns | 207.4 ns | 0.995 | [0.991, 1.000] SLOWER |

The other 7 cases (barycentric_1d ×4, cc d2, equi d3, interplin d10) are within noise.

L2/L3 e2e (16 threads, `e2e.md`, 12 cases; operator ≈ total; 0 SLOWER, 2 faster):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interplin/sg/equi/b1/d10/L5/p32 | 2.469 s | 2.481 s | 1.001 | [0.980, 1.021] |
| interp/sg/cc/b0/d10/L2/p8 | 1.971 s | 2.007 s | 0.987 | [0.971, 1.004] |
| interplin/sg/equi/b1/d8/L5/p256 | 1.532 s | 1.525 s | 1.004 | [0.982, 1.028] |
| interp/sg/cc/b0/d5/L6/p64 | 1.445 s | 1.447 s | 0.998 | [0.991, 1.004] |
| interp/sg/cc/b0/d7/L4/p16 | 1.266 s | 1.262 s | 1.006 | [0.993, 1.019] |
| interp/sg/cheb1/b0/d5/L4/p64 | 344.0 ms | 338.2 ms | 1.018 | [1.005, 1.031] faster |

Sum of medians (total, 12 cases): A = 10.235 s, B = 10.266 s, saved = −0.031 s (0.997x, within noise) · Reports: `combigrid/perf/results/WP-29c/`
Peak memory: geomean B/A = 1.003 · 0 MEM+/MEM- · process floor A = 10.3, B = 10.1 MiB

## Owner refactoring (snapshot WP-29c-refactor = the tree as left)
- Change: the per-TG kernels moved to `tools/operators/{global,linear}_interpolation/` (see "Change"). The kernel template was renamed to `processTensorGridInternal`; before that, sharing the name with the wrappers made `processTensorGrid(VectorSourceFunc, …, double*)` bind to the template, an undefined reference. I fixed the two remaining call sites, which passed `.data()` instead of the span (`global_interpolation.cpp:69`, `vector_interpolation_test.cpp:210`). The `noinline` / `always_inline` attributes are commented out: LTO now inlines both collapse steps into one `processTensorGridInternal<misc::ScalarValues>` (0x131b B; before: three functions 0x2dc + 0x8c1 + 0xa78), and `interpolateLinear`'s OpenMP region grew from 0x553 to 0x670 B. So the scalar code is no longer instruction-identical to before, but that costs nothing measurable.
- vs. WP-29c-after (`*-refactor-vs-after*`): e2e 12 cases all within noise. First run sum 0.991x, driven by two wide-CI cases; with 8 rounds cc d5 1.006 [0.995, 1.018] and interplin d8 1.006 [0.943, 1.073] (bimodal). Memory 1.001. Micro (54 cases incl. vector m1/8/32): 49 within noise; vector kernels 0.988–1.008. Repeated scalar runs at cc d10 (`global_tg_scalar_x_m` m8/m32) 0.986 / 0.987 SLOWER, but `global_tg/cc/d10` 0.993 (noise): placement scale.
- vs. WP-29c-before (`e2e-refactor-vs-before`): sum 10.258 → 10.263 s (0.999x), 0 SLOWER, cheb1 d5 1.026 [1.012, 1.039] faster, memory 1.004, 0 flags.

## Attempts not kept
- **Per-call `std::vector` for the factors** (WP-24 prototype) in `OptBarycentricFormula`: 10 % of the vector time went to malloc/free (cc d2 m1 0.74x, m2 1.05x). Replaced by the stack blocks (0.82x / 1.10x). Also dropped: zero-filling `out` (a `memset` call per 1D interpolation).
- **V1** (template bodies not inlined into the exported wrappers; pointer-based value-type ops): e2e 0.996x, 0 SLOWER; micro interplin d2 0.972 SLOWER (wrapper tail-jump), `interpolateLaterDim<Scalar>` −274 B (the store address was computed before the opaque call). **V2** (+ `always_inline`): exported scalar functions restored, LaterDim still different; micro 0.992x. **V3 = my final (WP-29c-after)**: slot-based ops make both collapse kernels instruction-identical. **V4** (V3 + `aligned(64)` on both collapse templates): op-cache misses 2.6–3.0 M (before) → 17–18 M, cycles +1.6–4 %, dropped (`placement-and-numerics.md`). The remaining cc d10 micro −1.2 % has identical instructions; it comes from op-cache misses 2.6–3.0 M → 7.6–8.8 M (placement).

## Profile shift / follow-ups
- Numerics: vector ≠ scalar by ~1 ulp per 1D step. GCC contracts the multi-output numerator loop only in its unrolled remainder (an FMA), while the scalar formula has none. Max \|Δ\| ≤ 11 eps · max \|value\| (`placement-and-numerics.md`). Bitwise equality would need control over FP contraction (a flag/attribute change, not done).
- WP-30: vector m = 1 (interp 0.82–0.98x, interplin 0.89–1.01x). The out-of-line multi-output `hasTrivialResult` (0xc87 B) costs 5–7 % at n = 3 nodes.
- WP-06/WP-08 (scalar, unchanged): per-TG allocations in the global kernel (nodes, results, inputValues, `getInterpolationCntPerDim`); the generic formula's weights per 1D call (cheb1). Placement: interp cc d10 is sensitive to op-cache layout (±1–4 %). Strict aliasing: the touched code had no pre-WP-14 workarounds.
- Owner files now outdated: PLAN.md/ANALYSIS.md name `global_interpolation::interpolateFirstDim` / `linear_interpolation::computeNeighborGPWithWeight` (now in `tools::global_interpolation` / `tools::lin_interpolation`; with the refactor `interpolateFirstDim` is inlined into `processTensorGridInternal` and no longer a profile symbol). `combigrid.i:239–242` `%ignore global_interpolation::*` / `linear_interpolation::*` match nothing now (harmless; also the older stale `quadrature_operator::*` ignores, WP-29d). `module-reference.md` key-classes/layout lines do not mention the new tools files yet.
- Refactor leftovers (owner's code, not touched): commented-out declarations/definitions and `TODO: Document` blocks in the tools files; the comment in `global_interpolation_tools.cpp:66` still says the collapse steps are `noinline`; attributes on the template declarations in the header would not reach the `.cpp` definitions anyway (to restore the old structure: `[[gnu::noinline]]` on the definitions); file-scope `using namespace` in both operator `.cpp` (cpplint `build/namespaces`), cpplint TODO warning `global_interpolation.cpp:40`. Re-run ASan and the `SG_PYTHON=1` build before committing.
- Dimension-adaptive refinement: not blocked. Per-TG vector functions exist for both operators (per-ℓ contributions for error indicators). The vector run evaluates the same points in the same order as the scalar one (tested), so a step-independent cache keyed by point fits. Interpolation still regenerates nodes/methods per TG and call (WP-06).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; `--keep combigrid/tests/operators/interpolation_pinning_test.cpp` keeps the pinning test, which is useful on its own)
