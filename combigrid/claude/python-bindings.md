# Python bindings of the combigrid module (pysgpp)

Detailed reference for the SWIG bindings of the combination technique (CT) module. The essentials are
summarized in §7 of the top-level [CLAUDE.md](../../CLAUDE.md). Read this file before changing the
`.i` files or a bound C++ API, and when answering questions about the Python API.

Links are relative to this file; paths in backticks are relative to the repository root.

---

## Building and running

```bash
scons -j8 OPT=0 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 SG_PYTHON=1 COMPILE_BOOST_TESTS=0 RUN_PYTHON_TESTS=0
export LD_LIBRARY_PATH=<repo>/lib:$LD_LIBRARY_PATH
export PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp:$PYTHONPATH
python3 -c "import pysgpp; print(pysgpp.SparseGrid)"
```

- Requires SWIG ≥ 3.0.4 (developed with 4.2.0, also works with 4.4.0), Python 3 headers (`python3-dev`) and NumPy (base's `numpy.i`). The SWIG message `Deprecated command line option: -py3` is harmless.
- Every SCons run re-runs SWIG and recompiles the whole `pysgpp/pysgpp_wrap.cc` (~2 min): [pysgpp/SConscript](../../pysgpp/SConscript) deletes the wrapper on purpose because SCons' SWIG dependency tracking is unreliable.
- All modules are wrapped into **one** flat module (`pysgpp_swig`, re-exported as `pysgpp`), built by [pysgpp/pysgpp.i](../../pysgpp/pysgpp.i). It `%include`s `base/build/pysgpp/base.i` first and `combigrid/build/pysgpp/combigrid.i` last, defines the global `%exception` (every `std::exception` → `RuntimeError`) and compiles the wrapper against each module's umbrella header.
- Python examples: [combigrid/examples/python/](../examples/python/), 1:1 ports of the C++ examples in `examples/c++/global_interpolation/` and `examples/c++/quadrature/` (same file names). SCons runs them with `RUN_PYTHON_EXAMPLES=1` (`runExamples()` walks `examples/` recursively). They can also be run directly with `python3 <file>.py`.
- Comparing results with C++: operators use OpenMP `reduction(+)`, so with several threads the last ~1e-15 of a result varies between runs **in C++ as well**. Use `OMP_NUM_THREADS=1` for bit-identical C++/Python comparisons.

### Fast check without SCons

To check a change to the `.i` files in about a minute, without rebuilding or overwriting anything in the build tree, build a scratch copy of the module. Run from the repository root. `lib/libsgppbase.so` and `lib/libsgppcombigrid.so` must be up to date with the C++ sources, because the wrapper links against them. Use a Python interpreter that has NumPy installed.

```bash
S=<scratch dir>; mkdir -p $S/pkg/pysgpp
swig -c++ -python -fvirtual -threads -DSG_BASE=1 -DSG_COMBIGRID=1 -I. -Ibase/src -Icombigrid/src \
     -Ibase/build/pysgpp -Icombigrid/build/pysgpp -Ipysgpp \
     -o $S/pysgpp_wrap.cc -outdir $S/pkg/pysgpp pysgpp/pysgpp.i
g++ -c -O1 -fPIC -fopenmp -std=c++23 -DSG_BASE=1 -DSG_COMBIGRID=1 -DZLIB=1 -Ibase/src -Icombigrid/src \
    $(python3-config --includes) -I$(python3 -c "import numpy; print(numpy.get_include())") -Ipysgpp \
    -o $S/pysgpp_wrap.o $S/pysgpp_wrap.cc
g++ -shared -fopenmp -o $S/pkg/pysgpp/_pysgpp_swig.so $S/pysgpp_wrap.o -Llib -lsgppbase -lsgppcombigrid
echo 'from .pysgpp_swig import *' > $S/pkg/pysgpp/__init__.py
PYTHONPATH=$S/pkg LD_LIBRARY_PATH=$PWD/lib python3 combigrid/examples/python/<file>.py
```

- Replacing `-c -O1 ... -o $S/pysgpp_wrap.o` with `-fsyntax-only` only checks that the wrapper compiles (~5 s).
- The scratch package omits base's `pysgpp.extensions`, which combigrid does not need.
- A wrapper function that takes or returns a type SWIG does not know (e.g. `std::unique_ptr`, `std::span`, iterators) still compiles, but the Python side gets an unusable opaque pointer (`SwigPyObject`, often with a "memory leak … no destructor found" message). To find such functions: `grep -oE "SWIGTYPE_p_std__(unique_ptr|span)[A-Za-z0-9_]*" $S/pysgpp_wrap.cc | sort -u`, then look up the `_wrap_*` functions that use them.

### Never clang-format `.i` files

clang-format does not understand SWIG syntax. It turns `%include` into `% include`, breaks the Python code inside `%pythoncode %{ ... %}` across lines and splits long string literals. SWIG then fails with `Unknown SWIG preprocessor directive: ...`. This happened through VS Code's format-on-save (VS Code treats `*.i` as C, and the workspace uses clangd with `editor.formatOnSave`). The local, git-ignored `.vscode/settings.json` now maps `"*.i": "plaintext"`; other checkouts are not protected.

If it happens again: restore the last correctly formatted revision of the file. To find edits that were committed together with the reformatting, compare both revisions with all whitespace removed (or token by token).

---

## Files

| file | contents |
|---|---|
| [combigrid/build/pysgpp/combigrid.i](../build/pysgpp/combigrid.i) | entry point: `%shared_ptr` declarations, container templates, generation instructions, grids, operators, CT coefficients |
| [combigrid/build/pysgpp/MultiIndex.i](../build/pysgpp/MultiIndex.i) | `MI<unsigned int>` → `LvlMI` (+ `GPMI` alias), `MIVec<unsigned int>` → `LvlMIVec` |
| [combigrid/build/pysgpp/Functions.i](../build/pysgpp/Functions.i) | growth functions (`Lvl2GPCntFunc`), `NodeGenFunc` and its getters |
| [combigrid/build/pysgpp/SourceFunc.i](../build/pysgpp/SourceFunc.i) | `SourceFunc` from a Python callable; GIL/exception machinery; `COMBIGRID_SOURCE_FUNC_EXCEPTION` |
| [combigrid/src/sgpp_combigrid.hpp](../src/sgpp_combigrid.hpp) | umbrella header the wrapper is compiled against (exactly the bound headers) |

---

## What is exposed

Names are identical to C++; argument order and defaults are unchanged.

- **Multi-indices**: `LvlMI` / `GPMI` (constructors `()`, `(count)`, `(count, value)`, `(list)`; `toLinearIndex`, `productofElems`, `sumOfElems`, `nDim`, `at`, `front`, `back`, `empty`, `size`, `capacity`, `reserve`, `shrink_to_fit`, `clear`, `push_back`, `pop_back`, `resize`, `swap`; `== != < <= > >=`, `+ - += -=`; `len`, `[]`, `str`). `LvlMIVec` (constructors `(nDim, nMI)`, `(list of lists)`, `(list of LvlMI)`; `nDim`, `nMI`, `miVec(i, d)`, `setMI`, `moveMI`, `resize`, `shrink_to_fit`, `isDownwardsClosed`, `downwardsClosure`, `componentWiseMax`, `paretoMaxima`; `len`, `[]`).
- **Functions**: `SourceFunc(callable)` with `evaluate`, `evaluateNormalizedInPlace`, `evaluateNormalizedOutOfPlace`; `linearLvl2GPCntFunction`, `doublingLvl2GPCntFunction`; `NodeGenFunc` (`genNodes`, `genNodesWithBoundary`, `genNodesWithoutBoundary`, `genDeltaNodes`, `genDeltaNodesWithBoundary`, `genDeltaNodesWithoutBoundary`, `id`, `==`) and `getEquidistantNodeGenFunc`, `getClenshawCurtisNodeGenFunc`, `getFirstTypeChebyshevNodeGenFunc`, `getSecondTypeChebyshevNodeGenFunc`.
- **Generation instructions**: `SGGenInstr` (all getters/setters, `genFullMIVec`, `genReducedMIVec`, `genFullMIVecWithCoeffs` / `genReducedMIVecWithCoeffs` → tuple `(LvlMIVec, tuple of ints)`, `clone` → owning `SGGenInstr` proxy, `resize`, `getVolumeOfDomain`, `getUniqueNodeGenFuncs`), `CompleteSGGenInstr(maxLvl, nDim)` + `setMaxLvl`, `MIVecSGGenInstr(miVec)`.
- **Grids**: `SparseGrid(genInstr)` (`nDim`, `nTG`, `getTensorGrid(idx)`, `getTensorGrid(LvlMI)` → `TensorGridCTData` or `None`, `getTensorGrids` → tuple of copies, `getGenInstr`, `getMaxTGGPCnt`, `getMaxTGSumOverGPCntsPerDim`, `==`, `len`, `[]`, iteration); read-only `TensorGridCTData` (`mi`, `coefficient`, `tensorGrid`, `==`); `TensorGrid` (constructors, `nDim`, `nGP`, `getGPCntPerDim`, `getNodesPerDim`, all `getGridPoint` / `getGridPoints` overloads, `getGridPointAndMI` (out-parameter overload or tuple `(DataVector, LvlMI)`), `==`).
- **Operators / tools**: `interpolate`, `interpolateLinear`, `quadrature`, `computeCTCoeffs`, `computeCTCoeffSingle` (→ tuple of ints).
- **Helper container types** (base naming convention): `UnsignedIntVector`, `UnsignedIntVectorVector`, `LvlMIVector`, `DoubleDoublePair`, `HyperCubeArea`, `NodeGenFuncVector`, `TensorGridCTDataVector`. Plain Python lists/tuples are accepted wherever these are expected. This does **not** apply to `LvlMI` / `LvlMIVec`, which must be constructed explicitly.

C++ → Python translation:

| C++ | Python |
|---|---|
| `double f(const DataVector& p)`; `SourceFunc sf(f)` | `def f(p): ...` (`p` is a **tuple** of floats); `sf = pysgpp.SourceFunc(f)` |
| `genInstr.setDomain({-1, 1})` | `genInstr.setDomain((-1, 1))` |
| `HyperCubeArea{{0, 1}, {-1, 1}}` | `[(0, 1), (-1, 1)]` |
| `LvlMIVec mis{{0, 5, 0}, {0, 4, 1}}` | `pysgpp.LvlMIVec([[0, 5, 0], [0, 4, 1]])` |
| `LvlMI mi{1, 2}` | `pysgpp.LvlMI([1, 2])` |
| `DataVector point{0.1, 0.2}` | `pysgpp.DataVector([0.1, 0.2])` |
| `setLvl2GPCntFunc(doublingLvl2GPCntFunction)` | `setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)` |
| `tools::computeCTCoeffs(mis)` | `pysgpp.computeCTCoeffs(mis)` |

---

## Deliberately not exposed

| C++ API | reason |
|---|---|
| `SparseGridBase` (abstract base of `SparseGrid` and `DeltasSparseGrid`) | irrelevant for Python users; its public methods are declared again on `SparseGrid` (see decision 7). Its protected manual-assembly methods (`addTensorGrid`, `setTensorGrid`, …) stay hidden: a grid without instruction or with unset size caches crashes the operators |
| `DeltasSparseGrid` and its quadrature | not in the umbrella header |
| `VectorSourceFunc` and the vector-valued `quadrature` overloads | not bound yet (planned: perf phase WP-29d). `VectorSourceFunc` has no proxy and `std::span` no typemap, so `combigrid.i` `%ignore`s both overloads |
| `QuadRule`, `InterpolationMethod` and their implementations/getters, `NodeGenFunc::getQuadRule` / `getInterpolationMethod`, concrete `node_gen_funcs::*` classes | extension machinery; node types are chosen via the getters, quadrature/interpolation rules are selected automatically |
| `global_interpolation::*`, `linear_interpolation::*`, `quadrature_operator::*` | internal; because SWIG flattens namespaces they would otherwise become public functions (`*_interpolation::interpolate` would even be merged into the public `interpolate` / `interpolateLinear` as extra overloads) |
| `tools::computeCTCoeffsNaive` | reference implementation for tests |
| `tools::ct_coeff_calc::*` | internal helpers of the CT coefficient algorithm; `internalComputeCTCoeffSingle` takes unwrapped internal types and does not even compile in the wrapper |
| `constants::*` | `constexpr`, cannot be changed at runtime |
| everything else in `misc::` / `tools::` (lookups, bounding boxes, caches, scratch buffers, concurrency, math/hash helpers, benchmarker) | internal |
| iterators, rvalue-reference overloads, `std::initializer_list` constructors, conversion operators, `MI::data/insert/erase`, `MIVec::data/lookup/clearCachedValues`, `MIVecElemProxy`, `MI` operators mixing `MI` and `std::vector` | no Python meaning / covered by other overloads |
| directors (Python subclasses of `SGGenInstr`, `NodeGenFunc`, ...) | extension points; overrides would be called from OpenMP threads |

---

## Limitations (cannot be wrapped faithfully)

- **`Lvl2GPCntFunc` is a context-free C function pointer.** The two predefined functions are exposed as function-pointer constants (`%callback`); they can be passed and compared (`==`) but **not called** from Python. Custom growth functions written in Python are impossible without changing the C++ type (e.g. to `std::function`).
- **Python source functions are evaluated serially** (every call acquires the GIL), so OpenMP does not speed them up.
- **C++ `assert`s abort the Python process** in `OPT=0` builds (e.g. `interpolate` with a point of the wrong dimension); with `OPT=1` they are compiled out (undefined behaviour). The bindings add no extra validation. Likewise, C++ accessors without bounds checks stay unchecked (`MIVec::setMI`, `miVec(i, d)`, `SparseGrid.getTensorGrid(idx)`, `TensorGrid.getGridPoint(idx)`); only the Python-added `[]` operators raise `IndexError`.
- `SparseGrid.getGenInstr()` returns an `SGGenInstr` proxy (no downcast), and SWIG drops the `const` of the returned `const SGGenInstr&`: setters called on it modify the instruction the sparse grid's operators use.
- `LvlMI` comparisons are the C++ **component-wise partial order**, not lexicographic (`sorted()` on multi-indices is meaningless); `LvlMI` is unhashable.
- The `std::span` overloads of `NodeGenFunc.genDeltaNodes*` (in-place variants) are wrapped but cannot be called from Python (SWIG has no `std::span` typemap). The out-of-place overloads returning a `DataVector` work.

---

## Non-obvious typemap and ownership decisions

1. **`SourceFunc` from a Python callable** ([SourceFunc.i](../build/pysgpp/SourceFunc.i)), modelled on base's `OperationQuadratureMC.i` (reuses its `PyObject *pyfunc` typemap; callable receives a tuple).
   - `swig -threads` releases the GIL around every wrapped call and the operators call the function from OpenMP threads → every call into Python (and every `Py_INCREF`/`Py_DECREF`) happens under `PyGILState_Ensure`.
   - A Python exception cannot cross an OpenMP region: the first exception is stored (`PyErr_Fetch`), the evaluation returns NaN, later evaluations return NaN without calling Python, and `COMBIGRID_SOURCE_FUNC_EXCEPTION(function)` re-raises it after the C++ call returns. The `SourceFunc` is reusable afterwards.
   - `SourceFunc::func` is private, so the wrapper keeps a registry `SourceFunc*` → holder (entry removed in the `%extend` destructor).
2. **Keep-alive references (`_owner`)**: proxies returned by reference into another object store their owner's proxy via `%pythonappend` — `SparseGrid.getTensorGrid` (both overloads), `SparseGrid.getGenInstr`, `SGGenInstr.clone`, `TensorGrid.getGPCntPerDim` / `getNodesPerDim`, `TensorGridCTData.mi` / `tensorGrid` (re-implemented as Python properties, since `%pythonappend` does not apply to data members). `MIVecSGGenInstr` stores `_miVec`, `SparseGrid` stores `_genInstr` (the clone inside the grid references the caller's `LvlMIVec`). Base does **not** do this (e.g. `Grid.getStorage()` can dangle). Verified with valgrind: 0 errors; a control run with `_owner` removed produces invalid reads.
3. **`%shared_ptr` only for `SGGenInstr`, `CompleteSGGenInstr`, `MIVecSGGenInstr`.** `MIVec::componentWiseMax` / `paretoMaxima` return copies instead (helpers `*Copy` renamed to the C++ name), because `%shared_ptr(std::vector<size_t>)` would change how base's `SizeVector` is wrapped everywhere.
4. **`MI::operator+=` / `-=` are not wrapped directly**: SWIG would return a second, non-owning proxy that replaces the owning one on `a += b`, deleting the object. `__iadd__` / `__isub__` are Python methods calling `__iaddImpl` / `__isubImpl` and returning `self`.
5. **Pairs returned by value** (`gen*MIVecWithCoeffs`, `getGridPointAndMI(idx)`) use custom `out` typemaps producing tuples: `std_pair.i` cannot instantiate `pair<MIVec, ...>` because `MIVec` is not copy-assignable (`const` member).
6. **`std::vector<Lvl2GPCntFunc>`** (`setLvl2GPCntFuncs` / `getLvl2GPCntFuncs`) uses custom `in`/`out` typemaps: `std_vector.i` does not compile for function-pointer elements.
7. **`SparseGridBase` is not wrapped** (SWIG warning 401 suppressed via `%warnfilter`), so SWIG does not see the methods `SparseGrid` inherits. `%extend SparseGrid` declares the meaningful public ones again (`nDim`, `nTG`, both `getTensorGrid` overloads, `getTensorGrids`, `getMaxTG*`, `operator==`) as forwarders to the inherited C++ methods. Signature changes: `getTensorGrid(const LvlMI&)` returns the `TensorGridCTData` or `None` instead of a `const_iterator`; `getTensorGrids` returns a `std::vector` copy (→ tuple) instead of a `std::span` (no SWIG typemap). **New public methods of `SparseGridBase` must be added there to be visible in Python.**
8. **`SGGenInstr::clone`** returns a `std::unique_ptr`, which clashes with the `%shared_ptr` wrapping (a class cannot be wrapped as both). It is replaced by the `%extend` helper `cloneShared` (converts to `std::shared_ptr`), `%rename`d to `clone`. An `%ignore` with a given signature also hides an `%extend` method of the same name and signature — hence the helper name + `%rename` pattern.
9. Node getters return raw pointers to static singletons without `%newobject` → non-owning proxies.

