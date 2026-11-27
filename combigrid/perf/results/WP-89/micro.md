A/B `micro`: A=`WP-89-before` B=`WP-89-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.91ms | 5.92ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.97ms | 2.97ms | 0.994 | [0.975, 1.013] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.01ms | 14.00ms | 0.997 | [0.986, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.91ms | 5.91ms | 0.990 | [0.971, 1.010] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.99ms | 14.01ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 106.89ms | 106.89ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.41ms | 44.28ms | 1.006 | [0.997, 1.014] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.00ms | 112.04ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.69ms | 100.67ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.02ms | 44.18ms | 0.996 | [0.992, 1.000] | SLOWER |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.84ms | 106.68ms | 1.006 | [0.994, 1.017] | ~ (noise) |
| micro/quad/compensated_combination/equi/b1/d15/L6 | call | 119.16us | 119.14us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/compensated_combination/equi/b1/d20/L5 | call | 116.61us | 116.62us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/dsg/compensated_combination/equi/b1/d20/L6 | call | 451.54us | 451.49us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/dsg/compensated_combination/equi/b1/d15/L7 | call | 334.51us | 335.33us | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/dsg/operator/equi/b0/d3/L12 | call | 25.33ms | 25.32ms | 0.997 | [0.990, 1.005] | ~ (noise) |
| micro/dsg/operator/equi/b0/d5/L8 | call | 49.64ms | 49.72ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/dsg/operator/equi/b1/d15/L4 | call | 20.51ms | 20.49ms | 1.002 | [0.993, 1.011] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.49ms | 3.46ms | 1.014 | [1.002, 1.026] | faster |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.82ms | 15.74ms | 0.984 | [0.922, 1.049] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.69ms | 7.76ms | 0.997 | [0.988, 1.007] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.61us | 35.68us | 0.989 | [0.962, 1.017] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 982.2ns | 986.1ns | 0.986 | [0.959, 1.015] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 27.60us | 27.68us | 0.997 | [0.991, 1.002] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 1.40ms | 1.39ms | 1.003 | [0.996, 1.010] | ~ (noise) |
| micro/quadrule/cc_weights/n131073 | call | 6.19ms | 6.19ms | 1.000 | [0.994, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n1048577 | call | 67.75ms | 67.73ms | 0.994 | [0.981, 1.006] | ~ (noise) |
| micro/quadrule/cheb1_weights/n33 | call | 3.90us | 3.92us | 1.000 | [0.987, 1.014] | ~ (noise) |
| micro/quadrule/fejer1_weights/n33 | call | 4.63us | 4.63us | 1.003 | [0.993, 1.014] | ~ (noise) |
| micro/quadrule/cheb1_weights/n1025 | call | 114.59us | 114.27us | 1.003 | [0.998, 1.008] | ~ (noise) |
| micro/quadrule/fejer1_weights/n1025 | call | 206.81us | 207.34us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/quadrule/cheb1_weights/n32769 | call | 7.28ms | 7.29ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/quadrule/fejer1_weights/n32769 | call | 11.70ms | 11.67ms | 1.005 | [0.994, 1.016] | ~ (noise) |
| micro/quadrule/cheb1_weights/n131073 | call | 29.79ms | 29.90ms | 0.999 | [0.990, 1.008] | ~ (noise) |
| micro/quadrule/fejer1_weights/n131073 | call | 54.70ms | 54.37ms | 1.004 | [0.985, 1.024] | ~ (noise) |
| micro/quadrule/cheb1_weights/n1048577 | call | 336.11ms | 338.72ms | 0.978 | [0.911, 1.050] | ~ (noise) |
| micro/quadrule/fejer1_weights/n1048577 | call | 710.19ms | 708.68ms | 0.997 | [0.992, 1.003] | ~ (noise) |
| micro/quadrule/fejer1_weights/n131071 | call | 30.33ms | 30.26ms | 0.999 | [0.926, 1.077] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 156.77ms | 156.94ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.41ms | 3.42ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.31ms | 3.32ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.76ms | 4.77ms | 0.997 | [0.985, 1.008] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.70ms | 4.76ms | 0.991 | [0.979, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.48ms | 7.49ms | 0.995 | [0.976, 1.013] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.36ms | 65.39ms | 1.000 | [0.991, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.94us | 124.00us | 0.996 | [0.967, 1.026] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 96.16ms | 96.31ms | 0.999 | [0.990, 1.008] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.54ms | 19.34ms | 1.014 | [0.993, 1.035] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.08ms | 9.59ms | 0.954 | [0.921, 0.988] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.78ms | 54.43ms | 1.007 | [0.995, 1.019] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.68ms | 90.57ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 288.98ms | 284.12ms | 1.013 | [1.004, 1.022] | faster |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 223.62ms | 224.04ms | 1.003 | [0.994, 1.013] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 131.73ms | 124.68ms | 1.175 | [0.930, 1.484] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 93.16ms | 89.26ms | 1.103 | [0.983, 1.237] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 91.16ms | 86.98ms | 1.039 | [0.989, 1.092] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 30.68ms | 30.53ms | 1.015 | [0.986, 1.044] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 908.99us | 909.38us | 1.000 | [0.987, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.22us | 242.83us | 1.001 | [0.980, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.12ms | 39.19ms | 0.996 | [0.988, 1.005] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 918.28us | 920.08us | 0.999 | [0.991, 1.007] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.57us | 69.63us | 0.998 | [0.993, 1.002] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.73ms | 126.81ms | 0.993 | [0.978, 1.008] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 62.17ms | 62.04ms | 1.006 | [0.997, 1.015] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.31ms | 12.32ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.92ms | 12.89ms | 1.003 | [0.999, 1.006] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.002 | [0.989, 1.015] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.51ms | 18.57ms | 0.996 | [0.986, 1.006] | ~ (noise) |
| micro/interp/sg/cc/b0/d1/L6 | call | 4.34us | 4.39us | 0.991 | [0.984, 0.999] | SLOWER |
| micro/interp/sg/cc/b0/d2/L4 | call | 26.63us | 26.59us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L3 | call | 113.71us | 113.77us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.17ms | 35.07ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.91ms | 10.89ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.14ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.29ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 417.99us | 418.21us | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 828.1ns | 832.6ns | 0.995 | [0.982, 1.009] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.39us | 37.32us | 1.001 | [0.992, 1.010] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.6ns | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.7ns | 196.7ns | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.40us | 36.39us | 1.001 | [0.995, 1.006] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 177.96us | 177.52us | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 206.9ns | 201.9ns | 1.025 | [1.020, 1.031] | faster |
| micro/interplin/tg/equi/b1/d5/L8 | call | 3.00us | 2.99us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.64ms | 23.56ms | 1.000 | [0.992, 1.009] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 606.72ms | 604.82ms | 1.004 | [1.000, 1.008] | faster |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.88ms | 47.89ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.78ms | 3.78ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.99us | 8.93us | 1.005 | [0.996, 1.015] | ~ (noise) |
| micro/interplin/sg/equi/b1/d1/L10 | call | 1.34us | 1.35us | 0.996 | [0.985, 1.008] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L8 | call | 5.01us | 4.98us | 1.003 | [0.984, 1.021] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L6 | call | 35.86us | 35.74us | 1.003 | [0.997, 1.010] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L10 | call | 93.74us | 93.81us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d4/L6 | call | 251.96us | 251.53us | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 88.36ms | 88.54ms | 0.977 | [0.921, 1.037] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 8.98ms | 9.00ms | 0.986 | [0.955, 1.019] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 697.86us | 697.27us | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 7.50us | 7.46us | 0.978 | [0.872, 1.095] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d10/L2 | call | 41.18ms | 41.18ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d5/L6 | call | 996.94us | 996.82us | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/sol/interp_tg/cheb1/b0/d5/L4 | call | 483.64us | 480.87us | 1.007 | [1.004, 1.011] | faster |
| micro/sol/interp_tg/cc/b0/d2/L12 | call | 1.05ms | 1.04ms | 1.004 | [0.998, 1.010] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 193.32us | 191.46us | 1.010 | [1.001, 1.020] | faster |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 36.05us | 36.08us | 0.978 | [0.917, 1.043] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 198.5ns | 196.8ns | 1.013 | [0.997, 1.028] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.96us | 2.95us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.83ms | 22.92ms | 0.992 | [0.969, 1.016] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d10/L2 | call | 39.09ms | 38.98ms | 1.006 | [0.992, 1.019] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d5/L6 | call | 886.87us | 887.86us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/sol/interp_tg_integrand/cheb1/b0/d5/L4 | call | 227.24us | 226.70us | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d2/L12 | call | 853.50us | 852.44us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d10/L5 | call | 190.38us | 191.32us | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d8/L5 | call | 35.94us | 35.95us | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d2/L16 | call | 182.3ns | 188.1ns | 0.969 | [0.962, 0.976] | SLOWER |
| micro/sol/interplin_tg_integrand/equi/b1/d5/L8 | call | 2.91us | 2.91us | 0.999 | [0.994, 1.003] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d16/L2 | call | 22.91ms | 22.78ms | 1.006 | [0.997, 1.016] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n9 | call | 11.0ns | 11.0ns | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n65 | call | 83.5ns | 83.5ns | 0.990 | [0.962, 1.020] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.46ms | 45.72ms | 0.995 | [0.986, 1.005] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 11.84ms | 11.70ms | 1.008 | [0.999, 1.018] | ~ (noise) |
| micro/gen/sg/cc/b1/d20/L5 | call | 11.85ms | 11.76ms | 1.007 | [1.001, 1.014] | faster |
| micro/gen/sg/cc/b0/d2/L20 | call | 136.13ms | 137.49ms | 0.991 | [0.988, 0.995] | SLOWER |
| micro/gen/sg/equi/b0/d5/L10 | call | 117.27us | 117.51us | 0.992 | [0.977, 1.007] | ~ (noise) |
| micro/gen/dsg/equi/b1/d20/L6 | call | 18.82ms | 18.45ms | 1.020 | [1.012, 1.029] | faster |
| micro/gen/dsg/equi/b1/d15/L7 | call | 10.87ms | 10.87ms | 0.996 | [0.978, 1.015] | ~ (noise) |
| micro/gen/dsg/equi/b0/d3/L16 | call | 1.79ms | 1.78ms | 0.998 | [0.989, 1.007] | ~ (noise) |
| micro/gen/sg/equi/b1/d10/L7 | call | 2.41ms | 2.42ms | 1.001 | [0.991, 1.010] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 320.97us | 319.24us | 1.011 | [1.001, 1.021] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.05us | 11.09us | 0.997 | [0.982, 1.012] | ~ (noise) |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 502.04us | 495.92us | 1.012 | [1.002, 1.022] | faster |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.54ms | 1.52ms | 1.008 | [0.999, 1.018] | ~ (noise) |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 565.79us | 563.27us | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 109.82us | 109.60us | 1.002 | [0.996, 1.007] | ~ (noise) |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 5.90ms | 6.00ms | 0.995 | [0.950, 1.043] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 90.91us | 90.89us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.20ms | 15.33ms | 0.992 | [0.985, 1.000] | SLOWER |
| micro/crosscall/quad_sg/build/cc/b0/d2/L20 | call | 261.47ms | 259.49ms | 0.996 | [0.981, 1.011] | ~ (noise) |
| micro/crosscall/quad_sg/call/cc/b0/d2/L20 | call | 2.207s | 2.209s | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d2/L20 | call | 1.947s | 1.947s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d5/L10 | call | 118.85us | 120.01us | 0.992 | [0.988, 0.995] | SLOWER |
| micro/crosscall/quad_sg/call/cc/b0/d5/L10 | call | 2.250s | 2.251s | 0.985 | [0.946, 1.025] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d5/L10 | call | 2.250s | 2.248s | 1.010 | [0.980, 1.042] | ~ (noise) |
| micro/crosscall/quad_sg/build/equi/b1/d20/L4 | call | 5.61us | 5.96us | 0.941 | [0.928, 0.953] | SLOWER |
| micro/crosscall/quad_sg/call/equi/b1/d20/L4 | call | 106.74ms | 106.78ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/equi/b1/d20/L4 | call | 106.87ms | 106.78ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/crosscall/quad_dsg/build/cc/b0/d5/L10 | call | 1.26ms | 1.27ms | 1.000 | [0.992, 1.008] | ~ (noise) |
| micro/crosscall/quad_dsg/call/cc/b0/d5/L10 | call | 306.01ms | 312.78ms | 0.965 | [0.909, 1.024] | ~ (noise) |
| micro/crosscall/quad_dsg/call_flipped/cc/b0/d5/L10 | call | 303.17ms | 303.94ms | 0.991 | [0.973, 1.009] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b1/d15/L5 | call | 5.31ms | 5.30ms | 1.007 | [0.998, 1.016] | ~ (noise) |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | call | 185.19ms | 185.30ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/crosscall/quad_dsg/call_flipped/equi/b1/d15/L5 | call | 190.79ms | 191.19ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b0/d3/L14 | call | 4.04ms | 4.06ms | 0.995 | [0.992, 0.999] | SLOWER |
| micro/crosscall/quad_dsg/call/equi/b0/d3/L14 | call | 120.09ms | 120.28ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/crosscall/quad_dsg/call_flipped/equi/b0/d3/L14 | call | 124.16ms | 124.23ms | 0.990 | [0.962, 1.020] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d7/L4 | call | 59.53us | 60.33us | 0.989 | [0.982, 0.995] | SLOWER |
| micro/crosscall/interp_dsg/call/cc/b0/d7/L4 | call | 105.27ms | 104.83ms | 1.004 | [0.996, 1.012] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d7/L4 | call | 105.24ms | 104.89ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d2/L12 | call | 1.53ms | 1.52ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/crosscall/interp_dsg/call/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d2/L12 | call | 6.34ms | 6.47ms | 0.984 | [0.975, 0.992] | SLOWER |
| micro/crosscall/interp_dsg/build/equi/b0/d3/L5 | call | 11.23us | 11.18us | 1.033 | [0.941, 1.134] | ~ (noise) |
| micro/crosscall/interp_dsg/call/equi/b0/d3/L5 | call | 417.87us | 417.93us | 1.010 | [0.981, 1.040] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/equi/b0/d3/L5 | call | 430.37us | 430.46us | 1.003 | [0.979, 1.027] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d10/L2 | call | 22.67us | 23.11us | 1.001 | [0.951, 1.053] | ~ (noise) |
| micro/crosscall/interp_dsg/call/cc/b0/d10/L2 | call | 545.25ms | 545.25ms | 1.008 | [0.972, 1.046] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d10/L2 | call | 544.97ms | 545.19ms | 1.004 | [0.979, 1.029] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d2/L16 | call | 7.34ms | 7.48ms | 0.979 | [0.975, 0.983] | SLOWER |
| micro/crosscall/interplin_dsg/call/equi/b1/d2/L16 | call | 7.22us | 7.27us | 0.993 | [0.981, 1.006] | ~ (noise) |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d2/L16 | call | 7.31ms | 7.46ms | 0.978 | [0.975, 0.982] | SLOWER |
| micro/crosscall/interplin_dsg/build/equi/b1/d10/L5 | call | 2.38ms | 2.42ms | 0.975 | [0.952, 1.000] | SLOWER |
| micro/crosscall/interplin_dsg/call/equi/b1/d10/L5 | call | 84.24ms | 84.35ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d10/L5 | call | 86.43ms | 86.30ms | 1.002 | [0.997, 1.006] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d5/L8 | call | 428.15us | 436.58us | 0.983 | [0.966, 1.000] | ~ (noise) |
| micro/crosscall/interplin_dsg/call/equi/b1/d5/L8 | call | 685.47us | 686.34us | 0.998 | [0.993, 1.003] | ~ (noise) |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d5/L8 | call | 1.24ms | 1.25ms | 0.990 | [0.971, 1.009] | ~ (noise) |
| micro/textbook/quad/textbook/cc/b0/d3/L6 | call | 4.72ms | 4.68ms | 1.009 | [0.998, 1.020] | ~ (noise) |
| micro/textbook/quad/optimized/cc/b0/d3/L6 | call | 2.08ms | 2.08ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/textbook/quad/textbook/equi/b1/d5/L5 | call | 12.40ms | 12.49ms | 1.010 | [0.966, 1.055] | ~ (noise) |
| micro/textbook/quad/optimized/equi/b1/d5/L5 | call | 6.32ms | 6.37ms | 0.993 | [0.984, 1.002] | ~ (noise) |
| micro/textbook/interp/textbook/cc/b0/d3/L5 | call | 1.94ms | 1.93ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| micro/textbook/interp/optimized/cc/b0/d3/L5 | call | 823.12us | 824.49us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/textbook/interp/textbook/cc/b0/d5/L4 | call | 35.17ms | 35.21ms | 1.001 | [0.987, 1.016] | ~ (noise) |
| micro/textbook/interp/optimized/cc/b0/d5/L4 | call | 18.17ms | 18.19ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| micro/textbook/interplin/textbook/equi/b1/d5/L5 | call | 1.22ms | 1.21ms | 1.003 | [0.998, 1.007] | ~ (noise) |
| micro/textbook/interplin/optimized/equi/b1/d5/L5 | call | 733.38us | 733.26us | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/textbook/interplin/textbook/equi/b1/d8/L4 | call | 29.31ms | 29.27ms | 1.000 | [0.992, 1.008] | ~ (noise) |
| micro/textbook/interplin/optimized/equi/b1/d8/L4 | call | 18.23ms | 18.33ms | 0.994 | [0.982, 1.006] | ~ (noise) |

Sum of medians (call, common cases): A = 16.823s, B = 16.811s, saved = 11.99ms (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.8%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 200, 198, 198, 198, 198 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 992s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
