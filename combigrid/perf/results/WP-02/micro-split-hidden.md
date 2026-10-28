A/B `micro`: A=`WP-02-after` B=`WP-02-split-hidden`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.51us | 124.15us | 0.998 | [0.989, 1.007] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 20.02ms | 19.92ms | 1.002 | [0.992, 1.013] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.04ms | 9.14ms | 0.992 | [0.983, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.94ms | 55.29ms | 1.024 | [0.982, 1.068] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.88ms | 91.25ms | 0.994 | [0.986, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 284.23ms | 287.26ms | 0.993 | [0.987, 0.999] | SLOWER |

Sum of medians (call, common cases): A = 460.23ms, B = 462.98ms, saved = -2754557.7ns (0.994x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.5%, 1.4%, 0.5%, 0.2%, 0.3%, 0.3%
Round durations (s): 8, 8, 8, 9, 9, 9, 8, 8 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 68s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
