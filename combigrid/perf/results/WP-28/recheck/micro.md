A/B `micro`: A=`WP-28-before` B=`WP-28-refactor`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.18ms | 6.19ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 2.98ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.96ms | 13.88ms | 1.002 | [0.991, 1.014] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.95ms | 25.04ms | 0.995 | [0.988, 1.002] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.89ms | 13.89ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.12ms | 108.61ms | 1.003 | [0.999, 1.008] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.48ms | 53.14ms | 0.989 | [0.983, 0.994] | SLOWER |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.47ms | 3.39ms | 1.022 | [0.993, 1.051] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.40ms | 15.44ms | 1.000 | [0.991, 1.008] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.40ms | 7.48ms | 0.990 | [0.980, 0.999] | SLOWER |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.07us | 35.71us | 0.980 | [0.959, 1.000] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.10us | 2.15us | 0.976 | [0.963, 0.988] | SLOWER |
| micro/quadrule/cc_weights/n1025 | call | 51.25us | 51.59us | 0.993 | [0.986, 0.999] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.34ms | 4.35ms | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.71ms | 7.65ms | 1.036 | [0.960, 1.118] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.96ms | 66.05ms | 0.998 | [0.990, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 122.96us | 123.10us | 1.003 | [0.994, 1.012] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 110.72ms | 111.71ms | 0.995 | [0.986, 1.004] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 505.35us | 508.15us | 0.994 | [0.973, 1.015] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 839.6ns | 829.8ns | 1.020 | [0.985, 1.056] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.38us | 37.43us | 1.000 | [0.991, 1.009] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.9ns | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.9ns | 196.9ns | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 41.93us | 42.08us | 0.994 | [0.988, 1.001] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 44.78ms | 44.64ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 89.66ms | 89.97ms | 0.998 | [0.989, 1.008] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 325.22us | 321.61us | 1.011 | [1.005, 1.016] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.01us | 11.28us | 0.976 | [0.963, 0.989] | SLOWER |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.08ms | 15.09ms | 1.000 | [0.989, 1.010] | ~ (noise) |

Sum of medians (call, common cases): A = 590.32ms, B = 591.76ms, saved = -1441124.7ns (0.998x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.4%, 0.2%
Round durations (s): 17, 17, 17, 17, 17 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 85s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
