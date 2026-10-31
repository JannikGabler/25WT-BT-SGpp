# WP-26 — decision data presented to the owner (2026-10-31)

Summary of the in-session decisions: `combigrid/perf/summaries/WP-26-caching-in-memory.md`.

## Decision 5: index (own open addressing vs. Boost.Unordered) — quick measurement

`sharded_oa_spin` (added to `benchmarks/perf/perf_cache_proto.hpp`): 64 shards × spin lock + per-shard
arena as WP-25's `sharded_spin`, index = own linear probing with 8-byte slots (32-bit hash
fingerprint | record index + 1), grown at load 3/4 under the shard lock. Same keys, same hash (own
wyhash). Snapshot = the WP-26-before build, `perf.py run micro` (raw: `decision5-tables-t{1,16}.json`).

| case | Boost 1 t | own OA 1 t | Boost 16 t wall | own OA 16 t wall |
|---|---|---|---|---|
| lookup d2 n1e4 / n1e6 | 50.0 / 117.7 | 55.8 / 112.6 | 10.0 / 14.7 | 10.8 / 15.9 |
| lookup d10 n1e4 / n1e6 | 78.1 / 263.8 | 79.1 / 251.2 | 11.7 / 22.3 | 13.6 / 25.7 |
| lookup d20 n1e4 / n1e6 | 110.3 / 333.4 | 113.8 / 349.8 | 13.5 / 28.8 | 15.1 / 35.7 |
| insert n1e6 d2 / d10 / d20 | 170 / 261 / 364 | 188 / 316 / 454 | 40.8 / 58.4 / 75.3 | 45.4 / 67.6 / 86.4 |
| bytes/entry n1e6 d2 / d10 / d20 | 42 / 107 / 187 | 49 / 113 / 194 | | |

(ns per item.) Own OA: equal at 1 t, 16 t lookups +8…+24 %, inserts +10…+15 %, +7 B/entry; no
dependency. Boost: `unordered_flat_set` needs Boost ≥ 1.81 (Ubuntu 22.04 / Debian 12 ship 1.74, CI's
ubuntu-latest 1.83); `<execution>` is only pulled in by Boost's concurrent containers (not used),
so `BOOST_UNORDERED_DISABLE_PARALLEL_ALGORITHMS` is not needed. Hash: own wyhash 6.4 / 8.5 / 15.8 /
21.7 ns, XXH3 3.6 / 8.3 / 10.7 / 18.6 ns (d = 2 / 5 / 10 / 20; WP-25). **Owner: Boost.Unordered +
XXH3**; XXH3 from the system header + CI apt line. Missing libraries (revised by the owner after
the first delivery): detection only in `optional_libraries.hpp` (`__has_include` + version macros,
no SCons code); the cache classes are always compiled and their constructors throw.

## Decision 6: domain mapping — tolerance keys and pinning

- Tolerance ("close enough") keys, variant T1 = round the last k mantissa bits before hashing and
  comparing (`decision6_quantized_keys.cpp`, XXH3, 1 thread): exact 4.1 / 8.2 / 11.4 / 19.3 ns,
  rounded +2.7 / +3.6 / +4.0 / +5.0 ns per key (d = 2 / 5 / 10 / 20). Not taken: merges ulp
  neighbours only with probability 1 − 2^-k per coordinate, relative rounding never merges the
  cancellation results near 0 (absolute tolerance needs unit-domain keys = no pure wrapper), a hit
  returns f(x′) with x′ ≠ x and which neighbour is stored first depends on the thread schedule
  (run-to-run nondeterministic at 16 t), false merges of distinct points closer than the tolerance.
  A true tolerance cannot be hashed (2^d neighbour cells or a spatial index).
- Pinning (`decision6_fma_contraction.cpp`, GCC 15.2, `-O3 -march=native`): `x * w + a` and two
  statements are contracted to `vfmadd`; `__builtin_assoc_barrier(x * w) + a` stays unfused in the
  scalar epilogue but is **contracted in the vectorized loop**; only an empty-asm register barrier
  keeps it unfused. `std::fma` is fused everywhere (correctly rounded library call without
  hardware FMA). **Owner: pinned fused `std::fma`** (`tools::mapToDomain`).
- Result: refcheck 104/104 OK (interpolation points on shifted domains moved ≤ 1 ulp, no case beyond
  the noise allowance); interpolation micro cases 0.8–6.2 % faster (one FMA instead of mul + add);
  quadrature unchanged in the judged build (it was contracted before; results bitwise equal at 1 t).

## Variant measured during the implementation

Statistics counters of a shard in the cache line of its lock (every lookup writes that line anyway)
instead of after the index: `variant-counters-t16.md` (A = counters after the index, B = in the lock
line): 16 t lookups 1.12–1.30x, warm operators 1.17–1.24x, always 1.01–1.16x; 1 t
(`variant-counters-t1.md`) sum 1.009x. Kept.
