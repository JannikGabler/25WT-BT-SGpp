A/B `micro`: A=`WP-24-before` B=`WP-24-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.31ms | 6.18ms | 1.021 | [1.017, 1.026] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 3.00ms | 0.987 | [0.968, 1.007] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.85ms | 13.86ms | 1.001 | [0.985, 1.017] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.28ms | 25.18ms | 1.004 | [0.998, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.82ms | 13.87ms | 0.999 | [0.988, 1.011] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.14ms | 107.66ms | 1.013 | [1.008, 1.018] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.20ms | 51.99ms | 1.026 | [0.972, 1.083] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.13ms | 1.13ms | 1.004 | [0.986, 1.022] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 515.48us | 508.81us | 1.001 | [0.961, 1.042] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 820.8ns | 822.6ns | 1.003 | [0.986, 1.019] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.36us | 37.42us | 0.998 | [0.989, 1.007] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 32.0ns | 31.9ns | 1.003 | [0.998, 1.007] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.8ns | 196.7ns | 1.001 | [0.997, 1.005] | ~ (noise) |

Sum of medians (call, common cases): A = 225.25ms, B = 223.43ms, saved = 1.82ms (1.008x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.3%, 0.2%
Round durations (s): 6, 6, 6, 6, 6 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 30s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
