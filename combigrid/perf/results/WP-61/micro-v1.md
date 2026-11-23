A/B `micro`: A=`WP-61-before` B=`WP-61-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.93ms | 5.92ms | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.97ms | 2.97ms | 1.007 | [0.988, 1.027] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.99ms | 14.02ms | 1.000 | [0.989, 1.012] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.93ms | 5.94ms | 1.058 | [0.885, 1.265] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.00ms | 14.02ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.08ms | 106.92ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.35ms | 44.35ms | 0.999 | [0.985, 1.013] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.02ms | 112.15ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.69ms | 100.76ms | 0.997 | [0.990, 1.004] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.16ms | 44.08ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.72ms | 106.63ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 156.73ms | 156.47ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.41ms | 3.41ms | 1.015 | [0.972, 1.061] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.32ms | 3.32ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.79ms | 4.77ms | 1.007 | [0.991, 1.023] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.76ms | 4.74ms | 1.002 | [0.997, 1.008] | ~ (noise) |

Sum of medians (call, common cases): A = 730.86ms, B = 730.46ms, saved = 401.21us (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.1%, 0.1%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 52s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
