A/B `e2e`: A=`baseline` B=`rp-3`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 718.25ms | 404.51ms | 1.773 | [1.589, 1.979] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 680.39ms | 330.01ms | 2.046 | [1.831, 2.285] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.53ms | 30.83ms | 1.027 | [0.922, 1.143] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 705.69ms | 252.67ms | 2.788 | [2.756, 2.821] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 702.18ms | 250.16ms | 2.805 | [2.773, 2.837] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.16ms | 2.01ms | 1.122 | [0.811, 1.552] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.625s | 501.08ms | 3.253 | [3.219, 3.286] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.622s | 498.63ms | 3.263 | [3.229, 3.297] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.94ms | 1.78ms | 1.103 | [1.026, 1.186] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.307s | 935.16ms | 3.543 | [3.516, 3.570] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.303s | 932.59ms | 3.551 | [3.524, 3.577] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 4.30ms | 2.36ms | 1.329 | [0.817, 2.164] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.127s | 310.76ms | 3.659 | [3.582, 3.739] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.118s | 302.05ms | 3.728 | [3.655, 3.802] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.85ms | 5.28ms | 1.093 | [0.840, 1.421] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.365s | 522.58ms | 4.536 | [4.479, 4.593] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.340s | 497.58ms | 4.708 | [4.641, 4.775] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.19ms | 15.58ms | 1.097 | [0.965, 1.246] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.118s | 232.31ms | 4.939 | [4.558, 5.351] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.094s | 208.72ms | 5.368 | [4.940, 5.834] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 23.67ms | 14.27ms | 1.430 | [0.983, 2.082] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.525s | 2.418s | 1.458 | [1.446, 1.470] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.523s | 2.415s | 1.459 | [1.447, 1.471] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.97ms | 1.91ms | 1.029 | [0.926, 1.143] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.401s | 1.201s | 1.164 | [1.083, 1.250] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.305s | 1.117s | 1.170 | [1.070, 1.278] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 87.50ms | 79.80ms | 1.060 | [0.917, 1.224] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 179.00ms | 87.03ms | 2.060 | [2.031, 2.089] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 176.54ms | 84.20ms | 2.094 | [2.068, 2.120] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.07ms | 2.05ms | 1.003 | [0.800, 1.258] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 780.31ms | 325.57ms | 2.393 | [2.384, 2.402] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 777.21ms | 322.72ms | 2.405 | [2.396, 2.415] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.50ms | 2.38ms | 1.003 | [0.797, 1.262] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.820s | 701.67ms | 2.590 | [2.577, 2.602] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.817s | 699.07ms | 2.595 | [2.582, 2.609] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.07ms | 1.97ms | 1.046 | [0.955, 1.144] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.612s | 1.312s | 2.756 | [2.740, 2.772] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.608s | 1.309s | 2.760 | [2.746, 2.775] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.67ms | 2.50ms | 0.983 | [0.521, 1.854] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.35ms | 92.71ms | 1.143 | [1.135, 1.150] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 104.91ms | 91.41ms | 1.143 | [1.136, 1.151] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.17ms | 1.00ms | 1.123 | [0.918, 1.374] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.62ms | 201.75ms | 1.107 | [1.101, 1.112] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.80ms | 199.47ms | 1.107 | [1.103, 1.111] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.66ms | 1.28ms | 1.247 | [0.942, 1.651] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.230s | 1.135s | 1.085 | [1.081, 1.090] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.222s | 1.128s | 1.086 | [1.082, 1.090] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.60ms | 3.01ms | 1.200 | [1.181, 1.220] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.653s | 5.396s | 1.048 | [1.044, 1.052] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.644s | 5.387s | 1.048 | [1.044, 1.052] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.85ms | 3.55ms | 1.347 | [1.015, 1.789] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.972s | 1.220s | 3.255 | [3.238, 3.273] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.893s | 1.146s | 3.398 | [3.381, 3.415] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 48.58ms | 43.62ms | 1.030 | [0.882, 1.202] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.766s | 1.227s | 5.515 | [5.495, 5.536] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.651s | 1.124s | 5.918 | [5.896, 5.940] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 73.72ms | 61.68ms | 1.169 | [1.052, 1.299] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.51ms | 247.97ms | 1.088 | [1.084, 1.092] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.60ms | 244.02ms | 1.089 | [1.085, 1.093] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.72ms | 2.39ms | 1.141 | [1.042, 1.249] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.518s | 1.441s | 1.054 | [1.049, 1.059] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.507s | 1.431s | 1.054 | [1.049, 1.059] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.44ms | 4.08ms | 1.186 | [1.036, 1.359] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.965s | 6.785s | 1.023 | [1.012, 1.035] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.955s | 6.775s | 1.023 | [1.012, 1.035] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.23ms | 4.55ms | 1.099 | [0.719, 1.680] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.85ms | 175.97ms | 1.349 | [1.279, 1.422] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 234.49ms | 175.59ms | 1.349 | [1.280, 1.423] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 215.25us | 216.79us | 1.008 | [0.923, 1.101] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 307.54ms | 245.93ms | 1.258 | [1.227, 1.291] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.37ms | 245.79ms | 1.259 | [1.227, 1.291] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 81.71us | 72.56us | 1.122 | [1.022, 1.231] | faster |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.744s | 1.449s | 1.204 | [1.197, 1.211] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.743s | 1.449s | 1.204 | [1.197, 1.211] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 127.74us | 96.59us | 1.231 | [0.880, 1.723] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.471s | 1.261s | 1.170 | [1.155, 1.185] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.471s | 1.261s | 1.170 | [1.155, 1.185] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 79.55us | 65.79us | 1.204 | [1.141, 1.270] | faster |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.231s | 1.990s | 1.124 | [1.116, 1.133] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.231s | 1.990s | 1.124 | [1.116, 1.132] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 40.16us | 32.90us | 1.764 | [0.596, 5.221] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.31ms | 181.81ms | 1.032 | [1.026, 1.037] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.22ms | 181.73ms | 1.032 | [1.026, 1.037] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.01us | 27.59us | 1.216 | [0.850, 1.740] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 366.30ms | 342.73ms | 1.069 | [1.055, 1.083] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 366.18ms | 342.62ms | 1.069 | [1.055, 1.083] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.60us | 34.36us | 1.181 | [0.917, 1.520] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.24ms | 51.60ms | 1.018 | [0.971, 1.067] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.16ms | 51.53ms | 1.018 | [0.971, 1.067] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.65us | 21.02us | 1.181 | [0.943, 1.479] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 113.09ms | 108.28ms | 1.042 | [1.003, 1.082] | faster |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 106.84ms | 102.26ms | 1.042 | [1.000, 1.086] | faster |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.75ms | 1.66ms | 1.072 | [0.986, 1.166] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 500.28ms | 443.57ms | 1.146 | [1.104, 1.189] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 497.22ms | 438.92ms | 1.147 | [1.106, 1.190] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.21ms | 2.19ms | 0.925 | [0.682, 1.255] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.708s | 1.527s | 1.122 | [1.083, 1.161] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.705s | 1.525s | 1.122 | [1.083, 1.161] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.35ms | 2.08ms | 1.155 | [0.731, 1.827] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.695s | 2.468s | 1.091 | [1.069, 1.114] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.692s | 2.465s | 1.091 | [1.069, 1.113] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.91ms | 1.76ms | 1.201 | [0.832, 1.732] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.271s | 742.62ms | 3.052 | [2.894, 3.220] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.266s | 738.82ms | 3.060 | [2.901, 3.229] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.62ms | 3.01ms | 1.534 | [1.485, 1.583] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.249s | 483.62ms | 6.744 | [6.381, 7.127] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.868s | 475.14ms | 3.986 | [3.695, 4.300] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.379s | 7.55ms | 182.533 | [178.531, 186.624] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.888s | 27.53ms | 66.674 | [61.123, 72.729] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 100.63ms | 24.80ms | 4.206 | [3.766, 4.697] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.789s | 2.45ms | 570.523 | [285.379, 1140.577] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.914s | 3.08ms | 595.221 | [550.544, 643.523] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 10.41ms | 2.06ms | 5.119 | [4.727, 5.543] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.904s | 796.74us | 2058.010 | [1572.852, 2692.820] | faster |

