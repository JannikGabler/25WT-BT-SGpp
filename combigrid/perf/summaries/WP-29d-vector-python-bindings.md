# WP-29d — Vector-valued source functions, design T, step (d): Python bindings (+ WP-29c cleanup)

Status: PROPOSED · Date: 2026-10-31 · Base commit: fac9c5787
Recommendation: accept — the agreed design-T Python interface (`pysgpp.VectorSourceFunc(callable, nOutputs)` + `DataVector` overloads of all three operators), with checked results and exception propagation; the C++ cleanup changes no machine code (`.text` of both libraries byte-identical, e2e 0.995x = A/A noise, refcheck OK)
Machine: `perf.py calibrate` ratios e2e 1.003 / 0.999, micro 1.000 / 1.002 (burst / sustained)

## Change
- Bindings: new `VectorSourceFunc.i` (`%ignore` of both `std::function` ctors, of `evaluate(point, span)` and `evaluateNormalizedInPlace`; `%extend` ctor `VectorSourceFunc(PyObject* pyfunc, size_t nOutputs)`, dtor, `evaluate(point)` → `DataVector`). `SourceFunc.i`: `PyCallableHolder` split into `invoke` (GIL, tuple, call, error recording; unchanged logic) + `call` (scalar) + `callVector` (`PySequence_Fast`; wrong length → `ValueError`, non-sequence / non-number → `TypeError`, recorded like a callable's exception: m NaNs, re-raised after the C++ call); registry as a template per function type (`sourceFuncRegistry<Func>`, one mutex), `raisePendingSourceFuncError(arg1)` picks it by `arg1`'s type, so `COMBIGRID_SOURCE_FUNC_EXCEPTION` per operator name covers scalar and vector overloads. `combigrid.i`: removed the three `%ignore`s of the `DataVector` vector overloads (the three `std::span` ones stay) and the six stale `%ignore`s `global_interpolation::*`, `linear_interpolation::*`, `quadrature_operator::*` (each re-checked: no bound header declares these namespaces any more; the tools headers are not `%include`d). DSG quadrature stays unbound.
- Cleanup (no behavior change): tools headers get real Doxygen (`@file`, the three kernel templates, `sumOverCorners`) instead of `TODO: Document`; the comment on the collapse steps now states the measured structure (no attributes; LTO inlines both into `processTensorGridInternal`, which stays one function per value type — verified with `nm`: the exported wrappers are 0x3d / 0x1f B calls; the old comment claimed `noinline` and that the kernel is inlined into the wrappers). Banner on `global_interpolation_tools.cpp` (was missing), trailing newlines, include-what-you-use (`<span>`, `<utility>`, `<limits>`, `power.hpp`, …); file-scope `using namespace` removed in both operator `.cpp` (calls qualified); cpplint TODO in `global_interpolation.cpp:40` removed. The "commented-out declarations/definitions" named in the prompt no longer existed in HEAD (removed at the owner's accept).
- Files: `combigrid/build/pysgpp/{SourceFunc.i,combigrid.i}`, new `VectorSourceFunc.i`; `operators/{global,linear}_interpolation/*.cpp`, `tools/operators/{global,linear}_interpolation/*_tools.{hpp,cpp}`; new `combigrid/examples/python/vector_func_full_sg_quadrature_interpolation.py`; docs CLAUDE.md §2/§6, `module-reference.md` (layout, key classes, operators, limitations), `python-bindings.md`. Umbrella header unchanged (already had `vector_source_function.hpp`).

## Gain vs. cost
- Gain (feature, Python, `python-vector-timing.md`; one vector run vs. m scalar runs, cheap callable; "same work" = m exps per call, "shared" = one exp): **1 thread** m = 4: 1.42–1.60x / 2.45–2.64x; m = 32: 2.02–2.46x / 8.2–9.8x; m = 1: 0.68–0.76x. **16 threads** m = 4: 3.7–4.1x; m = 32: 24.3–30.6x; m = 1: 0.95–1.07x (a Python call costs ≈ 23 µs at 16 t vs. 0.7 µs at 1 t: WP-34 GIL contention; one call per point instead of m dominates). The binding adds ≈ 27 ns per call at m = 1 (constant list vs. constant float, 0.283 vs. 0.256 µs); the rest of the m = 1 cost is the callable building its list.
- C++: unchanged — `.text` of `libsgppcombigrid.so` and `libsgppbase.so` byte-identical before/after (`objcopy --only-section=.text` + `cmp`), so the A/Bs are A/A runs: e2e 12 cases 0.995x, 0 SLOWER / 0 faster; micro 0.999x.
- Memory: geomean B/A 1.001, 0 MEM+/MEM- (largest `interp/sg/cc/b0/d2/L12/p64` 14.8 → 15.1 MiB, inside its 3 % spread); floor 10.3 / 10.4 MiB.
- API / functionality: the agreed design-T Python interface only. Side effect: a plain callable passed to an operator still raises `TypeError`, now with SWIG's overload message ("Wrong number or type of arguments for overloaded function 'quadrature'") instead of "argument 1 of type SourceFunc const &". C++ signatures and scalar results unchanged (Python scalar results bitwise identical, below). No bound C++ header changed.
- Maintainability: bindings +61 (new file) / +110 −41 (`SourceFunc.i`) / +9 −19 (`combigrid.i`) lines incl. comments; the holder's call path exists once for both function types. C++ cleanup: +98 −31 lines (mostly Doxygen and includes), −2 `using namespace`.

## Tests
- Pinning (passed 3× debug on the unchanged code): full Boost suite (incl. `interpolation_pinning`, `vector_interpolation`, `vector_quadrature`, `vector_source_function`); Python: 448 scalar results (quadrature / interpolate / interpolateLinear × equi/cc/cheb1 × b0/b1 × unit/shifted × d1–3 complete + MIVec × 2 points × 2 functions) dumped with the **unchanged** bindings at 1 and 16 t (`scalar-before-t*.json`), all 8 examples OK.
- New: `results/WP-29d/python_checks.py`: vector component k vs. scalar run (896 comparisons: same matrix × m = 1/3; 1 t: 820 bitwise, max 0.23 units of eps·Σ|c|·max|f|·max(1, vol), limit 64); wrong return length (too long / short / empty) → `ValueError`, non-sequence / `None` / non-number → `TypeError` for all three operators and `evaluate`; tuple / NumPy / int results accepted; a Python exception raised at the 5th evaluation propagates as the same object (1 and 16 t) and the function is reusable afterwards; `nOutputs = 0` → `RuntimeError`, `-1` / missing → `TypeError`/`OverflowError`, non-callable → `TypeError`; overload selection (`SourceFunc` → float, `VectorSourceFunc` m = 1 → `DataVector` of size 1 = scalar); reference count of the callable +1 / back after `del`; a temporary lambda stays alive. New example (vector quadrature + both interpolations, component k vs. scalar run, asserts).
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan (capped 8 G) ✓ · Python checks 1 / 16 t ✓ with the normal libs (`python-checks.txt`; scalar results 449/449 bitwise at 1 t, ≤ 1.8e-15 rel. at 16 t = reduction order), ✓ under ASan (instrumented wrapper + ASan libs, `python-checks-asan.txt`, no reports) · full `SG_PYTHON=1` SCons build: only SWIG warning 389 (×2), 0 wrapper warnings from the new code (`sg-python-build.txt`); all 9 examples OK; checks ✓ (`python-checks-sg-python-build.txt`: its OPT=0 libs differ from the OPT=1 dump by ≤ 1.1e-14 rel.; old vs. new bindings on the same OPT=0 libs: 449/449 bitwise). No new opaque `std::span` users (only `NodeGenFunc.genDeltaNodes*`, as documented).

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads (`refcheck.txt`; 104 OK). Expected: identical machine code.

## Benchmarks (A = WP-29d-before, B = WP-29d-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`, 8 cases, all within noise; sum 50.58 → 50.61 ms):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interp/global_tg/cc/b0/d10/L2 | 47.16 ms | 47.19 ms | 0.997 | [0.988, 1.006] |
| interp/global_tg/cc/b0/d5/L6 | 1.12 ms | 1.12 ms | 0.999 | [0.986, 1.012] |
| interp/global_tg/cheb1/b0/d5/L4 | 508.65 µs | 508.57 µs | 1.002 | [0.981, 1.023] |
| interplin/tg/equi/b1/d10/L5 | 242.67 µs | 244.91 µs | 0.981 | [0.942, 1.022] |
| interplin/tg/equi/b1/d8/L5 | 42.47 µs | 42.49 µs | 1.000 | [0.995, 1.006] |

L2/L3 e2e (16 threads, `e2e.md`, `^interp/|^interplin/`, 12 cases, all within noise; operator ≈ total):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.467 s | 2.494 s | 0.990 | [0.970, 1.010] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.985 s | 1.989 s | 1.001 | [0.985, 1.017] |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.528 s | 1.548 s | 0.988 | [0.962, 1.014] |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.448 s | 1.448 s | 1.001 | [0.994, 1.007] |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.261 s | 1.258 s | 1.001 | [0.988, 1.014] |

Sum of medians (total, 12 cases): A = 10.230 s, B = 10.276 s, saved = −0.046 s (0.995x, within noise; identical `.text`) · Full reports: `combigrid/perf/results/WP-29d/`
Peak memory: geomean B/A = 1.001 · 0 MEM+/MEM- · process floor A = 10.3, B = 10.4 MiB

## Attempts not kept
- none (one design). Fixed during development: `COMBIGRID_SOURCE_FUNC_EXCEPTION(VectorSourceFunc::evaluate)` placed after the `%extend` block is silently not applied (global handler; exceptions became NaN) → moved before it, documented in python-bindings.md "How to extend" item 6.

## Profile shift / follow-ups
- Base (outside combigrid): `DataVector` in Python has an unchecked `__getitem__` and no `__iter__`, so `list(dv)` / `sum(dv)` / `for x in dv` read past the end and segfault — hit twice while writing the checks, now documented (python-bindings.md, example). Fix candidate: `__iter__` or `IndexError` in base's `DataVector.i`.
- WP-34 (GIL): numbers above; one vector call per point is the cheapest Python-side mitigation today. Python m = 1: use `SourceFunc` (documented).
- Tooling: `tools/check_copyright_banners.py` evaluates `processFilePy(path) or processFile(path)`; `processFilePy` returns `True` for non-`.py` files, so C++ banners are never checked (only cpplint's `legal/copyright`, and only when a `.lint` target is stale — why WP-29c reported 0 style warnings with a bannerless `.cpp`). CLAUDE.md §5 ("build fails otherwise") is therefore not true.
- Out-of-scope leftovers: `TODO: Document` blocks in `tools/operators/quadrature/{quadrature_tools,deltas_sparse_grid_quadrature_tools}.hpp`, `// TODO: Delete` at `quadrature_tools.cpp:112`; `tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.{hpp,cpp}` without banner; pre-existing `-Wunused-variable` `sum` in `grids/tensor_grid.cpp:34`.
- Strict aliasing: the touched C++ had no pre-WP-14 workarounds; nothing to simplify.
- Dimension-adaptive refinement / WP-26: not blocked. The Python `VectorSourceFunc` is a thin `std::function` adapter, so a C++ cache wrapping either function type (record = key + m doubles) sits behind it unchanged; cache hits would also skip the GIL (WP-25: 1925–2502x warm at 16 t). Per-TG vector kernels exist in C++ (not bound, like the scalar ones).
- Owner files now outdated: CLAUDE.md §5 banner statement (above); PLAN.md/ANALYSIS.md symbol names of WP-29c (see the WP-29c summary). The working tree also holds the master's uncommitted PLAN.md / STATUS.md edits (issuing this WP; not touched).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; the Python checks in `results/WP-29d/` stay usable against any build with `VectorSourceFunc`)
