A/B `micro`: A=`WP-88-before` B=`WP-88-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 908.03us | 908.54us | 0.998 | [0.985, 1.011] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.15us | 242.57us | 1.002 | [0.981, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.24ms | 39.17ms | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 919.26us | 918.90us | 0.997 | [0.988, 1.007] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.50us | 69.45us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 831.9ns | 817.2ns | 1.019 | [1.000, 1.039] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.34us | 37.33us | 1.000 | [0.991, 1.009] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.7ns | 31.8ns | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.3ns | 196.0ns | 1.002 | [0.998, 1.006] | ~ (noise) |

Sum of medians (call, common cases): A = 41.42ms, B = 41.34ms, saved = 77.19us (1.002x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.2%, 0.3%, 0.2%
Round durations (s): 4, 4, 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 20s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
