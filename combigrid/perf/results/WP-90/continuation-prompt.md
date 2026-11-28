You are a work session of the combigrid optimization phase of SG++ (repo root = current
directory). You **continue** work package WP-90, which an earlier session started; follow the
standard procedure in the skill `perf-work` (.claude/skills/perf-work/SKILL.md) where it applies
(tests, snapshots, reference check, benchmarks, revert rules, summary format). Command reference:
combigrid/perf/README.md. Do not commit.

Work package WP-90 (continued): Dimension-adaptive CT — restructuring of WP-89's implementation,
point by point with the owner (owner 2026-11-27). Interactive session: the owner brings the points
(and decides every API / behavior change in-session); your own findings go to the owner as
candidate points and are never implemented unasked; findings that relate to an owner point are
reported with that point.

State at the start of this session (details and per-point records: combigrid/perf/results/WP-90/points.md,
the resumable log; summary draft: combigrid/perf/summaries/WP-90-adaptive-restructuring.md):
- Done and verified (debug `--repeat 3`, Python build + checks + 12 examples, normal build,
  refcheck = only the known WARN `interp/sg/equi/b0/d2/L5/cos-shifted`, `perf_reference` bitwise
  identical at 1 / 16 threads):
  - P1: `SGGenInstr::clone()` removed; files renamed to `sparse_grid_generation_instruction.hpp`,
    `complete_sparse_grid_generation_instruction.*`, `multiindex_vector_sparse_grid_generation_instruction.*`;
    Python `%shared_ptr` of the setup / instructions dropped (`getSetup` returns a copy by value).
  - P2: `NodeGenFunc` → `NodeFamily` (pointer-based singletons kept, non-copyable), `EquidistantNodes`,
    `ClenshawCurtisNodes` (+ alias `SecondTypeChebyshevNodes`), `FirstTypeChebyshevNodes`, getters
    `get*Nodes()`, setup `set/getNodeFamily(ForDim)` / `set/getNodeFamilies` / `getUniqueNodeFamilies`;
    files in a top-level `node_families/`.
  - P5: unused FNV-1a ids removed (`NodeFamily`, `QuadRule`, `InterpolationMethod`,
    `tools/hashing/fnv_1a_hash.hpp`); `getBarycentricFormula()` is one static instance.
  - P1, P2, P5 are committed by the owner (checkpoint commits).
  - P3 (in the working tree; check `git status` whether the owner committed it since): `NodeStorage`
    (shared, lazily generated, reference-counted 1D node rows keyed by `NodeRowKey`; default
    `NodeStorage::global()`; thread-safe; C++ only, hidden from Python) + non-owning `NodeTable`
    (span + `shared_ptr` per row); rows always include the boundary nodes (levels without them view
    `[1, n−1)`); domain rows on [0, 1] are the unit rows; `SparseGridSetup::get/setNodeStorage`
    (excluded from `==`); `node_table_generation::genNodeTables` / `setRows`; the refinement's
    `RefinementNodeStorage` copies row handles. Also ASan ✓, TSan (`results/WP-90/tsan/`: 0 reports
    without libgomp workers). Exploratory micro A/B (`results/WP-90/explore_p3_micro.md`): kernels
    within noise, unit-cube generation 1.05–1.94x faster, `gen/sg/equi/b0/d5/L10` +5 µs (0.959),
    grid heap sg cc b0 d2 L20 67 → 34 MB (= WP-58's gain).
- Open:
  - P6 (proposed, **not decided**): simplify `NodeTable` to a per-dimension map level → storage
    row (explicit serial `request`, `reserve` / `truncate` for the refinement, no row sharing between
    dimensions inside the table, `QuadWeightTable` deduplicates weights by (family, node count,
    boundary)) — full proposal and alternatives in points.md "P6".
  - P4 (refinement node handling): options in points.md "P4"; with P6 `RefinementNodeStorage`
    disappears entirely.
  - The default schedule (`Synchronized`, `speculationWindow` 0 "for now") is still to be revisited
    with the owner.
  - Session candidate points C1–C10 (points.md §3: engine without the planned chunking / work gate,
    engine and state structure, speculation side effects, option names, duplicate hand-over work,
    restart semantics, public target interface, async schedule, capacity rows (resolved by P3),
    `SparseGrid&` only) — the owner reviews them after their own points.
  - Hygiene findings F1–F3 (+ missing banners in `node_families/*.cpp`, "node generator" wording,
    raw-pointer `getQuadRule` / `getInterpolationMethod`) go to the summary for the master's hygiene
    backlog only.
- Snapshots: `WP-90-before` (182f9cd30), `WP-90-p1`, `WP-90-p2`, `WP-90-p5`, `WP-90-p3` (each a normal
  build after its point; bitwise reference outputs of each in the previous session's scratchpad are
  gone — regenerate them from the snapshots when needed: README "Pitfalls").
- Benchmarks of judged code (owner): one full `ab e2e` + memory at the end of the WP
  (`WP-90-before` vs. `WP-90-after`), plus the feature benchmarks (`perf_adaptive`,
  `micro/adaptive/*`) before / after for the refinement points; exploratory runs in between are fine.

Phase 1 — re-orientation (before anything else; no code changes):
- `git status --short` / `git log -5` (expected: the P3 changes uncommitted or committed by the owner,
  the master's perf files, the owner's untracked `compile*.sh`); `perf.py verify`, `envcheck`,
  `calibrate` (ratios into points.md).
- Read points.md completely (P1–P6, §3 candidates), the summary draft, and the code of P3:
  `grids/node_storage.*`, `grids/node_row_key.*`, `grids/node_table.*`, `grids/tensor_grid_view.hpp`,
  `grids/sparse_grid_setup.*`, `tools/sparse_grid_generation/node_table_generation.*`,
  `miscellaneous/data_contexts/quadrature_weight_table.*`, `miscellaneous/refinement/refinement_node_storage.*`,
  `tools/refinement/refinement_hand_over_tools.*`; tests `tests/grids/node_storage_test.cpp`,
  `tests/grids/sparse_grid_storage_test.cpp`. For WP-89 background only as needed:
  results/WP-89/architecture.md, summaries/WP-89-dimension-adaptive-ct.md.
- Build (`perf.py build debug`) and run `perf.py test` once to confirm the tree is green.
- Then give the owner a **short** status (what is done, what is open: P6 proposal, P4, default
  schedule, candidates) and **wait for the owner's plan for what comes next**. Do not start P6 or
  any other point before the owner says so.

Phase 2 — per owner point (as before): (1) restate it and locate the affected code; (2) propose
options with their effect on API, behavior / results (bitwise?), performance and complexity, plus a
recommendation; (3) the owner decides; (4) implement with tests first / updated, keep the build and
the tests green after every point; (5) record the point in points.md (request, decision, change,
files, tests, measurements, status). After each point the owner may make a checkpoint commit.

Checks (as before): per point the relevant tests; debug `perf.py test --repeat 3` whenever judged
code changed and at the end; ASan when memory handling / lifetimes change and once at the end
(memory-capped: `systemd-run --user --scope -q -p MemoryMax=8G`); TSan via results/WP-90/tsan/tsan.sh
(`serial` = no libgomp workers, every report real) or the WP-89 driver if the engine's threading
changes; where judged code (grids, generation, operators, weight tables, kernels) is touched:
refcheck (1 and 16 t), the snapshot `perf_reference` bitwise diff, and the final `ab e2e`. Python:
after changes to bound headers rebuild with `SG_PYTHON=1` (in-tree; it shares `lib/`, so never in
parallel with another build) and run results/WP-90/python_checks_point{1,2}.py + the examples in
combigrid/examples/python with the repo's `.venv/` (`list(DataVector)` aborts: WP-37, use indices).
Protected files (README "Protection"): when a rename touches them, stop and give the owner the
exact `sed` command + `perf.py protect --authorized-by-owner`; never edit them yourself.

Conventions to keep (CLAUDE.md): banner, `#pragma once`, absolute includes, clang-format (never on
`.i` files), Doxygen, one type per file, methods ≤ 30 lines, comments stay general (no WP ids in
code), docs (CLAUDE.md §2 / §6, combigrid/claude/module-reference.md, python-bindings.md) kept in
line with every change. Strict aliasing is on. Prefer generic designs; no allocations on hot paths.

Deliverables at the end of the WP (unchanged): summary combigrid/perf/summaries/WP-90-adaptive-restructuring.md
(from TEMPLATE.md; "Change" lists the points with the owner's decisions; F1–F3 etc. under
follow-ups), results/WP-90/points.md, results/WP-90/architecture-delta.md (changes to WP-89's
architecture.md), reports in combigrid/perf/results/WP-90/, snapshot WP-90-after. Points left open
at the end: list them in points.md and propose follow-up WPs in the summary. End with a short
message: status, recommendation, headline numbers.
