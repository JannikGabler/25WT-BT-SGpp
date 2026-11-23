# WP-61 — SG quadrature: bitwise thread-count-independent results (ordered, compensated per-TG sums)

Status: PROPOSED · Date: 2026-11-23 · Base commit: c43b8f21b
Recommendation: accept: SG quadrature (incl. mivec) is now bitwise reproducible (1 t == 2 t == 3 t == 16 t, run to run), its combination round-off dropped from up to 2e-8 to ≈ 1 ulp, and runtime/memory are unchanged within noise.
Machine: `perf.py calibrate` ratios e2e 1.003 / 1.000 (burst / sustained), micro 0.996 / 1.001 (1.000 = as at baseline time)

## Change
- `operators/quadrature/quadrature.cpp` (scalar + vector `quadrature` on `SparseGrid`): the `reduction(+)` (scalar) and per-thread partials + `critical` (vector) are gone. Now the parallel TG loop (`tools::quadrature::estimateTensorGrids`, same `schedule(dynamic)` + `computeAllRows` as before) writes the estimate Q_ℓ of TG i to **slot i** of a per-call buffer (`make_unique_for_overwrite`, nTG doubles, × m for vector-valued). The buffer is indexed by TG, not by processing position, so WP-77's largest-first order stays bitwise neutral. The buffer is then combined serially in TG order.
- **Owner decision in the session (2026-11-23): compensated summation instead of the plain loop.** Measurements below: the plain ordered loop had up to 20x the round-off of the old 16-thread split, and up to 300x the discretization error. The summation is new reusable tools code (owner: "outsource … to tools/ … re-usable, e.g. interpolation"). `tools/math/compensated_sum.hpp` is header-only and was generalized on the owner's request. It provides:
  - the accumulator `tools::CompensatedSum` (`add`, `addProduct`, `value`): Neumaier summation plus the exact product error (`std::fma`). The result is as accurate as twice-double precision then rounded, and independent of FMA contraction. It is inline because it runs in the caller's per-term loop.
  - templates `compensatedSum(r)`, `compensatedDotProduct(a, b)` and the one-pass `compensatedDotProducts(a, b, result)` for m outputs. They accept any contiguous range of arithmetic values (`std::span`, `std::vector`, `std::array`, `DataVector`; int / int64 / float / double …), constrained by the new concepts `tools::Arithmetic` / `tools::ArithmeticRange` (`tools/concepts/arithmetic.hpp`).
- `combineTensorGrids` (vector, parallel) was removed, along with per-thread `std::vector` allocations and the `critical` section. Docs: `quadrature.hpp` (comment only, umbrella header), `quadrature_tools.hpp`, `combigrid/claude/module-reference.md`.
- Files: `operators/quadrature/quadrature.{cpp,hpp}`, `tools/operators/quadrature/quadrature_tools.{cpp,hpp}`, new `tools/math/compensated_sum.hpp`, new `tools/concepts/arithmetic.hpp`, `benchmarks/perf/perf_micro.cpp`, tests (below), `combigrid/claude/module-reference.md`.

## Gain vs. cost
- Gain (functional): bitwise reproducibility.
  - B gave **1 distinct result per case over the 5 e2e rounds in all 24 cases**; A gave 4–5. The new test covers 1/2/3/16 t and repeated calls.
  - Accuracy: combination round-off ≈ 0 against the exact rational sum of the same Q_ℓ in all 22 judged grids (`results/WP-61/combination_variants.md`). The 16-t error vs. the exact integral is now the discretization error:

    | case | A (16 t, 5 rounds) | B |
    |---|---|---|
    | `b1/d10/L7` | 1.0–1.3e-10 | 5.0e-12 |
    | `b1/d15/L6` | 3.5–5.0e-9 | 2.0e-10 |
    | `cc-uq/d5/L10` | 7.0–7.7e-13 | 6.0e-13 |

    No case is worse than A's range (`e2e-results-per-round.md`).
