A/B `e2e`: A=`WP-52-before` B=`WP-52-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 447.99ms | 438.42ms | 1.017 | [0.911, 1.135] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 363.80ms | 359.81ms | 1.012 | [0.906, 1.131] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.16ms | 31.33ms | 0.995 | [0.894, 1.108] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 266.67ms | 264.57ms | 1.006 | [0.979, 1.034] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 264.11ms | 261.98ms | 1.006 | [0.979, 1.034] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.03ms | 1.93ms | 1.016 | [0.890, 1.159] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 516.05ms | 513.87ms | 1.008 | [0.998, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 513.55ms | 511.31ms | 1.008 | [0.998, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.85ms | 1.87ms | 1.003 | [0.894, 1.124] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 910.90ms | 911.12ms | 1.001 | [0.995, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 907.92ms | 907.46ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.27ms | 2.28ms | 0.815 | [0.440, 1.509] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 299.48ms | 297.18ms | 1.003 | [0.972, 1.036] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 290.69ms | 289.56ms | 1.003 | [0.970, 1.038] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.16ms | 5.20ms | 1.002 | [0.782, 1.284] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 519.66ms | 524.16ms | 0.973 | [0.909, 1.041] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.19ms | 500.70ms | 0.971 | [0.904, 1.042] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.38ms | 15.03ms | 1.030 | [0.920, 1.154] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 234.86ms | 232.31ms | 1.028 | [0.990, 1.068] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 203.53ms | 202.25ms | 1.026 | [0.977, 1.077] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.24ms | 20.14ms | 1.058 | [0.783, 1.429] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.402s | 2.407s | 0.998 | [0.989, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.399s | 2.404s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.90ms | 1.89ms | 1.022 | [0.898, 1.164] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.204s | 1.203s | 1.016 | [0.966, 1.068] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.117s | 1.120s | 1.021 | [0.976, 1.068] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 74.68ms | 83.26ms | 0.948 | [0.705, 1.274] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.75ms | 87.47ms | 0.984 | [0.950, 1.020] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.86ms | 84.84ms | 0.987 | [0.966, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.04ms | 2.09ms | 0.906 | [0.680, 1.206] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 325.79ms | 322.90ms | 1.008 | [0.996, 1.020] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 321.81ms | 320.30ms | 1.004 | [0.993, 1.016] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.42ms | 2.33ms | 1.421 | [0.752, 2.688] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 693.91ms | 693.87ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 691.26ms | 691.32ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.02ms | 1.92ms | 1.040 | [0.964, 1.121] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.320s | 1.320s | 0.999 | [0.988, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.318s | 1.315s | 1.000 | [0.990, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.40ms | 3.38ms | 0.789 | [0.387, 1.608] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.62ms | 96.82ms | 0.993 | [0.983, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.32ms | 95.53ms | 0.994 | [0.985, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 996.35us | 1.05ms | 0.931 | [0.761, 1.140] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.68ms | 210.82ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 207.90ms | 208.05ms | 1.005 | [0.990, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.21ms | 1.14ms | 0.882 | [0.574, 1.356] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.173s | 1.172s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.165s | 1.165s | 1.001 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.01ms | 3.00ms | 1.010 | [0.993, 1.026] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.363s | 5.364s | 1.000 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.355s | 5.356s | 1.000 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.04ms | 3.10ms | 1.059 | [0.798, 1.406] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.224s | 1.225s | 0.998 | [0.993, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.151s | 1.152s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.95ms | 42.61ms | 1.009 | [0.865, 1.178] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.215s | 1.217s | 0.998 | [0.992, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.114s | 1.117s | 1.000 | [0.989, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.75ms | 60.36ms | 0.976 | [0.872, 1.091] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.08ms | 250.21ms | 0.996 | [0.993, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.31ms | 246.21ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.31ms | 2.46ms | 0.954 | [0.850, 1.072] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.441s | 1.442s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.432s | 1.432s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.01ms | 4.69ms | 1.046 | [0.853, 1.284] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.846s | 6.853s | 0.998 | [0.992, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.836s | 6.844s | 0.998 | [0.993, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.61ms | 4.22ms | 1.034 | [0.676, 1.580] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 130.88ms | 129.00ms | 1.010 | [0.991, 1.029] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 130.63ms | 128.74ms | 1.010 | [0.992, 1.029] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 206.24us | 204.77us | 0.996 | [0.877, 1.132] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 186.44ms | 186.50ms | 1.004 | [0.983, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 186.30ms | 186.35ms | 1.004 | [0.983, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.47us | 75.22us | 0.984 | [0.914, 1.060] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.124s | 1.127s | 0.990 | [0.970, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.124s | 1.126s | 0.990 | [0.970, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 152.67us | 160.28us | 0.928 | [0.740, 1.163] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.004s | 1.012s | 0.991 | [0.978, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.004s | 1.012s | 0.991 | [0.978, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 87.72us | 75.78us | 1.144 | [0.899, 1.454] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.598s | 1.622s | 0.988 | [0.975, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.598s | 1.622s | 0.988 | [0.975, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 44.77us | 43.79us | 0.764 | [0.242, 2.410] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.28ms | 34.32ms | 1.013 | [0.986, 1.041] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.20ms | 34.23ms | 1.013 | [0.986, 1.041] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 27.24us | 26.33us | 1.074 | [0.750, 1.536] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.41ms | 116.46ms | 1.006 | [0.981, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.31ms | 116.36ms | 1.006 | [0.981, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 38.97us | 52.52us | 0.941 | [0.731, 1.211] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.98ms | 12.01ms | 0.992 | [0.976, 1.009] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.91ms | 11.94ms | 0.992 | [0.975, 1.009] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.79us | 19.21us | 1.092 | [0.755, 1.580] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.26ms | 52.37ms | 0.991 | [0.973, 1.010] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.97ms | 52.07ms | 0.991 | [0.973, 1.010] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 135.28us | 135.14us | 0.981 | [0.926, 1.038] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.82ms | 42.86ms | 0.951 | [0.785, 1.152] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 43.63ms | 42.67ms | 0.951 | [0.784, 1.153] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 65.86us | 65.51us | 0.992 | [0.953, 1.032] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 171.56ms | 171.41ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 171.31ms | 171.16ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 79.13us | 77.66us | 1.053 | [0.945, 1.173] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 149.19ms | 149.07ms | 1.007 | [0.989, 1.026] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 149.01ms | 148.90ms | 1.007 | [0.989, 1.026] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 59.02us | 58.04us | 1.050 | [0.793, 1.388] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 393.17ms | 392.62ms | 0.997 | [0.984, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 393.07ms | 392.54ms | 0.997 | [0.984, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 36.25us | 34.29us | 1.099 | [0.761, 1.589] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.55ms | 6.52ms | 1.011 | [0.983, 1.040] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.46ms | 6.45ms | 1.011 | [0.983, 1.040] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 24.44us | 21.65us | 1.055 | [0.840, 1.325] | ~ (noise) |

Sum of medians (total, common cases): A = 32.069s, B = 32.101s, saved = -32326427.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 4.93e-12 -> B 7.01e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.0 MiB | 620.1 MiB | 1.000 | 0.1% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.8 MiB | 15.9 MiB | 1.006 | 2.0% / 3.4% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.7 MiB | 14.9 MiB | 1.013 | 2.8% / 4.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.2 MiB | 13.2 MiB | 1.006 | 2.1% / 2.1% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.4 MiB | 26.0 MiB | 0.986 | 4.1% / 2.5% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.2 MiB | 53.2 MiB | 1.001 | 3.6% / 3.9% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 60.1 MiB | 58.1 MiB | 0.966 | 14.4% / 12.4% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.8 MiB | 14.9 MiB | 1.008 | 4.6% / 3.9% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1067.6 MiB | 1080.6 MiB | 1.012 | 1.0% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.8 MiB | 39.9 MiB | 0.980 | 4.1% / 5.3% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.6 MiB | 17.6 MiB | 0.999 | 5.4% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.2 MiB | 15.2 MiB | 1.002 | 3.8% / 6.4% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 13.2 MiB | 1.008 | 1.2% / 1.8% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.6 MiB | 62.4 MiB | 0.997 | 1.7% / 1.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.5 MiB | 26.1 MiB | 0.984 | 4.6% / 3.7% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.4 MiB | 42.5 MiB | 1.002 | 5.9% / 3.4% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.1 MiB | 42.3 MiB | 1.004 | 5.0% / 4.9% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.9 MiB | 151.1 MiB | 1.001 | 4.2% / 3.0% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.6 MiB | 217.5 MiB | 0.999 | 0.9% / 1.0% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.9 MiB | 25.7 MiB | 0.991 | 4.4% / 3.3% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 43.0 MiB | 42.9 MiB | 0.998 | 8.8% / 5.1% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 43.0 MiB | 42.3 MiB | 0.983 | 2.4% / 5.9% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 16.2 MiB | 15.8 MiB | 0.972 | 2.0% / 2.8% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.3 MiB | 1.003 | 3.7% / 3.0% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.4 MiB | 12.3 MiB | 0.994 | 3.8% / 3.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.3 MiB | 12.3 MiB | 1.004 | 1.2% / 3.1% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.1 MiB | 12.1 MiB | 1.002 | 1.8% / 1.5% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.2 MiB | 1.000 | 1.4% / 2.0% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.3 MiB | 1.007 | 1.6% / 3.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.2 MiB | 0.993 | 2.8% / 1.4% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.1 MiB | 16.4 MiB | 1.015 | 3.3% / 4.7% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.7 MiB | 12.7 MiB | 0.998 | 2.1% / 1.4% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.5 MiB | 12.5 MiB | 1.000 | 4.0% / 1.7% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.5 MiB | 12.4 MiB | 0.988 | 1.2% / 1.5% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.1 MiB | 0.993 | 1.4% / 2.1% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.2 MiB | 0.999 | 3.6% / 4.2% | ~ |

Peak memory (common cases): geomean B/A = 0.998; largest B/A: `interp/dsg/cc/b0/d2/L12/p64` 16.1 MiB -> 16.4 MiB (1.015x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 137, 136, 137, 136, 137 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 683s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
