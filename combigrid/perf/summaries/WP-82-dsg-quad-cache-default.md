# WP-82 — DSG quadrature: cross-call operator data cache on by default

Status: PROPOSED · Date: 2026-11-18 · Base commit: b9bf8bb65 (+ the master's uncommitted doc edits, kept)
Recommendation: accept. This is the owner's default decision, implemented as a separate options type. Repeated DSG quadrature calls run 1.04–1.31x faster at 16 t, and the judged suite is neutral. Caveat: grids dominated by long 1D rules keep a large index (cc d2 L20: 64 MiB).
Machine: `perf.py calibrate` ratios e2e 1.000 / 0.999, micro 0.998 / 1.002

## Change
- New public `DeltasSparseGridQuadratureOptions` (`operators/quadrature/deltas_sparse_grid_quadrature_options.hpp`, field `useCrossCallOperatorDataCache = true`). All three DSG `quadrature` overloads (scalar, span, `DataVector`) take it as their last defaulted argument. `QuadratureOptions` is now `SparseGrid`-only (default stays `false`). This follows WP-73's style (one options type per operator, the default is in the struct), so the struct's value is always the default you get by leaving the argument out. Per-overload default arguments were not used: there, `QuadratureOptions{}` passed explicitly would have meant "off" on a DSG.
- **API change** (agreed default flip; type split is the chosen mechanism): C++ / Python code that passed `QuadratureOptions` to the DSG quadrature no longer compiles / raises `TypeError` (only since WP-73). Braced options `quadrature(f, dsg, {.useCrossCallOperatorDataCache = x})` work for both grid types.
- SWIG: `%include` of the new header, the DSG span `%ignore` spells out the new default argument. Umbrella header includes the new header. Doxygen `@note`s / option docs updated (incl. memory caveat).
- Docs: CLAUDE.md §2 (defaults per grid type), `module-reference.md` (cache paragraph, sizes), `python-bindings.md` (options list, checks script).
- `perf_micro.cpp`: the `quad_dsg` crosscall lambda now passes `DeltasSparseGridQuadratureOptions{.useCrossCallOperatorDataCache = !flipped}`. The case meanings stay the same (`call` = default options, `call_flipped` = default flipped); the doc comment was adjusted. The before snapshot uses the old file.
- Files: `deltas_sparse_grid_quadrature_options.hpp` (new), `deltas_sparse_grid_quadrature.{hpp,cpp}`, `quadrature_options.hpp`, `sgpp_combigrid.hpp`, `combigrid.i`, `cross_call_operator_data_test.cpp`, `perf_micro.cpp`, CLAUDE.md, `combigrid/claude/{module-reference,python-bindings}.md`.

## Gain vs. cost
- Gain: repeated DSG quadrature calls on one grid (default options, micro `crosscall/quad_dsg/call`). At 16 t: equi d3 L14 1.311x, equi b1 d15 L5 1.308x, cc d5 L10 1.043x. At 1 t: 1.044x / 1.031x / 1.005x. Plain DSG quad, cc d5 L5, 16 t: 1.149x (cheap integrand) and 1.056x (c100); the c1000 integrand is within noise. The index build is serial, so it matters more at 16 t than WP-73's estimate (1.03–1.15x). The judged suite is neutral (one call per e2e process): all 18 `total` cases are within noise, suite group 17.730 → 17.726 s (1.000x), `expensive/` 3.813 → 3.816 s (0.999x). `operator` is 1.005–1.032x on 3 cases (d3 L16, d15, d20) only because the index is now freed when the grid is destroyed, not at the end of the operator phase; `total` is unchanged.
- Memory: peak RSS unchanged (geomean B/A 1.004, 0 MEM+/MEM-, largest 1.020x on a 13 MiB case). The index was built in the call before as well. The difference is retention: the index now stays until the grid is destroyed. Measured retained bytes for the e2e DSG grids: equi d3 L16 8.0 MiB, d5 L12 1.2, d7 L10 0.9, d10 L7 1.2, b1 d15 L7 9.4, b1 d20 L6 17.8 MiB; micro cases 0.4–2.0 MiB. **cc d2 L20: 64 MiB.** The weights cover the full 1D rule of every level (like the SG table, which keeps 32 MiB there) plus the trie. So the reason SG quadrature stays off applies to such DSG grids as well. It is documented in the option's Doxygen; users can opt out per call or with `clearCrossCallOperatorDataCache()`.
- API / functionality: default flip agreed by the owner (2026-11-18). The new option type is the mechanism (allowed by the prompt) and is documented above. Results do not depend on the option (bitwise at 1 t, tests). DSG quadratures now take the cache lock (WP-73 variant (a), tens of ns per call).
- Maintainability: +1 small public header; the docs got simpler (each options struct describes one grid type).
- Dimension-adaptive CT: neutral. Each refinement step builds a new grid with an empty cache. A loop that integrates every step's grid once keeps one index per grid that is still alive, so dropping old grids frees them.

## Tests
- Pinning tests (passed on the unchanged code before the change): new `cross_call_operator_data/DSG_Quadrature_Default_Equals_Both_Option_Values` (repeated scalar + vector default calls bitwise = explicit off / on; d1/d3, b0/b1; braced options so it compiles before and after); existing `cross_call_operator_data/*`, `repeated_operator_calls/*`, `cross_call_operator_data_cache/*`, `quadratureDelta_*`.
- New / extended tests: `Defaults_Keep_Only_The_DSG_Interpolation_Indices` → `Defaults_Keep_Only_The_DSG_Operator_Data` (struct defaults of all four option types; SG quadrature scalar + vector store nothing; 4 DSG quadrature calls (2 scalar, `DataVector`, span) → exactly 1 build, one entry). The helper uses the new type. `Concurrent_Calls_Clears_And_Views` now also runs DSG quadratures (default options) concurrently with clears / views.
- Python: `results/WP-82/python_checks.py` (defaults of all option classes equal C++, entries, results default = on = off, clear, `TypeError` for the other grid type's options) OK at 1 / 16 t. All 11 Python examples run (`python_checks.txt`). SG_PYTHON=1 build OK.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan n.a. (no memory handling / lifetime code changed; the concurrent test covers the default path) · TSan not rerun (synchronization unchanged since WP-73).
- Stale now: `results/WP-73/python_checks.py` and `results/WP-73/tsan_driver.cpp` pass `QuadratureOptions` to the DSG quadrature. Their DSG calls need `DeltasSparseGridQuadratureOptions` (or braced options) to run again.

## Reference check
- `perf.py refcheck`: WARN (1 and 16 threads), 145 OK. The only WARN is the known `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34; owner 10-25: tolerance stays; also in WP-82-before; untouched code). No INFO.

## Benchmarks (A = WP-82-before, B = WP-82-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `micro.md`; 16 t: `micro_t16.md`):

| case | A | B | speedup | 95% CI | 16 t speedup |
|---|---|---|---|---|---|
| crosscall/quad_dsg/call/equi/b0/d3/L14 | 124.25 ms | 120.10 ms | 1.044 | [1.019, 1.070] | 1.311 (20.47 → 15.61 ms) |
| crosscall/quad_dsg/call/equi/b1/d15/L5 | 190.96 ms | 185.30 ms | 1.031 | [1.027, 1.035] | 1.308 (25.39 → 19.50 ms) |
| crosscall/quad_dsg/call/cc/b0/d5/L10 | 303.47 ms | 302.04 ms | 1.005 | [1.001, 1.009] | 1.043 (36.56 → 35.08 ms) |
| crosscall/quad_dsg/call_flipped/* (now: off) | — | — | 0.974–1.000 | | 0.772–0.958 (mirror) |
| cache/op/quad/dsg/cc/b0/d5/L5/none/c0 · c100 · c1000 | 3.72 · 9.06 · 99.7 ms | 3.67 · 8.96 · 100.1 ms | 1.011 · 1.013 · 0.999 | | 1.149 · 1.056 · 1.002 |
| crosscall/quad_sg/*, quad/operator/* (SG, controls) | | | 0.976–1.014 | all ~ (noise) | |

`crosscall/quad_dsg/build/cc/b0/d5/L10` 0.994 `SLOWER`: the index constructor is unchanged (placement).

L2/L3 e2e (16 threads, filter `^quad/dsg/|^mivec/dsg|^expensive/.*dsg`, 18 cases; `e2e.md`): every `total` is ~ (noise), 0.987–1.003. Main cases: `quad/dsg/cc-uq/b0/d10/L7` 6.877 → 6.875 s (1.001), `equi-exp/b0/d10/L7` 5.407 → 5.408 s (1.000), `b1/d20/L6` 1.138 → 1.136 s (1.003). `operator` faster: d3 L16 1.032, d20 L6 1.010, d15 L7 1.005 (deallocation moved out of the phase, see above).

Sum of medians (total): suite group A = 17.730 s, B = 17.726 s (1.000x) · `expensive/` A = 3.813 s, B = 3.816 s (0.999x)

Peak memory: no flags; largest `expensive/quad/dsg/cc-exp/b1/d10/L4/c100us` 13.0 → 13.3 MiB (1.020, spread 2.9 % / 1.1 %), `quad/dsg/equi-exp/b1/d20/L6` 82.9 → 82.9 MiB. Geomean B/A = 1.004 · process floor A = 10.4 MiB, B = 10.4 MiB

## Attempts not kept
- none (mechanism alternatives considered: per-overload default `QuadratureOptions{.useCrossCallOperatorDataCache = true}` was rejected as not transparent: an explicit `QuadratureOptions{}` would mean "off" on a DSG, so the struct value would differ from the overload default)

## Profile shift / follow-ups
- Owner decision candidate: large-rule DSG grids (cc d2 L20: 64 MiB retained by default). Options: (a) keep as is (documented); (b) cap: do not store indices above a size limit (new behavior → owner); (c) default off only for 1–2 dimensional grids (a dispatch rule, against the generic-design preference).
- The DSG quadrature index build is serial (4 ms at d3 L14 / d15 L5). Parallelizing `comp1DWeights` per (dim, level) would also speed up first calls and the `off` path (≈ 20 % of a 16 t call on those grids).
- Update WP-73's `tsan_driver.cpp` / `python_checks.py` to the new type if they are rerun.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests). Careful: the tree also holds the master's uncommitted doc edits (CLAUDE.md, module-reference.md, PLAN/STATUS/README, perf-master SKILL.md), which `reject` would revert as well. Use `--keep` for them.
