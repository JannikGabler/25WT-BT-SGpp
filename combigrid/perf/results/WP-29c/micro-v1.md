A/B `micro`: A=`WP-29c-before` B=`WP-29c-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.13ms | 0.999 | [0.986, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 513.52us | 510.36us | 1.011 | [0.988, 1.034] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.13ms | 47.47ms | 0.995 | [0.990, 1.000] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.39ms | 1.39ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.57us | 119.26us | 0.992 | [0.982, 1.003] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 828.7ns | 825.8ns | 1.003 | [0.995, 1.010] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.35us | 37.41us | 0.998 | [0.989, 1.007] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.6ns | 1.007 | [1.001, 1.013] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.5ns | 196.7ns | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.67us | 42.55us | 0.999 | [0.988, 1.011] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 243.62us | 236.20us | 1.045 | [0.959, 1.137] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 206.1ns | 211.4ns | 0.972 | [0.958, 0.985] | SLOWER |

Sum of medians (call, common cases): A = 50.59ms, B = 50.93ms, saved = -338222.5ns (0.993x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.4%, 0.3%, 0.2%, 0.2%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 27s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
