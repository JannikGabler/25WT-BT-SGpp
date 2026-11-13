A/B `micro`: A=`WP-52-after` B=`WP-52-types-inline`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.92ms | 19.93ms | 1.003 | [0.985, 1.022] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.17ms | 9.17ms | 1.001 | [0.993, 1.010] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.59ms | 55.06ms | 1.014 | [0.999, 1.029] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.95ms | 91.06ms | 1.002 | [0.995, 1.010] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.16ms | 283.54ms | 0.997 | [0.992, 1.001] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.69ms | 222.42ms | 1.003 | [0.997, 1.009] | ~ (noise) |

Sum of medians (call, common cases): A = 681.47ms, B = 681.18ms, saved = 291.97us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.3%, 0.2%, 0.2%, 0.3%, 0.2%, 0.2%
Round durations (s): 9, 9, 9, 9, 9, 9, 9, 9 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 72s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
