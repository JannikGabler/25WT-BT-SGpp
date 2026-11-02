# WP-40 — Caching: library-free public headers, leftovers, micro-suite compaction

Status: PROPOSED · Date: 2026-11-02 · Base commit: ef1e4aaf4
Recommendation: accept — the public headers no longer need Boost.Unordered, xxHash or LMDB, and their layouts no longer depend on the configuration. The cache hot path is bit-identical, e2e is neutral (0.998x, 0 flags), and the micro suite shrinks from 827 to 305 cases (build 27.0 → 17.7 s).
Machine: `perf.py calibrate` ratios e2e 0.999 / 0.999, micro 0.996 / 1.004 at start; 1.000 / 0.999, 0.997 / 0.998 before the A/Bs.

## Change
- Public headers (finding 2):
  - `source_function_cache.hpp` forward-declares `misc::BoostPointValueTable` / `misc::LMDBPointValueStore`, as at 73d6b3ac6.
  - `boost_point_value_table.hpp` forward-declares `boost_point_value_table::Shard` and the store.
  - `lmdb_point_value_store.hpp` forward-declares `FileInterface` (its pimpl again).
  - Only `.cpp` files include shard / file_interface / read_slot, i.e. the headers that use the libraries and test the macros.
  - Mechanism: forward declarations only. No new indirection: `shards_` / `interface_` were already `unique_ptr`s.
- Umbrella closure: 685 → 385 headers. Checked by the repeatable script `results/WP-40/check_public_headers.sh`, which runs 3 checks in 4 configurations:
  1. no `boost/unordered*`, `xxhash.h` or `lmdb.h`;
  2. no public header tests an `SGPP_COMBIGRID_*` configuration macro;
  3. the same closure with and without the `SGPP_COMBIGRID_DISABLE_*` flags.
  - Before: all 3 fail (`public-headers-before.txt`). After: all 3 pass.
- Leftovers (finding 5):
  - Doxygen for `FileInterface`, `ReadSlot`, `FileHeader`, `lmdb_tools`, `constants::lmdb_point_value_store`, plus banner / `@file` on probe / record_equal / record_hash / file_header.
  - Missing includes added (`<lmdb.h>`, `<memory>`, `<mutex>`, `<string>`, `<cstddef>`, `<algorithm>`, `<vector>`); quoted include → angle brackets.
  - `using namespace` → using-declarations in the 2 store `.cpp` files. Unused includes dropped (table header in `file_interface.cpp`; fcntl / sys/file / sys/stat in `lmdb_tools.cpp`).
  - `CacheFileException`: "TODO: Cache design" resolved as one type per file. The `MessageHolder` base is gone; the class has a `message_` member, and every constructor / assignment points `msg` at it. Same copy semantics, pinned by a new test.
  - `source_function_hook.hpp` deleted: fully commented out and included nowhere, not in the bindings either.
  - Correction to the prompt: `file_header.hpp` already had `#pragma once` / `<cstdint>`; only the banner and docs were missing.
- Micro suite (decision 2026-11-02):
  - Removed the WP-25/26 prototypes: oldproto, sharded_mutex, sharded_spin, sharded_oa_spin, boost_cfs, phmap, perthread, bypass, hashes wy / fnv_proto.
  - Removed all 267 `micro/vec/` cases (`perf_vector_proto.hpp` deleted). Its 4 helpers used by the WP-29 cases (`selectTGs`, `expSumK`, `cosProdK`, `compareResults`) moved into `perf_micro.cpp`.
  - `op/.../none` is now registered in `perf_cache.hpp` (same name, same timed work; counters from a `CachedSourceFunc`).
  - `FlatFile` deletes its file (pid in the name, `$SGPP_PERF_CACHE_DIR` like `BenchFile`, which now shares `diskDir()`); `/var/tmp` stays empty.
  - Kept references in `perf_cache_proto.hpp`: `sol`, `xxh3`, `ShardedTable` (timed in `flat_load`), `hashPoint`.
  - `perf_cache_counts.cpp` needed only `CacheStats::PerThread` + `hashPoint` (both kept); it runs.
  - Old and new case lists: `cases-{before,after}.txt` (no new names; 522 removed, all prototypes).
