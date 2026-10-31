# WP-26 — SourceFunc caching: in-memory cache, none / always, soft limit, end-of-operator hook, `pysgpp.CachedSourceFunc`

Status: PROPOSED · Date: 2026-10-31 · Base commit: 51a6207bf
Recommendation: accept — the agreed feature works in C++ and Python (bitwise = uncached, cross-operator hits on every domain); the judged suite is unchanged (e2e 1.000x, memory 1.006, refcheck OK).
Machine: `perf.py calibrate` ratios e2e 1.002 / 1.001, micro 1.007 / 1.007 (burst / sustained)

## Owner's decisions in this session (2026-10-31; data: `results/WP-26/decisions.md`)
1. API **B**: `CachedSourceFunc : SourceFunc`, `CachedVectorSourceFunc : VectorSourceFunc`; `final`, no data, no virtuals (slicing to `SourceFunc` keeps the caching); `cache()`, `statistics()`. (as recommended)
2. Configuration **both**: shared `std::make_shared<SourceFuncCache>(CacheConfig{…})` and `CachedSourceFunc(func[, CacheConfig])` with its own cache. (as recommended)
3. `CacheConfig` = `memoryLimit` only, **default 1 GiB**; the policy field comes with WP-27 (aggregate → source-compatible). (as recommended)
4. Vector variant on the **same cache class** (m fixed by the first cached function; another m throws). (as recommended)
5. **Boost.Unordered + XXH3** (owner; I recommended own open addressing + own hash = no dependency; own OA measured: = Boost at 1 t, 16 t lookups +8…24 %, inserts +10…15 %, +7 B/entry). XXH3 = **system header + `libxxhash-dev` in CI linux-full**. Missing libraries: first SCons detection + compiled-out classes; **revised by the owner after the first delivery**: detection only in a central header (`optional_libraries.hpp`: `__has_include` + version macros, opt-out `SGPP_COMBIGRID_DISABLE_CACHE`; no SCons code), cache classes **always compiled**, their constructors throw `base::not_implemented_exception` without the libraries (`SourceFuncCache::isAvailable()`; Python: `RuntimeError`).
6. **Pinned fused `std::fma`** for the unit → domain mapping of all operators (owner; I recommended pinned unfused). The owner first asked about "close enough" keys: rounded keys cost +2.7…5.0 ns/lookup but merge only probabilistically, fail near 0 and make 16-thread results nondeterministic; a true tolerance cannot be hashed → not taken. GCC 15: `__builtin_assoc_barrier` does not stop contraction in vectorized loops.
7. Location **combigrid**. NaN values are never stored; statistics are counted per shard under its lock (both accepted).

## Change
- Design W: a cached function is a `SourceFunc` / `VectorSourceFunc` whose stored callable asks `misc::PointValueTable` first (key = bit pattern of the user-domain point, XXH3); the cache (`SourceFuncCache`, `shared_ptr`) is also the new **end-of-operator hook** (`SourceFuncHook` member, null for plain functions; all 8 operator overloads call `finishOperatorCall()` once; WP-26: counts calls). Plain `SourceFunc::evaluate` unchanged.
- Table: 64 shards (hash bits 24..29), each `SpinLock` + statistics counters in one cache line, `RecordArena` (chunks of 128 records `[hash|x|f]`), `boost::unordered_flat_set<uint32_t>`; soft limit split per shard (stop inserting); d fixed at the first lookup (other d bypass), lazily created table.
- `tools::mapToDomain` (= `std::fma`) used by `denormalizeDataVector` (interpolation) and both quadratures' node mapping → all operators evaluate bitwise identical points.
- Optional libraries: `optional_libraries.hpp` defines `SGPP_COMBIGRID_HAS_CACHE_LIBRARIES` / `HAS_CACHE_LIBRARIES`; only `point_value_table.cpp` (real table vs. link stubs), the test (main suite vs. availability test) and `perf_cache.hpp` test it; public headers, umbrella and bindings are unconditional. Python: `CachedSourceFunc.i` (`CacheConfig`, `CacheStatistics`, `SourceFuncCache` as `%shared_ptr`, both cached classes registered under their base pointer → exception re-raise works); example `cached_func_full_sg_quadrature_interpolation.py`.
- Leftovers removed: `data_vector_hash.hpp` (copy kept in `perf_cache_proto.hpp` for the WP-25 cases), empty `constants::source_func`, `os.hpp`/umbrella comments (`USE_CACHE`, `CacheId` were already gone).
- Files: new `optional_libraries.hpp`, `functions/source_functions/{source_function_hook.hpp, caching/*}`, `miscellaneous/caching/{point_value_table.*, record_arena.hpp}`, `miscellaneous/concurrency/spin_lock.hpp`, `tests/.../cached_source_function_test.cpp`, `benchmarks/perf/perf_cache.hpp`, `build/pysgpp/CachedSourceFunc.i`; changed `source_function.*`, `vector_source_function.*`, 4 operator drivers, `normalization.*`, `quadrature_tools.cpp`, `deltas_sparse_grid_quadrature_tools.cpp`, `constants.hpp`, `sgpp_combigrid.hpp`, `SourceFunc.i`, `VectorSourceFunc.i`, `combigrid.i`, `perf_micro.cpp`, `perf_cache_proto.hpp`, `.github/workflows/ci.yml`, docs.

