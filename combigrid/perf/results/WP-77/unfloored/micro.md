A/B `micro`: A=`WP-77-before` B=`WP-77-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/operator/cc/b0/d2/L16 | call | 112.02ms | 112.08ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.42ms | 100.54ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.10ms | 44.82ms | 0.972 | [0.934, 1.012] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.75ms | 106.94ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| micro/dsg/operator/equi/b0/d3/L12 | call | 25.34ms | 25.25ms | 1.004 | [0.995, 1.012] | ~ (noise) |
| micro/dsg/operator/equi/b0/d5/L8 | call | 49.49ms | 49.69ms | 0.996 | [0.989, 1.002] | ~ (noise) |
| micro/dsg/operator/equi/b1/d15/L4 | call | 20.51ms | 20.56ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 156.59ms | 156.65ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.41ms | 3.42ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.32ms | 3.31ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.79ms | 4.76ms | 1.006 | [0.998, 1.014] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.76ms | 4.76ms | 0.997 | [0.993, 1.002] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.90ms | 126.91ms | 0.979 | [0.916, 1.046] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.61ms | 62.03ms | 0.997 | [0.988, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.32ms | 12.33ms | 0.989 | [0.963, 1.017] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.88ms | 12.92ms | 0.997 | [0.993, 1.000] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.005 | [0.986, 1.024] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.38ms | 18.56ms | 0.994 | [0.983, 1.006] | ~ (noise) |
| micro/interp/sg/cc/b0/d1/L6 | call | 4.37us | 4.37us | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L4 | call | 26.48us | 26.68us | 0.992 | [0.987, 0.997] | SLOWER |
| micro/interp/sg/cc/b0/d3/L3 | call | 113.63us | 113.61us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 605.25ms | 605.22ms | 0.996 | [0.985, 1.007] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.90ms | 48.08ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.77ms | 3.78ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.70us | 8.70us | 1.001 | [0.992, 1.010] | ~ (noise) |
| micro/interplin/sg/equi/b1/d1/L10 | call | 1.29us | 1.29us | 1.006 | [0.984, 1.028] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L8 | call | 4.81us | 4.82us | 0.998 | [0.989, 1.008] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L6 | call | 34.86us | 34.90us | 0.996 | [0.990, 1.002] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L10 | call | 91.16us | 90.84us | 1.004 | [0.998, 1.010] | ~ (noise) |
| micro/interplin/sg/equi/b1/d4/L6 | call | 249.50us | 249.15us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/gen/sg/cc/b1/d20/L5 | call | 11.61ms | 12.20ms | 0.955 | [0.944, 0.967] | SLOWER |
| micro/gen/sg/cc/b0/d2/L20 | call | 138.70ms | 138.67ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/gen/sg/equi/b0/d5/L10 | call | 94.21us | 114.70us | 0.817 | [0.803, 0.830] | SLOWER |
| micro/gen/dsg/equi/b1/d20/L6 | call | 16.77ms | 19.70ms | 0.851 | [0.845, 0.856] | SLOWER |
| micro/gen/dsg/equi/b1/d15/L7 | call | 10.15ms | 11.69ms | 0.867 | [0.858, 0.876] | SLOWER |
| micro/gen/dsg/equi/b0/d3/L16 | call | 1.80ms | 1.80ms | 0.994 | [0.988, 1.001] | ~ (noise) |
| micro/gen/sg/equi/b1/d10/L7 | call | 2.32ms | 2.52ms | 0.922 | [0.915, 0.929] | SLOWER |

Sum of medians (call, common cases): A = 1.704s, B = 1.711s, saved = -7379735.3ns (0.996x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 26, 26, 26, 26, 26 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 130s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
