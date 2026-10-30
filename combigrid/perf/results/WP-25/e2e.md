A/B `e2e`: A=`WP-25-before` B=`WP-25-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 405.44ms | 413.75ms | 0.990 | [0.887, 1.104] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 328.75ms | 341.77ms | 0.976 | [0.874, 1.091] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.18ms | 31.07ms | 0.999 | [0.897, 1.112] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 254.63ms | 256.06ms | 1.040 | [0.956, 1.131] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 251.19ms | 252.78ms | 1.040 | [0.955, 1.133] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.92ms | 2.06ms | 1.003 | [0.543, 1.853] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 501.40ms | 511.18ms | 0.983 | [0.973, 0.993] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | operator | 498.31ms | 508.66ms | 0.982 | [0.972, 0.992] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.84ms | 1.84ms | 1.174 | [0.715, 1.928] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 935.17ms | 944.67ms | 0.994 | [0.980, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 932.97ms | 942.22ms | 0.994 | [0.980, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.11ms | 2.21ms | 0.971 | [0.576, 1.636] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 303.78ms | 315.87ms | 0.974 | [0.952, 0.997] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | operator | 295.93ms | 306.72ms | 0.975 | [0.952, 0.998] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.12ms | 5.30ms | 0.899 | [0.744, 1.088] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 521.45ms | 535.19ms | 0.977 | [0.965, 0.989] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.94ms | 510.70ms | 0.975 | [0.962, 0.989] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.90ms | 15.91ms | 1.027 | [0.916, 1.150] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 238.88ms | 243.75ms | 0.975 | [0.962, 0.987] | SLOWER |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.06ms | 212.32ms | 0.979 | [0.974, 0.984] | SLOWER |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.37ms | 20.47ms | 0.928 | [0.686, 1.253] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.413s | 2.421s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.411s | 2.418s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.88ms | 1.87ms | 0.994 | [0.895, 1.105] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.218s | 1.219s | 1.021 | [0.977, 1.068] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.132s | 1.114s | 1.034 | [1.005, 1.064] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 75.86ms | 77.61ms | 0.943 | [0.771, 1.154] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.41ms | 88.49ms | 0.985 | [0.971, 0.999] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.71ms | 85.89ms | 0.983 | [0.971, 0.995] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 2.05ms | 1.005 | [0.987, 1.024] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 326.53ms | 329.98ms | 0.990 | [0.986, 0.995] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | operator | 323.71ms | 326.99ms | 0.990 | [0.986, 0.993] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.32ms | 2.47ms | 1.059 | [0.817, 1.373] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 701.28ms | 711.58ms | 0.987 | [0.982, 0.991] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 698.81ms | 708.98ms | 0.987 | [0.982, 0.992] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.86ms | 1.98ms | 0.957 | [0.887, 1.032] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.311s | 1.327s | 0.984 | [0.967, 1.001] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.307s | 1.324s | 0.983 | [0.966, 1.001] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 3.61ms | 2.33ms | 1.243 | [0.785, 1.968] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.19ms | 94.68ms | 0.981 | [0.975, 0.987] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.86ms | 93.46ms | 0.980 | [0.974, 0.986] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.02ms | 979.03us | 1.063 | [0.869, 1.301] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.58ms | 204.85ms | 0.984 | [0.979, 0.989] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 198.96ms | 202.50ms | 0.984 | [0.980, 0.988] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.14ms | 1.12ms | 0.977 | [0.757, 1.261] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.136s | 1.165s | 0.976 | [0.968, 0.984] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.128s | 1.158s | 0.976 | [0.968, 0.984] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.03ms | 3.04ms | 0.989 | [0.968, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.418s | 5.551s | 0.975 | [0.967, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.409s | 5.543s | 0.975 | [0.967, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.23ms | 3.46ms | 0.904 | [0.681, 1.201] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.229s | 1.231s | 0.994 | [0.980, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.157s | 1.160s | 0.994 | [0.982, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.28ms | 42.38ms | 0.987 | [0.846, 1.152] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225s | 1.250s | 0.982 | [0.976, 0.988] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.147s | 0.982 | [0.975, 0.990] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.82ms | 61.18ms | 0.961 | [0.917, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.20ms | 253.22ms | 0.980 | [0.976, 0.984] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.61ms | 249.52ms | 0.981 | [0.977, 0.984] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.29ms | 2.51ms | 0.895 | [0.766, 1.045] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.442s | 1.477s | 0.975 | [0.971, 0.980] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.433s | 1.467s | 0.975 | [0.970, 0.979] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.89ms | 4.22ms | 1.033 | [0.828, 1.287] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.773s | 6.891s | 0.982 | [0.976, 0.988] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.764s | 6.883s | 0.982 | [0.977, 0.986] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.22ms | 3.97ms | 1.039 | [0.680, 1.589] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 176.12ms | 175.41ms | 1.005 | [0.987, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 175.75ms | 175.06ms | 1.005 | [0.987, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 208.27us | 205.68us | 1.004 | [0.920, 1.097] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 244.04ms | 246.32ms | 0.992 | [0.972, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 243.90ms | 246.18ms | 0.992 | [0.972, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 72.98us | 74.17us | 0.986 | [0.915, 1.061] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.453s | 1.449s | 1.006 | [0.996, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.453s | 1.449s | 1.006 | [0.996, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 119.22us | 110.84us | 1.018 | [0.760, 1.363] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.259s | 1.266s | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.259s | 1.266s | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.77us | 63.84us | 1.074 | [0.965, 1.196] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.992s | 1.989s | 1.004 | [0.989, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.992s | 1.987s | 1.004 | [0.988, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.69us | 34.85us | 0.508 | [0.052, 4.922] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.12ms | 181.79ms | 0.976 | [0.933, 1.021] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.04ms | 181.71ms | 0.976 | [0.933, 1.021] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 27.73us | 26.61us | 1.031 | [0.720, 1.475] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 343.35ms | 340.42ms | 1.017 | [0.989, 1.045] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 343.24ms | 340.31ms | 1.017 | [0.989, 1.045] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 33.31us | 34.36us | 0.985 | [0.765, 1.268] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.23ms | 51.26ms | 0.981 | [0.869, 1.109] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.16ms | 51.19ms | 0.982 | [0.869, 1.109] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.46us | 20.53us | 0.941 | [0.756, 1.171] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 107.82ms | 109.32ms | 1.001 | [0.964, 1.039] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 101.68ms | 103.33ms | 1.000 | [0.959, 1.042] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.65ms | 1.66ms | 1.058 | [0.883, 1.268] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 436.55ms | 444.12ms | 0.963 | [0.907, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 433.82ms | 441.17ms | 0.962 | [0.907, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.08ms | 2.11ms | 1.016 | [0.894, 1.155] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.517s | 1.518s | 1.000 | [0.977, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.514s | 1.516s | 1.000 | [0.978, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.28ms | 2.16ms | 1.144 | [0.756, 1.731] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.464s | 2.479s | 0.999 | [0.979, 1.020] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.462s | 2.477s | 0.999 | [0.979, 1.020] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.70ms | 1.78ms | 0.970 | [0.898, 1.048] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 739.25ms | 749.27ms | 0.987 | [0.936, 1.041] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 735.39ms | 745.61ms | 0.987 | [0.935, 1.041] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.01ms | 3.01ms | 1.025 | [0.965, 1.088] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 484.80ms | 496.98ms | 0.973 | [0.886, 1.069] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 476.19ms | 488.52ms | 0.972 | [0.884, 1.070] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.51ms | 7.51ms | 0.999 | [0.983, 1.015] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.58ms | 27.03ms | 0.981 | [0.918, 1.050] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.88ms | 24.33ms | 0.977 | [0.908, 1.051] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.45ms | 2.43ms | 1.026 | [0.970, 1.086] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.14ms | 3.15ms | 0.974 | [0.940, 1.009] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.05ms | 2.12ms | 0.957 | [0.883, 1.038] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 867.00us | 815.46us | 1.010 | [0.901, 1.133] | ~ (noise) |

Sum of medians (total, common cases): A = 38.462s, B = 38.962s, saved = -499957161.0ns (0.987x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 2.52e-12 -> B 5.52e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 619.8 MiB | 1.000 | 1.6% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.5 MiB | 15.8 MiB | 1.018 | 3.5% / 4.1% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.4 MiB | 14.5 MiB | 1.009 | 3.3% / 3.7% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.7 MiB | 12.9 MiB | 1.016 | 2.4% / 3.4% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.7 MiB | 25.5 MiB | 0.994 | 3.1% / 1.9% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.3 MiB | 53.4 MiB | 1.002 | 3.6% / 4.6% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 59.7 MiB | 60.3 MiB | 1.011 | 11.5% / 6.8% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.4 MiB | 14.6 MiB | 1.009 | 1.2% / 2.6% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1080.7 MiB | 1073.2 MiB | 0.993 | 4.1% / 5.3% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.8 MiB | 40.4 MiB | 0.992 | 4.1% / 8.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.3 MiB | 17.7 MiB | 1.019 | 2.7% / 3.5% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.8 MiB | 14.8 MiB | 0.999 | 0.9% / 3.8% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 12.9 MiB | 1.002 | 2.4% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.0 MiB | 62.0 MiB | 0.999 | 1.6% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.7 MiB | 26.0 MiB | 1.012 | 4.8% / 4.7% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 41.6 MiB | 41.9 MiB | 1.009 | 2.0% / 3.7% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.8 MiB | 42.1 MiB | 1.008 | 3.3% / 3.2% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.0 MiB | 150.3 MiB | 1.002 | 1.3% / 1.5% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.1 MiB | 217.2 MiB | 1.000 | 0.1% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.8 MiB | 25.5 MiB | 0.992 | 2.3% / 2.3% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.5 MiB | 42.8 MiB | 1.007 | 4.5% / 6.5% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.2 MiB | 42.5 MiB | 1.006 | 3.9% / 2.3% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.0 MiB | 14.9 MiB | 0.997 | 4.4% / 2.8% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 11.9 MiB | 12.0 MiB | 1.006 | 0.8% / 3.2% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 11.9 MiB | 12.1 MiB | 1.014 | 1.8% / 1.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.8 MiB | 13.0 MiB | 1.020 | 5.2% / 3.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 23.8 MiB | 24.3 MiB | 1.021 | 4.0% / 4.6% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.8 MiB | 12.0 MiB | 1.016 | 1.6% / 1.9% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.8 MiB | 11.9 MiB | 1.011 | 1.8% / 2.1% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.8 MiB | 11.9 MiB | 1.007 | 1.2% / 1.7% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 19.4 MiB | 0.994 | 2.4% / 2.7% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.9 MiB | 13.0 MiB | 1.011 | 3.8% / 2.1% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 13.0 MiB | 1.021 | 3.1% / 4.3% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.3 MiB | 13.4 MiB | 1.003 | 3.6% / 6.6% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.3 MiB | 16.1 MiB | 0.991 | 4.1% / 1.5% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.3 MiB | 17.3 MiB | 1.001 | 2.3% / 5.0% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.8 MiB | 0.998 | 2.7% / 1.9% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.0 MiB | 0.983 | 1.3% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 1.005; largest B/A: `interp/sg/cc/b0/d10/L2/p8` 23.8 MiB -> 24.3 MiB (1.021x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.1%, 0.2%
Round durations (s): 164, 164, 163, 163, 163 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 818s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
