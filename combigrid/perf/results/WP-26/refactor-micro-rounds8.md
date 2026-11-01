A/B `micro`: A=`WP-26-before` B=`WP-26-after-r`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.96ms | 2.99ms | 0.989 | [0.968, 1.010] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.30ms | 109.30ms | 0.981 | [0.978, 0.984] | SLOWER |

Sum of medians (call, common cases): A = 110.27ms, B = 112.29ms, saved = -2020695.7ns (0.982x)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.2%, 0.1%, 0.1%, 0.1%, 0.2%, 0.2%, 0.2%
Round durations (s): 1, 1, 1, 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 11s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
