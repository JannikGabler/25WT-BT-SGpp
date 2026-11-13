A/B `micro`: A=`WP-52-after` B=`WP-52-types-inline`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.69ms | 7.69ms | 0.993 | [0.971, 1.016] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.21ms | 66.27ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.52us | 123.64us | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 112.94ms | 112.74ms | 1.001 | [0.992, 1.010] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.94ms | 20.09ms | 0.955 | [0.816, 1.117] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.20ms | 9.16ms | 1.005 | [1.000, 1.011] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.26ms | 55.47ms | 0.997 | [0.989, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.57ms | 91.24ms | 0.992 | [0.986, 0.998] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.92ms | 282.96ms | 0.996 | [0.984, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.34ms | 222.82ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.10ms | 35.09ms | 0.995 | [0.978, 1.012] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.83ms | 10.84ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.14ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 415.91us | 416.37us | 1.005 | [0.986, 1.024] | ~ (noise) |

Sum of medians (call, common cases): A = 921.98ms, B = 923.36ms, saved = -1379928.7ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 15, 15, 15, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 77s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
