# WP-29b: placement of the scalar DSG row loop, variant V1 vs. V2 (final)

`perf stat -e cycles,instructions,op_cache_hit_miss.op_cache_miss` of
`perf_e2e --filter '^quad/dsg/equi-exp/b0/d10/L7$' --samples 2 --warmups 1` (16 threads,
`OMP_PROC_BIND=close OMP_PLACES=threads`), two runs per snapshot, interleaved.

V1 = `updateCoeffSums` defined in the .cpp (as before the WP; snapshot `WP-29b-v1b`); V2 = defined
header-inline (final, snapshot `WP-29b-v2`, library identical in its DSG kernel to `WP-29b-after`).

| snapshot | cycles | instructions | op-cache misses |
|---|---|---|---|
| WP-29b-before | 516.2 G / 517.2 G | 1169.5 G / 1169.5 G | 380.3 M / 374.7 M |
| V1 | 523.8 G / 522.3 G (+1.2 %) | 1167.8 G / 1167.8 G (−0.15 %) | 414.1 M / 410.7 M (+9 %) |
| WP-29b-before | 515.7 G / 515.9 G | 1169.5 G / 1169.5 G | 386.1 M / 376.2 M |
| V2 | 516.2 G / 516.0 G (+0.04 %) | 1169.6 G / 1169.5 G | 387.2 M / 380.0 M (+0.6 %) |

Code (objdump of `forEachGPWithGlobalWeight<…ScalarValues…>`): before 3900 B; V1 3308 B (the call
`updateCoeffSums(scratch, nProjDepths - 1)` before the row loop is no longer inlined, the per-row
call still is); V2 3900 B, instruction-identical to before (diff of the normalized disassembly: 0
lines), start 64-byte aligned (0x43080). Side effect in V1: `ProjectedSubTrieBuilder::childBlock`
was no longer inlined into `addChildren` (`micro/dsg/sub_tries/equi/b0/d10/L7/s16` 0.975 SLOWER,
`micro-v1.md`; +1.9 % instructions); in V2 it is inlined again (calls only from `build()`, as before).

V1 e2e `^quad/dsg/` (`e2e-v1.md`): 0.992x, 5 SLOWER (b0 1.0–1.4 %), b1 within noise.
V2 e2e (`e2e.md`): 1.001x, 0 SLOWER.
