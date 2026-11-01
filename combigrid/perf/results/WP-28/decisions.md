# WP-28 — disk tier: decisions for the owner (2026-11-01)

Measurements of this session (`iobench.cpp`, ext4 on the SATA SSD, page cache warm, 1 thread;
records 56 B = d5 m1, 176 B = d20 m1), next to WP-25 / WP-26:

| operation | cost |
|---|---|
| bulk append, no sync (64-chunk writes) | 25–28 ns/record (d5), 67–77 ns/record (d20); 2.0–2.6 GB/s |
| bulk append + one `fdatasync` | 188–253 ns/record (d5), 611–785 ns/record (d20); 220–300 MB/s |
| small flush per operator call (append k records + header `pwrite`), no sync | 13 µs (k = 10), 49 µs (k = 1000) |
| same with `fdatasync` | 2.8–9.1 ms per call |
| no new records | 0 (nothing written) |
| `O_TMPFILE` create / `mkstemp`+`unlink` / `flock` lock+unlock | 28 µs / 46 µs / 2.2 µs |
| load at creation (WP-25) | 114–153 ns/record (1 t), 26–70 ns/record (16 t) incl. insert |
| random disk hit `pread` (WP-25) | 1.4–1.6 µs (page cache), ~0.1 ms cold SSD → spill pays only for f ≳ 5 µs |
| memory record (WP-26) | 66 B/entry at d5 (incl. index); compact disk index ~16–19 B/entry |

Context: a 1 µs function at 16 threads costs 62 ns wall per new point → a synchronous no-sync
flush adds +40 % (d5) to a **cold** persistent run of a 1 µs function, +4 % at 10 µs; warm runs
write nothing. `/tmp` is **tmpfs** (RAM) on this machine (Ubuntu default since 25.10): spilling
there only moves memory.

## D1 Shape of the config fields (`SourceFuncCacheConfig` stays an aggregate)
- (A, rec.) `std::string persistentFile` (empty = no persistent tier), `std::string functionId`,
  `SpillPolicy spill = SpillPolicy::Stop` (`Stop` | `Disk`; WP-27 adds `Adaptive`),
  `std::string spillDirectory` (temporary tier; empty = `$TMPDIR`, else `/var/tmp`),
  `bool syncFile = false`, `FileMismatch onFileMismatch = FileMismatch::Throw`.
  The temporary tier exists only as the spill store (no persistent file + `Disk`); with a
  persistent file, that file is the spill store (no second file).
- (B) `DiskTier tier = None | Temporary | Persistent` + one `std::string diskPath` (file or
  directory by tier) + the rest as in A. One field less, but the path changes meaning and
  "temporary without spill" is a no-op.
- Python: `std::string` and `enum class` wrap without typemaps (`pysgpp.SpillPolicy_Disk`).

## D2 At the memory limit
- (a, rec. default) `Stop`: as today (no further values stored; the persistent file holds what
  memory holds).
- (b) `Disk`: further records go to the file (per-shard 64 KiB buffers at offsets reserved with
  one atomic add, written when full and at the hook); memory keeps a compact index (hash → file
  offset, ~16–19 B instead of 66 B at d5: ~3.5–4x more points per GiB). Disk hits `pread`
  (1.4–1.6 µs warm). Load: up to the limit into memory, the rest indexed.
- Both implemented, `Stop` default; WP-27 can pick `Disk` from the measured function cost.

## D3 Flush timing (persistent tier)
- (a, rec.) synchronous at every `finishOperatorCall` (only if there are new records): append
  the shard arena tails as one checksummed block (`writev` of the chunk segments), then update
  the committed size in the header. Cost above. `syncFile = true` adds `fdatasync`
  (power-loss safe; 2.8–9 ms per call: unusable for per-point interpolation loops → off by
  default; without it, a process crash loses nothing that the hook wrote, only an OS crash /
  power loss can).
- (b) background writer thread fed with full chunks during the operator (hides the 25–67
  ns/record; adds a thread per cache, errors surface one call late, destructor joins). Follow-up
  if cold persistent runs of cheap functions matter.
- (c) only at destruction / explicit `flush()`: violates "all values in the file at the end of
  the operator".
- Plus (rec.): the destructor flushes what direct `evaluate` calls (no operator, no hook)
  added, errors swallowed; public `SourceFuncCache::flush()` for explicit control (Python too).

## D4 Validation and mismatch
- Header: magic, format version, key scheme (bit patterns of user-domain points, XXH3-64),
  d, m, function id, committed end; data = blocks `[nRecords | XXH3 of the records | records]`.
- Checked at creation: magic / version / key scheme / function id / checksums of all
  committed blocks; at the first cached function (`attach`): m; at the first lookup: d.
