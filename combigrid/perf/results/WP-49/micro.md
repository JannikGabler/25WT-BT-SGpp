A/B `micro`: A=`WP-49-before` B=`WP-49-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interplin/sg/equi/b1/d10/L5 | call | 604.76ms | 605.31ms | 1.002 | [0.992, 1.012] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.78ms | 47.97ms | 0.995 | [0.988, 1.002] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.77ms | 3.77ms | 1.001 | [0.994, 1.007] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.58us | 8.57us | 1.001 | [0.988, 1.014] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 139.63ms | 88.45ms | 1.579 | [1.568, 1.589] | faster |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 14.47ms | 9.02ms | 1.615 | [1.575, 1.657] | faster |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 1.36ms | 697.96us | 1.965 | [1.894, 2.039] | faster |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 8.19us | 7.63us | 1.069 | [1.057, 1.081] | faster |
| micro/interplin/sg_vec/equi/b1/d8/L5/m4 | call | 193.42ms | 192.38ms | 1.008 | [1.000, 1.016] | ~ (noise) |
| micro/interplin/dsg_vec/equi/b1/d8/L5/m4 | call | 38.47ms | 32.22ms | 1.208 | [1.180, 1.236] | faster |
| micro/interplin/sg_vec/equi/b1/d2/L16/m4 | call | 25.04us | 25.08us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/interplin/dsg_vec/equi/b1/d2/L16/m4 | call | 16.27us | 15.54us | 1.047 | [1.030, 1.064] | faster |

Sum of medians (call, common cases): A = 1.044s, B = 979.88ms, saved = 63.85ms (1.065x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.4%, 0.3%, 0.4%
Round durations (s): 12, 12, 12, 12, 12 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 59s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
