# Lessons — combigrid optimization phase

What did not work (copy the relevant lines into work-session prompts) and measurement notes that
are not in [README.md](README.md). Maintained by the master session; current state:
[STATUS.md](STATUS.md).

## Measurement notes

- **Machine / noise**: a reboot re-enables boost (up to 1.6x throttling; `envcheck` fails → ask the
  owner). Full e2e A/B ≈ 25–30 min since WP-76 (`expensive/` 5 rounds ≈ 7 min). Sanitizer / heap-profiler runs: `systemd-run --user --scope -p
  MemoryMax=8G`. OpenBLAS pool +63…+130 ms on the first operator call (not in `total`). Hot loops
  shift ±3–4 % with unrelated changes (op-cache misses; check before attributing); LTO may
  recompile unchanged code; generation ±20 ms page-fault noise. Clocksource HPET (TSC marked
  unstable): `steady_clock::now()` 1.9 µs, `rdtscp` 37 ns. interplin d8 at 16 t is bimodal in an
  identical binary (1.19–1.23 / 1.30–1.37 s per round) → A/A before trusting single d8 rounds.
  Benchmark-binary placement (RP-4): edits of the protected `perf_e2e.cpp` move the integrands →
  ±2–6 % on `exp` cases across snapshots (not pinned, WP-53 dropped) → check with a hybrid snapshot
  (same libraries, other `perf_e2e`; RP-4 `rp-4-binWP45`). WP-76's `perf_e2e.cpp` edit: cross-snapshot
  ratios spanning it carry ≈ 0.30 s / ±4–8 % on dsg / sg equi b0 and mivec d5 (reference:
  `results/WP-76/e2e-aa-old.md`).
- **Load-balance model** (RP-6, `results/RP-6/schedule_sim.py`): makespan with cost of a TG = its
  points reproduces `expensive/mivec/sg` c100us exactly (dynamic / guided). SG `interpolateLinear`
  has uniform per-TG cost (2^d corners) → no size-based order there.
- **Cross-call cache in e2e**: the pipelines build a new grid per run → the cache never hits in the
  judged suite; repeated-call gains need dedicated benchmarks (WP-73 / WP-82).

## Recurring test failures (→ WP-35; report recurrences)

- time-seeded `equidistant_nodes/sin_func_random_cube_3D`
  (WP-29a 2 of 16, WP-28 1 of ≈ 15, WP-47 1); `{equidistant,clenshaw_curtis}_nodes/linear_func_unit_cube_1D`
  (WP-47 once, WP-52 2 of ≈ 50; pre-existing).

## Do not retry

(`WP-NN <idea>: <numbers>, <why>`; details in the summaries)
- **Methodology** (README.md): profiles via `perf script -F ip,sym,symoff,dso -G` + `addr2line -a
  -i` (`results/RP-3/profile_split.py`), not callchains / `--sort symoff`; libgomp / OpenBLAS shares
  of ms-long processes = pool spin-up. TySan: > 12 GB, false positives → `-Wstrict-aliasing`.
- WP-01 rows always along dimension 0 (b1 47.2 vs. 27.9 ns/point); helpers reading scratch per row
  (2–9 % slower) → copy into locals.
- WP-02 plain-double projected coefficients (round-off 9x); global-width 24 B slots (RSS 36.5 → 65
  MiB); builder without hidden visibility (0.973–0.987); unaligned row loop (0.972x); linear child
  scan instead of `lower_bound` (0.948).
- WP-03 Pareto maxima as closure generators (slower d5–d20); exact sizing (count + `reserve`).
- WP-04 DSG b0 row loop blocks + single-pass block weights + `unroll 1`: quad/dsg 1.047x, rejected
  (complexity / pragmas). Blocks alone ±0, `novector` worse, i0-outer +3; `TrieNode` 16 B 0.982x.
- WP-17 batched evaluation (evaluator objects, blocks ≤ 256 points): 0.960x, NO-GAIN. Loop fission
  loses the overlap of the latency-bound integrand with the row work; removable per-point overhead
  ≈ 5–8 instr → no WP ranks on dispatch. Block caps 16 / 64, partial sums, `unroll 1`, explicit
  `std::fma` in `addScaled` also lost.
- WP-23 dropping base's `operator new` before aligning per-thread data (0.982x, interplin d8 +20–45 %
  false sharing).
- WP-24 one path scalar = m = 1 (SG 1.28–1.48x slower); compile-time m (≤ 5 %); row buffers / per-row
  batches (1.14–1.75x slower at m = 1); interp m-innermost (27 ulp) → innerk.
- WP-25 run-time policy branch in `evaluate` (0.987x); per-evaluation bypass flag (0.69–0.97x);
  per-thread tables (hit rate 0.47 vs. 0.82); quantized keys; dict in the Python callable;
  compile-time policies; arena chunks 4096 (→ 128).
