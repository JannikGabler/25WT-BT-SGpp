A/B `micro`: A=`WP-83-before` B=`WP-83-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.94ms | 5.91ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 2.99ms | 0.989 | [0.965, 1.014] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.05ms | 14.04ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.93ms | 5.91ms | 1.003 | [0.996, 1.011] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.02ms | 14.06ms | 0.996 | [0.986, 1.006] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 106.97ms | 107.03ms | 1.003 | [0.994, 1.012] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.34ms | 44.49ms | 0.995 | [0.991, 0.999] | SLOWER |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.44ms | 112.31ms | 0.915 | [0.711, 1.176] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.61ms | 100.72ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.26ms | 44.28ms | 1.000 | [0.994, 1.007] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.82ms | 106.79ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 987.8ns | 966.4ns | 1.026 | [1.005, 1.047] | faster |
| micro/quadrule/cc_weights/n1025 | call | 27.75us | 27.86us | 0.996 | [0.989, 1.003] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 1.40ms | 1.40ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quadrule/cc_weights/n131073 | call | 6.23ms | 6.25ms | 0.997 | [0.992, 1.003] | ~ (noise) |
| micro/quadrule/cc_weights/n1048577 | call | 68.55ms | 68.10ms | 1.006 | [1.000, 1.011] | faster |
| micro/quadrule/cheb1_weights/n33 | call | (not in A) | 3.91us | - | - | new case |
| micro/quadrule/fejer1_weights/n33 | call | (not in A) | 4.62us | - | - | new case |
| micro/quadrule/cheb1_weights/n1025 | call | (not in A) | 114.54us | - | - | new case |
| micro/quadrule/fejer1_weights/n1025 | call | (not in A) | 207.54us | - | - | new case |
| micro/quadrule/cheb1_weights/n32769 | call | (not in A) | 7.28ms | - | - | new case |
| micro/quadrule/fejer1_weights/n32769 | call | (not in A) | 11.66ms | - | - | new case |
| micro/quadrule/cheb1_weights/n131073 | call | (not in A) | 29.88ms | - | - | new case |
| micro/quadrule/fejer1_weights/n131073 | call | (not in A) | 54.70ms | - | - | new case |
| micro/quadrule/cheb1_weights/n1048577 | call | (not in A) | 330.33ms | - | - | new case |
| micro/quadrule/fejer1_weights/n1048577 | call | (not in A) | 707.74ms | - | - | new case |
| micro/quadrule/fejer1_weights/n131071 | call | (not in A) | 28.62ms | - | - | new case |
| micro/quad/operator/cheb1/b0/d2/L16 | call | (not in A) | 156.43ms | - | - | new case |
| micro/quad/operator/cheb1/b0/d3/L8 | call | (not in A) | 3.41ms | - | - | new case |
| micro/quad/operator/cc/b0/d3/L8 | call | (not in A) | 3.33ms | - | - | new case |
| micro/quad/operator/cheb1/b1/d5/L6 | call | (not in A) | 4.80ms | - | - | new case |
| micro/quad/operator/cc/b1/d5/L6 | call | (not in A) | 4.76ms | - | - | new case |

Sum of medians (call, common cases): A = 634.57ms, B = 634.31ms, saved = 256.78us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 17, 17, 17, 17, 18 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 87s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
