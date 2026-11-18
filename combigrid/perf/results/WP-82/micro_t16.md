A/B `micro`: A=`WP-82-before` B=`WP-82-after`, 16 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c0 | call | 550.82us | 477.78us | 1.149 | [1.136, 1.162] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c100 | call | 1.24ms | 1.16ms | 1.056 | [1.026, 1.086] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c1000 | call | 8.89ms | 8.82ms | 1.002 | [0.986, 1.019] | ~ (noise) |
| micro/expensive/quad_scalar_x_m/dsg/cc/b1/d7/L4/m4/c1us | call | 5.97ms | 5.65ms | 1.036 | [0.982, 1.093] | ~ (noise) |
| micro/expensive/quad_vec/dsg/cc/b1/d7/L4/m4/c1us | call | 1.70ms | 1.96ms | 1.032 | [0.449, 2.374] | ~ (noise) |
| micro/crosscall/quad_dsg/call/cc/b0/d5/L10 | call | 36.56ms | 35.08ms | 1.043 | [1.039, 1.048] | faster |
| micro/crosscall/quad_dsg/call_flipped/cc/b0/d5/L10 | call | 35.02ms | 36.56ms | 0.958 | [0.955, 0.962] | SLOWER |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | call | 25.39ms | 19.50ms | 1.308 | [1.293, 1.324] | faster |
| micro/crosscall/quad_dsg/call_flipped/equi/b1/d15/L5 | call | 19.46ms | 25.34ms | 0.772 | [0.761, 0.783] | SLOWER |
| micro/crosscall/quad_dsg/call/equi/b0/d3/L14 | call | 20.47ms | 15.61ms | 1.311 | [1.304, 1.317] | faster |
| micro/crosscall/quad_dsg/call_flipped/equi/b0/d3/L14 | call | 15.88ms | 20.56ms | 0.775 | [0.743, 0.807] | SLOWER |

Sum of medians (call, common cases): A = 171.14ms, B = 170.74ms, saved = 406.49us (1.002x)
Foreign CPU load per round (other processes, share of all CPUs): 0.0%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 7, 6, 6, 6, 6 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 32s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
