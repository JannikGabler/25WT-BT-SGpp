A/B `micro`: A=`WP-11-gcc` B=`WP-11-clang-thin-nosi`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.87ms | 6.33ms | 1.085 | [1.081, 1.089] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 3.17ms | 0.961 | [0.956, 0.966] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.02ms | 13.98ms | 1.004 | [0.992, 1.016] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.78ms | 24.58ms | 1.044 | [1.036, 1.052] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.01ms | 14.00ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 111.16ms | 117.02ms | 0.950 | [0.945, 0.954] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.07ms | 54.49ms | 0.980 | [0.966, 0.995] | SLOWER |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.56ms | 3.80ms | 0.934 | [0.913, 0.956] | SLOWER |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.86ms | 17.03ms | 0.932 | [0.922, 0.943] | SLOWER |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.86ms | 7.70ms | 1.025 | [1.012, 1.038] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.84us | 60.07us | 0.929 | [0.923, 0.935] | SLOWER |
| micro/quadrule/cc_weights/n33 | call | 2.11us | 2.10us | 1.003 | [0.996, 1.011] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 52.37us | 56.56us | 0.925 | [0.919, 0.931] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.47ms | 4.54ms | 0.985 | [0.981, 0.988] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.92ms | 8.04ms | 0.983 | [0.979, 0.987] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.64ms | 70.31ms | 0.977 | [0.969, 0.986] | SLOWER |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.76ms | 2.93ms | 0.944 | [0.921, 0.968] | SLOWER |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 326.31ms | 362.89ms | 0.907 | [0.896, 0.919] | SLOWER |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.35ms | 1.19ms | 1.163 | [1.105, 1.223] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 572.93us | 521.25us | 1.104 | [1.061, 1.149] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 842.2ns | 848.0ns | 0.995 | [0.976, 1.013] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.62us | 38.57us | 0.975 | [0.966, 0.984] | SLOWER |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 58.7ns | 33.4ns | 1.760 | [1.743, 1.777] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 402.3ns | 253.1ns | 1.592 | [1.583, 1.601] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.41us | 41.53us | 1.145 | [1.130, 1.161] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 53.29ms | 33.38ms | 1.583 | [1.565, 1.601] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 104.88ms | 89.26ms | 1.167 | [1.153, 1.181] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 595.37ms | 482.10ms | 1.235 | [1.229, 1.241] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.77ms | 12.92ms | 1.377 | [1.361, 1.392] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.04ms | 16.28ms | 0.982 | [0.966, 0.998] | SLOWER |

Sum of medians (call, common cases): A = 1.455s, B = 1.347s, saved = 108.15ms (1.080x)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 48, 48, 48, 48, 48 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 240s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
