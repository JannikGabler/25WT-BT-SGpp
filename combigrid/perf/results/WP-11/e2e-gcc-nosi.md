A/B `e2e`: A=`WP-11-gcc` B=`WP-11-gcc-nosi`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 445.89ms | 418.52ms | 1.037 | [0.930, 1.158] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 365.06ms | 341.13ms | 1.064 | [0.952, 1.188] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.24ms | 30.40ms | 1.045 | [0.939, 1.164] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 269.40ms | 249.67ms | 1.078 | [1.066, 1.091] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 266.78ms | 247.27ms | 1.077 | [1.065, 1.090] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.03ms | 1.89ms | 1.164 | [0.883, 1.535] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 515.83ms | 504.79ms | 1.022 | [0.999, 1.045] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 513.19ms | 502.33ms | 1.022 | [0.999, 1.045] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.91ms | 1.83ms | 1.043 | [0.944, 1.153] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 938.72ms | 910.73ms | 1.031 | [1.025, 1.037] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 936.24ms | 907.38ms | 1.032 | [1.025, 1.039] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.23ms | 2.22ms | 0.879 | [0.579, 1.335] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 306.04ms | 303.32ms | 1.014 | [0.994, 1.034] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 297.39ms | 294.59ms | 1.015 | [0.995, 1.035] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.87ms | 5.30ms | 0.951 | [0.786, 1.150] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 530.49ms | 521.59ms | 1.016 | [1.003, 1.029] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 504.15ms | 497.36ms | 1.015 | [1.001, 1.030] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.49ms | 15.74ms | 1.063 | [0.949, 1.191] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.09ms | 236.19ms | 1.007 | [0.977, 1.037] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.05ms | 205.33ms | 1.017 | [1.001, 1.033] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.62ms | 21.04ms | 0.910 | [0.644, 1.288] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.427s | 2.416s | 1.005 | [0.996, 1.013] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.424s | 2.414s | 1.005 | [0.996, 1.013] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.94ms | 1.89ms | 1.013 | [0.912, 1.126] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.239s | 1.215s | 1.031 | [0.992, 1.071] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.160s | 1.125s | 1.044 | [1.002, 1.087] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 76.14ms | 79.71ms | 0.904 | [0.782, 1.044] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 89.99ms | 87.22ms | 1.039 | [1.024, 1.053] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 87.63ms | 84.38ms | 1.043 | [1.030, 1.056] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.08ms | 2.06ms | 1.023 | [0.951, 1.100] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 342.65ms | 327.92ms | 1.045 | [1.041, 1.049] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 339.55ms | 325.24ms | 1.044 | [1.040, 1.048] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.47ms | 2.12ms | 1.163 | [1.061, 1.276] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 714.60ms | 698.88ms | 1.019 | [1.009, 1.029] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 711.73ms | 696.35ms | 1.018 | [1.008, 1.029] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.11ms | 1.97ms | 1.135 | [0.925, 1.392] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.311s | 1.324s | 0.990 | [0.985, 0.996] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.308s | 1.321s | 0.990 | [0.985, 0.995] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.42ms | 2.41ms | 1.117 | [0.764, 1.633] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 107.58ms | 98.75ms | 1.081 | [1.063, 1.100] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 106.17ms | 97.22ms | 1.088 | [1.076, 1.101] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.09ms | 1.19ms | 0.815 | [0.423, 1.570] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.25ms | 210.45ms | 1.061 | [1.056, 1.066] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.62ms | 207.90ms | 1.061 | [1.057, 1.065] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.52ms | 1.43ms | 1.031 | [0.799, 1.331] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.236s | 1.189s | 1.041 | [1.032, 1.049] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.227s | 1.181s | 1.040 | [1.031, 1.049] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.63ms | 3.02ms | 1.214 | [1.131, 1.304] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.675s | 5.525s | 1.027 | [1.023, 1.031] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.667s | 5.516s | 1.027 | [1.023, 1.031] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.94ms | 4.24ms | 0.968 | [0.729, 1.286] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.968s | 3.896s | 1.008 | [0.983, 1.034] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.896s | 3.837s | 1.006 | [0.980, 1.033] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.68ms | 35.83ms | 1.169 | [1.002, 1.364] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.780s | 6.678s | 1.017 | [1.012, 1.022] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.675s | 6.587s | 1.014 | [1.009, 1.018] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.33ms | 51.04ms | 1.399 | [1.228, 1.593] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.67ms | 258.39ms | 1.044 | [1.035, 1.053] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.87ms | 254.63ms | 1.044 | [1.034, 1.053] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.61ms | 2.39ms | 1.080 | [0.965, 1.209] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.523s | 1.489s | 1.022 | [1.018, 1.026] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.513s | 1.479s | 1.022 | [1.018, 1.026] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.83ms | 4.40ms | 1.082 | [0.945, 1.240] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.992s | 6.964s | 1.004 | [0.998, 1.010] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.981s | 6.955s | 1.003 | [0.999, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.35ms | 3.51ms | 1.470 | [0.962, 2.247] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.08ms | 188.51ms | 1.225 | [1.190, 1.262] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 233.73ms | 188.16ms | 1.226 | [1.190, 1.262] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 217.28us | 206.10us | 1.041 | [0.954, 1.137] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 310.20ms | 265.74ms | 1.176 | [1.138, 1.215] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 310.03ms | 265.59ms | 1.176 | [1.138, 1.215] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 77.45us | 74.38us | 1.054 | [0.978, 1.134] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.749s | 1.622s | 1.079 | [1.069, 1.088] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.749s | 1.621s | 1.079 | [1.069, 1.088] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 126.76us | 102.46us | 1.205 | [0.956, 1.518] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.478s | 1.409s | 1.052 | [1.038, 1.065] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.478s | 1.407s | 1.052 | [1.038, 1.066] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 79.62us | 70.40us | 0.670 | [0.159, 2.815] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.261s | 2.161s | 1.041 | [1.035, 1.047] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.261s | 2.161s | 1.041 | [1.035, 1.047] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 40.37us | 35.83us | 1.599 | [0.554, 4.613] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.21ms | 184.12ms | 1.042 | [0.990, 1.097] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.12ms | 184.04ms | 1.042 | [0.990, 1.097] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.15us | 28.43us | 1.146 | [0.801, 1.640] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 371.43ms | 357.85ms | 1.036 | [1.017, 1.055] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 371.32ms | 357.73ms | 1.036 | [1.017, 1.055] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.67us | 35.55us | 1.242 | [0.965, 1.599] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.58ms | 51.72ms | 1.040 | [1.012, 1.067] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.51ms | 51.65ms | 1.040 | [1.012, 1.067] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 23.54us | 23.82us | 1.029 | [0.839, 1.262] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 116.08ms | 110.90ms | 1.042 | [0.998, 1.087] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 109.90ms | 104.74ms | 1.045 | [0.999, 1.094] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.74ms | 1.73ms | 0.996 | [0.916, 1.083] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 510.63ms | 481.67ms | 1.046 | [1.006, 1.088] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 507.64ms | 478.56ms | 1.048 | [1.009, 1.089] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.24ms | 2.34ms | 0.781 | [0.491, 1.243] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.708s | 1.727s | 0.972 | [0.887, 1.065] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.705s | 1.725s | 0.972 | [0.886, 1.065] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 3.66ms | 2.28ms | 1.242 | [0.601, 2.566] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.690s | 2.706s | 0.990 | [0.964, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.687s | 2.704s | 0.990 | [0.964, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.82ms | 1.85ms | 0.868 | [0.592, 1.273] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 797.55ms | 725.12ms | 1.096 | [1.039, 1.156] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 792.16ms | 719.59ms | 1.097 | [1.040, 1.157] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.69ms | 4.73ms | 0.998 | [0.980, 1.018] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.853s | 1.814s | 1.025 | [1.005, 1.046] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 479.79ms | 470.52ms | 1.025 | [0.983, 1.068] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.366s | 1.330s | 1.026 | [1.006, 1.045] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.830s | 1.705s | 1.072 | [1.060, 1.084] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.10ms | 24.34ms | 1.015 | [0.938, 1.098] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.806s | 1.681s | 1.073 | [1.060, 1.085] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.905s | 1.880s | 1.012 | [1.009, 1.016] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 2.07ms | 1.011 | [0.933, 1.095] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.903s | 1.877s | 1.012 | [1.009, 1.016] | faster |

Sum of medians (total, common cases): A = 54.195s, B = 52.913s, saved = 1.282s (1.024x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 1.60e-12 -> B 4.30e-12 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.3%, 0.3%
Round durations (s): 222, 222, 222, 223, 222 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1113s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
