# combigrid performance suite and optimization phase

Infrastructure for optimizing the combination technique module in a sequence of small,
independent sessions. Who reads what:

| Reader | Start here |
|---|---|
| master session | skill `perf-master` (`.claude/skills/perf-master/SKILL.md`), then [STATUS.md](STATUS.md) |
| work session | skill `perf-work` (`.claude/skills/perf-work/SKILL.md`) + the prompt from the master |
| anybody running benchmarks | this file |

## Layout

```
combigrid/perf/
  README.md          this file (commands, methodology, protection)
  STATUS.md          compact status of the optimization phase (the master's main input)
  PLAN.md            ranked work packages (WPs) with location, hypothesis, evidence, risk, ...
  ANALYSIS.md        initial analysis and profiles (re-profiling results are appended)
  summaries/         TEMPLATE.md + WP-NN-<slug>.md, one per work session
  results/WP-NN/     A/B reports (.md + raw .json) and refcheck output of a work session
  baseline/          [protected] measurements of the unoptimized code + environment
  reference/         [protected] reference outputs (reference.jsonl) + tolerances.json
  protected.sha256   [protected] checksum manifest of all protected files
  scripts/perf.py    [protected] the driver (stdlib-only Python)
  .work/             git-ignored: snapshots, logs, raw runs, profiles, build stamp
combigrid/benchmarks/perf/   (SCons builds them with the configuration flags, never runs them)
  perf_harness.hpp   [protected] dependency-free harness (sampling, calibration, JSON output)
  perf_problems.hpp  [protected] integrands with exact integrals, points, multi-index sets
  perf_e2e.cpp       [protected] L3 end-to-end pipelines with L2 phase timings, public API only
  perf_reference.cpp [protected] reference outputs, public API only
  perf_micro.cpp     L1 micro-benchmarks of internals (extended/adapted by work packages)
```

Commits before the benchmark move (666b3c601, 2026-10-28; e.g. the baseline commit) have the
perf benchmarks in `combigrid/examples/c++/benchmarks/perf/`. `perf.py` looks them up per source
tree (`BENCH_DIRS`: current layout first), so `snapshot NAME --commit REV` works for both layouts.

## The judged configuration

Performance is judged as in CLAUDE.md ("Performance-relevant configuration") with Eigen and
Armadillo enabled (the owner guarantees both libraries in all performance-relevant runs):

```
scons -j16 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 OPT=1 ARCH=avx2 USE_EIGEN=1 USE_ARMADILLO=1 \
      CPPFLAGS="-march=native -flto=auto" LINKFLAGS="-march=native -flto=auto"
```

= `perf.py build normal`. Acceptance-relevant measurements run with **16 threads**
(`OMP_PROC_BIND=close OMP_PLACES=threads`); 1-thread runs (`OMP_PLACES={4}`) are diagnostics
(micro-benchmarks). Machine: the owner's laptop (Ryzen 7 5825U, 8 cores / 16 threads, 12 GB,
`performance` governor and platform profile, **CPU boost off** (`/sys/devices/system/cpu/cpufreq/boost
= 0`, all cores at the nominal 2.0 GHz; owner's decision on 2026-10-24 for reproducible numbers),
always on AC), otherwise idle, one benchmark at a time. `envcheck` refuses to measure when boost
or the platform profile differ from the baseline (boost is re-enabled by a reboot unless the
owner made the setting persistent). `perf.py` sets the OpenMP environment itself.

Changing compiler flags or the build configuration is a work package of its own and needs the
owner's approval.

## Commands

All commands: `python3 combigrid/perf/scripts/perf.py <command>` (from the repo root; the path
contains spaces, always quote it). `-h` shows all options.

| Command | Purpose |
|---|---|
| `envcheck` | idle CPU, governor, AC, boost state equal to the baseline, perf permissions |
| `calibrate` | ~70 s: runs fixed workloads with the `baseline` snapshot (16-thread e2e case for ~1 min: burst and sustained medians; 1-thread micro case) and compares them with `baseline/calibration.json`: detects machine-state changes (power limits, thermal state, background load) that `envcheck` cannot see; WARN beyond ±7 % |
| `build normal\|debug\|asan [--tests compile\|run\|skip]` | SCons build of base + combigrid (~3 min from scratch, switching configurations rebuilds everything); writes the build stamp |
| `test [--repeat N] [--run-test FILTER]` | runs `combigrid/tests/test_combigrid_boost` of the current build (tests are time-seeded: repeat) |
| `snapshot NAME` | copies the current **normal** build (libs + perf binaries) to `.work/snapshots/NAME` |
| `snapshot NAME --commit REV` | builds REV in a temporary git worktree and snapshots it (e.g. the baseline commit) |
| `run e2e\|micro [--snapshot NAME] [--filter RE]` | one run, table + raw JSON in `.work/runs/`; e2e: one process per case, with its peak memory (below) |
| `ab e2e\|micro A B [--filter RE] [--save DIR --name STEM]` | interleaved A/B comparison (below); e2e: plus peak memory per case |
| `refcheck [--snapshot NAME] [--threads 1,16]` | reference check (below); exit code 1 on FAIL |
| `profile CASE_REGEX [--suite e2e\|micro] [--threads N] [--children]` | `perf record -g` of one case, prints the top functions; data in `.work/profiles/` |
| `verify` | checksums of the protected files |
| `reject [--apply] [--keep PATH ...]` | reverts all changes vs. HEAD except `summaries/` and `results/` (dry run by default) |

