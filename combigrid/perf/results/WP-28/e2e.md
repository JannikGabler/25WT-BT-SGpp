A/B `e2e`: A=`WP-28-before` B=`WP-28-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 401.63ms | 402.00ms | 0.978 | [0.867, 1.103] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 330.11ms | 323.24ms | 0.986 | [0.852, 1.139] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.48ms | 31.03ms | 1.010 | [0.907, 1.125] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.63ms | 247.89ms | 0.989 | [0.963, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.22ms | 245.37ms | 0.989 | [0.963, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.94ms | 2.05ms | 0.947 | [0.830, 1.081] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 497.87ms | 496.87ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 495.38ms | 494.44ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.82ms | 1.81ms | 1.002 | [0.946, 1.060] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 915.17ms | 914.72ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 912.24ms | 912.21ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.19ms | 2.25ms | 0.873 | [0.560, 1.360] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 301.96ms | 301.48ms | 1.000 | [0.980, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 293.54ms | 293.03ms | 1.000 | [0.979, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.21ms | 5.22ms | 0.999 | [0.826, 1.209] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 519.70ms | 519.18ms | 1.001 | [0.989, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.32ms | 496.46ms | 1.000 | [0.986, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.85ms | 13.84ms | 1.051 | [0.932, 1.185] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.69ms | 231.85ms | 1.025 | [1.010, 1.039] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.29ms | 204.94ms | 1.008 | [0.984, 1.031] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.27ms | 17.19ms | 1.203 | [0.890, 1.625] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.400s | 2.400s | 1.000 | [0.992, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.397s | 2.398s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.88ms | 1.84ms | 0.976 | [0.879, 1.084] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.209s | 1.227s | 1.003 | [0.905, 1.112] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.122s | 1.135s | 1.000 | [0.897, 1.115] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 87.63ms | 83.80ms | 1.013 | [0.877, 1.171] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.25ms | 86.69ms | 1.002 | [0.979, 1.026] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.45ms | 84.01ms | 0.999 | [0.976, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.04ms | 1.111 | [0.831, 1.486] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.73ms | 321.38ms | 1.001 | [0.994, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 317.32ms | 318.05ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.11ms | 2.28ms | 0.980 | [0.573, 1.678] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 691.38ms | 693.06ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 688.76ms | 690.41ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.01ms | 1.97ms | 1.037 | [0.962, 1.119] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.307s | 1.302s | 1.002 | [0.995, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.304s | 1.299s | 1.002 | [0.996, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.43ms | 2.55ms | 1.050 | [0.614, 1.795] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.18ms | 92.40ms | 1.005 | [0.991, 1.019] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.88ms | 91.05ms | 1.006 | [0.991, 1.020] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.04ms | 1.07ms | 0.950 | [0.776, 1.162] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.09ms | 202.36ms | 1.005 | [0.988, 1.022] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.54ms | 199.97ms | 1.004 | [0.988, 1.020] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.28ms | 1.14ms | 1.103 | [0.855, 1.424] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.137s | 1.135s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.129s | 1.127s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.12ms | 3.12ms | 0.976 | [0.909, 1.047] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.397s | 5.398s | 0.998 | [0.992, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.389s | 5.389s | 0.998 | [0.992, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.34ms | 3.20ms | 0.998 | [0.752, 1.325] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.218s | 1.221s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.147s | 1.147s | 1.002 | [0.998, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.22ms | 43.08ms | 0.960 | [0.823, 1.121] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225s | 1.225s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.124s | 1.003 | [0.997, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.18ms | 60.60ms | 0.948 | [0.866, 1.037] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.53ms | 248.65ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.69ms | 245.01ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.42ms | 2.41ms | 0.940 | [0.723, 1.222] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.438s | 1.441s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.429s | 1.431s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.30ms | 4.99ms | 0.882 | [0.742, 1.049] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.785s | 6.785s | 1.016 | [0.964, 1.071] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.775s | 6.776s | 1.016 | [0.964, 1.072] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.45ms | 4.60ms | 1.013 | [0.663, 1.548] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 173.99ms | 173.88ms | 1.000 | [0.981, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 173.64ms | 173.50ms | 1.000 | [0.981, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 216.58us | 208.19us | 1.003 | [0.919, 1.096] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 243.48ms | 241.72ms | 1.021 | [0.987, 1.056] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 243.33ms | 241.57ms | 1.021 | [0.987, 1.056] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 75.99us | 73.05us | 1.047 | [0.921, 1.190] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.442s | 1.446s | 0.991 | [0.974, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.441s | 1.445s | 0.991 | [0.974, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.53us | 97.78us | 1.074 | [0.824, 1.400] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.256s | 1.256s | 1.000 | [0.987, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.256s | 1.255s | 1.000 | [0.987, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 63.62us | 62.93us | 1.032 | [0.968, 1.101] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.994s | 1.978s | 1.004 | [0.995, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.994s | 1.978s | 1.004 | [0.995, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.92us | 34.29us | 1.525 | [0.465, 5.000] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.59ms | 181.56ms | 1.002 | [0.995, 1.008] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.51ms | 181.48ms | 1.002 | [0.995, 1.008] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.22us | 27.80us | 0.933 | [0.652, 1.335] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 335.19ms | 332.61ms | 1.011 | [0.998, 1.025] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.08ms | 332.51ms | 1.011 | [0.998, 1.025] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.85us | 32.90us | 1.025 | [0.796, 1.319] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.95ms | 50.84ms | 1.024 | [0.960, 1.092] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.89ms | 50.77ms | 1.024 | [0.959, 1.092] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 19.49us | 20.11us | 0.964 | [0.827, 1.122] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.88ms | 111.62ms | 0.923 | [0.809, 1.054] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.83ms | 105.68ms | 0.920 | [0.802, 1.056] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.71ms | 1.69ms | 1.009 | [0.928, 1.097] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 439.20ms | 445.57ms | 0.986 | [0.946, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 436.21ms | 442.81ms | 0.985 | [0.944, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.28ms | 2.11ms | 1.167 | [0.804, 1.694] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.514s | 1.516s | 0.993 | [0.971, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.512s | 1.513s | 0.993 | [0.971, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.39ms | 2.23ms | 1.142 | [0.617, 2.114] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.432s | 2.428s | 1.002 | [0.981, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.430s | 2.425s | 1.002 | [0.981, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.75ms | 1.74ms | 0.986 | [0.913, 1.066] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 752.72ms | 735.23ms | 1.014 | [0.954, 1.079] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 749.03ms | 731.58ms | 1.015 | [0.954, 1.080] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.02ms | 2.99ms | 0.886 | [0.615, 1.277] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 491.93ms | 503.83ms | 1.000 | [0.915, 1.093] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 483.54ms | 495.43ms | 1.000 | [0.914, 1.095] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.45ms | 7.57ms | 0.982 | [0.962, 1.003] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.14ms | 26.25ms | 0.993 | [0.951, 1.036] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.43ms | 23.55ms | 0.992 | [0.943, 1.044] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.44ms | 2.43ms | 0.999 | [0.989, 1.008] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.18ms | 3.16ms | 0.931 | [0.785, 1.104] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.06ms | 2.06ms | 0.876 | [0.651, 1.180] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 876.99us | 871.54us | 1.083 | [0.889, 1.319] | ~ (noise) |

Sum of medians (total, common cases): A = 38.324s, B = 38.323s, saved = 828.42us (1.000x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 1.32e-12 -> B 1.01e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.9 MiB | 620.0 MiB | 1.000 | 0.0% / 2.2% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.8 MiB | 15.8 MiB | 0.998 | 5.1% / 4.3% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.4 MiB | 14.6 MiB | 1.017 | 3.8% / 4.0% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.9 MiB | 13.1 MiB | 1.022 | 2.0% / 3.2% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 26.0 MiB | 1.004 | 1.5% / 2.0% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.5 MiB | 51.6 MiB | 0.964 | 4.0% / 6.8% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 60.3 MiB | 56.3 MiB | 0.934 | 7.2% / 12.5% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 14.8 MiB | 1.007 | 3.6% / 0.7% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1075.2 MiB | 1069.5 MiB | 0.995 | 4.2% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.2 MiB | 40.8 MiB | 1.015 | 5.7% / 3.0% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.5 MiB | 17.5 MiB | 0.999 | 3.0% / 4.0% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 15.0 MiB | 1.002 | 2.0% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.0 MiB | 1.005 | 1.3% / 2.6% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.2 MiB | 62.6 MiB | 1.006 | 3.3% / 3.3% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.1 MiB | 26.3 MiB | 1.007 | 5.1% / 2.6% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 41.5 MiB | 42.1 MiB | 1.015 | 1.1% / 5.0% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.5 MiB | 41.7 MiB | 1.007 | 2.5% / 3.3% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.5 MiB | 151.6 MiB | 1.001 | 2.2% / 2.8% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.2 MiB | 217.5 MiB | 1.001 | 0.1% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 26.0 MiB | 25.8 MiB | 0.994 | 5.6% / 2.9% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 43.0 MiB | 41.8 MiB | 0.971 | 5.7% / 3.6% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.9 MiB | 43.1 MiB | 1.006 | 4.2% / 2.1% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.0 MiB | 15.1 MiB | 1.007 | 2.6% / 2.1% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.2 MiB | 1.013 | 2.3% / 3.7% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.2 MiB | 1.008 | 2.1% / 1.5% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.1 MiB | 13.2 MiB | 1.008 | 3.8% / 2.3% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 23.9 MiB | 24.8 MiB | 1.037 | 8.3% / 2.1% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 12.0 MiB | 1.005 | 1.1% / 2.1% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.1 MiB | 1.012 | 2.9% / 1.6% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.0 MiB | 12.2 MiB | 1.019 | 2.6% / 2.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.7 MiB | 1.000 | 2.7% / 3.2% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.2 MiB | 0.998 | 3.0% / 3.0% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.1 MiB | 13.0 MiB | 0.995 | 4.0% / 3.6% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.0 MiB | 13.8 MiB | 1.060 | 8.7% / 9.8% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.3 MiB | 16.2 MiB | 0.994 | 4.1% / 4.2% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.1 MiB | 17.5 MiB | 1.022 | 4.2% / 3.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 13.0 MiB | 1.001 | 1.4% / 2.0% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.3 MiB | 12.4 MiB | 1.011 | 2.2% / 2.8% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.0 MiB -> 13.8 MiB (1.060x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.1%, 0.1%, 0.2%
Round durations (s): 163, 164, 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 811s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
