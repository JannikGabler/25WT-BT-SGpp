A/B `e2e`: A=`WP-40-before` B=`WP-40-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 405.08ms | 385.77ms | 1.038 | [0.931, 1.159] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 345.62ms | 322.42ms | 1.034 | [0.925, 1.155] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.66ms | 29.39ms | 1.033 | [0.928, 1.150] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.67ms | 247.93ms | 0.999 | [0.987, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.14ms | 245.32ms | 0.999 | [0.988, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.96ms | 2.08ms | 0.959 | [0.803, 1.144] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 497.22ms | 497.47ms | 1.009 | [0.977, 1.041] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 494.84ms | 495.10ms | 1.009 | [0.977, 1.041] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.86ms | 1.81ms | 1.035 | [0.963, 1.113] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 914.37ms | 915.44ms | 0.999 | [0.982, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 911.83ms | 912.67ms | 0.999 | [0.983, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.18ms | 2.07ms | 0.936 | [0.498, 1.759] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 306.17ms | 306.52ms | 1.004 | [0.979, 1.031] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 297.78ms | 297.39ms | 1.001 | [0.982, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.23ms | 5.25ms | 1.168 | [0.760, 1.796] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 521.00ms | 519.32ms | 1.002 | [0.989, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.66ms | 495.29ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.82ms | 15.72ms | 1.021 | [0.912, 1.144] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 231.34ms | 235.66ms | 0.989 | [0.961, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.18ms | 205.49ms | 0.994 | [0.976, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.02ms | 19.49ms | 0.977 | [0.723, 1.320] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.400s | 2.402s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.398s | 2.399s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.87ms | 1.90ms | 1.002 | [0.902, 1.113] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.223s | 1.209s | 1.003 | [0.920, 1.093] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.133s | 1.113s | 0.998 | [0.916, 1.087] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 81.45ms | 75.66ms | 1.053 | [0.896, 1.238] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.27ms | 85.78ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.10ms | 83.62ms | 1.005 | [0.991, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.03ms | 1.032 | [0.976, 1.092] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.94ms | 321.91ms | 1.010 | [0.989, 1.032] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.97ms | 317.84ms | 1.006 | [0.994, 1.019] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.48ms | 2.32ms | 1.313 | [0.537, 3.210] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 692.28ms | 691.73ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 689.61ms | 688.99ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.97ms | 2.00ms | 0.971 | [0.901, 1.047] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.307s | 1.305s | 1.000 | [0.992, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.303s | 1.303s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.40ms | 2.26ms | 1.311 | [0.868, 1.980] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 91.49ms | 91.47ms | 0.993 | [0.963, 1.023] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 89.85ms | 90.01ms | 0.994 | [0.963, 1.025] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 988.32us | 1.15ms | 0.940 | [0.726, 1.216] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.57ms | 202.31ms | 0.996 | [0.959, 1.035] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.25ms | 199.79ms | 0.996 | [0.959, 1.035] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.02ms | 1.19ms | 0.946 | [0.733, 1.220] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.134s | 1.134s | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.126s | 1.127s | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.00ms | 2.99ms | 1.001 | [0.984, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.403s | 5.407s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.394s | 5.398s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.38ms | 3.29ms | 0.992 | [0.747, 1.318] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.219s | 1.218s | 1.001 | [0.996, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.146s | 1.146s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.07ms | 42.84ms | 0.990 | [0.848, 1.156] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.228s | 1.225s | 1.006 | [0.995, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.127s | 1.124s | 1.007 | [0.996, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.76ms | 60.61ms | 0.986 | [0.951, 1.023] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.36ms | 249.06ms | 0.993 | [0.972, 1.014] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.71ms | 245.43ms | 0.994 | [0.973, 1.015] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.31ms | 2.27ms | 0.949 | [0.809, 1.114] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.450s | 1.444s | 0.999 | [0.988, 1.010] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.441s | 1.434s | 1.000 | [0.988, 1.011] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.43ms | 4.96ms | 0.900 | [0.778, 1.041] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.782s | 6.819s | 0.996 | [0.990, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.772s | 6.809s | 0.996 | [0.991, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.60ms | 4.00ms | 1.111 | [0.727, 1.699] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 174.36ms | 173.18ms | 1.009 | [0.990, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 173.93ms | 172.77ms | 1.009 | [0.991, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 220.21us | 215.88us | 1.028 | [0.941, 1.123] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 242.13ms | 241.78ms | 1.001 | [0.981, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 241.99ms | 241.63ms | 1.001 | [0.981, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.54us | 74.45us | 1.029 | [0.914, 1.158] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.447s | 1.449s | 1.000 | [0.995, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.447s | 1.448s | 1.000 | [0.994, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 119.01us | 97.92us | 1.179 | [0.940, 1.478] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.260s | 1.262s | 1.033 | [0.938, 1.138] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.259s | 1.262s | 1.033 | [0.938, 1.138] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.35us | 66.77us | 0.992 | [0.906, 1.086] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.971s | 1.982s | 1.012 | [0.970, 1.056] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.971s | 1.982s | 1.012 | [0.970, 1.056] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.13us | 35.27us | 1.674 | [0.414, 6.762] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.50ms | 181.68ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.42ms | 181.59ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.63us | 28.56us | 0.682 | [0.211, 2.203] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 338.35ms | 334.09ms | 1.012 | [0.999, 1.026] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 338.24ms | 333.97ms | 1.012 | [0.999, 1.025] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 36.11us | 34.78us | 0.979 | [0.761, 1.260] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.85ms | 50.63ms | 0.957 | [0.839, 1.093] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.78ms | 50.56ms | 0.957 | [0.838, 1.093] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.02us | 20.53us | 1.018 | [0.875, 1.184] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.23ms | 111.31ms | 0.974 | [0.938, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.29ms | 105.32ms | 0.973 | [0.934, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.66ms | 1.66ms | 0.985 | [0.906, 1.071] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 439.20ms | 448.15ms | 0.982 | [0.957, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 435.05ms | 445.25ms | 0.981 | [0.956, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.17ms | 2.19ms | 1.083 | [0.808, 1.450] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.522s | 1.563s | 0.997 | [0.935, 1.062] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.520s | 1.561s | 0.997 | [0.935, 1.063] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.38ms | 2.28ms | 1.010 | [0.569, 1.794] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.486s | 2.505s | 0.977 | [0.929, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.484s | 2.502s | 0.977 | [0.929, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.77ms | 1.76ms | 1.014 | [0.939, 1.096] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 726.93ms | 734.92ms | 0.980 | [0.921, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 723.22ms | 731.21ms | 0.980 | [0.921, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.98ms | 3.00ms | 0.993 | [0.962, 1.024] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 493.30ms | 485.72ms | 1.014 | [0.986, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 484.77ms | 477.07ms | 1.014 | [0.986, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.48ms | 7.50ms | 0.993 | [0.978, 1.008] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.31ms | 26.65ms | 1.004 | [0.966, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.64ms | 23.99ms | 1.004 | [0.955, 1.056] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.43ms | 1.000 | [0.988, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.13ms | 3.27ms | 0.921 | [0.757, 1.120] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.06ms | 2.08ms | 1.019 | [0.941, 1.103] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 831.04us | 945.37us | 0.756 | [0.476, 1.198] | ~ (noise) |

Sum of medians (total, common cases): A = 38.382s, B = 38.466s, saved = -84352647.0ns (0.998x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.24e-15 -> B 2.15e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.0 MiB | 619.9 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.9 MiB | 15.8 MiB | 0.995 | 4.1% / 3.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 14.7 MiB | 1.011 | 3.4% / 1.9% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.9 MiB | 13.1 MiB | 1.012 | 2.7% / 1.3% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.8 MiB | 25.9 MiB | 1.005 | 1.6% / 4.8% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.9 MiB | 53.3 MiB | 0.988 | 2.4% / 4.3% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 57.3 MiB | 59.5 MiB | 1.039 | 11.7% / 13.1% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.7 MiB | 1.009 | 2.7% / 2.0% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1060.2 MiB | 1071.2 MiB | 1.010 | 2.2% / 4.3% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.3 MiB | 41.1 MiB | 1.019 | 4.9% / 6.5% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.6 MiB | 17.6 MiB | 0.997 | 8.5% / 3.3% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 15.2 MiB | 1.018 | 5.1% / 6.1% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 13.2 MiB | 1.004 | 2.3% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 62.2 MiB | 0.996 | 3.1% / 2.1% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.9 MiB | 26.0 MiB | 1.007 | 5.0% / 3.8% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.7 MiB | 41.7 MiB | 0.978 | 7.2% / 5.2% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.0 MiB | 42.5 MiB | 1.011 | 2.9% / 2.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.5 MiB | 151.3 MiB | 0.999 | 3.7% / 3.0% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.3 MiB | 217.4 MiB | 1.001 | 0.2% / 0.8% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.8 MiB | 26.0 MiB | 1.007 | 1.9% / 2.4% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.8 MiB | 43.0 MiB | 1.005 | 5.6% / 5.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.7 MiB | 42.1 MiB | 0.986 | 3.1% / 2.8% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 15.1 MiB | 1.014 | 2.4% / 4.0% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.4 MiB | 1.018 | 1.9% / 1.2% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.2 MiB | 0.998 | 1.3% / 1.2% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.1 MiB | 13.1 MiB | 0.997 | 4.2% / 2.7% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.4 MiB | 24.7 MiB | 1.012 | 3.4% / 2.0% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 12.2 MiB | 1.016 | 1.7% / 1.5% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.2 MiB | 1.010 | 1.4% / 1.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.0 MiB | 12.1 MiB | 1.004 | 3.3% / 0.9% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.7 MiB | 1.000 | 2.7% / 3.6% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.3 MiB | 1.005 | 2.2% / 1.4% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 13.0 MiB | 1.013 | 3.5% / 2.2% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.4 MiB | 13.2 MiB | 0.986 | 6.0% / 6.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.5 MiB | 16.3 MiB | 0.990 | 2.3% / 3.8% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.1 MiB | 17.5 MiB | 1.024 | 5.3% / 5.1% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 13.0 MiB | 1.009 | 2.3% / 2.6% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.4 MiB | 1.017 | 3.8% / 1.6% | ~ |

Peak memory (common cases): geomean B/A = 1.005; largest B/A: `quad/sg/equi-exp/b1/d20/L5` 57.3 MiB -> 59.5 MiB (1.039x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.4%, 0.4%, 0.4%
Round durations (s): 163, 163, 162, 163, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 812s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
