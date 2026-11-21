A/B `micro`: A=`WP-79-before` B=`WP-79-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.93ms | 5.94ms | 1.020 | [0.969, 1.073] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.97ms | 2.97ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.02ms | 13.99ms | 1.007 | [0.988, 1.027] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.91ms | 5.91ms | 1.056 | [0.911, 1.224] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.97ms | 14.00ms | 0.996 | [0.991, 1.002] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 106.74ms | 106.94ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.12ms | 44.21ms | 0.988 | [0.957, 1.020] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.11ms | 112.18ms | 0.997 | [0.990, 1.005] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.53ms | 100.47ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.21ms | 44.17ms | 0.996 | [0.978, 1.014] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.53ms | 106.61ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.49ms | 3.47ms | 0.995 | [0.959, 1.032] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.82ms | 15.71ms | 1.006 | [0.997, 1.014] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.90ms | 7.47ms | 1.054 | [1.043, 1.064] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.78us | 35.74us | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 965.6ns | 959.0ns | 1.005 | [0.999, 1.011] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 27.67us | 27.60us | 1.001 | [0.996, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 1.40ms | 1.40ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n131073 | call | 6.26ms | 6.25ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| micro/quadrule/cc_weights/n1048577 | call | 68.77ms | 68.38ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.46ms | 7.47ms | 0.991 | [0.964, 1.018] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.32ms | 65.45ms | 1.001 | [0.992, 1.009] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 124.00us | 123.88us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 98.34ms | 98.52ms | 0.999 | [0.991, 1.008] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.68ms | 19.43ms | 1.030 | [0.968, 1.095] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.04ms | 9.07ms | 0.998 | [0.988, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.59ms | 54.75ms | 0.992 | [0.977, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.32ms | 90.42ms | 1.002 | [0.994, 1.009] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 284.92ms | 284.47ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 223.62ms | 223.73ms | 0.997 | [0.988, 1.005] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 137.99ms | 136.98ms | 0.977 | [0.808, 1.182] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 100.49ms | 106.61ms | 1.004 | [0.837, 1.203] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 94.03ms | 104.63ms | 0.938 | [0.754, 1.168] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 30.58ms | 30.72ms | 0.997 | [0.986, 1.009] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 909.93us | 911.04us | 0.980 | [0.928, 1.035] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.17us | 243.67us | 0.998 | [0.977, 1.019] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.20ms | 39.20ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 919.75us | 918.46us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.68us | 69.56us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 127.12ms | 126.66ms | 1.003 | [0.998, 1.008] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.60ms | 61.68ms | 1.001 | [0.992, 1.009] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.33ms | 12.31ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.88ms | 12.87ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.50ms | 18.38ms | 1.003 | [0.997, 1.009] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.14ms | 35.10ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.87ms | 10.85ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.14ms | 0.997 | [0.992, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 1.005 | [0.991, 1.019] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 417.74us | 417.38us | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 829.5ns | 817.1ns | 1.017 | [1.010, 1.025] | faster |
| micro/interp/barycentric_1d/generic/n65 | call | 37.42us | 37.24us | 1.004 | [0.995, 1.013] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.8ns | 0.997 | [0.989, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.7ns | 196.7ns | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.38us | 36.39us | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 177.99us | 178.02us | 0.998 | [0.989, 1.007] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 205.0ns | 205.2ns | 0.996 | [0.987, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d5/L8 | call | 3.00us | 3.00us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.69ms | 23.70ms | 1.000 | [0.982, 1.018] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 606.25ms | 606.25ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.95ms | 47.90ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.78ms | 3.76ms | 1.004 | [0.997, 1.011] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.52us | 8.54us | 0.998 | [0.990, 1.006] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 88.43ms | 88.03ms | 1.004 | [0.998, 1.010] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 8.99ms | 8.99ms | 0.988 | [0.955, 1.022] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 698.62us | 698.88us | 1.010 | [0.978, 1.044] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 7.54us | 7.47us | 1.016 | [0.992, 1.040] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d10/L2 | call | 41.14ms | 41.30ms | 1.001 | [0.990, 1.012] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d5/L6 | call | 995.11us | 996.50us | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/sol/interp_tg/cheb1/b0/d5/L4 | call | 477.43us | 484.66us | 0.992 | [0.972, 1.012] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d2/L12 | call | 1.05ms | 1.05ms | 1.012 | [0.980, 1.046] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 188.50us | 190.72us | 0.975 | [0.920, 1.033] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 36.08us | 36.08us | 0.994 | [0.979, 1.010] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 203.7ns | 205.7ns | 0.992 | [0.988, 0.996] | SLOWER |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.95us | 2.96us | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.79ms | 22.83ms | 0.998 | [0.991, 1.006] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d10/L2 | call | 38.85ms | 39.53ms | 0.985 | [0.967, 1.002] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d5/L6 | call | 888.29us | 887.96us | 1.004 | [0.993, 1.014] | ~ (noise) |
| micro/sol/interp_tg_integrand/cheb1/b0/d5/L4 | call | 226.75us | 226.86us | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d2/L12 | call | 852.56us | 854.36us | 0.999 | [0.994, 1.003] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d10/L5 | call | 190.04us | 191.12us | 0.991 | [0.974, 1.009] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d8/L5 | call | 35.93us | 35.98us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d2/L16 | call | 190.8ns | 190.9ns | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d5/L8 | call | 2.91us | 2.92us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d16/L2 | call | 22.69ms | 22.74ms | 1.001 | [0.989, 1.014] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n9 | call | 11.1ns | 11.1ns | 0.996 | [0.989, 1.003] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n65 | call | 83.6ns | 83.6ns | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 46.16ms | 46.51ms | 0.997 | [0.984, 1.009] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 11.58ms | 11.61ms | 0.997 | [0.987, 1.006] | ~ (noise) |
| micro/gen/sg/cc/b1/d20/L5 | call | 11.64ms | 11.57ms | 1.005 | [1.001, 1.010] | faster |
| micro/gen/sg/cc/b0/d2/L20 | call | 139.26ms | 139.41ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/gen/sg/equi/b0/d5/L10 | call | 92.82us | 92.38us | 1.004 | [0.998, 1.010] | ~ (noise) |
| micro/gen/dsg/equi/b1/d20/L6 | call | 16.87ms | 16.88ms | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/gen/dsg/equi/b1/d15/L7 | call | 9.88ms | 9.94ms | 0.993 | [0.986, 1.000] | SLOWER |
| micro/gen/dsg/equi/b0/d3/L16 | call | 1.81ms | 1.81ms | 1.000 | [0.994, 1.005] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 327.68us | 322.69us | 1.014 | [1.007, 1.020] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.14us | 10.94us | 1.023 | [1.003, 1.045] | faster |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 500.87us | 477.92us | 1.047 | [1.024, 1.069] | faster |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.56ms | 1.55ms | 1.006 | [0.996, 1.016] | ~ (noise) |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 570.96us | 566.43us | 1.009 | [1.005, 1.013] | faster |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 111.34us | 113.85us | 0.975 | [0.963, 0.987] | SLOWER |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.008 | [0.992, 1.024] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 91.52us | 92.72us | 0.987 | [0.983, 0.991] | SLOWER |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.90ms | 15.90ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d2/L20 | call | 259.15ms | 258.18ms | 0.998 | [0.989, 1.008] | ~ (noise) |
| micro/crosscall/quad_sg/call/cc/b0/d2/L20 | call | 2.208s | 2.210s | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d2/L20 | call | 1.946s | 1.946s | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d5/L10 | call | 118.69us | 119.06us | 1.000 | [0.994, 1.005] | ~ (noise) |
| micro/crosscall/quad_sg/call/cc/b0/d5/L10 | call | 2.249s | 2.253s | 0.997 | [0.974, 1.020] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d5/L10 | call | 2.269s | 2.258s | 1.003 | [0.896, 1.123] | ~ (noise) |
| micro/crosscall/quad_sg/build/equi/b1/d20/L4 | call | 4.97us | 4.96us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/crosscall/quad_sg/call/equi/b1/d20/L4 | call | 106.91ms | 106.55ms | 1.015 | [0.980, 1.052] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/equi/b1/d20/L4 | call | 106.81ms | 106.82ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/crosscall/quad_dsg/build/cc/b0/d5/L10 | call | 1.21ms | 1.22ms | 0.992 | [0.988, 0.997] | SLOWER |
| micro/crosscall/quad_dsg/call/cc/b0/d5/L10 | call | 303.15ms | 302.05ms | 1.004 | [0.995, 1.014] | ~ (noise) |
| micro/crosscall/quad_dsg/call_flipped/cc/b0/d5/L10 | call | 303.23ms | 304.87ms | 0.985 | [0.959, 1.011] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b1/d15/L5 | call | 4.02ms | 4.01ms | 1.002 | [0.989, 1.016] | ~ (noise) |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | call | 185.29ms | 185.39ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/crosscall/quad_dsg/call_flipped/equi/b1/d15/L5 | call | 190.21ms | 190.51ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b0/d3/L14 | call | 4.05ms | 4.06ms | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/crosscall/quad_dsg/call/equi/b0/d3/L14 | call | 119.96ms | 120.16ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/crosscall/quad_dsg/call_flipped/equi/b0/d3/L14 | call | 124.34ms | 124.18ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d7/L4 | call | 59.05us | 59.34us | 0.996 | [0.988, 1.004] | ~ (noise) |
| micro/crosscall/interp_dsg/call/cc/b0/d7/L4 | call | 104.89ms | 104.78ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d7/L4 | call | 105.02ms | 104.89ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d2/L12 | call | 1.54ms | 1.54ms | 0.999 | [0.990, 1.008] | ~ (noise) |
| micro/crosscall/interp_dsg/call/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d2/L12 | call | 6.35ms | 6.35ms | 1.001 | [0.989, 1.014] | ~ (noise) |
| micro/crosscall/interp_dsg/build/equi/b0/d3/L5 | call | 11.01us | 10.90us | 1.012 | [0.990, 1.034] | ~ (noise) |
| micro/crosscall/interp_dsg/call/equi/b0/d3/L5 | call | 418.25us | 417.86us | 0.989 | [0.956, 1.023] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/equi/b0/d3/L5 | call | 430.18us | 429.47us | 0.993 | [0.977, 1.008] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d10/L2 | call | 22.56us | 22.45us | 1.020 | [0.969, 1.074] | ~ (noise) |
| micro/crosscall/interp_dsg/call/cc/b0/d10/L2 | call | 544.40ms | 544.73ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d10/L2 | call | 544.50ms | 544.81ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d2/L16 | call | 7.34ms | 7.33ms | 1.002 | [0.995, 1.010] | ~ (noise) |
| micro/crosscall/interplin_dsg/call/equi/b1/d2/L16 | call | 7.19us | 7.22us | 0.997 | [0.987, 1.007] | ~ (noise) |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d2/L16 | call | 7.43ms | 7.36ms | 1.004 | [0.996, 1.012] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d10/L5 | call | 2.35ms | 2.35ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| micro/crosscall/interplin_dsg/call/equi/b1/d10/L5 | call | 84.41ms | 84.18ms | 1.003 | [0.996, 1.010] | ~ (noise) |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d10/L5 | call | 86.29ms | 86.38ms | 1.001 | [0.989, 1.014] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d5/L8 | call | 426.37us | 427.18us | 0.999 | [0.982, 1.017] | ~ (noise) |
| micro/crosscall/interplin_dsg/call/equi/b1/d5/L8 | call | 684.77us | 689.00us | 0.993 | [0.985, 1.000] | ~ (noise) |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d5/L8 | call | 1.25ms | 1.24ms | 1.009 | [1.001, 1.017] | faster |

Sum of medians (call, common cases): A = 15.263s, B = 15.274s, saved = -10637171.4ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.3%, 1.1%, 0.3%
Round durations (s): 170, 169, 170, 170, 170 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 850s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
