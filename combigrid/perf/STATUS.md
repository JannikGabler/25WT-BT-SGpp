# Optimization status — combigrid (master file)

Last processed: RP-1 (2026-10-28) · Next free WP number: 20 · Re-profilings done: 1 (RP-1, 2026-10-28; RP-0 = setup)

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
- **Method note**: `ab micro` runs 0 warm-ups → allocation-heavy micro cases show first-call
  times (ratios valid; absolute numbers from `run micro`).
- **Machine note**: boost off since 2026-10-24 (throttling up to 1.6x with boost on); a reboot
  re-enables it, the owner disables it manually — if `envcheck` fails on boost, ask the owner.
  Full e2e A/B ≈ 18–23 min (sessions filter). `scons -c` is fine (owner 2026-10-25). Sanitizer
  and heap-profiler runs need a memory cap (`systemd-run --user --scope -p MemoryMax=8G`).
- **Toolchain**: GCC 15.2 (judged); Clang 21.1.8 + libomp + lld; JDK 21 (`SG_JAVA=1`).
  `perf/scripts/toolchain.py` (WP-11; variant `gcc-sa` from WP-14) builds compiler/flag variants.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed but must be
  labelled and weighed against API/functionality/maintainability/readability; functional/
  interface/flag changes only after consulting the owner; errors stay in the same order of
  magnitude (refcheck OK/WARN/FAIL); high dimensions first. **Memory** (2026-10-28): runtime
  ranks the plan; peak memory is reported to the owner with every runtime change and weighed in
  decisions; memory reduction is a valid secondary goal where potential exists.
- **Refcheck WARN case** (owner 2026-10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (Lebesgue constant ~1e16) WARNs on any FMA-placement change in `BarycentricFormula::evaluateFormula`.
- **Commits**: the owner commits after every session (one commit per WP: code + summary +
  `results/WP-NN/` + STATUS/PLAN, message "Accept WP-NN: …" / "Reject WP-NN: …"); the tree
  must be clean before the next work session starts.

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
| 1 | WP-02 | DSG quadrature: per-delta-tensor-grid trie work at high d | in progress (prompt issued 2026-10-28) | | |
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

Owner: commit RP-1 ("Accept RP-1: …": `ANALYSIS.md`, `results/RP-1/`, the summary, STATUS/PLAN).
The tree must be clean before the work session: `compileASan.sh`, `compileDebug.sh` and
`compileNormal.sh` are untracked (CLAUDE.md §4 calls them checked in) — commit or ignore them.
Then start WP-02 (prompt issued 2026-10-28). Next master start: process the WP-02 summary;
next candidate WP-03.

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
- WP-14 TySan (Clang 21 `-fsanitize=type`) over SG++: OpenBLAS × shadow memory > 12 GB per binary
  (crashed the PC uncapped), runtime crash in `isAliasingLegal` aborted 21/74 combigrid suites,
  all report classes false positives (vptr, OpenMP bounds, mt19937, layout), ~3 h → use the
  `-Wstrict-aliasing=3/2` scan instead; if ever rerun: `OPENBLAS_NUM_THREADS=1` + cgroup cap.
- WP-15 memory flag by ratio only (±2 %, ±3 %): false flags in 5 of 9 A/A cases, up to 57 % of
  the 5/5 round splits (bimodal `quad/sg/equi-exp/b1/d20/L5` 53.7 vs. 57–60 MiB) → 3 % + no
  overlap of rounds (≤ 0.8 %). `os.wait4`/`RUSAGE_CHILDREN` from Python: reports the driver's peak.
- RP-1 profiling: `perf script` callchains through libm are unreliable, and inlined helpers
  (since WP-13) have no own symbol → use `perf report --sort sym,srcfile|srcline` splits.

## Re-profiling

Last: RP-1 (2026-10-28, accepted). Accepted optimizations since RP-1: none. Next RP-2 when 2
optimizations are accepted (likely after WP-02 + WP-03), a group changes by ≥ 1.3x, or the
hot-spot ranking shifts.

## Log

- 2026-10-28 RP-1 accepted (owner; measurement only; commit pending). Re-ranked: WP-02 1, WP-03
  2, WP-04 3; RP-1 NEW-A (scratch) merged into WP-05 (owner; runtime rank kept → 8); D-4 → WP-17,
  D-5 → WP-18 as proposals (owner); NEW-C → WP-19. WP-02 prompt issued.

## Archive

- 2026-10-24 setup (fed2cdb4e), baseline re-recorded boost off (931158248). 2026-10-25 WP-01
  (5281e0b7d); WP-11 → WP-13, strict aliasing → WP-14. 2026-10-26 WP-11 (9ab8f642d), WP-13
  (a16733d65), WP-14 issued (checkpoint 50d552f00); supervisor progress report 044d2794d (28
  cases: 51.1 s master → 41.9 s baseline → 25.3 s WP-13; mivec generation d15/d20 faster on
  master → RP-1). 2026-10-28 WP-14 accepted (3dc286c3d; datadriven → `std::bit_cast`); owner
  requested peak-memory monitoring → WP-15 (a485cec02; memory: `quad/sg/cc-uq/b0/d2/L20`
  +240 MiB since WP-01 → RP-1). Benchmark move (staged) broke `verify`/paths → WP-16; RP-1
  prompts superseded. Refactor 666b3c601 (removed the WP-14 comment in `SGppConfigure.py`;
  CLAUDE.md §4 keeps it); WP-16 accepted (f48d2bbc1).
