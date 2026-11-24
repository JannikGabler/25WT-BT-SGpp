# WP-48 — Ordered compensated CT combination in all remaining operator drivers (+ small-call gate)

Status: PROPOSED · Date: 2026-11-24 · Base commit: 1b8e1495f
Recommendation: accept: every operator is now bitwise thread-count independent (0 of 146 reference cases vary at 1/2/3/16 t, before 53), runtime neutral, interplin/sg d2 1.12x; the gate (part 2) was refuted and is not in the tree.
Machine: `perf.py calibrate` ratios e2e 1.004 / 0.999, micro 0.996 / 1.001 (burst / sustained)

## Change
- WP-61 pattern in the three remaining drivers: the parallel loop writes TG i's value to slot i, then one serial compensated pass. Same names and return types as SG quadrature's `estimateTensorGrids` (scalar `std::vector<double>`, vector `FlatNestedVec<double, 2>(nTG, m)`). DSG quadrature: `tools::dsg_quadrature::estimateDeltaTensorGrids` (replaces `reduction(+)` and `combineDeltaTensorGrids`) + `compensatedSum(s)` (global weights already hold the coefficients). SG `interpolate` / `interpolateLinear`: `tools::{global,lin}_interpolation::estimateTensorGrids(f, normPoint, sg)` (replaces `reduction(+)` and the vector `combineTensorGrids` / per-thread vectors + `critical`) + `compensatedDotProduct(s)`. Schedules unchanged (dynamic / guided / static) and slots are TG-indexed, so WP-77 can reorder freely.
- The interpolations allocate their per-call vector like the quadratures. Owner review: a first version reused thread-local storage (`misc::TensorGridValueBuffer`) to avoid this allocation. Against the plain vector it gained nothing measurable, so it was removed (see "Attempts not kept").
- `compensatedDotProducts` no longer heap-allocates its m accumulators: blocks of `COMPENSATED_SUMS_PER_PASS` = 8 on the stack (bitwise the same results; one pass for m ≤ 8). New `compensatedSums` (m outputs) uses the same loop.
- Files: `operators/quadrature/deltas_sparse_grid_quadrature.cpp`, `operators/global_interpolation/global_interpolation.cpp`, `operators/linear_interpolation/linear_interpolation.cpp`, `tools/operators/{quadrature/deltas_sparse_grid_quadrature_tools,global_interpolation/global_interpolation_tools,linear_interpolation/linear_interpolation_tools}.{hpp,cpp}`, `tools/numerics/compensated_sum_tools.hpp`; tests (below); `benchmarks/perf/perf_micro.cpp` (+13 cases); docs `combigrid/claude/module-reference.md`. No umbrella-header header changed (no SWIG change).

## Gain vs. cost
- Gain: functional: all operators are bitwise thread-count and run-to-run independent. `perf_reference` at 1/2/3/16 t (+ 2nd 16-t run): before 53 of 146 cases thread-count dependent (48 differ run to run at 16 t), after 0 (`reference_bits.txt`). Python: 24 operator results 1 t == 16 t bitwise. Runtime: suite sum 36.347 → 36.343 s (neutral), `expensive/` 17.761 → 17.764 s (neutral). One case faster beyond noise: interplin/sg d2 L16 p16384 1.117x total (112.0 → 99.9 ms, −12 ms), operator 1.126x. This is presumably the `reduction(+)` combine at the end of each of the 16384 per-point regions that is gone now (not profiled).
- Memory: geomean B/A 1.003, 0 MEM+/MEM- (largest `quad/sg/cc-uq/b0/d2/L20` 191.9 → 198.9 MiB, within its 10.5 % spread; that driver is unchanged). Extra per call: nTG × m × 8 B (DSG quad d20 L6: ≈ 1.8 MB, scalar; interpolations: a few KiB).
- API / functionality: no public API change. 1-t result bits change once for DSG quadrature and SG interp / interplin (owner-agreed 2026-11-24). Internal: `combineDeltaTensorGrids` / `global_interpolation::combineTensorGrids` were replaced.
- Maintainability: the three drivers now share one shape (slot loop + compensated pass) with SG quadrature. Net code is small. No new class.
- Cost at 1 thread (micro): small SG interplin calls are 1.5–2.4 % slower (+30 ns d1 L10, +120 ns d2 L16 per call, measured with the buffer version; the final vector version is equal or slightly faster on these cases). Larger interpolation calls and DSG quad are within noise. `micro/interp/sg/cheb1/b0/d5/L4` 0.994 [0.988, 0.999]. `micro/dsg/compensated_combination`: ≈ 2.0 ns per delta TG (451 µs at nTG ≈ 2.3e5 for d20 L6), vs. 1.135 s for the e2e case.

