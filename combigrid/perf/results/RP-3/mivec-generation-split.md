# RP-3 — mivec: operator vs. generation, and the split of the generation

## Operator vs. generation (`e2e.md`, rp-3 medians, 16 threads)

| case | total | generation | operator |
|---|---|---|---|
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 742.6 ms | 3.0 ms | 739 ms |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 483.6 ms | 7.5 ms | 475 ms |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 27.5 ms | 2.4 ms | 25 ms |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 3.1 ms | 0.8 ms | 2 ms |
| **sum** | **1.257 s** | **13.8 ms (1.1 %)** | **1.241 s** |

The operator is the SG quadrature (`profile-splits.md`: integrand 70–77 %, `processTensorGrid`
12–15 % at d5/d10).

## Split of the generation (`mivec_gen_split.cpp`, scratch program, not part of the build)

Same MI sets and configuration as `perf_e2e`'s `addMIVec` (`perf::anisotropicMIs(d, n, S,
2000 + d)`, CC nodes, boundary offset 1), linked against the `rp-3` libraries; median of 21 (16
threads) / 11 (1 thread) warm repetitions in one process. Stages:
- closure = `LvlMIVec::downwardsClosure()` (WP-03);
- CT coefficients = `tools::computeCTCoeffs(closure)`;
- reduced = `MIVecSGGenInstr::genReducedMIVecWithCoeffs()` = closure + coefficients + dropping
  the zero coefficients (`moveMI` loop, `shrink_to_fit`) + `partitionForExtensiveSG`;
- SparseGrid = `SparseGrid(genInstr)` (= the e2e `generation` phase);
- materialization = SparseGrid − reduced (tensor grids: node lists, counts, struct entries).

16 threads (`OMP_PROC_BIND=close OMP_PLACES=threads`):

| case | MIs in | closure | tensor grids | closure | CT coefficients | filter + partition | materialization | SparseGrid (e2e generation) |
|---|---|---|---|---|---|---|---|---|
| d5/S16/n1000 | 1000 | 5592 | 777 | 0.51 ms (17 %) | 1.45 ms (49 %) | 0.52 ms (18 %) | 0.47 ms (16 %) | 2.96 ms (e2e 3.0) |
| d10/S12/n1000 | 1000 | 14384 | 2724 | 0.74 ms (10 %) | **4.82 ms (64 %)** | 1.38 ms (18 %) | 0.61 ms (8 %) | 7.56 ms (e2e 7.5) |
| d15/S10/n300 | 300 | 4066 | 516 | 0.19 ms (9 %) | 1.52 ms (71 %) | 0.35 ms (16 %) | 0.10 ms (5 %) | 2.16 ms (e2e 2.4) |
| d20/S7/n300 | 300 | 1026 | 353 | 0.08 ms (11 %) | 0.52 ms (70 %) | 0.09 ms (13 %) | 0.05 ms (6 %) | 0.74 ms (e2e 0.8) |

1 thread (`OMP_PLACES={4}`): CT coefficients 2.5 / 14.8 / 5.8 / 2.0 ms, closure 0.47 / 0.67 /
0.16 / 0.07 ms, SparseGrid 6.7 / 20.2 / 6.6 / 2.5 ms. The CT coefficients scale only 1.7–3.8x
from 1 to 16 threads (d10: 14.8 → 4.8 ms), the materialization 6–9x.

Assessment: the whole mivec generation is 13.8 ms = 0.036 % of the suite (38.45 s); CT
coefficients ≈ 8.3 ms of it. No WP is justified by the suite. The closure (WP-03) is ≤ 0.74 ms.
For dimension-adaptive refinement (owner, later), the incremental CT-coefficient update is the
part that would matter (computed from scratch for all MIs today).
