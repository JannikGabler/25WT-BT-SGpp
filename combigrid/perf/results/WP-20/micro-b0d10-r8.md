A/B `micro`: A=`WP-20-before` B=`WP-20-after`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.85ms | 68.91ms | 1.000 | [0.959, 1.043] | ~ (noise) |

Sum of medians (call, common cases): A = 68.85ms, B = 68.91ms, saved = -59190.0ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.5%, 0.7%, 0.3%, 0.1%, 0.3%, 0.4%, 0.4%
Round durations (s): 1, 1, 1, 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 5s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
