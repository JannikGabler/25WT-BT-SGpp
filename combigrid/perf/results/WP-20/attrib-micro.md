A/B `micro`: A=`WP-20-nsa` B=`WP-20-before`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.85ms | 13.90ms | 0.995 | [0.983, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.97ms | 7.90ms | 1.007 | [1.003, 1.011] | faster |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 69.37ms | 69.00ms | 1.005 | [0.996, 1.013] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.74ms | 2.80ms | 0.977 | [0.972, 0.981] | SLOWER |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 332.45ms | 326.45ms | 1.042 | [0.951, 1.142] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 170.00ms | 174.05ms | 0.975 | [0.971, 0.979] | SLOWER |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 23.07ms | 23.66ms | 0.976 | [0.969, 0.982] | SLOWER |

Sum of medians (call, common cases): A = 619.44ms, B = 617.77ms, saved = 1.66ms (1.003x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.6%, 0.2%, 0.2%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 52s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
