# WP-28 re-check — disk tier after the owner's refactoring (1e279c1d0)

Status: PROPOSED (refactoring verified; 3 build bugs of the refactoring fixed minimally) · Date: 2026-11-02 · Base commit: 1e279c1d0
Recommendation: accept WP-28 together with the 3 fixes. Runtime is behavior-neutral: e2e 1.002x vs. WP-28-before, 0.999x for the refactoring alone, 0 SLOWER, disk numbers = summary. Two points for the owner: the 32 GiB default limit, and that LMDB/Boost now reach the public headers (findings 1–2).
Machine: `perf.py calibrate` ratios e2e 1.001 / 1.000, micro 0.998 / 1.001 (burst / sustained)

## Change
- Refactoring (owner, 73d6b3ac6..1e279c1d0):
  - files moved into `miscellaneous/caching/{boost_point_value_table,lmdb_point_value_store}/`;
  - the store's pimpl `Impl` became the header-visible `misc::lmdb_point_value_store::FileInterface` (+ `ReadSlot`, `FileHeader`);
  - `tools/caching/lmdb_tools` (`lmdbMessage`, `mapSize`, `directoryOf`);
  - `LmdbPointValueStore` → `LMDBPointValueStore`, `RecordLayout` → `misc::source_func_caching`, `CacheFileException` → `sgpp::combigrid` (`miscellaneous/exceptions/`);
  - format constants → `constants::lmdb_point_value_store`;
  - SCons `USE_LMDB`;
  - `perf_cache.hpp`: formatting + namespace only (same cases, same work).
