A/B `micro`: A=`WP-11-gcc` B=`WP-11-clang-thin`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.88ms | 6.35ms | 1.083 | [1.078, 1.088] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 3.18ms | 0.956 | [0.952, 0.961] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.97ms | 13.99ms | 0.992 | [0.971, 1.013] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.77ms | 24.69ms | 1.040 | [1.027, 1.054] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.98ms | 14.00ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 111.11ms | 116.76ms | 0.951 | [0.947, 0.956] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.04ms | 54.53ms | 0.984 | [0.956, 1.013] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.55ms | 3.79ms | 0.936 | [0.928, 0.944] | SLOWER |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.87ms | 16.99ms | 0.924 | [0.898, 0.951] | SLOWER |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.84ms | 7.73ms | 1.005 | [0.977, 1.034] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.81us | 60.00us | 0.930 | [0.927, 0.934] | SLOWER |
| micro/quadrule/cc_weights/n33 | call | 2.11us | 2.10us | 0.973 | [0.887, 1.066] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 52.37us | 56.59us | 0.925 | [0.916, 0.934] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.49ms | 4.56ms | 0.985 | [0.980, 0.989] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.91ms | 8.02ms | 0.985 | [0.980, 0.990] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.50ms | 70.21ms | 0.977 | [0.968, 0.985] | SLOWER |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.77ms | 2.95ms | 0.939 | [0.928, 0.950] | SLOWER |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 326.92ms | 359.44ms | 0.912 | [0.904, 0.920] | SLOWER |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.37ms | 1.18ms | 1.167 | [1.134, 1.201] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 573.29us | 521.97us | 1.094 | [1.071, 1.117] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 846.2ns | 843.7ns | 1.006 | [0.988, 1.024] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.56us | 38.51us | 0.976 | [0.968, 0.985] | SLOWER |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 58.7ns | 33.4ns | 1.756 | [1.750, 1.763] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 402.0ns | 252.8ns | 1.590 | [1.578, 1.603] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.29us | 41.54us | 1.138 | [1.132, 1.145] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 52.46ms | 33.44ms | 1.573 | [1.539, 1.607] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 103.89ms | 90.40ms | 1.151 | [1.136, 1.166] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 594.84ms | 482.68ms | 1.233 | [1.228, 1.237] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.81ms | 12.89ms | 1.382 | [1.366, 1.398] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.10ms | 16.45ms | 0.978 | [0.970, 0.985] | SLOWER |

Sum of medians (call, common cases): A = 1.453s, B = 1.345s, saved = 107.91ms (1.080x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.3%, 0.3%, 0.3%
Round durations (s): 48, 48, 48, 48, 48 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 240s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
