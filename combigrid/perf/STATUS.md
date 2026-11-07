# Optimization status — combigrid (master file)

Last processed: WP-17 (2026-11-04; WP-17 rejected) · Next free WP number: 45 ·
Re-profilings: RP-1 10-28, RP-2 10-29, RP-3 10-30 (RP-0 = setup)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), GCC 15.2,
  16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (2.0 GHz), profile
  `performance`, AC, idle; nosi (WP-13), strict aliasing on (WP-14; off only for the SWIG Java wrapper).
- **Baseline**: setup commit **fed2cdb4e**, snapshot `baseline` (if cleaned: `perf.py snapshot
  baseline --commit fed2cdb4e`), built with its own flags (flag changes count as progress); never
  re-recorded. `rp-1` = f48d2bbc1, `rp-2` = 5215417ea (1.604x), **`rp-3` = 0c6ecfc4b** (e2e 69.92 →
  38.45 s, 1.818x; d ≥ 7 1.87x; memory geomean 1.014).
- **Memory** (WP-15): e2e reports give each process's peak RSS (floor ≈ 10.2–10.5 MiB); `MEM±` =
  |B/A − 1| > 3 % without overlap of the rounds. `run e2e` = one process per case (baseline file:
  one process → compare within one mode). `ab micro` has 0 warm-ups (allocation-heavy: ratios only).
- **Machine / noise**: a reboot re-enables boost (up to 1.6x throttling; `envcheck` fails → ask the
  owner). Full e2e A/B ≈ 18–23 min. Sanitizer / heap-profiler runs: `systemd-run --user --scope -p
  MemoryMax=8G`. OpenBLAS pool +63…+130 ms on the first operator call (not in `total`). Hot loops
  shift ±3–4 % with unrelated changes (op-cache misses; check before attributing); LTO may
  recompile unchanged code; generation ±20 ms page-fault noise. DSG row loop `[[gnu::aligned(64)]]`.
  Clocksource HPET (WP-27: kernel marked the TSC unstable at boot): `steady_clock::now()` 1.9 µs,
  `rdtscp` 37 ns; harness unaffected; owner may check BIOS / firmware.
- **Toolchain**: also Clang 21.1.8 + libomp + lld, JDK 21; `perf/scripts/toolchain.py` variants
  (`gcc-sa` WP-14, `gcc-nsa` WP-20).
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed (labelled, weighed
  against API / maintainability / readability); functional / interface / flag changes only after
  consulting the owner; errors stay in the same order of magnitude (refcheck OK/WARN/FAIL); high
  dimensions first; STATUS.md may exceed ~120 lines slightly. Dated decisions:
  - Memory (10-28): runtime ranks; peak memory reported with every change; reduction = secondary goal.
  - Prompt notes (PLAN.md header): strict aliasing (10-28, code-changing WPs), dimension-adaptive
    refinement (10-29, API WPs), Python `.venv/` (10-30, every prompt).
  - Vector-valued (10-30, WP-24): design T (`VectorSourceFunc`, template kernels, overloads);
    component k == scalar k within round-off. Doc updates by the sessions (WP-29b–d precedent).
  - Caching (10-30, WP-25): design W (caching in the stored callable, no per-evaluation check),
    bitwise user-domain keys, sharded table, stop inserting at the limit. API / disk fields decided
    in-session (WP-26, WP-28; owner 11-01); likewise all open decisions of WP-27 (adaptive
    caching; owner 11-01). Default `storageSpaceLimit` 32 GiB (owner 11-02). Public headers stay
    free of Boost / xxHash / LMDB (owner 11-02 → WP-40). `USE_LMDB` as `USE_GSL` / `USE_ZLIB`.