`current` as snapshot name means the in-tree build (must be a `normal` build made by
`perf.py build`; benchmarks refuse builds of other configurations or builds made outside of
`perf.py`).

## Benchmark levels

| Level | Where | What |
|---|---|---|
| L1 micro | `perf_micro` (1 thread) | one internal function at the hot spot: one tensor grid, one 1D rule, one MI-set operation; `micro/sol/...` are hand-written "speed of light" loops for the same task |
| L2 phase | `perf_e2e` metrics `generation`, `operator` | grid construction resp. operator application of a pipeline |
| L3 end-to-end | `perf_e2e` metric `total` (16 threads) | the complete user pipeline: source function, generation instruction, grid, operator |

E2E case names: `<operator>/<grid>/<nodes>[-<integrand>]/b<boundary offset>/d<dim>/L<level>[/...]`
with operators `quad` (quadrature), `interp` (global interpolation at `pN` points), `interplin`
(multilinear), `mivec` (`MIVecSGGenInstr` from `nN` anisotropic multi-indices with 1-norm ≤ `S`,
then quadrature); grids `sg` (`SparseGrid`), `dsg` (`DeltasSparseGrid`); integrands `exp`
(cheap: one `exp` per evaluation, exposes framework overhead), `uq` (forward-UQ integrand of the
thesis, Chapter 7), `sin` (product of sines, as the older benchmarks). All grids use the doubling
growth. Sizes (chosen with boost on): 0.05–7 s per pipeline at 16 threads with boost off; one
pass over all 38 cases ≈ 70 s. **High
dimensions have priority** (they run longest): rank by absolute time saved.

Warm runs: every pipeline runs `--warmups` times before the measured samples (first runs pay
for OpenMP thread creation, page faults and the OpenBLAS thread pool that Armadillo pulls in).

## A/B methodology

`perf.py ab SUITE A B`: for every round and every case, A and B run back to back as separate
processes (order alternating between rounds and cases, so thermal drift hits both equally),
each with `--warmups` + `--samples`. The dominant noise is **process to process** (memory
layout, thread placement; some cases are bimodal by up to ~9 %), not sample to sample — so the
defaults use many short processes: e2e 5 rounds × (1 warm-up + 1 sample) at 16 threads (full
suite ≈ 23 min; work sessions compare the relevant cases only), micro 5 rounds × 2 samples at
1 thread.

