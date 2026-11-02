A/B `e2e`: A=`WP-28-checkpoint` B=`WP-28-refactor`, 16 thread(s), 3 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 403.85ms | 417.92ms | 0.972 | [0.781, 1.210] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 327.58ms | 346.26ms | 0.963 | [0.771, 1.202] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.04ms | 32.07ms | 0.985 | [0.795, 1.221] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 249.01ms | 247.86ms | 1.018 | [0.954, 1.087] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.41ms | 245.39ms | 1.001 | [0.978, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.97ms | 1.96ms | 2.047 | [0.090, 46.658] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.44ms | 499.31ms | 0.997 | [0.977, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 493.99ms | 496.83ms | 0.997 | [0.977, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.82ms | 1.85ms | 0.976 | [0.871, 1.093] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 914.29ms | 915.44ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 912.30ms | 911.03ms | 1.001 | [0.985, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 1.98ms | 2.22ms | 0.727 | [0.230, 2.301] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 301.67ms | 304.60ms | 0.991 | [0.960, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 293.28ms | 296.24ms | 0.993 | [0.966, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.63ms | 5.21ms | 0.879 | [0.601, 1.286] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 519.03ms | 526.78ms | 0.987 | [0.962, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.30ms | 498.23ms | 0.990 | [0.962, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.19ms | 15.64ms | 0.938 | [0.735, 1.196] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 235.89ms | 235.11ms | 0.994 | [0.943, 1.048] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.42ms | 207.13ms | 0.998 | [0.981, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 19.83ms | 19.33ms | 0.894 | [0.489, 1.633] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.399s | 2.404s | 0.998 | [0.982, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.396s | 2.401s | 0.998 | [0.982, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.91ms | 1.81ms | 1.038 | [0.841, 1.281] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.197s | 1.209s | 1.002 | [0.827, 1.215] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.126s | 1.119s | 1.012 | [0.826, 1.241] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 72.40ms | 82.89ms | 0.883 | [0.661, 1.179] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.73ms | 87.31ms | 0.986 | [0.959, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.49ms | 84.46ms | 0.992 | [0.968, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.24ms | 2.27ms | 0.955 | [0.611, 1.492] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 322.76ms | 320.46ms | 1.002 | [0.985, 1.019] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.86ms | 317.65ms | 1.002 | [0.986, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.33ms | 2.36ms | 1.003 | [0.721, 1.395] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 692.19ms | 693.74ms | 0.997 | [0.981, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 689.57ms | 691.09ms | 0.998 | [0.981, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.95ms | 1.96ms | 0.969 | [0.833, 1.127] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.301s | 1.305s | 0.995 | [0.983, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.299s | 1.301s | 0.996 | [0.983, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.03ms | 3.88ms | 0.631 | [0.180, 2.217] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 91.76ms | 92.63ms | 0.996 | [0.963, 1.030] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.17ms | 91.29ms | 0.995 | [0.960, 1.031] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.11ms | 1.04ms | 1.101 | [0.735, 1.648] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.47ms | 202.10ms | 1.003 | [0.993, 1.013] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.90ms | 199.36ms | 1.004 | [0.993, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.25ms | 1.18ms | 1.077 | [0.647, 1.794] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.138s | 1.135s | 1.007 | [0.990, 1.025] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.131s | 1.127s | 1.007 | [0.990, 1.024] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.13ms | 3.12ms | 1.016 | [0.958, 1.079] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.408s | 5.412s | 1.001 | [0.992, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.400s | 5.404s | 1.001 | [0.992, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.29ms | 3.20ms | 0.994 | [0.564, 1.753] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.224s | 1.221s | 1.004 | [0.992, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.151s | 1.147s | 1.003 | [0.987, 1.018] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.68ms | 43.18ms | 1.032 | [0.758, 1.406] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.224s | 1.229s | 0.997 | [0.989, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.124s | 1.000 | [0.991, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.56ms | 61.89ms | 0.961 | [0.768, 1.201] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.44ms | 248.96ms | 0.998 | [0.991, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.81ms | 245.17ms | 0.999 | [0.991, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.25ms | 2.27ms | 0.972 | [0.794, 1.191] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.441s | 1.442s | 0.997 | [0.990, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.431s | 1.432s | 0.999 | [0.991, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.02ms | 5.08ms | 0.838 | [0.364, 1.929] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.783s | 6.781s | 0.999 | [0.988, 1.011] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.773s | 6.772s | 0.999 | [0.990, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.56ms | 3.98ms | 1.053 | [0.451, 2.462] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 174.45ms | 174.89ms | 1.008 | [0.948, 1.073] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 174.09ms | 174.48ms | 1.012 | [0.951, 1.076] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 208.06us | 208.06us | 0.496 | [0.025, 9.986] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 242.22ms | 245.42ms | 0.978 | [0.889, 1.075] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 242.08ms | 245.27ms | 0.978 | [0.889, 1.075] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.47us | 74.03us | 1.002 | [0.864, 1.162] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.443s | 1.447s | 0.997 | [0.987, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.443s | 1.447s | 0.997 | [0.987, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 113.91us | 102.60us | 1.074 | [0.682, 1.693] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.255s | 1.259s | 0.997 | [0.972, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.255s | 1.259s | 0.997 | [0.972, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.12us | 66.00us | 1.051 | [0.833, 1.328] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.987s | 1.982s | 1.002 | [0.992, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.987s | 1.982s | 1.002 | [0.992, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.62us | 36.18us | 0.880 | [0.463, 1.673] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.39ms | 181.58ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.30ms | 181.50ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.57us | 26.33us | 1.133 | [0.553, 2.321] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 333.28ms | 335.82ms | 0.991 | [0.965, 1.017] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 333.16ms | 335.69ms | 0.991 | [0.966, 1.017] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 36.18us | 34.29us | 1.034 | [0.375, 2.848] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.81ms | 51.07ms | 0.998 | [0.971, 1.026] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.74ms | 51.00ms | 0.998 | [0.971, 1.026] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.42us | 20.95us | 1.084 | [0.639, 1.841] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.18ms | 109.84ms | 1.047 | [0.864, 1.268] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.95ms | 103.43ms | 1.051 | [0.859, 1.285] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.68ms | 1.73ms | 0.987 | [0.835, 1.167] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 441.22ms | 444.68ms | 0.986 | [0.928, 1.047] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 438.40ms | 440.20ms | 0.988 | [0.925, 1.056] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.11ms | 3.77ms | 0.717 | [0.356, 1.444] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.522s | 1.527s | 1.005 | [0.943, 1.070] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.520s | 1.524s | 1.004 | [0.944, 1.069] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.33ms | 2.17ms | 1.175 | [0.513, 2.693] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.438s | 2.423s | 1.009 | [0.968, 1.051] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.436s | 2.421s | 1.009 | [0.968, 1.051] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.69ms | 1.77ms | 0.968 | [0.744, 1.258] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 731.35ms | 722.51ms | 0.994 | [0.861, 1.149] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 727.72ms | 718.85ms | 0.994 | [0.860, 1.150] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.02ms | 3.08ms | 0.990 | [0.930, 1.054] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 492.29ms | 486.11ms | 1.008 | [0.954, 1.064] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 483.74ms | 477.42ms | 1.008 | [0.954, 1.065] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.49ms | 7.45ms | 1.011 | [0.969, 1.055] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 27.44ms | 26.30ms | 1.037 | [0.976, 1.102] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.73ms | 23.59ms | 1.041 | [0.941, 1.152] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.44ms | 2.44ms | 1.002 | [0.984, 1.021] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.12ms | 3.14ms | 0.843 | [0.396, 1.794] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.05ms | 2.05ms | 0.964 | [0.739, 1.258] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 853.66us | 879.30us | 0.667 | [0.144, 3.097] | ~ (noise) |

Sum of medians (total, common cases): A = 38.316s, B = 38.350s, saved = -34505949.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 1.01e-12 -> B 1.51e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.0 MiB | 619.9 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.9 MiB | 15.8 MiB | 0.993 | 3.7% / 2.5% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.7 MiB | 14.5 MiB | 0.988 | 1.2% / 1.9% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 13.0 MiB | 0.993 | 1.1% / 2.7% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.8 MiB | 25.9 MiB | 1.003 | 4.3% / 4.8% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.4 MiB | 53.5 MiB | 1.002 | 1.2% / 6.4% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 59.6 MiB | 57.9 MiB | 0.971 | 12.7% / 6.3% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.8 MiB | 14.7 MiB | 0.993 | 1.6% / 3.2% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1075.2 MiB | 1082.2 MiB | 1.006 | 2.6% / 0.9% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 39.9 MiB | 40.8 MiB | 1.023 | 4.5% / 2.4% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.6 MiB | 17.8 MiB | 1.014 | 3.8% / 3.4% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 15.2 MiB | 1.015 | 0.6% / 4.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.4 MiB | 13.1 MiB | 0.980 | 0.9% / 3.0% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.0 MiB | 61.9 MiB | 0.999 | 2.1% / 0.6% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.4 MiB | 26.4 MiB | 1.001 | 0.7% / 1.3% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.1 MiB | 42.6 MiB | 1.013 | 1.6% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.3 MiB | 42.2 MiB | 0.997 | 3.2% / 2.6% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.6 MiB | 151.5 MiB | 1.000 | 1.9% / 0.9% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.8 MiB | 217.7 MiB | 1.000 | 0.8% / 0.4% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.5 MiB | 25.3 MiB | 0.994 | 2.1% / 3.8% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.0 MiB | 42.1 MiB | 1.002 | 3.9% / 3.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.2 MiB | 43.4 MiB | 1.027 | 1.2% / 1.2% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.2 MiB | 15.2 MiB | 1.001 | 0.5% / 4.0% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.2 MiB | 0.994 | 1.3% / 1.2% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.2 MiB | 1.013 | 4.2% / 1.2% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.2 MiB | 13.3 MiB | 1.004 | 3.4% / 2.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 25.3 MiB | 24.9 MiB | 0.987 | 6.3% / 6.5% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.1 MiB | 0.991 | 1.0% / 2.4% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.1 MiB | 0.999 | 0.2% / 0.8% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.1 MiB | 12.2 MiB | 1.008 | 1.1% / 0.8% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.9 MiB | 19.7 MiB | 0.987 | 0.3% / 1.6% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 13.2 MiB | 1.005 | 1.6% / 1.0% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 13.1 MiB | 1.008 | 1.2% / 1.2% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.7 MiB | 13.4 MiB | 0.974 | 2.9% / 4.8% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.5 MiB | 16.3 MiB | 0.991 | 0.6% / 2.3% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.5 MiB | 17.5 MiB | 1.000 | 3.7% / 1.9% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.0 MiB | 13.0 MiB | 1.000 | 0.9% / 1.9% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.4 MiB | 0.992 | 1.3% / 0.2% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `quad/dsg/cc-uq/b0/d10/L7` 42.2 MiB -> 43.4 MiB (1.027x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%
Round durations (s): 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 486s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