## Tests
- Pinning (passed 3x on the unchanged code, debug): all existing tests, incl. `grid_storage_pinning`, `quadrature` suites, `quadratureDelta_*`, `vector_{delta_,}quadrature`, interpolation (SG + DSG, scalar + vector), `textbook_operators`, `repeated_operator_calls`, `thread_count_independent_quadrature`.
- New: `operators/thread_count_independent_operators_test.cpp`. It covers DSG quadrature (with/without cross-call cache), SG interp + interplin and, as controls, DSG interp + interplin. Variants: scalar + vector m = 3; cc / equi / cheb1 (DSG nested only); b0 / b1; complete / mivec; shifted domains; d1 and d2 L2 (fewer TGs than threads). It checks 1/2/3/16 t + 5 repeated 16-t calls bitwise, and that the result equals the compensated TG-order combination of the per-TG kernels. It failed on the unchanged code (3041 failures, DSG: quadrature values only). Extended `compensated_sum_test` (`compensatedSums`, m > 8 bitwise == single output).
- `grid_storage_pinning` re-pinned: bitwise hashes of 74 (debug) / 76 (opt) `dsg` entries (DSG quadrature). Point hashes and SG values are unchanged. Old → new in `results/WP-48/grid_storage_pinning_repin.md`.
- Results (buffer version): debug `test --repeat 3` ✓ · normal ✓ (3x) · ASan ✓ (`results/WP-48/verify_debug_asan.txt`) · Python examples 12/12 ✓. Final vector version: normal ✓, debug `test --repeat 3` ✓, refcheck and reference bits as below. Flaky tests: no recurrence.

## Reference check
- `perf.py refcheck`: WARN (1 and 16 t), only the known owner-accepted `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34; identical |new−exact| = 3.797e-06 in WP-48-before, -after and -vector, so this WP does not change it). 145 OK. `WP-48-vector` (final): 0 of 146 cases thread-count dependent (`results/WP-48/vector-variant/reference_bits.txt`).

## Benchmarks (A = WP-48-before, B = WP-48-after; speedup = A/B, > 1 is faster)
B is the buffer version. The final vector version was compared against B directly (light A/B, 3 rounds; see "Attempts not kept").

L1 micro (1 thread; full table `results/WP-48/micro.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/dsg/compensated_combination/equi/b1/d20/L6 | 450.82us | 451.42us | 0.998 | [0.993, 1.004] |
| micro/dsg/operator/equi/b0/d5/L8 | 49.66ms | 49.65ms | 1.001 | [0.990, 1.012] |
| micro/interp/sg/cc/b0/d5/L5 | 61.54ms | 61.63ms | 0.999 | [0.995, 1.002] |
| micro/interp/sg/cheb1/b0/d5/L4 | 18.42ms | 18.54ms | 0.994 | [0.988, 0.999] |
| micro/interplin/sg/equi/b1/d2/L16 | 8.58us | 8.70us | 0.985 | [0.976, 0.995] |
| micro/interplin/sg/equi/b1/d1/L10 | 1.27us | 1.30us | 0.976 | [0.964, 0.988] |
| micro/interplin/sg_vec/equi/b1/d2/L16/m4 | 25.10us | 25.16us | 1.006 | [0.979, 1.034] |
| micro/expensive/quad_vec/dsg/cc/b1/d7/L4/m4/c1us | 22.26ms | 22.27ms | 1.000 | [0.996, 1.003] |

