# WP-61 owner rework — verification

Date: 2026-11-24 · A = checkpoint 809b8cfe1 (WP-61 as proposed) · B = staged tree (`git diff --cached`)
Machine: `perf.py calibrate` e2e 1.003 / 1.000 (burst / sustained), micro 0.997 / 0.996 · `verify` OK at start and end
Reports: `combigrid/perf/results/WP-61/rework/` · No source changes, nothing committed.

## Verdict
**OK: B breaks nothing.** Results are bit-identical to A, all tests pass (debug ×3, normal ×3, ASan), the Python bindings are unchanged, and runtime and memory are unchanged within noise. **The review found ISSUES** (below). None of them affects results. One is a constructor-overload pitfall in `FlatNestedVec` (R1). The rest are docs, TODOs, lint, naming and stale paths. They need the owner before the commit.

## Build/tests
- Normal (`perf.py build normal`) `test --repeat 3`: 3/3 clean. Debug `test --repeat 3`: 3/3 clean (the known flaky test did not occur). ASan (`MemoryMax=8G`): 1/1 clean.
- `grid_storage_pinning` passes without re-pinning (its test file is not in the diff).
- cpplint: 2 new warnings, both in `cpp_concepts.hpp` (R4). Everything else is pre-existing (`lint.txt`). The build did not re-lint these files (stale stamps), so its log shows nothing.
- SG_PYTHON=1 rebuild OK. Its SWIG warnings mentioning combigrid are identical in A and B: 389 ×2 plus a 509 on `VectorSourceFunc::evaluate`, which is pre-existing; `python-bindings.md` lists only 389. A standalone SWIG run of A and B gives a **byte-identical `pysgpp_swig.py`**, so no new helper reaches Python and no `%ignore` is needed.
- All 12 `combigrid/examples/python` scripts exit 0. `results/WP-61/python_checks.py`: 1 t == 16 t, and identical to WP-61's `python_checks_t1.txt`.

## Bits
- `perf_reference` (146 cases): **A == B bitwise at 1 t** (values, ints, node checksums).
  - The 54 SG quad / mivec cases are bitwise equal across 1/2/3/16 t in A and in B, and A == B.
  - At 2/3/16 t, 19/28/46 cases differ between A and B. They are all DSG quadrature or SG interp/interplin, which use thread reductions. A run against itself at 16 t differs in the same 46 cases, so this is run-to-run noise, not the rework (`reference_bits.txt`).
- `results/WP-61/pinning_quad_dump.cpp`: A and B outputs are identical and equal WP-61's `pinning_quad_values_after.txt`. The program forces 1 thread, so thread counts are covered by the reference run and `thread_count_independent_quadrature` (`pinning_quad_dump_ab.txt`).
- e2e per round (5 rounds, 16 t): **24/24 SG quad / mivec cases give one value per version, identical in A and B** (`e2e-results-per-round.md`).
- `compensatedSum` / `compensatedDotProduct(s)` / `CompensatedSum`, A's `tools::` vs B's `tools::numerics::` (`compsum_ab.cpp`): 6 800 checks, **0 bit differences**.
  - Inputs: n = 0 … 54 264, m = 1/2/3/5, int64 / float / double inputs, ill-conditioned and exact-cancellation inputs.
  - Compile modes: `-O0`, `-O3 -march=native`, `gnu++23 -ffp-contract=fast`, `-O3 -flto -DNDEBUG`.
- No bit differs, so `combination_variants` was not needed. Refcheck 1 and 16 t: 145 OK, 1 WARN = the known `interp/sg/equi/b0/d2/L5/cos-shifted` (×2.34) (`refcheck.txt`).

## Runtime/memory
Snapshots `WP-61-rework-A` (`--commit 809b8cfe1`) and `WP-61-rework-B`.
- Micro `^micro/(quad/|expensive/quad_(vec|scalar_x_m)/sg/)` (52 cases: the 48 `micro/quad/` cases incl. `compensated_combination`, plus the 4 SG expensive vector cases that run the vector operator end to end): **all ~ noise, 0 SLOWER**. Sum 22.347 → 22.344 s (1.000x). `compensated_combination` d15 L6: 119.27 → 119.37 µs (0.999, CI [0.996, 1.003]). `expensive/quad_vec/sg/.../c1us`: 61.04 → 61.05 ms (`micro.md`).
- Full e2e, 5 rounds, 63 cases (`e2e.md`):
  - Suite 36.378 → 36.375 s (1.000x). `expensive/` 17.780 → 17.782 s (1.000x). Cached 1.045 → 1.044 s.
  - First pass flagged 2 SLOWER (`mivec/sg/cc-exp/b1/d15/S10/n300` 0.990 [0.981, 1.000]; `expensive/interplin/dsg/.../c100us` 0.994, code the rework does not touch) and 1 faster (`quad/sg/cc-uq/b0/d2/L20`, bimodal).
  - Rerun with 8 rounds (`e2e-flagged-r8.md`): 1.014 ~, 1.003 (faster) and 1.013 ~. **None reproduces, so all three are noise.**
