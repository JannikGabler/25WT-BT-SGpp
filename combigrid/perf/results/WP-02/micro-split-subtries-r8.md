A/B `micro`: A=`WP-02-after` B=`WP-02-split`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.95ms | 20.11ms | 0.994 | [0.977, 1.012] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.05ms | 9.30ms | 0.973 | [0.958, 0.988] | SLOWER |

Sum of medians (call, common cases): A = 29.00ms, B = 29.41ms, saved = -407562.3ns (0.986x)
Foreign CPU load per round (other processes, share of all CPUs): 1.0%, 0.1%, 0.4%, 0.2%, 1.0%, 0.4%, 0.2%, 0.4%
Round durations (s): 2, 2, 2, 2, 2, 2, 2, 2 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 16s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
