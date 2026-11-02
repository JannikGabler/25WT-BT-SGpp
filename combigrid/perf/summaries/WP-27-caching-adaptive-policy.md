# WP-27 — Caching: adaptive policy (`CachePolicy::Adaptive`)

Status: PROPOSED · Date: 2026-11-02 · Base commit: 08df92b69
Recommendation: accept — a cheap function under Adaptive runs at 0.57–0.86x of plain instead of
0.07–0.30x with "always", an expensive one at 0.91–1.03x of "always"; judged suite neutral.
Machine: `perf.py calibrate` ratios e2e 1.005 / 1.000, micro 0.998 / 1.000 (OK)

## Change
- Owner's decisions in-session (all 12: `results/WP-27/decisions.md`; options with measurements:
  `decision-options.md`): `CachePolicy::Adaptive` switches by itself between None / MemoryOnly
  (start) / TemporaryStorage. The rules weight by hit rate: c_f vs. E = (h c_hit + (1−h) o_miss) / h,
  with factors 0.8 / 1.25 / 1.5 / 1.2 (`constants.hpp`, `ADAPTIVE_*`). Decisions are made in-call
  every 64 timed evaluations. Cold start: bound h = 1 until 2^16 lookups, afterwards the exact marginal
  hit rate. None is left via probes with backoff 1…64. The temporary file is created at the first
  spill, kept, and reused.
- Mechanism: a mode word read per evaluation in the callable of the adaptive cached functions only.
  Memory hits stay inline; misses and timed points go out of line (`source_function_cache_adaptive.cpp`).
  One point in P is timed (hash bits), P calibrated from the timer cost (rdtscp → 64). Window
  decisions are lock-free (`CostSampleWindow`), and pure rules are tested with injected costs
  (`misc::AdaptiveCacheController`, `adaptive_rules`). Table: atomic `store_` (lazy file),
  `insert(…, allowSpill)` returns stored/rejected.
- Machine finding: this laptop runs on the **HPET clocksource** (the kernel marked the TSC unstable at
  boot, warp 3e9 cycles between CPUs): `steady_clock::now()` costs 1.9 µs, `rdtscp` 37 ns.
- Files: new `caching/adaptive_cache_mode.hpp`, `caching/source_function_cache_adaptive.cpp`,
  `miscellaneous/caching/adaptive/*` (16 files), tests `tests/miscellaneous/caching/
  adaptive_cache_controller_test.cpp`, `tests/functions/source_functions/
  adaptive_cached_source_function_test.cpp`. Changed: `cache_policy.hpp`, `cache_statistics.hpp`
  (8 appended fields), `source_function_cache.{hpp,cpp}`, `cached_(vector_)source_function.{hpp,cpp}`,
  `boost_point_value_table.{hpp,cpp}`, `constants.hpp`, `sgpp_combigrid.hpp`, `CachedSourceFunc.i`
  (`AdaptiveCacheMode_None` keeps its name: a SWIG keyword rule is lifted for that header only),
  `perf_cache.hpp` (adaptive micro cases), `cached_source_function_test.cpp` (availability). Docs:
  CLAUDE.md §2/§6, module-reference.md, python-bindings.md.

## Gain vs. cost
- Gain (feature; `results/WP-27/adaptive.md`, ns per evaluation; speedups vs. plain):
  - cost-0 integrands, one fresh call: 0.71–0.86x at 1 thread and 0.57–0.82x at 16 threads, vs.
    "always" at 0.14–0.30x / 0.07–0.17x (2.9–5.2x / 4.3–8.5x faster). Steady state incl. probes:
    0.82–0.91x / 0.85–1.09x.
  - 1 µs function: Adaptive vs. "always" 0.97–1.02x (1 t) / 0.91–1.03x (16 t); steady state vs.
    warm 0.93–1.0x.
  - interp at c0: stays cached as it should (c_f 131 ns, hit 54 ns), 1.11x vs. plain.
  - Measured c_f is 986–1021 ns for the nominal 1000 ns function. One decision costs 2.3–2.4 µs per
    window (≈ 0.6 ns per evaluation).
- Losses: (a) deliberate myopia: DSG quadrature (h = 0) at 1 µs is switched off; steady state 182 vs.
  92 ns warm (values not kept for later operators). (b) Memory limit 0 at 1 µs and 16 threads: the
  exploration MemoryOnly → TemporaryStorage → MemoryOnly → None within one call costs 244 ns vs.
  73 plain (TemporaryStorage policy 489): the LMDB writes are serialized, but the costs are measured
  per thread.
- Memory: e2e geomean B/A 1.003, 0 MEM±; largest `interp/sg/equi/b0/d3/L5/p64` 1.022x.
- API / functionality: as decided (enum value, statistics fields, Python). Plain `SourceFunc`
  and the existing policies are unchanged in behavior; the default stays MemoryOnly.
