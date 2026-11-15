A/B `micro`: A=`WP-05-before` B=`WP-05-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 3.00ms | 0.992 | [0.981, 1.002] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.00ms | 14.17ms | 0.983 | [0.965, 1.001] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.04ms | 14.13ms | 0.994 | [0.986, 1.002] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.46ms | 3.48ms | 0.931 | [0.754, 1.149] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.72ms | 15.77ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.88ms | 7.75ms | 1.014 | [0.995, 1.034] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.47ms | 7.48ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.34ms | 65.46ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.72us | 123.86us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 910.69us | 911.03us | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.35us | 243.72us | 0.998 | [0.977, 1.020] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.27ms | 39.22ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 919.66us | 923.65us | 0.995 | [0.991, 0.999] | SLOWER |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.63us | 69.74us | 0.991 | [0.976, 1.006] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.69ms | 127.26ms | 0.996 | [0.990, 1.001] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.73ms | 61.69ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.31ms | 12.32ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.90ms | 12.91ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.49ms | 18.53ms | 0.996 | [0.989, 1.002] | ~ (noise) |

Sum of medians (call, common cases): A = 405.65ms, B = 406.56ms, saved = -905739.1ns (0.998x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 50s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
