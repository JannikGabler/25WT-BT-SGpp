# Optimization status — combigrid (master file)

Last processed: WP-02 (2026-10-29) · Next free WP number: 23 · Re-profilings done: 1 (RP-1, 2026-10-28; RP-0 = setup; RP-2 issued 2026-10-29)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), 16
  threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (all cores at
  2.0 GHz), platform profile `performance`, AC, idle machine. GCC 15.2 stays the judged compiler.
  Flags from site_scons (not compileNormal.sh/perf.py): `-fno-semantic-interposition` (WP-13, GCC
  `OPT=1`); strict aliasing on (WP-14: `-fno-strict-aliasing` only for the SWIG Java wrapper).
- **Baseline**: setup commit **fed2cdb4e** (source of 84299eeca); snapshot `baseline` in
  `.work/snapshots/` (if cleaned: `perf.py snapshot baseline --commit fed2cdb4e`); numbers in
  `baseline/`, ANALYSIS.md. Own site_scons flags (no nosi, `-fno-strict-aliasing`): flag changes
  count towards the progress (WP-13 1.023x, WP-14 0.988x). No re-recording. **At RP-1** (`rp-1`
  = f48d2bbc1): e2e 69.95 → 53.41 s (1.31x), memory geomean 1.028 (3 `MEM+`: WP-01 scratch).
- **Memory** (WP-15): `run e2e`/`ab e2e` report the peak RSS of every e2e process (GNU time
  launcher, outside the process, runtime impact nil); process floor 10.2 MiB; flag `MEM+`/`MEM-`
  = |B/A − 1| > 3 % and no overlap of the rounds. `run e2e` runs one process per case (owner
  2026-10-28); `baseline/e2e_t16.jsonl` was one process → compare per-case times within one mode.
  `ab micro` runs 0 warm-ups (allocation-heavy cases: ratios valid, absolutes from `run micro`).
- **Machine note**: boost off since 2026-10-24 (throttling up to 1.6x with boost on); a reboot
  re-enables it, the owner disables it manually — if `envcheck` fails on boost, ask the owner.
  Full e2e A/B ≈ 18–23 min (sessions filter). `scons -c` is fine (owner 2026-10-25). Sanitizer
  and heap-profiler runs need a memory cap (`systemd-run --user --scope -p MemoryMax=8G`).
- **Code placement** (WP-02): a hot loop can shift ±3–4 % when unrelated code changes, with
  identical instructions (op-cache misses). The DSG row loop is `[[gnu::aligned(64)]]` since
  WP-02. If unchanged code shifts, check op-cache misses before attributing the shift.
- **Toolchain**: GCC 15.2 (judged); Clang 21.1.8 + libomp + lld; JDK 21 (`SG_JAVA=1`).
  `perf/scripts/toolchain.py` (WP-11; `gcc-sa` WP-14, `gcc-nsa` = judged + `-fno-strict-aliasing`
  WP-20) builds compiler/flag variants.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed, labelled and
  weighed against API/functionality/maintainability/readability; functional/interface/flag
  changes only after consulting the owner; errors stay in the same order of magnitude (refcheck
  OK/WARN/FAIL); high dimensions first. **Memory** (2026-10-28): runtime ranks; peak memory is
  reported with every runtime change and weighed; reduction is a valid secondary goal. **Strict
  aliasing** (2026-10-28): every code-changing WP prompt carries the PLAN.md header note.