- Peak memory: geomean B/A 1.007, **0 MEM+/MEM−**. The largest ratio, `quad/sg/equi-exp/b0/d2/L20`, is 108.7 → 121.3 MiB (spread 25 % / 32 %; 41 TGs, so not the buffer). Process floor 10.1 / 10.2 MiB.
  - In B, the vector path also allocates nTG + 1 offsets (8 B/TG) and value-initializes both buffers. Negligible, and not visible in micro.

## Review findings (suggested fixes, not applied)
- **R1 `FlatNestedVec` constructor overloads change the meaning of existing brace syntax** (`flat_nested_vector.hpp:173`, `:193`; tested with A's and B's headers):
  - `FlatNestedVec<int, 1> v({5})` was `[5]` in A. In B it picks the capacity constructor and gives size 0.
  - `FlatNestedVec<int, 2>({{5}})`, `({{5, 6}})` and `= FlatNestedVec<int, 2>({{2, 3}})` built one sub-vector in A. In B they are ambiguous compile errors.
  - `FlatNestedVec<int, 1>(5)` reserves capacity, unlike `std::vector(5)`; this is documented. No current call site is affected.
  - Fix: replace both constructors with named factories (e.g. `static FlatNestedVec withShape(const std::array<size_type, D>&)`; drop the capacity constructor in favour of `reserve()`), or tag them (`FlatNestedVec(ShapeTag, shape)`). Add a test for `FlatNestedVec<int, 1>({5})` / `<int, 2>({{5}})`.
  - **Proposal (owner asked for constructors, 11-24):** replace both constructors with one variadic constructor taking exactly D integers, `FlatNestedVec<double, 2>(n, m)`, like the nested size constructors of `std::vector`; drop the capacity constructor (`reserve()`). Braced lists then always select the nested-vector constructor again, as in A. Prototype: `results/WP-61/rework/flat_nested_vector_proposal.diff` (all A meanings restored; adapted `testShapeConstructor` passes; `quadrature_tools.cpp` compiles with `estimates(nTG, nOutputs)`).
- **R2 Usability regression of the compensated-sum templates** (`compensated_sum_tools.hpp:41`, `:66`, `:96`):
  - The parameters are `std::span<const IType>`, so neither `std::vector` nor a non-const `std::span<double>` deduces. Every call site must spell `<double, CTCoeffType, double>`: `quadrature.cpp:39`, `:60`, `perf_micro.cpp:274` and all tests.
  - `static_castable_to` now accepts `bool`, enums and class types with explicit conversions; A's `Arithmetic` excluded `bool`.
  - Fix: take `std::ranges::contiguous_range R` (+ `sized_range`), constrained on `static_castable_to<std::ranges::range_value_t<R>, OType>` (or an arithmetic concept), with `OType = double` as the first, defaulted template parameter.
  - **Proposal (11-24):** new concept `misc::ContiguousRangeCastableTo<R, To>` (contiguous + sized, `static_castable_to`, no `bool`); templates `<std_real_type OType = double, ContiguousRangeCastableTo<OType> R...>` taking `const R&`, bodies on `std::span(r)`; `compensatedDotProducts` takes `std::type_identity_t<std::span<OType>>` so `OType` stays `double` for a `std::vector` result. Calls become `compensatedDotProduct(c, v)` / `compensatedDotProducts(c, tgEstimates, result)`. Prototype: `results/WP-61/rework/compensated_sum_deduction_proposal.diff`; bits == A (`compsum_ab.cpp` with deduced calls, 6 800 checks, 4 compile modes); standalone timing equal (dot n = 54 264: 119–121 µs both; m = 3: 433 µs both).
- **R3 Stale / TODO docs:**
  - `compensated_sum_tools.hpp:7` has `@file compensated_sum.hpp`, a duplicate of the other header's `@file`, which Doxygen merges. Its `@brief` (l. 8–14) still says "in double", "the accumulator", "every contiguous range of arithmetic values".
  - `@tparam R/RA/RB` (l. 35, 57–59, 88–89) do not match `OType/IType*`. The three `TODO: Document (update)`.
  - `quadrature_tools.hpp:33`, `:49`: `TODO` / missing `@return`. `:39` still says `tools::compensatedDotProduct`.
  - `cpp_concepts.hpp:7`, `:18`, `:27`, `:31`: `TODO`, no `@file` / `@brief`.
  - `compensated_sum.hpp:12` and `compensated_sum_tools.hpp:18`: `<cassert>` unused in the former. `cpp_concepts.hpp` uses `std::remove_cvref_t` without `<type_traits>`.
- **R4 Lint / naming in `cpp_concepts.hpp`:**
  - l. 38: no final newline. l. 33: cpplint false positive on the requires-expression (`// NOLINT(readability/braces)`).
  - `one_of`, `std_real_type`, `static_castable_to` are snake_case, next to `StrictSubclassOf` in the same file and the former `Arithmetic`. CLAUDE.md §5 has PascalCase for types. Suggest `OneOf`, `StaticCastableTo`, and `std::floating_point` (or `StdFloatingPoint`) instead of `std_real_type`.
  - "One type per file": four concepts in one header. It is the owner's call whether concepts count; A had one header per topic.
