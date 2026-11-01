# WP-28 — SourceFunc caching: disk tier (temporary / persistent storage, LMDB)

Status: PROPOSED · Date: 2026-11-01 · Base commit: ce575ac0d
Recommendation: accept — the agreed feature works in C++ and Python (bitwise = uncached, persistence across caches and SIGKILL, damaged files detected) and the judged suite is unchanged (e2e 38 cases 1.000x, 0 SLOWER, memory 1.004, refcheck OK); weigh the new build dependency (optional LMDB) and that disk writes cost µs per new point.
Machine: `perf.py calibrate` ratios e2e 1.001 / 0.999, micro 0.999 / 1.004 (burst / sustained)

## Owner's decisions in this session (details + data: `results/WP-28/decisions.md`)
1. `SourceFuncCacheConfig` (still an aggregate) + `policy` (`CachePolicy::MemoryOnly` default | `TemporaryStorage` | `PersistentStorage`; `Adaptive` left to WP-27), `file` (persistent: required, created or reused; temporary: must be empty, set by the cache), `storageSpaceLimit` (default unlimited = free disk space; reaching it warns once on `std::cerr`, further values are not stored). **Not** added (owner): function id, spill switch / directory, sync switch, mismatch switch. I recommended an optional `functionId` (a file of another function with the same d, m is used silently); owner kept it out → documented as `@warning`.
2. Storage policies cache **every** evaluation: memory (records + indices ≤ `memoryLimit`) first, the rest on disk; "like a database" → **LMDB** (owner: new libraries are fine; I recommended it over an own log + own disk index). Optional: SCons `detectLMDB` (CheckLibWithHeader) links it and defines `SGPP_COMBIGRID_LMDB_LINKED`; `dependencies_defines.hpp` combines it with `__has_include` (owner: build-config change agreed); CI linux-full + `liblmdb-dev`.
3. Flush synchronous at every end-of-operator hook (new values only); public `flush()` (persistent: + `fsync`; temporary: non-binding, writes the buffers); destructor writes like the hook (no fsync). Guarantees (owner): the persistent file never gets corrupted by program crashes; after an operator call of an uninterrupted run all its values are in the file; values of an interrupted call may be lost.
4. Mismatch / damage: always throw (`base::file_exception`). Temporary file unnamed (`mkstemp` + immediate `unlink`, `$TMPDIR` else `/var/tmp` — `/tmp` is tmpfs here), `file = /proc/self/fd/N`. Exclusive `flock`; `CacheStatistics` + `diskHits`, `diskEntries`, `loadedEntries`, `fileBytes`; `clear(deletePersistentFile = false)` (persistent + false: memory only, the file stays and is used).

