A/B `micro`: A=`WP-29c-after` B=`WP-29c-refactor`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 508.78us | 509.85us | 1.002 | [0.981, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.33ms | 47.36ms | 0.993 | [0.974, 1.012] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.39ms | 1.37ms | 1.001 | [0.962, 1.042] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 119.56us | 118.51us | 1.014 | [0.988, 1.041] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 829.4ns | 827.9ns | 1.010 | [0.997, 1.024] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.45us | 37.45us | 1.004 | [0.995, 1.013] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.8ns | 31.6ns | 1.011 | [1.003, 1.018] | faster |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.2ns | 197.0ns | 0.996 | [0.992, 1.000] | SLOWER |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.52us | 42.44us | 1.003 | [0.995, 1.012] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 245.33us | 243.95us | 1.035 | [0.961, 1.114] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 208.1ns | 208.0ns | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m1 | call | 1.42ms | 1.41ms | 1.009 | [1.002, 1.015] | faster |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m1 | call | 1.71ms | 1.69ms | 1.008 | [0.998, 1.017] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m1 | call | 1.12ms | 1.12ms | 1.001 | [0.996, 1.007] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m1 | call | 1.27ms | 1.27ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m1 | call | 508.97us | 507.79us | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m1 | call | 521.08us | 520.80us | 0.995 | [0.978, 1.012] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m1 | call | 48.94ms | 48.88ms | 1.004 | [0.992, 1.017] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m1 | call | 52.31ms | 52.13ms | 1.007 | [0.993, 1.021] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m1 | call | 210.4ns | 211.8ns | 0.994 | [0.989, 0.999] | SLOWER |
| micro/interplin/tg_vec/equi/b1/d2/L16/m1 | call | 235.1ns | 235.7ns | 1.004 | [0.979, 1.030] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m1 | call | 42.71us | 42.58us | 1.000 | [0.986, 1.014] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m1 | call | 43.88us | 44.05us | 1.000 | [0.988, 1.012] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m1 | call | 243.53us | 247.98us | 0.980 | [0.943, 1.018] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m1 | call | 259.35us | 258.24us | 1.007 | [0.994, 1.019] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m8 | call | 11.38ms | 11.32ms | 1.010 | [0.997, 1.024] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m8 | call | 8.35ms | 8.33ms | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m8 | call | 9.09ms | 9.08ms | 1.000 | [0.993, 1.006] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m8 | call | 7.65ms | 7.65ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m8 | call | 4.08ms | 4.08ms | 1.004 | [0.995, 1.013] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m8 | call | 2.17ms | 2.16ms | 1.003 | [0.997, 1.009] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m8 | call | 392.77ms | 400.39ms | 0.986 | [0.976, 0.997] | SLOWER |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m8 | call | 292.59ms | 292.65ms | 0.996 | [0.983, 1.009] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m8 | call | 1.75us | 1.75us | 0.992 | [0.981, 1.004] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m8 | call | 1.19us | 1.19us | 1.004 | [0.994, 1.013] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m8 | call | 343.14us | 343.26us | 1.000 | [0.988, 1.012] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m8 | call | 293.68us | 293.01us | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m8 | call | 2.07ms | 2.10ms | 0.983 | [0.958, 1.009] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m8 | call | 1.67ms | 1.67ms | 1.003 | [0.982, 1.024] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m32 | call | 46.28ms | 45.98ms | 1.005 | [0.993, 1.017] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m32 | call | 31.82ms | 31.80ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m32 | call | 36.69ms | 36.77ms | 0.992 | [0.968, 1.017] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m32 | call | 29.33ms | 29.36ms | 0.988 | [0.961, 1.016] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m32 | call | 16.34ms | 16.38ms | 0.998 | [0.991, 1.004] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m32 | call | 7.69ms | 7.69ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m32 | call | 1.622s | 1.647s | 0.987 | [0.983, 0.991] | SLOWER |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m32 | call | 1.059s | 1.059s | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m32 | call | 7.15us | 7.18us | 0.996 | [0.986, 1.007] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m32 | call | 4.52us | 4.52us | 1.000 | [0.972, 1.029] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m32 | call | 1.38ms | 1.38ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m32 | call | 1.15ms | 1.15ms | 1.000 | [0.992, 1.008] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m32 | call | 8.69ms | 8.67ms | 0.999 | [0.987, 1.012] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m32 | call | 6.11ms | 6.10ms | 0.998 | [0.991, 1.004] | ~ (noise) |

Sum of medians (call, common cases): A = 3.757s, B = 3.790s, saved = -32672901.1ns (0.991x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.3%, 0.3%
Round durations (s): 57, 57, 57, 57, 58 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 287s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
