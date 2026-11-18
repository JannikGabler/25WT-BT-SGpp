A/B `micro`: A=`WP-73-before` B=`WP-73-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/operator/cc/b0/d2/L16 | call | 112.17ms | 112.12ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.53ms | 100.71ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.27ms | 44.15ms | 1.004 | [0.997, 1.011] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.88ms | 106.92ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.76ms | 126.75ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.62ms | 61.66ms | 1.000 | [0.990, 1.010] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.31ms | 12.34ms | 0.997 | [0.993, 1.002] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 12.88ms | 12.89ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.003 | [0.985, 1.022] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.36ms | 18.50ms | 0.993 | [0.986, 1.000] | SLOWER |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.13ms | 35.24ms | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.89ms | 10.88ms | 1.000 | [0.988, 1.012] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.15ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.29ms | 5.28ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 417.68us | 418.57us | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 604.22ms | 606.09ms | 1.002 | [0.990, 1.013] | ~ (noise) |
| micro/interplin/sg/equi/b1/d8/L5 | call | 48.00ms | 47.88ms | 0.999 | [0.988, 1.009] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 3.78ms | 3.78ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 8.55us | 8.54us | 0.991 | [0.965, 1.018] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d10/L5 | call | 88.03ms | 88.43ms | 1.000 | [0.985, 1.015] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d8/L5 | call | 9.01ms | 9.02ms | 0.997 | [0.987, 1.006] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d5/L8 | call | 700.48us | 705.64us | 0.992 | [0.971, 1.013] | ~ (noise) |
| micro/interplin/dsg/equi/b1/d2/L16 | call | 7.72us | 7.52us | 1.051 | [0.979, 1.129] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d2/L20 | call | 257.21ms | 255.45ms | 1.006 | [1.002, 1.010] | faster |
| micro/crosscall/quad_sg/call/cc/b0/d2/L20 | call | 2.209s | 2.207s | 1.007 | [0.989, 1.025] | ~ (noise) |
| micro/crosscall/quad_sg/build/cc/b0/d5/L10 | call | 119.27us | 119.29us | 0.977 | [0.912, 1.048] | ~ (noise) |
| micro/crosscall/quad_sg/call/cc/b0/d5/L10 | call | 2.248s | 2.249s | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/crosscall/quad_sg/build/equi/b1/d20/L4 | call | 4.93us | 4.96us | 0.996 | [0.974, 1.017] | ~ (noise) |
| micro/crosscall/quad_sg/call/equi/b1/d20/L4 | call | 107.00ms | 106.76ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| micro/crosscall/quad_dsg/build/cc/b0/d5/L10 | call | 1.20ms | 1.21ms | 0.991 | [0.983, 0.998] | SLOWER |
| micro/crosscall/quad_dsg/call/cc/b0/d5/L10 | call | 303.08ms | 307.44ms | 0.990 | [0.975, 1.005] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b1/d15/L5 | call | 4.04ms | 4.04ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/crosscall/quad_dsg/call/equi/b1/d15/L5 | call | 190.56ms | 191.29ms | 0.994 | [0.984, 1.005] | ~ (noise) |
| micro/crosscall/quad_dsg/build/equi/b0/d3/L14 | call | 3.99ms | 4.07ms | 0.980 | [0.975, 0.984] | SLOWER |
| micro/crosscall/quad_dsg/call/equi/b0/d3/L14 | call | 124.08ms | 124.26ms | 1.003 | [0.994, 1.012] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d7/L4 | call | 58.80us | 59.52us | 0.986 | [0.974, 0.998] | SLOWER |
| micro/crosscall/interp_dsg/call/cc/b0/d7/L4 | call | 104.90ms | 104.66ms | 1.005 | [0.998, 1.011] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d2/L12 | call | 1.52ms | 1.54ms | 0.984 | [0.979, 0.989] | SLOWER |
| micro/crosscall/interp_dsg/call/cc/b0/d2/L12 | call | 5.29ms | 5.30ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/crosscall/interp_dsg/build/equi/b0/d3/L5 | call | 10.80us | 10.96us | 0.982 | [0.956, 1.009] | ~ (noise) |
| micro/crosscall/interp_dsg/call/equi/b0/d3/L5 | call | 417.73us | 418.06us | 0.999 | [0.994, 1.005] | ~ (noise) |
| micro/crosscall/interp_dsg/build/cc/b0/d10/L2 | call | 22.81us | 22.68us | 1.008 | [0.984, 1.032] | ~ (noise) |
| micro/crosscall/interp_dsg/call/cc/b0/d10/L2 | call | 545.01ms | 544.84ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d2/L16 | call | 7.36ms | 7.34ms | 1.003 | [0.998, 1.007] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d10/L5 | call | 2.36ms | 2.36ms | 1.004 | [0.996, 1.012] | ~ (noise) |
| micro/crosscall/interplin_dsg/build/equi/b1/d5/L8 | call | 423.72us | 422.27us | 1.002 | [0.990, 1.015] | ~ (noise) |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d2/L20 | call | (not in A) | 1.946s | - | - | new case |
| micro/crosscall/quad_sg/call_flipped/cc/b0/d5/L10 | call | (not in A) | 2.248s | - | - | new case |
| micro/crosscall/quad_sg/call_flipped/equi/b1/d20/L4 | call | (not in A) | 106.83ms | - | - | new case |
| micro/crosscall/quad_dsg/call_flipped/cc/b0/d5/L10 | call | (not in A) | 301.96ms | - | - | new case |
| micro/crosscall/quad_dsg/call_flipped/equi/b1/d15/L5 | call | (not in A) | 185.22ms | - | - | new case |
| micro/crosscall/quad_dsg/call_flipped/equi/b0/d3/L14 | call | (not in A) | 120.99ms | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d7/L4 | call | (not in A) | 105.02ms | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d2/L12 | call | (not in A) | 6.35ms | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/equi/b0/d3/L5 | call | (not in A) | 430.58us | - | - | new case |
| micro/crosscall/interp_dsg/call_flipped/cc/b0/d10/L2 | call | (not in A) | 544.84ms | - | - | new case |
| micro/crosscall/interplin_dsg/call/equi/b1/d2/L16 | call | (not in A) | 7.21us | - | - | new case |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d2/L16 | call | (not in A) | 7.40ms | - | - | new case |
| micro/crosscall/interplin_dsg/call/equi/b1/d10/L5 | call | (not in A) | 84.37ms | - | - | new case |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d10/L5 | call | (not in A) | 86.38ms | - | - | new case |
| micro/crosscall/interplin_dsg/call/equi/b1/d5/L8 | call | (not in A) | 683.90us | - | - | new case |
| micro/crosscall/interplin_dsg/call_flipped/equi/b1/d5/L8 | call | (not in A) | 1.24ms | - | - | new case |

Sum of medians (call, common cases): A = 7.521s, B = 7.525s, saved = -4462275.4ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 99, 99, 99, 99, 99 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 493s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
