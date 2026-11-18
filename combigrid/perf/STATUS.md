# Optimization status — combigrid (master file)

Last processed: WP-73 (2026-11-18; accepted, commit pending) ·
Next free WP number: 83 ·
Re-profilings: RP-1 10-28, RP-2 10-29, RP-3 10-30, RP-4 11-13, RP-5 11-15 (RP-0 = setup)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), GCC 15.2,
  16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (2.0 GHz), profile
  `performance`, AC, idle; nosi (WP-13), strict aliasing on (WP-14; off only for the SWIG Java wrapper).
- **Baseline**: setup commit **fed2cdb4e**, snapshot `baseline` (if cleaned: `perf.py snapshot
  baseline --commit fed2cdb4e`), built with its own flags (flag changes count as progress); never
  re-recorded. Re-profiling snapshots: `rp-1` f48d2bbc1, `rp-2` 5215417ea (1.604x), `rp-3`
  0c6ecfc4b (1.818x), `rp-4` 6ac45a05d (1.933x), **`rp-5` = 3261399ec** (38 cases 69.93 → 35.45 s,
  **1.973x**; memory geomean **0.724**; suite of 48 cases 37.44 s).
- **Memory** (WP-15): e2e reports give each process's peak RSS (floor ≈ 10.2–10.5 MiB); `MEM±` =
  |B/A − 1| > 3 % without overlap of the rounds. `run e2e` = one process per case (baseline file:
  one process → compare within one mode). `ab micro` has 0 warm-ups (allocation-heavy: ratios only).
- **Machine / noise**: a reboot re-enables boost (up to 1.6x throttling; `envcheck` fails → ask the
  owner). Full e2e A/B ≈ 25–30 min since WP-76 (`expensive/` 5 rounds ≈ 7 min). Sanitizer / heap-profiler runs: `systemd-run --user --scope -p
  MemoryMax=8G`. OpenBLAS pool +63…+130 ms on the first operator call (not in `total`). Hot loops
  shift ±3–4 % with unrelated changes (op-cache misses; check before attributing); LTO may
  recompile unchanged code; generation ±20 ms page-fault noise. Clocksource HPET (TSC marked
  unstable): `steady_clock::now()` 1.9 µs, `rdtscp` 37 ns. interplin d8 at 16 t is bimodal in an
  identical binary (1.19–1.23 / 1.30–1.37 s per round) → A/A before trusting single d8 rounds.
  Benchmark-binary placement (RP-4): edits of the protected `perf_e2e.cpp` move the integrands →
  ±2–6 % on `exp` cases across snapshots (not pinned, WP-53 dropped) → check with a hybrid snapshot
  (same libraries, other `perf_e2e`; RP-4 `rp-4-binWP45`).
- **Toolchain**: also Clang 21.1.8 + libomp + lld, JDK 21; variants in `perf/scripts/toolchain.py`.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed (labelled, weighed
  against API / maintainability / readability); functional / interface / flag changes only after
  consulting the owner; errors stay in the same order of magnitude (refcheck OK/WARN/FAIL); high
  dimensions first; STATUS.md may exceed ~120 lines slightly. Dated decisions:
  - Memory (10-28): runtime ranks; peak memory reported with every change; reduction = secondary goal.
  - Prompt notes (PLAN.md header): strict aliasing (10-28, code-changing WPs), dimension-adaptive
    refinement (10-29, API WPs), Python `.venv/` (10-30, every prompt).
  - Vector-valued (10-30): design T; component k == scalar k within round-off. Doc updates by the sessions.
  - Dimension-adaptive refinement (11-17): exploration WP-62 (Fable 5.2, autonomous) with few
    functional restrictions (prototypes may change API / behavior); implementation WPs from its plan.
  - Expensive group (WP-76, 11-17): `expensive/` (c1us / c100us FMA-chain integrands, `-ramp`,
    `-cached` unranked) has its own sum line; **ranking unchanged** (cheap-suite savings), its effects
    are reported separately; no baseline values (history in `results/WP-76/history/`).
  - Standard CT (11-17): operators on a plain `SparseGrid` implement the standard CT — optimizable,
    but their core stays; duplicate evaluations are intended (no more than the standard CT needs);
    the DSG variants are the performance alternative and may deviate.
  - Caching (10-30 … 11-02): design W, bitwise user-domain keys, sharded table, stop inserting at
    the limit; API / disk / adaptive details decided in-session; default `storageSpaceLimit` 32 GiB;
    public headers free of Boost / xxHash / LMDB; `USE_LMDB` as `USE_GSL` / `USE_ZLIB`.
