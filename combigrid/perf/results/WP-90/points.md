# WP-90 points — restructuring of the dimension-adaptive refinement (owner, point by point)

Session start: 2026-11-27 (HEAD 182f9cd30, branch `optimizations`). Resume from §1.

## 1. Current state and next step

- Phase 1 (orientation) done; orientation + candidate points (§3) given to the owner. The owner
  wants session findings that relate to an owner point reported with that point; the remaining
  candidates are reviewed by the owner after their own points.
- P1 done and verified (§2; owner checkpoint commit). P2 done and verified; P5 (ids, owner's
  third point) done and verified (owner checkpoint commit); P3 (NodeStorage) done and verified
  (in the working tree, not yet committed by the owner at the session's end); P6 (simpler
  NodeTable) proposed, **not decided** (owner ended the session for a continuation prompt; the next
  session waits for the owner's plan). P4 open (options in its entry below). Snapshots:
  `WP-90-before`, `WP-90-p1`, `WP-90-p2`, `WP-90-p5`, `WP-90-p3`.
- First session ended 2026-11-27; continuation prompt: `results/WP-90/continuation-prompt.md`. Benchmarks of judged code: one run at the end
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

### P3 — `NodeStorage` + non-owning `NodeTable`

- **Decisions**: as in "P2–P4" above (D1 (b) with the `shared_ptr`s inside `NodeTable`, D2 (A), D3
  global storage via `NodeStorage::global()` + hidden from Python + excluded from `==`, D5 names,
  boundary rows shared).
- **Change**:
  - New `grids/node_row_key.{hpp,cpp}` (`NodeRowKey`: family, growth (delta only), inner count /
    level, interval compared bitwise; `regular`, `delta`, `onInterval`, `isOnUnitInterval`, `hash`,
    `std::hash`) and `grids/node_storage.{hpp,cpp}` (`NodeStorage`: `global()`, `getRows(keys)` /
    `getRow`, `nRows`, `nStoredNodes`; map key → `weak_ptr` + map key → `shared_future` for rows
    being generated; a request claims missing rows under the lock, generates them without it (OpenMP
    `parallel for` dynamic, from `MIN_NODES_FOR_CONCURRENCY` nodes; exceptions caught per row),
    publishes them, fulfils the promises, then waits for rows of other requests (never waits while
    holding rows to generate → no deadlock); unit rows first, then rows on other intervals mapped
    from them with `tools::mapToDomain`; expired entries pruned when the map doubles). A row always
    holds the nodes with boundary (`genNodesWithBoundary(n+2)` / `genDeltaNodesWithBoundary`).
  - `NodeTable`: per row a `std::span<const double>` + `std::shared_ptr<const DataVector>` owner;
    `rowNodes()` → `setRow(row, owner[, span])`, new `rowSpan`, `rowOwner`; copies share rows.
    `TensorGridView` reads the spans (same two loads); `nodeVectorInDim` removed (textbook
    interpolations take spans).
  - `SparseGridSetup`: `std::shared_ptr<NodeStorage>` (forward-declared; default
    `NodeStorage::global()`), `getNodeStorage()` / `setNodeStorage()` (throws on null),
    hand-written `operator==` without the storage.
  - `node_table_generation`: `genNodeTables(setup, levels, deltaNodes)` → (unit, domain) via the
    storage (same row layout as before), `setRows`, `rowKey`, `includesBoundary`; removed
    `genUnitNodeTable`, `mapOntoDomain`, `genUnitRow`, `genDomainRow`; `genEmptyNodeTable` kept.
    `SparseGrid` / `DeltasSparseGrid` constructors use `genNodeTables`.
  - Refinement (minimal, rest in P4): `RefinementNodeStorage` copies row handles (`adopt` from the
    grid's const tables, `ensureRows` → storage via `setRows`, `release` const, `giveBack` and the
    presence flags removed); `SparseGridStorageAccess::{unit,domain}NodeTable` removed;
    `endHandOver(!changed)` leaves the grid as it is.
  - Python: `getNodeStorage` / `setNodeStorage` `%ignore`d; umbrella header includes
    `node_row_key.hpp`, `node_storage.hpp`.
  - Docs: CLAUDE.md §2 (pipeline) + §6 (new gotcha), module-reference (layout, `SparseGrid` storage,
    design rationale), python-bindings.md (not-bound table).
- **Tests**: new `tests/grids/node_storage_test.cpp` (8 cases: regular / delta rows == the families'
  lists with and without boundary bitwise; interval rows == `mapToDomain` bitwise and [0, 1] == the
  unit row; grids share rows, unit-cube domain rows == unit rows, level without boundary views
  `owner + 1`, rows freed with the last grid; `NodeTable` copies keep rows; setup storage default /
  null / `==` ignores it / different storages give equal tables at different addresses; 8 threads
  building SG + DSG concurrently get the same rows; a failing family (cheb1 delta with doubling)
  throws on every request and is not left pending). Adapted: synthetic tables in
  `sparse_grid_storage_test` (+ copy shares rows, partial row view), `quadrature_test`,
  `vector_quadrature_test`. Pinning: existing grid / storage / pinning / operator suites.
- **Checks**: debug build + `test --repeat 3` ✓; ASan ✓ (memory-capped); TSan (`tsan/`, driver: 8
  std::threads × 40 SG + DSG constructions / destructions on one storage + 1 thread with d2 L16
  grids): with OpenMP workers 39 reports, all fork / join pairs of the uninstrumented libgomp (row
  generated by a worker, read after the join + publication under the mutex); with
  `OMP_NUM_THREADS=1` (no workers) **0 reports**, 0 rows left (`tsan/summary.txt`); Python build ✓
  (point-1 / point-2 checks, `NodeStorage` / `get|setNodeStorage` absent, 12 / 12 examples);
  normal build + `test` ✓ (snapshot `WP-90-p3`); `refcheck` (`refcheck_p3.txt`): only the known
  WARN; `perf_reference` at 1 and 16 threads **bitwise identical** to `WP-90-p5`.
- **Exploratory micro A/B** (`explore_p3_micro.md`, `WP-90-p5` vs `WP-90-p3`, 1 thread, generation +
  kernel spots; not the WP's final measurement): kernels unchanged (`quad/process_all_tgs`,
  `interplin/tg`, `tg/iterate` all within noise: `TensorGridView` reads spans, same two loads);
  generation faster where the domain is the unit cube (domain rows = unit rows: no mapping, no second
  allocation): `gen/sg/cc/b0/d2/L20` 137.0 → 90.0 ms (1.52x), `gen/dsg/equi/b0/d3/L16` 1.78 → 0.93 ms
  (1.94x), `gen/dsg/equi/b1/d20/L6` 1.18x, `d15/L7` 1.19x, `gen/sg/cc/b1/d20/L5` 1.06x,
  `gen/sg/equi/b1/d10/L7` 1.05x; one small case SLOWER: `gen/sg/equi/b0/d5/L10` 116 → 121 µs (0.959,
  +5 µs: two storage requests, hash lookups, a `shared_ptr` per row). Each iteration constructs and
  destroys its grid, so rows are regenerated every time (no cross-iteration reuse with D1 (b)).
  Heap held by one grid (`grid_heap_bytes`): sg cc b0 d2 L20 67.2 → 33.6 MB, dsg equi b0 d3 L16
  2.15 → 1.09 MB, sg equi b0 d5 L10 155 → 127 KB; b1 high-d +1.6 KB (boundary nodes kept in every
  row + control blocks). → WP-58 (domain rows share the unit rows) is achieved by P3.
- **Status**: done (2026-11-27); e2e A/B with the rest of the WP at the end.

### P4 — simplification of the refinement's node handling (open)

- After P3 most of the planned machinery is already gone (`moveRows`, `giveBack`, presence flags, the
  grid's node-table accessors); `RefinementNodeStorage` is a thin class (two fixed-capacity,
  non-owning `NodeTable`s per call, `ensureRows` from the storage, `release` = compact copies).
  Candidate C9 resolved by P3.
- Options given to the owner: (a) stop here (optionally rename to `RefinementNodeTables`);
  (b) replace the class by free functions on two engine-owned tables; (c) also drop the call's unit
  table from `RefinementTarget::beginRefinementCall` by indexing 1D weights by storage row (weights as
  companion data of `NodeStorage`; touches judged `QuadWeightTable`; overlaps WP-60). Recommendation
  was (a) + (c) as a separate point — **superseded by the owner's question that led to P6** (with P6,
  `RefinementNodeStorage` disappears entirely).
