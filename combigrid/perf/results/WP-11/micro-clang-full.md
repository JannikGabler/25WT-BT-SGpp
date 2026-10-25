A/B `micro`: A=`WP-11-gcc` B=`WP-11-clang-full`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.88ms | 6.34ms | 1.078 | [1.058, 1.099] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 3.16ms | 0.962 | [0.957, 0.966] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.97ms | 14.20ms | 0.984 | [0.973, 0.996] | SLOWER |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.81ms | 24.73ms | 1.039 | [1.025, 1.053] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.98ms | 14.21ms | 0.985 | [0.981, 0.988] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 111.20ms | 117.64ms | 0.943 | [0.935, 0.951] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.00ms | 55.40ms | 0.951 | [0.932, 0.971] | SLOWER |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.56ms | 3.67ms | 0.978 | [0.962, 0.994] | SLOWER |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.83ms | 17.25ms | 0.918 | [0.910, 0.926] | SLOWER |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.83ms | 7.81ms | 1.003 | [0.993, 1.012] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.78us | 60.61us | 0.925 | [0.915, 0.935] | SLOWER |
| micro/quadrule/cc_weights/n33 | call | 2.11us | 2.16us | 0.975 | [0.966, 0.985] | SLOWER |
| micro/quadrule/cc_weights/n1025 | call | 52.37us | 56.39us | 0.928 | [0.925, 0.932] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.47ms | 4.55ms | 0.984 | [0.979, 0.989] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.92ms | 8.07ms | 0.980 | [0.976, 0.984] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.67ms | 69.94ms | 1.016 | [0.927, 1.112] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.76ms | 2.95ms | 0.934 | [0.929, 0.939] | SLOWER |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 333.84ms | 353.18ms | 0.936 | [0.917, 0.954] | SLOWER |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.35ms | 1.18ms | 1.157 | [1.109, 1.208] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 574.45us | 534.71us | 1.075 | [1.053, 1.099] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 845.3ns | 841.7ns | 1.003 | [0.993, 1.014] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.65us | 38.67us | 0.974 | [0.966, 0.983] | SLOWER |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 58.6ns | 33.2ns | 1.772 | [1.756, 1.788] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 402.0ns | 234.6ns | 1.708 | [1.689, 1.726] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.39us | 41.83us | 1.133 | [1.127, 1.139] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 52.49ms | 33.80ms | 1.553 | [1.535, 1.571] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 104.59ms | 91.72ms | 1.138 | [1.126, 1.150] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 595.44ms | 427.93ms | 1.390 | [1.384, 1.395] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.76ms | 12.53ms | 1.417 | [1.401, 1.433] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.11ms | 16.08ms | 1.002 | [0.995, 1.009] | ~ (noise) |

Sum of medians (call, common cases): A = 1.461s, B = 1.287s, saved = 174.19ms (1.135x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.3%, 0.3%
Round durations (s): 46, 46, 46, 47, 46 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 232s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
