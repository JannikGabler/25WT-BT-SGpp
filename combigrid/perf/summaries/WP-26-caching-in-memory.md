# WP-26 — SourceFunc caching: in-memory cache, none / always, soft limit, end-of-operator hook, `pysgpp.CachedSourceFunc`

Status: PROPOSED · Date: 2026-10-31 (owner's refactoring reviewed 2026-11-01) · Base commit: 51a6207bf (owner's checkpoint a66876cda)
Recommendation: accept — the agreed feature works in C++ and Python (bitwise = uncached, cross-operator hits on every domain); the judged suite is unchanged within noise also after the owner's refactoring (e2e 1.001x, memory 1.002, refcheck OK); the owner should weigh the one cost of the virtual hook (below).
Machine: `perf.py calibrate` ratios e2e 1.002 / 1.001, micro 1.007 / 1.007 (burst / sustained); before the refactoring benchmarks 1.001 / 0.998, 0.998 / 1.001

## Owner's decisions in this session (2026-10-31; data: `results/WP-26/decisions.md`)
1. API **B**: `CachedSourceFunc : SourceFunc`, `CachedVectorSourceFunc : VectorSourceFunc`; `final`, no data, no virtuals (slicing to `SourceFunc` keeps the caching); `cache()`, `statistics()`. (as recommended)
2. Configuration **both**: shared `std::make_shared<SourceFuncCache>(CacheConfig{…})` and `CachedSourceFunc(func[, CacheConfig])` with its own cache. (as recommended)
3. `CacheConfig` = `memoryLimit` only, **default 1 GiB**; the policy field comes with WP-27 (aggregate → source-compatible). (as recommended)
4. Vector variant on the **same cache class** (m fixed by the first cached function; another m throws). (as recommended)
5. **Boost.Unordered + XXH3** (owner; I recommended own open addressing + own hash = no dependency; own OA measured: = Boost at 1 t, 16 t lookups +8…24 %, inserts +10…15 %, +7 B/entry). XXH3 = **system header + `libxxhash-dev` in CI linux-full**. Missing libraries: first SCons detection + compiled-out classes; **revised by the owner**: detection only in one central header (`__has_include` + version macros, no SCons code), cache classes **always compiled**, constructors throw without the libraries (final form after the owner's refactoring: `tools/defines/dependencies_defines.hpp`, `base::dependency_exception` naming the reason).
6. **Pinned fused `std::fma`** for the unit → domain mapping of all operators (owner; I recommended pinned unfused). The owner first asked about "close enough" keys: rounded keys cost +2.7…5.0 ns/lookup but merge only probabilistically, fail near 0 and make 16-thread results nondeterministic; a true tolerance cannot be hashed → not taken. GCC 15: `__builtin_assoc_barrier` does not stop contraction in vectorized loops.
7. Location **combigrid**. NaN values are never stored; statistics are counted per shard under its lock (both accepted).

## Change (final state, after the owner's refactoring)
- Design W: a cached function is a `SourceFunc` / `VectorSourceFunc` whose stored callable asks `misc::BoostPointValueTable` first (key = bit pattern of the user-domain point, XXH3) and calls the function on misses only. End-of-operator hook (owner's refactoring): **virtual** `finishOperatorCall()` of `SourceFunc` / `VectorSourceFunc` (empty), overridden by the cached classes (they hold the cache as `cache_`); all 8 operator overloads call it once. `SourceFunc::evaluate` stays non-virtual.
- Table (owner: split into `boost_point_value_table_*` headers): `SourceFuncCacheConfig::nShards` shards (power of two ≤ 64, default 64; hash bits 24..29), each `SpinLock` + counters in one cache line, `RecordArena` (chunks of 128 records `[hash|x|f]`), `boost::unordered_flat_set<uint32_t>`; soft limit split per shard (stop inserting); d fixed at the first lookup (other d bypass).
- `tools::mapToDomain` (= `std::fma`) used by `denormalizeDataVector` (interpolation) and both quadratures' node mapping → all operators evaluate bitwise identical points.
- Optional libraries: `tools/defines/dependencies_defines.hpp` (owner) defines `SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE` or a reason string; opt-out `-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE`. Tested only in `boost_point_value_table.cpp` / `_shard.hpp` (real table vs. link stubs), `source_function_cache.cpp` (constructor throws `base::dependency_exception`), the test and `perf_cache.hpp`; public headers, umbrella and bindings unconditional. Python: `CachedSourceFunc.i` (`SourceFuncCacheConfig`, `CacheStatistics`, `SourceFuncCache` as `%shared_ptr`, cached classes registered under their base pointer → exceptions re-raised); example `cached_func_full_sg_quadrature_interpolation.py`.
- Leftovers removed: `data_vector_hash.hpp` (copy in `perf_cache_proto.hpp`), empty `constants::source_func`, `tools/os/os.hpp` (owner), stale comments.

## Review of the owner's refactoring (2026-11-01; fixed by me, all in the tree)
- Fixed: `xxhash.h` was included inside `namespace sgpp::combigrid::tools` (its declarations landed in the namespace) → preprocessor-only header; Boost included unconditionally in `_shard.hpp` (no-library build could not compile) → under the macro; `#else` stubs did not match the new class (nested `Shard`, constructor arity, unqualified `RecordLayout`); **`CachedVectorSourceFunc::cache_` never initialized** (null dereference at the end of every vector operator call); `checkShardsCnt` never called (nShards = 3 or 0 accepted → wrong masks / division by zero) → checked (also 0) in the `SourceFuncCache` constructor; without the libraries the only throw was in the lazily created table, i.e. at the first lookup **inside the operators' OpenMP regions (std::terminate)** → `SourceFuncCache` constructor throws `dependency_exception` again; quoted includes, `<sys/types.h>`, missing `<memory>`/`<vector>`/`<type_traits>`, missing final newlines, stale `@file` / docs (64 shards, `isAvailable`, `not_implemented_exception`, "no virtuals").
- Semantics changed by the refactoring (kept, documented): `isAvailable()` removed (Python example / checks now try to create a cache); misuse throws `std::invalid_argument` (Python `RuntimeError`); `SourceFunc` / `VectorSourceFunc` polymorphic. Slicing (a copy into the base class would keep the cached callable but lose `finishOperatorCall`, e.g. no WP-28 flush) is **rejected at compile time** (owner's request): deleted converting constructor / assignment constrained on `tools::StrictSubclassOf` (new concept header); copy / move of the classes themselves explicitly defaulted (the owner's virtual destructor would otherwise suppress the moves); all hidden from SWIG (`#ifndef SWIG`; a declared destructor made SWIG warn 302 about the `%extend` destructors). Python: `SourceFunc(cachedFunc)` → `TypeError`. Measured `WP-26-after-r` → `WP-26-final` (this change + the owner's virtual destructor): micro 24 cases 0.997x, all noise; e2e full suite 38.349 → 38.314 s (1.001x, 0 SLOWER), memory 1.003, 0 MEM± (`noslice-*.md`); refcheck OK, tests ✓.
- Cost of the virtual hook: the vtable pointer moves the `std::function` by 8 bytes → `SourceFunc::evaluate` = load + `add $8` + jump instead of one jump, kernels re-allocated registers. e2e full suite `WP-26-before` → `WP-26-after-r` 38.379 → 38.348 s (1.001x, 34 noise, 2 faster, 2 SLOWER: `mivec/…/d20` not reproduced with 8 rounds, `quad/dsg/equi-exp/b0/d3/L16` 0.982 / 0.991 with 8 rounds); micro sum 1.000x, `quad/process_all_tgs/equi/b1/d20/L4` 0.981 [0.978, 0.984] with 8 rounds (0.990 = noise before the refactoring). Memory geomean 1.002, 0 MEM±. A non-virtual hook member (the first version) avoids this.
- Cache path vs. the first version (`WP-26-after`): 1 t sum 0.994x (warm, all-hit cases 1–3.5 % slower: run-time `nShards` mask), 16 t sum 1.000x (noise).
- Dead files left for the owner: `optional_libraries.hpp`, `source_function_hook.hpp` (fully commented out, no longer included).

## Gain vs. cost
- Gain (feature; `cache-production.md`, `python-cache-timing.md`): C++ 1 µs function at 1 / 16 t: SG quad always 1.50–3.81x / 1.20–2.26x, warm 6.6–14.8x / 4.4–7.3x; interp 8.76x / 4.74x, warm via a quadrature on a **shifted** domain 11.7x / 6.0x (hit rate 1.000; WP-25: 44–77 % overlap); DSG always 0.89 / 0.78 (every point once). Cost-0 functions lose in quadrature (0.09–0.35x), as predicted. Python: cold quad 1.6–5.0x / 2.6–5.5x, interp 5.4–21x / 34–36x (1 / 16 t); warm 6.4–68x / **1309–2505x**, no Python call on hits.
- Production table vs. WP-25 prototype (same binary): lookups 1 t −14…+5 %, 16 t −16…+4 %; inserts 1 t up to 1.42x faster, 16 t within +5 %.
- Judged suite: first version e2e 38.595 → 38.602 s (1.000x), after the refactoring 38.379 → 38.348 s (1.001x); micro sum 1.000x both (interp/interplin +0.8…8 % from one FMA instead of mul + add).
- Memory: e2e geomean 1.006 / 1.002 (refactored), 0 `MEM±`; `SourceFunc` +8 B (vtable pointer). Cache: 66 B/entry at d5 (1.97 M points = 124.4 MiB, heap 124.8); limit 16 MiB → 253 952 points, 15.9 MiB, hit rate 0.75 / 0.67 (1 / 16 t).
- API / functionality: new public classes + `finishOperatorCall()` (agreed); interpolation points on non-unit domains move ≤ 1 ulp (decision 6). New optional build dependencies (decision 5).
- Maintainability: ~1 100 lines library code (incl. docs, 9 table files after the owner's split), ~560 test lines; library `#if`s in `boost_point_value_table.cpp`, `_shard.hpp`, `source_function_cache.cpp`.

## Tests
- Pinning (passed on the unchanged code, debug ×3 + normal): full suite incl. `source_function_test`, `vector_source_function_test`, `interpolation_pinning_test`, `vector_interpolation_test`, `vector_quadrature_test`.
- `source_function_test` changed with decision 6: identical points of all operators now also on shifted domains; mapping pinned bitwise to `std::fma`.
- New `source_function_cache_availability` (constructors throw exactly without the libraries) and `source_function_cache` suite (12 cases, + `any_shard_count_gives_the_same_results`, invalid `nShards`): bitwise cached == uncached (scalar + vector, equi / CC / cheb1, b0 / b1, d 1–3, unit + shifted, cold + warm, 1 t), 16 t within round-off, cross-operator reuse, statistics + hook counts, limit (0 / partial), NaN, copies / slicing (sliced copy: no hook) / shared cache, misuse, clear, concurrency.
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ · TSan (std::thread driver, 8 GiB cap, `tsan.txt`) ✓ · build with `-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE` (refactored code): full suite ✓, constructors throw `dependency_exception` naming the reason (C++ / Python `RuntimeError`, 1 / 16 t) ✓, no Boost / XXH symbols in the library, example exits early ✓ · Python checks 1 / 16 t ✓ (+ WP-29d checks, all 10 examples ✓; the cache example's 1e-13 absolute check was flaky at 16 t (1 in 7 runs) → relative 1e-12, 0 / 30 failures). All results above re-run on the refactored code. Not run: Python checks under ASan.

## Reference check
- `perf.py refcheck`: OK (104/104 at 1 and 16 threads; `refcheck.txt`), for the first version and the refactored code. Quadrature results bitwise unchanged at 1 t (it was already contracted).

## Benchmarks of the first version (A = WP-26-before, B = WP-26-after-v0; refactored code: see the review section, `refactor-*.md`)
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
