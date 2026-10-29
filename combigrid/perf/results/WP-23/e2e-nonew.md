A/B `e2e`: A=`WP-23-before` B=`WP-23-nonew`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 402.51ms | 446.74ms | 0.907 | [0.812, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 327.85ms | 345.72ms | 0.966 | [0.864, 1.079] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.97ms | 31.13ms | 0.996 | [0.895, 1.109] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 252.64ms | 252.42ms | 1.001 | [0.990, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 250.03ms | 249.94ms | 1.000 | [0.989, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.06ms | 1.83ms | 1.093 | [0.958, 1.248] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 501.88ms | 502.45ms | 1.001 | [0.991, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 499.40ms | 500.20ms | 1.002 | [0.991, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.74ms | 1.73ms | 0.912 | [0.645, 1.291] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 935.99ms | 935.42ms | 1.000 | [0.994, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 932.99ms | 932.92ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.24ms | 2.09ms | 1.096 | [0.562, 2.140] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 307.80ms | 301.84ms | 1.007 | [0.985, 1.029] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 299.25ms | 295.60ms | 1.003 | [0.979, 1.027] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.33ms | 3.77ms | 1.199 | [0.991, 1.450] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 521.21ms | 518.77ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 497.20ms | 497.37ms | 1.000 | [0.986, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.72ms | 14.19ms | 1.072 | [0.957, 1.201] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 239.00ms | 237.15ms | 1.022 | [0.988, 1.057] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 209.08ms | 208.70ms | 1.011 | [0.986, 1.037] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.11ms | 19.04ms | 1.097 | [0.787, 1.531] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.415s | 2.412s | 1.001 | [0.993, 1.010] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.412s | 2.410s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.93ms | 1.72ms | 1.123 | [1.011, 1.248] | faster |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.209s | 1.216s | 0.971 | [0.924, 1.020] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.123s | 1.129s | 0.963 | [0.911, 1.019] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 82.66ms | 81.87ms | 1.086 | [0.828, 1.424] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.07ms | 87.30ms | 0.995 | [0.981, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.51ms | 85.06ms | 0.995 | [0.983, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 2.05ms | 1.026 | [0.973, 1.081] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 325.75ms | 325.90ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 322.80ms | 323.06ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.42ms | 2.26ms | 1.071 | [1.000, 1.147] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 700.61ms | 699.55ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 698.08ms | 697.24ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.94ms | 1.82ms | 1.064 | [0.987, 1.147] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.314s | 1.313s | 1.003 | [0.993, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.311s | 1.311s | 1.003 | [0.993, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.38ms | 2.19ms | 1.073 | [0.663, 1.737] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.56ms | 93.28ms | 0.999 | [0.985, 1.014] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.26ms | 92.21ms | 0.996 | [0.981, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.03ms | 800.03us | 1.346 | [1.100, 1.647] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.88ms | 201.78ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.16ms | 199.53ms | 0.997 | [0.992, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.15ms | 879.86us | 1.304 | [1.010, 1.682] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.135s | 1.131s | 1.007 | [0.997, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.128s | 1.126s | 1.006 | [0.995, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.02ms | 2.86ms | 1.131 | [1.026, 1.247] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.404s | 5.392s | 1.002 | [0.998, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.395s | 5.385s | 1.001 | [0.998, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 2.99ms | 3.32ms | 0.954 | [0.719, 1.267] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.218s | 1.211s | 1.005 | [1.001, 1.010] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.146s | 1.148s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.07ms | 39.19ms | 1.129 | [0.967, 1.318] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.227s | 1.215s | 1.010 | [1.004, 1.016] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.123s | 1.125s | 1.000 | [0.993, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.30ms | 55.78ms | 1.088 | [0.886, 1.337] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.73ms | 247.86ms | 1.003 | [0.999, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.09ms | 244.50ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.40ms | 2.15ms | 1.158 | [1.031, 1.301] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.439s | 1.434s | 1.002 | [0.996, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.429s | 1.426s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.91ms | 3.41ms | 1.374 | [1.199, 1.573] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.787s | 6.770s | 0.999 | [0.983, 1.015] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.777s | 6.762s | 0.999 | [0.983, 1.015] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.99ms | 4.03ms | 1.037 | [0.678, 1.585] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 176.59ms | 179.75ms | 0.965 | [0.858, 1.085] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 176.05ms | 179.36ms | 0.964 | [0.857, 1.085] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 205.40us | 209.03us | 0.988 | [0.905, 1.079] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 245.97ms | 289.86ms | 0.878 | [0.774, 0.996] | SLOWER |
| interp/sg/cc/b0/d3/L9/p64 | operator | 245.81ms | 289.73ms | 0.878 | [0.774, 0.996] | SLOWER |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.54us | 71.52us | 1.028 | [0.955, 1.107] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.455s | 1.454s | 1.000 | [0.992, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.455s | 1.454s | 1.000 | [0.992, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 109.37us | 102.60us | 1.018 | [0.812, 1.277] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.265s | 1.269s | 0.997 | [0.984, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.265s | 1.269s | 0.997 | [0.984, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.35us | 58.18us | 0.686 | [0.193, 2.440] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.004s | 2.039s | 0.978 | [0.948, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.004s | 2.039s | 0.978 | [0.948, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.20us | 32.20us | 1.087 | [0.989, 1.194] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.34ms | 179.91ms | 1.008 | [1.003, 1.013] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.26ms | 179.84ms | 1.008 | [1.002, 1.013] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.50us | 25.70us | 1.035 | [0.723, 1.481] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 341.80ms | 328.79ms | 1.038 | [1.019, 1.057] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 341.69ms | 328.68ms | 1.038 | [1.019, 1.057] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.57us | 32.90us | 0.981 | [0.762, 1.263] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.13ms | 50.40ms | 0.996 | [0.951, 1.043] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.07ms | 50.35ms | 0.996 | [0.951, 1.043] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.60us | 21.79us | 1.016 | [0.682, 1.511] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 108.17ms | 110.09ms | 0.982 | [0.943, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 101.95ms | 105.11ms | 0.970 | [0.929, 1.012] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.67ms | 1.69ms | 0.988 | [0.909, 1.074] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 435.23ms | 436.90ms | 0.997 | [0.972, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 432.36ms | 434.32ms | 0.997 | [0.971, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.16ms | 2.23ms | 0.978 | [0.860, 1.112] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.536s | 2.196s | 0.806 | [0.631, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.533s | 2.193s | 0.805 | [0.631, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.24ms | 2.17ms | 1.040 | [0.687, 1.573] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.470s | 2.491s | 0.962 | [0.863, 1.072] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.467s | 2.489s | 0.962 | [0.863, 1.072] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.80ms | 1.70ms | 0.993 | [0.881, 1.120] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 741.16ms | 734.75ms | 1.007 | [0.954, 1.062] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 737.43ms | 731.84ms | 1.005 | [0.953, 1.061] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.00ms | 2.21ms | 1.344 | [1.287, 1.404] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 493.18ms | 481.45ms | 1.015 | [0.981, 1.050] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 482.90ms | 475.01ms | 1.010 | [0.976, 1.046] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.57ms | 5.60ms | 1.407 | [1.254, 1.578] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.09ms | 27.52ms | 0.977 | [0.935, 1.021] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.39ms | 25.17ms | 0.955 | [0.908, 1.004] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 1.94ms | 1.233 | [1.170, 1.300] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.15ms | 2.95ms | 1.030 | [0.949, 1.118] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.05ms | 2.06ms | 0.976 | [0.886, 1.075] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 808.27us | 687.79us | 1.161 | [1.026, 1.313] | faster |

Sum of medians (total, common cases): A = 38.498s, B = 39.218s, saved = -719916786.0ns (0.982x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 6.40e-13 -> B 1.07e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 619.7 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.6 MiB | 15.2 MiB | 0.978 | 2.9% / 4.5% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.2 MiB | 13.7 MiB | 0.962 | 2.8% / 2.6% | MEM- |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.7 MiB | 0.994 | 1.6% / 3.5% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 22.4 MiB | 0.865 | 3.9% / 2.0% | MEM- |
| quad/sg/equi-exp/b1/d15/L6 | 53.1 MiB | 47.1 MiB | 0.886 | 1.1% / 5.9% | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 59.6 MiB | 52.9 MiB | 0.888 | 13.8% / 12.2% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.5 MiB | 13.9 MiB | 0.955 | 3.2% / 2.3% | MEM- |
| quad/sg/cc-uq/b0/d2/L20 | 1081.1 MiB | 1060.8 MiB | 0.981 | 3.3% / 1.7% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.9 MiB | 40.4 MiB | 0.986 | 6.7% / 5.1% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.4 MiB | 17.0 MiB | 0.979 | 2.4% / 1.6% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 14.3 MiB | 0.963 | 2.1% / 3.4% | MEM- |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 12.6 MiB | 0.973 | 1.8% / 3.1% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 61.7 MiB | 57.0 MiB | 0.924 | 1.5% / 2.7% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 25.9 MiB | 24.6 MiB | 0.952 | 2.1% / 4.1% | MEM- |
| quad/dsg/equi-exp/b0/d7/L10 | 42.4 MiB | 37.7 MiB | 0.889 | 7.5% / 4.8% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 42.0 MiB | 37.2 MiB | 0.885 | 2.5% / 2.6% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 151.1 MiB | 127.4 MiB | 0.844 | 4.5% / 2.1% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 217.3 MiB | 194.0 MiB | 0.893 | 0.9% / 0.6% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 25.9 MiB | 24.2 MiB | 0.934 | 3.4% / 3.5% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 42.8 MiB | 37.1 MiB | 0.865 | 6.3% / 5.0% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 42.1 MiB | 38.1 MiB | 0.906 | 2.7% / 2.2% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 14.7 MiB | 14.6 MiB | 0.988 | 2.7% / 3.7% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.0 MiB | 1.003 | 2.6% / 0.5% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.0 MiB | 0.997 | 1.4% / 1.8% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 12.6 MiB | 0.981 | 2.2% / 2.8% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.6 MiB | 24.1 MiB | 0.980 | 6.0% / 5.7% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 11.9 MiB | 0.996 | 1.4% / 4.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 11.9 MiB | 0.995 | 3.2% / 2.4% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.8 MiB | 0.996 | 3.2% / 1.9% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 18.9 MiB | 0.969 | 2.9% / 1.8% | MEM- |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.7 MiB | 0.988 | 1.3% / 2.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.6 MiB | 0.965 | 5.7% / 2.2% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 12.9 MiB | 0.977 | 4.8% / 4.9% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 15.8 MiB | 0.981 | 3.8% / 1.9% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.3 MiB | 15.8 MiB | 0.912 | 4.7% / 2.0% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.5 MiB | 0.982 | 2.8% / 1.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.1 MiB | 12.2 MiB | 1.006 | 1.4% / 2.1% | ~ |

Peak memory (common cases): geomean B/A = 0.952; largest B/A: `mivec/sg/cc-exp/b1/d20/S7/n300` 12.1 MiB -> 12.2 MiB (1.006x); 0 MEM+, 16 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 163, 163, 163, 162, 163 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 815s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
