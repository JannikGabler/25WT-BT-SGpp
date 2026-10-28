# WP-16 — Adapt `perf.py` and the protection to the benchmark move

Status: PROPOSED · Date: 2026-10-28 · Base commit: 666b3c601
Recommendation: accept. `build`, `snapshot`, `run`, `refcheck` and `ab` work with the new layout. `snapshot --commit` of pre-move commits still works. The A/A comparison of the old and new layout is equal within noise for runtime and memory. The owner still has to rehash the manifest (commands at the end).
Machine: `perf.py calibrate` ratios e2e 1.011 burst / 1.003 sustained, micro 0.996 / 1.000 (1.000 = as at baseline time)

## Change
- `perf.py` (protected; the owner authorized the change for WP-16), +25/−12 lines:
  - `BENCH_SRC` is replaced by `BENCH_DIRS = ["combigrid/benchmarks/perf", "combigrid/examples/c++/benchmarks/perf"]` and a new `bench_dir(tree)`. It returns the first entry whose `perf_e2e.cpp` exists in that tree. It checks the source rather than the directory, so binaries left behind at a former location are ignored. If neither entry matches, it falls back to the new layout. `current_build` (in-tree build) and `copy_build` (in-tree and `snapshot --commit` worktree) use it, so the layout is resolved per tree without a list of commits.
  - `PROTECTED_GLOBS`: the 9 globs point to the new paths, mapped 1:1 (same set of files; `thesis/comparison` → `benchmarks/bachelor_thesis/comparison`).
  - Nothing else changed: timing, peak memory (WP-15), rounds, noise model, verdicts, reports, `reject`.
- `README.md`: the layout block shows `combigrid/benchmarks/perf/` plus a note on the pre-move layout and `BENCH_DIRS`. The protection paragraph uses the new paths.
- `toolchain.py` (not protected): the warning-area classifier gets `combigrid/benchmarks`; otherwise those warnings would be counted as "other". Snapshot and current-build handling come from `perf.py`.
- `CLAUDE.md`: two `SGppConfigure.py` line anchors were stale (the refactor commit removed 6 lines): `-fno-semantic-interposition` 510-518 → 502-511, `ARCH=avx2` 568-571 → 571-574 (the second was already off before the commit).
- `.gitignore`: the owner added `*/benchmarks/**` / `!*/benchmarks/**/` / `!*/benchmarks/**/*.*` and `compile_commands.json` during this session. My own rule was identical, so I removed my copy: the `.gitignore` diff is entirely the owner's. Checked: the 19 binaries (plus `.o`, `.lint`, `build.log`) are ignored; `.cpp`/`.hpp` files stay trackable, including new files and new subdirectories (`git check-ignore`).
- Files: `combigrid/perf/scripts/perf.py`, `combigrid/perf/scripts/toolchain.py`, `combigrid/perf/README.md`, `CLAUDE.md`, `results/WP-16/`, this summary. The `PLAN.md`/`STATUS.md` changes are the master's and were not touched. Skills and `combigrid/claude/*.md` had no stale paths.

## Gain vs. cost
- Gain: infrastructure only, no runtime effect. A/A `WP-16-old` (a485cec02, old layout) vs. `WP-16-new` (working tree, new layout): all 15 rows `~`. Sum of medians 15.961 → 15.940 s (1.001x).
- Memory: unchanged. Geomean B/A 0.994, 0 `MEM+`/`MEM-`, process floor 10.6 / 10.5 MiB.
- API / functionality: none in combigrid. **Protection gap until the rehash**: the hook still reads the old manifest, so the moved files are unguarded. A synthetic Edit of `combigrid/benchmarks/perf/perf_e2e.cpp` → exit 0. Dry run with a manifest generated in the scratchpad (real manifest untouched): exit 2 for `perf_e2e.cpp`, `sin_example_template.cpp` and a new `benchmarks/quadrature/*.cpp`; exit 0 for `perf_micro.cpp` and for the old path.
- Maintainability: one 7-line function plus a 2-entry list. Keep the old entry: the baseline commit fed2cdb4e is pre-move.

