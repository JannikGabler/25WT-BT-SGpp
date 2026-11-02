# WP-27 — adaptive caching policy: decision options for the owner (2026-11-02)

Inputs: WP-25 `design-options.md` / `cache-tables.md`, WP-26, WP-28 `decisions.md`, plus quick
measurements of this session (`perf.py build normal`, boost off; scratch program, not kept):

| measurement | 1 t | 16 t |
|---|---|---|
| `std::chrono::steady_clock::now()` | **1913 ns** | 2370 ns per call per thread |
| `__rdtscp` (incl. CPU id) / `__rdtsc` | 37 / 18 ns | – |
| `clock_gettime(CLOCK_THREAD_CPUTIME_ID)` | 1290 ns | – |
| "off" flag in the wrapper (own cache line) vs. plain `SourceFunc`, cost 0, quad sg cc d5 / equi b1 d20 / equi d2 / dsg cc d5 / interp / interplin | 0.946 / 0.958 / 1.061 / 0.966 / 0.990 / 0.993x | 0.959 / 0.938 / 1.009 / 0.970 / 0.988 / 0.994x |
| always (fresh cache per call), same cases | 0.298 / 0.139 / 0.207 / 0.178 / 1.181 / 0.971x | 0.142 / 0.067 / 0.101 / 0.142 / 0.784 / 0.688x |

**Machine finding**: this laptop's clocksource is HPET — the kernel marked the TSC unstable at boot
("Measured 2974888065 cycles TSC warp between CPUs"). Every `steady_clock` read is a ~1.9 µs MMIO
read (on TSC machines ~20 ns). A design that times evaluations with the standard clock is
therefore machine-dependent by a factor of ~100; the measurement must adapt to the timer's cost.
(The perf harness times whole samples of ms, unaffected.)

Cost model (WP-25): per evaluation, cached `C_on = h·c_hit + (1−h)·(c_miss_overhead + c_f)`,
uncached `C_off = c_f + c_bypass`. Break-even "always" 120–310 ns (1 t) / 270–630 ns (16 t),
warm (h = 1) 85–160 / 120–220 ns. Near break-even both choices cost about the same: a wrong
decision there is cheap; far from it the decision is easy. That makes a coarse estimate enough.

## D1 API

- **(A, rec.)** `CachePolicy::Adaptive` (appended: the values of the existing enumerators stay),
  no new config field. `memoryLimit`, `nShards`, `storageSpaceLimit` apply; `file` must be empty.
  Without LMDB it still works (never spills) instead of throwing. Python `CachePolicy_Adaptive`.
- (B) as A plus tuning fields (e.g. `adaptiveHysteresis`, `adaptiveWindow`): the defaults are
  ratios, not machine numbers, so knobs are hardly needed; can be added later (aggregate).
- (C) orthogonal `bool adaptive` combinable with every policy (incl. Persistent: persist only
  when on). Persistent + off would also skip the file's hits and leave the next run without
  values: the file is the user's explicit wish → not recommended.

## D2 Semantics: three modes, switched by measured costs

- **Off** (no lookup, no insert: the wrapper calls the function), **Memory** (as MemoryOnly),
  **Memory + spill** (as TemporaryStorage: values beyond the memory limit go to an unnamed file).
- Off ↔ Memory: Off when `C_on > (1 + δ)·C_off` (cache loses ≥ δ); back on after a probe with
  `C_on < C_off`. With a relative δ, an expensive function is never switched off even at hit rate
  0 (`C_on ≈ c_f + 0.1–0.6 µs`), so its values are kept for later calls (the "future value" a
  per-call hit rate cannot see; e.g. DSG quadrature, then interpolation). **δ = 0.25 (rec.)**.
- Spill: only once memory is full; on iff `c_f > c_write + c_disk_hit` (measured per record:
  WP-28 0.7–4.8 µs writes, 0.8–0.9 µs disk hits at 1 t) — "pays from its first reuse".
  **Lazy file (rec.)**: created at the end-of-operator hook (outside the parallel region, so an
  error can only disable spilling with one warning on `std::cerr`, never fail an operator) when
  memory is full and `c_f` exceeds the memory-overhead bound; its write / hit costs are calibrated
  then (a few hundred synthetic records, ≈ 0.3–1.3 ms once, removed again) and refined by the
  real spills. Caches that never fill memory never create a file.
- Alternatives: (b) Adaptive = Off / Memory only (users choose TemporaryStorage explicitly);
  (c) eager temporary file at construction (28–46 µs + LMDB env per cache, a file per cache).

## D3 Decision point, window, hysteresis

