A/B `micro`: A=`WP-61-after-v1` B=`WP-61-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/compensated_combination/equi/b1/d15/L6 | call | 119.87us | 119.38us | 1.005 | [1.001, 1.008] | faster |
| micro/quad/compensated_combination/equi/b1/d20/L5 | call | 117.43us | 117.36us | 1.000 | [0.989, 1.011] | ~ (noise) |

Sum of medians (call, common cases): A = 237.30us, B = 236.74us, saved = 556.7ns (1.002x)
Foreign CPU load per round (other processes, share of all CPUs): 1.1%, 0.8%, 0.2%, 0.1%, 0.0%
Round durations (s): 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 4s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
