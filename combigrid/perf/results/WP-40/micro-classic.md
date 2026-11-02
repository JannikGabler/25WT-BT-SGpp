A/B `micro`: A=`WP-40-before` B=`WP-40-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.19ms | 6.00ms | 1.036 | [1.023, 1.050] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 2.98ms | 1.018 | [1.000, 1.036] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.90ms | 13.90ms | 0.996 | [0.983, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.07ms | 24.90ms | 1.010 | [1.005, 1.015] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.87ms | 13.89ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 108.61ms | 109.79ms | 0.990 | [0.986, 0.995] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.96ms | 52.66ms | 1.008 | [0.999, 1.017] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.39ms | 3.40ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.44ms | 15.47ms | 0.992 | [0.972, 1.013] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.47ms | 7.43ms | 1.006 | [0.996, 1.015] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.62us | 35.02us | 1.018 | [1.010, 1.025] | faster |
| micro/quadrule/cc_weights/n33 | call | 2.12us | 2.14us | 1.009 | [0.979, 1.039] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.59us | 51.49us | 1.002 | [0.996, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.37ms | 4.35ms | 1.001 | [0.993, 1.010] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.66ms | 7.65ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.15ms | 66.44ms | 0.987 | [0.963, 1.011] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.30us | 123.77us | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.63ms | 113.33ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 1.000 | [0.987, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 511.59us | 506.07us | 1.008 | [0.987, 1.030] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 826.6ns | 831.4ns | 0.975 | [0.916, 1.039] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.43us | 37.44us | 0.999 | [0.991, 1.008] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.9ns | 0.998 | [0.987, 1.008] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 197.0ns | 196.8ns | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 41.97us | 41.99us | 0.999 | [0.993, 1.004] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.29ms | 46.19ms | 0.984 | [0.972, 0.997] | SLOWER |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 90.60ms | 90.89ms | 1.019 | [0.951, 1.093] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 323.68us | 322.10us | 1.003 | [0.995, 1.012] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.16us | 10.90us | 1.025 | [1.013, 1.037] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.05ms | 15.06ms | 1.001 | [0.997, 1.004] | ~ (noise) |

Sum of medians (call, common cases): A = 594.96ms, B = 596.59ms, saved = -1630230.6ns (0.997x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.3%, 0.2%, 0.2%
Round durations (s): 17, 17, 17, 17, 17 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 86s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