## Tests
- Not applicable (no combigrid code): no pinning tests and no before/after snapshots in the usual sense. Instead, `WP-16-old` / `WP-16-new` are the same sources (`git diff -M a485cec02 HEAD`: pure renames, 0 lines changed in `base/src`, `combigrid/src` and the perf benchmarks).
- `perf.py test` (normal) ✓. `build normal` 268 s with 2 compiler warnings, both already present in the committed code: unused `sum` in `tensor_grid.cpp:34`; `#pragma once` in `quadrature_utilities.cpp`.
- `verify` at the start: fails, 22 of the 34 manifest entries missing. The master's figure of "34" is the size of the manifest; 22 entries are the moved files. At the end: 22 missing (old paths), 22 "new file in protected location" (the same files at the new paths), `perf.py` modified. 33 of 34 hashes are identical at the new paths; only `perf.py` differs. → OK after `protect`.
- `bench_dir`: fed2cdb4e and a485cec02 resolve to the old layout, HEAD to the new one. Synthetic trees: old sources plus a leftover binary in the new directory → old; new → new; neither → new.

## Reference check
- `perf.py refcheck --snapshot WP-16-new`: OK, 104/104 at 1 and 16 threads (`results/WP-16/refcheck-WP-16-new.txt`).

## Benchmarks (A = WP-16-old = a485cec02 via `snapshot --commit`, B = WP-16-new = working tree; A/A)
Smoke tests on the in-tree build: `run e2e` (2 cases, with peak RSS) and `run micro` (`micro/sol/quad_tg`, 2 cases) work.

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | total | 7.032s | 7.031s | 0.999 | [0.964, 1.036] |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.413s | 2.412s | 1.000 | [0.992, 1.008] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.989s | 1.988s | 1.001 | [0.989, 1.014] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.452s | 2.438s | 1.015 | [0.979, 1.053] |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.074s | 2.070s | 1.014 | [0.980, 1.049] |

Sum of medians: A = 15.961 s, B = 15.940 s (1.001x, within noise) · Foreign load 0.2 %/round · Full report: `results/WP-16/aa-e2e-old-vs-new.md`

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | 327.7 MiB | 328.8 MiB | 1.004 | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.1 MiB | 24.1 MiB | 0.999 | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.6 MiB | 0.995 | ~ |

Geomean B/A = 0.994 · process floor A = 10.6 MiB, B = 10.5 MiB

## Attempts not kept
- My `.gitignore` rule: an exact duplicate of the owner's simultaneous edit, so I removed it.

## Profile shift / follow-ups
- `perf_problems.hpp:132` (protected, moved) still names `combigrid/examples/c++/thesis/comparison/uq_electron_beam_template.cpp`. Owner: if you fix it, do so **before** `protect` (the hash changes).
- The refactor commit also deleted the WP-14 comment in `SGppConfigure.py` (`configureGNUCompiler`: why strict aliasing is not disabled globally). Possibly unintended; CLAUDE.md §4 still has the explanation.
- Complete-project checks: the C++ example count changes. The benchmarks and the thesis comparison templates are no longer run by `RUN_CPP_EXAMPLES`; the templates exited 1 without arguments in WP-14. Compare only against references with the same layout. The README does not describe these checks, so I added no README note.
- Historical records keep the old paths: summaries, `results/`, `ANALYSIS.md`, `baseline/environment.json` (protected).

## Owner's post-review commands
1. Re-enable the hook: delete `.claude/settings.local.json` (or remove `"disableAllHooks": true`).
2. `python3 combigrid/perf/scripts/perf.py protect --authorized-by-owner`. Then `git diff combigrid/perf/protected.sha256` should show only path changes, the 9 `#glob` lines and the new `perf.py` hash.
3. `python3 combigrid/perf/scripts/perf.py verify` → expect `verify: 34 protected files unchanged`.
4. Hook test (expect the BLOCKED message and `exit 2`):
   `printf '{"tool_name":"Edit","tool_input":{"file_path":"%s/combigrid/benchmarks/perf/perf_e2e.cpp"},"cwd":"%s"}' "$PWD" "$PWD" | python3 .claude/hooks/protect_perf.py; echo "exit $?"`

## Revert
Do not run `perf.py reject --apply` while the master's `PLAN.md`/`STATUS.md` and the owner's `.gitignore` are uncommitted: it would revert them too. Instead: `git restore combigrid/perf/scripts/perf.py combigrid/perf/scripts/toolchain.py combigrid/perf/README.md CLAUDE.md`.
