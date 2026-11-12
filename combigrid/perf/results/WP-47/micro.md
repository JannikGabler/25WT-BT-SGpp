A/B `micro`: A=`WP-47-before` B=`WP-47-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.40us | 36.41us | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 178.22us | 177.77us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 203.2ns | 212.7ns | 0.950 | [0.941, 0.959] | SLOWER |
| micro/interplin/tg/equi/b1/d5/L8 | call | 2.99us | 3.00us | 0.995 | [0.991, 0.999] | SLOWER |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.74ms | 23.66ms | 1.001 | [0.992, 1.011] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 606.27ms | 606.10ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.83ms | 47.87ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.74ms | 3.76ms | 0.985 | [0.957, 1.015] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.49us | 8.52us | 0.998 | [0.990, 1.006] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | (not in A) | 140.03ms | - | - | new case |
| micro/interplin/dsg/equi/b1/d8/L5 | call | (not in A) | 14.57ms | - | - | new case |
| micro/interplin/dsg/equi/b1/d5/L8 | call | (not in A) | 1.38ms | - | - | new case |
| micro/interplin/dsg/equi/b1/d2/L16 | call | (not in A) | 8.54us | - | - | new case |
| micro/interplin/sg_vec/equi/b1/d8/L5/m4 | call | (not in A) | 193.36ms | - | - | new case |
| micro/interplin/dsg_vec/equi/b1/d8/L5/m4 | call | (not in A) | 38.62ms | - | - | new case |
| micro/interplin/sg_vec/equi/b1/d2/L16/m4 | call | (not in A) | 24.50us | - | - | new case |
| micro/interplin/dsg_vec/equi/b1/d2/L16/m4 | call | (not in A) | 16.63us | - | - | new case |

Sum of medians (call, common cases): A = 681.81ms, B = 681.61ms, saved = 197.65us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.3%, 0.3%, 0.3%, 0.3%
Round durations (s): 12, 12, 12, 12, 12 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 59s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
