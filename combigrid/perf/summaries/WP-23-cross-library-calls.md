# WP-23 — Cross-library calls into `libsgppbase` (`base::DataVector`) — measurement

Status: NO-GAIN · Date: 2026-10-29 · Base commit: 0273aa1d7 (+ owner's uncommitted `type_defs.hpp` comment, no codegen effect)
Recommendation: reject options (b)/(c) (inlining upper bound within noise); owner decision on the new allocator finding (follow-up below)
Machine: `perf.py calibrate` ratios e2e 1.002 / 0.999, micro 0.997 / 1.001 (burst / sustained)

## Change
- No source change kept. Measured: (1) inventory of base symbols, (2) DSO-split profiles, (3) upper
  bound V1 = the 4 imported `DataVector` constructors inline in `DataVector.hpp` (bodies unchanged),
  (4) extra variant V2 = base's global `operator new/delete` replacement (`AlignedMemory.cpp`,
  `posix_memalign(64)`) compiled out. Both reverted; rebuilt HEAD libs byte-identical to WP-23-before.
- Inventory (`results/WP-23/inventory-and-shares.md`): combigrid imports only `DataVector()`,
  `DataVector(n)`, `DataVector(init_list)`, `DataMatrix(r,c)` and the global `operator new/delete`
  (base replaces them for the whole process). No call site is per point or per row: scratch per
  thread, node/weight vectors per tensor grid, `DataMatrix` only in `getGridPoints()`. The
  integrands use only `x.size()`/`x[k]` (inline `std::vector` members) → **no base call per point**.
- Files: none (only `combigrid/perf/results/WP-23/`, this summary).

## Gain vs. cost
- Share (16 thr, top case per group): `libsgppbase` + base PLT stubs ≤ 0.09 % at d ≥ 7 in all groups;
  only sg d2 shows `DataVector(n, 0.0)` at 2.2 / 3.0 % (cc-uq / equi-exp L20) = zero-fill of the
  per-thread scratch + page faults, not call overhead (→ WP-05).
- Upper bound V1 (inline ctors): e2e sum 38.423 → 38.462 s (0.999x), all totals within noise except
  interp/cheb1/d5 1.018x (same instructions −0.04 %, op-cache misses 4.29 → 2.67 % = code placement)
  and mivec d15 1.019x (CI [1.000, 1.037], identical counters). Micro sum 1.000x.
- Options: (a) combigrid-side raw `data()`/`size()`: already the state of all hot paths → 0.
  (b) inline members in the base header: upper bound within noise; costs a base ABI change and slows
  the `TensorGridMutableIterator` micro by 6 % (its design relies on opaque ctors). (c) static /
  cross-module LTO: bounded by (b) + `operator new` call overhead (≤ 0.034 % self, ≤ 0.015 % PLT) ≈ 0;
  not built (snapshots expect `.so`; pysgpp needs shared libs).
- Memory: V1 unchanged (below); V2 see "Attempts not kept". API / functionality, maintainability: none.

## Tests
- Pinning tests: existing suite (no source change kept). Results: normal `test --repeat 3` ✓ on
  HEAD, V1, V2 · debug n.a. (no kept change) · ASan n.a.

## Reference check
- `perf.py refcheck`: OK (1 and 16 threads) for WP-23-before, V1 (`refcheck.txt`), V2 (`refcheck-nonew.txt`).

## Benchmarks (A = WP-23-before, B = WP-23-after = V1; speedup = A/B, > 1 is faster)
L1 micro (1 thread; all 23 kernels that construct `DataVector`s, `micro.md`; the SLOWER rows run
the same instructions within 0.1 %, the iterator +0.8 %, `perfstat-micro-raw.txt`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| gen/sparse_grid/cc/b1/d20/L5 | 91.96 ms | 90.85 ms | 1.013 | [1.003, 1.022] |
| dsg/process_delta_tg/equi/b0/d10/L7 | 66.69 ms | 66.36 ms | 1.002 | [0.991, 1.013] |
| quad/process_tg/cc/b0/d2/L16 | 25.38 ms | 26.09 ms | 0.972 | [0.959, 0.987] |
| quadrule/cc_weights/n32769 | 4.35 ms | 4.48 ms | 0.973 | [0.960, 0.986] |
| tg/iterate_mutable/equi/b0/d10/L4 | 8.38 ms | 8.94 ms | 0.937 | [0.928, 0.947] |

L2/L3 e2e (16 threads, full suite, `e2e.md`), largest cases:

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.776 s | 6.781 s | 1.000 | [0.994, 1.006] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.403 s | 5.398 s | 1.001 | [0.997, 1.005] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.472 s | 2.482 s | 0.992 | [0.970, 1.016] |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.415 s | 2.414 s | 1.000 | [0.992, 1.008] |
| quad/sg/equi-exp/b0/d2/L20 | operator | 337.2 ms | 325.8 ms | 1.041 | [0.931, 1.163] |

Sum of medians (38 cases): A = 38.423 s, B = 38.462 s, saved = −0.038 s (0.999x) · Reports: `combigrid/perf/results/WP-23/`

Peak memory: V1 geomean B/A = 0.998, 0 MEM± (largest interplin d10 13.1 → 13.6 MiB, within spread) · floor A 10.0, B 10.2 MiB

## Attempts not kept
- V1 (inline ctors): numbers above; within noise → reverted (measurement variant).
- V2 (no global `operator new` replacement, A/B vs WP-23-before, `e2e-nonew.md`): sum 38.498 → 39.218 s
  (0.982x); memory geomean 0.952, **16 MEM−** (dsg b1 d15 151.1 → 127.4 MiB, d20 217.3 → 194.0 MiB,
  sg b1 d10 25.9 → 22.4 MiB); generation 1.1–1.4x faster (ms phases, e.g. mivec d10 7.6 → 5.6 ms); but
  **bimodal slowdowns**: interplin d8 1.53 → 2.20 s in 3/5 rounds (re-run: 5/8 processes +20–25 %),
  interp cc d3 SLOWER 0.878, interplin d10 one round 3.0 s. `perf c2c` (IBS): in a slow process
  `computeNeighborGPWithWeight` takes 375 cross-core cache loads vs. 1 in a fast one, on 2 cache lines
  of the main-thread heap → **false sharing**. Base's 64-byte `operator new` currently protects
  combigrid's per-thread data from it (`c2c-nonew-interplin-raw.txt`). Reverted.

## Profile shift / follow-ups
- WP-04/05/17: `DataVector` is no cross-library cost on their paths; WP-05's sg d2 share is fill + page faults.
- WP-25: `SourceFunc::evaluate` is LTO-inlined into all kernels (per point only the `std::function`
  indirect call). Moving `SourceFunc` (or a per-point cache lookup) into base keeps this only if
  those functions stay header-inline; otherwise every point gets a non-inlinable cross-library call.
- NEW (proposal): cache-line-align combigrid's per-thread scratch/shared query data itself
  (independent of base's allocator; MinGW builds already compile the replacement out). This is the
  prerequisite before the owner weighs dropping or limiting the replacement (−5 % memory geomean,
  −23 MiB at dsg b1; datadriven's aligned AVX loads rely on it). The objects on the 2 lines are not identified yet.
- Do not retry: header-inline `DataVector` ctors / static cross-module LTO for speed (bound ≤ noise).

## Revert
Nothing to revert in source (`git status`: only `results/WP-23/` and this summary are new).
