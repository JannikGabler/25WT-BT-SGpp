# WP-15 — Peak-memory measurement in the benchmark harness

Status: PROPOSED · Date: 2026-10-28 · Base commit: 3dc286c3d
Recommendation: accept. Every e2e process's peak RSS is now measured from outside the process and the runtime impact is nil. The owner decided on 2026-10-28 to keep `run e2e` at one process per case (see "API / functionality").
Machine: `perf.py calibrate` ratios e2e 1.005 burst / 0.999 sustained, micro 1.001 / 1.000 (1.000 = as at baseline time)

## Change
- `perf.py` (protected, owner-authorized for WP-15): e2e processes of `run` and `ab` are launched through GNU time (`/usr/bin/time -f %M -o .work/peak_rss.txt`). Its `ru_maxrss` of the benchmark is attached to each record as `peak_rss_kib`. Nothing runs inside the measured binary, so it works with every snapshot incl. `baseline`. New helpers: `launch`, `process_floor` (peak of `perf_e2e --list`), `memory_section`, `fmt_mem`. `run_exe` gets `peak_rss=False`. Unchanged: calibrate, baseline, refcheck, profile, micro, timing, case order, rounds, warm-ups/samples, environment, noise model, verdicts, foreign-load accounting.
- `ab e2e` report: memory table (median per side, B/A, spread, flag) and footer (geomean B/A, largest B/A, flag counts, floor of A and B). Flag `MEM+`/`MEM-` = |B/A − 1| > 3 % **and** the rounds of A and B do not overlap. `run e2e`: column "peak RSS" + floor line. JSON gains `peak_rss_kib` per record and `process_floor_kib`; existing fields unchanged.
- `run e2e` now runs **one process per case** (as `ab` always did); needed for per-case peaks.
- Files: `combigrid/perf/scripts/perf.py` (+130/−13 lines), `combigrid/perf/README.md` (Commands, new section "Peak memory", 2 pitfalls), `summaries/TEMPLATE.md`, `.claude/skills/perf-work/SKILL.md`, `.claude/skills/perf-master/SKILL.md`; reports in `results/WP-15/`. The modified `PLAN.md`/`STATUS.md` are the master's WP-15 scheduling (were uncommitted at start, not touched).

## Gain vs. cost
- Gain: infrastructure. **Runtime impact nil**: full suite, launcher on vs. off (same snapshot, interleaved, 5 rounds): 36/38 `~`, 2 `faster` (1.012), **0 `SLOWER`**, sum 53.453 → 53.440 s (1.0002x). A/A `WP-14-after` vs. itself over 9 cases: all 27 rows `~`, sum 1.001x. Overhead: ≈ 1.2 ms per process (hyperfine: 2.1 vs. 0.95 ms), outside the timed region.
- Floor: `perf_e2e --list` 10.1–10.3 MiB; the smallest cases 11.5–12.2 MiB; 22 of 38 cases < 20 MiB; largest 1.07 GiB (`quad/sg/cc-uq/b0/d2/L20`).
- Threshold: from all 252 5/5 splits of the A/A rounds per case. Ratio-only 2 %: false flags in 5 of 9 cases (up to 57 % of the splits, bimodal `quad/sg/equi-exp/b1/d20/L5`: 53.7 vs. 57–60 MiB). Ratio-only 3 %: 57 % there. 3 % + non-overlap: ≤ 0.8 % per case (= 2/252, the minimum for 5 vs. 5 rounds).
- Memory (first look, `baseline` vs. `WP-14-after`, 9 cases): geomean 1.034; **1 `MEM+`: `quad/sg/cc-uq/b0/d2/L20` 805.9 → 1075.8 MiB (1.335x)**; the other 8 within ±1.8 %. Per-snapshot `run` (3 reps each): the step (+≈ 240 MiB) comes with WP-01 (811–843 → 1054–1072 MiB), not with WP-13/WP-14.
- API / functionality: none in combigrid. **Owner decision (2026-10-28): keep the new mode.** `run e2e` (one process per case) now measures like `ab`, but differs from a shared process for a few small cases. Over 5 alternating full passes the sums are equal (53.33–53.47 vs. 53.46–53.59 s; geomean 1.0025). Two cases are systematic: `quad/dsg/equi-exp/b0/d3/L16` 96 vs. 82 ms, `interp/sg/cheb1/b0/d5/L4/p64` 337 vs. 385 ms. The launcher itself is not the cause (see above). `baseline/e2e_t16.jsonl` was recorded in one process, so compare per-case numbers only within one mode (README pitfall). Rejected alternatives: back to one process (only one peak per run), an `--one-process` switch, or two passes per case.
- Maintainability / readability: +≈ 117 net lines in `perf.py` (4 small functions plus comments); the memory section is separate from the runtime logic.

