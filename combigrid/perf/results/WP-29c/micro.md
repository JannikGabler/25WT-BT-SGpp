A/B `micro`: A=`WP-29c-before` B=`WP-29c-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.917 | [0.725, 1.161] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 514.34us | 508.44us | 0.951 | [0.808, 1.118] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.03ms | 47.62ms | 0.988 | [0.980, 0.997] | SLOWER |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.39ms | 1.39ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.72us | 118.65us | 1.004 | [0.993, 1.014] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 830.5ns | 825.6ns | 1.001 | [0.984, 1.019] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.45us | 37.44us | 0.981 | [0.930, 1.035] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.8ns | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.4ns | 196.6ns | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.60us | 42.48us | 1.003 | [0.997, 1.008] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 240.09us | 241.01us | 1.011 | [0.963, 1.061] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 206.3ns | 207.4ns | 0.995 | [0.991, 1.000] | SLOWER |

Sum of medians (call, common cases): A = 50.49ms, B = 51.08ms, saved = -590749.8ns (0.988x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.2%, 0.3%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 27s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
