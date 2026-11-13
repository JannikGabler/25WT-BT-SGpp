# WP-52 — placement and bitwise checks (A = WP-52-before, B = WP-52-after)

## Bitwise results
`perf_reference` (all 132 cases) run directly from both snapshots:
- 1 thread: output files byte-identical (`cmp`).
- 16 threads: A vs B differ in ~70–80 cases, but so do A vs A and B vs B (3 runs each: 69–81
  differing cases between any two runs): reassociation from the dynamic OpenMP scheduling of the
  reductions, not a change. `perf.py refcheck` gives the same verdict for both
  (131 OK, 1 WARN `interp/sg/equi/b0/d2/L5/cos-shifted` x2.34, already present in WP-52-before).

## Kernel .text (nm -S, libsgppcombigrid.so; size in bytes, start offset mod 64)
| function | A | B |
|---|---|---|
| sumOverRows<ScalarValues> | 1821 @32 | 1821 @48 |
| sumOverRows<VectorValues> | 3913 @0 | 3913 @16 |
| forEachGPWithGlobalWeight (4 instantiations) | 5 / 29 / 3900 / 4557 @0 | same @0 |
| prepareDeltaTensorGrid | 2427 @16 | 2427 @48 |
| compRowWeights / compSubTrie / processDeltaTensorGrid | 1082 / 85 / 400+98 | same |
| mapNodesOntoDomain / setUpIterDims / updateWeightProducts | 1104 / 1419 / 444 | same |
| ProjectedSubTrieBuilder::build | 15051 @48 | 15003 @32 |
| global_interpolation::processTensorGrid (scratch overloads) | 706 / 596 | 674 / 566 |
| mapNodeIdx → mapDeltaNodeIdxToRegular | 92 @48 | 92 @48 |
| total .text | 518674 | 518706 |

## Op-cache counters of the two SLOWER cases (perf stat, 3 alternating processes each)
- micro `interp/sg/cc/b0/d8/L2` (1 thread): instructions 8.833e9 / 8.834e9, cycles 3.556e9 →
  3.620e9 (+1.8 %), op-cache accesses 1.681e9 → 1.720e9 (+2.4 %), op-cache misses 5.72e6 → 6.21e6
  (+9 %). Control `interp/sg/cc/b0/d5/L5`: instructions equal, cycles −0.2 %, op-cache misses −3 %.
- e2e `interp/sg/cc/b0/d10/L2/p8` (16 threads): instructions 2.541e11 both, cycles 1.491e11 →
  1.510e11 (+1.2 %), op-cache accesses 4.107e10 → 4.156e10 (+1.2 %), op-cache misses 1.25e8 →
  1.50e8 (+20 %).
Identical instruction counts with more op-cache misses: code placement (the scalar row loop
starts 16 bytes later in its 64-byte line), not the refactoring itself.
