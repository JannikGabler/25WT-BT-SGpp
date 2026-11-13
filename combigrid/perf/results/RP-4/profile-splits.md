# RP-4 — profile splits (snapshot rp-4 = HEAD 6ac45a05d)

`run_profiles.sh` (perf.py profile, 16 t, self time, 999 Hz; short cases with more samples) + `../RP-3/profile_split.py` (RP-2/RP-3 inline-chain method) → `profiles/*-t16{,-split}.txt`; condensed by `summarize_profiles.py`. Shares in % of all samples of the process. integrand = perf_e2e (integrand, PLT stubs, `_M_invoke`) + libm; the row loop's inline helpers in brackets (`evaluate` / `addScaled` = per-point dispatch).

| case | outside combigrid | combigrid symbols ≥ 0.8 % (inline helpers) | combigrid total |
|---|---|---|---|
| interp_dsg_cc_b0_d10_L2_p8 | integrand 81.4 · libgomp 6.2 · OpenBLAS 3.0 · other 2.2 | `forEachGPWithGlobalWeight` 6.7 (updateCoeffSums 1.9, compRowWeights 1.6, evaluate 0.9, addScaled 0.3) | 6.8 |
| interp_dsg_cc_b0_d2_L12_p64 | integrand 40.2 · libgomp 39.2 · OpenBLAS 4.1 · libc 1.9 · other 1.3 | `forEachGPWithGlobalWeight` 9.7 (compRowWeights 2.2, updateCoeffSums 2.2, evaluate 2.2, addScaled 0.6, evaluate 0.2) · `DataVector` 0.9 | 13.4 |
| interp_dsg_cc_b0_d5_L6_p64 | integrand 72.8 · libgomp 4.0 · OpenBLAS 3.7 · other 1.7 · libc 0.9 | `forEachGPWithGlobalWeight` 13.5 (updateCoeffSums 5.0, compRowWeights 3.2, evaluate 1.6, addScaled 0.5) · `build` 1.6 | 16.9 |
| interp_dsg_cc_b0_d7_L4_p16 | integrand 79.0 · OpenBLAS 4.4 · libgomp 3.1 · other 1.9 · libc 0.5 | `forEachGPWithGlobalWeight` 10.2 (updateCoeffSums 3.2, compRowWeights 2.3, evaluate 1.4, addScaled 0.4) | 11.1 |
| interp_dsg_equi_b0_d3_L5_p64 | libgomp 56.6 · integrand 22.7 · OpenBLAS 5.4 · other 1.9 · libc 1.2 | `forEachGPWithGlobalWeight` 3.8 (updateCoeffSums 1.4, compRowWeights 1.0, evaluate 0.5) · `barycentricFormulaWeights` 2.9 · `build` 2.6 | 11.9 |
| interp_sg_cc_b0_d10_L2_p8 | integrand 88.9 · libgomp 4.5 · other 1.6 · OpenBLAS 1.3 | `sumOverRows` 3.6 (evaluate 0.7, updateWeightProducts 0.3, addScaled 0.3) | 3.6 |
| interp_sg_cc_b0_d2_L12_p64 | integrand 45.2 · libgomp 43.2 · OpenBLAS 4.2 · other 1.3 | `sumOverRows` 4.8 (evaluate 2.6, addScaled 0.4) | 5.9 |
| interp_sg_cc_b0_d5_L6_p64 | integrand 86.9 · libgomp 4.6 · OpenBLAS 1.8 · other 0.8 | `sumOverRows` 5.7 (evaluate 1.9, updateWeightProducts 0.4, addScaled 0.4, evaluate 0.3) | 5.8 |
| interp_sg_cc_b0_d7_L4_p16 | integrand 88.2 · libgomp 3.7 · OpenBLAS 2.1 · other 1.2 | `sumOverRows` 4.8 (evaluate 1.2, updateWeightProducts 0.4, addScaled 0.4, evaluate 0.2) | 4.8 |
| interp_sg_cheb1_b0_d5_L4_p64 | integrand 73.9 · libgomp 13.5 · OpenBLAS 4.5 · other 1.7 | `sumOverRows` 5.3 (evaluate 1.5, addScaled 0.5, updateWeightProducts 0.4, evaluate 0.3) | 6.0 |
| interplin_dsg_equi_b1_d10_L5_p32 | integrand 44.6 · libgomp 8.9 · OpenBLAS 2.4 · other 1.8 | `gatherCorners` 20.4 · `markCorners` 17.2 · `evaluateMarked` 3.8 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 0.8 | 42.2 |
| interplin_dsg_equi_b1_d2_L16_p16384 | OpenBLAS 32.2 · libgomp 30.6 · integrand 7.3 · other 7.1 · libc 1.3 | `evaluateMarked` 4.9 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 4.4 · `gatherCorners` 3.6 · `markCorners` 3.3 · `prepareDimension` 1.6 · `lowerBoundFrom` 1.5 | 21.3 |
| interplin_dsg_equi_b1_d5_L8_p1024 | integrand 21.0 · libgomp 20.3 · OpenBLAS 3.3 · other 3.2 | `markCorners` 22.5 · `gatherCorners` 20.4 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 5.2 · `evaluateMarked` 3.3 | 51.7 |
| interplin_dsg_equi_b1_d8_L5_p256 | integrand 39.2 · libgomp 12.4 · OpenBLAS 3.2 · other 2.0 | `gatherCorners` 20.0 · `markCorners` 17.5 · `evaluateMarked` 4.0 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 1.4 | 43.0 |
| interplin_sg_equi_b1_d10_L5_p32 | integrand 86.8 · libgomp 4.5 · other 1.4 · OpenBLAS 1.0 | `sumOverCorners` 6.0 | 6.2 |
| interplin_sg_equi_b1_d5_L8_p1024 | integrand 72.7 · libgomp 10.1 · OpenBLAS 3.3 · other 1.5 | `sumOverCorners` 8.2 · `getNeighborsForLinInterpolation` 2.1 · `prepareCorners` 1.8 | 12.3 |
| interplin_sg_equi_b1_d8_L5_p256 | integrand 84.0 · libgomp 5.9 · OpenBLAS 1.7 · other 1.2 | `sumOverCorners` 6.5 | 7.1 |
| mivec_sg_cc_exp_b1_d10_S12_n1000 | integrand 73.5 · libgomp 8.2 · OpenBLAS 4.9 · other 1.2 | `sumOverRows` 11.6 (evaluate 6.7, addScaled 1.1, updateWeightProducts 0.2) | 11.9 |
| mivec_sg_cc_exp_b1_d5_S16_n1000 | integrand 78.1 · libgomp 3.1 · OpenBLAS 2.9 · other 1.2 | `sumOverRows` 14.6 (evaluate 9.2, addScaled 1.6) | 14.7 |
| quad_dsg_cc_uq_b0_d10_L7 | integrand 63.9 · libc 1.6 · other 0.8 | `forEachGPWithGlobalWeight` 32.4 (updateCoeffSums 10.6, compRowWeights 8.3, evaluate 3.9, addScaled 1.4, evaluate 0.5) | 33.0 |
| quad_dsg_cc_uq_b0_d7_L10 | integrand 59.0 · libgomp 1.8 · OpenBLAS 1.6 · libc 1.5 · other 1.0 | `forEachGPWithGlobalWeight` 33.6 (updateCoeffSums 11.2, compRowWeights 8.3, evaluate 4.2, addScaled 1.5, evaluate 0.6) | 35.0 |
| quad_dsg_equi_exp_b0_d10_L7 | integrand 52.5 · libc 2.2 · other 0.7 | `forEachGPWithGlobalWeight` 43.0 (updateCoeffSums 14.1, compRowWeights 11.1, evaluate 5.3, addScaled 1.6, evaluate 0.5) | 43.8 |
| quad_dsg_equi_exp_b0_d3_L16 | integrand 39.1 · libgomp 23.0 · OpenBLAS 5.9 · other 3.0 · libc 1.3 | `forEachGPWithGlobalWeight` 25.2 (compRowWeights 6.2, updateCoeffSums 6.0, evaluate 5.1, addScaled 1.6, evaluate 0.5) · `DataVector` 1.5 | 27.7 |
| quad_dsg_equi_exp_b0_d7_L10 | integrand 49.8 · libc 2.1 · libgomp 1.8 · OpenBLAS 1.8 · other 1.1 | `forEachGPWithGlobalWeight` 41.5 (updateCoeffSums 13.6, compRowWeights 10.7, evaluate 5.3, addScaled 1.8, evaluate 0.6) | 43.4 |
| quad_dsg_equi_exp_b1_d15_L7 | integrand 54.8 · libgomp 2.7 · other 2.6 · OpenBLAS 2.2 · libc 1.6 | `forEachGPWithGlobalWeight` 25.0 (compRowWeights 6.8, updateCoeffSums 5.3, evaluate 4.6, addScaled 1.2, evaluate 0.3) · `addChildren` 8.5 · `build` 0.9 | 36.1 |
| quad_dsg_equi_exp_b1_d20_L6 | integrand 52.1 · other 4.3 · libgomp 2.9 · OpenBLAS 2.1 · libc 1.7 | `forEachGPWithGlobalWeight` 20.9 (compRowWeights 6.0, updateCoeffSums 4.7, evaluate 3.5, addScaled 1.1, evaluate 0.3) · `addChildren` 12.4 · `build` 1.2 | 36.8 |
| quad_sg_cc_uq_b0_d10_L4 | integrand 79.8 · OpenBLAS 1.6 · other 1.4 · libgomp 0.9 | `sumOverRows` 16.3 (evaluate 4.8, updateWeightProducts 1.3, addScaled 1.2, evaluate 1.0) | 16.3 |
| quad_sg_cc_uq_b0_d2_L20 | integrand 39.0 · other 23.3 · OpenBLAS 4.9 · libgomp 4.7 · libc 1.4 | `kiss_cpx_fft` 14.5 · `sumOverRows` 5.1 (evaluate 3.1, addScaled 0.6) · `genWeightsFFT` 3.3 · `DataVector` 2.1 | 26.7 |
| quad_sg_equi_exp_b0_d10_L4 | integrand 72.7 · OpenBLAS 2.3 · other 1.1 · libgomp 1.1 | `sumOverRows` 22.7 (evaluate 6.3, addScaled 1.9, updateWeightProducts 1.8, evaluate 1.4) | 22.7 |
| quad_sg_equi_exp_b0_d2_L20 | integrand 53.6 · other 16.0 · libgomp 8.0 · OpenBLAS 5.9 | `sumOverRows` 10.7 (evaluate 6.4, addScaled 1.1) · `DataVector` 4.2 | 16.4 |
| quad_sg_equi_exp_b1_d20_L5 | integrand 70.2 · libgomp 6.2 · OpenBLAS 4.0 · other 2.2 · libc 1.2 | `sumOverRows` 14.0 (evaluate 4.8, evaluate 1.7, addScaled 0.9, updateWeightProducts 0.8) | 16.0 |
| quad_sg_equi_sin_b0_d7_L7 | integrand 92.2 · OpenBLAS 0.9 · other 0.8 · libgomp 0.5 | `sumOverRows` 5.6 (evaluate 2.1, addScaled 0.5, evaluate 0.4, updateWeightProducts 0.3) | 5.6 |
