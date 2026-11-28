# WP-88 — rename map (old → new)

Paths below `combigrid/src/sgpp/combigrid/` unless stated. Produced by `rename.py` (same directory;
`moves` = file moves, `SUBS` = ordered text substitutions) plus the hand edits listed at the end.
No deprecated aliases.

## Files / directories
| Old | New |
|---|---|
| `sparse_grid_generation_instructions/sg_gen_instruction.{hpp,cpp}` | `sparse_grid_generation_instructions/sparse_grid_generation_instruction.{hpp,cpp}` |
| `sparse_grid_generation_instructions/complete_sg_gen_instruction.{hpp,cpp}` | `…/complete_sparse_grid_generation_instruction.{hpp,cpp}` |
| `sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.{hpp,cpp}` | `…/multiindex_vector_sparse_grid_generation_instruction.{hpp,cpp}` |
| `functions/node_generation_functions/` (directory) | `node_families/` (top level; no `default_functions/` level) |
| `functions/node_generation_functions/node_generation_function.{hpp,cpp}` | `node_families/node_family.{hpp,cpp}` |
| `…/default_functions/equidistant_node_generation_function.{hpp,cpp}` | `node_families/equidistant_nodes.{hpp,cpp}` |
| `…/default_functions/first_type_chebyshev_node_generation_function.{hpp,cpp}` | `node_families/first_type_chebyshev_nodes.{hpp,cpp}` |
| `…/default_functions/second_type_chebyshev_node_generation_function.{hpp,cpp}` | `node_families/second_type_chebyshev_nodes.{hpp,cpp}` |
| `…/getter/<f>_node_generation_function_getter.{hpp,cpp}` (f = clenshaw_curtis, equidistant, first_type_chebyshev, second_type_chebyshev) | `node_families/getter/<f>_nodes_getter.{hpp,cpp}` |
| `tools/functions/node_generation_functions/equidistant_node_generation_function_utilities.{hpp,cpp}` | `tools/node_families/equidistant_nodes_utilities.{hpp,cpp}` |
| `tools/hashing/fnv_1a_hash.hpp` | removed (only user: the ids) |
| `combigrid/tests/node_generation_functions/node_generation_functions_test.cpp` | `combigrid/tests/node_families/node_families_test.cpp` |
| `combigrid/tests/node_generation_functions/delta_node_generation_functions_test.cpp` | `combigrid/tests/node_families/delta_node_families_test.cpp` |
| — | new: `combigrid/tests/node_families/node_family_singletons_test.cpp` |

## Types, namespaces, functions (C++ and Python, same names)
| Old | New |
|---|---|
| `NodeGenFunc` | `NodeFamily` |
| `node_gen_funcs::EquidistantNodeGenFunc` | `node_families::EquidistantNodes` |
| `node_gen_funcs::FirstTypeChebyshevNodeGenFunc` | `node_families::FirstTypeChebyshevNodes` |
| `node_gen_funcs::SecondTypeChebyshevNodeGenFunc` | `node_families::SecondTypeChebyshevNodes` (owner: no `ClenshawCurtisNodes` alias) |
| `tools::equidistant_node_gen_function` (namespace) | `tools::equidistant_nodes` |
| `getEquidistantNodeGenFunc()` | `getEquidistantNodes()` |
| `getClenshawCurtisNodeGenFunc()` | `getClenshawCurtisNodes()` |
| `getFirstTypeChebyshevNodeGenFunc()` | `getFirstTypeChebyshevNodes()` |
| `getSecondTypeChebyshevNodeGenFunc()` | `getSecondTypeChebyshevNodes()` (same singleton as CC, unchanged) |
| `SGGenInstr::setNodeGenFunc` | `setNodeFamily` |
| `SGGenInstr::setNodeGenFuncForDim` / `getNodeGenFuncForDim` | `setNodeFamilyForDim` / `getNodeFamilyForDim` |
| `SGGenInstr::setNodeGenFuncs` / `getNodeGenFuncs` | `setNodeFamilies` / `getNodeFamilies` |
| `SGGenInstr::getUniqueNodeGenFuncs` | `getUniqueNodeFamilies` |
| member `nodeGenFuncs` | `nodeFamilies` |
| Python `NodeGenFuncVector` | `NodeFamilyVector` |
| `NodeGenFunc::id()`, `InterpolationMethod::id()`, `QuadRule::id()` + their id constructors | removed (Python: `NodeFamily.id` gone) |
| `getBarycentricFormula(const NodeGenFunc*)` | `getBarycentricFormula()` (one static instance, no map / mutex) |
| `BarycentricFormula(const NodeGenFunc*)`, `OptBarycentricFormula()` ctor | implicit default constructors |
| `// using NodeGenFuncId = uint64_t;` (`type_defs.hpp`, commented out) | removed |

## Locals, parameters, test / benchmark helpers
`nodeGenFunc` → `nodeFamily`, `nodeGenFuncs` → `nodeFamilies`, `uniqueNodeGenFuncs` → `uniqueNodeFamilies`,
`nestedNodeGenFuncs` → `nestedNodeFamilies`, `equidistantNodeGenFunc` / `clenshawCurtisNodeGenFunc` /
`firstTypeChebyshevNodeGenFunc` / `secondTypeChebyshevNodeGenFunc` → `equidistantNodes` / `clenshawCurtisNodes` /
`firstTypeChebyshevNodes` / `secondTypeChebyshevNodes`, `nodeGenFuncByName` → `nodeFamilyByName`
(incl. protected `perf_problems.hpp`), test helper `NoRuleNodeGenFunc` → `NoRuleNodeFamily`, Boost suites
`NodeGenFunc_{delta_node_count,delta_nodes,nestedness,node_index_maps}` → `NodeFamily_…`.
Prose: "node-generation function(s)" / "node generation function(s)" / "node-generation rule(s)" →
"node family / families" (Doxygen, comments, docs). Section headings "Node generation" (the operation) kept.

## Unchanged on purpose (owner)
Class names `SGGenInstr`, `CompleteSGGenInstr`, `MIVecSGGenInstr`; `constants::sg_gen_instr`; test case
`quadrature_test/multiindex_vector_sg_gen_instr`.

## Protected files (patch `protected.patch`, applied + rehashed by the owner in-session)
`perf_problems.hpp`, `perf_e2e.cpp`, `perf_reference.cpp`, `bachelor_thesis/comparison/{sin_example,uq_electron_beam}_template.cpp`,
`dsg_quadrature/template_complete_dsg_quadrature.hpp`, `multiindex_vector/multiindex_vector_benchmark.cpp`,
`quadrature/template_complete_sg_quadrature.hpp`: include paths, `nodeFamilyByName`, `get*Nodes()`, `setNodeFamily`
(31 lines; no other change; include order left as is).
