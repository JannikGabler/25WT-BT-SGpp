# Optimization status — combigrid (master file)

Last processed: WP-20 (2026-10-28) · Next free WP number: 21 · Re-profilings done: 1 (RP-1, 2026-10-28; RP-0 = setup)

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
- **Toolchain**: GCC 15.2 (judged); Clang 21.1.8 + libomp + lld; JDK 21 (`SG_JAVA=1`).
  `perf/scripts/toolchain.py` (WP-11; `gcc-sa` WP-14, `gcc-nsa` = judged + `-fno-strict-aliasing`
  WP-20) builds compiler/flag variants.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed but must be
  labelled and weighed against API/functionality/maintainability/readability; functional/
  interface/flag changes only after consulting the owner; errors stay in the same order of
  magnitude (refcheck OK/WARN/FAIL); high dimensions first. **Memory** (2026-10-28): runtime
  ranks the plan; peak memory is reported to the owner with every runtime change and weighed in
  decisions; memory reduction is a valid secondary goal where potential exists. **Strict
  aliasing** (2026-10-28): pre-WP-14 code assumed `-fno-strict-aliasing` and may be simplifiable
  → every code-changing WP prompt carries the note from the PLAN.md header.
- **Refcheck WARN case** (owner 2026-10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue constant ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session (one per WP: code, summary, `results/WP-NN/`,
  STATUS/PLAN; "Accept/Reject WP-NN: …"); the tree is clean before the next work session.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00 | Setup: infrastructure, baseline, analysis, plan | done (committed by the setup session) | – | [WP-00](summaries/WP-00-setup.md) |
| – | WP-01 | SG quadrature: per-grid-point work | accepted | quad/sg 2.4x (22.3 → 9.4 s), mivec 1.45x; 15.8 s saved; b1 d20 4.7x. Memory (found by WP-15): `quad/sg/cc-uq/b0/d2/L20` 806 → 1076 MiB (1.335x) | [WP-01](summaries/WP-01-sg-quad-per-point.md) |
| – | WP-11 | Toolchain measurement: GCC flags, Clang ThinLTO, Clang full LTO | accepted (gcc-nosi → WP-13) | gcc-nosi e2e 1.024x (interp 1.065x, mivec 1.043x, quad 1.02x); Clang not adopted | [WP-11](summaries/WP-11-toolchain.md) |
| – | WP-13 | `-fno-semantic-interposition` in site_scons (GCC, `OPT=1`) | accepted | e2e 1.023x (54.03 → 52.79 s), interp 1.062x, 0 SLOWER; all 8 modules' tests pass | [WP-13](summaries/WP-13-nosi-scons.md) |
| – | WP-14 | `-fno-strict-aliasing` only for the SWIG Java wrapper; datadriven punning → `std::bit_cast` | accepted | e2e 0.988x (52.77 → 53.42 s): interp 1.10x, interplin 1.10x, quad/sg 0.995x, quad/dsg 0.970x, mivec 0.886x; refcheck OK; complete project identical | [WP-14](summaries/WP-14-strict-aliasing.md) |
| – | WP-15 | Peak-memory measurement in the benchmark harness (owner's request) | accepted | infrastructure: runtime impact nil (full suite 1.0002x, 0 SLOWER); floor 10.2 MiB; MEM± rule 3 % + no overlap | [WP-15](summaries/WP-15-memory-measurement.md) |
| – | WP-16 | Adapt `perf.py` and the protection to the benchmark move (`combigrid/benchmarks/`) | accepted | infrastructure: A/A old vs. new layout 1.001x, memory 0.994, 0 flags; refcheck OK; pre-move commits still snapshot | [WP-16](summaries/WP-16-benchmark-move.md) |
| – | RP-1 | Re-profiling after WP-01/WP-13/WP-14 (+ cumulative memory) | accepted | baseline → rp-1 e2e 1.31x (69.95 → 53.41 s; d ≥ 7 1.28x); quad/sg 2.43x, quad/dsg 0.986x (51 % of the suite), mivec 1.34x; memory geomean 1.028, 3 MEM+ (WP-01 scratch) | [RP-1](summaries/RP-1-reprofiling.md) |
| – | WP-20 | `FlatNestedVector` under strict aliasing (owner's request) | accepted | no UB; local spans in `appendSubTrieChildren`: quad/dsg 1.037x (27.08 → 26.10 s, −0.98 s), b1 d20 1.076x, b1 d15 1.068x; `^quad/` 1.027x; bitwise identical; memory geomean 1.001, 0 flags | [WP-20](summaries/WP-20-flat-nested-vector.md) |
| 1 | WP-02 | DSG quadrature: per-delta-tensor-grid trie work at high d | in progress (prompt issued 2026-10-28, after WP-20) | | |
| 2 | WP-03 | Downwards closure of MI sets | ready | | |
| 3 | WP-04 | DSG quadrature: row loop (b0) | ready (after WP-02) | | |
| 4 | WP-17 | Batched source-function evaluation (from D-4; proposal) | ready (after WP-04) | | |
| 5 | WP-09 | Multi-point interpolation API (proposal; from D-1) | ready | | |
| 6 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09) | | |
| 7 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09) | | |
| 8 | WP-05 | SG quadrature at low d: CC 1D weights + per-thread scratch on demand (incl. NEW-A) | ready | | |
| 9 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 10 | WP-19 | SG quadrature: load balancing at low d | ready (after WP-05) | | |
| 11 | WP-18 | Shared 1D node arrays per (dimension, level) (from D-5; memory; proposal) | ready | | |
| 12 | WP-10 | Do not link Armadillo when Eigen provides the FFT (from D-3; measure first) | ready | | |
| 13 | WP-12 | Toolchain re-measurement after WP-02/WP-03 (optional) | planned (owner decides) | | |
Decisions (PLAN.md): D-1 → WP-09 · D-2 done (WP-13) · D-3 → WP-10 · D-4 batched `SourceFunc`
→ WP-17 · D-5 shared node arrays → WP-18. Ranking notes below the PLAN.md table.

## Next action

Owner: commit WP-20 ("Accept WP-20: …": code, tests, micro cases, `toolchain.py` (`gcc-nsa`),
the summary, `results/WP-20/`, STATUS/PLAN). The tree must be clean before the work session:
`compileASan.sh`, `compileDebug.sh` and `compileNormal.sh` are still untracked (CLAUDE.md §4
calls them checked in) — commit or ignore them. Then start WP-02 (use the re-issued prompt
with the strict-aliasing note).
Next master start: process the WP-02 summary; if accepted, RP-2 is due before WP-03.

## Open decisions for the owner

- WP-09 / WP-17 / WP-18 (after their summaries): adopt the proposed interface or not.
- WP-10 (after its summary): implement only if the measured start-up gain is worth it.
- WP-12 (after WP-02/WP-03): re-run the GCC/Clang comparison or not.

## Do not retry

(one line per rejected attempt or dropped variant: `WP-NN <idea>: <numbers>, <why>`)
- WP-00 noise: bootstrap within rounds (18/114 false positives; noise is between processes) →
  paired per-round t-interval; plain std inflated by throttling onsets (20 %) → MAD; micro right
  after 16-thread e2e 10–25 % slow → idle settle, calibrate, micro, e2e.
- WP-00 refcheck: ratio tolerance above a 16-unit round-off floor missed 5x error growth →
  explicit `e_new ≤ ratio·e_ref + 4 units`.
- WP-01 rows always along dimension 0: b1 slower (equi/b1/d20/L4 47.2 vs. 27.9 ns/point), rows
  walk the leading 1-node dimensions → iterate only the multi-node dimensions.
- WP-01 row helpers reading scratch data per row (2–9 % slower): the opaque `SourceFunc::evaluate`
  forces reloads → copy scratch data into locals before the hot loop.
- WP-11 `-fno-plt -Wl,-Bsymbolic-functions` on top of nosi: e2e 1.015x vs. 1.024x (nosi alone).
- WP-11 Clang ThinLTO: e2e 1.034x but quad/sg 0.964x, quad/dsg 0.980x, 14/38 SLOWER, refcheck
  WARN, 612 warnings → GCC kept (owner 2026-10-25); full LTO 1.018x; + nosi 1.033x. Clang builds
  need explicit `CC=clang CXX=clang++ LINK=clang++` (configure checks otherwise run g++).
- WP-14 TySan (Clang 21 `-fsanitize=type`): > 12 GB per binary (OpenBLAS × shadow; crashed the
  PC uncapped), crash in `isAliasingLegal` aborted 21/74 suites, only false positives, ~3 h → use
  the `-Wstrict-aliasing=3/2` scan; if ever rerun: `OPENBLAS_NUM_THREADS=1` + cgroup cap.
- WP-15 memory flag by ratio only (±2 %, ±3 %): false flags in 5 of 9 A/A cases, up to 57 % of
  the 5/5 round splits (bimodal `quad/sg/equi-exp/b1/d20/L5` 53.7 vs. 57–60 MiB) → 3 % + no
  overlap of rounds (≤ 0.8 %). `os.wait4`/`RUSAGE_CHILDREN` from Python: reports the driver's peak.
- RP-1 profiling: `perf script` callchains through libm are unreliable, and inlined helpers
  (since WP-13) have no own symbol → use `perf report --sort sym,srcfile|srcline` splits.
- WP-20 V2 `FlatNestedVec` offsets of a type distinct from the payload (`Index = uint32_t`):
  chained read + `push_back` 0.75 vs. 0.77 ns/element, local view 0.57 in both → no gain; the
  alias that matters is pointer-to-`T` (`const double*` stores), not the offsets.

## Re-profiling

Last: RP-1 (2026-10-28, accepted). Accepted optimizations since RP-1: 1 (WP-20; quad/dsg
1.037x, hot-spot ranking unchanged). Next RP-2 when 2 optimizations are accepted (i.e. after
WP-02 if accepted), a group changes by ≥ 1.3x, or the hot-spot ranking shifts.

## Log

- 2026-10-28 WP-20 accepted (owner; commit pending): no UB, quad/dsg 1.037x. WP-02 re-estimated
  ≈ 3.8–5.0 s (ties WP-03 → DSG first); findings → WP-02/WP-04. Owner: strict-aliasing
  simplification note for every code-changing WP (PLAN.md header). WP-02 prompt issued with it.

## Archive

- 10-24 setup (fed2cdb4e), baseline boost off (931158248). 10-25 WP-01 (5281e0b7d). 10-26 WP-11
  (9ab8f642d), WP-13 (a16733d65); progress report 044d2794d (28 cases: 51.1 s master → 41.9 s
  baseline → 25.3 s WP-13). 10-28 WP-14 (3dc286c3d), WP-15 memory (a485cec02), benchmark move
  666b3c601 → WP-16 (f48d2bbc1), RP-1 (b0b7faac7: re-ranked; NEW-A → WP-05, D-4 → WP-17, D-5 →
  WP-18, NEW-C → WP-19; owner: WP-20 before WP-02).