- Runtime: e2e within noise everywhere, 0 SLOWER. Suite sum 9.236 → 9.254 s (0.998x); the largest shift is the bimodal `cc-uq/b0/d2/L20` (41 TGs, CI [0.900, 1.032]), not the combination. `expensive/` 9.018 → 9.038 s (0.998x, all cases ~ noise). The serial pass costs **119 µs at the largest nTG of the suite (54 264, `quad/sg/equi-exp/b1/d15/L6`, 2.2 ns/TG, `micro/quad/compensated_combination`)**: 0.025 % of that case's 0.49 s and not measurable in e2e. A plain loop would cost ≈ 84 µs.
- Memory: + nTG × 8 B × m per call: 434 KB at nTG 54 264 (scalar). e2e peak RSS geomean 0.999, 0 `MEM+`/`MEM−` (largest B/A `cc-uq/b0/d3/L14` 1.018).
- API / functionality: SG quadrature results change bits once (agreed 2026-11-15; summation choice by the owner 2026-11-23). At 1 t: 116 of the 432 pinned values changed, max |Δ| 8.9e-15 (15 ulp). No public API change.
- Maintainability: the drivers got simpler (no reduction / `critical`); +1 small reusable header-only tools module (accumulator + 3 templates, ≈ 70 lines of code) + 1 concept header.

## Tests
- Pinning tests (passed 3x on the unchanged code before the change): `grid_storage_pinning`, `quadrature`, `quadrature_weights`, `vector_quadrature`, `first_type_chebyshev_quadrature`, `textbook_operators`, `repeated_operator_calls`, `delta_quadrature`, `vector_delta_quadrature`.
- New:
  - `thread_count_independent_quadrature` (scalar + vector m = 3; b0/b1; cc/equi/cheb1; complete d4 L6 / d6 L4 shifted, mivec d5; nTG < threads: d1 (1 TG), d2 L2 (5 TGs); with/without cache; 1/2/3/16 t + 5 repeated 16-t calls; result == compensated TG-order combination of the per-TG kernel, at 1 and 16 t). On the unchanged code it failed with 1047 mismatches, at 2/3/16 t only.
  - `compensated_sum`: empty / single term, exact cancellation, exact product error, condition ≥ 1e6 vs. a double-double reference (≤ 2 ulp, n up to 5e4), multi-output == single-output bitwise, other element / range types (double, float, int64, span, DataVector) bitwise equal, `compensatedSum` / `CompensatedSum` == dot product bitwise on exact products.
- The generalization (v1: out-of-line non-template functions → header-only templates) changed no bits: all 432 pinned values and all 24 e2e results are identical to v1.
- **Re-pinned**: `grid_storage_pinning`, only SG entries: 45 debug + 46 optimized bitwise hashes (old → new in `results/WP-61/repinned_hashes.txt`). All DSG hashes and all point hashes unchanged; values before/after in `pinning_quad_values_*.txt`.
- Results: debug `test --repeat 3` ✓ (final code: 1 failure in 3 runs, the known flaky `equidistant_nodes/sin_func_random_cube_3D` (WP-35); 0 / 60 alone with the new and the before library) · normal `test --repeat 3` ✓ · ASan ✓ (final code, memory-capped) · Python: SG_PYTHON=1 rebuild (`quadrature.hpp` comment only; on the out-of-line version v1, whose results the final version reproduces bitwise) ✓; `results/WP-61/python_checks.py`: 8 grids × (scalar + 3 vector outputs), 1 t == 16 t bitwise, 3 repeated calls equal; all 12 `combigrid/examples/python` scripts run

## Reference check
- `perf.py refcheck`: WARN (1 and 16 threads), only the known owner-accepted `interp/sg/equi/b0/d2/L5/cos-shifted` (×2.34, unrelated). All quadrature / mivec cases OK.

## Benchmarks (A = WP-61-before, B = WP-61-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; all 16 classic `^micro/quad/` cases ~ noise, sum 732.90 → 732.20 ms):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/quad/operator/equi/b1/d20/L4 (nTG 10 626) | 106.99 ms | 107.00 ms | 1.000 | [0.997, 1.004] |
| micro/quad/operator/cc/b1/d10/L5 | 44.31 ms | 44.19 ms | 1.003 | [0.999, 1.007] |
| micro/quad/operator/cc/b0/d2/L16 | 112.32 ms | 112.43 ms | 0.999 | [0.996, 1.003] |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 107.59 ms | 107.19 ms | 1.007 | [0.992, 1.023] |
| micro/quad/compensated_combination/equi/b1/d15/L6 (new; A = v1 out-of-line, B = header-only) | 119.87 µs | 119.38 µs | 1.005 | [1.001, 1.008] |
| micro/quad/compensated_combination/equi/b1/d20/L5 (same) | 117.43 µs | 117.36 µs | 1.000 | [0.989, 1.011] |

