# Optimization status — combigrid (master file)

Last processed: WP-29b (2026-10-31) · Next free WP number: 36 · Re-profilings done: 3 (RP-1 10-28, RP-2 10-29, RP-3 10-30; RP-0 = setup)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), GCC
  15.2, 16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (2.0 GHz),
  profile `performance`, AC, idle. site_scons flags: `-fno-semantic-interposition` (WP-13, GCC
  `OPT=1`), strict aliasing on (WP-14; `-fno-strict-aliasing` only for the SWIG Java wrapper).
- **Baseline**: setup commit **fed2cdb4e**; snapshot `baseline` in `.work/snapshots/` (if cleaned:
  `perf.py snapshot baseline --commit fed2cdb4e`); numbers in `baseline/`, ANALYSIS.md. Built
  with its own flags (no nosi, `-fno-strict-aliasing`): flag changes count as progress. No
  re-recording. Snapshots `rp-1` = f48d2bbc1, `rp-2` = 5215417ea (1.604x), **`rp-3` = 0c6ecfc4b**
  (e2e 69.92 → 38.45 s, 1.818x; d ≥ 7 1.87x; memory geomean 1.014).
- **Memory** (WP-15): `run e2e` / `ab e2e` report each e2e process's peak RSS (GNU time, outside
  the process); floor 10.2 MiB; `MEM+`/`MEM-` = |B/A − 1| > 3 % and no overlap of the rounds.
  `run e2e` = one process per case (`baseline/e2e_t16.jsonl` was one process → compare within one
  mode). `ab micro` has 0 warm-ups (allocation-heavy cases: ratios valid, absolutes from `run micro`).
- **Machine**: a reboot re-enables boost (throttling up to 1.6x); if `envcheck` fails on boost, ask
  the owner. Full e2e A/B ≈ 18–23 min (sessions filter). `scons -c` is fine. Sanitizer /
  heap-profiler runs need `systemd-run --user --scope -p MemoryMax=8G`. OpenBLAS pool: +63…+130 ms
  on each process's first operator call, not in `total` (RP-3).
- **Code placement** (WP-02): hot loops shift ±3–4 % with unrelated code changes (op-cache misses);
  DSG row loop `[[gnu::aligned(64)]]`. Check op-cache misses before attributing a shift; LTO may
  recompile unchanged code (RP-2: closure 1.06–1.13x). Generation timings: ±20 ms page-fault noise.
- **Toolchain**: also Clang 21.1.8 + libomp + lld, JDK 21 (`SG_JAVA=1`); `perf/scripts/toolchain.py`
  builds variants (`gcc-sa` WP-14, `gcc-nsa` = judged + `-fno-strict-aliasing` WP-20).
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed, labelled and
  weighed against API / functionality / maintainability / readability; functional / interface /
  flag changes only after consulting the owner; errors stay in the same order of magnitude
  (refcheck OK/WARN/FAIL); high dimensions first; STATUS.md may exceed ~120 lines slightly.
  Dated decisions:
  - Memory (10-28): runtime ranks; peak memory reported with every runtime change and weighed;
    reduction is a valid secondary goal.
  - Prompt notes from the PLAN.md header: strict aliasing (10-28, every code-changing WP),
    dimension-adaptive refinement (10-29, API-designing / -changing WPs), Python `.venv/` (10-30,
    every prompt).
  - Vector-valued (10-30, WP-24): design T (separate `VectorSourceFunc`, template kernels,
    overloads); component k == scalar k within round-off. Docs: WP-29b–d update the vector-valued
    lines of CLAUDE.md §2/§6, `module-reference.md`, `python-bindings.md` themselves (others: list
    in the summary).
  - Caching (10-30, WP-25): design W (caching in the stored callable of a plain `SourceFunc`, no
    per-evaluation check), bitwise user-domain keys, sharded in-memory table, stop inserting at
    the limit, flat-file disk tier; API details before WP-26.
