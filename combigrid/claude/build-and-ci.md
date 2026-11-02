# Build, test and CI details (combigrid)

On-demand reference for [CLAUDE.md](../../CLAUDE.md). The everyday commands are in CLAUDE.md.

## SCons

- Driven by [SConstruct](../../SConstruct) and per-module `SConscript`s. Module logic (source
  discovery, library building, Boost tests, style checks):
  [site_scons/ModuleHelper.py](../../site_scons/ModuleHelper.py); configuration / detection:
  [site_scons/SGppConfigure.py](../../site_scons/SGppConfigure.py). All variables: `scons -h`.
- Modules: `SG_ALL=<bool>` (default 1) + `SG_<NAME>=<bool>` overrides; `SG_PYTHON=<bool>`;
  `OPT=<bool>`; `CPP_STD` (C++23 for combigrid, applied by `checkCppStandard()`).
- `ModuleHelper.scanSource()` picks up every `*.cpp`/`*.hpp` under `combigrid/src/` (combigrid's
  `excludeFiles` is empty); `buildBoostTests()` picks up every test `.cpp` under `combigrid/tests/`.
- Flag sources: `OPT=1` adds `-O3 -DNDEBUG` ([SGppConfigure.py:44](../../site_scons/SGppConfigure.py#L44))
  and, for GCC, `-fno-semantic-interposition` for compiling and linking (`configureGNUCompiler`);
  `ARCH=avx2` adds `-mavx2 -mfma -mfpmath=sse`. `-fno-strict-aliasing` only for the SWIG Java wrapper
  ([jsgpp/SConscript](../../jsgpp/SConscript)).
- `CPPFLAGS=` / `LINKFLAGS=` are appended, not overridden. Build variables persist in
  `buildVars.out` (`vars.Save`).
- Style checks (`CHECK_STYLE=1`, default) touch a `.lint` sibling for every checked file; copyright
  banner (`tools/check_copyright_banners.py`), include style (`tools/check_includes.py`), cpplint
  ([CPPLINT.cfg](../../CPPLINT.cfg): filters `build/c++11`, `runtime/references`; line length 100).
  Enforced only by the scons build ([SConstruct:390-431](../../SConstruct#L390-L431)); no pre-commit hook.
- Outputs: `lib/libsgppbase.so`, `lib/libsgppcombigrid.so`; `compile_commands.json` in the repo root.
  `*.os` / `*.gcda` / `*.lint` are build artifacts / coverage data / lint markers (`scons -c` cleans).

## Optional dependencies

- `USE_EIGEN=1` / `USE_ARMADILLO=1` (default off): O(n log n) Waldvogel FFT for Clenshaw-Curtis
  weights ([clenshaw_curtis_quadrature_rule.cpp](../src/sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.cpp)),
  otherwise O(n²). Eigen wins if both are set (FFT 1.2–3.7× faster). Checked in
  [base/SConscript](../../base/SConscript) (adds `-DUSE_EIGEN` / `-DUSE_ARMADILLO`); the code checks
  these macros, never `__has_include`.
- Source-function cache (`SourceFuncCache`, `CachedSourceFunc`, `CachedVectorSourceFunc`; always
  compiled, its constructors throw `base::dependency_exception` with the reason if a library is
  missing):
  - point table (every policy): Boost ≥ 1.81 (Boost.Unordered) + xxHash ≥ 0.8, both header-only;
    detected in the preprocessor only
    ([dependencies_defines.hpp](../src/sgpp/combigrid/tools/defines/dependencies_defines.hpp),
    `__has_include` + version macros), no SCons variable.
  - storage policies (`TemporaryStorage`, `PersistentStorage`): LMDB ≥ 0.9.14 (+ POSIX).
    `USE_LMDB` follows the convention of `USE_GSL` / `USE_ZLIB` (owner's decision 2026-11-02):
    `detectLMDB` ([SGppConfigure.py](../../site_scons/SGppConfigure.py)) always runs, links
    `-llmdb` and defines `SGPP_COMBIGRID_LMDB_LINKED` for every module when it finds `liblmdb` +
    `lmdb.h` (default `USE_LMDB=0` does not disable it); `USE_LMDB=1` aborts the configure step if
    LMDB is missing. `dependencies_defines.hpp` then checks the header and the version.
  - Fallbacks (the stubs that throw): `CPPFLAGS=-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE` and / or
    `-DSGPP_COMBIGRID_DISABLE_LMDB_STORE`.
  - Only `.cpp` files and internal headers include `dependencies_defines.hpp` (and thus `xxhash.h`,
    `lmdb.h`, Boost.Unordered); the public headers forward-declare the table and the store, so
    user code needs none of these libraries and sees the same class layouts in every
    configuration. Check: `bash combigrid/perf/results/WP-40/check_public_headers.sh`.
- `USE_DAKOTA=1`: declared combigrid-only, described in
  [combigrid/doc/doxygen/install_dakota.doxy](../doc/doxygen/install_dakota.doxy), but no use in
  `combigrid/src/` (dormant; unverified whether it is still meant to be wired up).

## Tests

- Boost.Test, dynamic link; all files under [combigrid/tests/](../tests/) → one executable
  `combigrid/tests/test_combigrid_boost`; `BOOST_TEST_MODULE` in
  [test_base.cpp](../tests/test_base.cpp). Layout mirrors `src/`. Representative test:
  [grids/sparse_grid_construction_test.cpp](../tests/grids/sparse_grid_construction_test.cpp).
- `COMPILE_BOOST_TESTS=0` skips compiling (and style + Boost linkage); `RUN_BOOST_TESTS=0` compiles
  only. Boost paths: `BOOST_INCLUDE_PATH=` / `BOOST_LIBRARY_PATH=` (defaults `/usr/include`,
  `/usr/lib/x86_64-linux-gnu`).
- `RUN_PYTHON_TESTS=1` is a no-op for combigrid (no Python tests registered); the bindings are checked
  via the Python examples ([combigrid/examples/python/](../examples/python/), `RUN_PYTHON_EXAMPLES=1`).

## CI

[.github/workflows/ci.yml](../../.github/workflows/ci.yml), on push to `master` and every PR:
windows (MinGW, `SG_PYTHON=1`, static/shared), linux-minimal (`SG_PYTHON=0`, `-Werror`),
linux-medium (`SG_ALL=0 SG_BASE=1 SG_PYTHON=1`, `-Werror`), linux-full (everything, `-Werror`),
linux-packaging (deb + wheel). **Combigrid is only built in linux-full** — reproducing it locally
needs the default `scons` with Boost, SWIG, NumPy, GSL, Eigen, Armadillo, Gmm++, UMFPACK.
[.jenkins/Jenkinsfile](../../.jenkins/Jenkinsfile) drives the Stuttgart Jenkins.

## Other pointers

- Docs: https://sgpp.github.io/SGpp/ (API reference), http://sgpp.sparsegrids.org.
- Install notes: [INSTRUCTIONS](../../INSTRUCTIONS), [INSTRUCTIONS_MAC](../../INSTRUCTIONS_MAC),
  [INSTRUCTIONS_WINDOWS](../../INSTRUCTIONS_WINDOWS). License: [LICENSE.md](../../LICENSE.md)
  (modified BSD).
- Examples: [combigrid/examples/c++/](../examples/c++/) (minimal:
  `quadrature/simple_const_func_full_sg_quadrature.cpp`,
  `global_interpolation/sin_func_full_sg_interpolation.cpp`; custom MI set:
  `global_interpolation/sin_func_multiindex_vector_sg_interpolation.cpp`).