L2/L3 e2e (16 threads; all 24 cases ~ noise in `total` and `operator`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b1/d15/L6 (nTG 54 264) | operator | 481.92 ms | 481.51 ms | 1.001 | [0.987, 1.016] |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.31 ms | 201.42 ms | 1.001 | [0.990, 1.012] |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.37 ms | 241.11 ms | 1.001 | [0.996, 1.006] |
| quad/sg/equi-exp/b0/d10/L4 | total | 903.80 ms | 903.42 ms | 1.001 | [0.994, 1.008] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.310 s | 1.306 s | 1.001 | [0.995, 1.007] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 372.71 ms | 378.30 ms | 0.988 | [0.972, 1.004] |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.155 s | 2.157 s | 0.999 | [0.995, 1.003] |

Sum of medians: suite A = 9.236 s, B = 9.254 s (0.998x, within noise) · `expensive/` 9.018 → 9.038 s (0.998x, within noise) · cached 576.7 → 577.0 ms · Full reports: `combigrid/perf/results/WP-61/` (`*-v1.*`: the out-of-line version, `*-plain.*`: the plain loop)

Peak memory: geomean B/A = 0.999, 0 flags; largest B/A `quad/sg/cc-uq/b0/d3/L14` 15.2 → 15.5 MiB (1.018); `quad/sg/cc-uq/b0/d2/L20` 190.6 → 189.1 MiB, `quad/sg/equi-exp/b1/d15/L6` 18.6 → 18.2 MiB · process floor A = 10.3, B = 10.4 MiB

## Attempts not kept
- Plain ordered loop (the prompt's design; snapshot `WP-61-plain`, `e2e-plain.md`, `micro-plain.md`): same runtime (suite 0.999x, 0 SLOWER), kept the 1-t bits (no re-pinning). But its round-off was large: e.g. `b1/d10/L7` 1.5e-9 vs. discretization 5e-12, 16-t error 1.2e-10 → 1.5e-9; `b1/d15/L6` 1.9e-8. Condition numbers Σ|cQ|/|ΣcQ| reach 8e5 at nTG 5e4. Replaced on the owner's decision.
- v1 of the adopted version: non-template `compensatedDotProduct(s)` for `CTCoeffType` × `double`, defined in a `.cpp` (snapshot `WP-61-after-v1`). Superseded by the generalized header-only module at the owner's request: same bits, same cost.
- Variants measured on the 22 judged grids (`combination_variants.{cpp,py,md}`; serial cost at nTG 54k, 1 t):

  | variant | worst rel. round-off | cost at nTG 54k |
  |---|---|---|
  | plain | 1.9e-8 | 84 µs |
  | old 16-way split | 4.1e-10 | 45 µs |
  | pairwise | 9.6e-11 | 50 µs |
  | Neumaier + product errors (adopted) | 0 | 165 µs in the tool; 119 µs in production |

## Profile shift / follow-ups
- WP-48 (`std::fma`-pinned combination): for SG quadrature it is done (the compensated sum is independent of contraction). The other drivers can reuse `tools::CompensatedSum` / `compensatedDotProduct(s)` too. Candidates are SG `interpolate` / `interpolateLinear`, which combine per-TG values with the same alternating coefficients and still use reductions over threads, and DSG quadrature, which still uses `reduction(+)` and so is not bitwise thread-count independent either.
- WP-77: the TG-indexed buffer makes the largest-first order bitwise neutral; the `estimateTensorGrids` loop is the only place to change.
- Python test scripts must not iterate a `DataVector` or use negative indices (abort in OPT=0 builds; WP-37).
- Pre-existing, unrelated: cpplint line length in `benchmarks/perf/perf_cache.hpp:109` (re-linted because `perf_micro.cpp` changed).
- Tests on the intermediate plain-loop version: 1 failure in 9 debug runs, name not captured. The 6 reruns with failure names captured were clean, and the final code was clean 3/3 (debug), 3/3 (normal) and 1/1 (ASan). Presumably it was the known flaky `equidistant_nodes/sin_func_random_cube_3D` (WP-35).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