- **Refcheck WARN case** (owner 10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue constant ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session ("Accept/Reject WP-NN: …": code, summary,
  `results/WP-NN/`, STATUS/PLAN); the tree is clean before the next work session.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00, 11, 13–16, 20 | Setup; toolchain; flags; memory harness; benchmark move; `FlatNestedVector` | done / accepted | WP-13 nosi 1.023x; WP-14 strict aliasing 0.988x; WP-16 A/A 1.001x; WP-20 quad/dsg 1.037x | [summaries/](summaries/) |
| – | WP-01 | SG quadrature: per-grid-point work | accepted | quad/sg 2.4x (22.3 → 9.4 s), b1 d20 4.7x, mivec 1.45x; memory sg/cc-uq/b0/d2/L20 806 → 1076 MiB | [WP-01](summaries/WP-01-sg-quad-per-point.md) |
| – | WP-02 | DSG quadrature: trie work at high d | accepted | quad/dsg 1.46x (26.14 → 17.91 s), b1 d20 5.34x; memory b1 d20 328 → 217 MiB, 6 b0 MEM+ (+1.9…+6.4 MiB) | [WP-02](summaries/WP-02-dsg-trie-work.md) |
| – | WP-03 | Downwards closure of MI sets | accepted | mivec 5.08x (6.32 → 1.24 s), d20 531x; memory 0.992 | [WP-03](summaries/WP-03-downwards-closure.md) |
| – | RP-1–3 | Re-profilings | accepted | **rp-3 1.818x** (69.92 → 38.45 s; d ≥ 7 1.87x): quad/sg 2.42x, quad/dsg 1.50x, interp 1.16x, interplin 1.10x, mivec 7.42x; memory 1.014 (8 MEM+ / 3 MEM−) | [RP-1](summaries/RP-1-reprofiling.md), [RP-2](summaries/RP-2-reprofiling.md), [RP-3](summaries/RP-3-reprofiling.md) |
| – | WP-10, WP-12 | Armadillo linking; toolchain re-measurement | dropped | WP-10: cold first call only; WP-12: Clang's lead gone with WP-03 | |
| – | WP-23 | Cross-library calls into `libsgppbase` (measurement) | rejected: NO-GAIN | base ≤ 0.09 % at d ≥ 7; e2e 0.999x; allocator finding → WP-32 / WP-33 | [WP-23](summaries/WP-23-cross-library-calls.md) |
| – | WP-24, WP-25 | Explorations: vector-valued (design T); caching (design W) | accepted | T scalar ≈ 0, vector m = 32 1.3–2.8x; W costs 0, Python warm ~2000x at 16 t; benchmarks `perf_vector_proto.hpp`, `perf_cache_proto.hpp`, `perf_cache_counts.cpp` kept | [WP-24](summaries/WP-24-vector-valued-exploration.md), [WP-25](summaries/WP-25-caching-exploration.md) |
| – | WP-29a | Vector-valued (a): `VectorSourceFunc`, value types, SG quadrature (+ owner's restructuring) | accepted | scalar 1.002x, memory 1.002, refcheck OK; vector vs. m scalar runs at m = 32: d10 2.68, b1 d20 2.48, d2 L16 1.29; m = 1 0.66–0.93 | [WP-29a](summaries/WP-29a-vector-sg-quadrature.md) |
| – | WP-29b | Vector-valued (b): DSG quadrature | accepted (10-31) | scalar 1.001x (17.748 → 17.738 s), memory 0.999, refcheck OK; vector at m = 32: b0 d10 2.96, b1 d20 1.96 (incl. sub-tries 3.18), bitwise; m = 1 0.93–0.96 | [WP-29b](summaries/WP-29b-vector-dsg-quadrature.md) |
| 1 | WP-29c–d | Vector-valued: (c) interp + interplin, (d) bindings | **WP-29c in progress** (10-31); d ready | | |
| 2 | WP-26 | Caching: wrapper W, in-memory table, policies, `pysgpp.CachedSourceFunc` | needs decision (API details 1–6; after WP-29d) | | |
| 3 | WP-28 | Caching: disk tier | needs decision (after WP-26) | | |
| 4 | WP-27 | Caching: adaptive policy | needs decision (after WP-28) | | |
| 5 | WP-04 | DSG quadrature: row loop (b0) (+ `TrieNode` 24 → 16 B) | ready (after caching) | | |
| 6 | WP-17 | Batched source-function evaluation (proposal) | ready (after WP-04) | | |
| 7 | WP-09 | Multi-point interpolation API (proposal) | ready | | |
| 8 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09) | | |
| 9 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09) | | |
| 10 | WP-21 | DSG b1: slack break in the projection DFS + sub-trie reuse | ready (after WP-04) | | |
| 11 | WP-18 | Tensor-grid storage: shared 1D nodes + struct of arrays (memory; proposal) | ready (protected-benchmark decision first) | | |
| 12 | WP-05 | SG quadrature at low d: CC 1D weights + scratch on demand | ready | | |
| 13 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 14 | WP-19 | SG quadrature: load balancing at low d | ready (after WP-05) | | |
| 15 | WP-32 | Cache-line-align per-thread scratch / shared query data | ready (≈ 0 in the judged build) | | |
| 16 | WP-33 | Base's global `operator new` replacement | needs decision (after WP-32) | | |
| 17 | WP-31 | MI-set cleanups (O(n·d) closedness test, dead helpers, small cleanups) | ready (low) | | |
| 18 | WP-35 | Flaky time-seeded tests + seed audit (test-only) | ready (low; owner may move it up) | | |
| 19 | WP-30 | Vector-valued: specific optimizations (small m) | planned (after WP-29) | | |
| 20 | WP-34 | Python source functions: GIL contention at 16 t (36x) | planned (owner places it) | | |

