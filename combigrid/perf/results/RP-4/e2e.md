A/B `e2e`: A=`baseline` B=`rp-4`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 719.94ms | 439.59ms | 1.648 | [1.477, 1.839] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 680.86ms | 364.44ms | 1.885 | [1.688, 2.106] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.74ms | 32.56ms | 0.985 | [0.885, 1.097] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 704.27ms | 264.84ms | 2.660 | [2.630, 2.692] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 701.75ms | 262.20ms | 2.676 | [2.646, 2.707] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.14ms | 2.10ms | 1.044 | [0.853, 1.278] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.623s | 515.01ms | 3.156 | [3.123, 3.188] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.620s | 512.67ms | 3.166 | [3.133, 3.199] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.92ms | 1.78ms | 1.085 | [0.941, 1.250] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.304s | 910.21ms | 3.632 | [3.603, 3.662] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.302s | 906.38ms | 3.640 | [3.612, 3.668] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.38ms | 2.26ms | 1.023 | [0.626, 1.671] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.139s | 302.87ms | 3.754 | [3.664, 3.846] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.130s | 294.62ms | 3.828 | [3.738, 3.920] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.79ms | 5.18ms | 1.131 | [0.919, 1.391] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.367s | 519.11ms | 4.528 | [4.442, 4.616] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.340s | 495.47ms | 4.701 | [4.633, 4.771] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.19ms | 15.68ms | 1.046 | [0.862, 1.270] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.115s | 228.85ms | 4.847 | [4.710, 4.988] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.087s | 202.23ms | 5.344 | [5.276, 5.413] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 22.27ms | 18.22ms | 1.155 | [0.855, 1.561] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.524s | 2.404s | 1.468 | [1.450, 1.486] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.521s | 2.401s | 1.468 | [1.450, 1.486] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.99ms | 1.90ms | 1.070 | [0.963, 1.189] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.391s | 1.235s | 1.128 | [1.097, 1.161] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.291s | 1.139s | 1.133 | [1.082, 1.187] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 82.17ms | 84.87ms | 0.978 | [0.798, 1.199] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 181.06ms | 87.29ms | 2.169 | [1.916, 2.455] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 178.69ms | 84.45ms | 2.209 | [1.947, 2.506] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.08ms | 2.07ms | 0.987 | [0.898, 1.084] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 780.52ms | 322.96ms | 2.406 | [2.387, 2.425] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 776.19ms | 320.18ms | 2.415 | [2.399, 2.432] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.50ms | 2.28ms | 1.236 | [0.849, 1.799] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.814s | 694.83ms | 2.614 | [2.601, 2.627] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.811s | 692.27ms | 2.620 | [2.606, 2.633] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.16ms | 2.00ms | 1.096 | [0.986, 1.218] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.619s | 1.317s | 2.746 | [2.730, 2.762] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.616s | 1.315s | 2.748 | [2.734, 2.763] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.59ms | 2.21ms | 1.184 | [0.891, 1.574] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 107.08ms | 96.06ms | 1.115 | [1.104, 1.126] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.76ms | 94.65ms | 1.115 | [1.103, 1.127] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.17ms | 1.00ms | 1.143 | [0.934, 1.398] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.91ms | 212.01ms | 1.057 | [1.049, 1.064] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.93ms | 209.11ms | 1.057 | [1.051, 1.064] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.55ms | 1.23ms | 1.097 | [0.850, 1.415] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.231s | 1.172s | 1.051 | [1.047, 1.055] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.222s | 1.165s | 1.050 | [1.046, 1.054] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.63ms | 3.02ms | 1.314 | [1.094, 1.578] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.685s | 5.368s | 1.056 | [1.047, 1.065] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.675s | 5.359s | 1.056 | [1.046, 1.065] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.86ms | 3.17ms | 1.446 | [1.089, 1.920] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.957s | 1.225s | 3.236 | [3.212, 3.259] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.890s | 1.151s | 3.375 | [3.355, 3.395] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.51ms | 43.39ms | 0.986 | [0.845, 1.151] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.759s | 1.214s | 5.571 | [5.551, 5.592] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.658s | 1.112s | 5.989 | [5.948, 6.031] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.08ms | 61.23ms | 1.133 | [1.004, 1.278] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.92ms | 249.77ms | 1.073 | [1.047, 1.099] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.80ms | 245.92ms | 1.074 | [1.051, 1.098] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.63ms | 2.40ms | 1.067 | [0.851, 1.337] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.522s | 1.441s | 1.053 | [1.047, 1.058] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.512s | 1.432s | 1.053 | [1.048, 1.058] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.00ms | 4.07ms | 1.180 | [1.030, 1.351] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.964s | 6.848s | 1.018 | [1.012, 1.024] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.955s | 6.838s | 1.018 | [1.013, 1.023] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.27ms | 4.57ms | 1.138 | [0.744, 1.739] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 238.50ms | 129.02ms | 1.855 | [1.806, 1.906] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 238.11ms | 128.76ms | 1.856 | [1.807, 1.907] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 211.20us | 213.85us | 0.993 | [0.909, 1.084] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 307.83ms | 186.55ms | 1.652 | [1.618, 1.686] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.67ms | 186.40ms | 1.652 | [1.619, 1.686] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 79.90us | 74.45us | 1.038 | [0.943, 1.141] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.738s | 1.123s | 1.548 | [1.540, 1.556] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.738s | 1.123s | 1.548 | [1.540, 1.556] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 129.62us | 130.74us | 0.989 | [0.767, 1.274] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.470s | 1.007s | 1.462 | [1.443, 1.481] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.470s | 1.007s | 1.462 | [1.443, 1.481] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 80.67us | 98.69us | 0.828 | [0.611, 1.122] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.235s | 1.627s | 1.376 | [1.364, 1.387] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.235s | 1.626s | 1.376 | [1.364, 1.387] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 41.00us | 56.92us | 1.646 | [0.433, 6.267] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 186.83ms | 34.36ms | 5.433 | [5.405, 5.462] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 186.73ms | 34.29ms | 5.443 | [5.414, 5.471] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 30.52us | 26.75us | 1.171 | [0.818, 1.675] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 367.02ms | 117.02ms | 3.081 | [2.896, 3.278] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 366.90ms | 116.91ms | 3.083 | [2.898, 3.280] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 41.07us | 42.32us | 0.928 | [0.652, 1.319] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.29ms | 11.98ms | 4.465 | [4.381, 4.550] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.22ms | 11.92ms | 4.483 | [4.399, 4.569] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.86us | 19.49us | 1.186 | [0.953, 1.477] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 112.51ms | 111.13ms | 0.986 | [0.925, 1.052] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 106.38ms | 104.95ms | 0.986 | [0.922, 1.055] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.72ms | 1.65ms | 1.046 | [0.956, 1.146] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 502.44ms | 354.44ms | 1.419 | [1.378, 1.461] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 499.42ms | 351.36ms | 1.421 | [1.379, 1.465] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.28ms | 2.14ms | 1.138 | [0.880, 1.472] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.671s | 1.200s | 1.407 | [1.366, 1.450] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.668s | 1.196s | 1.409 | [1.369, 1.451] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.18ms | 2.33ms | 0.750 | [0.457, 1.230] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.687s | 1.850s | 1.454 | [1.424, 1.484] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.684s | 1.848s | 1.454 | [1.425, 1.484] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.86ms | 1.77ms | 1.059 | [0.980, 1.144] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.210s | 824.70ms | 2.718 | [2.577, 2.867] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.204s | 820.93ms | 2.723 | [2.581, 2.873] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.66ms | 3.00ms | 1.650 | [1.379, 1.976] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.324s | 482.24ms | 6.899 | [6.714, 7.089] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.949s | 473.64ms | 4.137 | [3.982, 4.299] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.373s | 7.51ms | 182.810 | [179.833, 185.837] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.885s | 27.38ms | 69.717 | [67.244, 72.282] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 100.97ms | 24.68ms | 4.161 | [3.956, 4.377] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.784s | 2.43ms | 735.036 | [730.504, 739.595] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.908s | 3.09ms | 613.280 | [595.491, 631.601] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 10.41ms | 2.04ms | 5.143 | [4.749, 5.570] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.897s | 812.25us | 2261.234 | [2077.792, 2460.872] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | total | (not in A) | 52.27ms | - | - | new case |
| interp/dsg/cc/b0/d3/L9/p64 | total | (not in A) | 42.93ms | - | - | new case |
| interp/dsg/cc/b0/d5/L6/p64 | total | (not in A) | 170.91ms | - | - | new case |
| interp/dsg/cc/b0/d7/L4/p16 | total | (not in A) | 149.15ms | - | - | new case |
| interp/dsg/cc/b0/d10/L2/p8 | total | (not in A) | 393.44ms | - | - | new case |
| interp/dsg/equi/b0/d3/L5/p64 | total | (not in A) | 6.50ms | - | - | new case |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | (not in A) | 183.88ms | - | - | new case |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | (not in A) | 186.49ms | - | - | new case |
| interplin/dsg/equi/b1/d8/L5/p256 | total | (not in A) | 392.14ms | - | - | new case |
| interplin/dsg/equi/b1/d10/L5/p32 | total | (not in A) | 434.77ms | - | - | new case |

