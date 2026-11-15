A/B `micro`: A=`WP-18-before` B=`WP-18-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.98ms | 5.94ms | 1.006 | [0.995, 1.018] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.99ms | 2.97ms | 1.003 | [0.979, 1.028] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.88ms | 14.01ms | 0.992 | [0.980, 1.004] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.87ms | 24.86ms | 1.002 | [0.992, 1.013] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.91ms | 14.01ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.35ms | 109.79ms | 0.997 | [0.992, 1.001] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.67ms | 52.39ms | 1.006 | [1.001, 1.011] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.43ms | 3.46ms | 0.996 | [0.974, 1.020] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.61ms | 15.84ms | 0.987 | [0.977, 0.996] | SLOWER |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 8.56ms | 7.56ms | 1.133 | [1.122, 1.144] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.87us | 35.66us | 1.003 | [0.996, 1.010] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.13us | 2.13us | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.20us | 51.13us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.36ms | 4.36ms | 1.000 | [0.995, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.64ms | 7.49ms | 0.986 | [0.899, 1.082] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.14ms | 65.61ms | 1.007 | [0.999, 1.016] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.63us | 123.76us | 1.009 | [0.979, 1.040] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 112.95ms | 98.24ms | 1.148 | [1.135, 1.160] | faster |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 20.11ms | 19.65ms | 1.019 | [0.991, 1.047] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.26ms | 9.02ms | 1.034 | [0.991, 1.078] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.52ms | 54.92ms | 1.014 | [0.996, 1.032] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.82ms | 90.69ms | 1.002 | [0.996, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.32ms | 284.77ms | 0.993 | [0.986, 1.001] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 223.01ms | 227.78ms | 0.985 | [0.968, 1.003] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 200.90ms | 124.82ms | 1.353 | [0.940, 1.949] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 86.02ms | 94.64ms | 0.894 | [0.789, 1.013] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.97ms | 88.03ms | 1.014 | [0.904, 1.137] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 31.26ms | 30.94ms | 1.012 | [0.996, 1.029] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 910.74us | 911.37us | 1.000 | [0.987, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.62us | 243.51us | 0.999 | [0.979, 1.021] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.95ms | 39.25ms | 1.014 | [1.003, 1.026] | faster |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.01ms | 920.33us | 1.096 | [1.089, 1.103] | faster |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.73us | 69.92us | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 128.90ms | 126.55ms | 1.021 | [1.011, 1.032] | faster |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.74ms | 61.49ms | 1.003 | [0.997, 1.010] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.32ms | 12.31ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 13.03ms | 12.88ms | 1.011 | [1.008, 1.015] | faster |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.58ms | 18.51ms | 1.005 | [0.998, 1.012] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.09ms | 35.16ms | 0.970 | [0.893, 1.053] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.83ms | 10.88ms | 0.996 | [0.990, 1.002] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.15ms | 3.14ms | 1.004 | [1.000, 1.008] | faster |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 0.997 | [0.987, 1.008] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 416.63us | 417.90us | 0.997 | [0.991, 1.004] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 837.3ns | 834.2ns | 1.009 | [0.988, 1.030] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.37us | 37.49us | 0.996 | [0.987, 1.005] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.6ns | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.7ns | 196.9ns | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.38us | 36.40us | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 177.99us | 177.84us | 1.002 | [0.995, 1.009] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 203.4ns | 206.3ns | 0.984 | [0.976, 0.993] | SLOWER |
| micro/interplin/tg/equi/b1/d5/L8 | call | 2.98us | 3.00us | 0.995 | [0.991, 0.999] | SLOWER |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.72ms | 23.66ms | 1.005 | [0.993, 1.018] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 606.10ms | 605.17ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.96ms | 47.94ms | 1.013 | [0.976, 1.052] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.73ms | 3.77ms | 0.988 | [0.984, 0.992] | SLOWER |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.41us | 8.55us | 0.986 | [0.976, 0.996] | SLOWER |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 140.61ms | 143.01ms | 0.985 | [0.974, 0.996] | SLOWER |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 14.64ms | 14.51ms | 1.008 | [1.002, 1.014] | faster |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 1.41ms | 1.40ms | 1.009 | [0.987, 1.031] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 8.47us | 8.29us | 1.025 | [1.017, 1.034] | faster |
| micro/sol/interp_tg/cc/b0/d10/L2 | call | 41.12ms | 41.19ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d5/L6 | call | 997.58us | 994.51us | 1.004 | [0.999, 1.009] | ~ (noise) |
| micro/sol/interp_tg/cheb1/b0/d5/L4 | call | 480.29us | 486.09us | 0.993 | [0.982, 1.004] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d2/L12 | call | 1.05ms | 1.04ms | 1.010 | [1.006, 1.013] | faster |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 191.21us | 188.96us | 1.022 | [0.982, 1.065] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 35.99us | 36.08us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 209.8ns | 194.9ns | 1.076 | [1.072, 1.080] | faster |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.96us | 2.95us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.82ms | 22.79ms | 1.003 | [0.995, 1.010] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d10/L2 | call | 38.91ms | 38.99ms | 0.992 | [0.966, 1.017] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d5/L6 | call | 887.27us | 888.49us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/sol/interp_tg_integrand/cheb1/b0/d5/L4 | call | 226.98us | 227.37us | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d2/L12 | call | 854.74us | 856.82us | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d10/L5 | call | 191.08us | 191.97us | 0.994 | [0.979, 1.009] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d8/L5 | call | 35.94us | 35.94us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d2/L16 | call | 196.0ns | 182.5ns | 1.073 | [1.069, 1.077] | faster |
| micro/sol/interplin_tg_integrand/equi/b1/d5/L8 | call | 2.92us | 2.91us | 1.004 | [1.000, 1.008] | faster |
| micro/sol/interplin_tg_integrand/equi/b1/d16/L2 | call | 22.80ms | 22.87ms | 0.997 | [0.991, 1.002] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n9 | call | 11.0ns | 11.0ns | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n65 | call | 83.9ns | 83.8ns | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 46.07ms | 45.59ms | 1.010 | [0.992, 1.029] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 91.55ms | 11.62ms | 7.886 | [7.812, 7.960] | faster |
| micro/gen/sg/cc/b1/d20/L5 | call | 91.87ms | 11.66ms | 7.866 | [7.789, 7.943] | faster |
| micro/gen/sg/cc/b0/d2/L20 | call | 306.85ms | 139.14ms | 2.205 | [2.190, 2.219] | faster |
| micro/gen/sg/equi/b0/d5/L10 | call | 2.45ms | 92.83us | 26.424 | [26.146, 26.706] | faster |
| micro/gen/dsg/equi/b1/d20/L6 | call | 288.03ms | 16.89ms | 17.094 | [16.846, 17.345] | faster |
| micro/gen/dsg/equi/b1/d15/L7 | call | 205.95ms | 9.89ms | 20.796 | [20.559, 21.036] | faster |
| micro/gen/dsg/equi/b0/d3/L16 | call | 7.46ms | 1.81ms | 4.128 | [4.093, 4.163] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 323.86us | 324.07us | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.14us | 11.13us | 1.004 | [0.988, 1.019] | ~ (noise) |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 495.97us | 502.30us | 0.987 | [0.978, 0.996] | SLOWER |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.54ms | 1.54ms | 1.001 | [0.995, 1.006] | ~ (noise) |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 563.72us | 563.77us | 1.001 | [0.993, 1.009] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 111.44us | 110.93us | 1.006 | [0.991, 1.021] | ~ (noise) |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.005 | [0.975, 1.036] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 93.18us | 90.79us | 1.025 | [1.020, 1.030] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.02ms | 15.10ms | 0.995 | [0.988, 1.001] | ~ (noise) |

Sum of medians (call, common cases): A = 3.895s, B = 3.013s, saved = 882.21ms (1.293x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.5%, 0.5%, 0.5%, 0.5%
Round durations (s): 65, 66, 66, 66, 65 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 327s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