Sum of medians (total, common cases): A = 69.922s, B = 38.451s, saved = 31.471s (1.818x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-sin/b0/d7/L7`: A 7.97e-12 -> B 1.42e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 364.1 MiB | 619.7 MiB | 1.702 | 0.2% / 0.1% | MEM+ |
| quad/sg/equi-exp/b0/d5/L10 | 15.2 MiB | 15.7 MiB | 1.033 | 3.2% / 6.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.5 MiB | 14.6 MiB | 1.009 | 3.6% / 3.2% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.0 MiB | 12.9 MiB | 0.988 | 2.8% / 2.2% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.6 MiB | 25.7 MiB | 1.002 | 2.5% / 3.1% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 54.0 MiB | 53.1 MiB | 0.982 | 4.0% / 5.6% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 60.2 MiB | 53.9 MiB | 0.894 | 13.5% / 14.4% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.3 MiB | 14.5 MiB | 1.012 | 4.9% / 5.3% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 804.7 MiB | 1063.6 MiB | 1.322 | 8.4% / 3.7% | MEM+ |
| quad/sg/cc-uq/b0/d3/L14 | 37.6 MiB | 41.0 MiB | 1.088 | 7.5% / 9.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 17.3 MiB | 1.008 | 1.9% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 14.7 MiB | 0.988 | 2.3% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.005 | 2.3% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.4 MiB | 62.3 MiB | 0.998 | 3.7% / 0.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.2 MiB | 26.0 MiB | 1.075 | 5.3% / 2.4% | MEM+ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.2 MiB | 42.0 MiB | 1.129 | 4.4% / 3.4% | MEM+ |
| quad/dsg/equi-exp/b0/d10/L7 | 36.7 MiB | 41.6 MiB | 1.135 | 2.7% / 2.7% | MEM+ |
| quad/dsg/equi-exp/b1/d15/L7 | 234.0 MiB | 150.2 MiB | 0.642 | 2.0% / 2.7% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 330.9 MiB | 217.5 MiB | 0.657 | 2.3% / 0.3% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 24.4 MiB | 25.5 MiB | 1.046 | 2.0% / 4.4% | MEM+ |
| quad/dsg/cc-uq/b0/d7/L10 | 37.3 MiB | 42.3 MiB | 1.134 | 1.6% / 5.0% | MEM+ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.2 MiB | 42.3 MiB | 1.135 | 2.3% / 4.7% | MEM+ |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.9 MiB | 1.006 | 1.9% / 3.7% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.0 MiB | 0.997 | 1.1% / 1.1% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.0 MiB | 0.990 | 2.3% / 1.1% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.8 MiB | 13.0 MiB | 1.020 | 4.1% / 6.0% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.5 MiB | 24.7 MiB | 1.009 | 7.0% / 3.0% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 11.8 MiB | 0.998 | 1.7% / 2.8% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.0 MiB | 1.000 | 2.5% / 3.3% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.0 MiB | 1.008 | 1.1% / 2.4% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.6 MiB | 19.4 MiB | 0.989 | 2.0% / 2.6% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.0 MiB | 1.005 | 5.4% / 3.4% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.8 MiB | 1.006 | 4.1% / 3.8% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.4 MiB | 13.1 MiB | 0.980 | 4.6% / 3.0% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.0 MiB | 16.2 MiB | 1.016 | 2.9% / 2.0% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.7 MiB | 17.1 MiB | 0.969 | 2.7% / 2.7% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.8 MiB | 1.004 | 3.4% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.3 MiB | 12.2 MiB | 0.989 | 1.1% / 2.0% | ~ |

Peak memory (common cases): geomean B/A = 1.014; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 364.1 MiB -> 619.7 MiB (1.702x); 8 MEM+, 3 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 225, 226, 225, 225, 225 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1127s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
