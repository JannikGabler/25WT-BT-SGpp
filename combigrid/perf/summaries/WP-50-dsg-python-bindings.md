# WP-50 — Python bindings for `DeltasSparseGrid` and its operators

Status: PROPOSED · Date: 2026-11-12 · Base commit: 007cf530f
Recommendation: accept — the agreed Python API (option (a)): `pysgpp.DeltasSparseGrid` + DSG overloads of `quadrature` / `interpolate` / `interpolateLinear` (scalar, vector, cached), checked against the `SparseGrid` operators. Python callers get the DSG evaluation savings 1:1 (1 t 4.2–7.0x, 16 t 2.2–5.8x); C++ machine code unchanged (`.text` byte-identical, e2e A/A)
Machine: `perf.py calibrate` ratios e2e 1.003 / 0.998, micro 0.996 / 1.001 (burst / sustained)

## Change
- Umbrella header `sgpp_combigrid.hpp`: + `grids/deltas_sparse_grid.hpp` and the three DSG operator headers (public-header check `WP-40/check_public_headers.sh`: OK in all 4 configurations; the umbrella header is not included by any library source).
- `combigrid.i`: the `SparseGrid` forwarders (decision 7) became the SWIG macro `COMBIGRID_SPARSE_GRID_BASE_METHODS(Class)` (forwarders to `SparseGridBase`, `getTensorGrid(LvlMI)` → `None`, `getTensorGrids` → tuple, `==` per class, `__len__` / `__getitem__`, keep-alives `_genInstr` / `_owner`), applied to `SparseGrid` (generated proxy identical except two Python comment lines moved into `.i` comments) and `DeltasSparseGrid`; `%ignore DeltasSparseGrid::getOperatorData`; `%ignore` of the three DSG `std::span` overloads; `%include` of the three DSG operator headers (they get `COMBIGRID_SOURCE_FUNC_EXCEPTION` via the existing per-name `%exception`). No other internals reachable: the index / trie / context types live in headers that are not `%include`d. Generated wrapper: exactly 18 new `_wrap_*` (DSG class: ctor, dtor, 9 methods; 6 operator overloads), one new SWIG type (`DeltasSparseGrid`), no new `std::span` / `unique_ptr` opaque types; dispatchers check (first arg, grid) per overload — `SparseGrid` and `DeltasSparseGrid` are unrelated proxies, no ambiguity.
- C++ comments only: `deltas_sparse_grid.hpp` gets banner, `@file`, class / ctor Doxygen (was `TODO: Document`; documents delta tensor grids, kept zero-coefficient MIs, `LvlMIVec` lifetime, nested nodes); `deltas_sparse_grid_quadrature.hpp` `@file` + scalar Doxygen; `deltas_sparse_grid.cpp` banner + final newline.
- New examples: `examples/c++/deltas_sparse_grid/sin_func_deltas_sg_quadrature_interpolation.cpp` and its Python port (SG vs. DSG results and evaluation counts for all three operators, vector quadrature, cheb1 → exception; both assert).
- Docs: CLAUDE.md §2 ("C++ only" notes removed) and §6 (zero-coefficient gotcha: DSG keeps them), `module-reference.md` (layout, key classes, limitations), `python-bindings.md` (build note: NumPy-equipped `python3` on PATH for SCons; examples, exposed API, translation table, not-exposed table, GIL limitation with the DSG numbers, decisions 2 and 7).
- Files: `combigrid/src/sgpp_combigrid.hpp`, `combigrid/build/pysgpp/combigrid.i`, `grids/deltas_sparse_grid.{hpp,cpp}`, `operators/quadrature/deltas_sparse_grid_quadrature.hpp`, 2 new examples, `CLAUDE.md`, `combigrid/claude/{module-reference,python-bindings}.md`.

