A/B `micro`: A=`WP-08-after` B=`WP-08-export`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.30us | 36.37us | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 177.87us | 177.88us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 202.9ns | 203.1ns | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d5/L8 | call | 2.98us | 2.98us | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.43ms | 23.60ms | 0.998 | [0.980, 1.016] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 192.51us | 192.30us | 1.001 | [0.989, 1.013] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 36.08us | 36.05us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 210.2ns | 209.5ns | 1.003 | [0.998, 1.007] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.96us | 2.96us | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.85ms | 22.86ms | 0.999 | [0.993, 1.004] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m1 | call | 204.8ns | 205.0ns | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m1 | call | 236.2ns | 235.2ns | 1.007 | [0.996, 1.019] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m1 | call | 36.39us | 36.41us | 0.985 | [0.946, 1.026] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m1 | call | 37.28us | 37.33us | 0.997 | [0.992, 1.002] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m1 | call | 180.99us | 181.17us | 1.000 | [0.993, 1.007] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m1 | call | 185.67us | 185.80us | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m2 | call | 407.4ns | 408.5ns | 0.999 | [0.980, 1.018] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m2 | call | 364.8ns | 363.7ns | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m2 | call | 72.72us | 72.79us | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m2 | call | 72.45us | 72.45us | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m2 | call | 361.25us | 361.68us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m2 | call | 389.14us | 388.23us | 1.002 | [0.995, 1.009] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m4 | call | 835.7ns | 837.3ns | 0.997 | [0.990, 1.003] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m4 | call | 653.7ns | 653.6ns | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m4 | call | 145.58us | 145.38us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m4 | call | 144.45us | 144.74us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m4 | call | 723.58us | 724.32us | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m4 | call | 779.51us | 776.84us | 0.995 | [0.975, 1.016] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m8 | call | 1.68us | 1.69us | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m8 | call | 1.20us | 1.20us | 0.996 | [0.971, 1.021] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m8 | call | 293.89us | 292.51us | 1.004 | [0.996, 1.012] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m8 | call | 286.25us | 286.56us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m8 | call | 1.52ms | 1.53ms | 1.072 | [0.879, 1.308] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m8 | call | 1.56ms | 1.56ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m32 | call | 6.89us | 6.91us | 0.973 | [0.898, 1.055] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m32 | call | 4.51us | 4.50us | 1.002 | [0.995, 1.009] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m32 | call | 1.17ms | 1.17ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m32 | call | 1.14ms | 1.14ms | 0.991 | [0.962, 1.021] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m32 | call | 6.23ms | 6.24ms | 1.010 | [0.969, 1.054] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m32 | call | 5.96ms | 5.95ms | 1.000 | [0.997, 1.004] | ~ (noise) |

Sum of medians (call, common cases): A = 68.04ms, B = 68.22ms, saved = -182235.3ns (0.997x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.3%, 0.5%, 0.4%
Round durations (s): 18, 18, 18, 18, 18 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 89s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
