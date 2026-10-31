(Variant V1, `updateCoeffSums` defined in the .cpp; snapshot `WP-29b-after` of V1, renamed `WP-29b-v1b` afterwards. Not the final state, see the summary.)

A/B `micro`: A=`WP-29b-before` B=`WP-29b-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.69ms | 7.65ms | 1.008 | [0.995, 1.021] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.27ms | 65.90ms | 1.005 | [0.992, 1.017] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.25us | 123.07us | 0.992 | [0.946, 1.041] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.30ms | 113.39ms | 0.999 | [0.990, 1.008] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.74ms | 19.68ms | 1.000 | [0.982, 1.019] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 8.89ms | 9.18ms | 0.975 | [0.952, 0.999] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.13ms | 55.14ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.08ms | 91.88ms | 0.994 | [0.987, 1.001] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.59ms | 282.25ms | 1.009 | [0.992, 1.027] | ~ (noise) |

Sum of medians (call, common cases): A = 645.81ms, B = 645.20ms, saved = 611.93us (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.5%, 0.7%, 0.6%, 0.5%
Round durations (s): 11, 11, 11, 11, 11 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 56s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
