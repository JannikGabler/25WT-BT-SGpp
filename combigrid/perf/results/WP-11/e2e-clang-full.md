A/B `e2e`: A=`WP-11-gcc` B=`WP-11-clang-full`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 447.77ms | 514.42ms | 0.864 | [0.774, 0.964] | SLOWER |
| quad/sg/equi-exp/b0/d2/L20 | operator | 365.12ms | 393.17ms | 0.931 | [0.834, 1.040] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.16ms | 27.33ms | 1.152 | [1.035, 1.283] | faster |
| quad/sg/equi-exp/b0/d5/L10 | total | 269.33ms | 267.48ms | 1.006 | [0.995, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 266.72ms | 266.22ms | 1.001 | [0.989, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.12ms | 707.28us | 3.242 | [2.333, 4.504] | faster |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.03ms | 543.11ms | 0.946 | [0.936, 0.956] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.46ms | 541.89ms | 0.944 | [0.934, 0.953] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.89ms | 524.58us | 3.542 | [2.967, 4.228] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 938.63ms | 943.24ms | 0.998 | [0.987, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 936.19ms | 942.76ms | 0.996 | [0.984, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.23ms | 254.99us | 9.439 | [6.187, 14.398] | faster |
| quad/sg/equi-exp/b1/d10/L7 | total | 305.07ms | 285.58ms | 1.076 | [1.060, 1.092] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 296.91ms | 276.32ms | 1.078 | [1.064, 1.093] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.02ms | 5.77ms | 1.034 | [0.766, 1.395] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 532.68ms | 556.42ms | 0.959 | [0.945, 0.973] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | operator | 506.68ms | 524.63ms | 0.963 | [0.949, 0.977] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.01ms | 21.04ms | 0.902 | [0.681, 1.195] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.11ms | 243.00ms | 0.973 | [0.944, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 207.39ms | 212.10ms | 0.978 | [0.972, 0.983] | SLOWER |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.06ms | 18.53ms | 1.034 | [0.765, 1.397] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.421s | 2.650s | 0.914 | [0.906, 0.921] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.419s | 2.648s | 0.913 | [0.906, 0.921] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.91ms | 739.75us | 2.700 | [1.855, 3.930] | faster |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.233s | 1.204s | 1.051 | [0.995, 1.110] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.159s | 1.134s | 1.040 | [0.989, 1.095] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 86.05ms | 72.58ms | 1.190 | [0.983, 1.441] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 89.94ms | 93.69ms | 0.959 | [0.945, 0.972] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | operator | 86.95ms | 91.77ms | 0.949 | [0.938, 0.961] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.07ms | 1.78ms | 1.210 | [1.118, 1.309] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 342.13ms | 350.22ms | 0.975 | [0.969, 0.981] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | operator | 339.22ms | 348.54ms | 0.972 | [0.965, 0.978] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.38ms | 1.07ms | 2.151 | [1.655, 2.796] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 713.45ms | 742.61ms | 0.966 | [0.950, 0.983] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 710.85ms | 740.82ms | 0.965 | [0.948, 0.982] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.00ms | 1.04ms | 1.991 | [1.689, 2.346] | faster |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.316s | 1.588s | 0.828 | [0.816, 0.840] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.313s | 1.587s | 0.826 | [0.815, 0.838] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.42ms | 338.94us | 9.162 | [5.409, 15.519] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.72ms | 100.72ms | 1.056 | [0.993, 1.124] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.14ms | 99.38ms | 1.053 | [0.995, 1.115] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.15ms | 1.09ms | 1.277 | [0.733, 2.224] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.57ms | 215.64ms | 1.038 | [1.031, 1.046] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.78ms | 213.32ms | 1.036 | [1.028, 1.044] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.47ms | 1.03ms | 1.520 | [1.048, 2.206] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.232s | 1.254s | 0.986 | [0.977, 0.995] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.224s | 1.245s | 0.987 | [0.977, 0.996] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.65ms | 3.88ms | 0.925 | [0.846, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.663s | 5.616s | 1.007 | [1.002, 1.011] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.653s | 5.605s | 1.007 | [1.002, 1.011] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.85ms | 4.49ms | 0.959 | [0.702, 1.312] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.968s | 4.118s | 0.963 | [0.960, 0.967] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.898s | 4.032s | 0.967 | [0.963, 0.970] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.77ms | 42.06ms | 0.992 | [0.850, 1.158] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.765s | 7.112s | 0.951 | [0.947, 0.954] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.665s | 6.990s | 0.952 | [0.949, 0.956] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.10ms | 61.05ms | 1.045 | [0.939, 1.164] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.35ms | 263.85ms | 1.026 | [1.010, 1.042] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.14ms | 260.80ms | 1.023 | [1.009, 1.036] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.46ms | 1.90ms | 1.409 | [0.919, 2.161] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.520s | 1.526s | 0.994 | [0.988, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.510s | 1.515s | 0.994 | [0.987, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.53ms | 4.87ms | 1.049 | [0.916, 1.202] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.975s | 7.991s | 0.874 | [0.864, 0.885] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.966s | 7.981s | 0.874 | [0.864, 0.885] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.31ms | 5.07ms | 0.874 | [0.572, 1.336] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 233.76ms | 188.35ms | 1.240 | [1.217, 1.263] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 233.36ms | 188.07ms | 1.240 | [1.218, 1.263] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 212.46us | 232.08us | 0.897 | [0.821, 0.979] | SLOWER |
| interp/sg/cc/b0/d3/L9/p64 | total | 308.05ms | 284.00ms | 1.073 | [1.039, 1.108] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.89ms | 283.82ms | 1.073 | [1.039, 1.108] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 77.38us | 94.91us | 0.819 | [0.761, 0.882] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.755s | 1.576s | 1.113 | [1.101, 1.125] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.754s | 1.576s | 1.113 | [1.101, 1.125] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 133.60us | 123.34us | 1.049 | [0.837, 1.316] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.478s | 1.351s | 1.094 | [1.079, 1.108] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.478s | 1.350s | 1.094 | [1.080, 1.108] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 77.59us | 87.93us | 0.460 | [0.065, 3.256] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.255s | 2.006s | 1.125 | [1.117, 1.133] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.255s | 2.006s | 1.125 | [1.117, 1.133] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 41.41us | 47.28us | 1.317 | [0.500, 3.468] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.86ms | 206.97ms | 0.927 | [0.877, 0.981] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.76ms | 206.84ms | 0.927 | [0.877, 0.981] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.01us | 48.26us | 0.594 | [0.415, 0.849] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 371.69ms | 372.04ms | 1.007 | [0.989, 1.026] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 371.58ms | 371.89ms | 1.007 | [0.989, 1.026] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.53us | 61.04us | 0.680 | [0.528, 0.875] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.31ms | 58.19ms | 0.917 | [0.907, 0.928] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.23ms | 58.10ms | 0.918 | [0.908, 0.928] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | generation | 23.68us | 30.31us | 0.810 | [0.696, 0.943] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 112.77ms | 127.13ms | 0.856 | [0.780, 0.940] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 106.68ms | 119.69ms | 0.857 | [0.776, 0.947] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.68ms | 1.16ms | 1.674 | [1.193, 2.350] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 489.88ms | 470.81ms | 1.034 | [1.001, 1.068] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 486.81ms | 469.70ms | 1.031 | [0.998, 1.064] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.19ms | 274.75us | 7.794 | [6.855, 8.860] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.683s | 1.586s | 1.084 | [1.026, 1.144] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.679s | 1.585s | 1.082 | [1.025, 1.143] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.22ms | 295.99us | 8.161 | [5.393, 12.348] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.697s | 2.488s | 1.083 | [1.061, 1.106] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.694s | 2.487s | 1.083 | [1.061, 1.105] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.79ms | 735.91us | 2.486 | [2.203, 2.805] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 796.64ms | 785.55ms | 1.022 | [0.968, 1.078] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 791.29ms | 780.33ms | 1.022 | [0.968, 1.078] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.64ms | 4.45ms | 1.001 | [0.871, 1.150] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.857s | 1.359s | 1.358 | [1.336, 1.380] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 481.70ms | 496.51ms | 0.976 | [0.953, 0.999] | SLOWER |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.365s | 861.90ms | 1.577 | [1.550, 1.604] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.808s | 1.063s | 1.703 | [1.692, 1.714] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.10ms | 28.15ms | 0.860 | [0.818, 0.905] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.782s | 1.035s | 1.726 | [1.716, 1.737] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.901s | 1.007s | 1.889 | [1.872, 1.907] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.14ms | 2.39ms | 0.890 | [0.822, 0.964] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.899s | 1.004s | 1.892 | [1.875, 1.909] | faster |

Sum of medians (total, common cases): A = 54.072s, B = 53.108s, saved = 964.70ms (1.018x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.84e-09 -> B 1.87e-08 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 222, 222, 223, 222, 223 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1113s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
