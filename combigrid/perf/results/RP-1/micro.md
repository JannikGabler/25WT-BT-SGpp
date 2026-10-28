A/B `micro`: A=`baseline` B=`rp-1`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 13.21ms | 6.32ms | 2.077 | [2.037, 2.118] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 7.60ms | 2.98ms | 2.543 | [2.523, 2.563] | faster |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 46.89ms | 13.88ms | 3.373 | [3.333, 3.413] | faster |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 31.86ms | 25.01ms | 1.270 | [1.262, 1.278] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 47.01ms | 13.84ms | 3.394 | [3.355, 3.434] | faster |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 538.01ms | 109.22ms | 4.956 | [4.854, 5.061] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.55ms | 3.46ms | 1.020 | [1.007, 1.033] | faster |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.84ms | 15.39ms | 1.029 | [1.015, 1.044] | faster |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.83ms | 7.34ms | 1.064 | [1.045, 1.084] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.85us | 37.75us | 1.412 | [1.231, 1.620] | faster |
| micro/quadrule/cc_weights/n33 | call | 2.09us | 2.10us | 0.996 | [0.990, 1.002] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 52.39us | 51.23us | 1.023 | [1.018, 1.027] | faster |
| micro/quadrule/cc_weights/n32769 | call | 4.43ms | 4.36ms | 1.017 | [1.012, 1.022] | faster |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.92ms | 7.90ms | 1.007 | [0.994, 1.020] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.51ms | 68.97ms | 0.994 | [0.986, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.76ms | 2.80ms | 0.985 | [0.977, 0.993] | SLOWER |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 324.66ms | 330.27ms | 0.990 | [0.971, 1.010] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.36ms | 1.13ms | 1.213 | [1.169, 1.259] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 556.11us | 508.98us | 1.094 | [1.071, 1.118] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 840.6ns | 832.2ns | 1.006 | [0.997, 1.016] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.72us | 37.45us | 1.008 | [0.999, 1.017] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 59.0ns | 31.8ns | 1.858 | [1.840, 1.875] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 406.4ns | 196.4ns | 2.067 | [2.059, 2.076] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.23us | 42.51us | 1.112 | [1.106, 1.118] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 52.06ms | 44.92ms | 1.160 | [1.153, 1.168] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 104.28ms | 89.94ms | 1.162 | [1.151, 1.172] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 593.46ms | 621.86ms | 0.955 | [0.951, 0.959] | SLOWER |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.87ms | 18.88ms | 0.948 | [0.938, 0.959] | SLOWER |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.33ms | 15.96ms | 0.959 | [0.954, 0.963] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | (not in A) | 52.35ms | - | - | new case |

Sum of medians (call, common cases): A = 1.905s, B = 1.405s, saved = 500.10ms (1.356x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 53, 53, 53, 53, 53 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 265s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
