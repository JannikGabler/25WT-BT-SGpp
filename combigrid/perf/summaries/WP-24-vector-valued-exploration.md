# WP-24 — Vector-valued source functions: exploration (decision data)

Status: PROPOSED · Date: 2026-10-30 · Base commit: 2221501b2
Recommendation: owner decision needed — adopt design **T** (one generic kernel per operator, template on the value type; new `VectorSourceFunc` type, overloaded operators) for WP-29: scalar cost 0 (measured), vector gain 1.3–2.8x at m = 32; the one-path design costs SG +28…48 %, DSG +14…27 %, interp +3…16 % at m = 1
Machine: `perf.py calibrate` ratios e2e 1.003 / 1.000, micro 1.000 / 0.998 (burst / sustained)

## Change
- Kept: comparison benchmark `combigrid/benchmarks/perf/perf_vector_proto.hpp` (267 `micro/vec/...` cases,
  registered in `perf_micro.cpp`, +4 lines). All variants live in one TU: copies of today's kernels
  (SG row loop, DSG row loop via the public `forEachGPWithGlobalWeight`, per-TG global interpolation;
  checked bitwise against production) and the candidate designs, with header-inline wrappers (= production
  after LTO). Integrands as the e2e cases (`perf::expSum` / `cosProd` as function pointers, m = 1) and m
  outputs `expSumK` / `cosProdK` (m = 1, 2, 4, 8, 32; each output costs one scalar evaluation).
- Measured and reverted: **production prototype of design T** in SG quadrature + global interpolation
  (`results/WP-24/production-prototype.patch`, = snapshot WP-24-after); libs rebuilt byte-identical to WP-24-before.
