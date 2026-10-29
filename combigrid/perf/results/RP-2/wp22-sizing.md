# RP-2 — sizing of WP-22 (tensor grids of a sparse grid as struct of arrays)

Current layout: `SparseGridBase::tensorGridData` = `std::vector<TensorGridCTData>`.
- Each entry is 112 B inline: `LvlMI mi` 24 + `CTCoeffType coefficient` 4 + padding 4 +
  `TensorGrid` 80 (`nGP_` 8, `GPMI nGPPerDim` 24, `base::DataVector nodesPerDim` 48 =
  `std::vector<double>` + the empty Kahan `correction` vector).
- Each entry owns **three heap allocations**: the level MI (4d B), the per-dimension counts (4d B)
  and the node list (8 Σ_k n_k B).
- `genTGData` copies every MI out of the generated `LvlMIVec` (`LvlMI mi = mis[miIdx]`).

## Memory (heaptrack, rp-2, 16 threads; `heap/*.txt`; MB = 1e6 B, requested sizes)

| case (peak RSS / heap peak) | entries | struct array | level MIs | counts (GPMI) | node lists | allocations of the grid |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 (217 MiB / 178.4 MB) | 230 230 | 25.8 MB | 18.4 MB (80 B each, glibc chunk 96 B) | 18.4 MB | 63.8 MB (avg 277 B) | 690 690 of 692 268 in the process |
| quad/dsg/equi-exp/b1/d15/L7 (151 MiB / 111.2 MB) | 170 544 | 19.1 MB | 10.2 MB (60 B, chunk 80 B) | 10.2 MB | 43.0 MB | ≈ 511 600 of 513 364 |
| quad/sg/equi-exp/b1/d20/L5 (57 MiB / 35.8 MB) | 53 130 | 6.0 MB | 4.3 MB | 4.3 MB | 16.8 MB | 159 390 of 159 969 (+ the generated `LvlMIVec` 4.3 MB, transient) |
| quad/sg/{equi-exp,cc-uq}/b0/d2/L20 (620 / 1072 MiB) | 41 | < 0.01 MB | – | – | 100.6 MB | 123 |
| mivec/sg/cc-exp/b1/d20/S7/n300 (12.3 MiB / 0.43 MB) | small | – | – | – | – | – (nothing material) |


When the peaks occur: b1 d20 and d15 peak inside the operator's `CTCoeffTrie` construction.
- The DSG holds 126.4 MB (d20) / 82.5 MB (d15).
- The construction transient adds 51.7 / 28.5 MB: the column-major level copy 18.4 / 10.2 MB
  (`copyNonZeroCoeffTGs`); the node `FlatNestedVec` during its last doubling, 16.8 + 8.4 MB at d20;
  the `order` arrays 3.7 + 2.6 MB; the coefficients 1.8 MB.
- RSS − heap ≈ 47 / 45 MiB: process floor 10.1 MiB + thread stacks and arenas + glibc chunk
  overhead of the ≈ 3 small allocations per entry, ≈ 11 MB at d20.

Struct-of-arrays layout:
- levels: one `LvlMIVec`, which can take over the generated vector without a copy;
- coefficients: one `std::vector<CTCoeffType>`;
- grid points: `nGP` and a node offset (8 + 8 B per entry); counts either derived from
  (level, `Lvl2GPCntFunc`, boundary offset) or stored flat (4d B);
- nodes: one flat array, or shared per (dim, level) = WP-18.

| case | now (incl. ≈ 16–20 B glibc overhead per allocation) | SoA, counts derived, nodes flat | SoA, counts stored | SoA + WP-18 (shared nodes) |
|---|---|---|---|---|
| dsg b1 d20 | ≈ 137 MB | 86.9 MB (**−50 MB**) | 105.3 MB (−32 MB) | 23–41 MB (**−96…−114 MB**) |
| dsg b1 d15 | ≈ 92 MB | 56.6 MB (−36 MB) | 66.8 MB (−26 MB) | 14–24 MB (−68…−78 MB) |
| sg b1 d20 L5 | ≈ 34 MB | ≈ 22 MB (−12 MB) | ≈ 26 MB (−8 MB) | ≈ 5–9 MB (−25…−29 MB) |
| sg b0 d2 L20 | 101 MB | 101 MB (nil) | 101 MB | ≈ 0.05 MB (−101 MB, WP-18 alone) |

The b1 peaks would also lose part of the trie transient if the trie read the levels from the
contiguous `LvlMIVec`. The column copy is kept for the radix pass, but could be freed before the
node arrays grow; with exact node reservation that is −8…−25 MB at d20.

## Runtime (code that walks the tensor-grid list)

| place | measured | SoA effect |
|---|---|---|
| Serial DSG preparation, b1 d20 (`micro/dsg/prepare`, 65.7 ms/call = 5.4 % of the wall time) | level copy from `TensorGridCTData` (`copyNonZeroCoeffTGs`, one pointer per `mi`) 8.6 % of the samples, 22.4 % of the cache misses; `comp1DWeights` (max-level scan through `mi`) 7.9 % / 18.0 %; together ≈ 18 ms per call | contiguous levels: ≈ 2–4x on these scans → −10…−15 ms (d20), −6…−8 ms (d15) |
| Generation (parallel; warm `generation` d20 61 ms, d15 43 ms, sg b1 d20 L5 17.5 ms) | 3 allocations per tensor grid; malloc/free ≈ 18 % of `micro/gen/sparse_grid/cc/b1/d20/L5`; ≈ half of the warm d20 generation is page faults (`generation-page-faults.md`) | 1 allocation per array and −37…−50 MB less to fault in → −10…−25 ms (d20), −7…−17 ms (d15), −3…−5 ms (sg b1 d20) |
| Operators' per-tensor-grid setup | `prepareDeltaTensorGrid` 0.4 / 0.2 % (b1 d20 / d15), quad/sg `getWeights` 0.2 % (b1 d20 L5) | nil |
| CT coefficients (`mivec/ct_coeffs`: 15.6 ms/call) and the closure | work on `LvlMIVec` already | nil |