- Fixes (bugs of the refactoring; evidence `results/WP-28/recheck/nolib-builds.txt`):
  1. `boost_point_value_table_shard.hpp`: namespaces opened inside `#ifdef`, stub outside → `-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE` did not compile. Each branch now opens and closes its own namespaces (owner's final variant; the stub is in `misc::boost_point_value_table`).
  2. `boost_point_value_table.cpp` stub constructor still used `LmdbPointValueStore` (same build). Renamed.
  3. `lmdb_point_value_store_file_interface.hpp`: the `#else` stub `FileInterface` was in the global namespace → `-DSGPP_COMBIGRID_DISABLE_LMDB_STORE` (and every build without LMDB) did not compile. Put into `misc::lmdb_point_value_store`.
  4. `cache_file_exception.hpp`: base `MessageHolder` in an anonymous namespace of a header → `-Wsubobject-linkage` in every TU, ODR violation. Named namespace `cache_file_exception` again (as at 73d6b3ac6).
- Also: stale paths in `results/WP-28/tsan.sh`; stale names in `CLAUDE.md` §6, `module-reference.md` (caching layout), and the doc comments of `source_function_cache.hpp` / `dependencies_defines.hpp`.
- Files: `boost_point_value_table_shard.hpp`, `boost_point_value_table.cpp`, `lmdb_point_value_store_file_interface.hpp`, `cache_file_exception.hpp`, `source_function_cache.hpp` (comment), `dependencies_defines.hpp` (comment), `CLAUDE.md`, `combigrid/claude/module-reference.md`, `results/WP-28/tsan.sh`.

## Findings for the owner (review of 73d6b3ac6..1e279c1d0; not changed)
1. **Default `storageSpaceLimit` changed: unlimited → 32 GiB** (`constants.hpp`).
   - The doc comment of `SourceFuncCacheConfig::storageSpaceLimit` still says "default unlimited", and decision 7 said unlimited.
   - Decide which one is wanted. With the limit, a 32 GiB map size is used even if the disk has more room.
2. **The pimpl was removed, so `source_function_cache.hpp` (umbrella, public) now includes `boost_point_value_table.hpp` → shard → `boost/unordered`, `dependencies_defines.hpp` (`xxhash.h`, `lmdb.h`) → `file_interface.hpp` (`lmdb.h`).**
   - This breaks the documented rule that only `.cpp` / internal files include `xxhash.h` / `lmdb.h`.
   - User TUs now need these headers.
   - The class layouts depend on the configuration defines: a user TU compiled without `-DSGPP_COMBIGRID_DISABLE_*` against a library built with them sees different `Shard` / `FileInterface` definitions (ODR).
   - The forward declarations were commented out (`// namespace misc {...}`) instead of removed.
   - SWIG is unaffected (it does not follow those includes; bindings rebuilt and checked).
3. **SCons:**
   - `detectLMDB` now also runs when `SG_COMBIGRID=0` (defines `SGPP_COMBIGRID_LMDB_LINKED` for every module; harmless).
   - New `USE_LMDB` (default 0): `USE_LMDB=1` without LMDB aborts the configure step. LMDB is still auto-enabled if found, so `USE_LMDB=0` cannot disable it.
   - `combigrid/claude/build-and-ci.md` does not mention `USE_LMDB`.
4. **No other semantic change:**
   - LMDB flags, the open checks (foreign / truncated / header / key = hash while loading), 64Ki-record transactions, the `MDB_KEYEXIST` collision count, full / `ENOSPC` handling, error collection (`keepError` / `rethrowError`) are token-identical;
   - so are the hook / `flush()` (+ `mdb_env_sync`) / destructor (no fsync), the `onDisk_` / spill / `collectUnflushed` paths, lock and `mkstemp` + `unlink` (`$TMPDIR`, else `/var/tmp`), and the `CacheFileException` copy semantics (own message);
   - `mi_vec` / generation thresholds in `constants.hpp` were only moved (same values) → no operator impact;
   - no type punning via pointer casts: the two `reinterpret_cast<const double*>` of `uint64_t` records (`boost_point_value_table.cpp:137` `insertLoaded`, `lmdb_point_value_store.cpp:253` `forEachRecord`; both already at 73d6b3ac6) are never dereferenced as `double`: the pointer goes only to `std::memcmp` (`RecordEqual::holds`) and `XXH3_64bits` (reads through `memcpy`), and every transfer between doubles and the `uint64_t` arena is `memcpy` → no strict-aliasing violation. Only a future `point[i]` on these pointers would be UB.
5. **Style / leftovers:**
   - "TODO: Document" on `FileInterface`, `ReadSlot`, `lmdb_tools`, `constants::lmdb_point_value_store`; "TODO: Cache design" in `cache_file_exception.hpp`;
   - `lmdb_point_value_store_file_header.hpp` has no `#pragma once` / `<cstdint>`;
   - `read_slot.hpp` uses `MDB_txn` without `<lmdb.h>`; `file_interface.hpp` lacks `<memory>`, `<mutex>`, `<string>`; `lmdb_tools.{hpp,cpp}` lack `<cstddef>` / `<algorithm>` (all compile via include order);
   - quoted include `"sgpp/base/exception/dependency_exception.hpp"` in the stub branch of `lmdb_point_value_store.cpp` (CLAUDE.md: angle brackets);
   - `using namespace` directives in two `.cpp` files (cpplint);
   - `tensor_grid.cpp:34` unused variable (pre-existing).

## Gain vs. cost
- Gain: none intended (refactoring). Feature numbers of WP-28 reproduced (disk table below).
- Memory:
  - e2e vs. WP-28-before: geomean 1.009, 2 MEM+ (`interp/sg/cc/b0/d10/L2/p8` 24.1 → 25.2 MiB, `quad/dsg/equi-exp/b0/d7/L10` 41.7 → 43.0 MiB; WP-28's 5-round A/B showed both at 1.037 / 1.015 without a flag, and with 3 rounds the non-overlap test is easier to meet).
  - Refactoring alone: geomean 0.999, 0 MEM±. Floors 10.4 / 10.5 MiB.
- API / functionality: none beyond agreed WP-28 (internal names only; bindings unaffected). Finding 1 is a default-value change of the owner.
- Maintainability: the store split adds 6 files.

## Tests
- Pinning: full suite on HEAD before the fixes (debug ×3 ✓).
- Results after the fixes:
  - debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓;
  - TSan (`tsan.sh`, 8 GiB cap, 8 configurations, 0 wrong; `recheck/tsan.txt`) ✓;
  - no-LMDB and no-Boost builds: full suite ✓, `dependency_exception` with the reason, 0 `mdb_` symbols without LMDB;
  - Python (`SG_PYTHON=1`): `python_checks.py` 28/28 at 1 / 16 threads, 10/10 examples ✓.
- No new tests: the uncovered refactored behavior is the no-library build, which the build matrix above covers. A build without LMDB detected was not built separately (it needs LMDB uninstalled; same preprocessor branches as `-DSGPP_COMBIGRID_DISABLE_LMDB_STORE`).
- Flaky `equidistant_nodes/sin_func_random_cube_3D`: no recurrence (≈ 12 full runs).

## Reference check
- `perf.py refcheck`: OK (104/104, 1 and 16 threads; `recheck/refcheck.txt`).

## Benchmarks (speedup = A/B, > 1 is faster)
- Owner's choice in-session: micro = the 30-case cross-section of WP-11/13/14 (5 rounds) instead of all 443 cases; e2e full suite with **3 rounds**.
- Snapshot `WP-28-refactor` = HEAD + fixes 1–4 (fixes 1–3 are not compiled in the judged build).

| comparison | cases | sum of medians | verdicts |
|---|---|---|---|
| e2e WP-28-before → refactor (`e2e.md`) | 38 | 38.385 → 38.304 s (1.002x) | 114 noise, 0 SLOWER |
| e2e checkpoint → refactor (`e2e-refactor.md`) | 38 | 38.316 → 38.350 s (0.999x) | 114 noise, 0 SLOWER |
| micro WP-28-before → refactor (`micro.md`) | 30 | 590.3 → 591.8 ms (0.998x) | 24 noise, 5 SLOWER (0.976–0.993), 1 faster |
| micro, those 6, checkpoint → refactor (`micro-flagged-refactor.md`) | 6 | 1.001x | 5 noise, `quadrule/cc_weights/n1025` 0.991 (also 0.991 SLOWER in WP-28's own A/B; untouched code → layout) |
| memory-only cache, checkpoint → refactor (`micro-cache-memory-refactor.md`) | 42 | 6.247 → 6.102 s (1.024x) | 42 noise |

Main e2e cases (total, 16 t, vs. WP-28-before): `quad/dsg/cc-uq/b0/d10/L7` 6.788 → 6.782 s (1.005), `quad/dsg/equi-exp/b1/d20/L6` 1.226 → 1.224 s, `interp/sg/cc/b0/d10/L2/p8` 1.984 → 1.966 s, `interplin/.../d10/L5/p32` 2.438 → 2.429 s (all noise).

Disk tier (ns per lookup, c1000 = 1 µs function; checkpoint / refactor; full table incl. c0 and primitives: `recheck/disk-table.md`):

| case, threads | uncached | memory cold | persistent cold | persistent warm | temp. spill | spill warm |
|---|---|---|---|---|---|---|
| quad cc d5 L4, 1 t | 954 / 957 | 259 / 259 | 330 / 333 | 109 / 108 | 1158 / 1165 | 539 / 541 |
| quad cc d5 L4, 16 t | 72 / 72 | 33 / 32 | 124 / 125 | 44 / 45 | 476 / 477 | 102 / 104 |
| quad equi d20 L3, 1 t | 936 / 937 | 708 / 694 | 956 / 956 | 299 / 300 | 3166 / 3235 | 624 / 635 |
| quad equi d20 L3, 16 t | 65 / 65 | 75 / 75 | 408 / 409 | 164 / 162 | 2237 / 2247 | 102 / 103 |
| interp cc d5 p8, 1 t | 1093 / 1096 | 123 / 123 | 136 / 135 | 106 / 104 | 493 / 485 | 481 / 475 |
| interp cc d5 p8, 16 t | 93 / 86 | 18 / 18 | 33 / 33 | 19 / 19 | 112 / 112 | 102 / 99 |

- Persistent warm, cc d5: 108 / 45 ns at 1 / 16 t (summary: 107 / 45).
- Disk primitives refactor / checkpoint 0.94–1.05 (single runs, no CI; the extremes are the 12–23 ns memory lookups).

## Attempts not kept
- none (verification only). Two micro A/Bs over all 443 / 827 cases were stopped unfinished (wrong case set, then owner's choice of a shorter plan); no results used.

## Profile shift / follow-ups
- Owner: findings 1–3 (default limit + doc; restore a pimpl / forward declarations so that public headers stay library-free; `USE_LMDB` semantics + build-and-ci.md).
- Leftover from WP-28: `/var/tmp/sgpp_perf_cache_*` of the `flat_*` cases (none found now).

## Revert
- Fixes only: `git checkout -- <the 4 source files>`; the whole WP-28: `python3 combigrid/perf/scripts/perf.py reject --apply` (as in the WP-28 summary).