Reported per case and metric: medians of A and B, **speedup = median(A) / median(B)** (> 1: B is
faster; computed as geometric mean of the per-round ratios) and a 95 % t-confidence interval of
the per-round log ratios whose spread is bounded below by the **noise model**
(`baseline/noise_{e2e,micro}.json`: per-case spread measured by comparing the baseline with
itself over 6 rounds; floor 0.3 %). Verdict: `faster` if the CI lies above 1, `SLOWER` if below,
`~ (noise)` otherwise. The footer gives the sum of the medians (absolute time saved over all
compared cases) and an accuracy sanity line. Speedups inside the noise band are reported as
"within noise", never as gains. Noise figures: [ANALYSIS.md](ANALYSIS.md#measurement-noise).

**Foreign CPU load** (CPU time of other processes as a share of all CPUs, measured per round
from `/proc/stat` minus our own processes) is reported by `run`, `ab` and `baseline`; above 2 %
the report carries a WARN and the comparison must be repeated once the cause is gone. This
catches background jobs that start in the middle of a measurement (observed in the setup
session: unattended upgrades after a reboot slowed 16-thread runs by 1.6x for 30 minutes while
1-thread runs were unaffected) — `envcheck` only checks idleness before a run.

Rules: compare against a **fresh "before" snapshot built in the same session** (never against
old stored numbers: the laptop drifts). Do not build or run anything else while measuring.
Results of a work session go to `combigrid/perf/results/WP-NN/` (`--save`).

## Peak memory

Runtime ranks the plan; peak memory is monitored, reported with every runtime change and weighed
in decisions (owner, 2026-10-28). `run e2e` and `ab e2e` record the **peak resident set size**
(max RSS) of every benchmark process: GNU time (`/usr/bin/time -f %M -o .work/peak_rss.txt`)
launches the benchmark, waits for it and writes the kernel's `ru_maxrss` of its child. Nothing
runs inside the measured process and the timings are taken inside the binary as before, so the
runtime is unaffected (WP-15, full suite, launcher on vs. off: 0 `SLOWER`, sum of medians
1.0002x; the launcher adds ≈ 1.2 ms per process outside the timed region); it works for every
snapshot incl. `baseline`. Every case runs in its own process (since WP-15 also in `run
e2e`), so a peak is that of one case, warm-ups and samples included. The CPU time of GNU time
and of the benchmark below it still counts as own, not foreign load (`RUSAGE_CHILDREN` includes
waited-for grandchildren).

- Peaks read as **process floor + case memory**. Floor: `perf_e2e --list` (libraries loaded, no
  case run) 10.1–10.3 MiB; the smallest cases peak at 11.5–12.2 MiB (+ OpenMP threads, data);
  22 of the 38 cases stay below 20 MiB, the largest reaches ~1.07 GiB
  (`quad/sg/cc-uq/b0/d2/L20`). Reports print the floor of each snapshot.
- **Noise**: peaks vary from process to process (presumably thread-dependent malloc arenas):
  spread over 5 rounds mostly 2–6 %, `quad/sg/equi-exp/b1/d20/L5` is bimodal (53.7 vs.
  57–60 MiB, 12 %).
- `ab e2e` reports per case the medians over the rounds, B/A (> 1: B needs more), the spread
  (max/min − 1) and the flag `MEM+`/`MEM-` if **|B/A − 1| > 3 % and the rounds of A and B do not
  overlap** (`MEM_THRESHOLD` in `perf.py`). In the WP-15 A/A over all 5/5 splits of the rounds, a
  ratio threshold alone flagged up to 57 % (bimodal case), the combined rule ≤ 0.8 % per case.
  Changes inside the spread of a case stay unflagged; the table still shows them. Footer:
  geomean B/A, largest B/A, number of flags, process floors.
- e2e only: a micro process runs many cases and is dominated by the harness.
- Saved JSON: every e2e record gains `peak_rss_kib`, the file gains `process_floor_kib`; existing
  fields are unchanged.

## Reference check

`perf_reference` computes 104 cases (all operators; node families equi/cc/cheb1; boundary
offsets 0/1; unit and shifted domains; d = 1–10; complete, deltas and MIVec grids) with exact
solutions. `perf.py refcheck` compares a build against `reference/reference.jsonl` (recorded on
the unoptimized code) at 1 and 16 threads, using `reference/tolerances.json`:

- the **error against the exact solution** may not grow: `|new − exact| ≤ 1.1·|ref − exact| + noise`
  is OK; up to `3·|ref − exact| + noise` is **WARN** (must be stated and justified in the summary;
  the owner decides); more is **FAIL**. `noise` = 4 round-off units of the combination
  (2^-52 · magnitude · (Σ|c_ℓ| + √#points)); reassociation across thread counts moved results by
  at most 1.54 units in the setup session.
- structure (`ints`: number of tensor grids, grid points, order-independent hash of all
  (multi-index, coefficient) pairs, closure size/hash) must match **exactly** — a change is a
  functional change;
- node positions (`floats`: checksum) up to 1e-12 relative;
- the number of source function evaluations (`info`) may change (reported as INFO, state it).

Owner requirement: numerical errors stay in the same order of magnitude; they may grow slightly
but should generally not.

## Protection

Protected: `baseline/`, `reference/` (incl. tolerances), `protected.sha256`, `scripts/perf.py`,
`perf_harness.hpp`, `perf_problems.hpp`, `perf_e2e.cpp`, `perf_reference.cpp`, the pre-existing
benchmarks (`benchmarks/{quadrature,dsg_quadrature,multiindex_vector,source_function}`,
`benchmarks/bachelor_thesis/comparison`), `tools/benchmarking/benchmarker.hpp`, and the hook itself
(`.claude/hooks/protect_perf.py`, `.claude/settings.json`).

Enforcement: the `PreToolUse` hook `.claude/hooks/protect_perf.py` blocks edits of these files
(and obvious shell writes); `perf.py verify` checks their SHA-256 sums against
`protected.sha256` (every work session runs it at start and end; the owner sees violations in
the diff). Only the owner may change protected files: edit them, then run
`perf.py protect --authorized-by-owner` to rehash (e.g. for a new baseline after a change of the
judged configuration).

`perf_micro.cpp` is *not* protected: work packages add micro-benchmarks there. Existing case
names keep their meaning; if a case must change because an internal signature changed, the
summary says so; if its meaning changes, it gets a new name.

## Adding a micro-benchmark

In `perf_micro.cpp`, add a registration function and call it in `main()`:

```cpp
suite.addMicro("micro/<area>/<function>/<config>", [=](MicroRun& run) {
  // setup (not timed): build the grid, scratch buffers, inputs
  run.setItems(nItems);                     // e.g. grid points -> reports ns/item
  run.measure([&]() { doNotOptimize(f(...)); });  // timed; called many times
});
```

It must compile against the unoptimized code (it is part of the "before" snapshot). Check with
`perf.py run micro --filter <name> --no-envcheck` (exploratory) before the A/B.

## Profiling and tools

- `perf` works (`kernel.perf_event_paranoid = 1`, persisted): `perf.py profile`, or
  `perf record -g` / `perf annotate` / `perf stat` directly (set `LD_LIBRARY_PATH` to the
  snapshot's `lib/`).
- valgrind/callgrind (deterministic instruction counts, 1 thread), heaptrack (allocations),
  hyperfine are installed. No Google Benchmark, no NumPy (the scripts use the stdlib only).
- `objdump -dr lib/libsgppcombigrid.so` shows whether helpers are inlined (exported functions go
  through the PLT under `-fPIC` without `-fno-semantic-interposition`; GCC `OPT=1` builds have
  that flag since WP-13, see `configureGNUCompiler` in `site_scons/SGppConfigure.py`).
- Strict aliasing: since WP-14 SG++ code is compiled with GCC's default `-fstrict-aliasing`; only
  the SWIG Java wrapper (`jsgpp/SConscript`) gets `-fno-strict-aliasing`. Type punning through
  pointer casts is undefined behavior and may be miscompiled: use `std::bit_cast` or `memcpy`.
  The `baseline` snapshot (and every snapshot before WP-14) was built with
  `-fno-strict-aliasing`. `combigrid/perf/scripts/toolchain.py` variant `gcc-sa` re-enables it
  for a pre-WP-14 tree (the wrapper `scripts/wrappers/g++-sa` appends the flag last).

## Pitfalls

- `lib/` and all object files are shared by the debug, ASan and normal builds; `perf.py` refuses
  to benchmark a build it did not make (build stamp) or a non-`normal` build.
- Snapshots are copies of `lib/libsgpp{base,combigrid}.so` + perf binaries, run via
  `LD_LIBRARY_PATH`; they stay valid while the working tree changes.
- The Boost tests seed from the clock; run them several times (`--repeat 3`).
- **`run e2e` runs one process per case** since WP-15 (like `ab`); before, one process ran all
  cases, as when `baseline/e2e_t16.jsonl` was recorded. The suite sum is the same (53.4 vs.
  53.5 s), but a few small cases differ systematically between the two modes:
  `quad/dsg/equi-exp/b0/d3/L16` 96 vs. 82 ms, `interp/sg/cheb1/b0/d5/L4/p64` 337 vs. 385 ms
  (`results/WP-15/run-process-structure.md`). Compare per-case numbers only within one mode.
- **Peak memory of a child**: never read `ru_maxrss` of a process spawned directly from Python
  (`os.wait4`) or `RUSAGE_CHILDREN`: the kernel includes the high-water mark of the image the
  child replaced at exec, i.e. the driver (subprocess uses vfork). `/bin/true` from a Python
  process holding 300 MiB reports 330472 KiB that way, 1536 KiB through GNU time (WP-15).
- The repository path contains spaces; quote paths in shell commands.
- **Throttling (why boost is off)**: with boost on, the laptop throttled under sustained full
  load depending on the conditions (setup session: a 26-minute A/B ran 1.6x slower at noon than
  two days earlier at night; short and 1-thread runs were unaffected). With boost off, burst and
  sustained throughput are equal (`baseline/calibration.json`: 703 vs. 705 ms). A/B comparisons
  are valid either way (A and B are interleaved per case); absolute numbers are only comparable
  when `calibrate` ratios are near 1, and the round durations in A/B reports would show
  throttling. If boost is on after a reboot, `envcheck` fails: disable it again
  (`echo 0 | sudo tee /sys/devices/system/cpu/cpufreq/boost`) — ask the owner.
