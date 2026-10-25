A/B `micro`: A=`WP-11-gcc` B=`WP-11-gcc-nosi`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.88ms | 6.28ms | 1.095 | [1.091, 1.099] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 3.00ms | 1.006 | [0.990, 1.022] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.97ms | 13.90ms | 1.005 | [0.993, 1.017] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.74ms | 25.20ms | 1.021 | [1.014, 1.028] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.98ms | 13.88ms | 0.991 | [0.944, 1.040] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 111.17ms | 109.31ms | 1.026 | [0.997, 1.056] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.12ms | 53.22ms | 0.995 | [0.942, 1.050] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.55ms | 3.55ms | 1.006 | [0.986, 1.027] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.88ms | 15.89ms | 0.993 | [0.972, 1.014] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.84ms | 7.84ms | 0.975 | [0.914, 1.039] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 55.94us | 55.89us | 0.998 | [0.967, 1.030] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.11us | 2.11us | 1.000 | [0.994, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 52.31us | 51.83us | 1.010 | [1.004, 1.016] | faster |
| micro/quadrule/cc_weights/n32769 | call | 4.45ms | 4.43ms | 1.005 | [1.002, 1.009] | faster |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.93ms | 7.97ms | 1.131 | [0.783, 1.634] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.51ms | 69.48ms | 0.987 | [0.979, 0.996] | SLOWER |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.76ms | 2.75ms | 1.003 | [0.990, 1.015] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 331.51ms | 326.56ms | 1.007 | [0.985, 1.030] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.35ms | 1.29ms | 1.052 | [1.038, 1.065] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 573.33us | 553.21us | 1.034 | [1.012, 1.056] | faster |
| micro/interp/barycentric_1d/generic/n9 | call | 843.2ns | 831.7ns | 0.995 | [0.924, 1.071] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.66us | 37.39us | 1.007 | [0.998, 1.017] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 58.7ns | 31.7ns | 1.854 | [1.845, 1.864] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 402.4ns | 197.0ns | 2.042 | [2.035, 2.050] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 47.30us | 46.57us | 1.016 | [1.010, 1.021] | faster |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 52.12ms | 51.06ms | 1.019 | [1.001, 1.036] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 104.10ms | 92.93ms | 1.120 | [1.110, 1.131] | faster |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 595.36ms | 595.30ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.81ms | 17.77ms | 1.002 | [0.991, 1.014] | ~ (noise) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 16.14ms | 16.22ms | 0.997 | [0.990, 1.004] | ~ (noise) |

Sum of medians (call, common cases): A = 1.458s, B = 1.439s, saved = 19.40ms (1.013x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.5%, 0.3%, 0.3%, 0.3%
Round durations (s): 50, 50, 50, 50, 50 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 252s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
