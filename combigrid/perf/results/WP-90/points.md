# WP-90 points — restructuring of the dimension-adaptive refinement (owner, point by point)

Session start: 2026-11-27 (HEAD 182f9cd30, branch `optimizations`). Resume from §1.

## 1. Current state and next step

- Phase 1 (orientation) done; orientation + candidate points (§3) given to the owner. The owner
  wants session findings that relate to an owner point reported with that point; the remaining
  candidates are reviewed by the owner after their own points.
- P1 implemented (verification in progress, see §2). Benchmarks of judged code: one run at the end
  of the WP (owner 2026-11-27).
- Environment at start: `verify` ✓ (40 files), `envcheck` ✓ (busy 0.3 %, boost 0, AC),
  `calibrate` 1.005 / 1.000 (e2e burst / sustained), 0.998 / 0.998 (micro).
- Tree at start: the master's uncommitted `PLAN.md` / `STATUS.md` / `HISTORY.md` edits (WP-90 entry),
  the owner's untracked `compile*.sh`; no source change.
- Snapshot `WP-90-before` = in-tree normal build of 182f9cd30 (built at session start).

## 2. Points (owner's requests, in order)

Per point: request · options · decision · change · files · tests · measurements · status.

### P1 — remove `SGGenInstr::clone()`; rename the generation-instruction files

- **Request** (owner 2026-11-27): `clone()` should be obsolete since `SparseGridSetup`; if so, remove
  it with all notes and references. Rename `sg_gen_instruction.{hpp,cpp}` →
  `sparse_grid_generation_instruction.{hpp,cpp}` (naming consistency).
- **Finding**: obsolete — no C++ caller since WP-89 (grids copy the setup); its only user was the
  Python helper `cloneShared` (`%rename`d to `clone`), used by no example / test. Only the `.hpp`
  exists (no `.cpp`). The rename touches 8 protected files (10 include lines).
- **Decisions** (owner): (a) protected includes: the owner edits them and rehashes (`perf.py protect
  --authorized-by-owner`; done 2026-11-27, `verify` ✓); (b) rename the siblings too:
  `complete_sg_gen_instruction.*` → `complete_sparse_grid_generation_instruction.*`,
  `multiindex_vector_sg_gen_instruction.*` → `multiindex_vector_sparse_grid_generation_instruction.*`;
  (c) Python: drop the `%shared_ptr` wrapping of `SparseGridSetup` / `SGGenInstr` /
  `CompleteSGGenInstr` / `MIVecSGGenInstr` too (its only reason was `clone`). Class names unchanged.
  Benchmarks of judged code: delayed to one run at the end of the WP (owner).
- **Change**: `git mv` of the 5 files (+ stale `.os` / `.lint` of the old names deleted); include
  paths / `@file` / doc links in 81 files (src, tests, examples, unprotected benchmarks incl.
  `perf_micro` / `perf_adaptive` / `master_research_project`, `combigrid.i`, module-reference); not
  rewritten: `perf/results/**` (historical); `clone()` declaration + 2 overrides + 2 definitions,
  unused `<memory>` includes; `combigrid.i`: `cloneShared` / `%ignore` / `%rename` / `%pythonappend`
  of clone and the 4 `%shared_ptr` removed, `getSetup` `%extend` returns `SparseGridSetup` by value;
  docs: CLAUDE.md §6 (MIVec reference gotcha), module-reference (`SGGenInstr` entry),
  python-bindings.md (bound API list, decision 2 keep-alives, decision 3 `%shared_ptr`, decision 8
  removed / 9 → 8, troubleshooting row).
- **Related findings → summary (master: hygiene backlog)**: F1 `complete_…instruction.{hpp,cpp}` and
  `multiindex_vector_…instruction.hpp` lack the copyright banner, both headers use `#ifndef` guards
  with stale names (`COMBIGRID_FULL_SPARSE_GRID_…`, `COMBIGRID_MISETSGGENINSTR_HPP`) instead of
  `#pragma once`, `complete_…instruction.cpp` lacks the final newline; F2 `TODO: Document` on the
  `SGGenInstr` generation methods; F3 fully commented-out files `extensive_sparse_grid.{hpp,cpp}`,
  `sparse_grid_generation_node_lookup.{hpp,cpp}` + its test held the old include in comments (renamed;
  now > 100 columns → cpplint / clang-format warnings in dead code; `extensive_sparse_grid.cpp` lacks
  the final newline).
- **Tests**: no behavior change; existing suite (construction / setup tests, all operator tests).
  Python: `results/WP-90/python_checks_point1.py` (no `clone`, plain proxies, `getSetup` copy,
  grids without their instruction) + the 12 examples.
- **Checks**: debug build ✓, debug `test --repeat 3` (running); Python build + checks, normal build,
  refcheck, `perf_reference` diff vs. `WP-90-before`: pending.
- **Status**: implemented, verification in progress.

## 3. Candidate points raised by the session (not implemented unless the owner pulls them in)

Raised in the orientation (2026-11-27); the owner decides which become points.

- C1 Engine: architecture §5.1's chunked cheap tasks and work gate were **not implemented**
  (one lock round trip per TG, `notify_all` per decision; d100 cheap only 2.3x at 16 t).
- C2 `RefinementEngine` is one ~540-line class with all roles (pool, queues, decisions,
  speculation, stopping, statistics); `RefinementState` is a public struct of parallel vectors with
  sentinels (gain < 0 = not computed, `evaluated` beside `states`).
- C3 Speculative multi-indices enter the index set → index order (tie-break / commit order) differs
  from window 0 (round-off differences); unused ones stay in the state for good and are not counted
  by `maxTensorGrids`.
- C4 Option names / semantics: `maxGridPoints` is a per-call evaluation budget (incl. a restart's
  re-evaluation), not a grid size; `maxTensorGrids` counts the closure incl. coefficient-0 indices;
  `statistics.nTensorGrids` ≠ `grid.nTG()`; `RefinementStopReason::None` also on an exception.
- C5 Hand-over: `gridIndicesInOrder` (colex sort) runs twice per call (`endHandOver` and
  `combineEstimate`); `runCall` copies all coefficients for change detection; two estimates
  (running compensated sum for the relative tolerance vs. re-combination in colex order).
- C6 Restart semantics: a new object, or any foreign grid change, discards everything and
  re-evaluates the whole grid within the call's budget (budget < grid cost → stops at once).
- C7 Public `RefinementTarget` exposes internals (`NodeTable`, worker numbers, unit + domain
  `TensorGridView`, "rows generated during the call" contract); interpolation targets evaluate a
  TG P times (follow-up 7).
- C8 Asynchronous schedule: decides whenever < 2·workers tasks are queued → much worse grids per
  budget; redesign (follow-up 3) or drop.
- C9 Node storage: capacity rows for levels 0 … max(`maxLevel` = 30, existing) per dimension are
  allocated as empty rows each call (d100: 3100 rows per table); fine but O(d · maxLevel).
- C10 Only `SparseGrid` (not `SparseGridBase` / DSG); the object holds `SparseGrid&` (a moved grid
  dangles).

## 4. Open at the end

(filled at the end of the WP)