- **Refcheck WARN case** (owner 10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session ("Accept/Reject WP-NN: …": code, summary,
  `results/WP-NN/`, STATUS/PLAN); the tree is clean before the next work session.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00, 11, 13–16, 20 | Setup; toolchain; flags; memory harness; benchmark move; `FlatNestedVector` | done / accepted | nosi 1.023x; strict aliasing 0.988x; A/A 1.001x; quad/dsg 1.037x | [summaries/](summaries/) |
| – | WP-01, 02, 03 | SG quad per point; DSG trie at high d; MI-set closure | accepted | quad/sg 2.4x (b1 d20 4.7x; sg d2 806 → 1076 MiB); quad/dsg 1.46x (b1 d20 5.34x, 328 → 217 MiB); mivec 5.08x (d20 531x) | [01](summaries/WP-01-sg-quad-per-point.md), [02](summaries/WP-02-dsg-trie-work.md), [03](summaries/WP-03-downwards-closure.md) |
| – | RP-1–3 | Re-profilings | accepted | **rp-3 1.818x**: quad/sg 2.42x, quad/dsg 1.50x, interp 1.16x, interplin 1.10x, mivec 7.42x; memory 1.014 | [RP-3](summaries/RP-3-reprofiling.md) |
| – | WP-10, 12, 23 | Armadillo linking; toolchain re-measure; base cross-library calls | dropped / dropped / rejected (NO-GAIN) | WP-23: e2e 0.999x → WP-32 / WP-33 | [WP-23](summaries/WP-23-cross-library-calls.md) |
| – | WP-24, 25 | Explorations: vector-valued (T); caching (W) | accepted | proto benchmarks `perf_vector_proto.hpp`, `perf_cache_proto.hpp`, `perf_cache_counts.cpp` kept | [24](summaries/WP-24-vector-valued-exploration.md), [25](summaries/WP-25-caching-exploration.md) |
| – | WP-29a–d | Vector-valued: SG, DSG, interpolation, Python | accepted (10-30/31) | scalar 0.995–1.002x; m = 32: SG 1.29–2.68x, DSG 1.96–2.96x, interp 1.25–2.12x, interplin 1.21–1.59x, Python 16 t 24–31x; m = 1 0.66–1.01x | [a](summaries/WP-29a-vector-sg-quadrature.md) [b](summaries/WP-29b-vector-dsg-quadrature.md) [c](summaries/WP-29c-vector-interpolation.md) [d](summaries/WP-29d-vector-python-bindings.md) |
| – | WP-26 | Caching in memory (`CachedSourceFunc` / `CachedVectorSourceFunc`, virtual hook, Python) | accepted (11-01) | e2e 1.001x, memory 1.002; hook micro b1 d20 0.981 (→ WP-17); 1 µs fn warm 6.6–14.8x; Python warm 1309–2505x | [WP-26](summaries/WP-26-caching-in-memory.md) |
| – | WP-28 | Caching: disk tier (LMDB; `CachePolicy` MemoryOnly / TemporaryStorage / PersistentStorage) | accepted (11-02, ef1e4aaf4; + owner's refactoring, re-check fixes) | e2e 1.000x (re-check 1.002x), memory 1.004, refcheck OK; persistent warm, 1 µs fn: cc d5 9.0x / 1.6x (1 / 16 t), d20 16 t 0.44x (serial load) | [WP-28](summaries/WP-28-caching-disk-tier.md), [re-check](summaries/WP-28-recheck.md) |
| – | WP-40 | Caching: library-free public headers (fwd decls) + leftovers + micro compaction | accepted (11-02, 08df92b69) | e2e 0.998x (114 noise), memory 1.005, refcheck OK; umbrella closure 685 → 385 headers, 3 / 3 header checks in 4 configs; micro 827 → 305 cases (build 27.0 → 17.7 s) | [WP-40](summaries/WP-40-caching-headers-micro-compaction.md) |
| – | WP-27 | Caching: adaptive policy (`CachePolicy::Adaptive`: None / MemoryOnly / TemporaryStorage, hit-rate-weighted rules, in-call windows, probes) | accepted (11-03, 5a1229aab) | e2e 0.999x, memory 1.003, refcheck OK; cost-0 fn 0.57–0.86x of plain (vs. "always" 0.07–0.30x); 1 µs fn 0.91–1.03x of "always" | [WP-27](summaries/WP-27-caching-adaptive-policy.md) |
| – | WP-04 | DSG quadrature: row loop (b0) (block loop + `unroll 1`; `TrieNode` 24 → 16 B) | rejected (11-03; complexity vs. gain) | measured quad/dsg 1.047x (−0.79 s), memory 1.004; kept: tests, micro `dsg/row_loop`, `process_delta_tgs/.../s256`; patches in `results/WP-04/` | [WP-04](summaries/WP-04-dsg-row-loop.md) |
| – | WP-17 | Batched source-function evaluation (evaluator objects, `PointBlock` ≤ 256 points, batch API, Python `fromBatch`) | rejected (11-04; NO-GAIN) | judged e2e 0.960x (−1.6 s, 29/38 SLOWER), memory 1.003, refcheck OK; Python 16 t 28x, warm cache 1.12–1.29x | [WP-17](summaries/WP-17-batched-evaluation.md) |
| 1 | WP-36 | Interpolation operators: exploration (measurement) | in progress (prompt 11-04) | | |
| 2 | WP-09 | Multi-point interpolation API (proposal) | ready (after WP-36) | | |
| 3 | WP-06 | Global interpolation: operator overhead | after WP-36 / WP-09 | | |
| 4 | WP-08 | Linear interpolation: corner loop | after WP-36 / WP-09 | | |
| 5 | WP-21 | DSG b1: slack break in the projection DFS + sub-trie reuse | ready | | |
| 6 | WP-18 | Tensor-grid storage: shared 1D nodes + SoA (memory; proposal) | ready (protected-benchmark decision first) | | |
| 7 | WP-05 | SG quadrature at low d: CC 1D weights + scratch on demand | ready | | |
| 8 | WP-07 | Generic barycentric formula: cache the weights | ready (after WP-36) | | |
| 9 | WP-19 | SG quadrature: load balancing at low d | ready (after WP-05) | | |
| 10 | WP-32 | Cache-line-align per-thread scratch / shared query data | ready (≈ 0 judged) | | |
| 11 | WP-33 | Base's global `operator new` replacement | needs decision (after WP-32) | | |
| 12 | WP-31 | MI-set cleanups | ready (low) | | |
| 13 | WP-35 | Fragile tests (time-seeded tolerance; FMA-dependent cache precondition) + audit (test-only) | ready (low; may move up) | | |
| 14 | WP-38 | Code hygiene: banner checker + combigrid leftovers | ready (low) | | |
| 15 | WP-37 | pysgpp `DataVector` iteration / bounds check (base) | needs decision | | |
| 16 | WP-30 | Vector-valued: small-m optimizations | planned | | |
| 17 | WP-34 | Python source functions: GIL contention at 16 t (36x) | planned (owner places it) | | |
| 18 | WP-39 | Disk tier: append-only log write path + parallel load (from WP-28) | planned (owner places it) | | |
| 19 | WP-41 | Adaptive caching: disk rule weighted by active threads (from WP-27) | planned (late, owner 11-03) | | |
| 20 | WP-42 | Adaptive caching: "keep" hint / non-myopic decisions (from WP-27) | planned (late, owner 11-03) | | |
| 21 | WP-44 | DSG row loop light: single-pass row weights + header-inline `compRowWeights` (from WP-04) | planned (owner places it; WP-17 rejected → unblocked) | | |
| last | WP-43 | Final clean-up: phase references out of code docs + phase leftovers | ready (always last, owner 11-03) | | |

## Next action

**WP-36 in progress** (interpolation exploration, measurement only; prompt issued 11-04). WP-17
reverted and committed (326cef0f0); tree clean (only untracked `compile*.sh`). 11-07: no new
summary yet. If the owner's PGO build ran, `lib/` holds PGO libraries: `perf.py build normal`
before the next work session. Next master start: process the WP-36 summary.

## Open decisions for the owner

- WP-09 / WP-18 (after their summaries): adopt the proposed interface or not. WP-18 (before
  its prompt): protected `perf_e2e.cpp:81` / `perf_reference.cpp:136-144` read `tgData.mi`,
  `.coefficient`, `.tensorGrid.*` → proxy keeps the names, or the owner adapts them.
- WP-33 (after WP-32): drop / restrict / align large allocations only / keep base's `operator new`
  (WP-23: memory −5 %, dsg b1 −23 MiB, generation 1.1–1.4x; datadriven relies on it).
- WP-04 `TrieNode` 24 → 16 B (optional): `git apply results/WP-04/block-loop.patch` +
  `trienode16.patch` = −3 MiB per b0 case for b0 0.982x; only on top of the rejected block loop.
- WP-37: base's Python `DataVector` iteration (segfault today) — base API change, placement.
- WP-34: placement + mechanism (serial operator for Python callables: WP-17 measured 1 t 0.78 vs.
  16 t 25.1 µs ≈ 32x / document; WP-17's batch path rejected 11-04). WP-39: placement.
- Adaptive as the default `CachePolicy` (WP-27 follow-up; not planned, owner 11-03): cheap
  functions 0.57–0.91x of plain instead of 0.07–0.30x; expensive pay ≈ 3–7 ns per hit.
- Optional moves: WP-05 before WP-18 (0.5–0.8 s at d ≤ 3 vs. ≈ 0.05 s); WP-35 up (test
  runs hit it: WP-29a 2 of 16, WP-28 1 of 15; re-check 0 of ≈ 12).
- Owner files outdated: CLAUDE.md §5 banner "build fails otherwise" (checker never checks C++,
  WP-38); ANALYSIS.md WP-29c symbol names; SG++ install docs (optional Boost ≥ 1.81 / xxHash ≥ 0.8,
  LMDB, CI `liblmdb-dev`; `build-and-ci.md` done in WP-40).

## Do not retry

(`WP-NN <idea>: <numbers>, <why>`; details in the summaries)
- **Methodology**: bootstrap within rounds (18/114 false positives) → paired per-round t-interval;
  std → MAD; micro right after e2e 10–25 % slow → settle, calibrate, micro, e2e; refcheck ratio
  tolerance → `e_new ≤ ratio·e_ref + 4 units`; memory flag by ratio only (5/9 A/A false) → 3 % + no
  overlap. Profiles: `perf script` callchains / `--sort symoff` unusable → `perf script -F
  ip,sym,symoff,dso -G` + `addr2line -a -i` (`results/RP-3/profile_split.py`); libgomp / OpenBLAS
  shares of ms-long processes = pool spin-up. TySan: > 12 GB, false positives → `-Wstrict-aliasing`.
- WP-01 rows always along dimension 0 (b1 47.2 vs. 27.9 ns/point); helpers reading scratch per row
  2–9 % slower → copy into locals.
- WP-02 plain-double projected coefficients (round-off 9x) → double-double; global-width 24 B slots
  (RSS 36.5 → 65 MiB); builder without hidden visibility (0.973–0.987) / unaligned row loop (0.972x);
  linear child scan instead of `lower_bound` (0.948).
- WP-03 Pareto maxima as closure generators (slower d5–d20); exact sizing (count + `reserve`).
- WP-04 DSG b0 row loop: blocks of rows + single-pass block weights + `#pragma GCC unroll 1`:
  quad/dsg 1.047x (−0.79 s of 1.5–2.5 s expected; IPC 3.8, −17 of 227 instr/point), rejected
  (owner: complexity / pragmas not worth it). Parts: blocks alone ±0, `unroll 1` alone −3.4,
  `novector` worse, fused per-row weights without blocks 0 on exp, i0-outer +3. `TrieNode`
  16 B: b0 0.982x (+4.2 instr/point), −3 MiB. `-fno-strict-aliasing` now 0.955–0.965x → the
  WP-20 aliasing remainder is closed.
- WP-17 batched evaluation (one evaluator object per `SourceFunc`, virtual `evaluateBlock`,
  blocks of ≤ 256 points over the leading rows, kernels sum per block): judged 0.960x (−1.6 s,
  29/38 SLOWER; b0 0.93–0.97), rejected 11-04 (NO-GAIN). Loop fission loses the overlap of the
  latency-bound integrand with the row work; removable per-point overhead is only ≈ 5–8 instr
  (the RP-1–3 "dispatch 4–9 %" counted the unavoidable call) → no WP ranks on dispatch. C++
  batch callable 1.24–1.33x slower than per-point (block copy, scalar libm). Parts: v1 rows
  summed in the kernel's row loop + nested evaluator loop (SG 0.857, DSG 0.85–0.89;
  `-funroll-loops` prologues); `unroll 1` on evaluator / row dot (310 → 317 M instr); block caps
  16 / 64 (cap 16: SG 0.87, DSG 0.91); four partial sums in the DSG block dot (0.936 → 0.964
  only); explicit `std::fma` in `addScaled` (still 34 cases non-bitwise, 4 interplin changed).
- WP-10 unlinking Armadillo with Eigen FFT: `total` 0.97–1.02, cold first call +63…+130 ms.
- WP-11 `-fno-plt -Bsymbolic-functions` (1.015x vs. 1.024x); Clang ThinLTO (quad/sg 0.964x, refcheck
  WARN), full LTO 1.018x → GCC. Clang needs `CC=clang CXX=clang++ LINK=clang++`.
- WP-20 `FlatNestedVec` offsets as `uint32_t`: no gain.
- WP-23 header-inline `DataVector` ctors / static LTO (0.999x); dropping base's `operator new`
  before aligning per-thread data (0.982x, interplin d8 +20–45 % false sharing).
- WP-24 one path scalar = m = 1 (SG 1.28–1.48x / DSG 1.14–1.27x slower); compile-time m (≤ 5 %);
  row buffers / per-row batches (1.14–1.75x slower at m = 1); interp m-innermost (27 ulp) → innerk.
- WP-25 run-time policy branch in `evaluate` (0.987x); per-evaluation bypass flag (0.69–0.97x);
  per-thread tables (hit rate 0.47 vs. 0.82); quantized keys; `DataVectorHash` prototype; dict in the
  Python callable (≤ 1.27x); compile-time policies; arena chunks 4096 (→ 128).
- WP-26 shard counters after the index (16 t 1.12–1.30x slower); rounded keys (+2.7…5.0 ns,
  nondeterministic at 16 t) → bitwise.
- WP-29b `updateCoeffSums` out-of-line (op-cache misses +9 %, e2e 0.992x) → header-inline.
- WP-29c per-call `std::vector` for innerk factors (0.74x) → stack blocks; zero-filling `out`;
  pointer-based ops / non-inlined templates (0.972, 0.992x) → slot-based; `aligned(64)` on both
  collapse templates (op-cache misses 6x).
- WP-28 `MDB_WRITEMAP` (cold writes / loads slower, sparse preallocation to the map size); pending
  lookup in `find` / spill inline in `insert` (memory-only inserts 2.6–4.8 % slower) → out of line +
  `[[unlikely]]`; fsync in the destructor (cold persistent runs 1.3–5x slower).
- WP-27 out-of-line adaptive hit path (+17 ns/evaluation vs. "always") → inline hits; window hit
  rate during the cold start (switches mid-cost functions off early) → bound h = 1, then marginal
  rate after 2^16 lookups; memory hit rate in TemporaryStorage (misses the file's hits) → file
  rule only; counting the switching-off call as a None call (immediate probe); callable swap at
  the hook (races with concurrent calls) → mode word; `atomic_thread_fence` in the seqlock (TSan
  cannot model it) → release / acquire on the fields.

## Re-profiling

Last: RP-3 (2026-10-30; rp-3 = 0c6ecfc4b). Accepted optimizations since: 0. Features (WP-29a–d,
WP-26, WP-28, WP-40, WP-27 accepted 11-03; scalar 0.995–1.002x) count only if a group changes
≥ 1.3x or a hot-spot ranking shifts (none did).
WP-04 (11-03), WP-17 (11-04) rejected. RP-4 after 2 accepted optimizations or another rule-7
trigger (WP-36 is measurement only and refreshes the interpolation shares).

## Log

- 2026-11-04 WP-17 summary processed (NO-GAIN): **rejected** by the owner (option a; judged
  0.960x, −1.6 s; Python 28x not worth the kernel regression). Code uncommitted in the tree →
  owner saves the patch, `reject --apply`, commits. PLAN: dispatch-based estimates void; WP-34
  keeps the serial option only; WP-44 unblocked (owner places it). No RP-4. **WP-36 prompt issued.**
- 2026-11-07 Sync: WP-17 rejection committed (326cef0f0); no new summary (WP-36 still running).
  Owner's PGO build (`master_research_project/.../compile.sh`, owner-side, not a WP): cache test
  `shared_cache_and_concurrent_operator_calls` fails when its TU is PGO-compiled (FMA contraction
  in `shiftedDomain` → no spill; library correct) → added to WP-35 (owner). `lib/` may hold PGO
  libraries → `perf.py build normal` before the next work session.
- Test failures: once-seen (WP-02; 0/150 reruns); time-seeded `equidistant_nodes/sin_func_random_cube_3D`
  (seed 1793379453): WP-29a 2 of 16, WP-28 1 of ≈ 15 (0 / 300 alone) → WP-35. Report recurrences.

## Archive

- 10-24 setup (fed2cdb4e), baseline boost off (931158248). 10-25 WP-01 (5281e0b7d). 10-26 WP-11
  (9ab8f642d), WP-13 (a16733d65); progress report 044d2794d. 10-28 WP-14 (3dc286c3d), WP-15
  (a485cec02), benchmark move 666b3c601 → WP-16 (f48d2bbc1), RP-1 (b0b7faac7), WP-20 (29ade3a58).
  10-29 WP-02 (5215417ea), RP-2 (50a8737e0: 1.604x), WP-03 (8adcb9536; WP-12 dropped), caching
  prototype removed (6dfcf971a), WP-23 rejected (2221501b2; 0273aa1d7). 10-30 WP-24 (8118ea9a2),
  WP-25 (5ced35885; order RP-3 → WP-29a–d → WP-26 → WP-28 → WP-27 → WP-04 → WP-17), CLAUDE.md
  compacted (0c6ecfc4b), RP-3 (e1257538d: 1.818x; WP-10 dropped), WP-29a (6dad1b375 + 984c646b5).
  10-31 WP-29b (a14f723a0), WP-29c (fac9c5787; owner's tools refactor; WP-36 before WP-06/07/08/09),
  WP-29d (51a6207bf; follow-ups WP-37, WP-38). 11-01 WP-26 checkpoint a66876cda, accepted
  (ce575ac0d; decisions 1 B, 2 both, 3 memoryLimit + nShards, 4 same class, 5 Boost.Unordered +
  XXH3, 6 fused `std::fma`; owner's refactoring: virtual `finishOperatorCall()`, dependency header,
  slicing rejected at compile time; hook cost → WP-17); WP-28 issued (disk fields in-session),
  checkpoint 73d6b3ac6, refactoring 1e279c1d0 → re-check; owner: all WP-27 decisions in-session.
  11-02 WP-28 accepted (ef1e4aaf4) after the re-check; WP-40 issued (owner: micro compaction);
  WP-40 accepted (08df92b69; micro 827 → 305; follow-ups → WP-38), WP-27 issued. 11-03 WP-27
  accepted (5a1229aab; follow-ups WP-41, WP-42 late; Adaptive-as-default not planned); owner:
  comments rule in CLAUDE.md §5 → final WP-43; owner's thesis-benchmark commits; WP-04 issued; WP-04
  rejected (e5bd574ce; follow-ups WP-44, block-as-batch → WP-17; protected files rehashed), WP-17 issued.
