A/B `e2e`: A=`WP-26-before` B=`WP-26-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 411.09ms | 397.34ms | 1.034 | [0.927, 1.154] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 328.98ms | 322.33ms | 1.016 | [0.909, 1.135] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.25ms | 31.11ms | 1.000 | [0.898, 1.113] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 245.99ms | 249.65ms | 0.982 | [0.944, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 243.34ms | 246.98ms | 0.981 | [0.943, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.14ms | 1.91ms | 1.072 | [0.842, 1.364] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 500.86ms | 499.92ms | 0.999 | [0.985, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 498.61ms | 497.40ms | 0.999 | [0.985, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.84ms | 1.87ms | 0.978 | [0.873, 1.095] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 924.84ms | 927.20ms | 0.995 | [0.982, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 922.37ms | 924.84ms | 0.995 | [0.984, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.20ms | 2.17ms | 1.055 | [0.396, 2.814] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 308.97ms | 304.67ms | 1.009 | [1.004, 1.015] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 297.67ms | 296.10ms | 1.004 | [0.996, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 6.02ms | 5.18ms | 1.224 | [0.912, 1.643] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 520.98ms | 520.91ms | 0.999 | [0.986, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 497.24ms | 495.44ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.38ms | 16.14ms | 0.951 | [0.772, 1.173] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 238.16ms | 235.79ms | 1.025 | [0.976, 1.077] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 206.24ms | 205.03ms | 1.041 | [0.977, 1.109] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.05ms | 19.54ms | 0.863 | [0.638, 1.166] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.404s | 2.404s | 0.999 | [0.991, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.401s | 2.401s | 0.999 | [0.991, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.89ms | 1.83ms | 1.034 | [0.931, 1.149] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.268s | 1.296s | 0.991 | [0.903, 1.088] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.171s | 1.216s | 0.988 | [0.899, 1.085] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 79.41ms | 74.51ms | 1.046 | [0.905, 1.209] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.25ms | 86.70ms | 0.981 | [0.935, 1.030] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.56ms | 83.61ms | 0.982 | [0.940, 1.026] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.04ms | 2.09ms | 0.927 | [0.737, 1.167] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.21ms | 321.86ms | 0.998 | [0.970, 1.027] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.32ms | 319.16ms | 1.002 | [0.981, 1.024] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.39ms | 2.32ms | 0.787 | [0.343, 1.808] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 697.98ms | 692.67ms | 1.011 | [1.004, 1.019] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 695.29ms | 690.05ms | 1.011 | [1.004, 1.018] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.99ms | 1.97ms | 1.012 | [0.930, 1.101] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.308s | 1.307s | 1.004 | [0.991, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.305s | 1.304s | 1.004 | [0.992, 1.016] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.36ms | 2.50ms | 0.936 | [0.520, 1.686] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.16ms | 93.45ms | 0.983 | [0.973, 0.993] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.62ms | 92.08ms | 0.982 | [0.970, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.02ms | 1.01ms | 1.066 | [0.684, 1.663] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.57ms | 202.34ms | 0.983 | [0.951, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 198.87ms | 199.26ms | 0.985 | [0.958, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.19ms | 1.22ms | 0.766 | [0.392, 1.498] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.134s | 1.134s | 0.997 | [0.991, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.126s | 1.127s | 0.997 | [0.991, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.02ms | 3.05ms | 1.074 | [0.772, 1.494] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.411s | 5.401s | 1.000 | [0.992, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.403s | 5.393s | 1.000 | [0.992, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.28ms | 3.18ms | 1.099 | [0.827, 1.459] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.221s | 1.222s | 0.999 | [0.992, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.148s | 1.147s | 1.000 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.92ms | 44.05ms | 0.980 | [0.840, 1.144] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.227s | 1.229s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.126s | 1.002 | [0.991, 1.014] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 62.08ms | 65.35ms | 0.963 | [0.852, 1.089] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.30ms | 248.51ms | 0.996 | [0.973, 1.021] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.33ms | 244.79ms | 0.996 | [0.972, 1.020] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.29ms | 2.26ms | 1.018 | [0.947, 1.094] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.444s | 1.451s | 0.999 | [0.986, 1.011] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.434s | 1.441s | 0.999 | [0.986, 1.012] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.30ms | 4.88ms | 0.964 | [0.831, 1.119] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.780s | 6.769s | 1.001 | [0.996, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.770s | 6.760s | 1.001 | [0.997, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.96ms | 4.00ms | 1.008 | [0.659, 1.541] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 175.41ms | 173.33ms | 0.998 | [0.952, 1.047] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 175.03ms | 172.92ms | 0.998 | [0.952, 1.047] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 204.70us | 214.62us | 0.978 | [0.890, 1.075] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 245.08ms | 241.76ms | 1.007 | [0.987, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.91ms | 241.61ms | 1.007 | [0.987, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 74.17us | 74.24us | 0.992 | [0.922, 1.068] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.448s | 1.442s | 1.008 | [0.995, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.448s | 1.442s | 1.007 | [0.997, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.04us | 103.99us | 1.009 | [0.805, 1.266] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.259s | 1.257s | 1.002 | [0.990, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.259s | 1.256s | 1.002 | [0.990, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.81us | 66.84us | 1.047 | [0.942, 1.163] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.989s | 1.991s | 0.996 | [0.991, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.989s | 1.991s | 0.996 | [0.991, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.78us | 34.71us | 1.708 | [0.477, 6.119] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.18ms | 182.42ms | 1.004 | [0.977, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 182.10ms | 182.34ms | 1.004 | [0.977, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.08us | 30.59us | 0.875 | [0.611, 1.252] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 335.58ms | 336.75ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.47ms | 336.63ms | 0.998 | [0.985, 1.012] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.78us | 36.95us | 0.958 | [0.744, 1.233] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.88ms | 52.08ms | 0.947 | [0.867, 1.034] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.81ms | 52.02ms | 0.947 | [0.867, 1.035] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.79us | 19.63us | 1.046 | [0.853, 1.284] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.02ms | 109.37ms | 0.999 | [0.962, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.96ms | 102.73ms | 1.001 | [0.960, 1.043] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.65ms | 1.64ms | 1.014 | [0.933, 1.102] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 441.58ms | 436.14ms | 1.011 | [0.986, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 438.55ms | 433.24ms | 1.010 | [0.983, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.32ms | 2.19ms | 1.240 | [0.774, 1.987] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.572s | 1.624s | 0.995 | [0.918, 1.078] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.569s | 1.621s | 0.995 | [0.918, 1.078] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.21ms | 2.20ms | 0.987 | [0.652, 1.494] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.547s | 2.507s | 1.018 | [0.998, 1.039] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.545s | 2.505s | 1.018 | [0.998, 1.039] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.76ms | 0.993 | [0.912, 1.083] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 733.26ms | 727.39ms | 1.009 | [0.957, 1.065] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 729.54ms | 723.61ms | 1.009 | [0.957, 1.065] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.01ms | 3.01ms | 1.005 | [0.994, 1.017] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 482.94ms | 497.39ms | 0.918 | [0.740, 1.138] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.40ms | 488.80ms | 0.916 | [0.735, 1.141] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.52ms | 7.60ms | 1.043 | [0.914, 1.190] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.31ms | 26.58ms | 0.999 | [0.970, 1.030] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.63ms | 23.86ms | 0.999 | [0.950, 1.051] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.44ms | 2.43ms | 1.003 | [0.990, 1.017] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.41ms | 3.33ms | 1.073 | [0.907, 1.269] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.11ms | 2.06ms | 0.996 | [0.884, 1.122] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 885.93us | 869.73us | 1.223 | [0.663, 2.258] | ~ (noise) |

Sum of medians (total, common cases): A = 38.595s, B = 38.602s, saved = -6454967.0ns (1.000x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 8.33e-14 -> B 6.40e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.9 MiB | 619.9 MiB | 1.000 | 0.1% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.6 MiB | 15.7 MiB | 1.005 | 4.0% / 3.7% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.4 MiB | 14.6 MiB | 1.014 | 2.2% / 3.1% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.9 MiB | 12.9 MiB | 1.000 | 4.5% / 3.9% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.7 MiB | 26.0 MiB | 1.010 | 1.6% / 3.1% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.5 MiB | 54.3 MiB | 1.015 | 6.6% / 7.7% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 57.7 MiB | 59.1 MiB | 1.024 | 10.3% / 5.3% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 14.6 MiB | 0.994 | 3.7% / 4.0% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1080.8 MiB | 1063.3 MiB | 0.984 | 6.1% / 1.2% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.0 MiB | 41.0 MiB | 1.027 | 7.5% / 7.6% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.6 MiB | 17.6 MiB | 1.001 | 4.7% / 3.7% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.8 MiB | 15.0 MiB | 1.015 | 2.9% / 5.2% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.007 | 2.4% / 1.7% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.7 MiB | 62.6 MiB | 0.999 | 1.8% / 0.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.8 MiB | 26.4 MiB | 1.024 | 4.8% / 1.0% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.3 MiB | 41.8 MiB | 0.989 | 5.8% / 6.4% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.5 MiB | 42.0 MiB | 0.988 | 2.5% / 2.3% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.5 MiB | 152.8 MiB | 1.008 | 3.8% / 2.8% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.4 MiB | 217.5 MiB | 1.001 | 0.3% / 0.8% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.5 MiB | 25.6 MiB | 1.004 | 4.2% / 2.6% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.5 MiB | 43.0 MiB | 1.013 | 2.8% / 5.9% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.0 MiB | 42.3 MiB | 1.008 | 4.4% / 3.6% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.7 MiB | 0.997 | 4.5% / 2.9% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.2 MiB | 1.009 | 1.7% / 1.8% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.1 MiB | 0.996 | 2.1% / 3.0% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 13.1 MiB | 1.016 | 3.6% / 2.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.3 MiB | 25.1 MiB | 1.033 | 4.4% / 3.5% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 12.0 MiB | 1.005 | 3.0% / 3.1% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.8 MiB | 12.0 MiB | 1.016 | 2.6% / 3.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.0 MiB | 12.0 MiB | 1.001 | 1.3% / 2.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.3 MiB | 19.6 MiB | 1.014 | 1.9% / 2.5% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.2 MiB | 1.005 | 2.6% / 0.9% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.9 MiB | 0.993 | 2.4% / 3.4% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.5 MiB | 1.025 | 4.5% / 3.7% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.3 MiB | 16.2 MiB | 0.994 | 1.9% / 6.0% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.1 MiB | 17.2 MiB | 1.002 | 2.4% / 2.9% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 12.9 MiB | 1.002 | 2.2% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.3 MiB | 0.985 | 4.4% / 5.0% | ~ |

Peak memory (common cases): geomean B/A = 1.006; largest B/A: `interp/sg/cc/b0/d10/L2/p8` 24.3 MiB -> 25.1 MiB (1.033x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.4%, 0.5%, 0.4%
Round durations (s): 163, 163, 163, 163, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 814s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
