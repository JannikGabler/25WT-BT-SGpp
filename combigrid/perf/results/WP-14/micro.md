A/B `micro`: A=`WP-14-before` B=`WP-14-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.29ms | 6.32ms | 1.002 | [0.983, 1.021] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.00ms | 2.99ms | 1.006 | [0.997, 1.014] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.85ms | 13.94ms | 0.995 | [0.983, 1.007] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.17ms | 25.14ms | 1.004 | [0.999, 1.008] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.90ms | 13.90ms | 1.025 | [0.968, 1.086] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.18ms | 109.27ms | 0.992 | [0.969, 1.014] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.04ms | 52.40ms | 1.009 | [1.003, 1.015] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.55ms | 3.46ms | 1.025 | [1.017, 1.034] | faster |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.88ms | 15.37ms | 1.031 | [1.022, 1.039] | faster |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.84ms | 7.31ms | 1.073 | [1.062, 1.083] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.76us | 37.60us | 1.492 | [1.463, 1.522] | faster |
| micro/quadrule/cc_weights/n33 | call | 2.10us | 2.10us | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.96us | 51.29us | 1.010 | [1.000, 1.021] | faster |
| micro/quadrule/cc_weights/n32769 | call | 4.46ms | 4.39ms | 1.015 | [1.011, 1.018] | faster |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.97ms | 7.91ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 69.45ms | 68.96ms | 1.057 | [0.919, 1.215] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.76ms | 2.81ms | 0.987 | [0.965, 1.009] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 325.52ms | 329.65ms | 0.992 | [0.984, 1.001] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.29ms | 1.12ms | 1.159 | [1.117, 1.202] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 550.16us | 510.00us | 1.054 | [0.985, 1.129] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 834.5ns | 829.3ns | 1.007 | [0.997, 1.017] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.37us | 37.59us | 0.995 | [0.986, 1.004] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.8ns | 0.995 | [0.991, 0.999] | SLOWER |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 197.0ns | 196.3ns | 1.005 | [1.001, 1.008] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 46.65us | 42.48us | 1.096 | [1.084, 1.108] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 51.00ms | 44.97ms | 1.131 | [1.121, 1.140] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 93.36ms | 90.24ms | 1.033 | [1.023, 1.043] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 594.77ms | 622.31ms | 0.956 | [0.952, 0.960] | SLOWER |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.75ms | 18.85ms | 0.943 | [0.932, 0.954] | SLOWER |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.21ms | 15.91ms | 1.017 | [1.013, 1.021] | faster |

Sum of medians (call, common cases): A = 1.437s, B = 1.458s, saved = -20918628.4ns (0.986x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 51, 51, 51, 51, 51 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 255s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
