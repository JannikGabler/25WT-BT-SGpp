# WP-88 — Preparation for dimension-adaptive refinement: renames + removal of the unused ids

Status: PROPOSED · Date: 2026-11-28 · Base commit: 765738082
Recommendation: accept. The refactoring is as agreed (renames + ids removed). Results are bitwise unchanged, e2e is neutral, and as a by-product the SG interpolation with equi / cheb1 nodes is faster.
Machine: `perf.py calibrate` ratios before measuring e2e 1.009 / 1.000 (burst / sustained), micro 0.995 / 0.999 (at session start the burst ratio was 1.194 because of a transient foreign load of 2.0 %; sustained 1.000)

## Change
- Renames (full map: `results/WP-88/rename-map.md`, reproducible via `results/WP-88/rename.py`):
  - `sg_gen_instruction` / `complete_sg_gen_instruction` / `multiindex_vector_sg_gen_instruction` → `…sparse_grid_generation_instruction`.
  - `NodeGenFunc` → `NodeFamily`. Classes `EquidistantNodes`, `FirstTypeChebyshevNodes`, `SecondTypeChebyshevNodes` in namespace `node_families`. Getters `get*Nodes()`.
  - Generation instructions: `setNodeFamily(ForDim)`, `getNodeFamilyForDim`, `get/setNodeFamilies`, `getUniqueNodeFamilies`. Python `NodeFamilyVector`.
  - Files moved to the top-level `node_families/` (+ `getter/`), `tools/node_families/` and `tests/node_families/`.
  - Locals, helpers, test suites and Doxygen text renamed as well. No aliases.
- Ids removed: `NodeFamily::id`, `InterpolationMethod::id`, `QuadRule::id` (FNV-1a hashes of names; inventory: `results/WP-88/inventory.md`), plus their constructors and `tools/hashing/fnv_1a_hash.hpp`. No observable use: equality is `typeid`, node-table row sharing compares pointers, cross-call / source-function cache keys never used them.
- Barycentric machinery: `BarycentricFormula` is stateless. `getBarycentricFormula()` (no parameter) returns one static instance instead of a mutex-guarded `unordered_map<const NodeGenFunc*, unique_ptr>` with one identical instance per family. Hot loops untouched. The SG `interpolate` asks for the method per point × TG × dim inside the parallel TG loop, so dropping the mutex helps there (below).
- Owner decisions in-session (2026-11-28; list and answers in `inventory.md`):
  - Q1: full WP-90 map (as on branch `optimizations-dim-adap-ref-snapshot`).
  - Q2: CC class `SecondTypeChebyshevNodes`, no `ClenshawCurtisNodes` alias.
  - Q3: all three generation-instruction files; `constants::sg_gen_instr` / class names stay.
  - Q4: ids + hashing removed, parameterless getter.
  - Protected patch (`results/WP-88/protected.patch`, 8 files, 31 lines) applied + rehashed by the owner.
- Files: 23 renamed, 4 getter `.cpp` re-created as new files (git pairs them as delete + add), `fnv_1a_hash.hpp` deleted, 141 modified (src, tests, examples C++ / Python, benchmarks, `Functions.i` / `combigrid.i`, umbrella header, CLAUDE.md, `claude/module-reference.md`, `claude/python-bindings.md`). New: `tests/node_families/node_family_singletons_test.cpp`. `perf_micro.cpp`: only the `getBarycentricFormula()` call; case names unchanged.

## Gain vs. cost
- Gain: none intended. By-product (re-measured with 8 rounds), from the mutex leaving the per-point path:
  - `interp/sg/equi/b0/d3/L5/p64`: 1.116x (8.33 → 7.48 ms).
  - `interp/sg/cheb1/b0/d3/L6/p64`: 1.030x (22.88 → 22.20 ms).
  - cheb1 d5: 1.006 (noise). CC and DSG: unchanged (they never used the map / they look up once per row).
  - Absolute: ≈ 2 ms, i.e. minimal.
- Suite: 35.954 → 35.962 s (1.000x). `expensive/`: 15.681 → 15.679 s (1.000x). Cached: 1.001 → 1.003 s (0.998x, noise).
- Memory: geomean B/A 1.004, 0 MEM flags, largest `quad/dsg/cc-uq/b0/d7/L10` 28.4 → 29.4 MiB (1.034, inside spread).
- API / functionality: agreed renames (C++ + Python, incl. `NodeFamily.id` gone); `getBarycentricFormula()` without parameter; equi and cheb1 now share one formula instance.
- Maintainability: −1 header, −1 mutex/map, −3 id members with 9 hash-constructors; consistent naming.

## Tests
- Pinning (passed unchanged, debug 3/3): the whole suite, in particular `NodeGenFunc_*` (→ `NodeFamily_*`), `op_gen*Nodes`, barycentric / optimized barycentric formula tests, `vector_interpolation_test`, `basis_values_test`, `fejer_first_quadrature_rule`, interpolation pinning tests.
- Adjusted (not weakened):
  - Fejér `small_rules`: the id inequality → the rules' weights differ (n = 3).
  - `Built_In_Overrides_Equal_Default`: the context prints the family name instead of the id.
  - The test methods `LagrangeMethod` / `LinearCombinationMethod` lose their id ctor argument.
