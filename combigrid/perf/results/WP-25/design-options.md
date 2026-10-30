# WP-25 — `SourceFunc` caching: design options with measured costs

Decision data for the owner (summary: `combigrid/perf/summaries/WP-25-caching-exploration.md`).
All numbers: judged build (`perf.py build normal`), boost off, 1 thread = `OMP_PLACES={4}`,
16 threads = `OMP_PLACES=threads`. Sources: `gain-bound.md` (counts), `cache-tables.md`
(from `cache-t{1,16}.json`, `cache-lookup-t{1,16}.json`), `libs-*.jsonl` (TBB / LMDB / RocksDB /
SQLite / HDF5, `libs/`), `python-callable.md`, `micro.md` / `e2e.md` (production prototype).
"ns wall" at 16 threads = wall time per item; the cost per thread is 16x that.

## 1. Gain bound (`gain-bound.md`, all e2e configurations)

| family | evaluations / distinct points | remark |
|---|---|---|
| quad/sg (13 cases) | 2.83 (d2) · 5.37 (cc d3) · 11.06 (d5) · 13.62 (d7) · 9.87 (d10) · 6.37 / 4.54 / 3.34 (b1 d10 / d15 / d20) | duplicates of the CT |
| quad/dsg (9) | 1.00 | the DSG already evaluates every point once (= a reduced representation) |
| interp (8) | per call 2.15–7.79; over the p calls 33.7–499 | every call re-evaluates all grid points |
| interplin (4) | over the p calls 2.57–26.2 | also evaluates domain-boundary points (no grid points) |
| mivec (4) | 1.52–5.54 | |

- Distinct points of the e2e cases: up to 5.0e7 (SG) and 1.8e9 (DSG): 4–300 GB of records,
  far beyond a 1 GB limit. Cheap integrands only reach such sizes; an expensive function
  (>= 1 µs) affords <= ~1e7 points. The e2e suite therefore cannot profit and must not regress.
- Reduced representations (Tasmanian / SGMK) evaluate each point once: = the "distinct" column.
  For quadrature the DSG already does it; for interpolation WP-09 (multi-point API: values per
  tensor grid evaluated once for all points) removes the p-fold part without a hash table.

## 2. Key