- Docs:
  - `build-and-ci.md`: `USE_LMDB` (GSL / ZLIB convention), Boost / xxHash / LMDB versions, `DISABLE_*` fallbacks, header rule + check.
  - `CLAUDE.md` §6: one sentence plus the check.
  - `module-reference.md`: caching layout.
  - `perf/README.md`: layout, micro composition, simpler classic filter `^micro/(?!cache/)(?!.*(_vec|_scalar_x_m)/)` (the old one is still needed for pre-WP-40 snapshots).
  - `tsan.sh`: no `.cpp` was added or removed, so its file list is unchanged.
- Files: the 17 `combigrid/src` files in `git status`; benchmarks `perf_cache.hpp`, `perf_cache_proto.hpp`, `perf_micro.cpp`, `perf_vector_proto.hpp` (deleted); docs above; new test `tests/miscellaneous/cache_file_exception_test.cpp`; scripts and reports in `results/WP-40/`.

## Gain vs. cost
- Gain:
  - Correctness / hygiene: user code needs none of the 3 libraries, and ODR violations between differently configured user code and the library are no longer possible.
  - No runtime gain intended. e2e is neutral: 38.382 → 38.466 s (0.998x), 114 / 114 within noise.
  - Benchmark maintenance: perf_micro 827 → 305 cases; build 27.0 → 17.7 s (compile 16.0 → 11.7 s, LTO link 11.0 → 5.9 s); binary 14.4 → 8.7 MB (stripped 1.13 → 0.64 MB); benchmark source −1852 lines net (vec proto 1462 deleted, cache proto 1380 → 943).
- Memory: geomean B/A 1.005, 0 MEM±. Largest: `quad/sg/equi-exp/b1/d20/L5` 57.3 → 59.5 MiB, a bimodal case with spread 12 / 13 %. Process floor 10.1 / 10.5 MiB (known range).
- API / functionality: none. Public classes, bindings and file format are unchanged; `CacheFileException` (internal, not bound) keeps its interface.
- Maintainability: fewer files, docs complete, the header rule is checked by script.

## Tests
- Pinning: full suite (debug ×3 ✓ on the unchanged code). The new `cache_file_exception/*` (3 cases) also passed on the unchanged code.
- After the change:
  - Debug ×3 ✓, normal ×6 ✓, ASan ✓ (8 GiB cap).
  - TSan (`tsan.sh`) ✓: 8 configurations, 0 wrong (`tsan.txt`).
  - DISABLE matrix (BOOST / LMDB / both; `nolib_matrix.sh`, `nolib-builds.txt`): build + full suite ✓. A user program compiled without the flags gets `dependency_exception` with the reason; 0 `mdb_` symbols without LMDB.
  - Python (`SG_PYTHON=1`): WP-26 checks all OK, WP-28 checks 14 / 14 at 1 / 16 threads, 10 / 10 examples ✓ (`python-checks.txt`).
- Flaky `equidistant_nodes/sin_func_random_cube_3D`: no recurrence in ≈ 16 full runs.

## Reference check
- `perf.py refcheck`: OK (104 / 104, 1 and 16 threads; `refcheck.txt`).

## Benchmarks (A = WP-40-before, B = WP-40-after; speedup = A/B, > 1 is faster)
Machine code (`compare_text.py`, `text-compare.txt`):
- 745 / 767 functions identical, including every cache lookup / insert function: `find`, `insert`, `hash`, `findOnDisk`, the `wrap` lambdas, Boost `table_core` / `table_locator`.
- Changed: the `CacheFileException` throw paths (store, `.cold` clones); plus LTO noise in `comp1DWeights` (register swap), `CTCoeffTrie` (+1 instruction) and `interpolateLinear` (+3).
- Therefore the full e2e suite was run.

