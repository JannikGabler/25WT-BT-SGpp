A/B `micro`: A=`WP-11-gcc` B=`WP-11-gcc-nosi-bsym`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.88ms | 6.07ms | 1.133 | [1.128, 1.137] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 2.96ms | 0.994 | [0.914, 1.082] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.98ms | 13.86ms | 1.011 | [0.999, 1.023] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.76ms | 25.09ms | 1.026 | [1.016, 1.036] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.97ms | 13.81ms | 1.011 | [1.004, 1.018] | faster |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 111.05ms | 108.62ms | 1.021 | [1.015, 1.027] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.03ms | 52.15ms | 1.014 | [1.009, 1.020] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.57ms | 3.56ms | 1.006 | [0.995, 1.016] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.88ms | 15.65ms | 1.013 | [1.004, 1.023] | faster |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.82ms | 7.82ms | 1.000 | [0.990, 1.010] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.72us | 55.97us | 1.005 | [0.943, 1.070] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.11us | 2.09us | 1.014 | [1.006, 1.023] | faster |
| micro/quadrule/cc_weights/n1025 | call | 52.41us | 52.41us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.46ms | 4.46ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.91ms | 7.92ms | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.49ms | 69.06ms | 0.991 | [0.982, 0.999] | SLOWER |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.75ms | 2.80ms | 0.987 | [0.967, 1.007] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 329.64ms | 330.09ms | 0.998 | [0.969, 1.028] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.35ms | 1.30ms | 1.030 | [1.005, 1.056] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 574.03us | 557.62us | 1.026 | [1.004, 1.048] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 847.2ns | 836.1ns | 1.010 | [0.999, 1.021] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.59us | 38.12us | 0.986 | [0.977, 0.995] | SLOWER |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 58.6ns | 31.8ns | 1.839 | [1.827, 1.851] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 406.1ns | 196.5ns | 2.062 | [2.047, 2.077] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.33us | 47.09us | 1.006 | [0.999, 1.012] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 52.49ms | 51.40ms | 0.993 | [0.904, 1.090] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 103.64ms | 93.29ms | 1.113 | [1.102, 1.123] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 594.92ms | 618.60ms | 0.960 | [0.954, 0.966] | SLOWER |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.80ms | 18.63ms | 0.955 | [0.944, 0.966] | SLOWER |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.16ms | 16.13ms | 1.001 | [0.991, 1.010] | ~ (noise) |

Sum of medians (call, common cases): A = 1.455s, B = 1.464s, saved = -8684537.9ns (0.994x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.3%, 0.3%, 0.3%, 0.3%
Round durations (s): 51, 51, 51, 51, 51 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 256s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