## Gain vs. cost
- Gain (feature, Python, `python-dsg-timing.md`, cheap callable, scalar / vector m = 4): **1 t** quad cc d5 L5 6.52x / 6.59x (699k → 103k calls), d10 L2 4.17x / 4.17x; interp cc d5 L5 6.78x / 7.02x, d10 L2 4.40x / 4.52x, equi d3 L5 4.85x / 4.67x; interplin equi d5 L6 4.52x / 4.84x, d8 L5 6.56x / 6.81x (329k → 41k calls). **16 t** (one level less) 2.2–5.8x (quad / interp cc d5 L4 5.7x, d10 L1 2.2–2.5x; interplin d8 L4 5.8x). The speedup ≈ the ratio of Python calls (time per call equal for both grids), i.e. the C++ evaluation savings carry over 1:1; a Python call costs ≈ 23 µs at 16 t vs. 0.6–0.9 µs at 1 t (WP-34 GIL contention, documented, not solved).
- C++: unchanged — `.text` of `libsgppbase.so`, `libsgppcombigrid.so`, `perf_e2e`, `perf_micro`, `perf_reference` byte-identical before/after (only debug info and build ID differ: line shifts). e2e = A/A: 31 cases 1.002x (run 1) / 0.998x (re-run), all within noise except one flag per run (below).
- Memory: re-run geomean B/A 1.001, 0 MEM+/MEM-, floors 10.1 / 10.0 MiB. (Run 1 flagged 24 MEM- of ≈ 0.85 MiB = its floor difference 11.2 vs. 10.4 MiB: my `cmp` / `objcopy` of the before snapshot had pulled its files into the page cache, and fault-around maps cached pages into RSS; after `POSIX_FADV_DONTNEED` on both snapshots the floors overlap and the re-run is clean.)
- API / functionality: new Python API only (agreed 2026-11-12, option (a)); no C++ API or behavior change. A `SparseGrid` and a `DeltasSparseGrid` compare unequal (`==` → `False`). Point-dimension mismatches stay `assert`-only, as for `SparseGrid` (documented limitation; no extra validation in the bindings).
- Maintainability: `combigrid.i` +56 −31 lines (the macro removes the duplication a second `%extend` would need); C++ +52 −9 lines, comments only; examples 124 + 90 lines.

## Tests
- Pinning (passed on the unchanged code): Boost suite 3× debug (incl. `dsg_global_interpolation`, `dsg_linear_interpolation`, `quadratureDelta_*`, `vector_delta_quadrature`, `DeltasSparseGrid_*`); Python with the unchanged bindings: all 10 examples, WP-29d checks + scalar dump (1 / 16 t), WP-26/27/28 cache checks (1 / 16 t).
- New: `results/WP-50/python_checks.py` — 32 grids (equi / cc × b0 / b1 × unit / shifted × d1–3 complete + MIVec) × 2 points: DSG vs. SG for all three operators with `SourceFunc`, `VectorSourceFunc` (m = 3, also vs. scalar DSG), `CachedSourceFunc`, `CachedVectorSourceFunc` (2308 comparisons; max 1.52 units, tolerance 64 units of eps·scale, interp scale with the 1D Lebesgue sums as the C++ WP-45 test — needed: equi d1 L5 differs by 1.6e-7 between SG and DSG by ill-conditioning); evaluated point sets (DSG quad / interp: every SG point once, bitwise; interplin: the SG corner set once); a cached DSG operator after the SG operator calls Python 0 times; grid queries, coefficients (0 for closure-only MIs); cheb1 → `RuntimeError` ("nested"); wrong vector length → `ValueError`, non-sequence → `TypeError`; callable exception propagates as the same object, function reusable; plain callable → `TypeError`; `getOperatorData` absent; keep-alive of a temporary `MIVecSGGenInstr` / `LvlMIVec` and of `TensorGridCTData`; callable refcount restored. Plus `keepalive_control.py` (ASan control).
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan Boost ✓ · Python checks 1 / 16 t ✓ (`python-checks.txt`; SG scalar results old vs. new bindings 449/449 bitwise on the same libraries; cache suites ✓; 11 examples ✓) · under ASan ✓ (`python-checks-asan.txt`, 0 reports; control: removing `_genInstr` by hand → heap-use-after-free, with it clean) · SCons `SG_PYTHON=1` build ✓ (`sg-python-build.txt`: only SWIG warning 389 ×2, 0 wrapper warnings from combigrid code; `python_checks.py` 1 / 16 t ✓ and 11 examples ✓ with the SCons-built module and its OPT=0 libraries; needs `.venv/bin` first on PATH — with the system `python3` the configure step finds no NumPy and the wrapper fails on `numpy/arrayobject.h`).

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads (`refcheck.txt`, 132 OK). Expected: identical machine code.