- **R5 Stale paths:**
  - `combigrid/claude/module-reference.md:256` (`tools::StrictSubclassOf`, link to the removed `tools/concepts/`) and `:365-369` (`tools/math/compensated_sum.hpp`, `tools::CompensatedSum`, `tools::ArithmeticRange`).
  - `perf_micro.cpp:253` (`tools::compensatedDotProduct`).
  - Test brief `compensated_sum_test.cpp:9`: `tools::compensatedDotProduct<double, CTCoeffType, double>(s)`, which neither names the namespace nor reads well.
  - Stale artifact `src/.../tools/math/compensated_sum.hpp.lint`. No reference remains in `.i` files.
  - `python-bindings.md:162`: "only 389 expected", but 509 is pre-existing (not from the rework).
- **R6 Test location:** `tests/tools/math/compensated_sum_test.cpp` tests `tools/numerics/` → move it to `tests/tools/numerics/compensated_sum_test.cpp` and delete the old `.lint` / `.os`.
- **R7 Error message:** `quadrature.cpp:50` dropped the `quadrature: ` prefix. The other five operators keep `<operator>: The size of the result …`. Restore the prefix.
- **R8 Lost rationale:** `quadrature.cpp` lost the comment explaining the TG-indexed buffer + compensated combination (bitwise thread independence; 2e-8 round-off of a plain loop). It survives only partly in `quadrature_tools.hpp:37-40`. Suggest a one-line comment at the call (CLAUDE.md: comments explain complex parts).
- **R9 Namespace consistency (nit):** `tools::numerics` is a new sub-namespace for a general helper. `tools/math/` and `tools/data_vector/` use plain `tools`, and `miscellaneous/numerics/double_double.hpp` uses plain `misc`. Either align them or document the pattern.
- **Checked, OK:**
  - Shape-constructor edge cases: zeros at any depth, D = 1, narrow `Index`, and the Index overflow assert (`nParents ≤ max/nChildren` covers `off[nParents]` and the next depth's product).
  - Zero-length rows (m = 0: empty `subspan`). The vector buffer is index-major, matching A's layout.
  - `misc` under `miscellaneous/`, `tools::numerics` under `tools/`; banners and include style are clean.

`lib/` currently holds the OPT=0 SG_PYTHON build; run `perf.py build normal` before the next benchmark.

## Follow-up (2026-11-24, applied on the owner's request)
- R1: `FlatNestedVec(n1, ..., nD)` (exactly D integers) replaces the capacity and `std::array` shape constructors; braced lists select the nested-vector constructor again; tests adapted + `testBracedListSelectsNestedConstructor`.
- R2: option A: `compensatedSum / compensatedDotProduct(s)<OType>(ranges...)`, `OType` explicit (no default), ranges deduced (`misc::ContiguousRangeCastableTo`, no `bool`); all call sites `<double>`; compile-time accept / reject checks in the test. Prototype bits == A, timing equal (above).
- R3 / R4: docs, `@file`, `@return`, TODOs, includes; concepts renamed `OneOf`, `StdFloatingPoint`, `StaticCastableTo`; cpplint clean (one-line `requires` + `// NOLINT`). Open: "one type per file" for `cpp_concepts.hpp` (owner).
- R5: module reference (paths, layout, compensated sum), `perf_micro.cpp` comment, `python-bindings.md` (509 expected), stale `.lint`; option left to the owner: drop the redundant `%extend VectorSourceFunc::evaluate` (removes 509).
- R6: test moved to `tests/tools/numerics/compensated_sum_test.cpp` (plain `mv`, not staged); old `.lint` / `.os` deleted.
- R7: `quadrature: ` prefix restored. R8: comment at the combination restored. R9: kept `tools::numerics`, documented the topic sub-namespaces in CLAUDE.md §5 and the module reference.
- **Verification of the follow-up** (`results/WP-61/rework/*-followup.*`):
  - Debug `test --repeat 3`: 2/3 clean. Run 1 failed in `source_function_cache_adaptive/concurrent_operator_calls_on_one_function` (`modeSwitches >= 1`, got 0). That check is timing-dependent and pre-existing: interleaved, A (809b8cfe1, debug worktree) and the current tree each failed **2 of 60** suite runs, always on the same check; alone it passed 40/40. **New flaky test** (not WP-35) → LESSONS / a follow-up WP.
  - Normal `test --repeat 3`: 2/3 clean; run 3 hit the known flaky `equidistant_nodes/sin_func_random_cube_3D` (alone 20/20 clean). ASan 1/1 clean.
  - SG_PYTHON=1: only SWIG warnings 389 ×2 and 509; `pysgpp_swig.py` byte-identical to A's; 12/12 examples pass; `python_checks.py` 1 t == 16 t == the earlier bits.
  - Snapshot `WP-61-rework-C`: refcheck 1 / 16 t = only the known WARN; `perf_reference` == A bitwise at 1 t (146 cases); the 54 SG quad / mivec cases == A at 2/3/16 t and thread-count independent.
  - `lib/` is the normal build again.