- Bytes after the committed end (torn tail of a crash) are ignored and overwritten silently.
- On a mismatch or corruption (rec.) `Throw` (`base::file_exception`) by default, option
  `Overwrite` (discard the file, start empty). d is only known at the first lookup inside the
  operators' parallel region (cannot throw there): the operator call runs uncached for that d
  (existing rule: points of another d bypass) and the hook then throws (`Throw`) resp. clears
  cache + file (`Overwrite`, later calls cache). Alternative: a `nDim` config field to check d
  up front (rejected by me: redundant for the user).

## D5 Concurrency
- In a process: one `SourceFuncCache` per file; several functions / concurrent operator calls
  share it (flushes serialized by a mutex, tails snapshotted under the shard locks).
- Across caches / processes: (rec.) exclusive `flock(LOCK_EX | LOCK_NB)` at creation → a second
  cache on the same file throws `base::file_exception`. Options: blocking wait; read-only
  fallback (load, never write).

## D6 Misc semantics
- `clear()` also truncates the persistent file (cache = file) (rec.); option: keep the file.
- `CacheStatistics` (aggregate) gains `diskEntries` (indexed on disk only), `diskHits`,
  `loadedEntries`, `fileBytes` (rec.).
- Portability: POSIX `open/pread/pwrite/writev/flock/fdatasync` (macOS: `fsync`, `mkstemp` +
  `unlink`); Windows: disk fields throw `base::not_implemented_exception` at creation. Detection
  in `dependencies_defines.hpp` (`__has_include(<unistd.h>)`).
- Temporary file: `O_TMPFILE` (Linux), else `mkstemp` + immediate `unlink` → removed at any
  exit incl. SIGKILL; created at the first spill only.

## Impact
- WP-27: `SpillPolicy::Adaptive` (spill if the measured cost ≳ 5 µs); a cost estimate could be
  stored in the header later (version bump).
- Dimension-adaptive refinement: one persistent cache across steps / runs; every step writes
  only its new points (13–50 µs per call, 0 if none); restart after a crash keeps all finished
  steps.
- Python: fields as attributes; hits and flush never take the GIL.
- Judged suite: unaffected (plain `SourceFunc` unchanged; flush only in the cached hook).

---

# Owner's decisions (2026-11-01, in-session; supersede D1–D6 above)

LMDB measurements of this session (`lmdbbench.cpp`, 1e6 records, 8-byte hash keys, batches of
64k per transaction, `MDB_NOSYNC`): write 1669 / 369 ns per record unsorted / sorted (d5),
3281 / 484 (d20); lookup 0.79–0.93 µs (1 t), 54–62 ns wall (16 t); 69–98 / 206–289 B per record.

1. **Policy field** `policy`: `MemoryOnly` (as today: memory only, stop caching when full) |
   `TemporaryStorage` (memory + a temporary file deleted when the process ends; holds as many
   entries as the disk allows; `file` must be empty and is set by the cache) |
   `PersistentStorage` (`file` required; created if missing, else reused; kept after the
   process; `flush()` guarantees all values are on disk) | `Adaptive` (WP-27, not now).
   Fields `file`, `storageSpaceLimit` (per file; reaching it warns once and stops caching
   further values). **No** `functionId`, spill switch, spill directory, sync switch or
   mismatch switch (owner). My objection on `functionId` (a file of another function with the
   same d, m is silently used) was heard; the owner keeps it out → documented prominently.
2. **Every evaluation is cached somewhere** with the storage policies: memory (records +
   indices, all counted against `memoryLimit`) first, beyond it the disk store.
3. **Disk store = LMDB** (owner installs further libraries if needed): optional, SCons
   `CheckLibWithHeader` links `-llmdb` and defines a flag; `dependencies_defines.hpp` combines it
   with `__has_include(<lmdb.h>)`; without LMDB the storage policies throw
   `base::dependency_exception` (MemoryOnly works). CI linux-full: `liblmdb-dev`.
4. **Flush**: synchronous at every end-of-operator hook (new records only); `flush()` public
   (Persistent: all values on disk incl. `mdb_env_sync`; Temporary: non-binding); the
   destructor flushes. Guarantees (owner): the persistent file never gets corrupted, also on
   program crashes; after an operator call of an uninterrupted run all its evaluations are in
   the file; evaluations of an interrupted operator call may be lost.
5. **Mismatch / corruption**: always throw (`base::file_exception`); no overwrite option.
6. **Temporary file**: unnamed (created + unlinked immediately) in `$TMPDIR`, else `/var/tmp`;
   `file` = `/proc/self/fd/N` (Linux; `/dev/fd/N` elsewhere); created at cache construction.
7. Exclusive `flock` on the persistent file (a second cache on it throws); storage-limit warning
   = one message on `std::cerr` + `rejectedInserts`; `storageSpaceLimit` default unlimited, disk
   full handled like the limit; `CacheStatistics` + `diskEntries`, `diskHits`, `loadedEntries`,
   `fileBytes`.
8. `clear(deletePersistentFile = false)`: Persistent + false → memory and statistics only, the
   file is kept and still used; true → the file is emptied as well. No effect otherwise.
