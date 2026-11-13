A/B `micro`: A=`WP-52-after` B=`WP-52-types`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.66ms | 7.66ms | 0.997 | [0.987, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.25ms | 66.12ms | 1.007 | [0.988, 1.027] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.57us | 123.45us | 1.006 | [0.993, 1.019] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.09ms | 113.39ms | 1.000 | [0.991, 1.008] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 20.11ms | 21.50ms | 0.936 | [0.928, 0.943] | SLOWER |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.19ms | 9.33ms | 0.980 | [0.958, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.82ms | 56.93ms | 0.981 | [0.965, 0.998] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.80ms | 92.09ms | 0.984 | [0.974, 0.994] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.35ms | 283.46ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.32ms | 222.53ms | 0.996 | [0.983, 1.009] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.03ms | 35.06ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.84ms | 10.86ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.15ms | 0.997 | [0.993, 1.000] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.29ms | 5.30ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 416.03us | 419.13us | 0.994 | [0.989, 0.999] | SLOWER |

Sum of medians (call, common cases): A = 923.43ms, B = 927.93ms, saved = -4498336.0ns (0.995x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 15, 15, 15, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 77s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