- New: `NodeFamily_singletons` (stable singletons, CC = second-kind, family equality; `getInterpolationMethod(n)` = the shared generic formula for equi / cheb1 and every n, the CC formula for CC; concurrent lookup from 8 OpenMP threads).
- Results: debug 3/3 ✓ · normal 3/3 ✓ · ASan ✓ (memory-capped) · without Eigen / Armadillo (OPT=0, `SG_PYTHON=1`) ✓.
- Python: 12/12 `combigrid/examples/python` ✓ + `results/WP-88/python_checks.py` ✓ (new names present, old names and `id` absent).

## Reference check
- `perf.py refcheck`: WARN, identical before and after. `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34, Lebesgue ~1e16; the owner keeps the tolerance) already appears on `WP-88-before`.
- `perf_reference` outputs before / after: bitwise identical, 146 cases at 1 and 16 threads (`results/WP-88/reference-diff.txt`).

## Benchmarks (A = WP-88-before, B = WP-88-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread): all 9 cases within noise; sum 41.42 → 41.34 ms (1.002x).

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/interp/barycentric_1d/generic/n65 | 37.34us | 37.33us | 1.000 | [0.991, 1.009] |
| micro/interp/barycentric_1d/opt_cc/n65 | 196.3ns | 196.0ns | 1.002 | [0.998, 1.006] |
| micro/interp/global_tg/cc/b0/d10/L2 | 39.24ms | 39.17ms | 1.002 | [0.997, 1.008] |
| micro/interp/global_tg/cheb1/b0/d5/L4 | 243.15us | 242.57us | 1.002 | [0.981, 1.023] |

L2/L3 e2e (16 threads, full suite `.`, 5 rounds). 3 flags, re-measured with 8 rounds (`e2e-recheck.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interp/sg/equi/b0/d3/L5/p64 (8 rounds) | total | 8.33ms | 7.48ms | 1.116 | [1.072, 1.162] |
| interp/sg/cheb1/b0/d3/L6/p64 (8 rounds) | total | 22.88ms | 22.20ms | 1.030 | [1.023, 1.038] |
| quad/dsg/equi-exp/b0/d5/L12 (8 rounds) | total | 197.49ms | 198.62ms | 0.996 | [0.992, 0.999] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.376s | 5.392s | 0.998 | [0.992, 1.004] |
| quad/sg/equi-exp/b1/d20/L5 | total | 212.62ms | 212.70ms | 1.003 | [0.990, 1.016] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.298s | 1.297s | 0.997 | [0.985, 1.008] |

- `quad/dsg/equi-exp/b0/d5/L12` SLOWER (0.4 %): code placement (`placement-check.md`). Same instruction count, op-cache misses ×1.96. A hybrid snapshot is impossible because the renamed symbols do not link across snapshots.
- `expensive/…/c100us-cached` generation 2.04 → 3.76 ms (5 rounds): not reproduced (8 rounds: 2.13 → 1.99 ms, noise).
- Sum of medians (total): suite A = 35.954 s, B = 35.962 s (1.000x); `expensive/` 15.681 → 15.679 s · Reports: `combigrid/perf/results/WP-88/`

Peak memory: geomean B/A = 1.004 · 0 MEM+/MEM- · process floor A = 10.1 MiB, B = 10.3 MiB.

## Attempts not kept
- none

## Profile shift / follow-ups
- Context: the owner's snapshot branch already had WP-90 P1/P2/P5 with nearly the same map. WP-88 follows it except for `SecondTypeChebyshevNodes` (no CC alias) and keeps `SGGenInstr::clone()` (WP-90 removed it).
- WP-07 (weights cache per (family, n)): the generic formula no longer knows its family. Key the cache by the shared node row (`NodeTable` rows are shared per family and n) or pass the family in.
- Dimension-adaptive (WP-62 plan): this WP helps. Family identity is the singleton pointer, which is stable within a process. A persistent resume (C2) that serializes the setup needs a stable family name. A `name()` / enum would be cleaner than the removed name hashes.
- SG `interpolate` (`fillBasisValues`): the virtual `getInterpolationMethod` runs per point × TG × dim. It could be hoisted per TG (now cheap, low priority).
- Hygiene (for the backlog): the getter `.cpp` / `.hpp` files of `node_families/getter/` lack the banner (pre-existing, A1; only the two rewritten barycentric getters got it). `NodeFamily` keeps `TODO: Document` blocks. `NodeFamily` is copyable now that the `const id_` is gone; WP-90 deleted its copy operations.
- Python (base module): `list(DataVector)` reads past the end (debug assertion in `DataVector.__getitem__`; `pysgpp` has no `__iter__` / `IndexError`).
- Index state: many tracked changes are staged in the git index (not by this session except `git mv` / `git rm`). Use `git diff HEAD` to review.
- Remaining old names (repo grep): only `PLAN.md`, `STATUS.md`, `HISTORY.md` (master files, kept on purpose) and the thesis-discrepancy note in `claude/module-reference.md`. The thesis, `perf/results/`, `perf/summaries/` were not touched. Stale `*.os` / `*.lint` files of the snapshot branch remain under `node_families/` (git-ignored).
- Strict aliasing: the touched code has no pre-WP-14 patterns (no hot loop changed).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; it also restores the 8 protected
files and `protected.sha256` to HEAD together, so `verify` stays consistent).