- **Status**: open; decide after P6.

### P6 — simplify `NodeTable` to a per-dimension map level → storage row (proposed, not decided)

- **Owner's question** (2026-11-27, before touching the refinement): isn't it enough that
  `NodeTable` maps (dim, lvl) to a row of a `NodeStorage`, storing the rows queried and adding new
  ones on request?
- **Answer / proposal** (session): yes in principle — row identity and lifetime live in
  `NodeStorage` since P3; the table only needs per-dimension arrays level → (span, owner). Three
  points shape it:
  1. Rows are added only by an explicit, serial `request(levels)` (grid construction; the
     refinement's management before publishing a task), never as a side effect of reads — the
     operators read the tables from many threads in hot loops (`TensorGridView`: one array index per
     dimension); lazy insertion on read would need locks / atomics there.
  2. Entries must not move while views exist: `reserve(maxLvl)` (the refinement on the grid's own
     tables at the start of a call); requests within the capacity never relocate; `truncate(maxLvls)`
     at the end of a call drops rows of levels no tensor grid uses (unused speculative / pending
     candidates; otherwise the refined grid ≠ the `MIVecSGGenInstr` grid and wasted weight work). Then
     `RefinementNodeStorage`, `adopt` / `release` and the node part of the hand-over disappear (most
     of P4).
  3. `QuadWeightTable` (judged) relies on rows shared between dimensions (one weight slot per table
     row; d2 L20 CC would otherwise compute its 2^20-node weight rows twice, ≈ 67 ms each): it would
     deduplicate by the rule's identity (node family, node count, includes boundary), computed at
     construction (also for rows set later); one extra index load per `weights(dim, lvl)` call (per
     TG and dimension, not per point); rows with equal counts across dimensions / growth functions
     share weights too. DSG indices use their own row structures (unaffected).
- Proposed API: `NodeTable(setup, deltaNodes, onDomain)` (empty; keeps the storage pointer + per-dim
  key parts: family, growth, interval, boundary offset — not a setup copy); `request(LvlMIView)` /
  `request(maxLvls)` (one storage request for all missing rows, all levels up to the requested one);
  `reserve`, `truncate`; getters `maxLvl`, `nNodes`, `nodes`, `nodeSpan`, `row` / `nRows` (entry index
  for `QuadWeightTable`); `operator==` (max levels + nodes bitwise). Removed: `sharesRowsWith`,
  `setRow`, and in `node_table_generation` everything except `compNodeCnt` (`layOutRows`,
  `findDimsWithSameNodes`, `genEmptyNodeTable`, `setRows`, `genNodeTables`, `rowKey`,
  `includesBoundary` move into / behind the table). Expected: results bitwise unchanged, kernels
  unchanged, tables slightly larger (32 B per (dim, level)), generation slightly cheaper (no layout
  pass).
- Alternative offered: same API but keep dimension sharing inside the table (layout logic stays,
  `QuadWeightTable` untouched); or not now.
- **Status**: proposed; the owner has not decided (session ended for a continuation prompt).

### P7 — simplify everything built on the old node handling (owner, 2026-11-27, 2nd session)

- **Request** (owner): WP goal = large-scale simplification after WP-89. After P3 much code still
  follows the old node handling (e.g. `TensorGridView` holds a pointer to `std::span`; `NodeTable`
  should only be an abstraction over `NodeStorage` — query by (dim, level) — that keeps its rows
  alive; rename to something like `SGNodesView`). Analyze all affected code, "don't be too gentle"
  (esp. with new code), rewrite concepts where needed. Supersedes P6 and P4.
- **Session start**: tree = P3 uncommitted (partly staged by the owner; `node_storage.*`,
  `node_table.*`, `sparse_grid_setup.cpp` edited after the last verified P3 build); debug build +
  `test` ✓; `verify` ✓, `envcheck` ✓, `calibrate` 1.005 / 1.000 (e2e), 0.999 / 0.999 (micro).
- **Analysis / proposal** (session, 2026-11-27): see the chat proposal — A (core): `SGNodesView`
  (flat per-dim entries span + row handle, levels 0..maxLvl always set, no dimension sharing, fixed
  capacity + serial `extend`), builds itself from setup + levels; `TensorGridView` = levels +
  view pointer; `SparseGridBase::setStorage(levels, coeffs)` builds its views; refinement: own views
  per call, `RefinementNodeStorage` / adopt / release / node hand-over / `replaceStorage` tables gone;
  `QuadWeightTable(setup, maxLvls)` independent of nodes (dedupe by family, count, boundary);
  `RefinementTarget::beginRefinementCall` without `NodeTable`; `node_table_generation` deleted
  (`compNodeCnt` stays). A2: `NodeStorage` rewritten with once-initialized slots (no pending map /
  promises / futures / generic generators). B (option): one view with unit + domain nodes (public
  API + Python + 2 protected lines). Related finds: R1 `DSGGlobalInterpIndex` copies its own
  regular nodes; R2 `InterpolationMethod::basisValues` takes `std::vector` (per-TG node copy in
  `fillBasisValues`); R3 `nodesInDim` / `nGPInDim` / `nodeSpanInDim` → one span getter.
- **Status**: proposed; awaiting the owner's decisions.

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
