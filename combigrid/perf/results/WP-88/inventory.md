# WP-88 — inventory of ids and rename candidates, owner's answers (2026-11-28)

## Context found at session start
- The owner's branch `optimizations-dim-adap-ref-snapshot` (on top of HEAD 765738082 + WP-89 draft)
  already contains a **WP-90** that made the same renames (checkpoints 48071b669, 83e69c8ac, f248ee8f5).
  Its rename map was proposed as the default; the owner accepted it with one change (CC class name, below).
  The stale build artifacts in `src/sgpp/combigrid/node_families/` (`*.os`, `*.lint`) stem from that branch.
- Task 1 names `sg_gen_instr.{hpp,cpp}`; the actual files are `sg_gen_instruction.{hpp,cpp}`.

## Ids (task 3)
All three are FNV-1a hashes of a name (`tools/hashing/fnv_1a_hash.hpp`, the only file in `tools/hashing/`):

| Id | Set in | Uses (whole repo) |
|---|---|---|
| `NodeGenFunc::id()` / `id_` (ctor arg) | ctors of `EquidistantNodeGenFunc`, `FirstTypeChebyshevNodeGenFunc`, `SecondTypeChebyshevNodeGenFunc` | only `BarycentricFormula`'s ctor (combined into its own id); Python: auto-wrapped `NodeGenFunc.id()` (no Python user) |
| `InterpolationMethod::id()` / `id_` | `BarycentricFormula` (hash ⊕ node-family id), `OptBarycentricFormula`, test subclasses (`LagrangeMethod(42)`, `LinearCombinationMethod(12345)`) | only a `BOOST_TEST_CONTEXT` print in `vector_interpolation_test.cpp` |
| `QuadRule::id()` / `id_` (+ "TODO: Is the id still necessary?") | `ClenshawCurtisQuadRule`, `SimpsonQuadRule`, `TrapezoidalQuadRule`, `FejerFirstQuadRule(includeBoundary)` | only `BOOST_CHECK_NE(WITH_BOUNDARY.id(), WITHOUT_BOUNDARY.id())` in `fejer_first_quadrature_rule_test.cpp` |

Not id-based (checked): node-family equality (`operator==` = `typeid`), node-table row sharing
(`node_table_generation.cpp`: pointer equality of `getNodeGenFuncForDim` + same `Lvl2GPCntFunc`),
cross-call operator data cache keys (`tools::cross_call`), source-function cache keys (domain bit
patterns), SWIG typemaps. Commented-out `// using NodeGenFuncId = uint64_t;` in `type_defs.hpp`.
→ Removing the ids changes no observable behavior except the Python method `NodeFamily.id()`.

Barycentric machinery: `BarycentricFormula` stores nothing but the id; `getBarycentricFormula(nodeGenFunc)`
keeps a mutex-guarded `unordered_map<const NodeGenFunc*, unique_ptr<…>>` only to give each family its own
(otherwise identical) instance; it is called once per tensor grid and dimension (SG `interpolate` setup)
and per row (DSG index). `OptBarycentricFormula` (CC) has no other id-only state.

## Rename candidates beyond tasks 1–2 and the owner's answers
| # | Candidate | Owner |
|---|---|---|
| Q1 | Full WP-90 map: `setNodeGenFunc`/`ForDim` → `setNodeFamily`/`ForDim`, `getNodeGenFuncForDim` → `getNodeFamilyForDim`, `get/setNodeGenFuncs` → `get/setNodeFamilies`, `getUniqueNodeGenFuncs` → `getUniqueNodeFamilies`, member `nodeGenFuncs` → `nodeFamilies`, Python `NodeGenFuncVector` → `NodeFamilyVector`, locals / params `nodeGenFunc` → `nodeFamily` (`equidistantNodeGenFunc` → `equidistantNodes`, …), `nodeGenFuncByName` → `nodeFamilyByName` (incl. protected `perf_problems.hpp`), `NoRuleNodeGenFunc` → `NoRuleNodeFamily`, test suites `NodeGenFunc_*` → `NodeFamily_*`, namespace `node_gen_funcs` → `node_families`, `tools::equidistant_node_gen_function` → `tools::equidistant_nodes`, Doxygen "node-generation function" → "node family", directories `functions/node_generation_functions/` → `node_families/` (files `node_family`, `<family>_nodes`, `getter/<family>_nodes_getter`), `tools/functions/node_generation_functions/` → `tools/node_families/`, `tests/node_generation_functions/` → `tests/node_families/` | **Full WP-90 map** |
| Q2 | CC / second-kind class name (WP-90: `ClenshawCurtisNodes` + alias `SecondTypeChebyshevNodes`) | **`SecondTypeChebyshevNodes`**, file `second_type_chebyshev_nodes.hpp`, no alias (getters `getClenshawCurtisNodes()` / `getSecondTypeChebyshevNodes()`, same singleton) |
| Q3 | Also `complete_sg_gen_instruction.*`, `multiindex_vector_sg_gen_instruction.*` (WP-90: all three); optionally `constants::sg_gen_instr`, test case `multiindex_vector_sg_gen_instr` | **All three files**; constants namespace / test case name stay |
| Q4 | Remove all three ids + getters (incl. Python `id`), delete `fnv_1a_hash.hpp`, stateless `BarycentricFormula`, `getBarycentricFormula()` = one static instance without parameter | **Yes, as proposed** |

Not proposed / unchanged: class names `SGGenInstr`, `CompleteSGGenInstr`, `MIVecSGGenInstr`;
`Lvl2GPCntFunc`, `NodeIdxLvlMapFunc`; section headings "Node generation" (they name the operation);
`NodeGenExpr*` in `tools/PythonTidy.py` (unrelated: Python generator expressions).