(`micro-t16.md`: the same interpolation cases at 16 t. Unusable: µs calls measure as ms with CIs up to 5x, from wake-up artifacts of the micro harness at 16 t. Not used.)

L2/L3 e2e (16 threads, full suite incl. `expensive/` and cached):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 112.03ms | 99.87ms | 1.117 | [1.075, 1.160] |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 105.15ms | 93.02ms | 1.126 | [1.081, 1.174] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.409s | 5.403s | 1.001 | [0.997, 1.004] |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.875s | 6.875s | 0.993 | [0.981, 1.005] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.607s | 1.604s | 1.001 | [0.994, 1.008] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.853s | 1.856s | 1.001 | [0.981, 1.022] |

Sum of medians: suite A = 36.347 s, B = 36.343 s (1.000x); `expensive/` 17.761 → 17.764 s; cached 1.044 → 1.043 s. No case SLOWER; 1 faster. Reports: `combigrid/perf/results/WP-48/`

Peak memory: geomean B/A = 1.003, 0 flags; process floor A 10.3 MiB, B 10.5 MiB.

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 191.9 MiB | 198.9 MiB | 1.037 | ~ (spread 10.5 %) |
| interplin/sg/equi/b1/d2/L16/p16384 | 21.1 MiB | 20.6 MiB | 0.975 | ~ |

## Attempts not kept
- `misc::TensorGridValueBuffer` (thread-local per-call buffer of the SG interpolations, ≈ 60 lines + constant + test), required by the WP prompt's "no per-call heap allocation". A = buffer (`WP-48-after`), B = plain `std::vector` / `FlatNestedVec` (`WP-48-vector`, final), 3 rounds (`results/WP-48/vector-variant/`):
  - micro, 1 t, 25 interp / interplin / DSG cases: all within noise or B faster (`interplin/sg` d2 L8 1.009x [1.002, 1.017]); sum 1.000x.
  - e2e `^(expensive/)?interp(lin)?/sg/`, 21 cases: all within noise. Suite 7.738 → 7.745 s (0.999x); `expensive/` 4.925 → 4.948 s (0.995x, from one wide-CI case: interplin c100us [0.931, 1.039]). Memory geomean 0.996.
  - Verdict: the malloc of a few KiB is negligible next to the per-thread scratches every call already allocates. Removed for simplicity and consistency with SG quadrature.
- Part (2), small-call gate for SG interplin (`interplin/sg` d2 L16 p16384, e2e at 16 t, env-switched in the new code, 2 runs each): always full team 95–107 ms operator; serial below a corner-visit gate 194–196 ms (2x slower); thread cap 8 t 98–99, 4 t 129–130, 2 t 172–173 ms. Refuted: with `schedule(static)` each thread keeps the same TGs across calls and finds their 2^16-node rows in its own L2. One thread over all 33 TGs misses cache (12 µs vs. 5.8 µs per call). Serial pays only for tiny grids (1 t micro: d1 L10 0.44 vs. 4.7 µs, d2 L8 3.9 vs. 5.3 µs at 16 t), which are not in the suite. A corner-count gate also ignores the source-function cost (expensive functions would serialize). No gate for SG interp either (no small suite case gains). Recorded in the comment of `lin_interpolation::forEachTensorGrid`.

## Profile shift / follow-ups
- WP-77 (largest-first / chunked dynamic for SG interplin): the static schedule's per-thread cache locality is worth ~2x on interplin/sg d2 L16 (see above). A dynamic schedule there risks losing it, so A/B `^interplin/sg/` carefully or keep static for interplin.
- Optional: run a call on the calling thread when nTG == 1 (d1 grids, 4.7 → ~0.4 µs per call); untracked, tiny grids only.
- The per-call per-thread scratches of the SG interpolations (`GlobalInterpScratch`, `LinInterpScratch`: several heap allocations per thread per call) could reuse thread-local storage like the TG buffer. Matters only for small grids (µs calls).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep combigrid/tests/operators/thread_count_independent_operators_test.cpp` to keep the
new test: it fails on the reverted code by design)
