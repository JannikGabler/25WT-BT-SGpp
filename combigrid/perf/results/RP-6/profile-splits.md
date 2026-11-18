| expensive_interplin_dsg_equi_exp_b1_d5_L5_p16_c100us | integrand 47.8 · libgomp 44.0 · OpenBLAS 6.2 · other 2.0 |  | 0.1 |
| expensive_interplin_sg_equi_exp_b1_d10_L4_p16_c1us | integrand 91.5 · OpenBLAS 2.9 · libgomp 1.9 · other 1.4 | `sumOverCorners` 2.4 | 2.4 |
| expensive_interplin_sg_equi_exp_b1_d5_L5_p16_c100us_ramp | integrand 86.5 · libgomp 9.8 · OpenBLAS 2.6 · other 1.1 |  | 0.0 |
| expensive_mivec_dsg_cc_exp_b1_d15_S6_n300_c100us | integrand 90.9 · OpenBLAS 5.4 · libgomp 2.0 · other 1.6 |  | 0.0 |
| expensive_mivec_dsg_cc_exp_b1_d20_S10_n300_c1us | integrand 82.4 · libgomp 7.8 · OpenBLAS 4.5 · other 1.2 | `forEachGPWithGlobalWeight` 2.8 (updateCoeffSums 1.3, compRowWeights 0.6, evaluate 0.3, addScaled 0.2) | 3.8 |
| expensive_mivec_sg_cc_exp_b1_d15_S6_n300_c100us | integrand 94.1 · OpenBLAS 3.3 · libgomp 1.3 · other 1.2 |  | 0.0 |
| expensive_mivec_sg_cc_exp_b1_d20_S10_n300_c1us | integrand 87.6 · libgomp 6.2 · OpenBLAS 4.0 · other 1.2 | `sumOverRows` 0.9 (evaluate 0.3) | 1.0 |
| expensive_quad_sg_cc_exp_b1_d15_L5_c1us | integrand 93.4 · OpenBLAS 2.6 · other 1.1 · libgomp 0.7 | `sumOverRows` 2.1 (evaluate 0.6, evaluate 0.3) | 2.1 |
| interp_dsg_cc_b0_d10_L2_p8 | integrand 82.2 · libgomp 5.5 · OpenBLAS 3.1 · other 1.9 | `forEachGPWithGlobalWeight` 6.9 (updateCoeffSums 1.9, compRowWeights 1.7, evaluate 0.9, addScaled 0.3) | 7.0 |
| interp_dsg_cc_b0_d2_L12_p64 | integrand 39.6 · libgomp 39.2 · OpenBLAS 4.1 · libc 1.9 · other 1.6 | `forEachGPWithGlobalWeight` 10.8 (compRowWeights 2.6, evaluate 2.6, updateCoeffSums 2.2, addScaled 0.8, evaluate 0.3) | 13.6 |
| interp_dsg_cc_b0_d5_L6_p64 | integrand 73.0 · libgomp 3.8 · OpenBLAS 3.6 · other 1.6 · libc 0.8 | `forEachGPWithGlobalWeight` 13.7 (updateCoeffSums 5.0, compRowWeights 3.6, evaluate 1.7, addScaled 0.5) · `build` 1.5 | 17.1 |
| interp_dsg_cc_b0_d7_L4_p16 | integrand 78.8 · OpenBLAS 4.4 · libgomp 2.8 · other 2.0 · libc 0.5 | `forEachGPWithGlobalWeight` 10.5 (updateCoeffSums 3.3, compRowWeights 2.7, evaluate 1.3, addScaled 0.4) | 11.4 |
| interp_dsg_equi_b0_d3_L5_p64 | libgomp 55.7 · integrand 23.5 · OpenBLAS 5.3 · other 1.8 · libc 1.2 | `forEachGPWithGlobalWeight` 4.1 (updateCoeffSums 1.4, compRowWeights 1.0, evaluate 0.5) · `barycentricFormulaWeights` 2.8 · `build` 2.4 | 12.3 |
| interp_sg_cc_b0_d10_L2_p8 | integrand 89.0 · libgomp 4.5 · other 1.4 · OpenBLAS 1.3 | `sumOverRows` 3.7 (evaluate 0.8, updateWeightProducts 0.4, addScaled 0.3, evaluate 0.3) | 3.7 |
| interp_sg_cc_b0_d2_L12_p64 | integrand 45.9 · libgomp 42.9 · OpenBLAS 4.2 · other 1.2 | `sumOverRows` 5.0 (evaluate 2.6, addScaled 0.5, updateWeightProducts 0.2) | 5.7 |
| interp_sg_cc_b0_d5_L6_p64 | integrand 86.8 · libgomp 4.5 · OpenBLAS 1.8 · other 0.9 | `sumOverRows` 5.8 (evaluate 1.8, updateWeightProducts 0.5, addScaled 0.5, evaluate 0.4) | 5.9 |
| interp_sg_cc_b0_d7_L4_p16 | integrand 87.9 · libgomp 3.9 · OpenBLAS 2.1 · other 1.4 | `sumOverRows` 4.7 (evaluate 1.2, updateWeightProducts 0.5, addScaled 0.4, evaluate 0.3) | 4.8 |
| interp_sg_cheb1_b0_d5_L4_p64 | integrand 74.3 · libgomp 12.5 · OpenBLAS 4.6 · other 2.1 | `sumOverRows` 5.4 (evaluate 1.6, addScaled 0.6, updateWeightProducts 0.5, evaluate 0.3) | 6.1 |
| interplin_dsg_equi_b1_d10_L5_p32 | integrand 68.7 · libgomp 12.1 · OpenBLAS 4.3 · other 2.4 | `evaluateChunks` 5.9 · `transformFiber` 5.8 | 12.0 |
| interplin_dsg_equi_b1_d2_L16_p16384 | libgomp 33.7 · OpenBLAS 32.9 · integrand 7.1 · other 5.8 · libc 1.7 | `transformFiber` 7.2 · `evaluateChunks` 5.6 · `prepareDimension` 1.6 · `lowerBoundFrom` 1.5 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 1.3 | 18.6 |
| interplin_dsg_equi_b1_d5_L8_p1024 | integrand 32.3 · libgomp 28.9 · OpenBLAS 5.2 · other 5.1 | `transformFiber` 20.1 · `evaluateChunks` 5.4 · `sgpp::combigrid::tools::dsg_lin_interpolation::` 1.7 | 28.0 |
| interplin_dsg_equi_b1_d8_L5_p256 | integrand 57.6 · libgomp 18.3 · OpenBLAS 4.8 · other 2.8 | `transformFiber` 9.3 · `evaluateChunks` 5.9 | 15.8 |
| interplin_sg_equi_b1_d10_L5_p32 | integrand 87.3 · libgomp 3.1 · other 1.4 · OpenBLAS 1.2 | `sumOverCorners` 6.7 | 7.0 |
| interplin_sg_equi_b1_d5_L8_p1024 | integrand 70.6 · libgomp 11.7 · OpenBLAS 3.1 · other 2.0 | `sumOverCorners` 8.3 · `prepareCorners` 4.0 | 12.5 |
| interplin_sg_equi_b1_d8_L5_p256 | integrand 84.8 · libgomp 4.8 · OpenBLAS 1.7 · other 1.1 | `sumOverCorners` 7.1 | 7.6 |
| mivec_sg_cc_exp_b1_d10_S12_n1000 | integrand 72.6 · libgomp 8.1 · OpenBLAS 4.9 · other 1.1 | `sumOverRows` 13.1 (evaluate 7.4, addScaled 1.4, updateWeightProducts 0.3) | 13.3 |
| mivec_sg_cc_exp_b1_d15_S10_n300 | libgomp 46.2 · integrand 29.5 · OpenBLAS 15.4 · other 3.4 | `sumOverRows` 4.3 (evaluate 2.4, addScaled 0.4) · `computeCTCoeffs` 0.8 | 5.2 |
| mivec_sg_cc_exp_b1_d20_S7_n300 | libgomp 51.0 · OpenBLAS 34.9 · integrand 6.8 · other 5.8 | `sumOverRows` 1.1 (evaluate 0.5) | 1.4 |
| mivec_sg_cc_exp_b1_d5_S16_n1000 | integrand 76.5 · libgomp 3.1 · OpenBLAS 3.0 · other 1.0 | `sumOverRows` 16.3 (evaluate 10.1, addScaled 1.8) | 16.3 |
| quad_dsg_cc_uq_b0_d10_L7 | integrand 62.8 · libc 1.5 · other 0.7 | `forEachGPWithGlobalWeight` 33.8 (updateCoeffSums 10.8, compRowWeights 9.1, evaluate 4.2, addScaled 1.2, evaluate 0.4) | 34.3 |
| quad_dsg_cc_uq_b0_d7_L10 | integrand 58.1 · OpenBLAS 1.4 · libc 1.4 · libgomp 1.4 · other 0.9 | `forEachGPWithGlobalWeight` 35.4 (updateCoeffSums 11.8, compRowWeights 9.3, evaluate 4.5, addScaled 1.2, evaluate 0.5) | 36.8 |
| quad_dsg_equi_exp_b0_d10_L7 | integrand 51.5 · libc 2.0 · other 0.7 | `forEachGPWithGlobalWeight` 44.4 (updateCoeffSums 14.6, compRowWeights 12.3, evaluate 5.1, addScaled 1.5, evaluate 0.6) | 45.0 |
| quad_dsg_equi_exp_b0_d3_L16 | integrand 37.7 · libgomp 23.4 · OpenBLAS 6.2 · other 2.9 · libc 1.3 | `forEachGPWithGlobalWeight` 27.4 (compRowWeights 7.7, updateCoeffSums 6.2, evaluate 5.4, addScaled 1.7, evaluate 0.6) | 28.5 |
| quad_dsg_equi_exp_b0_d7_L10 | integrand 47.7 · libc 2.1 · OpenBLAS 1.8 · libgomp 1.6 · other 1.0 | `forEachGPWithGlobalWeight` 44.0 (updateCoeffSums 14.8, compRowWeights 11.7, evaluate 5.4, addScaled 1.7, evaluate 0.5) | 45.8 |
| quad_dsg_equi_exp_b1_d15_L7 | integrand 56.5 · OpenBLAS 2.7 · libgomp 2.0 · other 1.2 · libc 1.0 | `forEachGPWithGlobalWeight` 26.2 (compRowWeights 6.9, updateCoeffSums 5.8, evaluate 4.8, addScaled 1.2, evaluate 0.3) · `addChildren` 8.4 · `build` 0.8 | 36.6 |
| quad_dsg_equi_exp_b1_d20_L6 | integrand 54.6 · OpenBLAS 3.1 · libgomp 2.1 · other 1.4 · libc 1.1 | `forEachGPWithGlobalWeight` 22.6 (compRowWeights 6.2, updateCoeffSums 5.1, evaluate 3.9, addScaled 1.0, evaluate 0.3) · `addChildren` 12.6 · `build` 0.9 | 37.6 |
| quad_sg_cc_uq_b0_d10_L4 | integrand 80.0 · OpenBLAS 1.6 · other 1.1 · libgomp 0.8 | `sumOverRows` 16.5 (evaluate 4.5, updateWeightProducts 1.8, addScaled 1.4, evaluate 1.3) | 16.5 |
| quad_sg_cc_uq_b0_d2_L20 | integrand 60.6 · libgomp 10.9 · OpenBLAS 9.1 · other 5.0 | `sumOverRows` 10.4 (evaluate 6.2, addScaled 1.1) · `kiss_cpx_fft` 2.7 | 14.2 |
| quad_sg_equi_exp_b0_d10_L4 | integrand 72.7 · OpenBLAS 2.2 · other 1.4 · libgomp 1.1 | `sumOverRows` 22.6 (evaluate 6.1, updateWeightProducts 2.2, addScaled 2.0, evaluate 1.7) | 22.6 |
| quad_sg_equi_exp_b0_d2_L20 | integrand 62.7 · libgomp 11.3 · OpenBLAS 7.6 · other 3.4 | `sumOverRows` 14.3 (evaluate 8.2, addScaled 1.7, updateWeightProducts 0.3, evaluate 0.3) | 14.6 |
| quad_sg_equi_exp_b1_d20_L5 | integrand 72.8 · libgomp 5.4 · OpenBLAS 4.2 · other 1.4 | `sumOverRows` 15.3 (evaluate 5.5, evaluate 1.5, addScaled 1.0, updateWeightProducts 1.0) | 16.1 |
| quad_sg_equi_sin_b0_d7_L7 | integrand 92.0 · OpenBLAS 0.8 · other 0.8 | `sumOverRows` 5.9 (evaluate 2.0, addScaled 0.5, evaluate 0.4, updateWeightProducts 0.4) | 5.9 |