- **(a, rec.)** Windows inside the operator calls: the thread that adds the W-th timing sample
  (W = 64, of which ≥ 8 misses; `c_f` is carried over between windows) evaluates the rule; Memory
  → Off can happen in the middle of an operator call (a single long quadrature of a cheap
  function pays only the first window: with P = 64 ≈ 4 096 evaluations). Off → Memory only via a
  **probe at the hook**: after k operator calls in Off, the next call runs cached until its first
  window decides; k = 1, 2, 4, … ≤ 64 (exponential backoff, reset when a probe stays on). Cost of a
  probe ≈ one window (≈ 0.3 ms at 1 t for a cheap function), ≤ 1/64 of the calls in steady state.
- (b) per operator call only (decided at the hook): simpler, but the first call always runs fully
  cached (0.07–0.30x for a cheap integrand) and a single big call never adapts.
- Initial mode: Memory (the samples need lookups). Concurrent operator calls on one function: the
  mode is one atomic word; any switch is safe because results are bitwise identical in every mode.

## D4 "Off" mechanism

- **(a, rec.)** Mode word read per evaluation inside the cached function's callable (relaxed atomic
  load, own cache line, read-mostly): table above, 0.94–1.06x of plain at cost 0 (WP-25's
  0.69–0.97x had the flag next to written data). Plain `SourceFunc` and the existing policies get
  their own callable as today (no new instruction on their paths; Adaptive gets a separate one).
- (b) swap the stored callable at the hook: a data race when one function runs in concurrent
  operator calls (UB), unless `SourceFunc` holds an atomic callable pointer → changes plain
  `SourceFunc` (WP-25 run-time branch: 0.987x judged) → rejected.
- (c) a begin-of-operator hook in all 8 operator overloads: an operator change for nothing (the
  per-evaluation check stays unless combined with b) → rejected.

## D5 Cost measurement (and the test seam)

- **(a, rec.)** Hash-sampled timing: a point is timed iff its hash (computed anyway) has its low
  bits zero (1 in P points, P a power of two chosen from the timer cost calibrated once per
  process so that timing costs ≤ ~1 ns per evaluation). Hits, misses (whole lookUpOrEvaluate) and
  the function alone (inside a miss) are timed; the hash itself is calibrated once per table.
  Timer: `rdtscp` on x86-64 (37 ns here; samples where the thread changed CPUs are dropped,
  which also covers this machine's TSC warp) → P = 64; `steady_clock` elsewhere (P = 4 096 on an
  HPET machine, 64 on a TSC one). Costs are compared as ratios (ticks), so the tick rate does not
  matter; statistics convert to ns with a once-per-process calibration (≈ 1 ms).
- (b) `steady_clock` everywhere: same design, decisions ~64x later on HPET machines.
- (c) operator-level timing (one clock pair per call): needs per-evaluation counts in Off
  (a shared counter at 16 t) and comparable calls → rejected.
- Tests: the rule and the state machine live in an internal class (`misc::`) fed with samples, so
  the decisions, hysteresis, backoff and spill rule are tested deterministically with injected
  costs; end-to-end tests use functions 100x away from break-even (empty vs. ~20 µs spin) and check
  bitwise identity with the plain function in every mode and across forced mid-call switches.

## D6 Python

Nothing special: Python callables cost 0.66–5.3 µs (1 t) / 24–30 µs (16 t, GIL) — far above
break-even, Adaptive stays on (the measured `c_f` includes the GIL wait, which is real cost).

## D7 Statistics (`CacheStatistics`, appended fields, aggregate stays source compatible)

**(rec.)** `adaptiveMode` (0 off / 1 memory / 2 memory + spill; size_t for SWIG), `modeSwitches`,
`offOperatorCalls` (operator calls that ended in Off), `functionCostNs`, `hitCostNs`,
`missOverheadNs` (sampled means; 0 for the other policies). Evaluations in Off are not counted
(would need a per-evaluation counter).

## Impact

- WP-39 (append-only log for the persistent hook): independent (Adaptive never uses the persistent
  file); its cheaper writes would just lower the measured spill cost.
- WP-17 (batched evaluation): the mode check and the timing move to one per batch (no sampling
  needed then); `findBatch` / `insertBatch` unaffected.
- Dimension-adaptive refinement: windows follow the changing hit rate per step; an expensive
  function stays on (δ), a cheap one goes off and is re-probed with backoff.
- Judged suite: unchanged (plain `SourceFunc` untouched; the hook is the existing virtual call).
- Default policy stays MemoryOnly (making Adaptive the default is outside this WP: it would cost
  plain-cache users ~1–2 ns per evaluation and give cheap functions 0.94–1.06x instead of 0.07–0.30x).