## Gain vs. cost
- Gain (feature; `cache-production.md`, `python-cache-timing.md`): C++ 1 µs function at 1 / 16 t: SG quad always 1.50–3.81x / 1.20–2.26x, warm 6.6–14.8x / 4.4–7.3x; interp 8.76x / 4.74x, warm via a quadrature on a **shifted** domain 11.7x / 6.0x (hit rate 1.000; WP-25: 44–77 % overlap); DSG always 0.89 / 0.78 (every point once). Cost-0 functions lose in quadrature (0.09–0.35x), as predicted. Python: cold quad 1.6–5.0x / 2.6–5.5x, interp 5.4–21x / 34–36x (1 / 16 t); warm 6.4–68x / **1309–2505x**, no Python call on hits.
- Production table vs. WP-25 prototype (same binary): lookups 1 t −14…+5 %, 16 t −16…+4 %; inserts 1 t up to 1.42x faster, 16 t within +5 %.
- Judged suite: e2e 38.595 → 38.602 s (1.000x); micro sum 1.000x (interp/interplin +0.8…6.2 % from one FMA instead of mul + add).
- Memory: e2e geomean 1.006, 0 `MEM±`; `SourceFunc` +16 B. Cache: 66 B/entry at d5 (1.97 M points = 124.4 MiB, heap 124.8); limit 16 MiB → 253 952 points, 15.9 MiB, hit rate 0.75 / 0.67 (1 / 16 t).
- API / functionality: new public classes + `finishOperatorCall()` (agreed); interpolation points on non-unit domains move ≤ 1 ulp (decision 6). New optional build dependencies (decision 5).
- Maintainability: ~1 000 lines library code (incl. docs), ~540 test lines; one `#if` in the library (`point_value_table.cpp`).

## Tests
- Pinning (passed on the unchanged code, debug ×3 + normal): full suite incl. `source_function_test`, `vector_source_function_test`, `interpolation_pinning_test`, `vector_interpolation_test`, `vector_quadrature_test`.
- `source_function_test` changed with decision 6: identical points of all operators now also on shifted domains; mapping pinned bitwise to `std::fma`.
- New `source_function_cache_availability` (constructors throw exactly without the libraries) and `source_function_cache` suite (11 cases): bitwise cached == uncached (scalar + vector, equi / CC / cheb1, b0 / b1, d 1–3, unit + shifted, cold + warm, 1 t), 16 t within round-off, cross-operator reuse, statistics + hook counts, limit (0 / partial), NaN, copies / slicing / shared cache, misuse, clear, concurrency.
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ · TSan (std::thread driver, 8 GiB cap, `tsan.txt`) ✓ · build with `-DSGPP_COMBIGRID_DISABLE_CACHE` (final code): full suite ✓, constructors throw (C++ / Python `RuntimeError`, 1 / 16 t) ✓, no Boost / XXH symbols in the library, example exits early ✓ · Python checks 1 / 16 t ✓ (+ WP-29d checks, all 10 examples ✓). Not run: Python checks under ASan.

