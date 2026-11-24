A/B `micro`: A=`WP-61-rework-A` B=`WP-61-rework-B`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.92ms | 5.91ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.96ms | 2.98ms | 0.984 | [0.960, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.00ms | 13.99ms | 0.999 | [0.988, 1.011] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.91ms | 5.92ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.02ms | 14.02ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.12ms | 106.75ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.37ms | 44.36ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.21ms | 112.02ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.71ms | 100.61ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.15ms | 44.05ms | 1.008 | [0.992, 1.024] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.73ms | 106.83ms | 1.000 | [0.994, 1.005] | ~ (noise) |
| micro/quad/compensated_combination/equi/b1/d15/L6 | call | 119.27us | 119.37us | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/quad/compensated_combination/equi/b1/d20/L5 | call | 116.55us | 116.59us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 156.58ms | 156.67ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.41ms | 3.41ms | 1.000 | [0.995, 1.004] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.32ms | 3.31ms | 0.998 | [0.990, 1.007] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.79ms | 4.79ms | 1.001 | [0.991, 1.010] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.74ms | 4.71ms | 0.999 | [0.987, 1.012] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m1 | call | 6.29ms | 6.30ms | 1.002 | [0.987, 1.017] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m1 | call | 9.53ms | 9.49ms | 1.007 | [0.994, 1.019] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m1 | call | 33.77ms | 33.69ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m1 | call | 37.72ms | 37.77ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m1 | call | 250.67ms | 250.50ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m1 | call | 271.10ms | 270.73ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m2 | call | 12.59ms | 12.58ms | 1.005 | [0.992, 1.019] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m2 | call | 15.47ms | 15.47ms | 0.998 | [0.991, 1.006] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m2 | call | 67.54ms | 67.53ms | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m2 | call | 48.94ms | 49.08ms | 0.996 | [0.990, 1.001] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m2 | call | 501.13ms | 501.64ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m2 | call | 348.07ms | 347.50ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m4 | call | 25.18ms | 25.12ms | 1.022 | [0.969, 1.077] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m4 | call | 24.62ms | 24.63ms | 1.003 | [0.993, 1.014] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m4 | call | 134.94ms | 134.86ms | 0.998 | [0.991, 1.006] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m4 | call | 77.61ms | 77.58ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m4 | call | 1.003s | 1.003s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m4 | call | 590.88ms | 590.73ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m8 | call | 50.21ms | 50.42ms | 0.981 | [0.940, 1.023] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m8 | call | 42.64ms | 42.65ms | 0.990 | [0.965, 1.016] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m8 | call | 269.89ms | 270.28ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m8 | call | 120.57ms | 120.59ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m8 | call | 2.007s | 2.006s | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m8 | call | 938.63ms | 937.41ms | 1.003 | [0.997, 1.009] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m32 | call | 201.44ms | 200.89ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m32 | call | 159.51ms | 159.61ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m32 | call | 1.080s | 1.081s | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m32 | call | 403.04ms | 403.04ms | 1.004 | [0.989, 1.018] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m32 | call | 8.028s | 8.027s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m32 | call | 3.305s | 3.306s | 0.999 | [0.985, 1.014] | ~ (noise) |
| micro/expensive/quad_scalar_x_m/sg/cc/b1/d7/L4/m4/c1us | call | 212.90ms | 212.88ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/expensive/quad_vec/sg/cc/b1/d7/L4/m4/c1us | call | 61.04ms | 61.05ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/expensive/quad_scalar_x_m/sg/cc/b1/d5/L3/m4/c100us | call | 1.022s | 1.021s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/expensive/quad_vec/sg/cc/b1/d5/L3/m4/c100us | call | 255.69ms | 255.89ms | 0.999 | [0.996, 1.003] | ~ (noise) |

Sum of medians (call, common cases): A = 22.347s, B = 22.344s, saved = 2.78ms (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 228, 228, 228, 228, 229 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1142s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