**Expected runtime saving: ≈ 0.03–0.07 s on the suite (minimal; 1–3 % of the b1 d15/d20 cases)**.
Most of it is the allocation and page-fault part of generation. The main value is memory.

## Interface (what the layout would change for users)

C++ public API (umbrella `sgpp_combigrid.hpp`):
- `TensorGridCTData` (public struct with public members `mi`, `coefficient`, `tensorGrid`).
- `SparseGridBase`, used through `SparseGrid` / `DeltasSparseGrid`:
  - `getTensorGrid(size_t) → const TensorGridCTData&`;
  - `getTensorGrid(const LvlMI&) → const_iterator` (= `std::vector<TensorGridCTData>::const_iterator`);
  - `getTensorGrids() → std::span<const TensorGridCTData>`;
  - `begin()/end()` incl. the **non-const** `iterator`, `cbegin`, `rbegin`, …, `operator==`;
  - protected `setTensorGrids(std::vector<TensorGridCTData>&&)`, `addTensorGrid`,
    `setTensorGrid`, `recomputeMetaValues` (the subclass constructors use them).
- `TensorGrid`: `getGPCntPerDim() → const GPMI&`, `getNodesPerDim() → const base::DataVector&`, the
  constructors from (`GPMI`, `DataVector`), `gridPointsMutable/Immutable()` (ranges over its
  storage). These change with derived counts or shared nodes (WP-18).
- In public headers but internal:
  - `global_interpolation::interpolateFirstDim/interpolateLaterDim(…, const TensorGridCTData&, …)`;
  - `linear_interpolation::interpolate(…, const TensorGridCTData&, …)`;
  - `tools::*_generation::genTGData → std::vector<TensorGridCTData>`;
  - `misc::CTCoeffTrie(std::span<const TensorGridCTData>)`;
  - `delta_quadrature_operator::*`.

**Protected benchmarks (owner-only)** use the entries by reference, so a layout change must stay
source-compatible with them, or the owner adapts them; the refcheck structure hash must stay
identical:
- `perf_e2e.cpp:81`: `for (const auto& tgData : grid) … tgData.tensorGrid.nGP()`.
- `perf_reference.cpp:136-144`: `for (lvl : tgData.mi)`, `tgData.coefficient`,
  `tgData.tensorGrid.nGP()`, `getGPCntPerDim()[k]`,
  `const DataVector& nodes = tgData.tensorGrid.getNodesPerDim()`.
- A proxy returned by value with exactly these member names compiles there: `const auto&` binds to
  the temporary, and `getNodesPerDim()` must return something bindable to `const DataVector&`.

Python (`combigrid/build/pysgpp/combigrid.i`):
- `TensorGridCTData` is a read-only proxy. The properties `mi` and `tensorGrid` return references
  into the struct (kept alive via `_owner`); `coefficient` is immutable.
- `SparseGrid.getTensorGrid(idx)` returns a reference + `_owner`; `getTensorGrid(mi)` returns a
  pointer or None.
- `getTensorGrids()` returns a tuple of owned copies (`%template(TensorGridCTDataVector)
  vector<TensorGridCTData>`); `__len__` / `__getitem__`.
- `TensorGrid.getGPCntPerDim()` / `getNodesPerDim()` return references.
- With SoA these become value proxies: `mi` as a copy or view, and the vector template becomes a
  vector of proxies. Observable Python behaviour can stay the same (everything is read-only
  already), but every `%extend` wrapper changes. `DeltasSparseGrid` is not bound.

Tests touching the entries: `tests/grids/{sparse_grid_base,sparse_grid_construction,deltas_sparse_grid,tensor_grid_iterator}_test.cpp`,
`tests/operators/quadrature/{quadrature,delta_quadrature}_test.cpp`.

## Merge with WP-18?

Yes. Both replace the per-entry storage of `SparseGridBase` and `TensorGrid`:
- WP-22 removes the per-entry MI / count copies and the array of structs; WP-18 removes the
  per-entry node copies.
- They change the same public accessors (`TensorGrid::getNodesPerDim()` / `getGPCntPerDim()`,
  `TensorGridCTData` by reference), the same bindings and tests, and both face the
  protected-benchmark constraint.
- Done one after the other, the public access changes twice.

Combined effect:
- **memory b1 d20 −96…−114 MB** (heap peak 178 → ≈ 65–80 MB, RSS 217 → ≈ 110–125 MiB), b1 d15
  −68…−78 MB, sg d2 L20 −101 MB, sg b1 d20 L5 −25 MB;
- runtime ≈ 0.03–0.07 s (WP-22) plus WP-18's node generation in generation (CC: ≈ 35 % of
  `micro/gen/sparse_grid/cc/b1/d20/L5`; generation phases are ≈ 0.3 s of the suite outside mivec).
