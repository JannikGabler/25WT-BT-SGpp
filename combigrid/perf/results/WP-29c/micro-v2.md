A/B `micro`: A=`WP-29c-before` B=`WP-29c-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.13ms | 1.13ms | 1.001 | [0.987, 1.014] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 512.96us | 510.05us | 1.017 | [0.984, 1.051] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.07ms | 47.46ms | 0.992 | [0.988, 0.996] | SLOWER |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.39ms | 1.39ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.55us | 119.38us | 0.992 | [0.986, 0.998] | SLOWER |
| micro/interp/barycentric_1d/generic/n9 | call | 828.2ns | 831.1ns | 0.996 | [0.987, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.43us | 37.39us | 1.003 | [0.994, 1.012] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.8ns | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.9ns | 196.5ns | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.60us | 42.51us | 0.993 | [0.970, 1.017] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 240.93us | 242.35us | 0.976 | [0.900, 1.057] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 206.1ns | 207.9ns | 0.991 | [0.982, 0.999] | SLOWER |

Sum of medians (call, common cases): A = 50.54ms, B = 50.93ms, saved = -391572.0ns (0.992x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.3%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 27s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
