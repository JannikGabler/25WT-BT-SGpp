A/B `micro`: A=`WP-18-before` B=`WP-18-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.99ms | 5.90ms | 1.013 | [1.006, 1.020] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 3.00ms | 1.004 | [0.973, 1.036] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.88ms | 14.08ms | 0.987 | [0.976, 0.999] | SLOWER |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.75ms | 24.82ms | 0.998 | [0.987, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.93ms | 13.96ms | 0.996 | [0.985, 1.008] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.09ms | 109.38ms | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.51ms | 52.35ms | 0.995 | [0.969, 1.022] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.42ms | 3.53ms | 0.968 | [0.960, 0.976] | SLOWER |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.53ms | 15.85ms | 0.987 | [0.965, 1.009] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 8.56ms | 7.47ms | 1.202 | [1.049, 1.377] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.97us | 35.59us | 1.004 | [0.963, 1.046] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.12us | 2.10us | 1.008 | [1.002, 1.014] | faster |
| micro/quadrule/cc_weights/n1025 | call | 51.18us | 51.53us | 0.994 | [0.989, 1.000] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.35ms | 4.36ms | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.65ms | 7.46ms | 1.025 | [1.020, 1.030] | faster |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.20ms | 65.37ms | 1.016 | [0.997, 1.035] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.68us | 123.92us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 112.78ms | 97.81ms | 1.152 | [1.142, 1.162] | faster |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.94ms | 19.57ms | 1.026 | [0.994, 1.059] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.10ms | 9.01ms | 1.010 | [0.996, 1.025] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.89ms | 54.50ms | 1.007 | [1.001, 1.014] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.61ms | 90.72ms | 0.996 | [0.987, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.07ms | 284.44ms | 0.999 | [0.988, 1.009] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.41ms | 223.92ms | 0.995 | [0.991, 0.999] | SLOWER |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 201.12ms | 125.12ms | 1.407 | [1.100, 1.798] | faster |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 85.87ms | 86.14ms | 1.013 | [0.964, 1.065] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.55ms | 86.47ms | 1.065 | [0.866, 1.310] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 31.19ms | 30.66ms | 1.019 | [1.003, 1.036] | faster |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 909.55us | 909.75us | 1.000 | [0.987, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.37us | 243.10us | 1.001 | [0.980, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.94ms | 39.36ms | 1.014 | [1.010, 1.018] | faster |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.01ms | 922.00us | 1.097 | [1.092, 1.101] | faster |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.66us | 69.48us | 1.002 | [0.997, 1.006] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 128.84ms | 126.93ms | 1.017 | [1.006, 1.029] | faster |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.57ms | 61.63ms | 0.986 | [0.950, 1.023] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.32ms | 12.32ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 13.01ms | 12.95ms | 1.005 | [1.002, 1.009] | faster |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.54ms | 18.50ms | 1.005 | [0.997, 1.013] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.01ms | 35.29ms | 0.997 | [0.986, 1.007] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.84ms | 10.93ms | 0.995 | [0.985, 1.005] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.15ms | 3.15ms | 0.997 | [0.990, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.29ms | 5.30ms | 1.038 | [0.934, 1.152] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 417.02us | 416.93us | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 832.3ns | 844.2ns | 0.988 | [0.969, 1.008] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.34us | 37.35us | 0.999 | [0.990, 1.008] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.8ns | 0.995 | [0.991, 0.999] | SLOWER |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.6ns | 196.7ns | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.35us | 36.34us | 1.001 | [0.996, 1.007] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 177.86us | 178.00us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 203.1ns | 206.4ns | 0.984 | [0.978, 0.990] | SLOWER |
| micro/interplin/tg/equi/b1/d5/L8 | call | 2.98us | 2.99us | 0.994 | [0.989, 1.000] | ~ (noise) |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.49ms | 23.54ms | 1.013 | [0.981, 1.046] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 605.00ms | 604.52ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.77ms | 47.75ms | 0.997 | [0.978, 1.017] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.72ms | 3.77ms | 0.987 | [0.982, 0.992] | SLOWER |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.41us | 8.61us | 0.974 | [0.961, 0.986] | SLOWER |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 140.17ms | 139.00ms | 1.009 | [0.999, 1.018] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 14.63ms | 14.44ms | 1.015 | [0.995, 1.035] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 1.41ms | 1.35ms | 1.039 | [1.028, 1.050] | faster |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 8.51us | 8.13us | 1.052 | [1.024, 1.081] | faster |
| micro/sol/interp_tg/cc/b0/d10/L2 | call | 41.14ms | 41.24ms | 0.996 | [0.982, 1.011] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d5/L6 | call | 998.10us | 998.79us | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/sol/interp_tg/cheb1/b0/d5/L4 | call | 479.89us | 482.00us | 0.997 | [0.990, 1.005] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d2/L12 | call | 1.05ms | 1.05ms | 0.995 | [0.986, 1.004] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 191.60us | 191.79us | 1.019 | [0.924, 1.122] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 36.06us | 36.01us | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 209.9ns | 198.9ns | 1.051 | [1.043, 1.060] | faster |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.96us | 2.95us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.79ms | 22.77ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d10/L2 | call | 38.98ms | 38.83ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d5/L6 | call | 887.41us | 888.48us | 0.944 | [0.798, 1.118] | ~ (noise) |
| micro/sol/interp_tg_integrand/cheb1/b0/d5/L4 | call | 226.94us | 227.46us | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d2/L12 | call | 852.97us | 852.16us | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d10/L5 | call | 189.16us | 190.48us | 0.993 | [0.979, 1.008] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d8/L5 | call | 35.94us | 35.90us | 1.000 | [0.995, 1.004] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d2/L16 | call | 198.3ns | 183.1ns | 1.079 | [1.066, 1.091] | faster |
| micro/sol/interplin_tg_integrand/equi/b1/d5/L8 | call | 2.92us | 2.91us | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d16/L2 | call | 22.87ms | 22.80ms | 1.030 | [0.949, 1.118] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n9 | call | 11.0ns | 11.0ns | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/sol/barycentric_1d/generic_cached_weights/n65 | call | 83.7ns | 83.6ns | 1.001 | [0.995, 1.008] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.72ms | 45.63ms | 1.002 | [0.992, 1.013] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 91.40ms | 11.55ms | 7.895 | [7.814, 7.978] | faster |
| micro/gen/sg/cc/b1/d20/L5 | call | 91.82ms | 11.61ms | 7.899 | [7.810, 7.989] | faster |
| micro/gen/sg/cc/b0/d2/L20 | call | 307.59ms | 102.85ms | 2.985 | [2.973, 2.997] | faster |
| micro/gen/sg/equi/b0/d5/L10 | call | 2.46ms | 89.31us | 27.530 | [27.117, 27.949] | faster |
| micro/gen/dsg/equi/b1/d20/L6 | call | 287.23ms | 16.83ms | 17.053 | [16.777, 17.334] | faster |
| micro/gen/dsg/equi/b1/d15/L7 | call | 205.47ms | 9.91ms | 20.672 | [20.467, 20.879] | faster |
| micro/gen/dsg/equi/b0/d3/L16 | call | 7.42ms | 933.40us | 7.968 | [7.894, 8.042] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 326.92us | 324.30us | 1.006 | [0.999, 1.014] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.22us | 11.15us | 1.010 | [0.990, 1.029] | ~ (noise) |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 497.76us | 500.49us | 0.993 | [0.987, 0.999] | SLOWER |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.55ms | 1.54ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 566.59us | 568.90us | 0.991 | [0.980, 1.002] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 111.31us | 111.00us | 1.008 | [0.985, 1.032] | ~ (noise) |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.011 | [0.992, 1.030] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 93.02us | 91.18us | 1.019 | [1.013, 1.024] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.05ms | 15.08ms | 0.999 | [0.994, 1.003] | ~ (noise) |

Sum of medians (call, common cases): A = 3.888s, B = 2.955s, saved = 933.36ms (1.316x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 65, 65, 65, 65, 65 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 325s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
