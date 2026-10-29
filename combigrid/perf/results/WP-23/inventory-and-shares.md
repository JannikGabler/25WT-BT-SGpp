# WP-23 — base symbols used by combigrid: inventory and profile shares

Snapshot `WP-23-before` (HEAD 0273aa1d7, judged `normal` build). Scripts: `scripts/plt_sites.py`
(PLT call sites per calling function, from `objdump -d`), `scripts/dso_split.py` (self samples
per DSO from `perf report -n --sort dso,sym`).

## 1. Inventory (`nm -D --undefined-only` ∩ exports of `libsgppbase.so`, LC_ALL=C)

| importer | base symbols |
|---|---|
| `libsgppcombigrid.so` | `DataVector()`, `DataVector(size_t)`, `DataVector(std::initializer_list<double>)`, `DataMatrix(size_t, size_t)`, `operator new(size_t)`, `operator delete(void*, size_t)` |
| `perf_e2e`, `perf_micro`, `perf_reference` | `DataVector(size_t)`, `operator new/delete`, `operator new[]/delete[]` |

Not imported by anyone: `DataVector::set`, `getPointer`, `setAll`, `copyFrom`, `resizeZero`, the
arithmetic members, … `operator[]`, `size()`, `data()`, `get()`, `getSize()` are inline
(inherited from `std::vector<double>` or defined in the header). The integrands
(`perf_problems.hpp`: `expSum`, `uqIntegrand`, `sinProd`, `cosProd`) only use `x.size()` and
`x[k]` → **no out-of-line call per point**. `operator new/delete` are exported by base because
`base/src/sgpp/base/tools/AlignedMemory.cpp` **replaces the global allocation functions** for the
whole process (every `new`, incl. `std::vector`, goes to `posix_memalign(64)` / `free`).

PLT call sites in `libsgppcombigrid.so`, classified by how often they run:

| symbol | sites | per point / row | per tensor grid | per thread / setup |
|---|---|---|---|---|
| `DataVector(size_t)` | 71 | none | generation `genTGData` (sg 7, dsg 7), `NodeGenFunc::genNodes*` (6), 1D quad rules `genWeights` (6), interp `interpolateFirstDim/LaterDim` (3, per TG × query point), `TensorGrid::getGridPoint*` returning variants (3, not on operator paths) | `QuadratureScratch` (7), `DeltaQuadratureScratch` (6), `LinInterpScratch` (1), `SparseGridBase` ctor/resize (21) |
| `DataVector()` / `(init_list)` | 3 / 2 | none | CC `genWeights` n ≤ 1 | DSG `comp1DWeights` |
| `DataMatrix(size_t, size_t)` | 1 | none | – | `TensorGrid::getGridPoints()` (not used by operators) |
| `operator new` / `delete` | 204 / 430 | none | every per-TG container: SG `getWeights` (`resize`), CC `genWeights` temporary + Eigen FFT, interp `nodes`/`results`/`funcValues` per dim, DSG `ProjectedSubTrieBuilder`, generation (TensorGrid, MI vectors) | scratch, tries, generation instructions |

Per point the pipelines call `SourceFunc::evaluate` (combigrid, direct call since WP-13) →
`std::function` → integrand (benchmark binary); no base symbol on that path.

## 2. Share in profiles (16 threads, top case per group, high d first; self samples)

| case | samples | `libsgppbase.so` | base PLT stubs | libc allocator (malloc/free/memalign family) | libc `memset`/`memmove` |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 326697 | 0.001 % | 0.000 % | 0.011 % | 1.670 % |
| quad/dsg/equi-exp/b1/d20/L6 | 55891 | 0.077 % | 0.009 % | 0.798 % (memalign 0.152 %) | 1.084 % |
| quad/sg/equi-sin/b0/d7/L7 | 117893 | 0.001 % | 0.000 % | 0.006 % | 0.002 % |
| quad/sg/equi-exp/b1/d20/L5 | 12958 | 0.023 % | 0.015 % | 0.833 % (memalign 0.185 %) | 0.000 % |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 18012 | 0.017 % | 0.006 % | 0.189 % (memalign 0.077 %) | 0.000 % |
| interp/sg/cc/b0/d10/L2/p8 | 92380 | 0.000 % | 0.000 % | 0.003 % | 0.116 % |
| interplin/sg/equi/b1/d10/L5/p32 | 120472 | 0.000 % | 0.000 % | 0.007 % | 0.001 % |
| quad/sg/cc-uq/b0/d2/L20 | 29499 | **2.176 %** (`DataVector(n, v)` 2.173 %) | 0.000 % | 0.003 % | 1.377 % |
| quad/sg/equi-exp/b0/d2/L20 | 16553 | **3.027 %** (`DataVector(n, v)` 3.027 %) | 0.000 % | 0.018 % | 0.054 % |

- At d ≥ 7 base + its PLT stubs stay ≤ 0.09 % of the samples in every group.
- The only visible base share is `DataVector(size_t, double)` at sg d2: the body `assign(n, 0.0)`,
  i.e. the zero-fill of the per-thread `QuadratureScratch` vectors (sized by
  `getMaxTGSumOverGPCntsPerDim()`, ≈ 2^21 at L20) plus the first-touch page faults (`[unknown]`
  = kernel, 19.6 / 23.4 % of these two profiles). Inlining moves the fill into combigrid but
  does not remove it (→ WP-05: scratch on demand / without zero-fill).
- The allocator calls into base (`operator new` → `posix_memalign`) are per tensor grid; the
  libc allocator share is ≤ 0.83 % even in the generation-heavy b1 d20 cases, the
  memalign-specific part ≤ 0.19 %.
