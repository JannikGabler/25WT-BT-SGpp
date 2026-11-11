A/B `micro`: A=`WP-08-before` B=`WP-08-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.04us | 36.32us | 1.158 | [1.151, 1.166] | faster |
| micro/interplin/tg/equi/b1/d10/L5 | call | 229.07us | 178.09us | 1.273 | [1.228, 1.319] | faster |
| micro/interplin/tg/equi/b1/d2/L16 | call | 205.2ns | 202.8ns | 1.011 | [1.001, 1.022] | faster |
| micro/interplin/tg/equi/b1/d5/L8 | call | 3.42us | 2.98us | 1.146 | [1.141, 1.150] | faster |
| micro/interplin/tg/equi/b1/d16/L2 | call | 30.70ms | 23.53ms | 1.303 | [1.279, 1.328] | faster |
| micro/sol/interplin_tg/equi/b1/d10/L5 | call | 192.56us | 191.94us | 1.004 | [0.985, 1.022] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d8/L5 | call | 36.05us | 36.08us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d2/L16 | call | 208.1ns | 209.9ns | 0.989 | [0.978, 0.999] | SLOWER |
| micro/sol/interplin_tg/equi/b1/d5/L8 | call | 2.96us | 2.96us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/sol/interplin_tg/equi/b1/d16/L2 | call | 22.80ms | 22.81ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d10/L5 | call | 191.89us | 191.56us | 0.998 | [0.979, 1.016] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d8/L5 | call | 35.92us | 35.98us | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d2/L16 | call | 197.1ns | 196.4ns | 1.007 | [0.998, 1.016] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d5/L8 | call | 2.92us | 2.92us | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/sol/interplin_tg_integrand/equi/b1/d16/L2 | call | 22.79ms | 22.83ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m1 | call | 211.0ns | 205.1ns | 1.030 | [1.024, 1.035] | faster |
| micro/interplin/tg_vec/equi/b1/d2/L16/m1 | call | 236.7ns | 236.7ns | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m1 | call | 42.55us | 36.39us | 1.169 | [1.165, 1.173] | faster |
| micro/interplin/tg_vec/equi/b1/d8/L5/m1 | call | 43.26us | 37.28us | 1.159 | [1.151, 1.167] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m1 | call | 244.99us | 180.72us | 1.349 | [1.322, 1.377] | faster |
| micro/interplin/tg_vec/equi/b1/d10/L5/m1 | call | 241.26us | 185.75us | 1.299 | [1.272, 1.327] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m2 | call | 419.7ns | 408.0ns | 1.030 | [1.022, 1.039] | faster |
| micro/interplin/tg_vec/equi/b1/d2/L16/m2 | call | 372.0ns | 364.4ns | 1.020 | [1.016, 1.024] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m2 | call | 85.03us | 72.74us | 1.175 | [1.166, 1.184] | faster |
| micro/interplin/tg_vec/equi/b1/d8/L5/m2 | call | 78.67us | 72.49us | 1.084 | [1.078, 1.089] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m2 | call | 481.97us | 362.69us | 1.331 | [1.315, 1.348] | faster |
| micro/interplin/tg_vec/equi/b1/d10/L5/m2 | call | 463.53us | 389.21us | 1.191 | [1.181, 1.201] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m4 | call | 862.6ns | 835.5ns | 1.033 | [1.027, 1.038] | faster |
| micro/interplin/tg_vec/equi/b1/d2/L16/m4 | call | 651.6ns | 654.6ns | 0.994 | [0.988, 0.999] | SLOWER |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m4 | call | 170.04us | 145.56us | 1.169 | [1.163, 1.175] | faster |
| micro/interplin/tg_vec/equi/b1/d8/L5/m4 | call | 150.60us | 144.78us | 1.041 | [1.037, 1.045] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m4 | call | 966.94us | 723.96us | 1.338 | [1.312, 1.365] | faster |
| micro/interplin/tg_vec/equi/b1/d10/L5/m4 | call | 859.34us | 775.21us | 1.109 | [1.104, 1.113] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m8 | call | 1.74us | 1.69us | 1.023 | [1.004, 1.042] | faster |
| micro/interplin/tg_vec/equi/b1/d2/L16/m8 | call | 1.19us | 1.20us | 0.987 | [0.983, 0.990] | SLOWER |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m8 | call | 343.12us | 294.16us | 1.169 | [1.160, 1.178] | faster |
| micro/interplin/tg_vec/equi/b1/d8/L5/m8 | call | 292.55us | 286.24us | 1.022 | [1.018, 1.026] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m8 | call | 2.06ms | 1.52ms | 1.354 | [1.340, 1.369] | faster |
| micro/interplin/tg_vec/equi/b1/d10/L5/m8 | call | 1.65ms | 1.56ms | 1.065 | [1.048, 1.083] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m32 | call | 7.13us | 6.90us | 1.034 | [1.030, 1.037] | faster |
| micro/interplin/tg_vec/equi/b1/d2/L16/m32 | call | 4.51us | 4.51us | 1.007 | [0.990, 1.024] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m32 | call | 1.38ms | 1.18ms | 1.108 | [0.941, 1.304] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m32 | call | 1.15ms | 1.14ms | 1.007 | [1.003, 1.010] | faster |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m32 | call | 8.71ms | 6.25ms | 1.393 | [1.370, 1.416] | faster |
| micro/interplin/tg_vec/equi/b1/d10/L5/m32 | call | 6.08ms | 5.96ms | 1.020 | [1.007, 1.033] | faster |

Sum of medians (call, common cases): A = 102.54ms, B = 91.19ms, saved = 11.34ms (1.124x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 20, 20, 20, 20, 20 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 102s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