- Designs: **T** template on the value type (`ScalarValues` = today's `double`, `VectorValues` = runtime m,
  accumulators in per-thread buffers); **rt** one path (scalar = m = 1 of a runtime-m kernel; `_direct`: scalar
  callable wrapped directly, `_nested`: around today's `std::function`); **ct** compile-time m; **rowbuf/rowbufk**
  row chunk evaluated into a buffer, then accumulated per point / per output; **batch** one type-erased call per
  row chunk (WP-17 shape); interpolation layouts **inner** (m innermost), **innerk** (m innermost, factors once
  per node, then output by output), **outer** (m outermost, scalar formula per output).

## Gain vs. cost
- Scalar cost (m = 1, time / today, 1 thread, `vector-prototypes.md`): **T 0.974–1.001** (SG, 5 configs);
  rt_direct SG 1.28–1.45, DSG 1.14–1.27; rt_nested SG 1.31–1.48; ct SG 1.11–1.21 (no loops: the out-parameter
  turns today's tail call into one more call level + store/reload); rowbuf 1.30–1.69, rowbufk 1.24–1.75, batch SG
  1.15–1.41, DSG 1.14–1.16; interp inner 1.03–1.16, innerk 0.99–1.13, outer 1.00–1.08. Cause (profile): at b0 the
  row dimension is 0 and most TGs have level 0 there → rows of 3 points, so per-row m-loops / calls dominate.
  Rough extrapolation of rt to the judged suite: ≈ +5…10 s of 43.6 s (SG 9.2 s, DSG 17.7 s, interp 5.7 s × ratios).
- Production prototype of T (A = WP-24-before, B = WP-24-after): micro sum 1.008x (0 SLOWER), e2e `^quad/sg/|^interp/|^mivec/`
  sum 16.254 → 16.107 s (1.009x, 0 SLOWER; 4 "faster" 1.011–1.021 = placement, not claimed) → **scalar cost ≈ 0**.
- Vector gain (one vector run / m scalar runs, generic runtime-m kernel; m = 2 / 4 / 8 / 32):
  SG d10 L4 1.39 / 1.67 / 2.12 / 2.56; SG b1 d20 all TGs 1.44 / 1.66 / 2.05 / 2.37; SG d2 L16 0.82 / 1.01 / 1.19 / 1.33;
  DSG b0 d10 L7 1.45 / 1.77 / 2.30 / 2.77; DSG b1 d20 L6 1.30 / 1.43 / 1.71 / 1.89; interp (inner) cc d2 1.14 / 1.34 / 1.45 / 1.59,
  cc d5 1.06 / 1.15 / 1.21 / 1.27, cc d10 1.10 / 1.27 / 1.35 / 1.48, cheb1 d5 1.28 / 1.60 / 1.86 / 2.08. Layout: m innermost ≫ outermost
  (cheb1 m = 32: 2.08 vs. 1.06; cc d2: 1.59 vs. 1.28). Compile-time m ≤ 5 % better than runtime m (m = 4), equal at m = 32
  → **runtime m, no dispatch over m**. Caveat: the gains include ILP of m independent outputs (latency-bound integrand),
  not only amortized grid work; expensive integrands gain less, Python callables (one call for m values) more (not measured:
  no NumPy in the system Python, bindings not built).
- Memory: prototype e2e geomean B/A 1.002, 0 MEM± (largest `quad/sg/equi-exp/b1/d20/L5` 56.3 → 58.1 MiB, within its 12 % spread).
- API / functionality: none kept. Maintainability: the kept benchmark is 1.1k lines (drop it with `reject` if not wanted for WP-29/30).

## Recommended design (T) and API change list
- `SourceFunc` and all existing operator signatures unchanged; scalar results unchanged (prototype: refcheck OK; 102/104 reference
  cases bitwise at 1 thread, 2 × 1 ulp, see below). `perf_e2e.cpp` / `perf_reference.cpp`: no edit, behavior as today.
- New `VectorSourceFunc(std::function<void(const base::DataVector&, std::span<double>)>, size_t nOutputs)`: `nOutputs()`,
  `evaluate(point, out)`, `evaluateNormalizedInPlace(point, area, out)` (header-inline or LTO-visible, as `SourceFunc`).
- New overloads (selected at compile time by the function type, no runtime dispatch): `quadrature(const VectorSourceFunc&,
  const SparseGrid&, std::span<double>)` + a `base::DataVector`-returning variant (Python), same for `DeltasSparseGrid`,
  `interpolate`, `interpolateLinear`; WP-09 multi-point: points × m.
- `InterpolationMethod`: new virtual `interpolate(pos, nodes, values /* n × m, node-major */, m, out)` with a default
  (scalar formula per output) for external subclasses; both built-in formulas override it (factors once per node,
  then output by output = the scalar operations per output).
- Internal: kernels written once as templates over the value type (`tools::ScalarValues` / `VectorValues`, small views passed
  by value: WP-01 locals rule); contexts take the source function out of `misc::*Context` (micro cases adapt).
- Python: `pysgpp.VectorSourceFunc(callable, nOutputs)` (callable(tuple) → sequence of m floats; length checked; exception
  registry + `COMBIGRID_SOURCE_FUNC_EXCEPTION` for the new overloads), operator overloads return a `DataVector`.
- Alternative T′ (one class, scalar or vector callable, one runtime branch per operator call): same kernels; one type for
  WP-25 and Python, but `double quadrature()` must reject m ≠ 1 at run time and `SourceFunc` grows. One path (rt): rejected by the numbers.
- WP-29 plan: (a) `VectorSourceFunc`, value types, SG quadrature + mivec (as the patch), tests, A/B `^quad/sg/|^mivec/`;
  (b) DSG (`processDeltaTensorGrid` over the value type; trie/global weights once per point for all m), A/B `^quad/dsg/`;
  (c) global interpolation (as the patch) + linear interpolation (corner loop), A/B `^interp/|^interplin/`; (d) bindings,
  Python example, `python-bindings.md` (CLAUDE.md §8 "No native vector-valued source functions" is the owner's file).
  Tests per step: component k == scalar run k within round-off for equi/cc/cheb1, b0/b1, shifted domains, d = 1–4, m = 1/3.

## Tests
- Pinning tests (passed 3× on the unchanged code, debug): existing `quadrature_test`, `interpolation_test`,
  `delta_quadrature_test`, barycentric formula tests.
- New (prototype only, removed with it): `vector_source_function_prototype_test` (quadrature + interpolation, component k vs.
  scalar k). Results: debug `test --repeat 3` ✓ · normal ✓ (prototype and reverted tree) · ASan n.a. (no kept code).
- Benchmark checks: every `micro/vec` case compares with the scalar runs (`max_ulp` counter). Without FMA contraction
  (`-ffp-contract=off` build of the benchmark TU) all 210 vector/generic comparisons are **bitwise equal**; with the judged
  flags 1–14 ulp per TG (SG), 0 (DSG rt, interp innerk/outer), ≤ 27 ulp inner (values near 0). GCC's Zen tuning avoids FMAs in
  register accumulator chains but not on memory accumulators; even the unchanged scalar copy flips 1 ulp between builds.

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads for WP-24-before and for the prototype (`refcheck.txt`); bitwise compare
  `reference-bitwise.txt`: 2 cases (`quad/sg/cc/b{0,1}/d5/L5/sin-unit`) 1 ulp, error vs. exact unchanged.

## Benchmarks (A = WP-24-before, B = WP-24-after = production prototype T; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/process_tg/equi/b0/d10/L4 | 13.85 ms | 13.86 ms | 1.001 | [0.985, 1.017] |
| quad/process_tg/equi/b0/d2/L16 | 6.31 ms | 6.18 ms | 1.021 | [1.017, 1.026] |
| quad/process_all_tgs/equi/b1/d20/L4 | 109.14 ms | 107.66 ms | 1.013 | [1.008, 1.018] |
| interp/global_tg/cc/b0/d5/L6 | 1.13 ms | 1.13 ms | 1.004 | [0.986, 1.022] |
| interp/global_tg/cheb1/b0/d5/L4 | 515.5 µs | 508.8 µs | 1.001 | [0.961, 1.042] |

L2/L3 e2e (16 threads, `e2e.md`, 25 cases), largest cases:

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/equi-sin/b0/d7/L7 | total | 2.415 s | 2.405 s | 1.011 | [0.998, 1.023] |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.000 s | 2.003 s | 0.999 | [0.988, 1.009] |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.446 s | 1.452 s | 0.998 | [0.993, 1.003] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.315 s | 1.309 s | 0.994 | [0.968, 1.022] |
| quad/sg/equi-exp/b0/d10/L4 | operator | 933.8 ms | 921.7 ms | 1.012 | [1.005, 1.018] |

Sum of medians (25 cases): A = 16.254 s, B = 16.107 s, saved = 0.147 s (1.009x, placement-level) · Reports: `combigrid/perf/results/WP-24/`
(`vector-prototypes.md` + raw runs: all prototype tables)

Peak memory: geomean B/A = 1.002, 0 MEM± (table in `e2e.md`) · process floor A = 10.2, B = 10.3 MiB

## Attempts not kept
- One path rt / rt_nested / ct / rowbuf / rowbufk / batch: scalar cost above (+10…75 %) → not viable as the scalar path;
  their vector gains equal T's vector instantiation within ±5 % (rowbuf best at m = 2–8, batch worst at m = 32).
- Interpolation inner (per-node m loop): best vector gain at m = 32 but ulp differences to the scalar → innerk preferred.
- Production prototype T: measured (≈ 0 scalar cost), reverted as planned (patch kept for WP-29).

## Profile shift / follow-ups
- WP-17: batches along rows of n₀ points do not pay at b0 (rows of 3 points; batch at m = 1: SG 1.15–1.41x slower) → chunks
  must span several rows or a whole TG; layout batch × m point-major; accumulate output by output over a chunk.
- WP-25: record = key + m doubles (m fixed per function → fixed-size records; m in the disk header); one lookup serves all
  outputs; keys independent of m. WP-09: results points × m, per-TG value tensors node-major (m innermost).
- WP-23 / base move: `VectorSourceFunc::evaluate` must stay inline / LTO-visible like `SourceFunc::evaluate`.
- Dimension-adaptive refinement: not blocked; the per-TG kernels return m values per TG (`processTensorGrid(…, out)`) →
  per-output or aggregated indicators without extra evaluations; an operator variant returning per-TG contributions helps.
- Bitwise reproducibility (owner): if vector == scalar (or results across refactors) must be bitwise, pin the accumulations
  (explicit `std::fma` or unfused form) — changes today's results by ulps; measure the cost first.
- Do not retry: runtime-m one path at m = 1 (+28–45 % SG), compile-time m (≤ 5 %), per-row batching at b0.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add
`--keep combigrid/benchmarks/perf/perf_vector_proto.hpp --keep combigrid/benchmarks/perf/perf_micro.cpp` to keep the comparison benchmark)