- WP-26 shard counters after the index (16 t 1.12–1.30x slower); rounded keys (nondeterministic) → bitwise.
- WP-27 out-of-line adaptive hit path (+17 ns); window hit rate in the cold start → bound h = 1 then
  marginal rate after 2^16 lookups; memory hit rate in TemporaryStorage → file rule only; callable
  swap at the hook (races) → mode word; `atomic_thread_fence` in the seqlock (TSan) → release / acquire.
- WP-28 `MDB_WRITEMAP` (slower, sparse preallocation); pending lookup / spill inline (inserts 2.6–4.8 %
  slower) → out of line + `[[unlikely]]`; fsync in the destructor (cold runs 1.3–5x slower).
- WP-29b `updateCoeffSums` out-of-line (e2e 0.992x) → header-inline. WP-29c per-call `std::vector`
  for innerk factors (0.74x) → stack blocks; pointer-based ops / non-inlined templates (0.972,
  0.992x); `aligned(64)` on both collapse templates (op-cache misses 6x).
- WP-36 hashed cache for reuse in interp / interplin: 0.41–0.94x (lookup > integrand) → index-based reuse only.
- WP-47 DSG lin. interpolation: serial path inside `omp parallel if(false)` (d2 12.1 vs. 8.8 µs) →
  bypass regions below a work gate (256 corner visits; 4096 too high); branchless lower bound →
  `std::lower_bound` + galloping; claim variant (CAS per slot; slower at 1 and 16 t); per-thread mark
  bitsets (slower); evaluating all slots (non-corner evaluations are observable).
- WP-06 SG global interpolation as nested collapse with premapped nodes (WP-36 "sol"): cc 5–10 %,
  cheb1 2x slower than basis values + quadrature row loop.
- WP-45 DSG global interpolation: per-TG gather 2.0–7.2x slower than global weights; unordered
  reduction over delta TGs (thread-count dependent) → ordered sums; gate 512 → 128; serial basis
  values (≤ 55 % of an equi call) → in-region pass.
- WP-18 storage: SG interpolation in domain coordinates without a unit table (other corners on
  shifted domains); flat node buffer (serial zero fill, d2 L20 27 → 81 ms) → per-row `DataVector`s;
  view holding `const NodeTable*` → raw row pointers; lin-interp views by value (+15 % at d2) →
  const&; DSG kernel reading all nodes from the table (row_loop b0 d7 0.75) → copy the row dim only.
- WP-62 (adaptive defaults): GG safeguard w = 0.999 (4–6x the evaluations on the interaction case);
  fixed bulk batches without threshold (up to 7.6x at small budgets); per-step rebuild of
  `SparseGrid` (73.7 ms at d100) / public-API loop (147–369 µs per index); `CachedSourceFunc` as
  the driver's value store (+170–480 ns per point vs. 9–73 ns); prediction geo-mean / max / min
  identical → one variant.
- WP-49 DSG lin. interp.: pull formulation (per-id weight lists; d10 86.8 vs. 88.2 ms push, d2 9.46 µs;
  no gain, needs per-id lists) → push; register accumulation of the two bracketing ids (d5 731 vs.
  709 µs, d10 91.9 vs. 88.2 ms; loop is instruction-bound, not chain-bound).
- WP-76 judged expensive cases: integrands `uq` (b1: reference errors 0.7–1.0, refcheck insensitive)
  and `cosProd` (≈ 0 at d7–d10 on the shifted domain, 230 ns itself) → `exp`; global-interp reference
  at d10 L2 (abs. error 4e-2 on ≈ 1e-4) → d5 L4; shared atomic evaluation counter in timed runs
  (+30 % at c1us × 16 t) → counts only in the reference suite.
- SG operators: deduplicated evaluations / caching by default (WP-76: quad/sg d10 L4 c100us 1.535 vs.
  DSG 0.571 s): owner 11-17 — SG operators keep the standard CT; use the DSG variants.
- WP-73 cache synchronization (owner 11-18: variant (a), mutex + scan + `shared_ptr`): (b)
  unsynchronized (6 vs. 25 ns per lookup, drops shared first calls), (c) raw pointers (clear / view
  only without concurrent calls), (a-snap) `atomic<shared_ptr>` snapshot (8924 ns at 16 t nonstop);
  no difference on real calls. (a-rw) `shared_mutex` only if call rates ever matter.
- RP-6 (model) plain `guided` → `dynamic` on the SG global-interpolation TG loop: predicted to lose
  (d10 L2 0.934 → 0.830, d2 0.595 → 0.706 efficiency) → largest-first order instead (WP-77).
  interp/sg per-call basis-value table: ≤ 0.07 % at d ≥ 5 (≤ 0.001 s) → no WP.
- WP-79 textbook Lagrange product with numerator / denominator formed on distances × 4: 1D CC / cheb1
  sound up to level 9 instead of 8 (level ≥ 10 NaN), dropped (owner: readability over range; plain
  product ≤ 1e-14 up to level 8, documented). `{1, 0}` weights for a degenerate linear cell → `{0.5,
  0.5}` as `interpolateLinear` (same results, also for inf values).
- RP-4 `[[gnu::aligned(64)]]` on `weighted_sum::sumOverRows`: 0.994x → the WP-52 −1 % is not
  recoverable this way.