## Benchmarks (A = WP-50-before, B = WP-50-after; speedup = A/B, > 1 is faster)
L1 micro: none (prompt; `.text` of `perf_micro` and both libraries byte-identical).

L2/L3 e2e (16 threads, `^quad/dsg/|^interp(lin)?/`, 31 cases; `e2e-rerun.md`, operator ≈ total):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.849 s | 6.846 s | 1.003 | [0.994, 1.012] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.363 s | 5.363 s | 1.003 | [0.996, 1.009] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.978 s | 1.985 s | 0.998 | [0.983, 1.013] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.841 s | 1.852 s | 0.995 | [0.975, 1.015] |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 432.6 ms | 431.6 ms | 1.003 | [0.995, 1.012] |
| interp/dsg/cc/b0/d10/L2/p8 | total | 395.7 ms | 393.4 ms | 1.003 | [0.996, 1.011] |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.172 s | 1.182 s | 0.993 | [0.989, 0.998] SLOWER |

Sum of medians (total, 31 cases): run 1 A = 29.057 s, B = 29.000 s (1.002x); re-run A = 28.995 s, B = 29.041 s (0.998x) — within noise · Flags with identical `.text` are A/A false positives: run 1 `interplin/dsg/.../d5/L8` generation 2.07 → 3.36 ms (ms-scale page-fault noise), re-run `quad/dsg/equi-exp/b0/d7/L10` 0.993x (run 1: 1.002x). Full reports: `combigrid/perf/results/WP-50/`

Peak memory (re-run): geomean B/A = 1.001 · 0 MEM+/MEM- · largest B/A `interp/sg/cc/b0/d2/L12/p64` 15.1 → 15.4 MiB (1.019x, inside spread) · process floor A = 10.1, B = 10.0 MiB

## Attempts not kept
- none (one design). Timing script: d10 L3 cases (73 M Python calls, 52 s per run) were reduced to L2.

## Profile shift / follow-ups
- Python + OpenMP: with Python callables 1 thread is 30–40x faster than 16 for both grid types (23 µs vs. 0.6–0.9 µs per call) — WP-34 territory; a doc hint "use `OMP_NUM_THREADS=1` with Python callables" may be worth adding to the examples.
- perf README pitfall candidate: reading a snapshot's files (`cmp`, `objcopy`) before `ab e2e` raises that side's peak RSS by ≈ 0.8 MiB (fault-around maps page-cached pages); evict with `posix_fadvise(DONTNEED)` or compare `.text` afterwards.
- Owner note: `lib/` now holds the OPT=0 libraries of the `SG_PYTHON=1` SCons build (the snapshots are unaffected); run `perf.py build normal` before the next measurement.
- Lint (pre-existing, now visible because the files were touched): `DeltasSparseGrid(const SGGenInstr&)` not `explicit` (same on `SparseGrid`; changing it is a C++ API change), cpplint false positive on the structured binding in `deltas_sparse_grid.cpp:28`.
- Dimension-adaptive refinement: not blocked, but the DSG must be rebuilt per MI set (construction from an instruction only; no incremental `addMI`), and its operators return only the combined value (no per-MI contributions, as noted for WP-45). Helpful: a `CachedSourceFunc` shares values between SG and DSG operators and across refinement steps (checked: DSG after SG → 0 Python calls), so a Python refinement loop pays each point once.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; the Python checks in `results/WP-50/` need the WP-50 bindings)