---

## How to extend the bindings

1. Apply the scoping rule: bind what a default user needs to build, configure, evaluate and query a surrogate; skip extension machinery unless it appears in a signature a default user calls.
2. Add the header to [sgpp_combigrid.hpp](../src/sgpp_combigrid.hpp) — otherwise the generated `pysgpp_wrap.cc` does not compile (undeclared identifiers).
3. Put `%ignore` / `%rename` / `%extend` / `%pythonappend` / `%exception` directives **before** the `%include "combigrid/src/..."` of the header (features only apply to declarations parsed afterwards). New `%shared_ptr` declarations go to the top of `combigrid.i`, before any class that uses the type.
4. Instantiate container/templates with `%template` using base's naming (`<Elem>Vector`, `<A><B>Pair`) after the element type is declared. Check [base/build/pysgpp/base.i](../../base/build/pysgpp/base.i) first — an existing instantiation must not be duplicated (all modules share one namespace).
5. Ownership: a method returning a reference/pointer into `self` → `%pythonappend ... %{ val._owner = self %}`; a constructor/setter that stores a reference to an argument → keep the argument alive on `self`.
6. Any new wrapped function that evaluates a `SourceFunc` (with the `SourceFunc` as **first** argument) must get `COMBIGRID_SOURCE_FUNC_EXCEPTION(<qualified name>)` before its `%include`; otherwise Python exceptions are turned into NaN and only surface at the next checked call.
7. SWIG gotchas: `%pythoncode` blocks containing `#` comments must use `%{ ... %}` (with `{ ... }` SWIG parses `#` lines as preprocessor directives); new public free-function names must not clash with other modules' names; internal helper namespaces must be `%ignore`d.
8. Rebuild (full build or [fast check](#fast-check-without-scons)) and check the log for SWIG warnings mentioning `combigrid/`. The only expected one is 389 for `MI::operator[]` (handled by `%extend`). Inspect the generated proxies in `lib/pysgpp/pysgpp_swig.py`, run [combigrid/examples/python/](../examples/python/), and comment every non-obvious directive in the `.i` file.

### When a C++ refactor breaks the bindings

The bindings depend on the C++ declarations of every header in the umbrella header, so C++ changes can break them even when the C++ build and tests pass. Typical causes and fixes:

| symptom | cause | fix |
|---|---|---|
| `pysgpp_wrap.cc` does not compile (`'X' was not declared`, `'misc' has not been declared`) | a new helper (namespace) in a bound header takes internal types | `%ignore` it (SWIG flattens namespaces) |
| a new internal function appears in `pysgpp` | same, but it compiles | `%ignore` it |
| a stale `%ignore` | a function was renamed/removed | update/remove it — SWIG does not warn about `%ignore`s that match nothing |
| methods missing on a Python class, SWIG warning 401 | they moved into an unwrapped base class | declare them again via `%extend` (decision 7) or wrap the base class |
| Python gets an opaque `SwigPyObject` / "memory leak … no destructor found" | return/parameter type without typemap (`std::unique_ptr`, `std::span`, iterators) | `%extend` helper converting to a wrapped type (decision 8) or `%ignore` |