- **Refcheck WARN case** (owner 2026-10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue constant ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session (one per WP: code, summary, `results/WP-NN/`,
  STATUS/PLAN; "Accept/Reject WP-NN: …"); the tree is clean before the next work session.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00, 11, 13–16 | Setup; toolchain measurement; flags; memory harness; benchmark move | done / accepted | WP-11 → WP-13 nosi (e2e 1.023x); WP-14 strict aliasing (0.988x); WP-15 MEM± = 3 % + no overlap; WP-16 A/A 1.001x | [summaries/](summaries/) |
| – | WP-01 | SG quadrature: per-grid-point work | accepted | quad/sg 2.4x (22.3 → 9.4 s), mivec 1.45x; 15.8 s saved; b1 d20 4.7x. Memory (found by WP-15): `quad/sg/cc-uq/b0/d2/L20` 806 → 1076 MiB (1.335x) | [WP-01](summaries/WP-01-sg-quad-per-point.md) |
| – | RP-1 | Re-profiling after WP-01/WP-13/WP-14 (+ cumulative memory) | accepted | baseline → rp-1 e2e 1.31x (69.95 → 53.41 s; d ≥ 7 1.28x); quad/sg 2.43x, quad/dsg 0.986x (51 % of the suite), mivec 1.34x; memory geomean 1.028, 3 MEM+ (WP-01 scratch) | [RP-1](summaries/RP-1-reprofiling.md) |
| – | WP-20 | `FlatNestedVector` under strict aliasing (owner's request) | accepted | no UB; local spans in `appendSubTrieChildren`: quad/dsg 1.037x (27.08 → 26.10 s, −0.98 s), b1 d20 1.076x, b1 d15 1.068x; `^quad/` 1.027x; bitwise identical; memory geomean 1.001, 0 flags | [WP-20](summaries/WP-20-flat-nested-vector.md) |
| – | WP-02 | DSG quadrature: per-delta-tensor-grid trie work at high d | accepted | quad/dsg 1.46x (26.14 → 17.91 s, −8.23 s): b1 d20 5.34x, b1 d15 3.14x, b0 1.01–1.05x; memory b1 d20 328 → 217 MiB, d15 233 → 152 MiB (MEM−), 6 b0 MEM+ (+1.9…+6.4 MiB), geomean 0.981; refcheck OK | [WP-02](summaries/WP-02-dsg-trie-work.md) |
| 1 | RP-2 | Re-profiling after WP-20/WP-02 (+ cumulative memory) | in progress (prompt issued 2026-10-29) | | |
| 2 | WP-03 | Downwards closure of MI sets | ready | | |
| 3 | WP-04 | DSG quadrature: row loop (b0) (+ TrieNode 24 → 16 B, from WP-02) | ready | | |
| 4 | WP-17 | Batched source-function evaluation (from D-4; proposal) | ready (after WP-04) | | |
| 5 | WP-09 | Multi-point interpolation API (proposal; from D-1) | ready | | |
| 6 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09) | | |
| 7 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09) | | |
| 8 | WP-21 | DSG b1: reuse projected sub-tries per single-node-dimension set (from WP-02) | planned (RP-2 sizes it) | | |
| 9 | WP-05 | SG quadrature at low d: CC 1D weights + per-thread scratch on demand (incl. NEW-A) | ready | | |
| 10 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 11 | WP-19 | SG quadrature: load balancing at low d | ready (after WP-05) | | |
| 12 | WP-22 | Tensor grids as struct of arrays (owner's idea; proposal) | planned (RP-2 sizes it) | | |
| 13 | WP-18 | Shared 1D node arrays per (dimension, level) (from D-5; memory; proposal) | ready | | |
| 14 | WP-10 | Do not link Armadillo when Eigen provides the FFT (from D-3; measure first) | ready | | |
| 15 | WP-12 | Toolchain re-measurement after WP-02/WP-03 (optional) | planned (owner decides after WP-03) | | |

## Next action

Owner: commit WP-02 ("Accept WP-02: …": code incl. the new
`tools/operators/quadrature/projected_sub_trie_builder.{hpp,cpp}`, tests, micro cases, summary,
`results/WP-02/`, STATUS/PLAN); commit or ignore the untracked `compile{ASan,Debug,Normal}.sh`
so the tree is clean. Then start RP-2 (snapshot `rp-2` = the WP-02 commit).
Next master start: process the RP-2 summary, re-rank (now WP-03 ≈ 5.1 s before WP-04 1.5–3 s).

## Open decisions for the owner

- WP-09 / WP-17 / WP-18 / WP-22 (after their summaries): adopt the proposed interface or not.
- WP-10 (after its summary): implement only if the measured start-up gain is worth it.
- WP-12 (after WP-03; WP-02 is done): re-run the GCC/Clang comparison or not.

## Do not retry

(one entry per rejected attempt or dropped variant: `WP-NN <idea>: <numbers>, <why>`)
- WP-00 noise: bootstrap within rounds (18/114 false positives) → paired per-round t-interval;
  plain std (throttling onsets) → MAD; micro right after 16-thread e2e 10–25 % slow → idle
  settle, calibrate, micro, e2e. Refcheck: ratio tolerance above a 16-unit floor missed 5x
  error growth → `e_new ≤ ratio·e_ref + 4 units`.
- WP-01 rows always along dimension 0: b1 47.2 vs. 27.9 ns/point (rows walk leading 1-node
  dimensions) → only multi-node dimensions. Row helpers reading scratch per row: 2–9 % slower
  (opaque `SourceFunc::evaluate` forces reloads) → copy scratch into locals before the loop.
- WP-11 `-fno-plt -Wl,-Bsymbolic-functions` on top of nosi: 1.015x vs. 1.024x. Clang ThinLTO:
  e2e 1.034x but quad/sg 0.964x, quad/dsg 0.980x, 14/38 SLOWER, refcheck WARN → GCC kept (owner
  2026-10-25); full LTO 1.018x. Clang builds need `CC=clang CXX=clang++ LINK=clang++`.
- WP-14 TySan (`-fsanitize=type`): > 12 GB per binary (crashed the PC uncapped), 21/74 suites
  aborted, only false positives → `-Wstrict-aliasing=3/2` scan; rerun only with
  `OPENBLAS_NUM_THREADS=1` + cgroup cap.
- WP-15 memory flag by ratio only (±2/3 %): false flags in 5 of 9 A/A cases (bimodal
  `quad/sg/equi-exp/b1/d20/L5`) → 3 % + no overlap. `os.wait4`/`RUSAGE_CHILDREN`: driver's peak.
- RP-1 profiling: `perf script` callchains through libm unreliable, inlined helpers have no own
  symbol → `perf report --sort sym,srcfile|srcline` splits.
- WP-20 `FlatNestedVec` offsets of a distinct type (`uint32_t`): 0.75 vs. 0.77 ns/element, no
  gain; the alias that matters is pointer-to-`T` (`const double*` stores), not the offsets.
- WP-02 projected coefficients in plain double: round-off 9x larger at b1/d15 → double-double
  (≤ 1–2 %). Slot blocks of global width with 24 B slots: b0/d10 RSS 36.5 → 65 MiB →
  1-norm-bounded blocks, split 8 B / 16 B arrays (41.9 MiB).
- WP-02 builder split without hidden visibility (`sub_tries` 0.987/0.973) or with an unaligned
  row loop (e2e 0.972x, b0 1.3–4 % SLOWER, +18 % op-cache misses) → hidden + aligned. Linear
  scan instead of `lower_bound` for the first child ≥ m_k: `sub_tries` b1/d20 0.948.

## Re-profiling

Last: RP-1 (2026-10-28, accepted). Accepted optimizations since RP-1: 2 (WP-20 quad/dsg 1.037x;
WP-02 quad/dsg 1.46x, b1 d20 5.34x, b1 hot-spot ranking shifted) → RP-2 due, issued 2026-10-29.

## Log

- 2026-10-29 WP-02 accepted (owner; commit pending). Follow-ups: WP-21 (new, planned); TrieNode
  24 → 16 B + 32-bit `order` → WP-04; b1 d20 peak = DSG itself → WP-18. A once-seen debug failure
  (output lost, 0/150 reruns; test prints its seed now): report recurrences. RP-2 prompt issued.
  Owner's idea: tensor grids as struct of arrays → WP-22 (planned); RP-2 prompt re-issued with it.

## Archive

- 10-24 setup (fed2cdb4e), baseline boost off (931158248). 10-25 WP-01 (5281e0b7d). 10-26 WP-11
  (9ab8f642d), WP-13 (a16733d65); progress report 044d2794d (28 cases: 51.1 s master → 41.9 s
  baseline → 25.3 s WP-13). 10-28 WP-14 (3dc286c3d), WP-15 memory (a485cec02), benchmark move
  666b3c601 → WP-16 (f48d2bbc1), RP-1 (b0b7faac7: re-ranked; NEW-A → WP-05, D-4 → WP-17, D-5 →
  WP-18, NEW-C → WP-19; owner: WP-20 before WP-02), WP-20 (29ade3a58; owner: strict-aliasing
  note in every code-changing WP prompt).