- Maintainability: +~2700 lines (new files 2195, of which ≈ 800 tests; ≈ 1000 code incl. Doxygen),
  one type per file, methods ≤ 30
  lines; the rules are a pure function.

## Tests
- Pinning (passed on the unchanged code, debug ×3): `source_function_cache*` suites (cached ==
  uncached bitwise for all operators / families / b0/b1 / domains, limits, storage policies),
  `source_function_test`, `cache_file_exception_test`.
- New: `adaptive_cache_rules` (3), `adaptive_cache_controller` (11, injected costs: first window,
  cold start, hysteresis, probes / backoff, end-of-call probe, disk tier, disk hit rate, no disk /
  failed file, outliers, reset), `adaptive_cache_measurement` (2), `source_function_cache_adaptive`
  (7: bitwise in all modes with mid-call switches for scalar and vector, off + probes, expensive
  stays, misuse / other d / clear, concurrent operator calls, TemporaryStorage at limit 0, failing
  file thrown once).
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ · TSan ✓ (`results/WP-27/tsan.txt`, both
  drivers) · DISABLE matrix ✓ (3 configs, Adaptive works without LMDB; `nolib_matrix.txt`) ·
  public-header check ✓ · Python (`SG_PYTHON=1`) WP-27/26/28 checks at 1 / 16 t + 10 examples ✓.
  The known flaky test did not recur.

## Reference check
- `perf.py refcheck`: OK (104/104 at 1 and 16 threads; `results/WP-27/refcheck.txt`).

## Benchmarks (A = WP-27-before, B = WP-27-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, existing cache cases: 78; `results/WP-27/micro.md`): sum 7.343 → 7.337 s
(1.001x); per case 0.957–1.127x in both directions. Untouched plain `none` d2 moved 0.974x
(code placement). The insert path (spill flag, bool return) runs 0.98–0.99x:

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| cache/insert/production/d10/n10000 | 938.6 µs | 955.9 µs | 0.980 | [0.971, 0.989] |
| cache/op/quad/sg/equi/b1/d20/L3/always/c0 | 36.91 ms | 38.61 ms | 0.957 | [0.953, 0.962] |
| cache/op/quad/sg/equi/b0/d2/L11/warm/c0 | 10.65 ms | 10.06 ms | 1.050 | [1.043, 1.056] |
| cache/lookup/production/d2/n10000 | 580.2 µs | 516.7 µs | 1.127 | [1.118, 1.135] |

L2/L3 e2e (16 threads, full judged suite: the operators' `.text` changed under LTO,
`compare_text.txt`): 1 SLOWER (`quad/dsg/equi-exp/b0/d3/L16` 92.1 → 94.6 ms, 0.972; plain
`SourceFunc`, known noisy small case), 1 faster, 36 noise.

Sum of medians (38 cases): A = 38.316 s, B = 38.335 s (0.999x, within noise) · Full reports: `combigrid/perf/results/WP-27/`

Geomean peak-memory B/A = 1.003 · 0 MEM± · process floor A = 10.3 MiB, B = 10.4 MiB

## Attempts not kept
- Out-of-line hit path (callable → `evaluateAdaptive` → `lookUpAdaptive`): +17 ns per evaluation
  vs. "always" (cc d5 c1000 steady 18.09 vs. warm 13.85 ms); with inline hits 15.09 ms (sampling
  share 0.7 ms). Replaced.
- Window hit rate during the cold start: switched mid-cost functions off early → bound h = 1 plus
  marginal hit rate after 2^16 lookups (owner). In TemporaryStorage the memory hit rate misses the
  file's hits → only the file's rule there. Counting the call that switched off as a None call →
  an immediate probe in the next call; not counted.
- `atomic_thread_fence` in the sample window's sequence lock: TSan cannot model it → release /
  acquire on the fields (same guarantee).

## Profile shift / follow-ups
- Disk rule at many threads: the LMDB writes are serialized, but the costs are measured per thread.
  Weighting the write cost with the number of active threads would end the exploration at limit 0
  earlier (244 → ~80 ns).
- Decisions are myopic (h = 0 → None even if later operators would reuse the values); option: a
  "keep" hint, or count inserts as future hits.
- Making Adaptive the default (not implemented, owner's call): cheap functions would get 0.57–0.91x
  of plain instead of 0.07–0.30x; expensive ones would pay ≈ 3–7 ns per hit (sampling ≈ 3.7 ns).
  A full judged A/B would not be needed (plain `SourceFunc` is untouched).
- WP-17 (batched evaluation): one mode check and one timed batch per call (no sampling); WP-39
  independent.
- Machine: the TSC warp (HPET clocksource) makes every `steady_clock` call 1.9 µs; the harness
  is unaffected, owner may want to look at it (BIOS/firmware).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
