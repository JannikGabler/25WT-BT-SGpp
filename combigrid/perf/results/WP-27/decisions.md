# WP-27 — adaptive caching policy: decisions of the owner (2026-11-02, in-session)

Options presented: [decision-options.md](decision-options.md) (with this session's measurements:
`steady_clock::now()` 1.9 µs on this laptop — HPET clocksource, the kernel marked the TSC unstable
at boot —, `rdtscp` 37 ns, "off" flag 0.94–1.06x of plain at cost 0).

1. **API**: `CachePolicy::Adaptive` (appended enumerator), no new `SourceFuncCacheConfig` field;
   `file` must be empty; works without LMDB (never uses a file). Python
   `pysgpp.CachePolicy_Adaptive`.
2. **Semantics (owner's model)**: the cached function chooses by itself among **None** (no
   caching), **MemoryOnly** and **TemporaryStorage**; **starts in MemoryOnly**. Each mode uses only
   its own tiers (None: no lookup at all; MemoryOnly: the file is not read even if it exists),
   because the measurements said a higher tier does not pay. Records stay in memory when switching
   to None (session's choice, owner left it open: a probe that switches back finds them;
   `clear()` frees them). The temporary file is created at the **first spill in TemporaryStorage
   mode** and kept (not used) when leaving TemporaryStorage, reused when coming back.
3. **Switching rules (owner's factors, configurable in `constants.hpp`)**, hit-rate weighted
   (owner chose the weighting over plain cost ratios); c_f = mean cost of the function,
   E = expected overhead of a tier per avoided evaluation, (h c_hit + (1 − h) o_miss) / h:
   - MemoryOnly → None: c_f < 0.8 E_memory (`ADAPTIVE_TO_NONE_FACTOR`);
   - None → MemoryOnly: c_f > 1.25 E_memory (`ADAPTIVE_FROM_NONE_FACTOR`, in probes);
   - MemoryOnly → TemporaryStorage: memory full and c_f > 1.5 E_disk
     (`ADAPTIVE_TO_DISK_FACTOR`; before the file was ever measured, E_memory as a lower bound);
   - TemporaryStorage → MemoryOnly: c_f ≤ 1.2 E_disk (`ADAPTIVE_FROM_DISK_FACTOR`).
   Implementation detail found in the tests: while the file is in use, the memory hit rate misses
   the file's hits, so only the file's rule applies in TemporaryStorage (chain None ← MemoryOnly ←
   TemporaryStorage, as the owner's rules list them).
4. **Decision point**: in-call windows (owner): every 64 timed evaluations
   (`ADAPTIVE_WINDOW_SAMPLES`) the thread completing the window decides; switches act at once,
   also in the middle of an operator call.
5. **Cold start** (owner: option "bound in-call, hit rate from the call", with the remark that a
   source function is mostly used for one big operator call, so a hit rate known only after a
   whole call is too late): until 2^16 lookups (`ADAPTIVE_WARMUP_LOOKUPS`) the rules use the
   bound h = 1 (switch off only if even all hits would lose: catches cheap functions in the first
   window); afterwards the exact **marginal** hit rate of the last 2^14–2^15 lookups
   (`ADAPTIVE_HIT_RATE_LOOKUPS`, the table's exact counters). Measured cumulative hit rates after
   ~4k lookups vs. whole call (1 / 16 threads): quad sg cc d5 0.65 / 0.09 vs. 0.83; equi d2 L11
   0.00 / 0.26 vs. 0.63; interplin 0.55 / 0.24 vs. 0.77; by 65k lookups the marginal rates are
   near their final values. Without any hit, the miss overhead stands in for the hit cost.
6. **Leaving None** (owner: "probes with backoff or an initial no-switch window, take the
   better"): probes — after k operator calls in None (k = 1, 2, 4, … 64,
   `ADAPTIVE_MAX_PROBE_INTERVAL`; reset when a probe stays) the next call runs in MemoryOnly; its
   windows switch off with the 1.25 threshold, its end decides with the exact hit rate of the call.
   The call during which None was entered does not count. Reason: None measures nothing, so only
   a probe can notice a change; the warm-up (5.) already plays the role of the initial window.
7. **Off mechanism**: the mode word (own cache line, relaxed load) in the callable of the adaptive
   cached functions; plain `SourceFunc` and the other policies keep their own callables (no new
   instruction). Swapping the callable at the hook races with concurrent operator calls
   (rejected).
8. **Timer** (owner): `rdtscp` on x86-64 (samples whose start and end CPU differ are dropped),
   `steady_clock` elsewhere; one point in P is timed (bits 32.. of its hash), P = 2^k from the
   timer cost calibrated once per process, ≤ 2 ns timer cost per evaluation
   (`ADAPTIVE_TIMER_BUDGET_NS`; rdtscp → P = 64, HPET steady_clock → P = 2048). Robust means:
   trimmed (1/8 each side) per window, decaying weights across windows.
9. **Temporary-file errors**: creation happens inside the parallel region (under the cache mutex);
   a failure disables TemporaryStorage for good (MemoryOnly) and the end of that operator call
   throws `base::file_exception` once (as WP-28's write errors).
10. **Python**: nothing special (callables cost 0.66–30 µs, far above break-even; the GIL wait
    counts as cost). `AdaptiveCacheMode` and the new statistics fields are bound.
11. **Statistics** (`CacheStatistics`, appended): `adaptiveMode`, `modeSwitches`,
    `noneOperatorCalls`, `functionCostNs`, `memoryHitCostNs`, `memoryMissOverheadNs`,
    `diskHitCostNs`, `diskMissOverheadNs`. Evaluations in None are not counted.
12. Not changed (outside the decisions): default policy stays MemoryOnly; persistent files are
    never adaptive; no file-format change.
