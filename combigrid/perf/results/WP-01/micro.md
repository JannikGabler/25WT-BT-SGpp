A/B `micro`: A=`WP-01-before` B=`WP-01-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 13.20ms | 6.92ms | 1.909 | [1.901, 1.916] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 7.56ms | 3.04ms | 2.480 | [2.448, 2.513] | faster |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 47.40ms | 14.01ms | 3.367 | [3.255, 3.482] | faster |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 31.86ms | 25.92ms | 1.222 | [1.201, 1.243] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 47.21ms | 13.97ms | 3.391 | [3.343, 3.441] | faster |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 536.42ms | 111.22ms | 4.790 | [4.668, 4.915] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 176.61ms | 53.21ms | 3.364 | [3.166, 3.574] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.56ms | 3.55ms | 0.999 | [0.980, 1.018] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.86ms | 15.88ms | 1.053 | [0.908, 1.221] | ~ (noise) |

Sum of medians (call, common cases): A = 879.68ms, B = 247.71ms, saved = 631.96ms (3.551x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.5%, 0.6%, 0.4%, 0.4%
Round durations (s): 7, 7, 7, 7, 7 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 36s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
