# WP-25 — `SourceFunc` caching: design exploration (decision data)

Status: PROPOSED · Date: 2026-10-30 · Base commit: 8118ea9a2
Recommendation: owner decision needed (API details deferred to WP-26–28, see "Open decisions") — adopt the **wrapper design W** (a cached `SourceFunc` is a plain `SourceFunc` whose callable looks the point up first; class, kernels and "none" unchanged = 0 cost), keys = bitwise user-domain points with one pinned domain mapping, a sharded in-memory table, "stop inserting" at the soft limit, a flat-file disk tier; the per-evaluation run-time branch costs 1.3 % on the suite (15/38 SLOWER)
Machine: `perf.py calibrate` ratios e2e 1.003 / 0.999, micro 1.003 / 1.002 (burst / sustained)

## Change
- Kept (benchmarks / tests only): `benchmarks/perf/perf_cache_proto.hpp` (292 `micro/cache/...` cases, registered in `perf_micro.cpp`, +4 lines): hashing, 7 table designs (removed prototype, sharded mutex / spin, Boost `concurrent_flat_set`, phmap, lock-free reference, per-thread), wrapper `makeCachedSourceFunc`, cost-controlled integrand (bitwise unchanged value), flat-file tier; `benchmarks/perf/perf_cache_counts.cpp` (gain bound of all e2e cases, 25 s, 1.1 GiB); `tests/functions/source_functions/source_function_test.cpp` (pins `SourceFunc` and the bit patterns the operators evaluate).
- `results/WP-25/`: `design-options.md` (**all answers with numbers**), `gain-bound.md`, `cache-tables.md` (+ `analyze_cache.py`, raw `cache-*.json`), `libs/` (TBB / LMDB / RocksDB / SQLite / HDF5 driver + `build.sh`, needs `-ltbb -llmdb -lrocksdb -lsqlite3 -lhdf5`; libraries installed by the owner today), `python_callable.py` + `python-callable.md` (scratch bindings with a C++ cache), `production-prototype.patch`.
- Measured and reverted: production prototype of the **run-time policy** (`cache` pointer in `SourceFunc`, `[[unlikely]]` test per evaluation, out-of-line cached path, end-of-operator hook in all 4 operators) = snapshot WP-25-after; libs rebuilt byte-identical to WP-25-before.
- Remaining hooks of the removed prototype: none in use; dead leftovers: `constants::source_func::USE_CACHE`, comments in `tools/os/os.hpp`, `type_defs.hpp` (`CacheId`), `data_vector_hash.hpp`, `sgpp_combigrid.hpp:18`. Outdated CLAUDE.md lines: 64 (`caching/` dir), 109 (`SourceFunc` "intended to cache … USE_CACHE"), 328 and 341 (USE_CACHE gotchas); likely 127 ("weights … cached per node distribution", cf. WP-07).

## Gain vs. cost
- Gain bound (evaluations / distinct points): quad/sg 2.8–13.6, **quad/dsg 1.00** (already every point once), interp 2.2–7.8 per call but **34–499 over the p calls**, interplin 2.6–26, mivec 1.5–5.5. E2e distinct sets reach 5e7 (SG) / 1.8e9 (DSG) points = 4–300 GB: the cheap e2e suite cannot profit.
- Integrand cost 0 (e2e integrands): every cache is slower (always 0.20–0.33x at 1 t, 0.10–0.18x at 16 t for SG; DSG 0.25 / 0.17). At 1 µs: always 1.5–3.8x (1 t) / 1.3–2.4x (16 t) of the bound 2.0–5.7; interp 9.1x / 4.1x; warm (cross-operator / persistent) 7–13x / 4–7.5x. **Break-even** (always): SG quadrature 120–310 ns (1 t), 270–630 ns (16 t); interp < 0 / 47 ns; warm 85–160 / 120–220 ns; DSG never within one call.
- **Python** (biggest win): a callable costs 0.66–5.3 µs (1 t) and **24–30 µs at 16 t** (GIL contention, 36x slower than 1 t). C++ cache in front: cold 1.9–5.1x (1 t) / 2.5–5.4x (16 t), warm 9–74x / **1925–2502x** (hits skip the GIL); a dict inside the Python callable: ≤ 1.27x at 16 t.
- Policy "none" as a run-time branch: micro 0.980x (7/14 SLOWER, −1.3…−3.0 % in the SG / DSG point loops), e2e 38.462 → 38.962 s (0.987x, −0.50 s), 15/38 SLOWER (all quadrature, 0.974–0.990), interp / interplin / mivec within noise → **not free**; W keeps today's code (0 by construction).
- Memory: prototype e2e geomean B/A 1.005, 0 MEM± (largest `interp/sg/cc/b0/d10/L2/p8` 23.8 → 24.3 MiB).
- API / functionality: none kept. Maintainability: kept benchmark 1.5k lines (proto 1249 + counts 287) (+~3 min per full `run micro`; drop with `reject` if unwanted).

