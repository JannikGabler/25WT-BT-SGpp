# Optimization history — combigrid

Finished work packages (accepted / rejected / dropped) with their results, and the archive of the
master's log. Maintained by the master session; current state: [STATUS.md](STATUS.md). Commits:
`git log --oneline --grep 'Accept\|Reject'` (owner's messages "Accept/Reject WP-NN: …").

## Finished work packages

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00, 11, 13–16, 20 | Setup; toolchain; flags; memory harness; benchmark move; `FlatNestedVector` | accepted | nosi 1.023x; strict aliasing 0.988x; quad/dsg 1.037x | [summaries/](summaries/) |
| – | WP-01, 02, 03 | SG quad per point; DSG trie at high d; MI-set closure | accepted | quad/sg 2.4x; quad/dsg 1.46x (b1 d20 5.34x); mivec 5.08x (d20 531x) | [01](summaries/WP-01-sg-quad-per-point.md) [02](summaries/WP-02-dsg-trie-work.md) [03](summaries/WP-03-downwards-closure.md) |
| – | RP-1–6 | Re-profilings | accepted | **rp-6 2.014x** (d ≥ 7 1.99x): quad/sg 2.74x, quad/dsg 1.51x (48.7 % of the suite), interp/sg 1.56x, interplin/sg 1.41x, mivec 8.49x; memory 0.716; RP-6 lever: largest-first TG order (→ WP-77) | [RP-5](summaries/RP-5-reprofiling.md) [RP-6](summaries/RP-6-reprofiling.md) |
| – | WP-24, 29a–d | Vector-valued (design T): exploration + SG, DSG, interpolation, Python | accepted | scalar 0.995–1.002x; m = 32: SG 1.29–2.68x, DSG 1.96–2.96x, interp 1.25–2.12x | [summaries/](summaries/) |
| – | WP-25, 26, 28, 40, 27 | Caching (design W): in memory, LMDB disk tier, library-free headers, adaptive policy | accepted | judged 0.998–1.001x; 1 µs fn warm 6.6–14.8x; Python warm 1309–2505x; Adaptive cost-0 fn 0.57–0.86x of plain | [26](summaries/WP-26-caching-in-memory.md) [28](summaries/WP-28-caching-disk-tier.md) [40](summaries/WP-40-caching-headers-micro-compaction.md) [27](summaries/WP-27-caching-adaptive-policy.md) |
| – | WP-04, 17, 23 | DSG row loop; batched evaluation; base cross-library calls | rejected | WP-04 quad/dsg 1.047x (complexity); WP-17 0.960x; WP-23 0.999x (→ WP-32 / 33) | [04](summaries/WP-04-dsg-row-loop.md) [17](summaries/WP-17-batched-evaluation.md) [23](summaries/WP-23-cross-library-calls.md) |
| – | WP-10, 12, 19, 38, 53, 59, 75 | Armadillo linking; toolchain re-measure; SG load balancing (→ WP-56); hygiene (→ backlog); integrand placement; interp guided → dynamic (→ WP-77, RP-6); byte-type stores (RP-6: no candidates) | dropped | | |
| – | WP-36, 08, 06, 52 | Interpolation: exploration; corner loop; SG global kernel; shared weighted-sum kernels | accepted | interplin/sg 1.29x (−1.02 s); interp/sg 1.35x (−1.47 s); WP-52 refactoring 0.999x | [36](summaries/WP-36-interpolation-exploration.md) [08](summaries/WP-08-corner-loop.md) [06](summaries/WP-06-global-interp-kernel.md) [52](summaries/WP-52-weighted-sum-kernels.md) |
| – | WP-47, 45, 50 | `DeltasSparseGrid` linear / global interpolation + Python bindings (+ judged `interp(lin)/dsg` cases) | accepted | DSG vs. SG 16 t: interplin d10 4.56x (d2 0.60x), interp cc d7 8.48x, d2 3.40x; Python 2.2–7.0x | [47](summaries/WP-47-dsg-linear-interpolation.md) [45](summaries/WP-45-dsg-global-interpolation.md) [50](summaries/WP-50-dsg-python-bindings.md) |
| – | WP-18 | Tensor-grid storage: unit + domain `NodeTable`s, SoA, views, explicit domain / unit accessors | accepted (11-15) | e2e 1.005x; **memory 0.810** (sg equi d2 L20 620 → 346 MiB, dsg b1 d20 218 → 83) | [WP-18](summaries/WP-18-tensor-grid-storage.md) |
| – | WP-05 | SG quadrature at low d: shared lazy weight table (`misc::QuadWeightTable`), CC rule in place | accepted (11-15) | cc d2 L20 1.555x (−460 ms); e2e 1.010x; **memory 0.938** (sg cc d2 862 → 310 MiB) | [WP-05](summaries/WP-05-cc-weight-table.md) |
| – | WP-56 | SG quadrature: real FFT of length m (re-pinned hashes), eager weight table (dynamic, largest row first), TG loop guided → dynamic | accepted (11-15) | cc d2 L20 792 → 438 ms (1.84x), equi b1 d10 L7 1.22x, mivec cc d10 1.32x; −551 ms; **memory 0.981** (cc d2 L20 323 → 195 MiB); weights \|Δ\| ≤ 4.2e-22 | [WP-56](summaries/WP-56-cc-weights-critical-path.md) |
| – | WP-62 | Dimension-adaptive refinement: exploration + implementation plan (owner's request) | accepted (11-17) | GG + profit 8.5–42x fewer evaluations than the complete CT (d10); d100 L2 CT 4.0e-3 vs. adaptive 7.5e-8 (2e5 evals); bookkeeping 10.5–47.5 ns/eval; judged suite unchanged | [WP-62](summaries/WP-62-dimension-adaptive-exploration.md) |
| – | WP-49 | DSG lin. interp.: slot weights by sum factorization over closure fibers, thread-count independent; gate 640 visits | accepted (11-17, 59f2899e4; DSG pinning hashes re-pinned) | interplin/dsg d10 1.58x, d8 1.54x, d5 1.50x, d2 1.13x; −0.363 s; memory 0.995 (d10 MEM+ 15.3 → 15.9 MiB, d8 MEM−) | [WP-49](summaries/WP-49-dsg-interplin-slot-weights.md) |
| – | WP-76 | Judged suite: cases with expensive source functions (group `expensive/`, owner's D1–D6; protected files by patch) | accepted (11-17, 76a83f06b) | 21 judged cases 17.77 s + 4 cached 1.05 s per pass (suite ≈ 55.5 s); A/A within noise; refcheck OK (146 refs); old cases 1.008x (placement), memory 1.011; efficiency mivec SG 0.42 / DSG 0.62, interplin/dsg d5 0.48 | [WP-76](summaries/WP-76-expensive-source-function-cases.md) |
| – | WP-73 | Cross-call operator data cache: clear / entries / bytes on `SparseGridBase`, options per operator (`useCrossCallOperatorDataCache`), concurrency (a); DSG `interplin` parallel-region bug fix (owner's request) | accepted (11-18, b9bf8bb65) | judged 0.999x, `expensive/` 1.001x, memory 1.005; repeated calls flag on 16 t: SG quad cc d2 L20 1.19x, DSG quad 1.03–1.15x | [WP-73](summaries/WP-73-operator-data-cache-control.md) |
| – | WP-82 | DSG quadrature: cross-call cache on by default via the new `DeltasSparseGridQuadratureOptions` (`QuadratureOptions` = `SparseGrid` only; API change agreed) | accepted (11-18, 37a84e1af) | judged 1.000x, `expensive/` 0.999x, memory 1.004; repeated DSG quad calls 16 t 1.04–1.31x; retained index 0.9–17.8 MiB (e2e), cc d2 L20 64 MiB (owner: keep, documented) | [WP-82](summaries/WP-82-dsg-quad-cache-default.md) |
| – | WP-79 | Textbook operators `textbookQuadrature` / `textbookInterpolate` / `textbookInterpolateLinear` (naive serial per-TG CT on `SparseGrid`, one vector core per operator, C++ + Python; owner's request; incl. the owner's rework) | accepted (11-21, d3b7f00ba) | feature: judged 1.002x, `expensive/` 0.999x, memory 1.008 (0 flags); refcheck = known WARN; textbook / optimized 1 t: quad 1.40–1.41x, interp 1.37–1.47x, interplin 1.13–1.17x | [WP-79](summaries/WP-79-textbook-operators.md) |
| – | WP-83 | SG quadrature with cheb1 nodes: rules obtained serially before the OpenMP regions (`std::terminate` → catchable exception), new `FejerFirstQuadRule(includeBoundary)` (Fejér-1 / interpolatory with boundary nodes, Bluestein FFT), additive virtual `NodeGenFunc::getQuadRule(n, includeBoundary)` (owner 11-22); bug fix + feature, incl. the owner's rework | accepted (11-22, commit pending) | feature: judged 0.996x (noise), `expensive/` 0.999x, memory 1.003 (0 flags); refcheck = known WARN; existing results bitwise; cheb1 rule ≈ 4.8x CC at equal n (n = 131073: 29.9 vs. 6.25 ms) | [WP-83](summaries/WP-83-cheb1-quadrature.md) |

Re-profiling snapshots: `rp-1` f48d2bbc1, `rp-2` 5215417ea (1.604x), `rp-3` 0c6ecfc4b (1.818x),
`rp-4` 6ac45a05d (1.933x), `rp-5` 3261399ec (1.973x; 38 cases 69.93 → 35.45 s; memory geomean 0.724;
suite of 48 cases 37.44 s), `rp-6` 37a84e1af (2.014x; 38 cases 69.89 → 34.70 s; memory 0.716; suite
36.33 s, `expensive/` 17.77 s). Dates: RP-1 10-28, RP-2 10-29, RP-3 10-30, RP-4 11-13, RP-5 11-15,
RP-6 11-18.

## Log (older entries, newest first)

- 2026-11-21 WP-79 summary processed: **accepted** by the owner incl. the owner's staged rework
  (d3b7f00ba): `textbookQuadrature` / `textbookInterpolate` / `textbookInterpolateLinear` (C++ +
  Python), judged 1.002x, `expensive/` 0.999x, memory 1.008 (0 flags), refcheck = known WARN.
  Follow-ups (owner): new **WP-83** (cheb1 SG quadrature `std::terminate` + cheb1 quadrature rule;
  owner: before WP-61); `interpolateLinear` boundary-cell doc sentence → backlog K. Not planned
  (owner): per-TG values variant for adaptive indicators, `CPPLINT.cfg` filter (backlog A1 stays).
  2 "Do not retry" lines. Ranks unchanged (no new optimization evidence). WP-83 prompt issued.
- 2026-11-18 Owner: **new ranking rule** (judgement-based over all inputs, no formula; all case
  types count; complexity, interactions, confidence; owner overrides) + **case-realism note** in
  every prompt; skill `perf-master` step 4 + prompt template updated (owner's permission), PLAN.md
  header + "Ranking inputs" table. Re-ranking: WP-61 8 → 1 (before WP-77: bitwise-neutral
  reordering, one re-pinning), WP-77 1 → 2, WP-21 2 → 4 (one group, < 1 % of quad/dsg), WP-74 3
  (6–10 % of interplin/dsg + expensive d5), WP-60 7 ↔ WP-58 8 (session with WP-57). Master's
  recommendations for owner-placed WPs: WP-44 ≈ rank 5, WP-78 size soon.
- 2026-11-18 RP-6 summary processed: **accepted** by the owner (commit pending). baseline → rp-6
  2.014x (38 cases 69.89 → 34.70 s), memory 0.716; suite 36.33 s (rp-5 → rp-6 1.031x), `expensive/`
  17.77 s. Re-rank: WP-77 extended (largest-first TG order in all TG loops; owner: TG split as an
  optional measured variant) → rank 1; WP-59 merged into WP-77 (guided → dynamic predicted to lose);
  WP-74 extended (evaluation chunks) → rank 3; WP-75 dropped (no candidates); WP-51 = row split
  only; interp/sg basis-value table: no WP. WP-82 committed (37a84e1af). WP-79 prompt issued.
  Owner asked whether `expensive/` counts like the cheap cases: no (D3, own sum line, unranked).
- 2026-11-18 WP-82 summary processed: **accepted** by the owner (37a84e1af): new
  `DeltasSparseGridQuadratureOptions` (default on), `QuadratureOptions` = SG only (off). Judged 1.000x,
  `expensive/` 0.999x, memory 1.004 (0 flags); repeated DSG quad calls 16 t 1.04–1.31x. Large retained
  indices (cc d2 L20 64 MiB): owner option (a), keep as documented. Serial index build → WP-60;
  stale WP-73 scripts → backlog J. RP-6 prompt issued.
- 2026-11-17 Owner: new **WP-81** (explanation / visualization foundations: exploration, requirements
  formulated with the owner in-session, then implementation WPs) → unranked. WP-80's one-core rule
  and `tools/` placement confirmed (the WP-79-specific rules apply to WP-79 only).
- 2026-11-17 Owner: new **WP-80** (grid-evaluation operator: each distinct grid point once; cache
  warm-up + testing; any source function; name / result / dedup in-session) → unranked.
- 2026-11-17 Owner: new **WP-79** (textbook operator variants: naive per-TG CT with standard
  solvers, understandability first, naming + scalar / vector decided in-session; no DSG variants; one
  core per operator; helpers in the operator's `.cpp`, not `tools/`) → after RP-6.
- 2026-11-17 WP-76 summary processed: **accepted** by the owner (commit pending; owner applied the
  patch, rehashed, added `noise-expensive.json` to `baseline/noise_e2e.json`). Group `expensive/`
  (D1–D6; ranking unchanged). Owner: SG operators keep the standard CT (no dedup WP). New WP-77
  (SG / mivec load balance, rank 3), WP-78 (cache overhead, owner places it); WP-59 + SG interplin;
  RP-6 profiles `expensive/`. WP-73 prompt reissued.
- 2026-11-17 WP-49 summary processed: **accepted** by the owner incl. the re-pinned DSG
  `grid_storage_pinning` hashes (commit pending). interplin/dsg d10 1.58x … d2 1.13x, −0.363 s, memory
  0.995 (d10 MEM+ +0.6 MiB). New WP-74 (fused evaluation / vectorized transform), WP-75 (byte-type
  stores); RP-6 entry; ranks −1. Counter 2 → RP-6 due; WP-73 prompt issued (owner: before RP-6).
- 2026-11-17 Owner: new **WP-76** (judged suite + expensive source functions; plan with open decisions,
  owner decides in-session, then implementation) before the next regular WP → prompt issued; WP-73
  back to ready (its prompt is superseded, reissue after WP-76).

## Archive (commits by date)

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
  11-18 WP-73 b9bf8bb65 (WP-82 from the owner's default decision), WP-82 37a84e1af (large index:
  option (a)), RP-6 accepted (WP-77 extended + optional TG split; WP-59 / WP-75 dropped).
  11-21 WP-79 d3b7f00ba (WP-83 defined; owner: before WP-61). 11-22 WP-83 accepted (checkpoint
  18346c71d + owner's rework; WP-84 / WP-85 defined from its follow-ups).
