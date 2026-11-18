# WP-73 — Cross-call operator data cache: user control + per-operator flags

Status: PROPOSED · Date: 2026-11-18 · Base commit: 76a83f06b
Recommendation: owner decision needed — accept the API as built (defaults = today, concurrency variant (a)); confirm defaults and variant (a), and the bug fix done in this WP (owner's choice in-session).
Machine: `perf.py calibrate` start e2e 1.004 / 1.001, micro 1.073 / 1.083 (burst / sustained; WARN, background state at session start); before the measurements e2e 0.999 / 0.999, micro 1.007 / 1.006

## Change
- **Cache** `misc::OperatorDataCache` → `misc::CrossCallOperatorDataCache` (`miscellaneous/cross_call_operator_data_cache/`, + `CrossCallOperatorDataEntry`, `CrossCallOperatorDataDescriptor`): one object per (type, key string); build-once per key under a per-entry lock (outside the cache lock: other keys proceed); objects handed out as `shared_ptr` (a clear drops only the cache's reference; a build that started before a clear is not stored); a throwing build stores nothing. Member `SparseGridBase::crossCallOperatorDataCache_`, internal accessor `getCrossCallOperatorDataCache()`.
- **Public API** (`SparseGridBase`, both grids, C++ + Python): `clearCrossCallOperatorDataCache()`, `getCrossCallOperatorDataCacheEntries()` → `std::vector<CrossCallOperatorDataInfo>{operatorName, description, key, bytes}`, `getCrossCallOperatorDataCacheBytes()` (all `const`, thread-safe). Stored types report `memoryBytes()` (capacities; SG weight table: all rows, race-free).
- **Options** (last, defaulted argument of every overload incl. vector / span): `QuadratureOptions` (SG `QuadWeightTable`, DSG new `misc::DSGQuadIndex` = delta-order 1D weights + `CTCoeffTrie`; default **off** = today), `GlobalInterpolationOptions` / `LinearInterpolationOptions` (DSG indices; default **on** = today); field `useCrossCallOperatorDataCache`. Off: data built per call, cache neither read nor written (no lock). SG `interpolate` / `interpolateLinear` keep nothing → no options (documented `@note`). Operators get their data through `tools::cross_call::getOperatorData` (+ build counter `nOperatorDataBuilds()` for tests). No extra operator-specific options: every entry depends on the grid only (key ""), scalar and vector calls share it.
- **Bug fix (pre-existing, owner agreed in-session)**: DSG `interpolateLinear` on its serial path (below `DSG_PARALLEL_MIN_CORNER_VISITS`) ran orphaned `omp for` loops without its own region, so called from a user's `omp parallel` region they bound to the caller's team and every call computed only part of its work (d3 L4: 0.9104 instead of 0.9266; reproduced on WP-73-before). Now plain loops on the serial path (`parallel` flag of `transformDimension` / `evaluateChunks`). All other orphaned `omp for` sit inside their own (possibly `if`-gated) region.
- Docs: Doxygen of all operators (`@note`: data, reuse, memory, default), cache / option / info headers; `module-reference.md`, CLAUDE.md §2 / §6, `python-bindings.md`; umbrella header (4 public headers). SWIG: forwarders in `COMBIGRID_SPARSE_GRID_BASE_METHODS`, `CrossCallOperatorDataInfoVector`, options `%include`d; the old `%ignore DeltasSparseGrid::getOperatorData` was not needed (`SparseGridBase` is not wrapped) and is removed; the span `%ignore`s now spell out the defaulted options argument (else SWIG wrapped the generated short span overloads — checked in the wrapper).
- Files: `grids/sparse_grid_base.{hpp,cpp}`, `grids/cross_call_operator_data_info.hpp` (new), `miscellaneous/cross_call_operator_data_cache/*` (renamed + 2 new), `data_contexts/deltas_sparse_grid_quadrature_index.{hpp,cpp}` (new), `memoryBytes`/`heapBytes` in `quadrature_weight_table`, both DSG interp indices, `closure_trie`, `ct_coefficient_trie`, `flat_nested_vector`; `tools/collections/vector_bytes.hpp`, `tools/operators/cross_call/*` (new); the 3 option headers (new); 6 operator headers + 4 operator cpps + 4 tools pairs; `sgpp_combigrid.hpp`; `combigrid.i`; `perf_micro.cpp`; tests below.

## Gain vs. cost
- Gain (feature, defaults = today): judged suite neutral — suite sum 0.999x (36.302 → 36.348 s, −46 ms from the noisy d2 L20 SG quadratures, CI ±20 %), `expensive/` 1.001x, cached 1.002x; no `total` case `SLOWER`; `interplin/dsg` d2 1.022x faster (serial path without libgomp loop calls). Repeated calls with the non-default flag (micro, 1 t / exploratory 16 t): SG quad cc d2 L20 on 1.13x / 1.19x (32 MiB), other SG quads 1.00x; DSG quad on 1.02–1.03x / 1.03–1.15x (0.4–2 MiB); DSG interp **off** d2 L12 0.83x / 0.46x, equi d3 0.97x / 0.85x, d7 / d10 1.00x; DSG interplin **off** d2 L16 ≈ 1/1000 (7.2 µs → 7.4 ms), d5 L8 0.55x / 0.20x, d10 L5 0.98x / 0.79x.
- Memory: geomean B/A 1.005, 0 `MEM+`/`MEM-` (largest `interplin/sg` d2 19.8 → 20.9 MiB, inside its spread 8.7 % / 11.4 %).
- API / functionality: agreed scope (options parameter on all DSG operators and both quadratures, clear / view, Python). Behavior change only by the bug fix (correct results for DSG `interpolateLinear` called from a user's parallel region).
- Maintainability: +~700 lines incl. tests and docs; per-grid data of every operator goes through one helper; one place to inspect memory.

## Tests
- Pinning (passed on the unchanged code): new `repeated_operator_calls/*` (all operators × grid × scalar/vector, repeated calls bitwise = fresh grid at 1 thread, round-off at 16 t; d1–4, b0/b1, shifted, cheb1 SG), existing `dsg_linear_interpolation/*`, `dsg_global_interpolation/*`, `operator_data_cache/*` (→ renamed), quadrature / interpolation pinning.
- New: `cross_call_operator_data_cache/*` (7: keys, throwing build, concurrent first calls, deletion, clear keeps held objects, entries / bytes, concurrent get + clear + view), `cross_call_operator_data/*` (7: defaults, on = one build shared by scalar/vector, off = no store and no read (build counter), bitwise on/off/first/later, clear → rebuild, `operator==` ignores the cache, concurrent calls + clears + views in an `omp parallel` region), `dsg_linear_interpolation/Calls_From_A_Parallel_Region_Equal_Serial_Calls` (fails on the old code). Copy semantics: grids are not copyable (const `unique_ptr`), nothing to define.
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ · TSan ✓ (`results/WP-73/tsan.sh`, std::threads: DSG interp / quad with clears + views, cache class alone; `tsan.txt`) · Python (`python_checks.py` 1 / 16 t, 11 examples) ✓. Note: `results/WP-50` / `WP-29d` `python_checks.py` are stale since WP-18 (`getTensorGrids`), unrelated.

## Reference check
- `perf.py refcheck` (1 / 16 t): 145 OK + the known WARN `interp/sg/equi/b0/d2/L5/cos-shifted` (identical in WP-73-before, since WP-06); no INFO (`results/WP-73/refcheck.txt`).

## Benchmarks (A = WP-73-before, B = WP-73-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; 59 common cases, sum 0.999x; full: `results/WP-73/micro.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/interplin/dsg/equi/b1/d2/L16 | 7.72 µs | 7.52 µs | 1.051 | [0.979, 1.129] |
| micro/interplin/dsg/equi/b1/d10/L5 | 88.03 ms | 88.43 ms | 1.000 | [0.985, 1.015] |
| micro/interp/dsg/cc/b0/d2/L12 | 5.29 ms | 5.28 ms | 1.001 | [0.998, 1.005] |
| micro/quad/operator/equi/b1/d20/L4 | 106.88 ms | 106.92 ms | 1.002 | [0.998, 1.006] |
| micro/crosscall/quad_sg/call/cc/b0/d2/L20 | 2.209 s | 2.207 s | 1.007 | [0.989, 1.025] |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | 190.56 ms | 191.29 ms | 0.994 | [0.984, 1.005] |

`SLOWER` only on untouched code (placement): `interp/sg/cheb1` 0.993, `crosscall/*/build` 0.980–0.991 (unchanged index / weight constructors). Inventory (build vs call, heap): `results/WP-73/inventory-before.json`, 16 t: `inventory-16t-raw.txt`.

L2/L3 e2e (16 threads, full suite incl. `expensive/`; `results/WP-73/e2e.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 160.42 ms | 156.81 ms | 1.022 | [1.009, 1.035] |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 271.58 ms | 271.85 ms | 1.029 | [0.932, 1.136] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.136 s | 1.137 s | 0.999 | [0.995, 1.002] |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 92.28 ms | 93.39 ms | 0.988 | [0.976, 0.999] |
| quad/sg/cc-uq/b0/d2/L20 | total | 452.95 ms | 457.51 ms | 0.905 | [0.719, 1.140] |

Sum of medians: suite A = 36.302 s, B = 36.348 s (0.999x) · `expensive/` 17.767 → 17.754 s · cached 1.044 → 1.042 s
Peak memory: geomean B/A = 1.005, 0 flags · process floor A = B = 10.4 MiB

**Concurrency variants** (`results/WP-73/concurrency.txt`, `concurrency_bench.cpp`; ns per lookup at 1 t / 16 threads looking up nonstop / µs per iteration of lookup + 7 µs work at 16 t): (a) mutex + scan + `shared_ptr` (built) 25 / 2853 / 7.41–7.65 · (a-rw) `shared_mutex` 22 / 2335 / 7.27–7.32 · (a-snap) `atomic<shared_ptr>` snapshot 39 / 8924 / 7.42–7.43 · (c) mutex + raw pointer (= before; clear / view would need "no concurrent calls") 18 / 1827 / 7.22–7.25 · (b) unsynchronized (single caller) 6 / 12 / 7.20. Real single-point calls, 16 concurrent `std::thread` callers, before → after (2 rounds): interplin d2 L16 12.2 / 11.9 → 11.8 / 11.6 µs, d10 L5 113 / 99.5 → 98.5 / 99.2 ms, interp equi d3 686 / 657 → 661 / 659 µs, cc d2 L12 8.78 / 7.89 → 7.94 / 8.05 ms: no measurable difference. Recommendation **(a)** (no inconsistency possible, cost ≤ 0.2–0.45 µs per call only under 16-way contention on calls of ≈ 7 µs); (b) would save at most that and drop the shared-first-call guarantee; (a-rw) is a cheap later refinement if such call rates matter.

## Attempts not kept
- none (variants (a-rw), (a-snap), (b), (c) measured in isolation only, see above).

## Profile shift / follow-ups
- Owner decisions: defaults (proposal: as built = today: quadratures off, DSG interpolations on; DSG quadrature "on" would give 1.03–1.15x on repeated calls at 16 t for 0.4–2 MiB), concurrency variant (a).
- WP-60 (DSG quadrature weight rows): its shared table becomes part of `DSGQuadIndex` (already cacheable). WP-07 does not merge in: SG `interpolate` stays without cached data (RP-4: generic weights ≈ 1 % of cheb1 d5); if WP-07 is done, its per-(dim, level) weights become a `GlobalInterpolationOptions`-controlled entry. WP-09 (multi-point) amortizes per-call data within one call, orthogonal.
- Adaptive refinement (WP-62 plan): a refinement step builds a new grid (immutable), so its cache starts empty — nothing to invalidate; a later in-place growing grid would need `clear` or keys with a grid version (the key string is there for that).
- Code-hygiene backlog: stale `results/WP-50` / `WP-29d` Python checks (`getTensorGrids`); `quadrature.hpp` / `quadrature_context.hpp` "TODO: Document" (one removed here).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
