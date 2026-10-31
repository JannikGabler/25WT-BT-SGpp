A/B `micro`: A=`WP-29b-before` B=`WP-29b-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.72ms | 7.72ms | 1.001 | [0.990, 1.012] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 67.10ms | 66.39ms | 1.012 | [0.998, 1.026] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 124.10us | 124.20us | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.73ms | 114.00ms | 1.000 | [0.991, 1.009] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 20.04ms | 19.81ms | 1.040 | [0.957, 1.131] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 8.99ms | 8.88ms | 1.013 | [0.996, 1.031] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.50ms | 55.34ms | 1.000 | [0.983, 1.017] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.26ms | 91.31ms | 0.999 | [0.986, 1.011] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 285.33ms | 284.40ms | 1.006 | [0.996, 1.016] | ~ (noise) |

Sum of medians (call, common cases): A = 649.78ms, B = 647.98ms, saved = 1.80ms (1.003x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 11, 11, 11, 11, 11 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 56s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