## Next action

WP-29c (global + linear interpolation) is running (prompt issued 2026-10-31). Owner: commit
"Accept WP-29b: …" first (tree clean): WP-29b code, tests, benchmarks, summary, `results/WP-29b/`,
STATUS/PLAN and the last master session's doc updates (CLAUDE.md, module-reference.md,
python-bindings.md); `compile{ASan,Debug,Normal}.sh` are untracked (snapshots "dirty": commit or
ignore). WP-26 doc lines: decide then. Next master start: process WP-29c, then issue WP-29d.

## Open decisions for the owner

- WP-09 / WP-17 / WP-18 (after their summaries): adopt the proposed interface or not.
- WP-18 (before its prompt): protected `perf_e2e.cpp:81` / `perf_reference.cpp:136-144` read
  `tgData.mi`, `.coefficient`, `.tensorGrid.*` → proxy keeps the names, or the owner adapts them.
- Optional: by the d ≥ 7 rule WP-18 (≈ 0.05 s; −96…−114 MB dsg b1 d20) precedes WP-05 (0.5–0.8 s
  at d ≤ 3; −350…−800 MiB sg d2); the owner may move WP-05 up.
- WP-33 (after WP-32): drop / restrict / align only large allocations / keep base's `operator new`
  replacement (WP-23: memory −5 %, dsg b1 −23 MiB, generation 1.1–1.4x; datadriven relies on it).
- WP-26 (after WP-29d): API details 1–6 (PLAN.md WP-26 "Open decisions"; rec. B
  `CachedSourceFunc`, `CacheConfig`, table, domain mapping, base vs. combigrid). WP-28 / WP-27:
  disk / adaptive fields of `CacheConfig`.
- WP-34: placement and mechanism (serial operator for Python callables / WP-17 batches / document).
- Optional: WP-35 ranks low by e2e, but repeated test runs hit it (WP-29a: 2 of 16) → may move up.

## Do not retry

(`WP-NN <idea>: <numbers>, <why>`; details in the summaries)
- **Methodology**: WP-00 bootstrap within rounds (18/114 false positives) → paired per-round
  t-interval; plain std → MAD; micro right after e2e 10–25 % slow → settle, calibrate, micro, e2e;
  refcheck ratio tolerance missed 5x growth → `e_new ≤ ratio·e_ref + 4 units`. WP-15 memory flag
  by ratio only: 5/9 A/A false → 3 % + no overlap; `os.wait4` gives the driver's peak. RP-1/2
  `perf script` callchains / `perf report --sort symoff` unusable → `perf script -F
  ip,sym,symoff,dso -G` + `addr2line -a -i` (`results/RP-3/profile_split.py`); non-precise
  op-cache sampling has skid; libgomp / OpenBLAS shares of ms-long processes = pool spin-up.
  WP-14 TySan: > 12 GB per binary, only false positives → `-Wstrict-aliasing=3/2`; rerun only
  with `OPENBLAS_NUM_THREADS=1` + cgroup cap.
- WP-01 rows always along dimension 0: b1 47.2 vs. 27.9 ns/point → multi-node dimensions only.
  Helpers reading scratch per row: 2–9 % slower (opaque `evaluate`) → copy into locals first.
- WP-02 projected coefficients in plain double: round-off 9x at b1/d15 → double-double. Global-width
  24 B slot blocks: b0/d10 RSS 36.5 → 65 MiB → 1-norm-bounded split 8 B / 16 B arrays. Builder
  split without hidden visibility (`sub_tries` 0.973–0.987) or unaligned row loop (e2e 0.972x)
  → hidden + aligned. Linear child scan instead of `lower_bound`: `sub_tries` b1/d20 0.948.
- WP-03 Pareto maxima as closure generators: slower in total at d5–d20 (maxima cost more than the
  recursion saves). Exact sizing (count + `reserve`) slower than geometric growth.
- WP-10 do not link Armadillo / OpenBLAS with Eigen FFT: `total` 0.97–1.02, cold first call
  +63…+130 ms (mivec d20 22x), floor +1…4.5 MiB → not worth the build change.