## Reference check
- `perf.py refcheck`: OK (104/104 at 1 and 16 threads; `refcheck.txt`), also before the final counter variant. Quadrature results bitwise unchanged at 1 t (it was already contracted).

## Benchmarks (A = WP-26-before, B = WP-26-after-v0; operator kernels byte-identical to the final code — only the counter offset in the inlined hook moved, `lock incq 0x28` → `0x30`; cache hot path identical)
L1 micro (1 thread, `micro.md`, 21 cases, sum 1.000x, 0 SLOWER):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/process_all_tgs/equi/b1/d20/L4 | 107.32 ms | 108.32 ms | 0.990 | [0.978, 1.003] |
| dsg/process_delta_tgs/equi/b1/d20/L6/s256 | 55.05 ms | 55.33 ms | 0.997 | [0.989, 1.005] |
| interp/global_tg/cc/b0/d10/L2 | 47.19 ms | 46.57 ms | 1.019 | [1.005, 1.033] |
| interplin/tg/equi/b1/d10/L5 | 245.65 µs | 231.23 µs | 1.062 | [1.015, 1.111] |

L2/L3 e2e (16 threads, full suite, `e2e.md`; 35 noise, 2 faster, 1 SLOWER not reproduced):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.780 s | 6.769 s | 1.001 | [0.996, 1.007] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.227 s | 1.229 s | 1.000 | [0.994, 1.006] |
| quad/sg/equi-exp/b1/d20/L5 | total | 238.16 ms | 235.79 ms | 1.025 | [0.976, 1.077] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.989 s | 1.991 s | 0.996 | [0.991, 1.001] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.547 s | 2.507 s | 1.018 | [0.998, 1.039] |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.16 ms | 93.45 ms | 0.983 | [0.973, 0.993] |
| … same, `--rounds 8` (`e2e-dsg-d3-rounds8.md`) | total | 93.36 ms | 93.46 ms | 0.999 | [0.989, 1.009] |

Sum of medians (38 cases): A = 38.595 s, B = 38.602 s, saved = −0.006 s (1.000x) · Reports: `combigrid/perf/results/WP-26/`
Peak memory: geomean B/A = 1.006, 0 MEM±, largest `interp/sg/cc/b0/d10/L2/p8` 24.3 → 25.1 MiB (1.033) · process floor A = B = 10.2 MiB
The report's "result sanity" line (`quad/sg/equi-exp/b0/d5/L10` rel. error 8.3e-14 → 6.4e-13) is 16-thread reduction order: at 1 t A and B are bitwise equal (3.6765516269088203).

## Attempts not kept
- Shard counters after the index (first version): 16 t lookups 1.12–1.30x slower, warm operators 1.17–1.24x slower than counters in the lock's cache line (`variant-counters-t{1,16}.md`) → moved.
- Decision measurements kept as benchmark: `sharded_oa_spin` (own OA index) in `perf_cache_proto.hpp`.

## Profile shift / follow-ups
- WP-28: flush at `finishOperatorCall` (arena tails per shard); a file per cache; needs a function id in `CacheConfig`.
- WP-27: "off" per call needs a callable swap at the hook (operators get `const SourceFunc&`; races if one function runs in concurrent operator calls) or a per-evaluation flag (WP-25: 0.69–0.97x) → decide there; add the policy field.
- WP-17: `PointValueTable::find/insert(point, hash)` → `findBatch`/`insertBatch` by shard.
- Refinement: helpful — same cache across steps evaluates only new points; misses per call = new evaluations per step. Not blocking (per-MI contributions are an operator concern).
- Limits below 64 chunks (e.g. 320 KiB at d3) store nothing (documented); 0.4–1 % duplicate evaluations at 16 t on small grids (in-flight markers only for ≥ ms functions).
- Owner files now outdated: `combigrid/claude/build-and-ci.md` (optional Boost ≥ 1.81 / xxHash ≥ 0.8, `optional_libraries.hpp`, CI apt line), SG++ install docs, PLAN.md WP-26/27/28; pre-existing lint warnings in touched files (`quadrature.cpp` using-directive, `quadrature_tools.*` TODO / final newline) → WP-38.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; reverts the tracked files incl. `.github/workflows/ci.yml`, `CLAUDE.md`, restores `data_vector_hash.hpp`, deletes the new files under `combigrid/`)
