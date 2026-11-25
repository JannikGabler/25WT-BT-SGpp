A/B `micro`: A=`WP-77-before` B=`WP-77-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/operator/cc/b0/d2/L16 | call | 112.28ms | 112.17ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.59ms | 100.65ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.20ms | 44.15ms | 1.003 | [0.992, 1.013] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.82ms | 106.98ms | 0.996 | [0.984, 1.008] | ~ (noise) |
| micro/dsg/operator/equi/b0/d3/L12 | call | 25.27ms | 25.32ms | 0.996 | [0.990, 1.003] | ~ (noise) |
| micro/dsg/operator/equi/b0/d5/L8 | call | 49.54ms | 49.80ms | 1.004 | [0.969, 1.041] | ~ (noise) |
| micro/dsg/operator/equi/b1/d15/L4 | call | 20.56ms | 20.51ms | 1.002 | [0.996, 1.007] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 156.54ms | 156.81ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.41ms | 3.41ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.32ms | 3.32ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.78ms | 4.73ms | 1.007 | [1.000, 1.015] | faster |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.76ms | 4.76ms | 1.004 | [0.982, 1.027] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.80ms | 127.09ms | 0.996 | [0.992, 1.001] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.65ms | 62.20ms | 0.991 | [0.987, 0.994] | SLOWER |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.30ms | 12.31ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.90ms | 12.92ms | 0.996 | [0.990, 1.002] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.47ms | 18.58ms | 0.995 | [0.985, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d1/L6 | call | 4.40us | 4.37us | 0.994 | [0.959, 1.030] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L4 | call | 26.47us | 26.61us | 0.994 | [0.991, 0.998] | SLOWER |
| micro/interp/sg/cc/b0/d3/L3 | call | 113.58us | 114.18us | 0.996 | [0.992, 0.999] | SLOWER |
| micro/interplin/sg/equi/b1/d10/L5 | call | 605.24ms | 605.06ms | 1.004 | [0.994, 1.015] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.96ms | 47.95ms | 1.001 | [0.994, 1.007] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.77ms | 3.78ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.68us | 8.70us | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/interplin/sg/equi/b1/d1/L10 | call | 1.30us | 1.29us | 1.007 | [0.999, 1.015] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L8 | call | 4.86us | 4.82us | 1.005 | [0.990, 1.020] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L6 | call | 34.87us | 34.82us | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L10 | call | 91.07us | 90.92us | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interplin/sg/equi/b1/d4/L6 | call | 249.07us | 249.08us | 0.987 | [0.950, 1.026] | ~ (noise) |
| micro/gen/sg/cc/b1/d20/L5 | call | 11.57ms | 12.01ms | 0.963 | [0.960, 0.967] | SLOWER |
| micro/gen/sg/cc/b0/d2/L20 | call | 138.84ms | 138.83ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/gen/sg/equi/b0/d5/L10 | call | 94.00us | 117.26us | 0.801 | [0.795, 0.807] | SLOWER |
| micro/gen/dsg/equi/b1/d20/L6 | call | 16.77ms | 18.86ms | 0.886 | [0.871, 0.902] | SLOWER |
| micro/gen/dsg/equi/b1/d15/L7 | call | 10.14ms | 10.87ms | 0.933 | [0.928, 0.937] | SLOWER |
| micro/gen/dsg/equi/b0/d3/L16 | call | 1.80ms | 1.81ms | 0.994 | [0.987, 1.001] | ~ (noise) |
| micro/gen/sg/equi/b1/d10/L7 | call | 2.33ms | 2.42ms | 0.961 | [0.952, 0.971] | SLOWER |

Sum of medians (call, common cases): A = 1.704s, B = 1.709s, saved = -4722637.8ns (0.997x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 26, 26, 26, 26, 26 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 129s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