- **Refcheck WARN case** (owner 10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session ("Accept/Reject WP-NN: …": code, summary,
  `results/WP-NN/`, STATUS/PLAN); the tree is clean before the next work session.

## Work packages (ranked; details in PLAN.md, commits in the Archive)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00, 11, 13–16, 20 | Setup; toolchain; flags; memory harness; benchmark move; `FlatNestedVector` | accepted | nosi 1.023x; strict aliasing 0.988x; quad/dsg 1.037x | [summaries/](summaries/) |
| – | WP-01, 02, 03 | SG quad per point; DSG trie at high d; MI-set closure | accepted | quad/sg 2.4x; quad/dsg 1.46x (b1 d20 5.34x); mivec 5.08x (d20 531x) | [01](summaries/WP-01-sg-quad-per-point.md) [02](summaries/WP-02-dsg-trie-work.md) [03](summaries/WP-03-downwards-closure.md) |
| – | RP-1–5 | Re-profilings | accepted | **rp-5 1.973x** (d ≥ 7 1.97x): quad/sg 2.59x, quad/dsg 1.50x (47.6 % of the suite), interp/sg 1.56x, interplin/sg 1.43x, mivec 7.33x; memory 0.724 | [RP-5](summaries/RP-5-reprofiling.md) |
| – | WP-24, 29a–d | Vector-valued (design T): exploration + SG, DSG, interpolation, Python | accepted | scalar 0.995–1.002x; m = 32: SG 1.29–2.68x, DSG 1.96–2.96x, interp 1.25–2.12x | [summaries/](summaries/) |
| – | WP-25, 26, 28, 40, 27 | Caching (design W): in memory, LMDB disk tier, library-free headers, adaptive policy | accepted | judged 0.998–1.001x; 1 µs fn warm 6.6–14.8x; Python warm 1309–2505x; Adaptive cost-0 fn 0.57–0.86x of plain | [26](summaries/WP-26-caching-in-memory.md) [28](summaries/WP-28-caching-disk-tier.md) [40](summaries/WP-40-caching-headers-micro-compaction.md) [27](summaries/WP-27-caching-adaptive-policy.md) |
| – | WP-04, 17, 23 | DSG row loop; batched evaluation; base cross-library calls | rejected | WP-04 quad/dsg 1.047x (complexity); WP-17 0.960x; WP-23 0.999x (→ WP-32 / 33) | [04](summaries/WP-04-dsg-row-loop.md) [17](summaries/WP-17-batched-evaluation.md) [23](summaries/WP-23-cross-library-calls.md) |
| – | WP-10, 12, 19, 38, 53 | Armadillo linking; toolchain re-measure; SG load balancing (→ WP-56); hygiene (→ backlog); integrand placement | dropped | | |
| – | WP-36, 08, 06, 52 | Interpolation: exploration; corner loop; SG global kernel; shared weighted-sum kernels | accepted | interplin/sg 1.29x (−1.02 s); interp/sg 1.35x (−1.47 s); WP-52 refactoring 0.999x | [36](summaries/WP-36-interpolation-exploration.md) [08](summaries/WP-08-corner-loop.md) [06](summaries/WP-06-global-interp-kernel.md) [52](summaries/WP-52-weighted-sum-kernels.md) |
| – | WP-47, 45, 50 | `DeltasSparseGrid` linear / global interpolation + Python bindings (+ judged `interp(lin)/dsg` cases) | accepted | DSG vs. SG 16 t: interplin d10 4.56x (d2 0.60x), interp cc d7 8.48x, d2 3.40x; Python 2.2–7.0x | [47](summaries/WP-47-dsg-linear-interpolation.md) [45](summaries/WP-45-dsg-global-interpolation.md) [50](summaries/WP-50-dsg-python-bindings.md) |
| – | WP-18 | Tensor-grid storage: unit + domain `NodeTable`s, SoA, views, explicit domain / unit accessors | accepted (11-15) | e2e 1.005x; **memory 0.810** (sg equi d2 L20 620 → 346 MiB, dsg b1 d20 218 → 83) | [WP-18](summaries/WP-18-tensor-grid-storage.md) |
| – | WP-05 | SG quadrature at low d: shared lazy weight table (`misc::QuadWeightTable`), CC rule in place | accepted (11-15) | cc d2 L20 1.555x (−460 ms); e2e 1.010x; **memory 0.938** (sg cc d2 862 → 310 MiB) | [WP-05](summaries/WP-05-cc-weight-table.md) |
| – | WP-56 | SG quadrature: real FFT of length m (re-pinned hashes), eager weight table (dynamic, largest row first), TG loop guided → dynamic | accepted (11-15) | cc d2 L20 792 → 438 ms (1.84x), equi b1 d10 L7 1.22x, mivec cc d10 1.32x; −551 ms; **memory 0.981** (cc d2 L20 323 → 195 MiB); weights \|Δ\| ≤ 4.2e-22 | [WP-56](summaries/WP-56-cc-weights-critical-path.md) |
| – | WP-62 | Dimension-adaptive refinement: exploration + implementation plan (owner's request) | accepted (11-17) | GG + profit 8.5–42x fewer evaluations than the complete CT (d10); d100 L2 CT 4.0e-3 vs. adaptive 7.5e-8 (2e5 evals); bookkeeping 10.5–47.5 ns/eval; judged suite unchanged | [WP-62](summaries/WP-62-dimension-adaptive-exploration.md) |
| – | WP-63 … 72 | Dimension-adaptive CT implementation (A1 nestedness + std CC growth, A2 core, A3 quad driver, A4 non-nested / vector, A5 interp, A6 Python, B1 lazy, B2 buffering, C1 stored values, C2 resume) | needs decision (D1–D9) · **unranked** (owner places them) | | PLAN.md |
| – | WP-49 | DSG lin. interp.: slot weights by sum factorization over closure fibers, thread-count independent; gate 640 visits | accepted (11-17, 59f2899e4; DSG pinning hashes re-pinned) | interplin/dsg d10 1.58x, d8 1.54x, d5 1.50x, d2 1.13x; −0.363 s; memory 0.995 (d10 MEM+ 15.3 → 15.9 MiB, d8 MEM−) | [WP-49](summaries/WP-49-dsg-interplin-slot-weights.md) |
| – | WP-76 | Judged suite: cases with expensive source functions (group `expensive/`, owner's D1–D6; protected files by patch) | accepted (11-17, 76a83f06b) | 21 judged cases 17.77 s + 4 cached 1.05 s per pass (suite ≈ 55.5 s); A/A within noise; refcheck OK (146 refs); old cases 1.008x (placement), memory 1.011; efficiency mivec SG 0.42 / DSG 0.62, interplin/dsg d5 0.48 | [WP-76](summaries/WP-76-expensive-source-function-cases.md) |
| – | WP-73 | Cross-call operator data cache: clear / entries / bytes on `SparseGridBase`, options per operator (`useCrossCallOperatorDataCache`), concurrency (a); DSG `interplin` parallel-region bug fix (owner's request) | accepted (11-18; commit pending) | judged 0.999x, `expensive/` 1.001x, memory 1.005; repeated calls flag on 16 t: SG quad cc d2 L20 1.19x, DSG quad 1.03–1.15x | [WP-73](summaries/WP-73-operator-data-cache-control.md) |
| – | WP-82 | DSG quadrature: cross-call cache **on by default** (SG quadrature stays off; owner 11-18) | **in progress** (prompt 11-18) | | |
| – | RP-6 | Re-profiling after WP-56 / WP-49 (+ WP-73 / WP-82 state) | ready (after WP-82) | | |
| – | WP-79 | Textbook operator variants (naive per-TG CT on `SparseGrid`, all three operators; no DSG; one core per operator; helpers in its `.cpp`; naming + scalar / vector in-session; owner's request) | ready (after RP-6) | | |
| – | WP-80 | Grid-evaluation operator: `f` once per distinct grid point (cache warm-up, testing; name / result / dedup in-session; owner's request) | ready · **unranked** (owner places it) | | |
| – | WP-81 | Explanation / visualization foundations: exploration + requirements with the owner in-session (info helpers, strings, plots) → implementation WPs | ready · **unranked** (owner places it) | | |
| 1 | WP-21 | DSG b1: slack break in the projection DFS + sub-trie reuse (RP-5: 0.1–0.15 s) | ready | | |
| 2 | WP-59 | Global interpolation: TG loop `guided` → `dynamic` (est. 0–0.2 s) + SG interplin schedule (WP-76 `-ramp`) | ready | | |
| 3 | WP-77 | SG quad / mivec: TG-loop load balance under expensive functions (WP-76: mivec c100us SG 0.42 / DSG 0.62; RP-6 sizes it) | planned | | |
| 4 | WP-51 | DSG row loop at low d: split large delta TGs (≤ 0.04 s) | ready (low; measure first) | | |
| 5 | WP-57 | Per-thread scratch without zero-filled max-TG buffers (dsg b0 d3 L16 ≈ −19 % RSS) | ready (low; memory) | | |
| 6 | WP-58 | Domain node table shares the unit table's rows on [0, 1] (−33.5 MB on sg d2 L20) | ready (low; memory) | | |
| 7 | WP-60 | DSG quadrature: share 1D weight rows via the unit node table (≤ 0.02 s) | ready (low) | | |
| 8 | WP-61 | SG quadrature: bitwise thread-count-independent results (per-TG sums in TG order) | ready (low; functional: agreed 11-15) | | |
| 9 | WP-09 | Multi-point interpolation API (proposal: gains vs. complexity) | ready | | |
| 10 | WP-32 | Cache-line-align per-thread scratch / shared query data | ready (≈ 0 judged) | | |
| 11 | WP-33 | Base's global `operator new` replacement | needs decision (after WP-32) | | |
| 12 | WP-31 | MI-set: O(n·d) `isMIVecDownwardsClosed` | ready (low) | | |
| 13 | WP-48 | Operator drivers: `std::fma`-pinned CT combination + small-call gate (interplin/sg d2 ≈ 20 ms) | ready (low) | | |
| 14 | WP-35 | Fragile tests + audit (test-only) | ready (low; may move up) | | |
| 15 | WP-54 | TODO inventory of combigrid (measurement) | ready (low) | | |
| 16 | WP-55 | Code-hygiene exploration: verify the backlog (PLAN.md end), propose clean-up WPs | ready (low; after WP-54) | | |
| 17 | WP-37 | pysgpp `DataVector` iteration / bounds check (base) | needs decision | | |
| 18 | WP-30 | Vector-valued: small-m optimizations | planned | | |
| 19 | WP-34 | Python source functions: GIL contention at 16 t (36x) | planned (owner places it) | | |
| 20 | WP-39 | Disk tier: append-only log write path + parallel load | planned (owner places it) | | |
| 21 | WP-41 | Adaptive caching: disk rule weighted by active threads | planned (late) | | |
| 22 | WP-42 | Adaptive caching: "keep" hint / non-myopic decisions | planned (late) | | |
| 23 | WP-78 | Caching: `CachedSourceFunc` overhead at 1 µs × 16 t (WP-76: cached interplin/sg d8 efficiency 0.20) | planned (owner places it) | | |
| 24 | WP-44 | DSG row loop light: single-pass row weights + inline `compRowWeights` (0.2–0.3 s at b0 d7/d10) | planned (owner places it) | | |
| 25 | WP-07 | Generic barycentric weights cache (≈ 0.003 s judged; may merge into WP-09) | ready (low) | | |
| 26 | WP-74 | DSG lin. interp.: fused evaluation (−1 barrier) + vectorized fiber transform (est. ≤ 0.03–0.05 s) | planned (low; RP-6 sizes it) | | |
| 27 | WP-75 | Strict aliasing: byte-type stores in hot loops → `bool` (WP-49: 5 % at d2) | planned (low; after RP-6) | | |
| last | WP-43 | Final clean-up: phase references out of code docs + phase leftovers | ready (always last) | | |

## Next action

**WP-82 in progress** (prompt 11-18: DSG quadrature default on, owner's WP-73 decision). The owner
commits WP-73 first (then `perf.py verify`). Next master start: process WP-82, then **RP-6**
(2 accepted optimizations since RP-5: WP-56, WP-49; interplin/dsg ≥ 1.3x; first profile of the
`expensive/` group: mivec load balance, WP-56 shift; sizes WP-77, the SG `interp` basis-value share,
WP-74, WP-75), then **WP-79** (textbook variants; owner: after RP-6), then the next optimization WP
by RP-6's ranking (today WP-21).
Adaptive WPs 63–72 wait for the owner (D1–D9 + placement). Hygiene findings → **Code-hygiene
backlog** (end of PLAN.md) → WP-54 → WP-55 → clean-up WPs.

## Open decisions for the owner

- **WP-62 D1–D9** (plan `results/WP-62/plan.md` §5; recommendations in PLAN.md WP-62) and the
  placement of WP-63 … 72 — owner decides later (11-17).
- WP-09 (after its summary): adopt the proposed interface or not.
- WP-78 (caching overhead at 1 µs × 16 t, caching callers only): placement.
- WP-80 (grid-evaluation operator), WP-81 (visualization exploration): placement.
- CC weights with `-ffp-contract=off` (bitwise build-independent weights; ≤ 6.7e-16/m): **not now** (11-15).
- Placement of WP-54 / WP-55 (hygiene; may move up); WP-35 up (time-seeded test hit in several sessions).
- WP-33 (after WP-32): drop / restrict / align large allocations only / keep base's `operator new`
  (WP-23: memory −5 %, generation 1.1–1.4x; datadriven relies on it).
- WP-04 `TrieNode` 24 → 16 B (optional; `results/WP-04/*.patch`): −3 MiB per b0 case for b0 0.982x.
- WP-37: base's Python `DataVector` iteration (segfault today) — base API change, placement.
- WP-34: placement + mechanism (Python callables 1 t 0.78 vs. 16 t 25.1 µs ≈ 32x; e.g. serial
  operator or `OMP_NUM_THREADS=1` hint). WP-39: placement. WP-44: placement (largest remaining DSG lever).
- Adaptive as the default `CachePolicy` (not planned): cheap functions 0.57–0.91x of plain instead
  of 0.07–0.30x; expensive pay ≈ 3–7 ns per hit.
- WP-06 option (B) (only if asked): fix dimensions with unit-vector basis values (on-node queries).
- Owner files outdated (backlog E): CLAUDE.md §5 banner claim; ANALYSIS.md WP-29c symbol names;
  SG++ install docs (optional Boost / xxHash, LMDB, CI `liblmdb-dev`).

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
- RP-4 `[[gnu::aligned(64)]]` on `weighted_sum::sumOverRows`: 0.994x → the WP-52 −1 % is not
  recoverable this way.

## Re-profiling

Last: RP-5 (2026-11-15; rp-5 = 3261399ec; accepted). Accepted optimizations since: **2** (WP-56;
WP-49 with interplin/dsg 1.50–1.58x) → **RP-6 due** before the next optimization WP; WP-82 (default flip)
runs first; WP-76 (suite extension) and WP-73 (feature, judged 0.999x) do not count. Rule: features / refactorings / measurement WPs count only if a group changes ≥
1.3x or a hot-spot ranking shifts. The `interp/dsg`, `interplin/dsg` and `expensive/` cases have no
baseline values (reported absolutely; `expensive/` history in `results/WP-76/history/`).

## Log

- 2026-11-18 WP-73 summary processed: **accepted** by the owner (commit pending): API as built,
  concurrency variant (a), DSG `interpolateLinear` parallel-region bug fix; defaults: SG quad off, DSG
  interpolations on, **DSG quad on** (owner; not built) → new **WP-82** before RP-6, prompt issued.
  Judged 0.999x, `expensive/` 1.001x, memory 1.005 (0 flags). Notes added to WP-60 (`DSGQuadIndex`),
  WP-07 (cacheable entry), backlog I. WP-76 committed as 76a83f06b.
- 2026-11-17 Owner: new WP-79 (textbook variants, after RP-6), WP-80 (grid-evaluation operator),
  WP-81 (visualization exploration) → WP-80 / 81 unranked. WP-76 accepted (D1–D6, `expensive/` own sum
  line; SG operators keep the standard CT); WP-49 accepted (re-pinned DSG hashes); WP-74 / 75 / 77 / 78
  added. Details in the Archive.
- Test failures (→ WP-35; report recurrences): time-seeded `equidistant_nodes/sin_func_random_cube_3D`
  (WP-29a 2 of 16, WP-28 1 of ≈ 15, WP-47 1); `{equidistant,clenshaw_curtis}_nodes/linear_func_unit_cube_1D`
  (WP-47 once, WP-52 2 of ≈ 50; pre-existing).

## Archive

- 10-24 setup fed2cdb4e, baseline boost off 931158248. 10-25 WP-01 5281e0b7d. 10-26 WP-11
  9ab8f642d, WP-13 a16733d65. 10-28 WP-14 3dc286c3d, WP-15 a485cec02, WP-16 f48d2bbc1, RP-1
  b0b7faac7, WP-20 29ade3a58. 10-29 WP-02 5215417ea, RP-2 50a8737e0, WP-03 8adcb9536, WP-23
  rejected 2221501b2. 10-30 WP-24 8118ea9a2, WP-25 5ced35885, RP-3 e1257538d, WP-29a 6dad1b375.
  10-31 WP-29b a14f723a0, WP-29c fac9c5787, WP-29d 51a6207bf. 11-01 WP-26 ce575ac0d. 11-02 WP-28
  ef1e4aaf4, WP-40 08df92b69. 11-03 WP-27 5a1229aab, WP-04 rejected e5bd574ce. 11-04 WP-17
  rejected 326cef0f0. 11-08 owner's mid-term quad benchmark (b1d062e75 / 9b3088335; low-d slope =
  pre-phase CC weights). 11-11 WP-36 c3cc48360, WP-08 6df60cb50. 11-12 WP-47 7d04b5dbc (cases
  6138b8b9e), WP-45 8665bba7b (cases 007cf530f), WP-50 266193bc6. 11-13 WP-06 85106468a, WP-52
  6ac45a05d, RP-4 cf698f31d. 11-15 WP-18 14cf31e23, WP-05 3261399ec, RP-5 d426bddcf, WP-56 14c1e3d16.
  11-17 WP-62 0c5071770 (owner's question on weight tables → WP-60 / WP-07 / RP-6), WP-73 defined
  (owner: cross-call data only with significant gain; transparency + consistency), WP-49 59f2899e4,
  WP-76 76a83f06b (D1–D6; owner: SG operators keep the standard CT; WP-77 / 78 new; WP-73 prompt
  reissued), WP-79 / 80 / 81 defined (owner; WP-80 one-core rule + `tools/` placement confirmed).
  11-18 WP-73 accepted (commit pending; WP-82 from the owner's default decision).