## Tests
- Not applicable (no combigrid code changed): no pinning tests, refcheck or before/after snapshots, as the prompt says. `perf.py verify` OK at start; at the end it flags `perf.py` as modified (expected; the owner rehashes).
- Validation (`results/WP-15/method-check.md`): GNU time `%M` = kernel VmHWM (0/100/500 MiB test program, ≤ 0.24 MiB off). `/bin/true` from a Python process holding 300 MiB: `os.wait4` gives 330472 KiB (the driver's peak), `perf.launch` gives 1536 KiB. One case, perf.py vs. manual `/usr/bin/time -v` (5 each): same distribution (24.4–25.7 MiB). The CPU time behind the launcher still counts as own load (`RUSAGE_CHILDREN` 62.02 vs. 61.96 s direct; foreign 0.11 %). Smoke tests: `run`/`ab` with e2e and micro, against the old (`baseline`) and current binaries; JSON fields checked.

## Reference check
- n.a. (no combigrid code changed).

## Benchmarks
Launcher on/off, full suite (`launcher-on-off-e2e.md`; A = direct launch, B = GNU time; selection):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | 7.022s | 7.012s | 1.000 | [0.993, 1.006] |
| quad/dsg/cc-uq/b0/d10/L7 | 7.052s | 7.020s | 1.002 | [0.996, 1.009] |
| quad/sg/equi-sin/b0/d7/L7 | 2.411s | 2.412s | 1.001 | [0.993, 1.010] |
| interp/sg/cc/b0/d10/L2/p8 | 2.001s | 2.000s | 1.002 | [0.986, 1.018] |
| interplin/sg/equi/b1/d10/L5/p32 | 2.459s | 2.451s | 1.010 | [0.985, 1.036] |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 2.069s | 2.081s | 0.999 | [0.990, 1.008] |

Sum of medians (38 cases): A = 53.453 s, B = 53.440 s (1.0002x) · Full reports: `combigrid/perf/results/WP-15/`

Peak memory, `ab e2e baseline WP-14-after` (`baseline-vs-WP-14-after-e2e.md`):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 805.9 MiB | 1075.8 MiB | 1.335 | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 327.2 MiB | 327.8 MiB | 1.002 | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 58.1 MiB | 57.8 MiB | 0.995 | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.1 MiB | 37.3 MiB | 1.007 | ~ |

Geomean B/A = 1.034 · process floor A = 10.2 MiB, B = 10.3 MiB. A/A memory (`aa-e2e-memory-final-rule.md`): geomean 0.994, medians within 1.7 %, 0 flags.

## Attempts not kept
- Flag rule ratio-only ±2 % (first version, used in the A/A run): the split analysis gave false flags in 5 of 9 cases (up to 57 %). Replaced by 3 % + non-overlap; `aa-e2e-memory-final-rule.md` re-renders the A/A data with the final rule.

## Profile shift / follow-ups
- RP-1: `quad/sg/cc-uq/b0/d2/L20` +240 MiB since WP-01 (SG quadrature per row; nodes mapped onto the domain once per tensor grid?). Find the cause; a memory-reduction candidate; check whether `quad/sg/equi-exp/b0/d2/L20` (620 MiB) follows the same pattern.
- Per-phase peaks (`VmHWM` + `/proc/self/clear_refs` "5" in `perf_e2e.cpp`; protected, the baseline binary lacks them): 22 of 38 cases are floor-dominated (< 20 MiB, floor 10.1–10.3 MiB). Changes of ≥ ~3 % stay visible there; per-phase peaks would only show which phase holds the peak. Option for a later memory WP, not needed for monitoring.
- `run e2e` process structure: decided (owner 2026-10-28): one process per case stays.

## Revert
Do not use `perf.py reject --apply` while the master's `PLAN.md`/`STATUS.md` edits are uncommitted (it would revert them too). Instead: `git restore combigrid/perf/scripts/perf.py combigrid/perf/README.md combigrid/perf/summaries/TEMPLATE.md .claude/skills/perf-work/SKILL.md .claude/skills/perf-master/SKILL.md`.