- WP-11 `-fno-plt -Wl,-Bsymbolic-functions` on top of nosi: 1.015x vs. 1.024x. Clang ThinLTO: e2e
  1.034x but quad/sg 0.964x, quad/dsg 0.980x, refcheck WARN → GCC kept; full LTO 1.018x. Clang
  needs `CC=clang CXX=clang++ LINK=clang++`.
- WP-20 `FlatNestedVec` offsets as `uint32_t`: no gain (the alias that matters is pointer-to-`T`).
- WP-23 header-inline `DataVector` ctors / static LTO: e2e 0.999x → not worth a base ABI change.
  Dropping base's `operator new` replacement without aligning per-thread data: e2e 0.982x,
  interplin d8 +20–45 % (false sharing) → WP-32 first, then WP-33.
- WP-24 one path (scalar = m = 1 of runtime m): SG 1.28–1.48x / DSG 1.14–1.27x slower at m = 1;
  compile-time m 1.11–1.21x slower, ≤ 5 % vector gain; row buffers 1.24–1.75x, per-row batch
  1.14–1.41x slower at m = 1 → design T. Interp m-innermost: up to 27 ulp vs. scalar → innerk.
- WP-25 run-time cache-policy branch in `SourceFunc::evaluate`: e2e 0.987x, 15/38 SLOWER → design
  W. Per-evaluation bypass flag: 0.69–0.97x → install the raw callable. Per-thread tables lose hits
  (0.47 vs. 0.82); quantized keys; prototype `DataVectorHash` (dyadic keys in 1/256 shards) and
  table (insert 2040 ns at 16 t); dict in the Python callable ≤ 1.27x; compile-time policies
  (doubles kernel code). Benchmark: arena chunks 4096 → 128 records; cold → hot query keys.
- WP-29b `updateCoeffSums` out-of-line with two kernel instantiations: LTO stops inlining it into
  the scalar row loop (op-cache misses +9 %) and `childBlock` into `addChildren` → e2e 0.992x,
  5 SLOWER → header-inline.

## Re-profiling

Last: RP-3 (2026-10-30; rp-3 = 0c6ecfc4b). Accepted optimizations since: 0. Features (WP-29a–d,
WP-26–28; scalar ≈ 0) count only if a group changes ≥ 1.3x or a hot-spot ranking shifts (WP-29a
1.002x, WP-29b 1.001x: no). RP-4 after 2 accepted optimizations (WP-04, WP-17, …) or another
rule-7 trigger.

## Log

- 2026-10-31 WP-29b accepted (owner; commit pending): vector DSG quadrature (2 overloads, wrong size
  → `std::invalid_argument` like SG), context without the function; scalar 1.001x, memory 0.999,
  refcheck OK; vector at m = 32 1.96–3.18x. Follow-ups → PLAN.md: WP-04 (functor by value /
  register accumulator; `childBlock` `always_inline`), WP-30 (DSG m = 1), WP-29c (stale comments),
  WP-29d (stale `%ignore`), WP-31 (`using namespace`, unused variable). No RP-4. WP-29c issued.
  STATUS.md compacted (owner's request).
- Test failures: once-seen (WP-02; 0/150 reruns); WP-29a: 2 of 16 normal runs failed a
  time-seeded test (one `equidistant_nodes/sin_func_random_cube_3D` seed 1793379453) → WP-35;
  WP-29b: 0 of 31. Report recurrences.

## Archive

- 10-24 setup (fed2cdb4e), baseline boost off (931158248). 10-25 WP-01 (5281e0b7d). 10-26 WP-11
  (9ab8f642d), WP-13 (a16733d65); progress report 044d2794d. 10-28 WP-14 (3dc286c3d), WP-15
  (a485cec02), benchmark move 666b3c601 → WP-16 (f48d2bbc1), RP-1 (b0b7faac7), WP-20 (29ade3a58).
  10-29 WP-02 (5215417ea), RP-2 (50a8737e0: 1.604x), WP-03 (8adcb9536; WP-12 dropped), caching
  prototype removed (6dfcf971a), WP-23 rejected (2221501b2; 0273aa1d7). 10-30 WP-24 (8118ea9a2),
  WP-25 (5ced35885; order RP-3 → WP-29a–d → WP-26 → WP-28 → WP-27 → WP-04 → WP-17), CLAUDE.md
  compacted (0c6ecfc4b), RP-3 (e1257538d: 1.818x; WP-10 dropped), WP-29a (6dad1b375 + 984c646b5).
