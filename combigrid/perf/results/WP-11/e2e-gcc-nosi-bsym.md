A/B `e2e`: A=`WP-11-gcc` B=`WP-11-gcc-nosi-bsym`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 425.72ms | 397.81ms | 1.073 | [0.961, 1.197] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 359.14ms | 323.65ms | 1.095 | [0.980, 1.223] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.31ms | 30.85ms | 1.019 | [0.916, 1.135] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 269.01ms | 243.43ms | 1.103 | [1.090, 1.116] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 266.62ms | 241.05ms | 1.104 | [1.091, 1.116] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.89ms | 1.91ms | 0.985 | [0.863, 1.124] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.37ms | 498.56ms | 1.028 | [1.004, 1.052] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.92ms | 496.41ms | 1.028 | [1.005, 1.051] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.84ms | 1.79ms | 1.055 | [0.961, 1.157] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 937.86ms | 922.06ms | 1.017 | [1.011, 1.023] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 935.47ms | 919.52ms | 1.017 | [1.011, 1.024] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.16ms | 2.30ms | 0.786 | [0.525, 1.177] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 306.22ms | 300.43ms | 1.017 | [0.975, 1.061] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 297.40ms | 292.05ms | 1.014 | [0.972, 1.059] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.67ms | 5.33ms | 1.158 | [0.957, 1.400] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 528.43ms | 527.39ms | 1.002 | [0.990, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 502.80ms | 502.26ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.00ms | 16.18ms | 1.029 | [0.889, 1.192] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 240.96ms | 236.27ms | 1.013 | [0.984, 1.043] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 207.33ms | 206.00ms | 1.005 | [0.989, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 21.14ms | 19.95ms | 1.120 | [0.829, 1.513] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.424s | 2.461s | 0.984 | [0.976, 0.992] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.422s | 2.458s | 0.984 | [0.976, 0.992] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.86ms | 1.93ms | 0.974 | [0.877, 1.082] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.213s | 1.227s | 1.025 | [0.939, 1.118] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.129s | 1.141s | 1.036 | [0.945, 1.134] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 76.57ms | 89.53ms | 0.920 | [0.796, 1.063] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 90.31ms | 86.68ms | 1.036 | [0.998, 1.075] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 87.26ms | 83.97ms | 1.032 | [0.997, 1.069] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.30ms | 2.09ms | 1.068 | [0.951, 1.199] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 342.38ms | 326.64ms | 1.043 | [1.013, 1.075] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 339.47ms | 323.64ms | 1.044 | [1.013, 1.075] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.30ms | 2.32ms | 1.005 | [0.908, 1.112] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 714.72ms | 700.32ms | 1.023 | [1.013, 1.033] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 712.02ms | 697.72ms | 1.023 | [1.013, 1.033] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.01ms | 1.98ms | 1.033 | [0.945, 1.129] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.311s | 1.321s | 1.003 | [0.984, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.309s | 1.319s | 1.004 | [0.984, 1.024] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.22ms | 2.38ms | 0.841 | [0.625, 1.131] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 107.18ms | 98.30ms | 1.093 | [1.074, 1.111] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.83ms | 97.01ms | 1.091 | [1.072, 1.111] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.24ms | 1.01ms | 1.219 | [0.996, 1.491] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.66ms | 204.94ms | 1.088 | [1.081, 1.095] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.83ms | 202.29ms | 1.088 | [1.081, 1.096] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.49ms | 1.31ms | 1.136 | [0.880, 1.465] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.232s | 1.188s | 1.036 | [1.032, 1.040] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.224s | 1.181s | 1.036 | [1.032, 1.040] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.61ms | 3.02ms | 1.192 | [1.173, 1.212] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.667s | 5.535s | 1.023 | [1.020, 1.027] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.658s | 5.527s | 1.023 | [1.020, 1.027] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.06ms | 4.20ms | 1.032 | [0.777, 1.370] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.978s | 4.006s | 0.993 | [0.986, 0.999] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.901s | 3.945s | 0.990 | [0.986, 0.993] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 44.32ms | 35.97ms | 1.281 | [0.961, 1.707] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.782s | 6.878s | 0.986 | [0.983, 0.990] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.666s | 6.788s | 0.983 | [0.979, 0.987] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 73.80ms | 51.35ms | 1.337 | [1.184, 1.509] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.10ms | 253.79ms | 1.058 | [1.045, 1.071] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.19ms | 250.05ms | 1.058 | [1.046, 1.071] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.38ms | 2.47ms | 0.993 | [0.924, 1.067] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.522s | 1.480s | 1.027 | [1.023, 1.031] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.512s | 1.470s | 1.027 | [1.023, 1.031] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.03ms | 4.93ms | 1.080 | [0.912, 1.278] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.981s | 6.933s | 1.008 | [1.002, 1.013] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.970s | 6.924s | 1.007 | [1.003, 1.012] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.31ms | 4.61ms | 1.185 | [0.775, 1.811] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.57ms | 190.61ms | 1.226 | [1.201, 1.252] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 234.17ms | 190.19ms | 1.226 | [1.202, 1.252] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 210.64us | 218.53us | 0.969 | [0.887, 1.058] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 307.94ms | 271.97ms | 1.126 | [1.096, 1.157] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.78ms | 271.81ms | 1.126 | [1.096, 1.157] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 77.59us | 73.82us | 1.079 | [0.951, 1.225] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.751s | 1.659s | 1.056 | [1.048, 1.063] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.751s | 1.659s | 1.056 | [1.048, 1.063] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 114.33us | 108.74us | 1.132 | [0.903, 1.420] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.481s | 1.429s | 1.036 | [1.023, 1.050] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.481s | 1.429s | 1.036 | [1.023, 1.050] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 79.13us | 68.79us | 1.115 | [1.005, 1.237] | faster |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.255s | 2.195s | 1.027 | [1.018, 1.036] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.255s | 2.194s | 1.027 | [1.018, 1.036] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 39.04us | 35.27us | 1.123 | [1.033, 1.220] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 188.19ms | 186.24ms | 1.017 | [1.002, 1.032] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 188.10ms | 186.15ms | 1.017 | [1.002, 1.032] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.01us | 27.10us | 1.176 | [0.822, 1.682] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 371.29ms | 355.78ms | 1.045 | [1.018, 1.074] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 371.17ms | 355.65ms | 1.045 | [1.018, 1.074] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 51.82us | 35.55us | 1.343 | [0.965, 1.869] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.80ms | 52.59ms | 1.165 | [0.909, 1.493] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.69ms | 52.51ms | 1.165 | [0.908, 1.494] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 26.12us | 24.23us | 1.170 | [0.722, 1.895] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 115.22ms | 110.90ms | 1.041 | [1.002, 1.081] | faster |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 109.16ms | 104.65ms | 1.045 | [1.003, 1.089] | faster |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.73ms | 1.79ms | 0.971 | [0.894, 1.056] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 500.37ms | 510.78ms | 0.981 | [0.928, 1.036] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 497.36ms | 506.52ms | 0.981 | [0.930, 1.036] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.20ms | 2.32ms | 0.873 | [0.606, 1.259] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.705s | 1.716s | 0.987 | [0.964, 1.009] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.702s | 1.711s | 0.987 | [0.965, 1.009] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.28ms | 2.20ms | 0.904 | [0.598, 1.369] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.699s | 2.720s | 0.990 | [0.970, 1.010] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.697s | 2.718s | 0.990 | [0.970, 1.010] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.76ms | 1.76ms | 1.001 | [0.927, 1.082] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 806.59ms | 730.77ms | 1.096 | [1.039, 1.156] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 801.12ms | 725.52ms | 1.096 | [1.039, 1.157] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.76ms | 4.58ms | 1.022 | [1.003, 1.043] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.858s | 1.797s | 1.022 | [0.999, 1.046] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 479.65ms | 481.87ms | 0.968 | [0.903, 1.036] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.378s | 1.317s | 1.043 | [1.027, 1.059] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.809s | 1.706s | 1.062 | [1.055, 1.068] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.59ms | 24.97ms | 0.987 | [0.937, 1.038] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.784s | 1.681s | 1.063 | [1.056, 1.069] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.897s | 1.881s | 1.013 | [1.001, 1.025] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.18ms | 2.07ms | 1.070 | [0.917, 1.249] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.894s | 1.879s | 1.013 | [1.001, 1.025] | faster |

Sum of medians (total, common cases): A = 54.113s, B = 53.339s, saved = 774.09ms (1.015x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 4.52e-16 -> B 1.24e-15 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 223, 224, 223, 223, 223 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1116s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
