A/B `micro`: A=`WP-36` B=`WP-36-sol`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.45ms | 3.48ms | 0.992 | [0.984, 1.000] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.46ms | 15.43ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.11us | 35.80us | 0.941 | [0.835, 1.060] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 506.59us | 507.01us | 1.000 | [0.979, 1.021] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.75ms | 46.71ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.37ms | 1.37ms | 1.003 | [0.967, 1.041] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.05us | 118.00us | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 832.2ns | 835.5ns | 0.992 | [0.981, 1.004] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.38us | 37.30us | 1.000 | [0.991, 1.009] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.7ns | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.4ns | 196.2ns | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.05us | 42.04us | 1.000 | [0.994, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 233.47us | 231.92us | 0.989 | [0.950, 1.030] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 205.4ns | 205.5ns | 1.000 | [0.994, 1.007] | ~ (noise) |

Sum of medians (call, common cases): A = 69.12ms, B = 69.07ms, saved = 49.33us (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.7%, 0.2%, 0.2%
Round durations (s): 7, 7, 7, 7, 7 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 35s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