## Change
- `misc::LmdbPointValueStore` (new, `miscellaneous/caching/lmdb_point_value_store.*`): one LMDB file (`MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOTLS | MDB_NORDAHEAD`), key = XXH3 of the point (`MDB_INTEGERKEY`), value = point + values (bitwise compare on lookup; a second point with the same 64-bit hash is not stored), sub-db `meta` = magic / version / key scheme / d / m; open checks foreign files, truncation (file size vs. last page), header, and every key = hash of its point while loading; one reset read transaction per shard (lookups in parallel); writes sorted, ≤ 64Ki records per transaction; `MDB_MAP_FULL` / `ENOSPC` → full + one warning; errors inside parallel regions are kept and thrown at the hook. Stub without LMDB (factories throw `dependency_exception`).
- `BoostPointValueTable`: optional store pointer; beyond a shard's memory share values go out of line (`spill`) to a per-shard buffer (256 records) written as one transaction when full; misses consult buffer + store only once the store holds points memory does not (`onDisk_`, relaxed load on the miss path, `[[unlikely]]`); `find` / the memory branch of `insert` are as in WP-26; `collectUnflushed` hands the hook the buffers and (persistent) the arena tails since the shard's flush mark (`Shard::flushed`, `Shard::pending` after the index: the lock's cache line is unchanged).
- `SourceFuncCache`: policy / file validation, opens or creates the store, loads a persistent file into memory up to the limit (rest stays on disk), `finishOperatorCall` writes + throws for write errors / points of another d (storage policies), `flush`, `clear(bool)`, destructor flush. `misc::CacheFileException` (new): base exceptions keep only the `const char*` — composed messages dangled (found by TSan).
- Files: `cache_policy.hpp` (new), `source_function_cache_config.hpp`, `cache_statistics.hpp`, `source_function_cache.{hpp,cpp}`, `boost_point_value_table{.hpp,.cpp,_shard.hpp}`, `lmdb_point_value_store.{hpp,cpp}` + `cache_file_exception.hpp` (new), `constants.hpp`, `dependencies_defines.hpp`, `sgpp_combigrid.hpp`, `site_scons/SGppConfigure.py`, `combigrid/SConscript`, `.github/workflows/ci.yml`, `CachedSourceFunc.i`, test, `perf_cache.hpp`, docs (CLAUDE.md §2/§6, module-reference.md, python-bindings.md).

## Gain vs. cost
- Gain (feature; `results/WP-28/disk-t{1,16}.txt`, `none-baseline.txt`; ns per lookup, 1 µs function c1000, A = uncached): quad cc d5 L4 1 t / 16 t: uncached 959 / 72, memory cold 261 / 32, **persistent cold 329 / 125, persistent warm (new process: load + operator) 107 / 45** (9.0x / 1.6x faster than uncached), temporary spill (memory 0) 1161 / 480, its warm run 537 / 102. quad equi d20 L3: uncached 939 / 71, persistent cold 958 / 403, warm 305 / 162 (3.1x / **0.44x**: the serial load, 0.27 µs per record, dominates). interp cc d5 p8: uncached 1094 / 87, persistent cold 135 / 33, warm 104 / 20 (10.5x / 4.4x).
- Disk primitives (1 t / 16 t, d5 | d20): new point incl. hook write into a new file 0.72 / 0.55 | 0.89 / 0.76 µs (memory 0.17 / 0.03); into a file of 1e6 points 1.53 / 1.40 | 3.23 / 3.04 µs; spill beyond memory 3.68 / 3.47 | 4.81 / 4.54 µs; disk hit 0.58 / 0.10 | 0.76 / 0.11 µs (memory 0.08 / 0.01 | 0.17 / 0.02); load 0.15 | 0.27 µs per record. **LMDB writes are serialized** (one writer) and random keys dirty about one 4 KiB page per record (`lmdbbatch.cpp`: 2.4–7.3 µs per record into 1e5–1e6 records, `MDB_WRITEMAP` 1.2–2.5 µs): the storage policies pay for functions ≳ 10 µs (16 t) / ≳ 1–3 µs (1 t), the spill path only for clearly more expensive ones.
- Judged suite: e2e full suite 38.324 → 38.323 s (1.000x, 37 noise, 1 faster = layout, 0 SLOWER); micro standard set (443 cases) sum 1.000x, 30 SLOWER / 28 faster within ±2.5 % (symmetric, code paths unchanged). Memory-only cache path (`micro-cache-memory-final.md`, 42 cases): sum 1.000x, 7 SLOWER (0.947–0.993; the two worst also in every variant, their identical-work twins 1.012 / 1.033 → layout), 4 faster.
- Memory: e2e geomean 1.004, 0 MEM±, largest `interplin/sg/equi/b1/d10/L5/p32` 13.0 → 13.8 MiB (spread case); process floor 10.3 → 10.5 MiB (liblmdb). Cache: memory accounting unchanged; storage policies + one 256-record buffer per shard beyond the limit; LMDB file 69–98 B per d5 record (page cache not counted).
- API / functionality (agreed): `CachePolicy`, config fields `policy`, `file`, `storageSpaceLimit`, `CacheStatistics` + 4 fields, `SourceFuncCache::flush()`, `clear(bool)`; new optional build dependency LMDB (SCons check + define, CI `liblmdb-dev`). Behaviour of MemoryOnly / plain `SourceFunc` unchanged.
- Maintainability: +≈ 1 000 library lines (store ≈ 560 incl. stub), +≈ 520 test lines, `perf_cache.hpp` +≈ 210; one more preprocessor-detected library.

## Tests
- Pinning (passed on the unchanged code, debug ×3 + normal): full suite incl. `source_function_cache`, `source_function_cache_availability`, `source_function_test`.
- New: `source_function_cache_availability/storage_policies_throw_exactly_without_lmdb`; suite `source_function_cache_storage` (11 cases): file vs. policy, bitwise = uncached for all problems × {temporary, persistent} × {limit 0, default}, unnamed temporary file gone also after SIGKILL, persistent round trip (scalar + vector, memory / disk), every finished operator call in the file after SIGKILL (+ `flush()` of direct evaluations), SIGKILL inside an operator → file valid, mismatch (m, d), foreign / truncated / bit-flipped / other-version file, lock, nothing left behind, storage limit (one warning), `clear(false/true)`, shared cache + 4 concurrent operator calls, spilling at 16 t.
- Results (final code): debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ · TSan (`tsan.sh`, 8 GiB cap, 8 policy/limit configurations incl. concurrent hooks, `tsan.txt`) ✓ — it found dangling `file_exception` messages (fixed) · no-library builds `-DSGPP_COMBIGRID_DISABLE_LMDB_STORE` and `…_BOOST_POINT_TABLE`: full suite ✓, `dependency_exception` names the reason, no `mdb_` symbols · Python (`python_checks.py`, 1 / 16 t, 28 checks) ✓, all 10 examples ✓.
- Pre-existing flaky test (not caused, no operator code changed): `equidistant_nodes/sin_func_random_cube_3D` (time-seeded, 2.8e-12 rel. vs. 1e-12 tolerance) failed once in ≈ 15 full runs, 0 / 300 alone.

## Reference check
- `perf.py refcheck`: OK (104/104, 1 and 16 threads; `refcheck.txt`).

## Benchmarks (A = WP-28-before, B = WP-28-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`, 443 cases, sum 110.762 → 110.714 s, 1.000x):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/process_all_tgs/equi/b1/d20/L4 | 109.25 ms | 108.46 ms | 1.007 | [1.002, 1.011] |
| dsg/process_delta_tgs/equi/b1/d20/L6/s256 | 55.07 ms | 54.89 ms | 1.001 | [0.997, 1.005] |
| interp/global_tg/cc/b0/d10/L2 | 46.64 ms | 46.64 ms | 1.000 | [0.986, 1.014] |
| sourcefunc/eval_normalized_in_place/d10 | 35.12 µs | 35.55 µs | 0.982 | [0.967, 0.998] (unchanged code) |

L2/L3 e2e (16 threads, full suite, `e2e.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.785 s | 6.785 s | 1.016 | [0.964, 1.071] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225 s | 1.225 s | 1.000 | [0.996, 1.004] |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.69 ms | 231.85 ms | 1.025 | [1.010, 1.039] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.994 s | 1.978 s | 1.004 | [0.995, 1.014] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.432 s | 2.428 s | 1.002 | [0.981, 1.022] |

Sum of medians (38 cases): A = 38.324 s, B = 38.323 s, saved = 0.001 s (1.000x) · Reports: `combigrid/perf/results/WP-28/`
Peak memory: geomean B/A = 1.004, 0 MEM±, largest `interplin/sg/equi/b1/d10/L5/p32` 13.0 → 13.8 MiB · floor A 10.3, B 10.5 MiB.
Note: e2e / micro / memory-cache A/Bs ran on the after-snapshot before the last fix (destructor without fsync); that path runs only with a disk store, which those benchmarks never create. The disk runs used the final snapshot.

## Attempts not kept
- `MDB_WRITEMAP`: spill / append 1.4–1.7x faster, but cold writes and loads slower and the file is preallocated (sparse) to the map size (= free disk by default) → dropped.
- First table variant (pending lookup in `find`, spill inline in `insert`): memory-only inserts 2.6–4.1 % slower, cold always-cases up to 4.8 % (`micro-cache-memory.md`) → moved out of line + `[[unlikely]]` (`-v2`, `-v3`, `-final`).
- Destructor with fsync: cold persistent runs 1.3–5x slower with 0.03–1.3 s outliers; contrary to decision 3 → fixed.

## Profile shift / follow-ups
- Write path: an own append-only log (25–67 ns per record measured, `iobench.cpp`) for the persistent hook, with LMDB (or own sorted index runs) only for points beyond memory, would remove the serialized µs-per-point LMDB writes; larger spill batches (4096: −30 %).
- Parallel load of persistent files (WP-25: 26–70 ns/record at 16 t vs. 0.15–0.27 µs serial now) — the d20 16-thread warm case is load-bound.
- WP-27: `CachePolicy::Adaptive` (spill / persist only above a measured cost); the policy enum is extensible.
- No function id (owner): a file of another function with the same d, m is used silently (documented).
- Leftovers: `/var/tmp/sgpp_perf_cache_{2,10,20}_1000000.bin` (300 MB, written by the WP-25 prototype cases `micro/cache/disk/flat_*`, which never delete them; deleting was not permitted in-session); `tensor_grid.cpp:34` unused variable (owner's tree).
- Owner files now outdated: `combigrid/claude/build-and-ci.md` (LMDB detection in `SGppConfigure.detectLMDB`, CI apt line), SG++ install docs (optional `liblmdb-dev`), PLAN.md / STATUS.md WP-28.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; reverts the tracked files incl. `CLAUDE.md`, `.github/workflows/ci.yml`, `site_scons/SGppConfigure.py`, `combigrid/SConscript`, and deletes the new files `cache_policy.hpp`, `cache_file_exception.hpp`, `lmdb_point_value_store.*`)
