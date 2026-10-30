---
name: perf-work
description: Standard procedure of a work session of the combigrid optimization phase (SG++): exactly one work package (WP) from combigrid/perf/PLAN.md, with pinning tests, before/after snapshots, reference check, A/B benchmarks at all levels and a fixed-format summary. Use when a prompt asks to carry out a combigrid performance work package or refers to the perf-work procedure.
---

# Work session procedure (combigrid optimization phase)

You carry out **exactly one work package** given in the prompt of the master session. The owner
reviews and commits afterwards; the master plans the next step from your summary alone. Command
reference and methodology: `combigrid/perf/README.md` (read it first). `perf.py` below means
`python3 combigrid/perf/scripts/perf.py` (run from the repo root; the path contains spaces).
`NN` is the WP number from the prompt.

## Rules

1. **Scope**: one coherent optimization, the one in the prompt. Ideas beyond it go into the
   summary ("Follow-ups"), not into the code.
2. **No commits**, no pushes, no branch switches. Leave the result in the working tree.
3. **Protected files** (baseline, reference, tolerances, `perf.py`, `perf_e2e.cpp`,
   `perf_reference.cpp`, `perf_harness.hpp`, `perf_problems.hpp`, pre-existing benchmarks) are
   never changed. A hook blocks edits; `perf.py verify` must pass at start and end.
4. **Functional or interface changes** (public API or observable behavior, results beyond a
   refcheck WARN, new dependencies, compiler flags, build configuration) are not made on your
   own. If the WP turns out to need one: stop before implementing it, finish with status
   `BLOCKED` and describe the options with their expected gain. Internal helpers in
   `*_operator` / `tools` / `misc` namespaces may change; keep the Python bindings
   (`combigrid/build/pysgpp/`) unaffected, and if a bound header changes, say so.
5. **Tests first**: tests that pin the current behavior of the code you change are written (or
   identified) and **pass on the unchanged code** before you change it. New code gets new tests.
6. **Evidence at every level**: L1 micro-benchmark at the optimized spot, L2 phase (`operator`
   or `generation` metric) and L3 end-to-end (`total`, 16 threads) for all e2e cases that
   exercise the changed code. Acceptance is decided by the owner, never by you. `ab e2e` also
   reports the peak memory (max RSS) per case: report it alongside the runtime.
7. **Honest reporting**: speedups inside the CI noise band are "within noise". Even minimal gains
   may be proposed, but say that they are minimal and weigh them against the costs (API,
   functionality, maintainability, readability).
8. **Failed variants** you try and drop are reverted by you and recorded in the summary (what,
   numbers, why). The final state you leave is your best variant (or nothing, see step 7).
9. **One measurement at a time** on an idle machine: no builds or other heavy commands while
   `perf.py run/ab/profile` runs. If `envcheck` fails, fix or wait; `--no-envcheck` only for
   exploratory numbers that do not go into the summary. A report with a foreign-CPU-load WARN
   (another process was busy during the measurement) is repeated, not used.
10. Code style per CLAUDE.md (banner, includes, Doxygen, naming). Efficiency matters: no
    allocations on hot paths, measure claims, prefer generic designs over special-case dispatch
    (see the user memories, if available).

## Steps

**0. Orient** (keep it short)
- Read the prompt, `combigrid/perf/README.md`, the code at the WP location, and the "Do not
  retry" entries quoted in the prompt.
