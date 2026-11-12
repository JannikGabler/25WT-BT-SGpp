A/B `micro`: A=`WP-45-before` B=`WP-45-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.64ms | 7.65ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.99ms | 65.92ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.36us | 123.44us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 112.35ms | 112.59ms | 0.999 | [0.990, 1.008] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.81ms | 20.19ms | 0.987 | [0.965, 1.009] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.12ms | 9.12ms | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.86ms | 55.14ms | 1.001 | [0.970, 1.033] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.59ms | 90.74ms | 0.997 | [0.991, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.33ms | 282.86ms | 0.994 | [0.976, 1.011] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.83ms | 222.48ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 161.98ms | 123.99ms | 1.101 | [0.933, 1.299] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 85.76ms | 85.70ms | 1.060 | [0.945, 1.189] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 86.66ms | 85.81ms | 1.062 | [0.956, 1.179] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 31.23ms | 31.31ms | 1.000 | [0.984, 1.015] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 1.003 | [0.990, 1.016] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 509.07us | 515.99us | 0.985 | [0.965, 1.006] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.58ms | 46.68ms | 0.991 | [0.975, 1.007] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.37ms | 1.36ms | 1.003 | [1.000, 1.007] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 119.11us | 120.69us | 0.985 | [0.979, 0.991] | SLOWER |
| micro/interp/sg/cc/b0/d8/L2 | call | 155.81ms | 156.12ms | 1.000 | [0.988, 1.013] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 74.19ms | 74.07ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 14.78ms | 14.77ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 16.64ms | 16.65ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 3.25ms | 3.26ms | 1.000 | [0.993, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 824.2ns | 832.4ns | 0.988 | [0.981, 0.995] | SLOWER |
| micro/interp/barycentric_1d/generic/n65 | call | 37.38us | 37.39us | 1.001 | [0.992, 1.011] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 32.0ns | 31.8ns | 1.003 | [0.993, 1.013] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.6ns | 196.6ns | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | (not in A) | 35.09ms | - | - | new case |
| micro/interp/dsg/cc/b0/d5/L5 | call | (not in A) | 10.85ms | - | - | new case |
| micro/interp/dsg/cc/b0/d3/L8 | call | (not in A) | 3.14ms | - | - | new case |
| micro/interp/dsg/cc/b0/d2/L12 | call | (not in A) | 5.30ms | - | - | new case |
| micro/interp/dsg/equi/b0/d3/L5 | call | (not in A) | 417.08us | - | - | new case |
| micro/interp/sg_vec/cc/b0/d5/L5/m4 | call | (not in A) | 262.22ms | - | - | new case |
| micro/interp/dsg_vec/cc/b0/d5/L5/m4 | call | (not in A) | 38.48ms | - | - | new case |
| micro/interp/sg_vec/cc/b0/d2/L12/m4 | call | (not in A) | 55.29ms | - | - | new case |
| micro/interp/dsg_vec/cc/b0/d2/L12/m4 | call | (not in A) | 19.67ms | - | - | new case |

Sum of medians (call, common cases): A = 1.546s, B = 1.508s, saved = 37.37ms (1.025x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 28, 28, 28, 28, 28 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 140s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
