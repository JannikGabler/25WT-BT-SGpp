# WP-90 — Dimension-adaptive CT: restructuring of WP-89's implementation, point by point with the owner

Status: (in progress) · Date: 2026-11-27 · Base commit: 182f9cd30
Recommendation: (at the end)
Machine: `perf.py calibrate` ratios e2e 1.005 / 1.000, micro 0.998 / 0.998 (session start)

## Change
Per point (details: `results/WP-90/points.md`):
- **P1** (owner): `SGGenInstr::clone()` removed (obsolete since `SparseGridSetup`); files renamed
  `sg_gen_instruction.hpp` → `sparse_grid_generation_instruction.hpp`, `complete_sg_gen_instruction.*` →
  `complete_sparse_grid_generation_instruction.*`, `multiindex_vector_sg_gen_instruction.*` →
  `multiindex_vector_sparse_grid_generation_instruction.*` (owner: all three); Python: `clone` and the
  `%shared_ptr` wrapping of `SparseGridSetup` / the instructions removed (owner), `getSetup` returns a copy by
  value. Protected include lines (8 files) changed and rehashed by the owner.
- Files: (at the end)

## Gain vs. cost
- (at the end)

## Tests
- (at the end)

## Reference check
- (at the end)

## Benchmarks (A = WP-90-before, B = WP-90-after; speedup = A/B, > 1 is faster)
(at the end)

## Attempts not kept
- (at the end)

## Profile shift / follow-ups
- **Code hygiene (for the master's hygiene backlog; found with P1, owner: note only)**:
  - F1: `complete_sparse_grid_generation_instruction.{hpp,cpp}` and
    `multiindex_vector_sparse_grid_generation_instruction.hpp` lack the copyright banner; both headers use
    `#ifndef` guards with stale names (`COMBIGRID_FULL_SPARSE_GRID_GENERATION_INSTRUCTION_HPP`,
    `COMBIGRID_MISETSGGENINSTR_HPP`) instead of `#pragma once`; `complete_…instruction.cpp` lacks the final
    newline (cpplint warnings).
  - F2: `TODO: Document` on the generation methods of `SGGenInstr` (`genFullMIVec`, `genReducedMIVec`,
    `…WithCoeffs`).
  - F3: fully commented-out dead files `grids/extensive_sparse_grid.{hpp,cpp}`,
    `tools/sparse_grid_generation/sparse_grid_generation_node_lookup.{hpp,cpp}` and
    `tests/tools/sparse_grid/sparse_grid_generation_node_lookup_test.cpp` contain commented-out includes of
    the renamed header (renamed with P1; the lines now exceed 100 columns → cpplint / clang-format warnings
    in dead code); `extensive_sparse_grid.cpp` lacks the final newline. Candidates for deletion.

## Revert
(at the end)
