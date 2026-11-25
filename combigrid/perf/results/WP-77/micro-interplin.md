A/B `micro`: A=`WP-77-before` B=`WP-77-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interplin/sg/equi/b1/d10/L5 | call | 605.27ms | 606.17ms | 1.008 | [0.988, 1.027] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.81ms | 48.07ms | 0.993 | [0.984, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.78ms | 3.78ms | 1.006 | [0.986, 1.027] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.67us | 9.00us | 0.956 | [0.932, 0.981] | SLOWER |
| micro/interplin/sg/equi/b1/d1/L10 | call | 1.30us | 1.34us | 0.969 | [0.959, 0.978] | SLOWER |
| micro/interplin/sg/equi/b1/d2/L8 | call | 4.82us | 4.99us | 0.966 | [0.961, 0.970] | SLOWER |
| micro/interplin/sg/equi/b1/d3/L6 | call | 34.87us | 35.64us | 0.980 | [0.962, 0.999] | SLOWER |
| micro/interplin/sg/equi/b1/d3/L10 | call | 91.15us | 93.60us | 0.979 | [0.964, 0.994] | SLOWER |
| micro/interplin/sg/equi/b1/d4/L6 | call | 249.36us | 251.98us | 0.989 | [0.986, 0.993] | SLOWER |

Sum of medians (call, common cases): A = 657.26ms, B = 658.42ms, saved = -1157807.7ns (0.998x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.2%, 0.2%, 0.2%
Round durations (s): 9, 9, 9, 9, 9 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 44s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
