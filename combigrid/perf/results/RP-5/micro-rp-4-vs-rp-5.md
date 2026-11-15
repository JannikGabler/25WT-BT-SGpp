A/B `micro`: A=`rp-4` B=`rp-5`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.00ms | 5.91ms | 1.026 | [1.004, 1.049] | faster |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.00ms | 3.00ms | 1.000 | [0.980, 1.020] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.89ms | 14.14ms | 0.981 | [0.970, 0.993] | SLOWER |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.82ms | 5.92ms | 4.199 | [4.172, 4.226] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.90ms | 14.12ms | 0.985 | [0.981, 0.988] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 110.97ms | 106.95ms | 1.145 | [0.867, 1.512] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.82ms | 44.50ms | 1.189 | [1.183, 1.195] | faster |
| micro/quadrule/cc_weights/n33 | call | 2.13us | 1.93us | 1.104 | [1.097, 1.111] | faster |
| micro/quadrule/cc_weights/n1025 | call | 51.13us | 49.25us | 1.039 | [1.035, 1.043] | faster |
| micro/quadrule/cc_weights/n32769 | call | 4.37ms | 3.18ms | 1.375 | [1.370, 1.381] | faster |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.65ms | 7.52ms | 0.983 | [0.899, 1.075] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.02ms | 65.46ms | 1.007 | [0.999, 1.016] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.60us | 123.74us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 112.50ms | 97.75ms | 1.151 | [1.141, 1.162] | faster |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 22.21ms | 19.49ms | 1.148 | [1.121, 1.176] | faster |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.35ms | 9.01ms | 1.038 | [1.034, 1.042] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 57.45ms | 54.94ms | 1.054 | [1.034, 1.074] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.50ms | 90.45ms | 1.011 | [1.007, 1.016] | faster |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.13ms | 284.43ms | 0.995 | [0.991, 1.000] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 223.43ms | 228.10ms | 0.974 | [0.947, 1.002] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 200.84ms | 186.08ms | 1.080 | [0.986, 1.183] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 85.90ms | 86.49ms | 0.992 | [0.861, 1.144] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.95ms | 106.57ms | 0.813 | [0.725, 0.912] | SLOWER |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 33.36ms | 30.57ms | 1.101 | [1.061, 1.142] | faster |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 910.78us | 909.87us | 1.001 | [0.988, 1.014] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.46us | 243.43us | 1.000 | [0.979, 1.022] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.94ms | 39.33ms | 1.015 | [1.008, 1.022] | faster |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.02ms | 923.99us | 1.100 | [1.096, 1.104] | faster |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.51us | 69.73us | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.07ms | 35.12ms | 0.995 | [0.985, 1.005] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.85ms | 10.88ms | 0.996 | [0.992, 1.000] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.15ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 416.96us | 417.75us | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 139.78ms | 142.60ms | 0.985 | [0.973, 0.997] | SLOWER |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 14.61ms | 14.59ms | 0.989 | [0.954, 1.026] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 1.38ms | 1.37ms | 1.008 | [1.002, 1.014] | faster |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 8.58us | 8.24us | 1.037 | [1.023, 1.052] | faster |
| micro/quad/operator/cc/b0/d2/L16 | call | (not in A) | 122.18ms | - | - | new case |
| micro/quad/operator/equi/b0/d2/L16 | call | (not in A) | 101.73ms | - | - | new case |
| micro/quad/operator/cc/b1/d10/L5 | call | (not in A) | 44.41ms | - | - | new case |
| micro/quad/operator/equi/b1/d20/L4 | call | (not in A) | 106.76ms | - | - | new case |
| micro/gen/sg/cc/b1/d20/L5 | call | (not in A) | 11.62ms | - | - | new case |
| micro/gen/sg/cc/b0/d2/L20 | call | (not in A) | 138.22ms | - | - | new case |
| micro/gen/sg/equi/b0/d5/L10 | call | (not in A) | 92.90us | - | - | new case |
| micro/gen/dsg/equi/b1/d20/L6 | call | (not in A) | 16.96ms | - | - | new case |
| micro/gen/dsg/equi/b1/d15/L7 | call | (not in A) | 10.03ms | - | - | new case |
| micro/gen/dsg/equi/b0/d3/L16 | call | (not in A) | 1.81ms | - | - | new case |

Sum of medians (call, common cases): A = 1.762s, B = 1.720s, saved = 42.34ms (1.025x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.1%, 0.2%, 0.3%
Round durations (s): 31, 31, 31, 32, 31 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 156s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
