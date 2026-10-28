A/B `micro`: A=`WP-20-before` B=`WP-20-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.84ms | 13.88ms | 0.997 | [0.985, 1.009] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.91ms | 7.92ms | 0.996 | [0.987, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.90ms | 69.00ms | 0.947 | [0.826, 1.086] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.81ms | 2.71ms | 1.034 | [1.026, 1.042] | faster |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 328.66ms | 329.18ms | 0.997 | [0.986, 1.007] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 173.40ms | 167.76ms | 1.045 | [1.007, 1.084] | faster |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 23.68ms | 22.96ms | 1.036 | [1.019, 1.053] | faster |

Sum of medians (call, common cases): A = 619.20ms, B = 613.41ms, saved = 5.79ms (1.009x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.4%, 0.3%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 52s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
