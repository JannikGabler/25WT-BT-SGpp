A/B `micro`: A=`WP-29d-before` B=`WP-29d-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 508.65us | 508.57us | 1.002 | [0.981, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.16ms | 47.19ms | 0.997 | [0.988, 1.006] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.38ms | 1.38ms | 1.002 | [0.981, 1.023] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.49us | 118.65us | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.47us | 42.49us | 1.000 | [0.995, 1.006] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 242.67us | 244.91us | 0.981 | [0.942, 1.022] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 208.4ns | 208.0ns | 1.003 | [0.998, 1.008] | ~ (noise) |

Sum of medians (call, common cases): A = 50.58ms, B = 50.61ms, saved = -29219.8ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.3%, 0.2%
Round durations (s): 4, 4, 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 19s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
