A/B `micro`: A=`rp-5` B=`rp-6`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.92ms | 5.89ms | 1.004 | [1.000, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.00ms | 2.97ms | 1.012 | [1.004, 1.020] | faster |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.14ms | 14.01ms | 1.008 | [0.997, 1.020] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.91ms | 5.91ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.15ms | 14.05ms | 1.007 | [0.999, 1.014] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 106.95ms | 107.13ms | 1.104 | [0.831, 1.466] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.57ms | 44.22ms | 0.996 | [0.961, 1.032] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 122.43ms | 112.35ms | 1.091 | [1.068, 1.114] | faster |
| micro/quad/operator/equi/b0/d2/L16 | call | 101.64ms | 100.68ms | 1.010 | [1.006, 1.014] | faster |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.38ms | 44.33ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.86ms | 106.94ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.47ms | 3.49ms | 0.994 | [0.986, 1.002] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.76ms | 15.86ms | 0.992 | [0.983, 1.000] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.74ms | 7.89ms | 0.980 | [0.970, 0.989] | SLOWER |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.84us | 35.88us | 1.001 | [0.977, 1.027] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 1.92us | 969.0ns | 1.996 | [1.951, 2.042] | faster |
| micro/quadrule/cc_weights/n1025 | call | 49.06us | 27.70us | 1.763 | [1.746, 1.780] | faster |
| micro/quadrule/cc_weights/n32769 | call | 3.18ms | 1.40ms | 2.259 | [2.214, 2.305] | faster |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.50ms | 7.49ms | 1.008 | [0.986, 1.030] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.53ms | 65.48ms | 0.997 | [0.988, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.73us | 123.96us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 97.85ms | 97.56ms | 1.005 | [0.996, 1.014] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.61ms | 19.36ms | 1.017 | [0.996, 1.037] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.04ms | 9.03ms | 1.005 | [0.985, 1.024] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.42ms | 54.42ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.46ms | 90.36ms | 1.007 | [0.989, 1.026] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 284.40ms | 284.05ms | 1.002 | [0.996, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 228.53ms | 223.71ms | 1.024 | [1.013, 1.036] | faster |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 127.58ms | 130.30ms | 0.997 | [0.702, 1.414] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 107.38ms | 91.01ms | 1.107 | [0.821, 1.492] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 94.21ms | 93.60ms | 1.051 | [0.855, 1.293] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 30.60ms | 30.72ms | 0.992 | [0.964, 1.021] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 910.82us | 909.23us | 1.003 | [0.990, 1.016] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.51us | 243.50us | 1.001 | [0.980, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.24ms | 39.21ms | 1.051 | [0.918, 1.202] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 922.90us | 918.17us | 1.005 | [1.001, 1.009] | faster |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.71us | 69.69us | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.69ms | 126.95ms | 0.996 | [0.989, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.51ms | 61.64ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.31ms | 12.32ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.91ms | 12.89ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.34ms | 18.35ms | 0.998 | [0.988, 1.007] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.14ms | 35.13ms | 1.019 | [0.967, 1.075] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.88ms | 10.88ms | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.14ms | 1.000 | [0.989, 1.011] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 417.50us | 417.52us | 0.998 | [0.991, 1.005] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 834.6ns | 828.1ns | 1.011 | [0.995, 1.027] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.40us | 37.35us | 1.002 | [0.993, 1.011] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.8ns | 0.994 | [0.988, 1.001] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.8ns | 196.5ns | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.41us | 36.39us | 0.978 | [0.918, 1.041] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 178.06us | 177.90us | 0.998 | [0.989, 1.006] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 206.1ns | 205.0ns | 1.005 | [0.982, 1.030] | ~ (noise) |
| micro/interplin/tg/equi/b1/d5/L8 | call | 2.99us | 3.00us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.59ms | 23.62ms | 0.997 | [0.985, 1.009] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 605.14ms | 606.97ms | 0.987 | [0.969, 1.006] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.87ms | 47.93ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.78ms | 3.78ms | 0.997 | [0.988, 1.007] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.55us | 8.49us | 1.008 | [0.996, 1.019] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 142.50ms | 88.54ms | 1.612 | [1.590, 1.634] | faster |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 14.57ms | 9.01ms | 1.629 | [1.588, 1.672] | faster |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 1.36ms | 701.23us | 1.936 | [1.910, 1.962] | faster |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 8.14us | 7.50us | 1.086 | [1.079, 1.093] | faster |
| micro/sol/interp_tg/cc/b0/d10/L2 | call | 41.70ms | 41.20ms | 1.011 | [1.006, 1.017] | faster |
| micro/sol/interp_tg/cc/b0/d5/L6 | call | 997.97us | 995.32us | 1.012 | [0.983, 1.042] | ~ (noise) |
| micro/sol/interp_tg/cheb1/b0/d5/L4 | call | 478.01us | 485.12us | 0.990 | [0.978, 1.002] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d2/L12 | call | 1.05ms | 1.05ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 192.53us | 191.79us | 0.989 | [0.939, 1.040] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 36.06us | 36.04us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 209.9ns | 203.4ns | 1.030 | [1.018, 1.043] | faster |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.96us | 2.95us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.81ms | 22.87ms | 0.996 | [0.991, 1.002] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d10/L2 | call | 38.92ms | 38.91ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d5/L6 | call | 886.37us | 888.50us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/sol/interp_tg_integrand/cheb1/b0/d5/L4 | call | 226.93us | 226.87us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d2/L12 | call | 855.16us | 854.07us | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d10/L5 | call | 190.52us | 189.95us | 1.002 | [0.997, 1.006] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d8/L5 | call | 35.97us | 35.90us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d2/L16 | call | 196.3ns | 190.6ns | 1.032 | [1.021, 1.043] | faster |
| micro/sol/interplin_tg_integrand/equi/b1/d5/L8 | call | 2.92us | 2.91us | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d16/L2 | call | 22.77ms | 22.75ms | 1.001 | [0.993, 1.010] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n9 | call | 11.1ns | 11.1ns | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n65 | call | 83.7ns | 83.8ns | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.89ms | 46.05ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 11.62ms | 11.62ms | 1.000 | [0.990, 1.009] | ~ (noise) |
| micro/gen/sg/cc/b1/d20/L5 | call | 11.66ms | 11.64ms | 1.000 | [0.988, 1.012] | ~ (noise) |
| micro/gen/sg/cc/b0/d2/L20 | call | 139.05ms | 139.22ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/gen/sg/equi/b0/d5/L10 | call | 92.54us | 92.96us | 0.997 | [0.988, 1.006] | ~ (noise) |
| micro/gen/dsg/equi/b1/d20/L6 | call | 16.95ms | 16.85ms | 1.007 | [1.001, 1.012] | faster |
| micro/gen/dsg/equi/b1/d15/L7 | call | 10.03ms | 9.91ms | 1.008 | [0.998, 1.019] | ~ (noise) |
| micro/gen/dsg/equi/b0/d3/L16 | call | 1.81ms | 1.81ms | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 324.70us | 328.26us | 0.992 | [0.984, 1.000] | SLOWER |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.11us | 11.11us | 1.000 | [0.981, 1.019] | ~ (noise) |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 494.48us | 500.44us | 0.987 | [0.977, 0.997] | SLOWER |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.55ms | 1.56ms | 0.996 | [0.992, 1.000] | SLOWER |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 573.03us | 574.08us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 115.37us | 111.63us | 1.036 | [1.030, 1.042] | faster |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.002 | [0.974, 1.031] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 91.15us | 91.31us | 0.998 | [0.993, 1.002] | ~ (noise) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.76ms | 15.88ms | 0.993 | [0.987, 1.000] | SLOWER |
| micro/quadrule/cc_weights/n131073 | call | (not in A) | 6.26ms | - | - | new case |
| micro/quadrule/cc_weights/n1048577 | call | (not in A) | 68.32ms | - | - | new case |
| micro/crosscall/quad_sg/build/cc/b0/d2/L20 | call | (not in A) | 257.27ms | - | - | new case |
| micro/crosscall/quad_sg/call/cc/b0/d2/L20 | call | (not in A) | 2.207s | - | - | new case |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d2/L20 | call | (not in A) | 1.946s | - | - | new case |
| micro/crosscall/quad_sg/build/cc/b0/d5/L10 | call | (not in A) | 118.28us | - | - | new case |
| micro/crosscall/quad_sg/call/cc/b0/d5/L10 | call | (not in A) | 2.249s | - | - | new case |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d5/L10 | call | (not in A) | 2.250s | - | - | new case |
| micro/crosscall/quad_sg/build/equi/b1/d20/L4 | call | (not in A) | 5.00us | - | - | new case |
| micro/crosscall/quad_sg/call/equi/b1/d20/L4 | call | (not in A) | 106.81ms | - | - | new case |
| micro/crosscall/quad_sg/call_flipped/equi/b1/d20/L4 | call | (not in A) | 106.80ms | - | - | new case |
| micro/crosscall/quad_dsg/build/cc/b0/d5/L10 | call | (not in A) | 1.22ms | - | - | new case |
| micro/crosscall/quad_dsg/call/cc/b0/d5/L10 | call | (not in A) | 302.16ms | - | - | new case |
| micro/crosscall/quad_dsg/call_flipped/cc/b0/d5/L10 | call | (not in A) | 303.29ms | - | - | new case |
| micro/crosscall/quad_dsg/build/equi/b1/d15/L5 | call | (not in A) | 4.02ms | - | - | new case |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | call | (not in A) | 185.48ms | - | - | new case |
| micro/crosscall/quad_dsg/call_flipped/equi/b1/d15/L5 | call | (not in A) | 190.47ms | - | - | new case |
| micro/crosscall/quad_dsg/build/equi/b0/d3/L14 | call | (not in A) | 4.04ms | - | - | new case |
| micro/crosscall/quad_dsg/call/equi/b0/d3/L14 | call | (not in A) | 120.01ms | - | - | new case |
| micro/crosscall/quad_dsg/call_flipped/equi/b0/d3/L14 | call | (not in A) | 124.32ms | - | - | new case |
| micro/crosscall/interp_dsg/build/cc/b0/d7/L4 | call | (not in A) | 59.13us | - | - | new case |
| micro/crosscall/interp_dsg/call/cc/b0/d7/L4 | call | (not in A) | 104.86ms | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d7/L4 | call | (not in A) | 104.96ms | - | - | new case |
| micro/crosscall/interp_dsg/build/cc/b0/d2/L12 | call | (not in A) | 1.53ms | - | - | new case |
| micro/crosscall/interp_dsg/call/cc/b0/d2/L12 | call | (not in A) | 5.31ms | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d2/L12 | call | (not in A) | 6.35ms | - | - | new case |
| micro/crosscall/interp_dsg/build/equi/b0/d3/L5 | call | (not in A) | 10.86us | - | - | new case |
| micro/crosscall/interp_dsg/call/equi/b0/d3/L5 | call | (not in A) | 416.63us | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/equi/b0/d3/L5 | call | (not in A) | 429.74us | - | - | new case |
| micro/crosscall/interp_dsg/build/cc/b0/d10/L2 | call | (not in A) | 22.55us | - | - | new case |
| micro/crosscall/interp_dsg/call/cc/b0/d10/L2 | call | (not in A) | 544.70ms | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d10/L2 | call | (not in A) | 544.94ms | - | - | new case |
| micro/crosscall/interplin_dsg/build/equi/b1/d2/L16 | call | (not in A) | 7.35ms | - | - | new case |
| micro/crosscall/interplin_dsg/call/equi/b1/d2/L16 | call | (not in A) | 7.18us | - | - | new case |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d2/L16 | call | (not in A) | 7.42ms | - | - | new case |
| micro/crosscall/interplin_dsg/build/equi/b1/d10/L5 | call | (not in A) | 2.35ms | - | - | new case |
| micro/crosscall/interplin_dsg/call/equi/b1/d10/L5 | call | (not in A) | 84.38ms | - | - | new case |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d10/L5 | call | (not in A) | 86.78ms | - | - | new case |
| micro/crosscall/interplin_dsg/build/equi/b1/d5/L8 | call | (not in A) | 423.07us | - | - | new case |
| micro/crosscall/interplin_dsg/call/equi/b1/d5/L8 | call | (not in A) | 685.99us | - | - | new case |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d5/L8 | call | (not in A) | 1.24ms | - | - | new case |

Sum of medians (call, common cases): A = 3.379s, B = 3.288s, saved = 91.14ms (1.028x)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.3%, 0.2%, 0.2%, 0.1%
Round durations (s): 116, 116, 116, 116, 116 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 581s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
