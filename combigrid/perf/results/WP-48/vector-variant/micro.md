A/B `micro`: A=`WP-48-after` B=`WP-48-vector`, 1 thread(s), 3 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/operator/equi/b0/d3/L12 | call | 25.26ms | 25.31ms | 0.996 | [0.977, 1.015] | ~ (noise) |
| micro/dsg/operator/equi/b0/d5/L8 | call | 49.66ms | 49.66ms | 1.004 | [0.982, 1.026] | ~ (noise) |
| micro/dsg/operator/equi/b1/d15/L4 | call | 20.43ms | 20.54ms | 0.992 | [0.970, 1.014] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.75ms | 126.56ms | 1.002 | [0.995, 1.010] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.67ms | 61.60ms | 1.001 | [0.994, 1.009] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.31ms | 12.31ms | 1.000 | [0.992, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.88ms | 12.87ms | 1.000 | [0.993, 1.008] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.005 | [0.997, 1.012] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.54ms | 18.49ms | 1.008 | [0.973, 1.043] | ~ (noise) |
| micro/interp/sg/cc/b0/d1/L6 | call | 4.36us | 4.39us | 0.996 | [0.982, 1.010] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L4 | call | 26.51us | 26.45us | 1.002 | [0.995, 1.010] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L3 | call | 113.19us | 113.11us | 1.001 | [0.994, 1.009] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d5/L5/m4 | call | 245.98ms | 245.20ms | 1.004 | [0.992, 1.016] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d2/L12/m4 | call | 52.56ms | 52.47ms | 1.002 | [0.994, 1.009] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 604.40ms | 604.78ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 47.90ms | 47.97ms | 0.996 | [0.966, 1.026] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.76ms | 3.76ms | 1.002 | [0.994, 1.009] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.70us | 8.64us | 1.009 | [0.993, 1.025] | ~ (noise) |
| micro/interplin/sg/equi/b1/d1/L10 | call | 1.29us | 1.27us | 1.015 | [0.971, 1.060] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L8 | call | 4.85us | 4.80us | 1.009 | [1.002, 1.017] | faster |
| micro/interplin/sg/equi/b1/d3/L6 | call | 34.91us | 34.71us | 1.005 | [0.997, 1.012] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L10 | call | 90.91us | 90.73us | 1.002 | [0.992, 1.011] | ~ (noise) |
| micro/interplin/sg/equi/b1/d4/L6 | call | 249.71us | 248.85us | 1.003 | [0.995, 1.010] | ~ (noise) |
| micro/interplin/sg_vec/equi/b1/d8/L5/m4 | call | 192.97ms | 193.30ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| micro/interplin/sg_vec/equi/b1/d2/L16/m4 | call | 25.17us | 25.24us | 0.997 | [0.990, 1.005] | ~ (noise) |

Sum of medians (call, common cases): A = 1.477s, B = 1.476s, saved = 251.78us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.3%
Round durations (s): 20, 20, 20 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 59s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