## Recommended design (W) and answers
- **Key**: bitwise user-domain point (no operator change; valid across grids, domains, refinement steps, runs). Verified: shared points of nested families are bitwise identical across tensor grids and levels (12 recorded cases == DSG count), SG == DSG quadrature, global == multilinear interpolation. **Not** between quadrature and interpolation on non-unit domains (44–77 % overlap): `denormalizeDataVector` stays unfused (store may alias the domain), the quadrature node mappings are FMA-contracted → WP-26: one mapping helper with pinned (unfused) rounding, per TG node only (quadrature points move ≤ 1 ulp; functional at round-off, refcheck). Quantized keys: rejected (hits return f(x′), x′ ≠ x). Discrete keys: robust, smaller, but need operator integration + config-pinned files (a reduced-representation design). Hash: XXH3 3.6–18.6 ns (d 2–20); the removed `DataVectorHash` put **all keys into 1 of 256 shards** (low byte key-independent for dyadic coordinates).
- **In-memory**: one shared table (per-thread tables lose hits: 0.47 vs 0.82, 0.75 vs 0.97); 64 shards × spin lock + per-shard record arena + `boost::unordered_flat_set` of 4-byte ids: lookup 78 / 253 ns (1 t, n = 1e4 / 1e6, d = 10), 12 / 21 ns wall (16 t); inserts scale (59 ns wall, 16 t) where shared-arena designs stall (Boost cfs 237, phmap 202, TBB 187, removed prototype 2040 ns). Bytes/entry 8 (d + m + 1) + ~10 (42 / 107 / 187 at d = 2 / 10 / 20; 1 GB ≈ 25 / 9.5 / 5.4 M records). Library: Boost.Unordered (BSL-1.0, header-only; library-side new dependency: owner) or own open addressing (lock-free reference: lookups 22–44 % faster at 1 t, but no growth); with TBB headers present Boost needs `BOOST_UNORDERED_DISABLE_PARALLEL_ALGORITHMS`.
- **Policies**: none = plain `SourceFunc`; always = wrapper; adaptive = decided per operator call at the end-of-operator hook (timed first k misses vs. measured lookup cost and hit rate; "off" installs the raw callable — a per-evaluation bypass flag runs at 0.69–0.97x of "none"). Compile-time policies: not needed (saves 2–5 ns near break-even, doubles kernel code).
- **Limit**: per-shard accounting, limit split evenly; at the limit stop inserting (CT reuse is spread over the whole operator, LRU/CLOCK not worth a per-hit update; not simulated) or spill to the temporary tier.
- **Disk**: flat file (header: magic, version, key scheme, d, m, function id; count updated after the data = crash-safe tail) loaded at creation (26–70 ns/record at 16 t), new records = shard arena tails appended at the end of each operator (157–1569 ns/record incl. sync, 38–260 MB/s on this SSD). Temporary: `O_TMPFILE` / `mkstemp`+`unlink` / `FILE_FLAG_DELETE_ON_CLOSE`. Stores: LMDB lookup 0.7–1.2 µs (50–83 ns wall at 16 t), write 1–3.5 µs; RocksDB 3.6–4.4 µs (13–17 B/record compressed); SQLite 10–13 µs; HDF5 bulk only. Random disk hits (`pread` 1.4–1.6 µs) pay only for functions ≳ 5 µs. Memory–disk interplay (memory primary, write-behind at the hook, spill + compact index at the limit): `design-options.md` §11.
- **Interplay**: WP-29 records `{d, m}`, one lookup for m outputs, separate caches per function; WP-17 needs `findBatch` / `insertBatch` (misses as one batch, one GIL acquisition); WP-23 the cache can live in base (hit path ≥ 50 ns), `SourceFunc::evaluate` stays inline; WP-01 untouched (lookup inside the opaque call); dimension-adaptive refinement: not blocked — user-domain keys are step-independent, the hook can report per-call hits/misses.
- **Split / order**: (RP-3 → WP-29a–d →) **WP-26** wrapper + in-memory table + none/always + limit + hook + unified mapping + `pysgpp.CachedSourceFunc` (+ vector variant) → **WP-28** disk (persistent + temporary) → **WP-27** adaptive (smallest value: only matters for cheap functions) → WP-04 → WP-17.
- **Fixed by the measurements** (whatever the API): no per-evaluation check in a plain `SourceFunc` (caching lives in the stored callable); the cache is held through a `shared_ptr` (copies copy the callable); `SourceFunc` gets one member for the end-of-operator hook (null for plain functions, read once per operator call).
- **Open decisions (owner, deferred to the implementing WPs — decide at WP-26 unless noted)**:
  1. Construction API: (A) extra `SourceFunc(func, cache)` constructor · (B) thin subclass `CachedSourceFunc` without virtuals (explicit type, `cache()` / statistics accessors, maps to `pysgpp.CachedSourceFunc`) · (C) free factory `makeCachedSourceFunc(func, cache)` (as the prototype). Recommended: B.
  2. Configuration: a shared cache object (`std::make_shared<SourceFuncCache>(CacheConfig{…})`, shareable across functions / grids / operator calls, statistics, `flush()` / `clear()`) and/or a `CacheConfig` passed directly (the function creates its own cache). Recommended: both constructors.
  3. `CacheConfig` contents and policy representation: policy as a run-time field (always / adaptive; "none" = plain `SourceFunc`), memory limit, disk tier (none / temporary + directory / persistent + path), function id for the file header. Disk fields: decide at WP-28, adaptive fields at WP-27.
  4. Vector-valued variant: `CachedVectorSourceFunc` on the same cache class (m = run-time field of the record layout); after WP-29.
  5. In-memory index: Boost.Unordered (header-only, new library dependency) or own open addressing.
  6. Unify the domain mapping (quadrature points move ≤ 1 ulp on non-unit domains) for cross-operator hits between quadrature and interpolation.