- `git status --short`: tracked files must be unmodified (the owner commits after every session);
  untracked files outside `combigrid/` (e.g. the owner's `compile*.sh`) belong to the owner —
  leave them alone. Anything else: ask the user before you start.
- `perf.py verify`, `perf.py envcheck` and `perf.py calibrate` (records whether the machine runs
  the baseline binaries like at baseline time; put the ratios into the summary). If the snapshot
  `baseline` is missing (`combigrid/perf/.work/snapshots/baseline`), build it first:
  `perf.py snapshot baseline --commit $(git log --diff-filter=A --format=%h -- combigrid/perf/baseline/e2e_t16.jsonl)`.

**1. Pin the behavior** (before changing source code)
- Identify existing tests covering the code; add or extend Boost tests in `combigrid/tests/`
  (mirror the src layout; CLAUDE.md §4) so that the results/structure of the code you change are
  pinned, including edge cases (d = 1, boundary offsets 0/1, non-unit domains, empty/size-1
  grids where applicable).
- `perf.py build debug` then `perf.py test --repeat 3` on the **unchanged** source: must pass.
  Record which tests pin the behavior.

**2. Micro-benchmark at the spot**
- Use an existing `micro/...` case or add one to `perf_micro.cpp` (README: "Adding a
  micro-benchmark"). It must compile against the unchanged code.

**3. "Before" snapshot**
- `perf.py build normal`, `perf.py test`, `perf.py snapshot WP-NN-before`.
- `perf.py refcheck --snapshot WP-NN-before` must be OK (otherwise the environment is broken:
  stop and report).
- Optional: `perf.py profile <case> --snapshot WP-NN-before` to confirm the hot spot.

**4. Implement**
- Iterate in the debug build (asserts on): `perf.py build debug`, `perf.py test`. For quick
  performance checks of variants: `perf.py build normal` + `perf.py run micro --filter ...`.
- Drop and revert variants that do not pay off (record them for the summary).

**5. Verify**
- Debug: `perf.py build debug`, `perf.py test --repeat 3`.
- ASan (if memory handling, indexing or lifetimes changed): `perf.py build asan`, `perf.py test`.
- Normal: `perf.py build normal`, `perf.py test --repeat 3`,
  `perf.py refcheck --save combigrid/perf/results/WP-NN/refcheck.txt`.
  FAIL → fix (or BLOCKED if inherent); WARN/INFO → keep only if justified, state it.

**6. Measure**
- `perf.py snapshot WP-NN-after`.
- L1: `perf.py ab micro WP-NN-before WP-NN-after --filter '<spot cases>' --save combigrid/perf/results/WP-NN --name micro`
- L2+L3: `perf.py ab e2e WP-NN-before WP-NN-after --filter '<relevant cases>' --save combigrid/perf/results/WP-NN --name e2e`
  (relevant = every case that runs the changed code; the prompt names them; if in doubt, the
  full suite). If CIs are too wide to decide, repeat with `--rounds 8` (more processes help;
  more samples per process do not).
- Optional: `perf.py profile` of the main case after the change ("profile shift").

**7. Assess**
- Gain vs. cost: absolute time saved (sum line), high-d cases first; API/functionality impact;
  maintainability/readability.
- Memory: peak-RSS change from the `ab e2e` report (geomean B/A, `MEM+`/`MEM-` cases) is weighed
  in the decision; runtime stays the ranking metric, a memory reduction is a valid secondary gain.
- If no variant gains beyond noise on any relevant case: status `NO-GAIN`, recommend reject, but
  leave the final variant in the tree for the owner's review (the owner runs `perf.py reject`).

**8. Summary** — `combigrid/perf/summaries/WP-NN-<slug>.md`, filled in from
`combigrid/perf/summaries/TEMPLATE.md` (fixed headings, ≤ ~80 lines, numbers from the saved
reports). Then `perf.py verify`, `git status --short`, and end with a short message to the user:
status, recommendation, headline numbers, summary path.

## Status values

- `PROPOSED`: the change works, evidence complete, awaiting the owner's decision.
- `NO-GAIN`: implemented and measured, no gain beyond noise; recommendation: reject.
- `BLOCKED`: needs a decision (functional/interface/flag change) or is not feasible; nothing
  half-done remains in the tree except tests/benchmarks that are useful on their own (list them).
- `ABORTED`: environment/tooling problem; describe it.
