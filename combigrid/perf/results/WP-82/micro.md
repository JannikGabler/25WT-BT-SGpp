A/B `micro`: A=`WP-82-before` B=`WP-82-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/operator/cc/b0/d2/L16 | call | 112.00ms | 112.19ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.71ms | 100.58ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.14ms | 44.20ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.71ms | 106.76ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c0 | call | 3.72ms | 3.67ms | 1.011 | [0.993, 1.028] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c100 | call | 9.06ms | 8.96ms | 1.013 | [1.005, 1.022] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c1000 | call | 99.73ms | 100.14ms | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/expensive/quad_scalar_x_m/dsg/cc/b1/d7/L4/m4/c1us | call | 79.52ms | 79.20ms | 1.004 | [1.000, 1.007] | ~ (noise) |
| micro/expensive/quad_vec/dsg/cc/b1/d7/L4/m4/c1us | call | 22.39ms | 22.33ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d2/L20 | call | 258.12ms | 257.85ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/crosscall/quad_sg/call/cc/b0/d2/L20 | call | 2.206s | 2.209s | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d2/L20 | call | 1.946s | 1.946s | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d5/L10 | call | 118.88us | 118.65us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/crosscall/quad_sg/call/cc/b0/d5/L10 | call | 2.251s | 2.250s | 0.976 | [0.935, 1.019] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d5/L10 | call | 2.251s | 2.249s | 1.001 | [0.993, 1.008] | ~ (noise) |
| micro/crosscall/quad_sg/build/equi/b1/d20/L4 | call | 4.98us | 4.96us | 1.014 | [0.987, 1.042] | ~ (noise) |
| micro/crosscall/quad_sg/call/equi/b1/d20/L4 | call | 106.78ms | 106.80ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/equi/b1/d20/L4 | call | 106.82ms | 107.14ms | 0.996 | [0.990, 1.002] | ~ (noise) |
| micro/crosscall/quad_dsg/build/cc/b0/d5/L10 | call | 1.21ms | 1.22ms | 0.994 | [0.991, 0.998] | SLOWER |
| micro/crosscall/quad_dsg/call/cc/b0/d5/L10 | call | 303.47ms | 302.04ms | 1.005 | [1.001, 1.009] | faster |
| micro/crosscall/quad_dsg/call_flipped/cc/b0/d5/L10 | call | 302.26ms | 303.00ms | 1.000 | [0.994, 1.005] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b1/d15/L5 | call | 4.04ms | 4.04ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | call | 190.96ms | 185.30ms | 1.031 | [1.027, 1.035] | faster |
| micro/crosscall/quad_dsg/call_flipped/equi/b1/d15/L5 | call | 186.18ms | 190.63ms | 0.975 | [0.971, 0.980] | SLOWER |
| micro/crosscall/quad_dsg/build/equi/b0/d3/L14 | call | 4.06ms | 4.06ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/crosscall/quad_dsg/call/equi/b0/d3/L14 | call | 124.25ms | 120.10ms | 1.044 | [1.019, 1.070] | faster |
| micro/crosscall/quad_dsg/call_flipped/equi/b0/d3/L14 | call | 120.94ms | 124.30ms | 0.974 | [0.970, 0.977] | SLOWER |

Sum of medians (call, common cases): A = 10.940s, B = 10.939s, saved = 1.17ms (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 96, 95, 95, 95, 96 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 478s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
