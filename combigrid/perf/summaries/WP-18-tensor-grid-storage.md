# WP-18 — Tensor-grid storage: shared node tables (unit + domain) + struct of arrays

Status: PROPOSED · Date: 2026-11-15 · Base commit: cf698f31d (checkpoint 5447e3c58 = first variant)
Recommendation: accept (the owner applied `results/WP-18/protected-files.patch` to the protected `perf_e2e.cpp` / `perf_reference.cpp` and re-protected them) — memory −19 % geomean (27 MEM−; sg d2 L20 620 → 346 MiB, dsg b1 d20 218 → 83 MiB), generation in high d 5–18× faster, e2e sum 1.005x; refcheck = before (131 OK, the known WARN since WP-06), `nodeChecksum` bitwise equal to the reference.
Machine: `perf.py calibrate` ratios e2e 0.999 / 0.999, micro 0.998 / 0.999 (start); 0.998 (before the final A/Bs, 2026-11-15)

## Change
- **Storage** (`SparseGridBase`, both grids): `LvlMIVec levels_` (moved in from the MI-set generation, owned: never aliases an `MIVecSGGenInstr`'s vector), `std::vector<CTCoeffType> coefficients_`, `NodeTable nodeTable_` (domain) + `unitNodeTable_` (unit interval), cached maxima, `OperatorDataCache` (moved up from the DSG). Was `std::vector<TensorGridCTData>` (3 heap blocks per TG).
- **`NodeTable`** (`grids/node_table.{hpp,cpp}`, new public): one node list per (dim, level 0…maxLvl(dim)), row = `firstRowOfDim[dim] + lvl`, counts = row length. SG rows: regular nodes; DSG rows: delta nodes (nested: each node once). **Two tables per grid, both built at construction** (owner's choice 2026-11-14): `getUnitNodeTable()` (the node-generation functions' nodes; dims with equal `NodeGenFunc*` + `Lvl2GPCntFunc` share rows) and `getDomainNodeTable()` (its `tools::mapToDomain` copy, bitwise the evaluated points; sharing also needs equal domain interval). Each row is its own `DataVector`, allocated and filled by the thread that generates it (`tools::node_table_generation::{genUnitNodeTable, mapOntoDomain}`, replace per-TG generation).
- **Views**: `TensorGridView` (levels + raw row pointers of the table: `nGPInDim`, `nodesInDim`, `getGPCntPerDim()`/`getNodesPerDim()` copies, `getGridPoint`, `toTensorGrid()`), `TensorGridCTDataView {mi, coefficient, tensorGrid}` (+ `toTensorGridCTData()`, `operator->` trick), random-access proxy iterator `misc::TensorGridCTDataIterator`. Grids (owner's choice 2026-11-15: every accessor names its coordinates, **no default iteration**): `getDomainTensorGrid(i | mi)` / `getUnitTensorGrid(i | mi)`, ranges `domainTensorGrids()` / `unitTensorGrids()` (`TensorGridRange` = `std::ranges::subrange` of the proxy iterator; by mi: iterator into the matching range), `getLevels()`, `getCoefficients()`, `getDomainNodeTable()`, `getUnitNodeTable()`. The protected `perf_e2e.cpp` / `perf_reference.cpp` iterated the grid directly: one-line patch each (see Reference check), applied by the owner; otherwise they compile unchanged (proxy keeps `.mi/.coefficient/.tensorGrid.*`).
- **Operators**: kernels take `TensorGridView`; SG quadrature / global interp read nodes from the table (no `mapNodesOntoDomain`, no per-thread `nodesPerDim`: −268 MiB zero fill at d2 L20); DSG kernel reads the table, copies only the row dimension's nodes into a growing scratch (`rowNodes`; reading the table directly made `micro/dsg/row_loop` b0 d7 20 % slower at equal instructions, IPC). Weights / basis values / interpolation cells stay on [0,1]^d: SG global and linear interpolation read the grid's unit table (computing in domain coordinates selected other corners for query points on nodes of shifted domains); `DSGLinInterpIndex` copies (sorted) the DSG unit rows instead of regenerating them. Lin-interp bracket: branchless lower bound, view by const& (by value: 32-byte stack copy, +15 % at d2). `CTCoeffTrie(levels, coeffs)` / `ClosureTrie(levels)` read the contiguous levels; `comp1DWeights`, DSG indices take `maxLvl` from the table (no TG scan).
- Docs: Doxygen of all touched classes, CLAUDE.md §2, `module-reference.md`, `python-bindings.md`. Python mirrors C++: `getDomainTensorGrid(idx | mi)` / `getUnitTensorGrid(idx | mi)` and `getDomainTensorGrids()` / `getUnitTensorGrids()` return owned copies; no `__iter__` / `__getitem__` any more (`len` stays); new `getLevels`, `getDomainNodes(dim, lvl)`, `getUnitNodes(dim, lvl)`, `nNodes`, `maxLvl`.
- Files: new `grids/{node_table,tensor_grid_view}.{hpp,cpp}`, `miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.{hpp,cpp}`, `miscellaneous/iterators/tensor_grid_ct_data_iterator.hpp`, `tools/sparse_grid_generation/node_table_generation.{hpp,cpp}`; deleted `tools/sparse_grid_generation/sparse_grid_generation.{hpp,cpp}`, `tests/grids/sparse_grid_base_test.cpp`; 72 tracked files changed (grids, generation, all operator tools/scratches/contexts, tries, DSG indices, `constants.hpp`, `type_defs.hpp` (`LvlMIView`), umbrella header, `combigrid.i`, tests, `perf_micro.cpp`, `perf_interp_sol.hpp`, `perf_interp_explore.cpp`). Deleted / proposed-for-deletion list and design decisions: `results/WP-18/design-and-obsolete-code.md`.

## Gain vs. cost
- Gain: e2e sum 38.519 → 38.338 s (1.005x, −182 ms; within noise as a whole). Faster: `quad/sg/equi-exp/b0/d2/L20` 1.227x (−89 ms), `quad/sg/cc-uq/b0/d2/L20` 1.096x (−96 ms), `quad/dsg/equi-exp/b1/d20/L6` 1.067x (−73 ms), `b1/d15/L7` 1.051x (−61 ms), `quad/sg/cc-uq/b0/d3/L14` 1.034, `interplin/dsg/equi/b1/d2` 1.026; `quad/sg/equi-exp/b1/d10–d20` 1.016–1.048 (~). Generation: d20 dsg 61.0 → 11.5 ms, d15 dsg 44.6 → 4.5, sg b1 d20 19.5 → 8.6, sg b1 d15 16.3 → 6.7, sg cc d2 L20 87.0 → 62.3, dsg b0 d7 3.2 → 0.18; **slower** sg equi d2 L20 30.5 → 36.5 ms (0.839: second table, see below). Slower totals: `quad/dsg/equi-exp/b0/d10/L7` 0.986 (+81 ms), `b0/d7/L10` 0.985 (+19 ms; operator 0.979–0.984, generation faster), `quad/sg/cc-uq/b0/d7/L7` 0.991 (+7 ms); `interp/dsg/cc/b0/d10` 0.980 (~) — DSG kernel unchanged by the two tables and the accessors; placement (the same cases were 0.975–0.995 in both earlier variants).
- Memory (main goal): geomean B/A 0.810, 27 MEM−, 1 MEM+. sg equi d2 L20 620 → 346 MiB, sg cc-uq d2 L20 1066 → 873, dsg b1 d20 218 → 83, dsg b1 d15 153 → 52, sg b1 d20 59 → 20, sg b1 d15 54 → 18, dsg b0 d3–d10 −30 %. MEM+: `interplin/dsg/equi/b1/d2/L16` 20.8 → 21.7 MiB (+0.9 MiB: the eager DSG unit table, 2^17 nodes, besides the index's sorted copy; 1.029 ~ in the two-table run). `interplin/sg/equi/b1/d2/L16` 19.9 → 20.1 (~; MEM+ 22.6 with the lazy unit table).
- **Explicit domain / unit accessors** (vs. snapshot `WP-18-twotables`, same code before the rename; `e2e-accessors.md`, 26 interp cases): neutral — sum 10.082 → 9.908 s (1.018x, ~), 1 case outside noise (1.005x), memory geomean 1.003; 1-thread `perf_reference` output bitwise identical.
- **Eager unit table vs. the first variant** (lazy `UnitNodeTable` operator data, snapshot `WP-18-lazyunit`; `e2e-unit-table.md`, 34 d2-L16/20 + interp cases): totals all within noise (sum 11.270 → 11.334 s, 0.994x); generation +19.6 ms (sg equi d2 L20), +12 ms (sg cc d2 L20), +0.1–1 ms in the d2 L12/L16 interp cases (second parallel region); peak memory geomean 0.994 (interplin sg d2 L16 MEM− 22.5 → 20.4 MiB). Standalone (`lazy-vs-eager.txt`, 16 threads): d2 L20 unit table 6.7 ms (equi) / 41 ms (cc), domain copy 8 ms and 32 MiB; high d < 0.01 ms each.
- API / functionality (agreed 2026-11-13): grids store domain nodes (`getNodesPerDim` etc. return domain coordinates); `getTensorGrid` / default iteration replaced by the explicit domain / unit accessors and ranges, which yield views instead of `const TensorGridCTData&`; removed `getTensorGrids()` (span), mutable/reverse iterators, protected hand-assembly methods. Operator results: 1 thread raw `perf_reference` differs only at round-off in 12/132 cases (≤ 6.9e-14 relative; code generation, nodes and evaluated points pinned bitwise; vs. the first variant 7 cases ≤ 1.1e-15, allocator-independent); `nodeChecksum` (patched reference: unit nodes) bitwise equal in all 132 cases.
- Maintainability: per-TG storage, its generation and meta bookkeeping removed (−2098 / +1331 lines in existing files, +~1100 in new files incl. tests/docs). New concepts: two node tables, views. The unit table removed the lazy operator-data table (`unit_node_table.hpp`, `getUnitNodeTable(sg)`), the node generation of `DSGLinInterpIndex` and of the benchmark helper `unitTGData`.

## Tests
- Pinning (passed 3× on unchanged code): new `grid_storage_pinning` (252 grid/config combos: hash of every evaluated point of all operators SG + DSG, scalar + vector; bitwise results of SG quadrature and all DSG operators (debug + optimized golden); SG interpolation ±1e-12); existing grid, operator, interpolation-pinning, vector, DSG, caching suites.
- New: `sparse_grid_storage` (both tables: layout/sharing incl. mixed families, boundary offsets, unequal intervals (shared on the unit interval only), max-level sharing, unit and domain groups with different deepest dimensions; bitwise unit nodes = node-generation function, domain nodes = `mapToDomain`(unit); views vs. copies; iterator; equality; DSG delta counts), `Nodes_Mapped_Onto_Domain` (SG + DSG). Adapted (domain nodes): construction, DSG, quadrature, closure-trie, coefficient-trie, kernel-pinning tests.
- Results: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ (memory-capped) · Python: `SG_PYTHON=1` build ✓, 11 examples ✓, `results/WP-18/python_check.py` ✓.
- Final state (domain / unit accessors, no default iteration, patched protected files): debug `--repeat 3` ✓ · ASan ✓ · normal `--repeat 3` ✓, incl. new `sparse_grid_storage/Domain_And_Unit_Ranges` (both ranges, lookup by mi, unit vs. domain nodes, std::ranges algorithms); Python build ✓, `python_check.py` (both variants, no default iteration) ✓, 11 examples ✓.

## Reference check
- `perf.py refcheck` (final, 1 and 16 threads): 131 OK, 1 WARN — `interp/sg/equi/b0/d2/L5/cos-shifted` x2.34, the known WARN recorded since WP-06 (= before). `nodeChecksum` bitwise equal to the reference in all cases.
- Protected files: `results/WP-18/protected-files.patch` (one line each, applied by the owner): `perf_e2e.cpp` iterates `grid.domainTensorGrids()`; `perf_reference.cpp` iterates `grid.unitTensorGrids()`, so its `nodeChecksum` is taken on the unit nodes, bitwise the nodes the reference was recorded with (replaces the earlier inverse-mapping draft). Without it (domain nodes): 106 `nodeChecksum` FAILs of the shifted-domain cases.

## Benchmarks (A = WP-18-before, B = WP-18-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, 98 rows incl. 6 new `micro/gen/{sg,dsg}/…`; 8 rows SLOWER, all 0.984–0.995; sum 3.895 → 3.013 s, 1.293x):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| gen/dsg/equi/b1/d20/L6 | 288.0 ms | 16.9 ms | 17.09 | [16.85, 17.35] |
| gen/sparse_grid/cc/b1/d20/L5 | 91.6 ms | 11.6 ms | 7.886 | [7.812, 7.960] |
| gen/sg/cc/b0/d2/L20 | 306.9 ms | 139.1 ms | 2.205 | [2.190, 2.219] (first variant: 102.9 ms, 2.985) |
| gen/dsg/equi/b0/d3/L16 | 7.46 ms | 1.81 ms | 4.128 | [4.093, 4.163] (first variant: 0.93 ms) |
| dsg/prepare/equi/b1/d20/L6 | 113.0 ms | 98.2 ms | 1.148 | [1.135, 1.160] |
| interp/global_tg/cc/b0/d2/L12 | 1.01 ms | 0.920 ms | 1.096 | [1.089, 1.103] |
| interplin/sg/equi/b1/d2/L16 | 8.41 µs | 8.55 µs | 0.986 | [0.976, 0.996] |
| interplin/dsg/equi/b1/d10/L5 | 140.6 ms | 143.0 ms | 0.985 | [0.974, 0.996] |
| dsg/process_delta_tgs/equi/b0/d10/L7/s256 | 223.0 ms | 227.8 ms | 0.985 | [0.968, 1.003] |
| dsg/row_loop/equi/b0/d7/L10/s64 | 86.0 ms | 94.6 ms | 0.894 | [0.789, 1.013] (~, bimodal case) |

Heap held by one grid (`grid_heap_bytes`, before): sg cc d2 L20 100.7 MB, dsg b1 d20 L6 136.5 MB; after: tables ≤ 2 × 32 MiB + levels (d20: 18.4 MB) — see e2e RSS.

L2/L3 e2e (16 threads, full suite 38 cases): see Gain; all rows in `results/WP-18/e2e.md`.
Sum of medians: A = 38.519 s, B = 38.338 s, saved = 0.182 s · Peak memory geomean B/A = 0.810 · floors 10.7 / 10.5 MiB. First variant vs. eager unit table: `results/WP-18/e2e-unit-table.md`; before vs. after the accessor rename: `e2e-accessors.md`.

## Attempts not kept
- Domain-coordinate SG interpolation (no unit table): corners differ for points on nodes of shifted domains (`dsg_linear_interpolation/Each_Distinct_Corner…` failed) → unit table.
- First variant: unit table lazily as operator data of the SG interpolations (snapshot `WP-18-lazyunit`, the checkpoint commit) → replaced by the eager unit table (owner's choice; numbers in Gain).
- Flat node buffer: serial zero fill, 16-thread generation sg d2 L20 shifted 27 → 81 ms → per-row `DataVector`s (22 ms).
- View with `const NodeTable*`: 4 dependent loads per node list → raw row pointers. Views by value in the lin-interp kernel (+15 % at d2) → const&; by-value kept elsewhere (no difference).
- DSG kernel reading all nodes from the table (row_loop b0 d7 0.75, e2e dsg b0 −5 %); copying all projected dimensions = no better than the row dimension only.

## Profile shift / follow-ups
- WP-05: weights keyed by the table row (or (QuadRule*, n)) would remove the remaining per-thread `weights1D` zero fill (sg d2 L20 still 346 MiB, mostly 16 × 16.8 MiB).
- Remaining ~1–2 % on DSG b0 / interp-dsg and SG interplin d2: placement-sensitive, low priority.
- d2 L20 generation (+8–20 ms, +32 MiB for the domain copy): on [0,1] the domain rows equal the unit rows bitwise and could alias them; generating each domain row right after its unit row (one parallel region, cache-hot) would save the second fork/join (+0.1 ms in the small interp cases). Both not done (special case / small gain).
- `DSGGlobalInterpIndex` still generates the regular SG nodes of every level (not in the DSG tables).
- Delete dead code (ranked list in `design-and-obsolete-code.md`: node-lookup stub, extensive SG, `getQuadRulesForAllDims`, `TensorGrid::getNeighborsForLinInterpolation`).
- Adaptive CT: rows are independent `DataVector`s (adding levels appends rows), but views hold raw pointers into the row vector → growth needs reserved/stable storage and an incremental `setStorage`; DSG operator data needs invalidation (details in the design note).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
