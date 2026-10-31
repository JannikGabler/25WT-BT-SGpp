A/B `micro`: A=`WP-26-before` B=`WP-26-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.93ms | 113.72ms | 1.002 | [0.993, 1.011] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.77ms | 20.29ms | 0.984 | [0.955, 1.014] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.24ms | 9.20ms | 1.006 | [1.002, 1.009] | faster |

Sum of medians (call, common cases): A = 142.94ms, B = 143.21ms, saved = -269259.5ns (0.998x)
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.1%, 0.2%, 0.8%, 5.8%
Round durations (s): 4, 4, 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
**WARN: other processes used > 2% of the CPUs in some rounds; the comparison may be distorted - find the cause and repeat.**
Duration: 18s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
