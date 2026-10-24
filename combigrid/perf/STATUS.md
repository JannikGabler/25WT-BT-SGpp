# Optimization status — combigrid (master file)

Last processed: WP-00 (setup, 2026-10-24) · Next WP number: 01 · Re-profilings done: 0 (RP-0 = setup)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`),
  16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (all cores at
  2.0 GHz), platform profile `performance`, AC, idle machine.
- **Baseline**: source of commit 84299eeca (unchanged by the setup); baseline commit = the setup
  commit, find it with
  `git log --diff-filter=A --format=%h -- combigrid/perf/baseline/e2e_t16.jsonl`.
  Snapshot name `baseline` (rebuild with `perf.py snapshot baseline --commit <that hash>` if
  `.work/` was cleaned). Numbers: `baseline/`, overview in ANALYSIS.md.
- **Machine note**: boost was disabled and the baseline re-recorded on 2026-10-24 because the
  laptop throttled under sustained load with boost on (up to 1.6x). Boost comes back after a
  reboot unless made persistent; `envcheck` then fails — ask the owner to disable it again
  (README, Pitfalls). A full e2e A/B takes ≈ 23 min; work sessions filter to relevant cases.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed but must be
  labelled and weighed against API/functionality/maintainability/readability; functional/
  interface/flag changes only after consulting the owner; errors stay in the same order of
  magnitude (refcheck OK/WARN/FAIL); high dimensions first.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00 | Setup: infrastructure, baseline, analysis, plan | done (committed by the setup session) | – | [WP-00](summaries/WP-00-setup.md) |
| 1 | WP-01 | SG quadrature: per-grid-point work | ready | | |
| 2 | WP-02 | DSG quadrature: per-delta-tensor-grid overhead at high d | ready | | |
| 3 | WP-03 | Downwards closure of MI sets | ready | | |
| 4 | WP-04 | DSG quadrature: row loop | ready (after WP-02) | | |
| 5 | WP-05 | SG quadrature: per-tensor-grid costs | ready (after WP-01) | | |
| 6 | WP-06 | Global interpolation: operator overhead | ready | | |
| 7 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 8 | WP-08 | Linear interpolation: corner loop | ready | | |
| – | D-1 | Multi-point interpolation API | needs decision | | |
| – | D-2 | `-fno-semantic-interposition` | needs decision (after WP-01) | | |
| – | D-3 | Armadillo/OpenBLAS thread pool | needs decision (low) | | |

## Next action

Issue WP-01 (no decision needed; expected 8–10.5 s of the 70 s e2e suite). Before WP-06…08, ask
the owner about D-1: a multi-point API would change what is worth optimizing in the interpolation
operators.

## Open decisions for the owner

- D-1 multi-point interpolation API (PLAN.md): new public API; potentially the largest gain for
  interpolation workloads.
- D-2 compiler flag `-fno-semantic-interposition`: decide after WP-01 has been measured.
- D-3 Armadillo/OpenBLAS linking: cold-start effect only; low priority.

## Do not retry

(one line per rejected attempt or dropped variant: `WP-NN <idea>: <numbers>, <why>`)
- none yet

## Re-profiling

Accepted optimizations since the last profile: 0 (trigger at 2, or ≥ 1.3x on an e2e group, or a
changed hot-spot ranking). Last profile: setup (ANALYSIS.md). Next: RP-1.

## Log

- 2026-10-24 WP-00 setup finished: perf suite, baseline, reference, noise model, analysis, plan.
- 2026-10-24 boost disabled by the owner; baseline, calibration and noise model re-recorded
  (boost off), ANALYSIS/PLAN numbers updated; reference results unchanged.
