A/B `e2e`: A=`WP-17-before` B=`WP-17-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 401.22ms | 399.67ms | 1.010 | [0.905, 1.127] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 326.35ms | 330.09ms | 0.997 | [0.893, 1.114] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.76ms | 32.61ms | 0.985 | [0.877, 1.107] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.96ms | 261.21ms | 0.959 | [0.932, 0.986] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.22ms | 258.61ms | 0.957 | [0.933, 0.981] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.05ms | 1.96ms | 1.170 | [0.841, 1.629] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 497.62ms | 524.65ms | 0.953 | [0.942, 0.963] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | operator | 494.51ms | 522.07ms | 0.947 | [0.937, 0.957] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.85ms | 1.85ms | 1.518 | [0.453, 5.087] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 913.53ms | 973.34ms | 0.936 | [0.926, 0.945] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | operator | 911.00ms | 970.49ms | 0.936 | [0.926, 0.945] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.25ms | 2.26ms | 0.930 | [0.678, 1.277] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 301.11ms | 306.09ms | 0.981 | [0.971, 0.992] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | operator | 293.51ms | 297.95ms | 0.981 | [0.970, 0.993] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.11ms | 5.01ms | 0.991 | [0.819, 1.198] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 521.22ms | 541.16ms | 0.962 | [0.949, 0.974] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | operator | 497.79ms | 514.71ms | 0.962 | [0.947, 0.977] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.53ms | 15.52ms | 0.973 | [0.849, 1.116] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 232.04ms | 240.06ms | 0.978 | [0.959, 0.998] | SLOWER |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.94ms | 207.98ms | 0.984 | [0.974, 0.995] | SLOWER |
| quad/sg/equi-exp/b1/d20/L5 | generation | 17.85ms | 21.15ms | 0.992 | [0.734, 1.340] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.401s | 2.420s | 0.991 | [0.983, 0.999] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.398s | 2.417s | 0.991 | [0.983, 0.999] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.89ms | 1.87ms | 1.045 | [0.920, 1.186] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.226s | 1.243s | 0.976 | [0.883, 1.079] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.138s | 1.145s | 0.972 | [0.875, 1.080] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 82.56ms | 86.32ms | 0.975 | [0.844, 1.127] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 85.99ms | 90.20ms | 0.969 | [0.929, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.59ms | 87.63ms | 0.967 | [0.926, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.05ms | 1.009 | [0.932, 1.092] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.84ms | 340.50ms | 0.945 | [0.933, 0.959] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.97ms | 337.59ms | 0.944 | [0.931, 0.956] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.51ms | 2.28ms | 1.171 | [0.870, 1.577] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 693.25ms | 723.37ms | 0.959 | [0.953, 0.966] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 690.56ms | 720.78ms | 0.959 | [0.953, 0.965] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.03ms | 1.98ms | 1.004 | [0.931, 1.083] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.302s | 1.355s | 0.962 | [0.956, 0.969] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.299s | 1.351s | 0.962 | [0.956, 0.968] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.42ms | 2.33ms | 1.071 | [0.616, 1.863] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.13ms | 96.05ms | 0.975 | [0.963, 0.987] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.56ms | 94.71ms | 0.968 | [0.959, 0.977] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.07ms | 872.87us | 1.532 | [0.966, 2.429] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.96ms | 214.17ms | 0.942 | [0.937, 0.947] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.10ms | 211.54ms | 0.941 | [0.937, 0.946] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.23ms | 1.08ms | 1.122 | [0.870, 1.448] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.137s | 1.229s | 0.925 | [0.921, 0.928] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.130s | 1.222s | 0.924 | [0.921, 0.928] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.04ms | 3.04ms | 0.993 | [0.977, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.404s | 5.791s | 0.931 | [0.925, 0.938] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.396s | 5.782s | 0.931 | [0.925, 0.938] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.21ms | 3.32ms | 0.991 | [0.747, 1.316] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.220s | 1.262s | 0.957 | [0.935, 0.980] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.145s | 1.188s | 0.955 | [0.929, 0.981] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 45.34ms | 43.14ms | 1.028 | [0.881, 1.200] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225s | 1.246s | 0.984 | [0.980, 0.988] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.123s | 1.143s | 0.982 | [0.979, 0.986] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.27ms | 61.08ms | 1.010 | [0.956, 1.068] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.11ms | 266.03ms | 0.933 | [0.926, 0.940] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.46ms | 262.35ms | 0.930 | [0.925, 0.936] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.43ms | 2.49ms | 1.083 | [0.863, 1.358] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.440s | 1.532s | 0.940 | [0.936, 0.943] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.430s | 1.522s | 0.939 | [0.936, 0.943] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.51ms | 4.93ms | 0.943 | [0.823, 1.080] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.792s | 7.208s | 0.938 | [0.929, 0.947] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.782s | 7.198s | 0.938 | [0.929, 0.947] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.61ms | 4.43ms | 0.993 | [0.649, 1.517] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 173.73ms | 183.77ms | 0.945 | [0.927, 0.962] | SLOWER |
| interp/sg/cc/b0/d2/L12/p64 | operator | 173.36ms | 183.39ms | 0.945 | [0.927, 0.962] | SLOWER |
| interp/sg/cc/b0/d2/L12/p64 | generation | 206.66us | 209.10us | 0.955 | [0.833, 1.095] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 241.06ms | 253.26ms | 0.953 | [0.933, 0.972] | SLOWER |
| interp/sg/cc/b0/d3/L9/p64 | operator | 240.90ms | 253.11ms | 0.953 | [0.933, 0.972] | SLOWER |
| interp/sg/cc/b0/d3/L9/p64 | generation | 72.56us | 73.12us | 0.983 | [0.913, 1.059] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.440s | 1.487s | 0.965 | [0.957, 0.974] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.440s | 1.487s | 0.965 | [0.957, 0.974] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | generation | 116.28us | 106.65us | 1.014 | [0.655, 1.569] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.255s | 1.287s | 0.977 | [0.964, 0.990] | SLOWER |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.255s | 1.286s | 0.977 | [0.964, 0.990] | SLOWER |
| interp/sg/cc/b0/d7/L4/p16 | generation | 63.35us | 63.70us | 0.953 | [0.841, 1.080] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.973s | 2.011s | 0.977 | [0.964, 0.990] | SLOWER |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.973s | 2.011s | 0.977 | [0.964, 0.990] | SLOWER |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.78us | 34.29us | 0.668 | [0.211, 2.116] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.25ms | 183.63ms | 0.972 | [0.933, 1.012] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.17ms | 183.55ms | 0.972 | [0.933, 1.012] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.33us | 34.36us | 0.708 | [0.395, 1.270] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 333.87ms | 341.01ms | 0.975 | [0.928, 1.023] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 333.74ms | 340.90ms | 0.975 | [0.928, 1.023] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.64us | 35.27us | 1.019 | [0.792, 1.312] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.90ms | 50.92ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.84ms | 50.86ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 18.93us | 20.81us | 0.950 | [0.817, 1.106] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.53ms | 111.01ms | 0.982 | [0.946, 1.020] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.54ms | 104.77ms | 0.981 | [0.941, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.70ms | 1.72ms | 1.011 | [0.930, 1.099] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 443.41ms | 458.92ms | 0.953 | [0.895, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 440.47ms | 454.43ms | 0.954 | [0.895, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.23ms | 2.21ms | 0.834 | [0.540, 1.288] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.501s | 1.548s | 0.970 | [0.944, 0.996] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.498s | 1.545s | 0.970 | [0.944, 0.996] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.06ms | 2.14ms | 0.973 | [0.643, 1.472] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.450s | 2.517s | 0.945 | [0.886, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.447s | 2.515s | 0.945 | [0.886, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.77ms | 1.75ms | 1.026 | [0.949, 1.108] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 735.88ms | 708.35ms | 1.023 | [0.970, 1.080] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 732.15ms | 704.76ms | 1.023 | [0.970, 1.080] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.06ms | 2.99ms | 1.017 | [0.989, 1.045] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 491.77ms | 498.05ms | 0.981 | [0.951, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 483.30ms | 489.40ms | 0.980 | [0.950, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.54ms | 7.50ms | 0.995 | [0.973, 1.017] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.86ms | 28.33ms | 0.977 | [0.934, 1.023] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.13ms | 25.64ms | 0.974 | [0.925, 1.025] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.46ms | 2.43ms | 1.013 | [1.002, 1.024] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.22ms | 3.22ms | 0.998 | [0.944, 1.055] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.05ms | 2.09ms | 0.989 | [0.913, 1.071] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 862.19us | 908.00us | 0.995 | [0.853, 1.162] | ~ (noise) |

Sum of medians (total, common cases): A = 38.316s, B = 39.931s, saved = -1615608780.0ns (0.960x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.13e-16 -> B 3.84e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.7 MiB | 619.7 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.8 MiB | 15.7 MiB | 0.991 | 5.2% / 2.9% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 14.9 MiB | 1.022 | 3.2% / 4.0% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.0 MiB | 13.2 MiB | 1.014 | 3.2% / 3.7% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.8 MiB | 26.1 MiB | 1.012 | 2.4% / 3.1% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.0 MiB | 53.5 MiB | 1.009 | 4.9% / 8.6% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 57.7 MiB | 60.8 MiB | 1.054 | 6.0% / 12.9% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.6 MiB | 1.002 | 1.4% / 2.9% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1070.2 MiB | 1075.5 MiB | 1.005 | 3.4% / 4.4% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.6 MiB | 41.4 MiB | 1.021 | 4.2% / 6.6% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.4 MiB | 17.6 MiB | 1.011 | 3.9% / 3.5% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.0 MiB | 15.0 MiB | 0.999 | 4.9% / 3.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 13.2 MiB | 1.005 | 3.1% / 4.1% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 57.6 MiB | 0.922 | 3.5% / 2.5% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 26.0 MiB | 26.1 MiB | 1.004 | 3.1% / 3.8% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 43.4 MiB | 41.8 MiB | 0.963 | 3.8% / 2.2% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.8 MiB | 42.0 MiB | 1.005 | 2.4% / 2.3% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.4 MiB | 151.2 MiB | 0.999 | 3.2% / 2.7% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.5 MiB | 217.6 MiB | 1.001 | 0.2% / 2.0% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.6 MiB | 25.9 MiB | 1.011 | 3.0% / 3.5% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.0 MiB | 43.4 MiB | 1.033 | 4.9% / 3.2% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.8 MiB | 42.5 MiB | 0.994 | 2.0% / 2.7% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.1 MiB | 15.0 MiB | 0.997 | 2.5% / 2.7% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.2 MiB | 1.010 | 1.1% / 1.8% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.2 MiB | 0.999 | 0.9% / 2.4% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.2 MiB | 13.1 MiB | 0.993 | 3.5% / 3.2% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.3 MiB | 25.0 MiB | 1.027 | 4.5% / 7.6% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.1 MiB | 12.1 MiB | 1.001 | 3.1% / 2.0% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.1 MiB | 11.9 MiB | 0.984 | 2.0% / 3.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.0 MiB | 12.1 MiB | 1.011 | 0.8% / 2.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.7 MiB | 0.999 | 0.9% / 2.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.2 MiB | 0.995 | 2.3% / 2.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 13.1 MiB | 1.010 | 2.7% / 5.1% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.4 MiB | 13.5 MiB | 1.012 | 2.5% / 4.7% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.4 MiB | 16.4 MiB | 1.000 | 4.8% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.6 MiB | 17.7 MiB | 1.008 | 2.8% / 3.7% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.0 MiB | 13.1 MiB | 1.003 | 1.6% / 1.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.3 MiB | 12.4 MiB | 1.009 | 3.3% / 1.0% | ~ |

Peak memory (common cases): geomean B/A = 1.003; largest B/A: `quad/sg/equi-exp/b1/d20/L5` 57.7 MiB -> 60.8 MiB (1.054x); 0 MEM+, 1 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.2%, 0.2%, 0.2%
Round durations (s): 165, 165, 165, 166, 165 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 826s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