| option | cross-operator | cost | remark |
|---|---|---|---|
| **bitwise user-domain point** (what the user function receives) | yes on unit domains; **no between quadrature and interpolation on non-unit domains** (44–77 % of the interpolation points are bitwise in the SG-quadrature set) | hash 6–22 ns (d 2–20) | no operator change; valid across grids, domains, node families, refinement steps, runs |
| bitwise unit-domain point | yes | as above | operators must pass the unit point (quadrature maps nodes once per TG → extra per-point work); persistent files must pin the domain |
| quantized point | merges ulp neighbours only probabilistically | +1 rounding per coordinate | a hit returns f(x') with x' != x: cached != uncached bitwise; not recommended |
| discrete (node family, level, index per dim) | yes | 2–4 B per dim | operator integration + config pinned in the file; enables a structural value store (no hashing) — a reduced-representation design, not a cache |

Verified bitwise properties (pinned by the new `source_function_test`): shared points of the
nested families (equi, CC; b0 / b1) are identical across tensor grids and levels (recorded
distinct == DSG point count in all 12 recorded SG / mivec cases; the DSG evaluates each point
once in its 3 recorded cases); SG quadrature == DSG quadrature point sets on all 10 shifted-
domain grids too; global == multilinear interpolation. **Cause of the non-unit mismatch**:
`tools::denormalizeDataVector` (`p *= w; p += a`) stays unfused (`vmulsd` + `vaddsd`: the
store to `point[dim]` may alias `area[dim].first`), the quadrature node mappings
(`nodes * width + lower`, `mapNodesOntoDomain`, `prepareDeltaTensorGrid`) are contracted to
`vfmadd`. The comment "same arithmetic as tools::denormalizeDataVector" is not bitwise true in
the judged build. Fix for WP-26 (owner decision, functional at round-off level): one mapping
helper for all operators with a pinned rounding (unfused: keeps the interpolation points, so
the Lebesgue-1e16 refcheck case is untouched; quadrature points move by <= 1 ulp; e.g.
`__builtin_assoc_barrier` or a no-contract attribute, per TG node only → no cost; to verify).

Hash: XXH3 (BSD-2, header-only) 3.6 / 8.3 / 10.7 / 18.6 ns at d = 2 / 5 / 10 / 20; own wyhash-
style 6.4 / 8.5 / 15.8 / 21.7; the removed prototype's `misc::DataVectorHash` 3.8 / 8.5 / 14.2 /
31.4 ns and **its low byte is independent of the key for dyadic coordinates: all keys landed in
one of its 256 shards** (imbalance 256.0).

## 3. In-memory structures (records `[hash | d keys | m values]` in arenas, index = 4-byte ids)

Lookup with a hot query point, ns (1 t n=1e4 / 1e6; 16 t wall n=1e4 / 1e6), d = 10:

| table | 1 t | 16 t wall | insert 16 t wall (n=1e6) | bytes/entry n=1e6 (d 2/10/20) | dependency |
|---|---|---|---|---|---|
| removed prototype (256 × mutex + `unordered_map<DataVector>`) | 164 / 546 | 547 / 1187 | 2040 | 124 / 188 / 268 | – |
| sharded, `std::mutex` (64 shards, own arenas, `boost::unordered_flat_set`) | 84 / 263 | 24 / 42 | 74 | 42 / 107 / 187 | Boost.Unordered (BSL-1.0, header-only) |
| **sharded, spin lock** (same) | 78 / 253 | 12 / 21 | **59** | 42 / 107 / 187 | Boost.Unordered |
| `boost::concurrent_flat_set` + shared arena | 80 / 257 | **10 / 18** | 237 | 44 / 108 / 188 | Boost.Unordered |
| `phmap::parallel_flat_hash_set` (64 × mutex) + shared arena | 91 / 270 | 26 / 50 | 202 | 43 / 107 / 188 | phmap (Apache-2.0, header-only) |
| oneTBB `concurrent_hash_map` + shared arena | 98 / 318 | 14 / 34 | 187 | (own allocator, not counted) | oneTBB (Apache-2.0, `-ltbb`) |
| lock-free fixed capacity (speed of light, no growth) | 61 / 143 | 6 / 16 | 77 | 49 / 114 / 194 | – |

- Inserts at 16 threads: every design with one shared arena (`fetch_add` per record) stalls at
  150–240 ns wall; per-shard arenas scale (24–75 ns). A shared-table design needs per-thread or
  per-shard record blocks.
- Per-thread tables (no sharing) lose hits at 16 threads: quad cc 0.47 vs 0.82, interp 0.75 vs
  0.97 → one shared table per cache. Concurrent first evaluations of the same point: 0.2 %
  extra calls (Python, 16 t) → no in-flight markers needed (option for >= ms functions).
- Boost caveat found here: with TBB headers installed, Boost's concurrent containers include
  `<execution>`, whose libstdc++ backend then needs `-ltbb` → define
  `BOOST_UNORDERED_DISABLE_PARALLEL_ALGORITHMS`.
- Bytes per entry = 8 (d + m + 1) + ~10 B (index + chunk slack); 1 GB holds 25 M (d = 2),
  9.5 M (d = 10), 5.4 M (d = 20) scalar records. Node-based maps pay a node each (base's
  64-byte-aligned `operator new`): removed prototype +80–90 B.

## 4. Operators with a cached function (wrapper design; `cache-tables.md`)

Speedup none / cached at integrand cost 0 (e2e integrands) and 1 µs, sharded spin table:

| operator (evals / distinct) | always, c = 0 (1 t / 16 t) | always, 1 µs | warm (all hits), 1 µs | break-even c (always / warm), ns 1 t / 16 t |
|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 (5.73) | 0.33 / 0.18 | 3.79 / 2.39 | 13.3 / 6.0 | 118 / 268 · 85 / 141 |
| quad/sg/equi/b1/d20/L3 (2.00) | 0.23 / 0.15 | 1.51 / 1.26 | 7.1 / 4.2 | 310 / 628 · 160 / 217 |
| quad/sg/equi/b0/d2/L11 (2.73) | 0.20 / 0.10 | 2.02 / 1.45 | 12.5 / 7.5 | 197 / 428 · 111 / 123 |
| quad/sg/equi/b1/d10/L4 (2.70) | 0.20 / 0.13 | 1.97 / 1.59 | 9.7 / 5.3 | 216 / 439 · 129 / 169 |
| quad/dsg/cc/b0/d5/L5 (1.00) | 0.25 / 0.17 | 0.88 / 0.78 | 10.5 / 5.2 | never · 105 / 146 |
| interp/sg/cc/b0/d5/L3/p8 (36.6) | 1.19 / 0.82 | 9.10 / 4.14 | 11.7 / 6.2 | <0 / 47 · <0 / 33 |
| interplin/sg/equi/b1/d8/L3/p8 (4.43) | 1.09 / 0.78 | 2.96 / 1.89 | 10.2 / 6.0 | <0 / 183 · <0 / <0 |

- Hit rates at 16 threads equal the 1-thread ones (0.82 / 0.50 / 0.63 / 0.63 / 0 / 0.97 / 0.77).
- Bitwise: cached == uncached for every operator at 1 thread (prototype test; counter `bitwise_equal_or_1thread_only`).
- Cost model: always pays when `c_f > (r c_lookup + c_insert) / (r - 1)`; with r >= 2 that is
  ~0.1–0.6 µs here. Python callables cost 0.66–5.3 µs (1 t) and 24–30 µs (16 t) → always above.

## 5. Policy mechanism (production prototype, `production-prototype.patch`)

| option | "none" cost | measured |
|---|---|---|
| **wrapper** (cached `SourceFunc` = a `SourceFunc` whose callable looks up first; today's class and kernels unchanged) | 0 (code unchanged) | cached path: +1 indirect call per evaluation |
| run-time branch per evaluation (`cache` pointer in `SourceFunc`, `[[unlikely]]` test in `evaluate`) + per-operator hook | load + test per evaluation | micro 0.980x sum, 7/14 SLOWER (-1.3…-3.0 %, SG / DSG point loops); e2e see summary |
| compile-time policy (kernels templated on it, e.g. with WP-29's value-type parameter) | 0 | not built: 2x code size per kernel; saves the indirect call (2–5 ns) near break-even only |
| per batch (WP-17) | per batch | needs WP-17's batch interface |

Adaptive "decided not to cache" through the wrapper (bypass flag): speed 0.83–0.97x of "none" at 1 t,
0.69–0.98x at 16 t vs "none" at cost 0 — use per operator call instead: at the end-of-operator
hook, install the raw callable (no wrapper) until the next re-probe.

## 6. Disk tier (`cache-tables.md`, `libs-*.jsonl`; ext4 on the SATA SSD, page cache warm)

| format | write n = 1e6 + sync, ns/record (d 2 / 20) | random lookup 1 t (16 t wall) | load all, ns/record | bytes/record (d 2 / 20) |
|---|---|---|---|---|
| **flat file** (own: header + fixed records) | 157 / 1569 (no sync 155–4582: writeback throttling, 38–260 MB/s) | `pread` 1.4–1.6 µs | 114–153 (26–70 at 16 t) incl. insert into the table | 32 / 176 |
| HDF5 (one dataset) | 81 / 648 | – (no index) | 3.4 / 25.8 (read only) | 24 / 168 |
| LMDB | 962 / 3497 | 0.68–1.16 µs (50–83 ns) | 23 / 61 | 52 / 316 |
| RocksDB (bloom 10 bit) | 5330 / 7579 | 3.6–4.4 µs (416–506 ns) | 440 / 641 | 13 / 17 (compressed) |
| SQLite (WAL, B tree) | 2171 / 5546 | 10–13 µs (2.0–3.0 µs) | 136 / 260 | 28 / 219 |

- Persistent tier access pattern = bulk load at creation + bulk append at the end of each
  operator: a flat file (or HDF5) fits; no key-value store needed. Records written since the last
  flush are the arena tails of the shards (append-only) → the flush is one sequential write.
- Crash safety: header record count updated after the data (`fdatasync` optional); a torn tail
  is ignored on load. Validation: magic, version, key scheme, d, m, function id (user string).
  Concurrency: one writer per file (`flock`), readers load a snapshot.
- Temporary tier (spill beyond the limit): `O_TMPFILE` (Linux; removed even on SIGKILL) /
  `mkstemp` + `unlink` (POSIX) / `FILE_FLAG_DELETE_ON_CLOSE` (Windows) in `$TMPDIR` or a user
  directory; disk hits cost 1–1.6 µs (page cache) to ~0.1 ms (SSD): only for functions >= ~5 µs
  (Python at 16 t, external simulations). LMDB is the only measured store fast enough for random
  disk hits; RocksDB / SQLite only for >= 50–100 µs functions.

Planned interplay of the memory table and the disk tiers: §11.

## 7. Memory limit

Accounting per shard (whole arena chunks + index slots), the limit split evenly (no global
atomic). The CT has little temporal locality (a point's reuses are spread over all tensor grids
containing it; coarse points have the highest multiplicity), so LRU / CLOCK buy little and cost a
per-hit update; recommended: **stop inserting** at the soft limit (hits on what is cached stay,
rejected inserts counted), or, with a temporary tier, spill new records to it. Not measured: hit
rates under a limit per policy (trace simulation; follow-up if the owner wants eviction).

## 8. Python (`python-callable.md`, scratch bindings with a C++ cache in front of the callable)

| | 1 t ns/eval | 16 t ns/eval |
|---|---|---|
| Python callable (`math` / NumPy) | 659–823 / 4852–5335 | 24095–24520 / 30014–30296 (GIL: 36x slower than 1 t) |
| dict cache inside the callable, warm | 534–703 (1.15–9.1x) | 23558–24359 (0.99–1.27x) |
| **C++ cache, cold** | 201–2142 (1.9–5.1x) | 4468–11828 (2.5–5.4x) |
| **C++ cache, warm** | 65–90 (9–74x) | 12–13 (1925–2502x) |

Hits of a C++ cache never take the GIL; a Python-side cache cannot avoid it.

## 9. Interplay

- WP-17: give the cache array lookups (`findBatch` / `insertBatch` over contiguous points) so a
  batched operator looks up a whole tensor grid, then evaluates the misses as one batch (one GIL
  acquisition for Python).
- WP-24 / WP-29: `RecordLayout{d, m}`, one lookup serves all m outputs, keys independent of m,
  m in the file header; one cache per function, `SourceFunc` and `VectorSourceFunc` wrappers
  share the cache classes, never one cache.
- WP-23 (base move): with the wrapper, `SourceFunc::evaluate` is unchanged; the cache classes can
  live in base (generic over key length and m) — the hit path is >= 50 ns, a cross-library call
  adds ~1–2 ns. Moving `SourceFunc` itself needs `evaluate` header-inline.
- WP-01: nothing inside the operators' loops changes (lookup inside the opaque call).
- Dimension-adaptive refinement: user-domain keys are operator-, grid- and step-independent →
  values of earlier steps are reused, persistent files across runs; the end-of-operator hook can
  report per-call hits / misses. Discrete keys would tie the cache to one generation config.
- Operator API: unchanged; the cache travels inside the `SourceFunc` (shared_ptr), sharing =
  passing the same cached `SourceFunc` (or several built on one cache) to all operators / grids.

## 10. Open decisions (owner, deferred to the implementing WPs)

Fixed by the measurements: a plain `SourceFunc` gets no per-evaluation check (caching lives in
the stored callable); the cache is held through a `shared_ptr` (copies of a `SourceFunc` copy
the callable, pinned by `source_function_test`); `SourceFunc` gets one member for the
end-of-operator hook (null for plain functions, read once per operator call).

| # | decision | options | recommendation | when |
|---|---|---|---|---|
| 1 | construction API | (A) `SourceFunc(func, cache)` constructor · (B) thin subclass `CachedSourceFunc`, no virtuals, all state in the base · (C) free factory `makeCachedSourceFunc(func, cache)` | B (explicit type, `cache()` / statistics accessors, `pysgpp.CachedSourceFunc`) | WP-26 |
| 2 | configuration | shared cache object `std::make_shared<SourceFuncCache>(CacheConfig{...})` and/or `CacheConfig` passed directly | both constructors | WP-26 |
| 3 | `CacheConfig` contents | policy as a run-time field (always / adaptive; "none" = plain `SourceFunc`), memory limit, disk tier (none / temporary + directory / persistent + path), function id; memory–disk interplay §11 | – | WP-26 (limit), WP-28 (disk), WP-27 (adaptive) |
| 4 | vector-valued variant | `CachedVectorSourceFunc` on the same cache class (m = run-time field of the record layout) | yes | after WP-29 |
| 5 | in-memory index | Boost.Unordered (header-only, new library dependency) or own open addressing | – (§3) | WP-26 |
| 6 | domain mapping | one helper with pinned rounding (quadrature points move <= 1 ulp on non-unit domains) or keep (no quad <-> interp hits there) | unify (§2) | WP-26 |

## 11. Memory–disk interplay (planned for WP-28; not built in WP-25)

Rule: **memory is always the primary lookup structure; the disk is written behind it and read
only when memory cannot hold everything.** Measured in WP-25: the parts (§6: flat-file write /
load, `pread` hits, LMDB / RocksDB / SQLite / HDF5); the tiered cache itself is WP-28's job.

Lookup path:

```
evaluate(x)
  └─ memory table (sharded, full records)        hit: ~50–330 ns (1 thread)
       └─ miss → disk index in memory (hash → file offset, ~12–16 B/entry)
            └─ found → pread one record           hit: ~1.5 µs (page cache), ~0.1 ms (cold SSD)
            └─ not found → call the user function, insert (memory or disk, see below)
```

The disk index exists only for records that live on disk but not in memory; when everything
fits, every lookup is a memory lookup.

**Persistent tier** (file kept across runs):
- On creation: validate the header (version, key scheme, d, m, function id) and load all
  records into the memory table (26–70 ns/record at 16 threads: ~0.05 s per 1e6 records). After
  that, operators never read the disk.
- During an operator call: misses go into the memory table only. Records are append-only per
  shard, so "dirty" = past the shard's flush mark (no per-record flag).
- End-of-operator hook: append each shard's tail sequentially, then update the record count in
  the header (optionally `fdatasync`). After every operator call all evaluated values are in the
  file; a crash loses at most the current call's new records (a torn tail is ignored on load,
  the count is written after the data).

**At the soft memory limit** (applies to the process's own memory: full records + disk index,
not to the OS page cache):
1. Up to the limit: full records in memory.
2. Beyond it, with a disk tier configured: new records are appended to the file, only a compact
   index entry (hash → offset, 12–16 B instead of 42–187 B; ~4–12x more entries per GB) stays in
   memory. A file larger than the limit at load time is split the same way (load up to the
   limit, index the rest).
3. When the index no longer fits: stop caching new values in this run; with a persistent file,
   values are still appended so that the next run finds them.

With a persistent tier, the persistent file doubles as the spill store (no second file).
Without one, the **temporary tier** is the same with an anonymous file (`O_TMPFILE` on Linux,
removed even on SIGKILL; `mkstemp` + `unlink`; `FILE_FLAG_DELETE_ON_CLOSE` on Windows) in
`$TMPDIR` or a user directory, used only once the limit is hit. Spill writes happen inside the
parallel region: each shard collects spilled records in a small buffer and writes it at a file
offset reserved with one atomic add (no global lock).

**Deliberately not planned**:
- No eviction or promotion between memory and disk: reuse in the CT is spread over the whole
  operator (all tensor grids containing a point), so LRU-style migration costs a per-hit update
  for little gain; records stay where they were first stored (revisit after a trace simulation).
- No database for the normal case: bulk load + bulk append is what a flat file does best. LMDB
  (0.7–1.2 µs random lookups, scales with threads) is the fallback if random disk hits become
  frequent (caches far larger than memory).

**When disk hits pay**: ~1.5 µs (page cache) to ~0.1 ms (cold SSD) per hit → spilling beats
recomputation only for functions ≳ 5 µs (≳ 100 µs if the OS does not cache the file); below
that, "stop inserting" is better. Choice: a `CacheConfig` field or the adaptive policy (WP-27)
from the measured function cost.

**To decide at WP-28** (open decision 3): full load vs. load up to the limit, spill on / off,
`fdatasync` on / off, `mmap` vs. `pread` for disk hits, the threshold for spilling.
