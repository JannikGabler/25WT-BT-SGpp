# Lessons — combigrid optimization phase

Ideas tried and dropped (the relevant lines go into work-session prompts as context) and
measurement notes that are not in [README.md](README.md). Maintained by the master session; current state:
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
  `results/WP-76/e2e-aa-old.md`). A WP that renames exported symbols (WP-88) cannot be checked with a hybrid
  snapshot (the renamed symbols do not link across snapshots) → compare instruction counts and op-cache
  misses instead (`results/WP-88/placement-check.md`: dsg equi d5 L12 0.996x, same instructions, op-cache
  misses ×1.96).
- **Load-balance model** (RP-6, `results/RP-6/schedule_sim.py`): makespan with cost of a TG = its
  points reproduces `expensive/mivec/sg` c100us exactly (dynamic / guided); WP-77 confirmed it
  (largest-first predicted 1.56, measured 1.564 s). SG `interpolateLinear` has uniform per-TG cost (2^d
  corners) → no size-based order there. Since WP-77 every TG loop runs largest-first with
  `schedule(dynamic)`; SG interplin is dynamic in index order (static kept each thread's TG rows in L2:
  d2 L16 −4 %, 1-t calls of 1–90 µs −2–4 %; owner accepted for `-ramp` 1.18x). Grid generation pays the
  order: DSG d20 L6 +2.1 ms (1 t), +8 B per TG.
- **Concurrent cache misses** (WP-77): two threads missing the same key both evaluate it; with the
  largest-first order the largest, overlapping TGs run concurrently → cached SG quad d10 L4 c100us 97–98 k
  instead of 90.56 k evaluations (0.927x) → WP-87 (optional).
- **Thread-count (in)dependence of results**: since WP-48 **every operator** is bitwise thread-count
  and run-to-run independent (TG-indexed per-TG values + serial compensated combination in TG order:
  SG quad / mivec WP-61; DSG quad, SG interp / interplin WP-48; DSG interpolations WP-45 / WP-49;
  `perf_reference` 0 of 146 cases vary at 1/2/3/16 t, `results/WP-48/reference_bits.txt`) → the e2e
  "result sanity" line is evidence of a change for all operators, and a TG reordering must be
  bitwise neutral (test `thread_count_independent_operators`). Before WP-48: 53 of 146 varied.
- **Micro at 16 t**: µs-long calls measure as ms with CIs up to 5x (wake-up artifacts of the micro
  harness; WP-48 `micro-t16.md`) → judge small calls with 1-t micro + e2e only.
- **Cross-call cache in e2e**: the pipelines build a new grid per run → the cache never hits in the
  judged suite; repeated-call gains need dedicated benchmarks (WP-73 / WP-82).

## Test reliability (resolved by WP-35, 2026-11-26)

- No known flaky test since WP-35: any failure is real. Former flakes (history): near-zero relative
  tolerances of cancelling integrals (`equidistant_nodes/sin_func_random_cube_3D`, `*/linear_func_unit_cube_1D`;
  1.0 % of seeds) → magnitude-scaled checks |Q − I| ≤ relTol · ∫|f|; FMA-dependent cache spill →
  count-based small memory; timing-dependent adaptive cold start → 1-thread first call + probes;
  FFT-backend-specific `grid_storage_pinning` hashes → golden values with tolerance.
- Replay a clock seed with the `LD_PRELOAD` shim `results/WP-35/tools/shim/fake_time.c`
  (`FAKE_TIME=<seed>`); sweep seeds with `results/WP-35/tools/fuzz.sh`.
- No test stores bit patterns (owner 2026-11-25): golden values pass changes below 1e-12 (1 + |g|)
  unnoticed → a WP claiming "bitwise unchanged" diffs the `perf_reference` output of its snapshots
  (1 and 16 t; README Pitfalls).
- A tests-only change can still move LTO inlining budgets (WP-35 follow-up: combigrid `.text` −2.8 kB,
  e2e 1.000x) → compare library hashes before skipping A/B benchmarks.

## Dropped earlier

