| interp_dsg_cc_b0_d10_L2_p8 | integrand 81.5 · libgomp 5.7 · OpenBLAS 3.0 · other 2.0 | `forEachGPWithGlobalWeight` 7.2 (updateCoeffSums 2.0, compRowWeights 1.7, evaluate 1.0, addScaled 0.3) | 7.3 |
| interp_dsg_cc_b0_d2_L12_p64 | integrand 39.7 · libgomp 39.6 · OpenBLAS 4.1 · libc 1.9 · other 1.3 | `forEachGPWithGlobalWeight` 10.5 (compRowWeights 2.8, evaluate 2.3, updateCoeffSums 2.3, addScaled 0.7) | 13.3 |
| interp_dsg_cc_b0_d5_L6_p64 | integrand 71.7 · libgomp 5.0 · OpenBLAS 3.8 · other 1.4 · libc 0.9 | `forEachGPWithGlobalWeight` 14.1 (updateCoeffSums 5.2, compRowWeights 3.5, evaluate 1.6, addScaled 0.5) · `build` 1.4 | 17.3 |
| interp_dsg_cc_b0_d7_L4_p16 | integrand 78.9 · OpenBLAS 4.4 · libgomp 2.6 · other 1.9 · libc 0.5 | `forEachGPWithGlobalWeight` 10.7 (updateCoeffSums 3.6, compRowWeights 2.5, evaluate 1.3, addScaled 0.3) | 11.5 |
| interp_dsg_equi_b0_d3_L5_p64 | libgomp 55.8 · integrand 23.5 · OpenBLAS 5.2 · other 2.1 · libc 1.2 | `forEachGPWithGlobalWeight` 4.4 (updateCoeffSums 1.5, compRowWeights 1.2, evaluate 0.5) · `barycentricFormulaWeights` 2.7 · `build` 2.3 | 12.0 |
| interp_sg_cc_b0_d10_L2_p8 | integrand 88.8 · libgomp 4.7 · other 1.4 · OpenBLAS 1.4 | `sumOverRows` 3.8 (evaluate 0.8, updateWeightProducts 0.3, addScaled 0.3) | 3.8 |
| interp_sg_cc_b0_d2_L12_p64 | integrand 45.6 · libgomp 43.5 · OpenBLAS 4.0 · other 1.2 | `sumOverRows` 4.6 (evaluate 2.4, addScaled 0.4) | 5.5 |
| interp_sg_cc_b0_d5_L6_p64 | integrand 87.3 · libgomp 4.1 · OpenBLAS 1.8 · other 1.0 | `sumOverRows` 5.7 (evaluate 1.8, addScaled 0.5, updateWeightProducts 0.5, evaluate 0.3) | 5.8 |
| interp_sg_cc_b0_d7_L4_p16 | integrand 87.7 · libgomp 3.8 · OpenBLAS 2.1 · other 1.4 | `sumOverRows` 5.0 (evaluate 1.2, addScaled 0.4, updateWeightProducts 0.4, evaluate 0.3) | 5.0 |
| interp_sg_cheb1_b0_d5_L4_p64 | integrand 74.5 · libgomp 12.9 · OpenBLAS 4.6 · other 1.8 | `sumOverRows` 5.1 (evaluate 1.6, updateWeightProducts 0.4, addScaled 0.3, evaluate 0.3) | 5.9 |
| interplin_dsg_equi_b1_d10_L5_p32 | integrand 45.2 · libgomp 7.5 · OpenBLAS 2.9 · other 1.6 | `gatherCorners` 20.7 · `markCorners` 17.3 · `evaluateMarked` 4.1 | 42.9 |
| interplin_dsg_equi_b1_d2_L16_p16384 | libgomp 32.3 · OpenBLAS 32.0 · integrand 7.4 · other 5.3 · libc 1.7 | `sgpp::combigrid::tools::dsg_lin_interpolation::` 4.5 · `gatherCorners` 4.1 · `evaluateMarked` 3.8 · `markCorners` 3.6 · `prepareDimension` 2.1 · `lowerBoundFrom` 1.4 | 21.0 |
| interplin_dsg_equi_b1_d5_L8_p1024 | integrand 22.3 · libgomp 19.8 · other 3.7 · OpenBLAS 3.5 | `markCorners` 22.6 · `gatherCorners` 20.0 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 4.6 · `evaluateMarked` 3.0 | 50.5 |
| interplin_dsg_equi_b1_d8_L5_p256 | integrand 37.4 · libgomp 15.8 · OpenBLAS 3.0 · other 2.1 | `gatherCorners` 19.3 · `markCorners` 17.0 · `evaluateMarked` 4.0 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 1.2 | 41.6 |
| interplin_sg_equi_b1_d10_L5_p32 | integrand 88.1 · libgomp 3.2 · other 1.3 · OpenBLAS 1.2 | `sumOverCorners` 6.0 | 6.2 |
| interplin_sg_equi_b1_d5_L8_p1024 | integrand 73.0 · libgomp 8.9 · OpenBLAS 3.3 · other 2.2 | `sumOverCorners` 8.5 · `prepareCorners` 3.9 | 12.6 |
| interplin_sg_equi_b1_d8_L5_p256 | integrand 84.3 · libgomp 5.4 · OpenBLAS 1.7 · other 1.4 | `sumOverCorners` 6.6 | 7.2 |
| mivec_sg_cc_exp_b1_d10_S12_n1000 | integrand 73.8 · libgomp 7.9 · OpenBLAS 4.7 · other 1.1 | `sumOverRows` 12.1 (evaluate 6.7, addScaled 1.2, updateWeightProducts 0.3) | 12.3 |
| mivec_sg_cc_exp_b1_d5_S16_n1000 | integrand 78.7 · libgomp 3.4 · OpenBLAS 2.9 · other 1.2 | `sumOverRows` 13.8 (evaluate 8.9, addScaled 1.6) | 13.8 |
| quad_dsg_cc_uq_b0_d10_L7 | integrand 63.2 · libc 1.5 · other 0.7 | `forEachGPWithGlobalWeight` 33.5 (updateCoeffSums 11.0, compRowWeights 9.1, evaluate 3.9, addScaled 1.3, evaluate 0.5) | 34.0 |
| quad_dsg_cc_uq_b0_d7_L10 | integrand 58.9 · libc 1.5 · OpenBLAS 1.4 · libgomp 1.4 · other 0.9 | `forEachGPWithGlobalWeight` 34.6 (updateCoeffSums 11.6, compRowWeights 9.5, evaluate 4.0, addScaled 1.4, evaluate 0.6) | 36.0 |
| quad_dsg_equi_exp_b0_d10_L7 | integrand 51.8 · libc 2.0 · other 0.7 | `forEachGPWithGlobalWeight` 44.0 (updateCoeffSums 14.4, compRowWeights 12.2, evaluate 5.1, addScaled 1.6, evaluate 0.6) | 44.7 |
| quad_dsg_equi_exp_b0_d3_L16 | integrand 39.4 · libgomp 22.9 · OpenBLAS 6.0 · other 2.3 · libc 1.5 | `forEachGPWithGlobalWeight` 26.6 (compRowWeights 7.3, updateCoeffSums 6.1, evaluate 5.3, addScaled 1.6, evaluate 0.6) | 27.9 |
| quad_dsg_equi_exp_b0_d7_L10 | integrand 49.2 · libc 1.9 · OpenBLAS 1.7 · libgomp 1.6 · other 0.9 | `forEachGPWithGlobalWeight` 43.1 (updateCoeffSums 14.2, compRowWeights 12.0, evaluate 5.3, addScaled 1.7, evaluate 0.6) | 44.8 |
| quad_dsg_equi_exp_b1_d15_L7 | integrand 56.4 · OpenBLAS 2.8 · libgomp 2.0 · other 1.2 · libc 1.0 | `forEachGPWithGlobalWeight` 26.3 (compRowWeights 7.0, updateCoeffSums 6.0, evaluate 4.6, addScaled 1.2, evaluate 0.3) · `addChildren` 8.5 | 36.6 |
| quad_dsg_equi_exp_b1_d20_L6 | integrand 54.7 · OpenBLAS 3.0 · libgomp 2.1 · other 1.4 · libc 0.9 | `forEachGPWithGlobalWeight` 22.5 (compRowWeights 6.2, updateCoeffSums 4.8, evaluate 4.1, addScaled 1.1, evaluate 0.3) · `addChildren` 12.8 · `build` 0.9 | 37.7 |
| quad_sg_cc_uq_b0_d10_L4 | integrand 79.6 · OpenBLAS 1.6 · other 1.2 · libgomp 0.9 | `sumOverRows` 16.7 (evaluate 4.6, addScaled 1.3, updateWeightProducts 1.2, evaluate 1.0) | 16.7 |
| quad_sg_cc_uq_b0_d2_L20 | integrand 56.4 · libgomp 11.0 · OpenBLAS 9.0 · other 7.2 | `sumOverRows` 8.9 (evaluate 5.6, addScaled 1.0) · `kiss_cpx_fft` 5.5 | 16.1 |
| quad_sg_equi_exp_b0_d10_L4 | integrand 71.8 · OpenBLAS 2.2 · other 1.2 · libgomp 1.1 | `sumOverRows` 23.6 (evaluate 6.3, addScaled 2.0, updateWeightProducts 1.8, evaluate 1.3) | 23.6 |
| quad_sg_equi_exp_b0_d2_L20 | integrand 65.9 · libgomp 11.6 · OpenBLAS 6.5 · other 3.2 | `sumOverRows` 12.3 (evaluate 7.2, addScaled 1.5) | 12.6 |
| quad_sg_equi_exp_b1_d20_L5 | integrand 73.5 · libgomp 5.3 · OpenBLAS 4.0 · other 1.6 | `sumOverRows` 15.1 (evaluate 5.0, evaluate 1.4, addScaled 1.2, updateWeightProducts 1.0) | 15.7 |
| quad_sg_equi_sin_b0_d7_L7 | integrand 92.0 · OpenBLAS 0.9 · other 0.8 · libgomp 0.5 | `sumOverRows` 5.8 (evaluate 2.0, addScaled 0.5, evaluate 0.3, updateWeightProducts 0.3) | 5.8 |
