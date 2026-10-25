A/B `e2e`: A=`WP-13-before` B=`WP-13-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 436.84ms | 407.54ms | 1.064 | [0.953, 1.187] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 360.16ms | 333.19ms | 1.075 | [0.962, 1.201] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.40ms | 32.53ms | 1.011 | [0.908, 1.126] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 269.34ms | 249.89ms | 1.078 | [1.065, 1.090] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 266.79ms | 247.52ms | 1.077 | [1.065, 1.089] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.11ms | 2.04ms | 1.133 | [0.761, 1.687] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.99ms | 500.36ms | 1.028 | [1.018, 1.039] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.39ms | 497.93ms | 1.027 | [1.017, 1.038] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.87ms | 1.79ms | 1.214 | [0.925, 1.595] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 937.93ms | 910.71ms | 1.028 | [1.022, 1.035] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 935.49ms | 908.50ms | 1.029 | [1.022, 1.035] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.16ms | 2.17ms | 0.873 | [0.556, 1.371] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 305.90ms | 301.21ms | 1.028 | [1.000, 1.056] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 297.11ms | 292.71ms | 1.028 | [1.001, 1.056] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.74ms | 5.40ms | 1.036 | [0.857, 1.253] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 528.51ms | 520.71ms | 1.016 | [1.003, 1.029] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 503.62ms | 496.49ms | 1.015 | [1.000, 1.029] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.56ms | 13.92ms | 1.084 | [0.796, 1.475] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.89ms | 231.13ms | 1.021 | [0.991, 1.053] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 207.18ms | 205.31ms | 1.011 | [0.997, 1.025] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 19.26ms | 18.29ms | 1.128 | [0.835, 1.524] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.420s | 2.416s | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.418s | 2.414s | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.97ms | 1.86ms | 1.035 | [0.932, 1.150] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.214s | 1.212s | 0.994 | [0.931, 1.061] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.132s | 1.140s | 0.994 | [0.939, 1.053] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 81.69ms | 82.03ms | 0.992 | [0.787, 1.251] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 90.36ms | 86.67ms | 1.037 | [1.018, 1.056] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 87.54ms | 83.96ms | 1.039 | [1.025, 1.054] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.08ms | 2.08ms | 0.915 | [0.764, 1.095] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 342.13ms | 328.59ms | 1.036 | [1.004, 1.069] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 339.08ms | 325.72ms | 1.036 | [1.004, 1.069] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.39ms | 2.31ms | 1.067 | [0.937, 1.215] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 713.21ms | 700.15ms | 1.021 | [1.016, 1.026] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 710.43ms | 697.64ms | 1.020 | [1.015, 1.026] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.04ms | 1.92ms | 1.046 | [0.970, 1.128] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.312s | 1.317s | 0.996 | [0.990, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.310s | 1.315s | 0.996 | [0.990, 1.001] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.35ms | 2.34ms | 1.054 | [0.617, 1.799] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.07ms | 97.68ms | 1.089 | [1.066, 1.112] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 104.69ms | 96.41ms | 1.090 | [1.068, 1.113] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.08ms | 1.05ms | 0.992 | [0.811, 1.213] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.38ms | 210.62ms | 1.060 | [1.051, 1.069] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.73ms | 208.05ms | 1.059 | [1.052, 1.065] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.47ms | 1.30ms | 1.248 | [0.791, 1.970] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.229s | 1.189s | 1.034 | [1.030, 1.039] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.221s | 1.181s | 1.034 | [1.030, 1.038] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.66ms | 3.06ms | 1.179 | [1.150, 1.209] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.660s | 5.526s | 1.024 | [1.021, 1.028] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.651s | 5.516s | 1.024 | [1.021, 1.028] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.28ms | 4.15ms | 1.060 | [0.799, 1.408] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.975s | 3.897s | 1.020 | [1.013, 1.027] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.895s | 3.838s | 1.018 | [1.012, 1.024] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.59ms | 35.67ms | 1.193 | [1.022, 1.392] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.766s | 6.678s | 1.008 | [0.995, 1.022] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.653s | 6.587s | 1.005 | [0.992, 1.018] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.41ms | 50.71ms | 1.367 | [1.209, 1.545] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.81ms | 258.11ms | 1.048 | [1.037, 1.060] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 266.02ms | 254.53ms | 1.048 | [1.037, 1.060] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.54ms | 2.45ms | 1.025 | [0.954, 1.102] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.518s | 1.490s | 1.020 | [1.016, 1.024] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.508s | 1.480s | 1.020 | [1.016, 1.024] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.57ms | 5.03ms | 1.003 | [0.762, 1.321] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.978s | 6.955s | 1.004 | [0.998, 1.010] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.968s | 6.945s | 1.004 | [0.999, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.22ms | 3.28ms | 1.202 | [0.786, 1.837] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.83ms | 188.16ms | 1.237 | [1.180, 1.297] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 234.44ms | 187.72ms | 1.237 | [1.180, 1.297] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 219.30us | 205.61us | 1.039 | [0.952, 1.135] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 307.43ms | 267.88ms | 1.152 | [1.128, 1.175] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.28ms | 267.71ms | 1.152 | [1.129, 1.176] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 80.04us | 73.12us | 1.065 | [0.989, 1.146] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.749s | 1.623s | 1.074 | [1.069, 1.080] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.749s | 1.623s | 1.074 | [1.069, 1.080] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 126.69us | 104.06us | 1.271 | [1.013, 1.594] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.478s | 1.405s | 1.052 | [1.039, 1.066] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.478s | 1.405s | 1.052 | [1.039, 1.066] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 78.64us | 67.33us | 1.187 | [1.074, 1.312] | faster |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.251s | 2.173s | 1.037 | [1.032, 1.043] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.251s | 2.173s | 1.037 | [1.032, 1.043] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 40.02us | 36.67us | 1.085 | [1.026, 1.148] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.81ms | 184.04ms | 1.012 | [0.992, 1.034] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.25ms | 183.95ms | 1.010 | [0.988, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.43us | 28.36us | 2.684 | [0.264, 27.327] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 371.62ms | 353.36ms | 1.063 | [1.032, 1.094] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 371.50ms | 353.24ms | 1.063 | [1.032, 1.094] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.67us | 35.55us | 1.249 | [0.970, 1.607] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.35ms | 52.08ms | 0.973 | [0.841, 1.126] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.28ms | 52.00ms | 0.973 | [0.840, 1.127] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 23.89us | 24.38us | 0.895 | [0.634, 1.264] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 115.42ms | 110.37ms | 1.062 | [1.017, 1.108] | faster |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 108.98ms | 104.19ms | 1.065 | [1.018, 1.115] | faster |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.75ms | 1.79ms | 0.991 | [0.912, 1.078] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 492.17ms | 479.21ms | 1.037 | [1.004, 1.071] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 489.34ms | 476.71ms | 1.036 | [1.006, 1.068] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.17ms | 2.15ms | 1.160 | [0.695, 1.939] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.685s | 1.679s | 1.002 | [0.976, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.682s | 1.675s | 1.002 | [0.976, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.36ms | 2.36ms | 0.870 | [0.473, 1.602] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.694s | 2.681s | 1.001 | [0.976, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.691s | 2.678s | 1.001 | [0.976, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.83ms | 1.77ms | 1.089 | [0.979, 1.212] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 802.87ms | 719.39ms | 1.120 | [1.062, 1.181] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 797.55ms | 713.95ms | 1.121 | [1.063, 1.183] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.65ms | 4.64ms | 1.000 | [0.972, 1.027] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.855s | 1.812s | 1.007 | [0.946, 1.072] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 484.29ms | 485.64ms | 0.978 | [0.916, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.368s | 1.327s | 1.018 | [0.951, 1.089] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.807s | 1.704s | 1.057 | [1.039, 1.075] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 25.03ms | 24.78ms | 1.010 | [0.947, 1.076] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.784s | 1.679s | 1.057 | [1.039, 1.076] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.898s | 1.876s | 1.014 | [1.005, 1.023] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.06ms | 2.07ms | 0.999 | [0.923, 1.082] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.896s | 1.874s | 1.014 | [1.005, 1.023] | faster |

Sum of medians (total, common cases): A = 54.032s, B = 52.792s, saved = 1.240s (1.023x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 1.45e-13 -> B 8.25e-13 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 222, 222, 222, 222, 222 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1110s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
