A/B `micro`: A=`WP-02-after` B=`WP-02-split`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.72us | 123.43us | 1.005 | [0.996, 1.014] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.80ms | 20.06ms | 0.987 | [0.975, 0.998] | SLOWER |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.14ms | 9.22ms | 0.989 | [0.975, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.41ms | 55.28ms | 0.998 | [0.987, 1.009] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.87ms | 91.70ms | 0.989 | [0.976, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 284.28ms | 284.99ms | 0.996 | [0.990, 1.002] | ~ (noise) |

Sum of medians (call, common cases): A = 459.63ms, B = 461.37ms, saved = -1745494.4ns (0.996x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.5%, 0.2%, 0.3%, 0.6%
Round durations (s): 9, 8, 8, 8, 9 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 43s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
