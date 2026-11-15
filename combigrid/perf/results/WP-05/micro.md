A/B `micro`: A=`WP-05-before` B=`WP-05-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.94ms | 5.92ms | 1.011 | [0.990, 1.032] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 3.00ms | 0.994 | [0.989, 0.998] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.01ms | 14.12ms | 0.986 | [0.973, 0.999] | SLOWER |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.64ms | 5.92ms | 4.164 | [4.144, 4.184] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.03ms | 14.15ms | 0.991 | [0.983, 0.999] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.52ms | 107.11ms | 1.022 | [1.017, 1.027] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.72ms | 44.43ms | 1.187 | [1.176, 1.198] | faster |
| micro/quad/operator/cc/b0/d2/L16 | call | 172.05ms | 122.11ms | 1.409 | [1.404, 1.415] | faster |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.84ms | 101.66ms | 0.999 | [0.979, 1.019] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 52.35ms | 44.37ms | 1.194 | [1.163, 1.226] | faster |
| micro/quad/operator/equi/b1/d20/L4 | call | 109.41ms | 106.79ms | 1.034 | [1.015, 1.054] | faster |
| micro/quadrule/cc_weights/n33 | call | 2.14us | 1.93us | 1.104 | [1.089, 1.119] | faster |
| micro/quadrule/cc_weights/n1025 | call | 51.17us | 48.99us | 1.043 | [1.039, 1.048] | faster |
| micro/quadrule/cc_weights/n32769 | call | 4.38ms | 3.17ms | 1.382 | [1.376, 1.387] | faster |

Sum of medians (call, common cases): A = 662.93ms, B = 572.79ms, saved = 90.14ms (1.157x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.3%, 0.3%, 0.2%
Round durations (s): 9, 9, 9, 9, 9 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 43s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
