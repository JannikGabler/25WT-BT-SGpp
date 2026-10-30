A/B `micro`: A=`WP-29a-before` B=`WP-29a-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.20ms | 6.24ms | 0.993 | [0.984, 1.002] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.00ms | 3.02ms | 0.992 | [0.980, 1.005] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.96ms | 14.00ms | 0.998 | [0.986, 1.010] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.19ms | 25.47ms | 0.988 | [0.978, 0.998] | SLOWER |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.96ms | 14.03ms | 0.995 | [0.984, 1.006] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 110.91ms | 109.03ms | 1.018 | [1.013, 1.023] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.67ms | 52.65ms | 1.009 | [0.983, 1.035] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.46ms | 3.49ms | 0.983 | [0.967, 1.001] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.65ms | 15.56ms | 1.002 | [0.983, 1.021] | ~ (noise) |

Sum of medians (call, common cases): A = 244.99ms, B = 243.50ms, saved = 1.50ms (1.006x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.2%, 0.2%, 0.2%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 23s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