Sum of medians (total, common cases): A = 69.894s, B = 36.157s, saved = 33.737s (1.933x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 7.91e-16 -> B 5.65e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 363.8 MiB | 620.0 MiB | 1.704 | 0.2% / 0.0% | MEM+ |
| quad/sg/equi-exp/b0/d5/L10 | 15.2 MiB | 15.9 MiB | 1.047 | 3.4% / 3.1% | MEM+ |
| quad/sg/equi-exp/b0/d7/L7 | 14.4 MiB | 14.7 MiB | 1.016 | 2.8% / 2.4% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 13.1 MiB | 1.030 | 5.6% / 3.0% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 26.2 MiB | 1.013 | 4.2% / 3.6% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 54.3 MiB | 54.1 MiB | 0.996 | 6.2% / 7.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 60.2 MiB | 57.5 MiB | 0.954 | 13.9% / 12.0% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.5 MiB | 14.8 MiB | 1.022 | 2.9% / 1.6% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 799.1 MiB | 1078.7 MiB | 1.350 | 10.5% / 3.2% | MEM+ |
| quad/sg/cc-uq/b0/d3/L14 | 38.2 MiB | 40.9 MiB | 1.073 | 4.7% / 4.1% | MEM+ |
| quad/sg/cc-uq/b0/d5/L10 | 17.0 MiB | 17.5 MiB | 1.028 | 3.2% / 3.0% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.0 MiB | 15.1 MiB | 1.008 | 2.6% / 4.0% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.3 MiB | 1.029 | 1.0% / 4.6% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.3 MiB | 62.6 MiB | 1.005 | 1.2% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.4 MiB | 26.3 MiB | 1.080 | 6.6% / 3.7% | MEM+ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.2 MiB | 42.8 MiB | 1.151 | 5.2% / 2.9% | MEM+ |
| quad/dsg/equi-exp/b0/d10/L7 | 37.2 MiB | 42.0 MiB | 1.128 | 6.6% / 3.5% | MEM+ |
| quad/dsg/equi-exp/b1/d15/L7 | 231.4 MiB | 150.6 MiB | 0.651 | 1.8% / 0.9% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 331.1 MiB | 217.6 MiB | 0.657 | 2.5% / 0.8% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 24.2 MiB | 26.0 MiB | 1.071 | 1.7% / 3.7% | MEM+ |
| quad/dsg/cc-uq/b0/d7/L10 | 36.9 MiB | 42.6 MiB | 1.154 | 2.9% / 5.8% | MEM+ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.0 MiB | 43.1 MiB | 1.163 | 2.5% / 5.8% | MEM+ |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 16.1 MiB | 1.086 | 1.5% / 1.6% | MEM+ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.3 MiB | 1.018 | 0.8% / 3.6% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.3 MiB | 1.018 | 3.8% / 1.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.0 MiB | 12.2 MiB | 0.941 | 4.2% / 2.2% | MEM- |
| interp/sg/cc/b0/d10/L2/p8 | 23.7 MiB | 12.2 MiB | 0.514 | 5.4% / 1.7% | MEM- |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 12.2 MiB | 1.023 | 1.4% / 1.6% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.2 MiB | 1.019 | 2.0% / 2.6% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.8 MiB | 12.1 MiB | 1.027 | 2.5% / 2.3% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 19.7 MiB | 1.006 | 1.5% / 0.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.4 MiB | 1.016 | 3.3% / 5.0% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 13.2 MiB | 1.033 | 3.2% / 3.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.4 MiB | 13.5 MiB | 1.009 | 5.8% / 3.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.0 MiB | 16.6 MiB | 1.036 | 2.2% / 2.6% | MEM+ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.4 MiB | 17.5 MiB | 1.005 | 4.9% / 3.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.7 MiB | 13.1 MiB | 1.031 | 3.4% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.1 MiB | 12.5 MiB | 1.025 | 4.0% / 2.0% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | (not in A) | 16.2 MiB | - | - | new case |
| interp/dsg/cc/b0/d3/L9/p64 | (not in A) | 12.6 MiB | - | - | new case |
| interp/dsg/cc/b0/d5/L6/p64 | (not in A) | 12.5 MiB | - | - | new case |
| interp/dsg/cc/b0/d7/L4/p16 | (not in A) | 12.5 MiB | - | - | new case |
| interp/dsg/cc/b0/d10/L2/p8 | (not in A) | 12.3 MiB | - | - | new case |
| interp/dsg/equi/b0/d3/L5/p64 | (not in A) | 12.2 MiB | - | - | new case |
| interplin/dsg/equi/b1/d2/L16/p16384 | (not in A) | 20.7 MiB | - | - | new case |
| interplin/dsg/equi/b1/d5/L8/p1024 | (not in A) | 13.3 MiB | - | - | new case |
| interplin/dsg/equi/b1/d8/L5/p256 | (not in A) | 13.2 MiB | - | - | new case |
| interplin/dsg/equi/b1/d10/L5/p32 | (not in A) | 17.0 MiB | - | - | new case |

Peak memory (common cases): geomean B/A = 1.014; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 363.8 MiB -> 620.0 MiB (1.704x); 12 MEM+, 4 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 226, 226, 226, 226, 226 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1130s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
