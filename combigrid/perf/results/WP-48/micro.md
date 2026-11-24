A/B `micro`: A=`WP-48-before` B=`WP-48-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/compensated_combination/equi/b1/d15/L6 | call | 119.55us | 119.17us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/quad/compensated_combination/equi/b1/d20/L5 | call | 116.62us | 116.67us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/dsg/compensated_combination/equi/b1/d20/L6 | call | 450.82us | 451.42us | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/dsg/compensated_combination/equi/b1/d15/L7 | call | 334.82us | 334.83us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/dsg/operator/equi/b0/d3/L12 | call | 25.29ms | 25.30ms | 0.986 | [0.955, 1.019] | ~ (noise) |
| micro/dsg/operator/equi/b0/d5/L8 | call | 49.66ms | 49.65ms | 1.001 | [0.990, 1.012] | ~ (noise) |
| micro/dsg/operator/equi/b1/d15/L4 | call | 20.56ms | 20.50ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.50ms | 126.60ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.54ms | 61.63ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.31ms | 12.29ms | 1.002 | [0.998, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.88ms | 12.88ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 0.991 | [0.973, 1.010] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.42ms | 18.54ms | 0.994 | [0.988, 0.999] | SLOWER |
| micro/interp/sg/cc/b0/d1/L6 | call | 4.35us | 4.38us | 0.993 | [0.982, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L4 | call | 26.50us | 26.49us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L3 | call | 113.34us | 113.30us | 1.006 | [0.989, 1.023] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d5/L5/m4 | call | 245.36ms | 245.35ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d2/L12/m4 | call | 52.62ms | 52.61ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 605.79ms | 604.70ms | 1.002 | [0.998, 1.007] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.83ms | 47.86ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.78ms | 3.77ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.58us | 8.70us | 0.985 | [0.976, 0.995] | SLOWER |
| micro/interplin/sg/equi/b1/d1/L10 | call | 1.27us | 1.30us | 0.976 | [0.964, 0.988] | SLOWER |
| micro/interplin/sg/equi/b1/d2/L8 | call | 4.79us | 4.85us | 0.986 | [0.974, 0.999] | SLOWER |
| micro/interplin/sg/equi/b1/d3/L6 | call | 34.64us | 34.88us | 0.997 | [0.979, 1.015] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L10 | call | 90.49us | 91.21us | 0.991 | [0.986, 0.996] | SLOWER |
| micro/interplin/sg/equi/b1/d4/L6 | call | 248.59us | 249.55us | 0.996 | [0.992, 0.999] | SLOWER |
| micro/interplin/sg_vec/equi/b1/d8/L5/m4 | call | 193.19ms | 193.36ms | 0.999 | [0.993, 1.004] | ~ (noise) |
| micro/interplin/sg_vec/equi/b1/d2/L16/m4 | call | 25.10us | 25.16us | 1.006 | [0.979, 1.034] | ~ (noise) |
| micro/expensive/quad_vec/sg/cc/b1/d7/L4/m4/c1us | call | 60.99ms | 60.94ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/expensive/quad_vec/dsg/cc/b1/d7/L4/m4/c1us | call | 22.26ms | 22.27ms | 1.000 | [0.996, 1.003] | ~ (noise) |

Sum of medians (call, common cases): A = 1.562s, B = 1.561s, saved = 715.96us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.3%, 0.3%
Round durations (s): 23, 23, 23, 23, 23 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 113s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
