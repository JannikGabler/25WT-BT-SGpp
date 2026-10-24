# WP-00 — Setup of the optimization phase

Status: DONE (setup; committed with the owner's permission) · Date: 2026-10-24 · Base commit: 84299eeca
Recommendation: accept — infrastructure only, no change to the module's source code
Machine: CPU boost off (since the re-recording, see the last section); `perf.py calibrate`
reference in `baseline/calibration.json`: burst 703 ms / sustained 705 ms for
`quad/sg/equi-exp/b0/d5/L10` at 16 threads

## Change
- Measurement infrastructure: dependency-free harness, 38 end-to-end pipelines with phase timings
  (L3 + L2, public API only), 29 micro-benchmarks incl. speed-of-light loops (L1), 104 reference
  cases with exact solutions, driver `perf.py` (build, test, snapshot, run, interleaved A/B,
  calibrate, refcheck, profile, verify, reject, baseline, protect).
- Baseline of the unoptimized code, noise model, reference results and tolerances (protected),
  initial analysis, ranked plan (8 WPs + 3 decisions), coordination layer: STATUS.md, summary
  template, skills `perf-master` / `perf-work`, protection hook, CLAUDE.md §12 pointer.
- Files: `combigrid/perf/**`, `combigrid/examples/c++/benchmarks/perf/*`, `.claude/settings.json`,
  `.claude/hooks/protect_perf.py`, `.claude/skills/perf-{master,work}/SKILL.md`, `CLAUDE.md`
  (§4 judged configuration, §12). Outside git: `compileNormal.sh` now sets
  `USE_EIGEN=1 USE_ARMADILLO=1` (owner: both libraries are available in performance runs).

## Gain vs. cost
- Gain: none intended (setup). Cost: no module source change; ~3 100 lines of benchmark/script
  code and ~1 100 lines of documentation.

## Tests
- Boost tests unchanged and passing: debug `test --repeat 2` ✓ · ASan `test --repeat 2` ✓ ·
  normal `test --repeat 3` ✓. Perf drivers: `perf_reference` in debug and ASan builds matches the
  reference (104/104 OK, no assertion failures, no ASan findings); micro (19 cases) and e2e (3
  cases) smoke runs ✓ in both.
- Reference check validated: identical within the tolerance at 1, 2, 3, 4, 8 and 16 threads;
  negative tests (error x1.5 → WARN, x5 → FAIL, structure/node/missing-case → FAIL, evaluation
  count → INFO, NaN → FAIL); errors at round-off level move within the 4-unit allowance.

## Reference check
- `reference/reference.jsonl` recorded from the baseline snapshot at 16 threads; `refcheck` OK at
  1 and 16 threads on the baseline snapshot and on the final normal build.

## Benchmarks
- Baseline (16 threads, boost off): sum over the 38 e2e cases 70.0 s; by group quad/sg 22.3 s,
  quad/dsg 26.8 s, interp 6.6 s, interplin 5.0 s, mivec 9.3 s (ANALYSIS.md). A full e2e A/B
  (5 rounds) takes ≈ 23 min.
- Noise model (6-round self-comparisons, boost off): median per-round spread 0.50 % (e2e) /
  0.39 % (micro), 90th percentile 2.0 % / 0.9 % → typical 95 % CI ±0.6 % / ±0.5 % with the
  default 5 rounds; no false positives in either self-comparison (with boost on: 0.67 % / 1.05 %
  and 4–10 % false positives without noise model, expected 5 %).

## Attempts not kept
- Bootstrap over samples within rounds (3 rounds × 2 samples): 18/114 false positives — the
  dominant noise is between processes → paired per-round t-interval with many short processes.
- Reference tolerance as error ratio above a round-off floor (16 units): missed 5x error growth for
  cases near round-off → explicit rule `e_new ≤ ratio·e_ref + 4 units`.
- Noise model from a plain std: single rounds at the onset of throttling inflated it (up to 20 %)
  → robust std (MAD).
- Baseline micro suite recorded right after the 16-thread e2e run was 10–25 % slow (throttled) →
  recording order: idle settle, calibration, micro, e2e.

## Profile shift / follow-ups
- Biggest potentials (PLAN.md): complete-SG quadrature per-point work (3x gap to speed of light,
  instruction-bound, ~555 instructions/point at d10), DSG sub-trie construction at high d
  (63 % at b1 d20), bounding-box downwards closure (96 % of MIVec at d20), per-query-point
  re-evaluation in interpolation (interface decision D-1).
- **Machine**: the laptop throttles under sustained 16-thread load, how much depends on the
  conditions (1.6x on 2026-10-24 noon vs. 2026-10-22 night in long A/B runs; short runs
  unaffected). A/B stays valid (interleaved); `calibrate` and the round durations in A/B reports
  make it visible. Owner option: disable boost for more stable absolute numbers.
- Open decisions for the owner: D-1 (multi-point interpolation API), D-2
  (`-fno-semantic-interposition`, after WP-01), D-3 (Armadillo/OpenBLAS linking).
- Pre-existing compiler warnings (not performance-related, untouched): `#pragma once` in
  `tools/operators/quadrature/quadrature_utilities.cpp:1`; unused variable `sum` in
  `grids/tensor_grid.cpp:34` (NDEBUG builds).

## Re-recording with boost off (2026-10-24, owner's instruction)
- The owner disabled CPU boost (`cpufreq/boost = 0`, platform profile `performance`) and improved
  the airflow. Re-recorded with `perf.py baseline --authorized-by-owner` and two 6-round
  self-comparisons: `baseline/` (environment, calibration, micro, e2e, noise models); manifest
  rehashed with `perf.py protect --authorized-by-owner`. `reference/` unchanged (boost does not
  affect results). Profiles, thread scaling and counters re-measured; ANALYSIS.md, PLAN.md,
  STATUS.md and README updated.
- Effect: no throttling any more (round durations 288 s ×6, burst = sustained); 1.62x slower at
  16 threads and 2.1x at 1 thread; profile shares unchanged (±2 points) except the CC weight FFT
  (18 % → 11 % of the d = 2 CC case); 8 threads now scale 7.7–7.9x, SMT adds 0–30 %.
- Boost is re-enabled by a reboot unless made persistent; `envcheck` then refuses to measure.

## Revert
`git revert <setup commit>` (the setup touches no module source).
