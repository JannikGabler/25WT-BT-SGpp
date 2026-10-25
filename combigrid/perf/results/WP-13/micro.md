A/B `micro`: A=`WP-13-before` B=`WP-13-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.88ms | 6.26ms | 1.098 | [1.093, 1.102] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 2.99ms | 1.013 | [1.006, 1.021] | faster |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.97ms | 13.86ms | 1.009 | [0.997, 1.021] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.75ms | 25.17ms | 1.025 | [1.017, 1.033] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.97ms | 13.84ms | 1.011 | [1.007, 1.015] | faster |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 111.19ms | 108.99ms | 1.019 | [1.014, 1.024] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.99ms | 53.02ms | 1.006 | [0.982, 1.030] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.55ms | 3.56ms | 0.998 | [0.989, 1.006] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.83ms | 15.89ms | 0.998 | [0.989, 1.007] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.84ms | 7.84ms | 1.002 | [0.992, 1.012] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.72us | 55.58us | 1.002 | [0.994, 1.010] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.11us | 2.10us | 1.008 | [1.002, 1.014] | faster |
| micro/quadrule/cc_weights/n1025 | call | 52.32us | 51.89us | 1.008 | [0.999, 1.018] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.45ms | 4.44ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.92ms | 7.94ms | 1.002 | [0.987, 1.017] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.49ms | 69.47ms | 0.976 | [0.948, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.77ms | 2.74ms | 1.010 | [0.991, 1.029] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 326.48ms | 330.83ms | 0.988 | [0.969, 1.007] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.35ms | 1.29ms | 1.031 | [0.991, 1.073] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 573.37us | 547.83us | 1.035 | [1.009, 1.062] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 841.5ns | 832.9ns | 0.993 | [0.946, 1.043] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.55us | 37.43us | 1.003 | [0.994, 1.012] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 58.6ns | 31.7ns | 1.846 | [1.839, 1.853] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 401.2ns | 196.9ns | 2.040 | [2.032, 2.047] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.27us | 46.63us | 1.014 | [1.008, 1.019] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 52.68ms | 51.31ms | 1.030 | [1.020, 1.040] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 103.56ms | 93.37ms | 1.108 | [1.098, 1.119] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 595.19ms | 594.68ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.76ms | 17.72ms | 1.003 | [0.991, 1.014] | ~ (noise) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.11ms | 16.24ms | 0.993 | [0.989, 0.997] | SLOWER |

Sum of medians (call, common cases): A = 1.453s, B = 1.442s, saved = 10.32ms (1.007x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.3%
Round durations (s): 50, 50, 50, 50, 50 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 252s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
