A/B `micro`: A=`WP-20-nsa` B=`WP-20-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.77ms | 2.72ms | 1.018 | [0.997, 1.040] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 171.88ms | 168.03ms | 1.021 | [1.011, 1.032] | faster |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 23.30ms | 22.99ms | 1.012 | [0.999, 1.025] | ~ (noise) |

Sum of medians (call, common cases): A = 197.95ms, B = 193.74ms, saved = 4.21ms (1.022x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.3%, 0.2%, 0.3%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 27s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
