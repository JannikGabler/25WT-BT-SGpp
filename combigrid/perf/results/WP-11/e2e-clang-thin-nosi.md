A/B `e2e`: A=`WP-11-gcc` B=`WP-11-clang-thin-nosi`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 438.44ms | 522.25ms | 0.847 | [0.759, 0.945] | SLOWER |
| quad/sg/equi-exp/b0/d2/L20 | operator | 360.65ms | 394.62ms | 0.923 | [0.826, 1.031] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.38ms | 26.85ms | 1.185 | [1.065, 1.320] | faster |
| quad/sg/equi-exp/b0/d5/L10 | total | 268.93ms | 260.19ms | 1.039 | [1.025, 1.053] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 266.39ms | 259.00ms | 1.032 | [1.019, 1.046] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.98ms | 579.68us | 3.797 | [2.691, 5.359] | faster |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.05ms | 537.45ms | 0.955 | [0.945, 0.965] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.51ms | 536.26ms | 0.953 | [0.943, 0.962] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.86ms | 518.92us | 3.017 | [2.187, 4.163] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 936.25ms | 922.00ms | 1.016 | [1.010, 1.022] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 934.14ms | 921.43ms | 1.014 | [1.008, 1.021] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.03ms | 256.67us | 8.274 | [6.948, 9.852] | faster |
| quad/sg/equi-exp/b1/d10/L7 | total | 311.84ms | 277.21ms | 1.121 | [1.093, 1.149] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 302.85ms | 269.84ms | 1.120 | [1.090, 1.150] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.67ms | 3.95ms | 1.368 | [1.131, 1.654] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 529.16ms | 541.86ms | 0.974 | [0.961, 0.986] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | operator | 503.24ms | 513.74ms | 0.978 | [0.965, 0.992] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | generation | 18.20ms | 16.46ms | 0.963 | [0.834, 1.113] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 234.57ms | 237.30ms | 0.990 | [0.968, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 207.49ms | 207.75ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 19.09ms | 17.23ms | 1.032 | [0.763, 1.394] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.420s | 2.643s | 0.916 | [0.909, 0.924] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.418s | 2.641s | 0.916 | [0.908, 0.923] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.95ms | 606.36us | 2.913 | [2.137, 3.972] | faster |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.317s | 1.344s | 0.979 | [0.884, 1.085] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.226s | 1.273s | 0.967 | [0.881, 1.061] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 80.23ms | 69.89ms | 1.143 | [0.926, 1.410] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 89.81ms | 93.09ms | 0.968 | [0.955, 0.982] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | operator | 86.94ms | 91.18ms | 0.955 | [0.941, 0.969] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.11ms | 1.78ms | 1.384 | [0.935, 2.047] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 342.22ms | 349.14ms | 0.977 | [0.969, 0.985] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | operator | 339.25ms | 347.46ms | 0.973 | [0.965, 0.980] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.49ms | 1.08ms | 2.432 | [2.028, 2.916] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 715.41ms | 743.23ms | 0.966 | [0.941, 0.991] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 712.79ms | 741.67ms | 0.964 | [0.940, 0.989] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.04ms | 980.57us | 1.857 | [0.936, 3.683] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.316s | 1.361s | 0.964 | [0.957, 0.972] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.313s | 1.361s | 0.962 | [0.955, 0.970] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.42ms | 282.86us | 8.412 | [6.331, 11.178] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.68ms | 102.62ms | 1.066 | [1.011, 1.125] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.33ms | 101.10ms | 1.068 | [1.011, 1.130] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.10ms | 1.21ms | 0.921 | [0.753, 1.127] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 222.77ms | 206.10ms | 1.077 | [1.060, 1.093] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.10ms | 204.07ms | 1.075 | [1.060, 1.090] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.16ms | 898.08us | 1.309 | [0.952, 1.801] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.229s | 1.241s | 0.994 | [0.987, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.221s | 1.232s | 0.994 | [0.988, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.67ms | 3.90ms | 0.961 | [0.817, 1.130] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.662s | 5.582s | 1.015 | [1.008, 1.022] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.651s | 5.572s | 1.015 | [1.008, 1.023] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.85ms | 4.20ms | 1.092 | [0.562, 2.121] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.967s | 4.094s | 0.968 | [0.963, 0.972] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.894s | 4.011s | 0.971 | [0.967, 0.975] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.11ms | 42.62ms | 0.941 | [0.756, 1.171] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.763s | 7.089s | 0.953 | [0.946, 0.960] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.657s | 6.969s | 0.953 | [0.946, 0.961] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.24ms | 58.62ms | 1.184 | [1.090, 1.285] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.74ms | 265.58ms | 0.937 | [0.739, 1.187] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.50ms | 262.23ms | 0.933 | [0.734, 1.185] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.77ms | 1.85ms | 1.409 | [1.225, 1.622] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.518s | 1.565s | 0.969 | [0.960, 0.977] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.508s | 1.555s | 0.969 | [0.960, 0.977] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.98ms | 5.06ms | 0.947 | [0.786, 1.142] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.964s | 7.093s | 0.982 | [0.976, 0.988] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.956s | 7.083s | 0.982 | [0.977, 0.987] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.86ms | 5.10ms | 0.953 | [0.623, 1.457] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 233.58ms | 185.66ms | 1.267 | [1.244, 1.291] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 233.19ms | 185.37ms | 1.267 | [1.244, 1.290] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 220.77us | 231.38us | 0.936 | [0.857, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 307.56ms | 284.82ms | 1.094 | [1.065, 1.123] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.41ms | 284.62ms | 1.094 | [1.065, 1.123] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 78.57us | 94.43us | 0.842 | [0.725, 0.979] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.743s | 1.582s | 1.102 | [1.096, 1.108] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.742s | 1.582s | 1.102 | [1.096, 1.108] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 126.41us | 127.81us | 0.928 | [0.699, 1.232] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.475s | 1.354s | 1.089 | [1.075, 1.104] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.475s | 1.354s | 1.089 | [1.075, 1.104] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 79.55us | 86.74us | 0.745 | [0.417, 1.330] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.246s | 2.010s | 1.113 | [1.104, 1.122] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.245s | 2.009s | 1.113 | [1.104, 1.122] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 38.76us | 46.23us | 1.087 | [0.539, 2.191] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 186.45ms | 204.30ms | 0.900 | [0.849, 0.955] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 186.37ms | 204.20ms | 0.900 | [0.849, 0.955] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.63us | 46.65us | 0.588 | [0.411, 0.842] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 373.68ms | 376.91ms | 0.978 | [0.925, 1.035] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 373.55ms | 376.75ms | 0.978 | [0.925, 1.035] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.67us | 62.23us | 0.613 | [0.476, 0.789] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.29ms | 58.00ms | 0.937 | [0.892, 0.984] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.22ms | 57.91ms | 0.937 | [0.892, 0.984] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | generation | 24.23us | 28.22us | 0.866 | [0.744, 1.008] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 113.58ms | 126.79ms | 0.896 | [0.863, 0.931] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 107.71ms | 119.49ms | 0.898 | [0.862, 0.936] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.72ms | 1.28ms | 1.502 | [0.990, 2.279] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 496.79ms | 458.04ms | 1.112 | [1.011, 1.222] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 491.98ms | 456.97ms | 1.107 | [1.006, 1.219] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.12ms | 276.36us | 8.755 | [6.003, 12.768] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.672s | 1.554s | 1.076 | [1.052, 1.101] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.669s | 1.553s | 1.074 | [1.051, 1.099] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.37ms | 281.04us | 9.468 | [6.257, 14.327] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.684s | 2.462s | 1.090 | [1.068, 1.113] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.681s | 2.460s | 1.090 | [1.068, 1.112] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 691.91us | 2.578 | [2.251, 2.953] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 791.73ms | 756.94ms | 1.045 | [0.991, 1.102] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 786.30ms | 751.44ms | 1.046 | [0.992, 1.104] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.63ms | 4.71ms | 0.919 | [0.726, 1.163] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.854s | 1.437s | 1.288 | [1.267, 1.309] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 482.30ms | 487.07ms | 0.974 | [0.942, 1.007] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.370s | 948.50ms | 1.452 | [1.433, 1.470] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.813s | 1.233s | 1.445 | [1.381, 1.512] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.68ms | 27.64ms | 0.865 | [0.822, 0.910] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.788s | 1.205s | 1.458 | [1.392, 1.527] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.894s | 1.181s | 1.606 | [1.595, 1.618] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 2.33ms | 0.884 | [0.816, 0.957] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.892s | 1.178s | 1.608 | [1.597, 1.619] | faster |

Sum of medians (total, common cases): A = 54.073s, B = 52.335s, saved = 1.738s (1.033x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.80e-09 -> B 1.96e-08 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.1%, 0.1%, 0.1%
Round durations (s): 221, 221, 221, 221, 220 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1104s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
