# WP-90 points — restructuring of the dimension-adaptive refinement (owner, point by point)

Session start: 2026-11-27 (HEAD 182f9cd30, branch `optimizations`). Resume from §1.

## 1. Current state and next step

- Phase 1 (orientation) done; orientation + candidate points (§3) given to the owner. The owner
  wants session findings that relate to an owner point reported with that point; the remaining
  candidates are reviewed by the owner after their own points.
- P1 done and verified (§2; owner checkpoint commit). P2 done and verified; P5 (ids, owner's
  third point) done and verified; waiting for the owner before P3 (owner 2026-11-27). Snapshots:
  `WP-90-before`, `WP-90-p1`, `WP-90-p2`, `WP-90-p5`. Benchmarks of judged code: one run at the end
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
- **Checks**: debug build ✓, debug `test --repeat 3` ✓; Python build (`SG_PYTHON=1`, OPT=0, in-tree)
  ✓: `python_checks_point1.py` ✓, WP-89 `python_checks.py` ✓, 12 / 12 examples ✓; normal build + `test`
  ✓ (snapshot `WP-90-p1`); `refcheck` (`refcheck_p1.txt`): WARN only the known
  `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34, since WP-06); `perf_reference` at 1 and 16 threads
  **bitwise identical** to `WP-90-before` (146 / 146). Benchmarks: at the end of the WP (owner).
- **Status**: done (2026-11-27).

### P2–P4 — node storage (owner's point 2, split into three steps)

- **Request** (owner 2026-11-27): replace the per-grid node tables (unit + domain, handed between
  grids and refinement objects) by a `NodeStorage` that manages 1D nodes for node families / counts;
  grids (and refinement objects, …) reference a storage (via `SparseGridSetup`; default a global
  static one, users may set their own) and read nodes from it; lazy generation, add-only at first;
  domain nodes: proposals wanted. `NodeTable` etc. may be renamed / changed / deleted.
- **Proposal and decisions** (owner 2026-11-27):
  - Rows with / without boundary share memory: the storage keeps the row with boundary, "without"
    is the span `[1, n−1)` (owner's idea; bitwise valid: the base class builds "with boundary" as
    `[0] + genNodesWithoutBoundary(n−2) + [1]`, regular rows have ≥ 2 nodes; also for delta and
    domain rows).
  - D1 lifetime: **(b) reference-counted rows** (storage maps key → `weak_ptr`; a row dies with the
    last holder); the `shared_ptr`s live **inside `NodeTable`** next to the spans (owner).
    Reasons: no unbounded growth of the global storage; no artificial cross-sample reuse in the
    judged e2e `generation` metric.
  - D2 domain nodes: **(A)** domain rows in the storage, key (unit key, interval bits), mapped with
    `tools::mapToDomain`; the identity domain reuses the unit row (`fma(u, 1, 0) == u`) → WP-58's
    memory gain.
  - D3: the global storage easy to reach for end users (e.g. a static `NodeStorage::global()`);
    `SparseGridSetup::operator==` excludes the storage; **`NodeStorage` hidden from Python** (Python
    grids always use the global storage).
  - D4: owner first considered value objects for node families, then decided to **keep
    pointer-based singletons** (passing objects is awkward with inheritance) and only rename
    `NodeGenFunc` → `NodeFamily`; identity = the singleton pointer (with D1 (b) safe: rows exist only
    while a grid holds them, and a grid's node families must outlive it, so a family address
    cannot be reused while rows keyed by it exist). Getters keep returning pointers;
    `NodeFamily` non-copyable.
  - D5: `NodeStorage` (shared, owns rows) + `NodeTable` (per grid, non-owning index + row handles).
  - Order (owner): **P2** NodeFamily rename → **P3** NodeStorage + non-owning NodeTable → **P4**
    refinement simplification; the owner checks in after P2 before P3 starts.

### P2 — rename `NodeGenFunc` → `NodeFamily` (pointer-based singletons kept)

- **Decisions** (owner): pointers + non-copyable; class names `*Nodes`; files in a top-level
  `node_families/` directory (not under `functions/`).
- **Change**: classes `NodeGenFunc` → `NodeFamily` (copy constructor / assignment deleted),
  `EquidistantNodeGenFunc` → `EquidistantNodes`, `SecondTypeChebyshevNodeGenFunc` →
  `ClenshawCurtisNodes` (+ alias `SecondTypeChebyshevNodes`), `FirstTypeChebyshevNodeGenFunc` →
  `FirstTypeChebyshevNodes`; getters `get{Equidistant,ClenshawCurtis,SecondTypeChebyshev,
  FirstTypeChebyshev}Nodes()`; setup `setNodeFamily(ForDim)`, `setNodeFamilies`,
  `getNodeFamily(ForDim)` / `getNodeFamilies`, `getUniqueNodeFamilies`, member `nodeFamilies`;
  Python `NodeFamilyVector`; namespaces `node_families`, `tools::equidistant_nodes`; variables and
  prose ("node-generation function" → "node family"). Files: `functions/node_generation_functions/**`
  → `node_families/{node_family, equidistant_nodes, clenshaw_curtis_nodes, first_type_chebyshev_nodes}.*`
  + `node_families/getter/*_nodes_getter.*`; `tools/functions/node_generation_functions/
  equidistant_node_generation_function_utilities.*` → `tools/node_families/equidistant_nodes_utilities.*`;
  `tests/node_generation_functions/` → `tests/node_families/{node_families_test, delta_node_families_test}.cpp`
  (Boost suites `NodeFamily_*`). Family ids unchanged (the hashed name strings are kept). clang-format:
  include re-sorting (55 files) + re-alignment of 6 renamed files. 116 files rewritten (not:
  `perf/results`, summaries, PLAN / HISTORY / STATUS, `claude/bachelor-thesis/`). Protected files (6,
  incl. `perf_problems.hpp`'s helper `nodeGenFuncByName` → `nodeFamilyByName`): edited and rehashed by
  the owner. Docs: CLAUDE.md §2 example / §6 gotcha, module-reference (layout entry `node_families/`),
  python-bindings.md.
- **Related findings**: Doxygen still says "node generator(s)" / "node-generation rules" in a few
  briefs; `NodeFamily::getQuadRule` / `getInterpolationMethod` return raw pointers of unclear
  ownership; `TODO: Document` on several `NodeFamily` methods.
- **Tests**: no behavior change; existing suite (incl. `tests/node_families/`). Python:
  `python_checks_point2.py` (old names gone, new names, CC == second-type Chebyshev singleton, setup
  setters / getters, `NodeFamilyVector`) + point-1 checks + 12 examples.
- **Checks**: debug build ✓, debug `test --repeat 3` ✓; Python build ✓: `python_checks_point2.py` ✓,
  `python_checks_point1.py` ✓ (updated to the P2 names), 12 / 12 examples ✓ (WP-89's
  `python_checks.py` uses the old names: historical, not updated); normal build + `test` ✓ (snapshot
  `WP-90-p2`); `refcheck` (`refcheck_p2.txt`): only the known WARN; `perf_reference` at 1 and 16
  threads **bitwise identical** to `WP-90-p1` and `WP-90-before`. Benchmarks: at the end of the WP.
- Note: `list(DataVector)` aborts in Python (no `__iter__`, reads past the end; known, WP-37) — hit
  by a first version of the check script.
- **Status**: done (2026-11-27).

### P5 — remove the FNV-1a id concept where unused (owner's third point; before P3 / P4)

- **Request** (owner 2026-11-27): several concepts use `fnv1aHash` ids (e.g. node families); remove
  the id concept where it is not used / unnecessary.
- **Finding**: three id concepts, none used functionally: `NodeFamily::id()` (only consumed by
  `BarycentricFormula`'s own id; Python `id`), `QuadRule::id()` (only a Fejér test checking that the
  two rules' ids differ; source comment "TODO: Is the id still necessary?"), `InterpolationMethod::id()`
  (only printed in a test context). Equality of node families is `typeid`-based. `fnv_1a_hash.hpp` had
  no other users. → all removed.
- **Decision** (owner): also make `getBarycentricFormula` a single instance — it kept one identical
  `BarycentricFormula` per node family in a mutex-guarded map only because of the family-dependent id
  (the formula derives its weights from the given nodes). Now `getBarycentricFormula()` (no argument)
  returns one static instance, like `getOptBarycentricFormula()`; no lock on the equi / cheb1
  `getInterpolationMethod` path any more (WP-07 had listed that mutex as a cost).
- **Change**: `NodeFamily` (id constructor / `id()` / `id_` removed; default constructor), the three
  families' id constructors removed; `QuadRule` (id constructor / `id()` / `id_` + TODO removed),
  `ClenshawCurtisQuadRule` / `SimpsonQuadRule` / `TrapezoidalQuadRule` constructors removed,
  `FejerFirstQuadRule` keeps its `includeBoundary` constructor; `InterpolationMethod` (id constructor /
  `id()` / `id_` removed), `BarycentricFormula(const NodeFamily*)` / `OptBarycentricFormula()`
  constructors removed; `getBarycentricFormula()` without argument (map, mutex, `<unordered_map>` etc.
  gone), callers in `equidistant_nodes.cpp` / `first_type_chebyshev_nodes.cpp`;
  `tools/hashing/fnv_1a_hash.hpp` deleted (directory gone). Tests: Fejér id check removed; two test
  methods' id constructors removed; `vector_interpolation_test` context prints `typeid` names;
  `barycentric_formula_test` / `vector_interpolation_test` use `getBarycentricFormula()`. Benchmark:
  `perf_micro.cpp` (`micro/interp/barycentric_1d/generic/*`, same meaning). Docs: module-reference
  (tools layout), python-bindings.md (`NodeFamily` without `id`). Python: `id` gone (check in
  `python_checks_point2.py`).
- **Judged code touched** (interpolation-method lookup on the global-interpolation path) → refcheck,
  bitwise diff; e2e at the end of the WP.
- **Checks**: debug build ✓, debug `test --repeat 3` ✓; Python build ✓: point-1 / point-2 checks ✓
  (`id` gone), 12 / 12 examples ✓; normal build + `test` ✓ (snapshot `WP-90-p5`); `refcheck`
  (`refcheck_p5.txt`): only the known WARN; `perf_reference` at 1 and 16 threads **bitwise
  identical** to `WP-90-p2`. Benchmarks (e.g. `micro/interp/barycentric_1d/generic/*`, equi / cheb1
  interpolation e2e): at the end of the WP.
- **Status**: done (2026-11-27).

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
