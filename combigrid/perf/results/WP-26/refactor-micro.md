A/B `micro`: A=`WP-26-before` B=`WP-26-after-r`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.30ms | 6.18ms | 1.020 | [1.016, 1.024] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.96ms | 3.00ms | 0.985 | [0.970, 0.999] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.93ms | 13.91ms | 0.982 | [0.926, 1.042] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.25ms | 25.15ms | 0.949 | [0.805, 1.119] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.92ms | 13.91ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.13ms | 109.08ms | 0.970 | [0.943, 0.999] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.43ms | 52.66ms | 0.994 | [0.987, 1.001] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.66ms | 7.66ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.27ms | 66.05ms | 1.011 | [0.971, 1.052] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.49us | 122.83us | 1.005 | [1.002, 1.009] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.81ms | 54.78ms | 1.002 | [0.993, 1.011] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.17ms | 91.04ms | 1.007 | [0.986, 1.029] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 284.35ms | 282.82ms | 1.002 | [0.992, 1.013] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 1.001 | [0.988, 1.015] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 510.84us | 504.45us | 1.012 | [0.991, 1.034] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.85ms | 46.61ms | 1.001 | [0.990, 1.012] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.37ms | 1.36ms | 1.008 | [1.000, 1.016] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.95us | 118.36us | 1.007 | [1.001, 1.012] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.44us | 41.89us | 1.013 | [1.008, 1.019] | faster |
| micro/interplin/tg/equi/b1/d10/L5 | call | 249.69us | 234.01us | 1.081 | [1.036, 1.127] | faster |
| micro/interplin/tg/equi/b1/d2/L16 | call | 211.8ns | 208.2ns | 1.021 | [1.006, 1.035] | faster |

Sum of medians (call, common cases): A = 776.58ms, B = 776.34ms, saved = 230.51us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.3%, 0.2%, 0.2%
Round durations (s): 15, 15, 15, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 75s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
