# RP-2 — front-end / op-cache measurements of the hot loops (snapshot rp-2, 16 threads)

Question (from WP-02): which hot loops are front-end-bound (op-cache misses), i.e. candidates for
`[[gnu::aligned(64)]]` like the DSG row loop? Proposals only, no source changes.

## Process-wide (`perf stat`, 1 warm-up + 2 samples; raw: `perfstat-frontend-rp-2.txt`)

| case | IPC | op-cache misses per 1000 accesses | front-end stall cycles | I-cache misses per 1e6 instructions |
|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | 2.25 | 2.79 | 0.69 % | 83.7 |
| quad/dsg/equi-exp/b1/d20/L6 | 1.80 | 15.46 | 1.43 % | 351.0 |
| quad/sg/equi-exp/b0/d10/L4 | 2.11 | 2.82 | 0.51 % | 83.5 |
| quad/sg/equi-exp/b1/d20/L5 | 1.48 | 9.99 | 0.93 % | 388.8 |
| interp/sg/cc/b0/d5/L6/p64 | 1.96 | 4.31 | 0.88 % | 123.3 |
| interp/sg/cc/b0/d10/L2/p8 | 1.71 | 3.90 | 2.82 % | 111.5 |
| interplin/sg/equi/b1/d8/L5/p256 | 1.80 | 3.34 | 2.86 % | 100.1 |
| interplin/sg/equi/b1/d10/L5/p32 | 1.66 | 3.09 | 5.65 % | 103.3 |
| mivec/sg/cc-exp/b1/d20/S7/n300 (from `mivec-rp-1-vs-rp-2.md`) | 1.94 | 2.46 | 4.08 % | 76.8 |

## Per symbol (`perf record -F 999 -e cycles,stalled-cycles-frontend,op_cache_hit_miss.{all_op_cache_accesses,op_cache_miss}`)

Caveat: these are non-precise counter samples. Skid moves samples out of a loop into the function
it calls; for example, the DSG row loop gets 28.8 % of the cycle samples here vs. 43.1 % in the
precise default profile. Per-symbol ratios are therefore smeared toward the process average.
`0x...26c76` is libgomp's spin-wait loop, `0x...a0e4c3` OpenBLAS's idle threads.

### `quad/dsg/equi-exp/b0/d10/L7`
process: front-end stalls 1.06 % of cycles, op-cache misses 6.92 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| __ieee754_exp_fma | 29.2 % | 0.70 % | 3.26 |
| void sgpp::combigrid::delta_quadrature_operator::forEachGPWithGlobalWeight<sgpp::combigrid | 28.8 % | 0.77 % | 3.44 |
| sgpp::combigrid::perf::expSum | 21.4 % | 0.74 % | 3.31 |
| exp@@GLIBC_2.29 | 11.5 % | 0.67 % | 3.24 |
| std::_Function_handler<double | 3.7 % | 0.81 % | 3.37 |
### `quad/sg/equi-exp/b0/d10/L4`
process: front-end stalls 0.84 % of cycles, op-cache misses 6.82 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| __ieee754_exp_fma | 38.8 % | 0.41 % | 2.97 |
| sgpp::combigrid::perf::expSum | 25.6 % | 0.58 % | 3.38 |
| exp@@GLIBC_2.29 | 15.2 % | 0.46 % | 3.06 |
| sgpp::combigrid::quadrature_operator::processTensorGrid | 9.6 % | 0.82 % | 3.81 |
| std::_Function_handler<double | 4.6 % | 0.62 % | 3.46 |
| 0x0000000000a0e4c3 | 2.1 % | 0.15 % | 5.24 |
### `interp/sg/cc/b0/d10/L2/p8`
process: front-end stalls 3.10 % of cycles, op-cache misses 9.89 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| __cos_fma | 66.6 % | 3.29 % | 5.55 |
| sgpp::combigrid::global_interpolation::interpolateFirstDim | 13.7 % | 1.93 % | 6.50 |
| sgpp::combigrid::perf::cosProd | 6.6 % | 3.01 % | 5.67 |
| sgpp::combigrid::global_interpolation::OptBarycentricFormula::barycentricFormula | 2.9 % | 1.75 % | 5.80 |
| 0x0000000000026c76 | 2.9 % | 0.14 % | 62.96 |
| sgpp::combigrid::global_interpolation::OptBarycentricFormula::interpolate | 1.5 % | 0.99 % | 5.62 |
| cos@plt | 1.2 % | 2.58 % | 4.98 |
| 0x0000000000a0e4c3 | 1.0 % | 0.12 % | 9.46 |
### `interp/sg/cc/b0/d5/L6/p64`
process: front-end stalls 1.25 % of cycles, op-cache misses 10.42 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| __cos_fma | 59.7 % | 1.04 % | 5.58 |
| sgpp::combigrid::global_interpolation::interpolateFirstDim | 15.4 % | 0.79 % | 5.77 |
| sgpp::combigrid::perf::cosProd | 7.2 % | 0.97 % | 5.29 |
| sgpp::combigrid::global_interpolation::OptBarycentricFormula::barycentricFormula | 4.8 % | 1.04 % | 10.06 |
| 0x0000000000026c76 | 3.1 % | 0.15 % | 80.74 |
| sgpp::combigrid::global_interpolation::OptBarycentricFormula::interpolate | 2.6 % | 1.18 % | 10.38 |
| 0x0000000000a0e4c3 | 1.2 % | 0.14 % | 7.44 |
| cos@plt | 1.1 % | 0.83 % | 5.55 |
### `interplin/sg/equi/b1/d10/L5/p32`
process: front-end stalls 5.91 % of cycles, op-cache misses 8.20 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| __cos_fma | 61.7 % | 6.03 % | 4.84 |
| sgpp::combigrid::linear_interpolation::computeNeighborGPWithWeight | 19.6 % | 6.29 % | 4.96 |
| sgpp::combigrid::perf::cosProd | 5.9 % | 6.47 % | 4.96 |
| sgpp::combigrid::linear_interpolation::interpolate | 4.5 % | 4.43 % | 4.56 |
| 0x0000000000026c76 | 2.4 % | 0.12 % | 83.67 |
| cos@plt | 1.1 % | 6.17 % | 4.73 |
### `interplin/sg/equi/b1/d8/L5/p256`
process: front-end stalls 3.20 % of cycles, op-cache misses 8.82 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| __cos_fma | 61.6 % | 2.69 % | 4.63 |
| sgpp::combigrid::linear_interpolation::computeNeighborGPWithWeight | 15.2 % | 4.59 % | 5.12 |
| sgpp::combigrid::perf::cosProd | 7.1 % | 2.88 % | 4.55 |
| sgpp::combigrid::linear_interpolation::interpolate | 4.8 % | 5.44 % | 5.40 |
| 0x0000000000026c76 | 4.2 % | 0.14 % | 88.17 |
| 0x0000000000a0e4c3 | 1.2 % | 0.11 % | 10.17 |
### `mivec/sg/cc-exp/b1/d20/S7/n300`
process: front-end stalls 4.37 % of cycles, op-cache misses 6.40 per 1000 accesses
| symbol | cycles | front-end stalls / own cycles | op-cache misses / 1000 accesses |
|---|---|---|---|
| sgpp::combigrid::MIVec<unsigned int> sgpp::combigrid::tools::genMIVecDownwardsClosure<unsi | 95.8 % | 4.16 % | 3.26 |
| 0x0000000000a0e4c3 | 1.0 % | 0.13 % | 5.18 |
