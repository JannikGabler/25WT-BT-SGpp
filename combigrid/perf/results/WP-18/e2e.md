A/B `e2e`: A=`WP-18-before` B=`WP-18-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 444.65ms | 342.55ms | 1.306 | [1.171, 1.458] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 365.61ms | 323.95ms | 1.137 | [1.017, 1.270] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.41ms | 18.71ms | 1.663 | [1.494, 1.851] | faster |
| quad/sg/equi-exp/b0/d5/L10 | total | 265.08ms | 262.86ms | 1.009 | [0.997, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 262.36ms | 261.21ms | 1.005 | [0.994, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.82ms | 1.51ms | 1.347 | [0.922, 1.969] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 513.87ms | 511.97ms | 1.008 | [0.998, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.48ms | 509.99ms | 1.007 | [0.997, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.81ms | 1.51ms | 1.080 | [0.794, 1.470] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 910.84ms | 909.92ms | 1.002 | [0.996, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 908.63ms | 907.69ms | 1.002 | [0.996, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.19ms | 2.19ms | 0.889 | [0.525, 1.507] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 299.64ms | 292.58ms | 1.030 | [1.004, 1.057] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 291.28ms | 290.30ms | 1.009 | [0.983, 1.035] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.38ms | 1.53ms | 3.414 | [2.823, 4.129] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 522.76ms | 506.12ms | 1.023 | [0.989, 1.058] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.86ms | 496.89ms | 0.993 | [0.964, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.13ms | 6.54ms | 2.329 | [1.779, 3.049] | faster |
| quad/sg/equi-exp/b1/d20/L5 | total | 232.77ms | 214.07ms | 1.077 | [1.050, 1.105] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 202.36ms | 202.42ms | 0.996 | [0.982, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.06ms | 8.44ms | 2.288 | [1.694, 3.092] | faster |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.405s | 2.403s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.403s | 2.401s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.89ms | 1.47ms | 1.131 | [0.742, 1.724] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.230s | 1.150s | 1.103 | [1.021, 1.192] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.136s | 1.089s | 1.074 | [0.988, 1.167] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 86.98ms | 50.16ms | 1.720 | [1.362, 2.170] | faster |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.04ms | 83.99ms | 1.033 | [1.019, 1.048] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.15ms | 83.02ms | 1.013 | [1.001, 1.026] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.04ms | 892.57us | 2.236 | [1.585, 3.153] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 322.99ms | 321.62ms | 1.009 | [0.990, 1.027] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 320.15ms | 319.80ms | 1.007 | [0.991, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.26ms | 1.63ms | 1.120 | [0.690, 1.819] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 693.88ms | 698.12ms | 0.990 | [0.975, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 691.31ms | 695.29ms | 0.990 | [0.974, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.93ms | 1.54ms | 1.067 | [0.688, 1.656] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.317s | 1.320s | 0.996 | [0.991, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.314s | 1.317s | 0.996 | [0.991, 1.001] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.29ms | 2.22ms | 1.077 | [0.811, 1.431] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.86ms | 94.78ms | 1.018 | [1.004, 1.032] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.48ms | 94.61ms | 1.007 | [0.994, 1.019] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.03ms | 283.27us | 4.254 | [2.118, 8.542] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.70ms | 211.67ms | 0.995 | [0.990, 1.000] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.24ms | 211.11ms | 0.986 | [0.982, 0.990] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.06ms | 471.15us | 2.286 | [1.772, 2.950] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.173s | 1.190s | 0.986 | [0.982, 0.989] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.165s | 1.189s | 0.980 | [0.976, 0.983] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.13ms | 175.51us | 17.476 | [16.210, 18.840] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.364s | 5.434s | 0.991 | [0.979, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.355s | 5.433s | 0.990 | [0.978, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.39ms | 473.59us | 8.179 | [5.805, 11.522] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.225s | 1.169s | 1.050 | [1.046, 1.055] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.154s | 1.160s | 0.994 | [0.991, 0.998] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 44.98ms | 4.09ms | 10.992 | [9.417, 12.829] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.213s | 1.138s | 1.065 | [1.061, 1.069] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.112s | 1.119s | 0.993 | [0.990, 0.997] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.78ms | 11.21ms | 5.202 | [4.605, 5.877] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 250.25ms | 249.08ms | 1.002 | [0.993, 1.012] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.50ms | 248.01ms | 0.992 | [0.981, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.33ms | 652.45us | 2.957 | [1.601, 5.462] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.444s | 1.449s | 0.995 | [0.990, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.434s | 1.449s | 0.989 | [0.983, 0.995] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.43ms | 222.86us | 19.990 | [16.853, 23.710] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.863s | 6.883s | 0.996 | [0.990, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.853s | 6.882s | 0.995 | [0.991, 1.000] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.59ms | 276.43us | 11.814 | [6.238, 22.374] | faster |
| interp/sg/cc/b0/d2/L12/p64 | total | 129.52ms | 128.13ms | 1.012 | [0.993, 1.031] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 129.25ms | 127.76ms | 1.012 | [0.993, 1.031] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 219.23us | 213.71us | 1.018 | [0.933, 1.112] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 186.48ms | 186.49ms | 1.023 | [0.961, 1.089] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 186.34ms | 186.27ms | 1.023 | [0.961, 1.090] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 75.71us | 171.81us | 0.442 | [0.411, 0.476] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.124s | 1.131s | 0.995 | [0.988, 1.003] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.124s | 1.130s | 0.995 | [0.988, 1.003] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 174.60us | 90.65us | 1.875 | [1.495, 2.352] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.009s | 1.000s | 1.007 | [0.995, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.009s | 1.000s | 1.007 | [0.994, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 110.28us | 68.03us | 2.877 | [0.541, 15.308] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.629s | 1.604s | 1.013 | [1.003, 1.023] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.629s | 1.604s | 1.013 | [1.003, 1.023] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 43.51us | 39.04us | 1.229 | [0.955, 1.582] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.38ms | 34.15ms | 1.011 | [0.993, 1.030] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.30ms | 34.06ms | 1.012 | [0.994, 1.030] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.19us | 47.28us | 0.612 | [0.427, 0.875] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.59ms | 116.13ms | 1.018 | [0.970, 1.068] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.45ms | 116.05ms | 1.017 | [0.969, 1.068] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 57.83us | 35.76us | 1.448 | [1.039, 2.017] | faster |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.91ms | 12.01ms | 1.000 | [0.969, 1.031] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.83ms | 11.95ms | 0.999 | [0.968, 1.031] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 19.14us | 22.35us | 1.019 | [0.718, 1.446] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.29ms | 51.93ms | 1.011 | [1.001, 1.022] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 52.00ms | 51.67ms | 1.011 | [1.000, 1.022] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 135.49us | 117.82us | 1.109 | [1.028, 1.195] | faster |
| interp/dsg/cc/b0/d3/L9/p64 | total | 42.93ms | 43.10ms | 1.010 | [0.960, 1.063] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.70ms | 42.92ms | 1.010 | [0.960, 1.063] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 67.05us | 98.20us | 0.688 | [0.612, 0.773] | SLOWER |
| interp/dsg/cc/b0/d5/L6/p64 | total | 171.80ms | 174.99ms | 0.975 | [0.956, 0.995] | SLOWER |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 171.56ms | 174.87ms | 0.974 | [0.955, 0.994] | SLOWER |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 76.48us | 61.53us | 1.186 | [1.009, 1.394] | faster |
| interp/dsg/cc/b0/d7/L4/p16 | total | 149.53ms | 151.19ms | 0.986 | [0.980, 0.991] | SLOWER |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 149.36ms | 151.10ms | 0.985 | [0.980, 0.991] | SLOWER |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 57.76us | 46.30us | 1.290 | [1.120, 1.487] | faster |
| interp/dsg/cc/b0/d10/L2/p8 | total | 392.59ms | 399.06ms | 0.981 | [0.972, 0.991] | SLOWER |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 392.51ms | 399.00ms | 0.981 | [0.972, 0.991] | SLOWER |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 35.27us | 34.29us | 0.977 | [0.880, 1.085] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.50ms | 6.55ms | 0.979 | [0.931, 1.030] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.43ms | 6.49ms | 0.977 | [0.929, 1.028] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 20.74us | 21.86us | 0.969 | [0.880, 1.067] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.89ms | 112.86ms | 1.004 | [0.918, 1.098] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.65ms | 107.24ms | 1.000 | [0.908, 1.101] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.68ms | 1.34ms | 1.287 | [1.184, 1.399] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 358.15ms | 357.62ms | 0.995 | [0.970, 1.021] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 355.38ms | 357.07ms | 0.989 | [0.963, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.15ms | 90.37us | 23.902 | [21.025, 27.172] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.206s | 1.194s | 1.006 | [0.983, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.203s | 1.191s | 1.005 | [0.983, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.26ms | 2.14ms | 1.041 | [0.688, 1.575] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.843s | 1.848s | 0.989 | [0.964, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.840s | 1.847s | 0.989 | [0.964, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.79ms | 1.52ms | 1.221 | [1.050, 1.420] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 186.36ms | 179.32ms | 1.041 | [1.026, 1.057] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 181.63ms | 174.45ms | 1.043 | [1.027, 1.059] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.14ms | 914.50us | 1.222 | [1.042, 1.435] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 186.56ms | 181.62ms | 1.030 | [1.022, 1.037] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 183.67ms | 181.07ms | 1.015 | [1.011, 1.019] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 2.33ms | 78.15us | 31.068 | [22.752, 42.423] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 374.23ms | 380.16ms | 0.986 | [0.951, 1.023] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 371.77ms | 377.91ms | 0.988 | [0.955, 1.023] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.19ms | 3.55ms | 0.713 | [0.504, 1.008] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 429.77ms | 431.04ms | 0.999 | [0.982, 1.016] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 427.72ms | 429.61ms | 0.998 | [0.981, 1.015] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.56ms | 1.52ms | 0.971 | [0.713, 1.324] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 785.97ms | 793.42ms | 0.993 | [0.942, 1.048] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 782.32ms | 790.24ms | 0.992 | [0.941, 1.047] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.00ms | 2.66ms | 1.125 | [1.104, 1.147] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 482.57ms | 484.79ms | 1.003 | [0.915, 1.100] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.06ms | 477.04ms | 1.002 | [0.912, 1.100] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.52ms | 7.19ms | 1.047 | [1.035, 1.060] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.18ms | 27.21ms | 0.845 | [0.565, 1.264] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.47ms | 24.69ms | 0.829 | [0.541, 1.271] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.44ms | 2.38ms | 1.000 | [0.918, 1.090] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.16ms | 3.05ms | 0.975 | [0.857, 1.108] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.04ms | 2.08ms | 0.959 | [0.857, 1.074] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 901.09us | 774.95us | 0.963 | [0.638, 1.454] | ~ (noise) |

Sum of medians (total, common cases): A = 38.133s, B = 37.866s, saved = 267.29ms (1.007x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 9.18e-13 -> B 1.57e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.2 MiB | 329.3 MiB | 0.531 | 0.1% / 15.7% | MEM- |
| quad/sg/equi-exp/b0/d5/L10 | 16.0 MiB | 12.6 MiB | 0.790 | 5.0% / 2.5% | MEM- |
| quad/sg/equi-exp/b0/d7/L7 | 14.9 MiB | 12.8 MiB | 0.858 | 3.8% / 1.9% | MEM- |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 12.7 MiB | 0.965 | 7.2% / 2.5% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.0 MiB | 13.3 MiB | 0.511 | 6.5% / 1.8% | MEM- |
| quad/sg/equi-exp/b1/d15/L6 | 53.4 MiB | 18.4 MiB | 0.344 | 10.4% / 1.3% | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 59.8 MiB | 20.3 MiB | 0.340 | 6.1% / 1.3% | MEM- |
| quad/sg/equi-sin/b0/d7/L7 | 15.1 MiB | 12.7 MiB | 0.841 | 4.1% / 2.6% | MEM- |
| quad/sg/cc-uq/b0/d2/L20 | 1081.4 MiB | 830.3 MiB | 0.768 | 4.5% / 15.7% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 40.7 MiB | 34.2 MiB | 0.842 | 6.5% / 7.7% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 17.9 MiB | 14.3 MiB | 0.800 | 2.6% / 3.2% | MEM- |
| quad/sg/cc-uq/b0/d7/L7 | 15.3 MiB | 13.0 MiB | 0.847 | 3.5% / 1.4% | MEM- |
| quad/sg/cc-uq/b0/d10/L4 | 13.2 MiB | 12.8 MiB | 0.969 | 3.3% / 2.5% | MEM- |
| quad/dsg/equi-exp/b0/d3/L16 | 63.0 MiB | 43.3 MiB | 0.688 | 2.0% / 3.2% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 26.2 MiB | 18.2 MiB | 0.694 | 5.2% / 2.2% | MEM- |
| quad/dsg/equi-exp/b0/d7/L10 | 42.9 MiB | 29.8 MiB | 0.695 | 2.9% / 6.9% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 42.8 MiB | 30.1 MiB | 0.703 | 3.2% / 3.2% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 152.6 MiB | 51.6 MiB | 0.338 | 2.3% / 0.4% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 218.0 MiB | 82.8 MiB | 0.380 | 1.4% / 0.3% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 26.0 MiB | 18.2 MiB | 0.700 | 3.8% / 1.3% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 42.9 MiB | 28.8 MiB | 0.673 | 4.8% / 6.7% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 42.7 MiB | 29.9 MiB | 0.701 | 4.2% / 7.0% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 15.9 MiB | 14.9 MiB | 0.940 | 4.1% / 2.9% | MEM- |
| interp/sg/cc/b0/d3/L9/p64 | 12.5 MiB | 12.5 MiB | 1.003 | 1.2% / 1.9% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.5 MiB | 12.3 MiB | 0.985 | 1.7% / 2.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.3 MiB | 0.987 | 3.0% / 1.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 1.002 | 1.3% / 1.9% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.5 MiB | 1.020 | 2.0% / 1.7% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.4 MiB | 12.5 MiB | 1.010 | 1.5% / 1.4% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.4 MiB | 0.999 | 1.6% / 1.5% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.4 MiB | 15.5 MiB | 0.943 | 3.8% / 2.5% | MEM- |
| interp/dsg/cc/b0/d3/L9/p64 | 12.7 MiB | 12.6 MiB | 0.988 | 1.8% / 1.8% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.8 MiB | 12.6 MiB | 0.986 | 1.0% / 2.2% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.6 MiB | 12.6 MiB | 0.998 | 2.0% / 1.6% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.5 MiB | 12.4 MiB | 0.999 | 1.2% / 2.4% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.4 MiB | 1.003 | 2.1% / 1.1% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.8 MiB | 22.6 MiB | 1.146 | 4.2% / 5.7% | MEM+ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 12.9 MiB | 0.978 | 4.8% / 2.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.2 MiB | 12.8 MiB | 0.972 | 1.8% / 2.1% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.5 MiB | 12.8 MiB | 0.948 | 2.7% / 1.7% | MEM- |
| interplin/dsg/equi/b1/d2/L16/p16384 | 20.8 MiB | 20.6 MiB | 0.991 | 2.2% / 1.4% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.1 MiB | 0.986 | 3.7% / 0.5% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 13.3 MiB | 12.7 MiB | 0.956 | 4.6% / 5.1% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 17.0 MiB | 15.3 MiB | 0.896 | 2.6% / 2.7% | MEM- |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.5 MiB | 14.7 MiB | 0.890 | 2.5% / 2.8% | MEM- |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.6 MiB | 14.9 MiB | 0.847 | 5.1% / 2.1% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.3 MiB | 1.003 | 2.2% / 1.2% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.7 MiB | 1.013 | 3.9% / 2.2% | ~ |

Peak memory (common cases): geomean B/A = 0.811; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 19.8 MiB -> 22.6 MiB (1.146x); 1 MEM+, 27 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.6 MiB, B 10.6 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 163, 163, 163, 163, 163 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 815s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