(`WP-NN <idea> (<context>): <numbers>, <why>`; details in the summaries.) Decision context, not
prohibitions: each line holds for the code, case and measurement it came from. Sessions read the
summary before letting a line shape a design, and retry or adapt an idea when the reason does not apply.
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
- WP-29b `updateCoeffSums` out-of-line (e2e 0.992x) → header-inline. WP-29c `std::vector` for the innerk
  factors per 1D interpolation call inside `OptBarycentricFormula` (hot path; malloc 10 %
  of the time, cc d2 m1 0.74x) → stack blocks (once per operator call is negligible, WP-48); pointer-based ops / non-inlined templates (0.972,
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
- WP-83 Bluestein FFT for the cheb1 / Fejér-1 rule with one `Eigen::FFT` object per transform + `fft.inv`
  (3 twiddle tables per row): 1.08–1.28x slower than one plan with forward transforms only (inverse
  by conjugation); cheb1 SG d2 L16 operator 161.7 vs. 157.6 ms. Enabling cheb1 quadrature in the
  cached-source-function tests: their `withQuadrature` flag also builds a DSG, which cheb1 (not nested)
  cannot have → keep cheb1 without quadrature there.
- WP-61 SG quadrature combination: plain loop in TG order (round-off up to 1.9e-8, 20x the old 16-way
  split, up to 300x the discretization error; 84 µs at nTG 54k), pairwise (9.6e-11) → Neumaier + exact
  product errors (≈ 0, 119 µs at nTG 54k; owner 11-23). Out-of-line non-template v1 (same bits / cost)
  → header-only templates. Rework review: `FlatNestedVec` capacity / `std::array` shape constructors
  hijack braced lists (`FlatNestedVec<int, 1>({5})` → size 0, `<int, 2>({{5}})` ambiguous) →
  `FlatNestedVec(n1, …, nD)` + `reserve()`; `std::span<const T>` parameters in the sum templates (no
  deduction from `std::vector` / non-const spans) → deduced contiguous ranges, `<OType>` explicit.
- RP-4 `[[gnu::aligned(64)]]` on `weighted_sum::sumOverRows`: 0.994x → the WP-52 −1 % is not
  recoverable this way.
- WP-48 thread-local per-call TG value buffer for the SG interpolations (`misc::TensorGridValueBuffer`,
  ≈ 60 lines + test): vs. a plain per-call `std::vector` micro 1.000x, e2e 0.999x (memory 0.996) → the
  few-KiB malloc is negligible next to the per-thread scratches; plain vector as SG quadrature.
- WP-48 small-call gate for SG `interpolateLinear` (serial below a corner-visit gate / thread cap):
  interplin/sg d2 L16 p16384 full team 95–107 ms, serial 194–196, cap 8 t 98–99, 4 t 129–130, 2 t
  172–173 ms. `schedule(static)` keeps each thread's TGs (2^16-node rows) in its L2 across calls;
  one thread misses cache (12 vs. 5.8 µs per call). Serial pays only for tiny grids (not in the suite);
  a corner-count gate also ignores the source-function cost → no gate (SG interp neither).
- WP-77 largest-first TG order without a floor (all TGs sorted; full suite, quad/dsg b1 d15 / d20 with
  many tiny delta TGs): suite −365 ms vs. −422 ms with the floor on the same filter; b1 d20 L6 operator
  0.981, d15 L7 0.991 (lost locality) → TGs below total/4096 points keep index order (≥ 99.8 % of the
  modelled efficiency).
- WP-77 split of TGs above the balanced share into row ranges (on top of the largest-first order;
  modelled with `results/WP-77/split_sim.py`, not built): only `expensive/mivec/sg` d15 S6 c100us (≈ −0.65 s
  each, −1.2 s) and mivec/sg d15 S10 (≈ −7 ms) stay above the bound; d2 L20 / interp d2 lose to hand-out
  granularity, not to a large TG. Cost: row-range entry into `sumOverRows`, per-call task list, partial
  buffers (scalar + vector), sub-row splitting for row dims with few rows, round-off change → dropped by
  the owner (11-25): DSG already reaches the bound there (0.552 vs. SG 1.564 s).
- WP-35 adaptive concurrent cold start with `cheapFunc` + only a `BOOST_REQUIRE` (test
  `concurrent_operator_calls_on_one_function`): hit cost under 16-t contention 110–300 ns vs. `cheapFunc`
  48–90 ns against the rule c_f < 0.8 E → still timing-dependent; replaced by a 1-thread first call + probes.
- WP-35 contraction-proof (`constexpr`) shifted domains in the pinning / cache tests: not needed (d ≤ 3
  products exact; cache tests no longer depend on point bits).
