A/B `micro`: A=`WP-01-after` B=`WP-01-split`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.92ms | 6.88ms | 1.005 | [1.001, 1.009] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.04ms | 3.04ms | 0.994 | [0.981, 1.007] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.00ms | 13.95ms | 1.021 | [0.970, 1.076] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.78ms | 25.68ms | 1.001 | [0.994, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.02ms | 13.96ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 110.95ms | 111.26ms | 0.982 | [0.939, 1.027] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 53.23ms | 53.03ms | 1.006 | [0.998, 1.015] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.55ms | 3.57ms | 0.985 | [0.950, 1.021] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.83ms | 15.84ms | 0.999 | [0.990, 1.007] | ~ (noise) |

Sum of medians (call, common cases): A = 247.32ms, B = 247.21ms, saved = 116.73us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.6%, 0.3%, 0.3%, 0.2%, 0.2%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 23s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