| comparison | cases | geomean | sum of medians | verdicts |
|---|---|---|---|---|
| e2e, full suite, 5 rounds (`e2e.md`) | 38 | — | 38.382 → 38.466 s (0.998x) | 114 noise |
| micro classic cross-section (`micro-classic.md`) | 30 | 1.003 | 0.997x | 24 noise, 4 faster, 2 SLOWER |
| cache production, 1 t (`micro-cache-t1.md`) | 101 | 1.000 (op 1.002, disk 1.001, table 0.984) | 0.998x | 79 noise, 15 faster, 7 SLOWER |
| cache production, 16 t (`micro-cache-t16.md`) | 101 | 0.988 (op 1.002, disk 0.992) | 1.001x | 88 noise, 9 faster, 4 SLOWER |
| flagged cases, library only (`micro-flagged-lib-only.md`) | 9 | 1.007 | 0.999x | 8 noise, 1 faster |
| flagged cases, perf_micro binary only (`micro-flagged-bin-only.md`) | 9 | 0.974 | 0.984x | 4 SLOWER, 1 faster |
| table cases, library only, 16 t, 8 rounds (`micro-cache-t16-lib-only.md`) | 12 | — | 0.996x | 11 noise, 1 SLOWER |

- Main e2e cases (total, 16 t), all within noise:

  | case | A | B | speedup |
  |---|---|---|---|
  | `quad/dsg/cc-uq/b0/d10/L7` | 6.782 s | 6.819 s | 0.996 |
  | `quad/dsg/equi-exp/b1/d20/L6` | 1.228 s | 1.225 s | 1.006 |
  | `interp/sg/cc/b0/d10/L2/p8` | 1.971 s | 1.982 s | 1.012 |
  | `interplin/.../d10/L5/p32` | 2.486 s | 2.505 s | 0.977 |

- Flagged micro cases are code placement in the rebuilt **perf_micro binary**:
  - Example: `lookup/production/d2/n10000`, 1 t, 0.891 (516 → 579 µs per call, +1 ns per lookup).
  - Method: hybrid snapshot `WP-40-binA-libB` (before binary + after library, in `.work/snapshots`).
  - The library change alone is neutral (lookup d2/n1e4 1.019, d10/n1e6 1.040). The binary alone reproduces 0.886 / 0.942.
  - The plain `op/.../equi/b0/d2/L11/none/c0`, which runs no cache code, moves 1.058–1.10x faster.
- 16 t `insert/production/d20/n1e6`: 0.973 / 0.980 (+2 %) with identical code. The hot functions moved by exactly +0x80, so their offset mod 64 is unchanged (`hot-function-alignment.txt`). Read as placement noise; the n = 10⁴ cases at 16 t are bimodal (CIs [0.17, 2.7]).
- `micro/quad/process_all_tgs/equi/b1/d20/L4` (virtual hook): 0.990 SLOWER. Its code is identical in the library (`process_all_tgs` is not in the changed list); the same benchmark-binary placement as above.
- Future cache micro A/Bs compare against the new binary, so these placement offsets cancel.

## Attempts not kept
- none. One accidental `git rm --cached` of `perf_vector_proto.hpp` during editing was unstaged immediately; nothing is staged.

## Profile shift / follow-ups
- `.claude/skills/perf-work/SKILL.md` (master's file, uncommitted master edits) still says "≈ 830 cases … `micro/vec/`": update it to 305 / no `micro/vec/` (not changed by this session).
- `source_function.cpp` keeps commented-out `SourceFuncHook` lines (owner's code, left alone).
- cpplint: the deletion of a header made SCons re-lint everything. 1289 pre-existing messages (mostly `build/include_order` of `<sgpp/...>`, missing banners in the node getters) became visible in the debug log. The changed files have fewer messages than at HEAD.
- `tensor_grid.cpp:34` unused variable (pre-existing).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; `--keep combigrid/tests/miscellaneous/cache_file_exception_test.cpp` keeps the new test). The restored `source_function_hook.hpp` / `perf_vector_proto.hpp` come back from HEAD.