## Tests
- Pinning tests (passed on the unchanged code, debug ×3 and normal): `quadrature_test`, `interpolation_test`, `delta_quadrature_test`, `tensor_grid_iterator_test`; new `source_function_test` (kept).
- Prototype test `source_function_cache_prototype_test` (cached == uncached bitwise for all 4 operators at 1 t, flush once per call, cross-operator hits): passed, removed with the prototype (in the patch).
- Results: debug `test --repeat 3` ✓ (before, prototype, final) · normal ✓ · ASan n.a. (no production code kept).

## Reference check
- `perf.py refcheck`: OK (104/104 at 1 and 16 threads) for WP-25-before and the prototype (`refcheck.txt`).

## Benchmarks (A = WP-25-before, B = WP-25-after = run-time-branch prototype; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`, 14 cases, sum 0.980x):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/process_all_tgs/cc/b1/d10/L5 | 52.37 ms | 54.08 ms | 0.970 | [0.959, 0.981] |
| quad/process_all_tgs/equi/b1/d20/L4 | 110.80 ms | 112.49 ms | 0.985 | [0.980, 0.990] |
| dsg/process_delta_tgs/equi/b0/d10/L6/s64 | 285.39 ms | 292.14 ms | 0.978 | [0.971, 0.985] |
| interp/global_tg/cc/b0/d5/L6 | 1.13 ms | 1.13 ms | 0.998 | [0.985, 1.011] |

L2/L3 e2e (16 threads, `e2e.md`, full suite):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.773 s | 6.891 s | 0.982 | [0.976, 0.988] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.418 s | 5.551 s | 0.975 | [0.967, 0.982] |
| quad/sg/equi-exp/b1/d15/L6 | total | 521.45 ms | 535.19 ms | 0.977 | [0.965, 0.989] |
| quad/sg/equi-exp/b1/d20/L5 | total | 238.88 ms | 243.75 ms | 0.975 | [0.962, 0.987] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.992 s | 1.989 s | 1.004 | [0.989, 1.020] |

Sum of medians (38 cases): A = 38.462 s, B = 38.962 s, saved = −0.500 s (0.987x) · Reports: `combigrid/perf/results/WP-25/`

Peak memory: geomean B/A = 1.005, 0 MEM± (largest B/A `interp/sg/cc/b0/d10/L2/p8` 23.8 → 24.3 MiB) · process floor A = 10.0, B = 10.2 MiB

## Attempts not kept
- Run-time policy branch in `SourceFunc::evaluate`: −1.3 % suite, −1.0…−2.6 % on 15 quadrature cases → W instead.
- Benchmark iterations: per-shard arena chunks of 4096 records (2.5 KB/entry at n = 1e4) → 128; lookups with cold query keys (+1 cache miss) → hot keys.

## Profile shift / follow-ups
- Python source functions are 36x slower at 16 than at 1 thread (GIL contention), independent of caching: run them serially or batch them (WP-17); new WP candidate.
- Cleanup of the prototype's leftovers (above; `DataVectorHash` unfit for power-of-two / mod-256 bucketing), with WP-31.
- WP-09 removes interpolation's p-fold repetition structurally (values per TG once for all points); a structural value store (discrete keys) belongs to a reduced-representation WP.
- Not measured: eviction hit rates under a limit (trace simulation), cold-SSD disk hits, libcuckoo (not packaged).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add
`--keep combigrid/benchmarks/perf/perf_cache_proto.hpp --keep combigrid/benchmarks/perf/perf_cache_counts.cpp --keep combigrid/benchmarks/perf/perf_micro.cpp --keep combigrid/tests/functions/source_functions/source_function_test.cpp` to keep the benchmarks and the pinning test)
