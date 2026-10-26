A/B `micro`: A=`WP-14-before` B=`WP-14-sa`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.29ms | 6.32ms | 0.994 | [0.989, 0.999] | SLOWER |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.00ms | 2.99ms | 1.010 | [0.987, 1.033] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.88ms | 13.85ms | 1.002 | [0.985, 1.019] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.21ms | 25.20ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.96ms | 13.92ms | 1.019 | [0.966, 1.074] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.13ms | 109.16ms | 1.009 | [0.984, 1.035] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.93ms | 52.76ms | 1.007 | [1.002, 1.011] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.56ms | 3.46ms | 1.028 | [1.020, 1.037] | faster |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.87ms | 15.44ms | 1.027 | [1.018, 1.036] | faster |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.84ms | 7.32ms | 1.066 | [1.053, 1.078] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.52us | 37.55us | 1.479 | [1.474, 1.485] | faster |
| micro/quadrule/cc_weights/n33 | call | 2.10us | 2.11us | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 52.02us | 51.15us | 1.011 | [0.998, 1.025] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.45ms | 4.36ms | 0.953 | [0.793, 1.146] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.96ms | 7.90ms | 1.007 | [1.003, 1.011] | faster |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 69.53ms | 68.86ms | 1.010 | [1.002, 1.019] | faster |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.75ms | 2.81ms | 0.993 | [0.951, 1.037] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 333.08ms | 326.65ms | 1.009 | [0.987, 1.031] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.29ms | 1.12ms | 1.144 | [1.129, 1.160] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 548.52us | 506.04us | 1.088 | [1.066, 1.112] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 830.0ns | 834.7ns | 0.994 | [0.986, 1.001] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.37us | 37.49us | 0.998 | [0.989, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.8ns | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.8ns | 196.4ns | 1.003 | [0.997, 1.008] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 46.71us | 42.55us | 1.100 | [1.089, 1.110] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 51.01ms | 45.23ms | 1.128 | [1.123, 1.134] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 93.17ms | 90.11ms | 1.033 | [1.024, 1.043] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 595.40ms | 621.73ms | 0.958 | [0.954, 0.961] | SLOWER |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.76ms | 18.85ms | 0.943 | [0.932, 0.953] | SLOWER |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.16ms | 15.95ms | 1.015 | [1.011, 1.019] | faster |

Sum of medians (call, common cases): A = 1.445s, B = 1.455s, saved = -9720767.3ns (0.993x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.4%, 0.2%, 0.3%
Round durations (s): 51, 51, 51, 51, 51 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 255s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
