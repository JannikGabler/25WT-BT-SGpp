A/B `e2e`: A=`baseline` B=`rp-1`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 737.28ms | 402.57ms | 1.822 | [1.633, 2.033] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 698.61ms | 324.79ms | 2.137 | [1.913, 2.388] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.22ms | 31.41ms | 1.027 | [0.922, 1.143] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 705.73ms | 252.54ms | 2.806 | [2.770, 2.843] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 703.08ms | 249.97ms | 2.824 | [2.788, 2.860] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.11ms | 2.07ms | 1.027 | [0.900, 1.172] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.637s | 501.77ms | 3.260 | [3.227, 3.294] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.634s | 499.28ms | 3.271 | [3.237, 3.305] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.94ms | 1.77ms | 1.081 | [0.902, 1.294] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.305s | 936.54ms | 3.519 | [3.461, 3.578] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.303s | 932.61ms | 3.530 | [3.477, 3.585] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.20ms | 4.21ms | 0.681 | [0.389, 1.193] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.110s | 308.72ms | 3.638 | [3.577, 3.701] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.101s | 300.44ms | 3.705 | [3.646, 3.765] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.69ms | 4.61ms | 1.166 | [0.949, 1.432] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.381s | 525.51ms | 4.546 | [4.487, 4.605] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.355s | 497.53ms | 4.734 | [4.668, 4.802] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.48ms | 17.51ms | 1.020 | [0.797, 1.305] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.114s | 235.62ms | 4.718 | [4.658, 4.779] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.081s | 208.55ms | 5.182 | [5.154, 5.210] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 21.62ms | 18.33ms | 1.177 | [0.871, 1.590] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.524s | 2.410s | 1.460 | [1.448, 1.472] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.521s | 2.407s | 1.461 | [1.449, 1.472] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.93ms | 1.77ms | 1.102 | [0.992, 1.224] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.430s | 1.189s | 1.209 | [1.143, 1.278] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.340s | 1.101s | 1.225 | [1.165, 1.287] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 78.56ms | 82.37ms | 0.966 | [0.831, 1.122] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 180.63ms | 87.37ms | 2.064 | [2.034, 2.095] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 178.45ms | 84.68ms | 2.102 | [2.076, 2.129] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 2.12ms | 0.997 | [0.900, 1.103] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 778.99ms | 326.04ms | 2.385 | [2.368, 2.402] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 775.84ms | 323.53ms | 2.395 | [2.378, 2.413] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.48ms | 2.14ms | 1.166 | [1.029, 1.321] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.826s | 703.69ms | 2.599 | [2.504, 2.698] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.821s | 701.04ms | 2.604 | [2.508, 2.704] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.09ms | 1.92ms | 1.247 | [0.809, 1.923] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.616s | 1.311s | 2.758 | [2.741, 2.775] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.613s | 1.308s | 2.762 | [2.747, 2.778] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.58ms | 3.76ms | 0.899 | [0.450, 1.794] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.73ms | 95.56ms | 1.110 | [1.096, 1.126] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.41ms | 94.13ms | 1.112 | [1.096, 1.128] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.04ms | 1.03ms | 1.007 | [0.823, 1.232] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.53ms | 213.58ms | 1.048 | [1.043, 1.053] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.81ms | 210.70ms | 1.047 | [1.043, 1.051] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.40ms | 1.33ms | 1.102 | [0.712, 1.706] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.232s | 1.216s | 1.020 | [0.999, 1.042] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.224s | 1.209s | 1.020 | [1.001, 1.038] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.64ms | 3.04ms | 1.202 | [0.539, 2.681] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.665s | 5.657s | 1.001 | [0.998, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.656s | 5.649s | 1.001 | [0.998, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.96ms | 4.17ms | 1.045 | [0.691, 1.582] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.966s | 4.091s | 0.968 | [0.964, 0.971] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.892s | 4.032s | 0.965 | [0.961, 0.969] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 45.01ms | 35.49ms | 1.211 | [1.037, 1.413] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.766s | 7.035s | 0.962 | [0.958, 0.965] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.656s | 6.943s | 0.958 | [0.955, 0.962] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 73.35ms | 50.82ms | 1.393 | [1.250, 1.552] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.42ms | 258.26ms | 1.042 | [1.037, 1.047] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.68ms | 254.58ms | 1.042 | [1.037, 1.047] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.54ms | 2.42ms | 1.041 | [0.896, 1.209] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.522s | 1.512s | 1.010 | [0.995, 1.026] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.511s | 1.502s | 1.010 | [0.995, 1.025] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.45ms | 4.39ms | 1.213 | [0.996, 1.478] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.971s | 7.020s | 0.993 | [0.987, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.960s | 7.011s | 0.993 | [0.988, 0.997] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.94ms | 3.60ms | 1.207 | [0.790, 1.845] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.87ms | 176.39ms | 1.316 | [1.186, 1.460] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 234.51ms | 176.00ms | 1.317 | [1.186, 1.461] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 213.71us | 206.94us | 1.014 | [0.929, 1.107] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 310.49ms | 246.46ms | 1.293 | [1.179, 1.417] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 310.34ms | 246.30ms | 1.289 | [1.185, 1.401] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 79.06us | 75.92us | 2.347 | [0.210, 26.223] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.743s | 1.448s | 1.205 | [1.199, 1.211] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.743s | 1.448s | 1.205 | [1.199, 1.211] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 129.83us | 102.53us | 1.241 | [0.990, 1.557] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.475s | 1.259s | 1.171 | [1.156, 1.186] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.475s | 1.259s | 1.171 | [1.156, 1.186] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 80.53us | 63.90us | 1.275 | [1.182, 1.375] | faster |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.241s | 1.994s | 1.122 | [1.107, 1.136] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.241s | 1.994s | 1.122 | [1.107, 1.136] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 39.39us | 32.90us | 1.188 | [1.120, 1.260] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.82ms | 181.47ms | 1.034 | [1.023, 1.045] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.73ms | 181.38ms | 1.034 | [1.023, 1.045] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.99us | 28.63us | 1.086 | [0.700, 1.683] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 368.74ms | 338.16ms | 1.089 | [1.074, 1.103] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 368.61ms | 338.05ms | 1.089 | [1.074, 1.103] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 40.16us | 33.17us | 1.197 | [0.930, 1.541] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.32ms | 51.08ms | 1.042 | [1.025, 1.060] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.25ms | 51.01ms | 1.042 | [1.025, 1.060] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 23.40us | 20.11us | 1.198 | [0.917, 1.565] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 113.34ms | 110.05ms | 1.034 | [0.996, 1.074] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 107.26ms | 103.90ms | 1.036 | [0.994, 1.080] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.72ms | 1.72ms | 1.001 | [0.920, 1.088] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 503.43ms | 442.48ms | 1.112 | [1.052, 1.176] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 498.46ms | 439.46ms | 1.113 | [1.053, 1.176] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.14ms | 2.28ms | 1.022 | [0.617, 1.692] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.688s | 1.497s | 1.120 | [1.088, 1.154] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.685s | 1.495s | 1.120 | [1.087, 1.154] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.41ms | 2.02ms | 1.245 | [0.823, 1.883] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.701s | 2.468s | 1.073 | [1.013, 1.137] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.699s | 2.465s | 1.073 | [1.013, 1.137] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.85ms | 1.77ms | 1.060 | [0.981, 1.145] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.221s | 756.54ms | 2.917 | [2.765, 3.077] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.216s | 750.62ms | 2.932 | [2.779, 3.093] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.64ms | 5.06ms | 0.924 | [0.911, 0.936] | SLOWER |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.237s | 1.988s | 1.633 | [1.605, 1.662] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.858s | 476.06ms | 3.864 | [3.694, 4.043] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.374s | 1.505s | 0.913 | [0.902, 0.924] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.894s | 2.102s | 0.902 | [0.894, 0.910] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 103.68ms | 23.83ms | 4.318 | [3.977, 4.688] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.790s | 2.077s | 0.862 | [0.856, 0.868] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.914s | 2.068s | 0.924 | [0.920, 0.927] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 10.19ms | 2.07ms | 4.927 | [4.549, 5.335] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.903s | 2.065s | 0.920 | [0.916, 0.923] | SLOWER |

Sum of medians (total, common cases): A = 69.953s, B = 53.414s, saved = 16.539s (1.310x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 3.50e-13 -> B 8.87e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 363.7 MiB | 619.9 MiB | 1.704 | 0.2% / 0.1% | MEM+ |
| quad/sg/equi-exp/b0/d5/L10 | 15.3 MiB | 15.9 MiB | 1.037 | 2.4% / 4.4% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 14.6 MiB | 1.004 | 3.7% / 2.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.8 MiB | 1.004 | 5.0% / 3.5% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 26.1 MiB | 1.009 | 3.5% / 3.2% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.3 MiB | 54.8 MiB | 1.027 | 4.0% / 8.7% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 59.8 MiB | 57.6 MiB | 0.964 | 2.5% / 5.1% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.5 MiB | 14.8 MiB | 1.014 | 4.7% / 3.6% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 786.2 MiB | 1079.1 MiB | 1.373 | 15.9% / 1.9% | MEM+ |
| quad/sg/cc-uq/b0/d3/L14 | 37.8 MiB | 40.5 MiB | 1.072 | 4.6% / 3.7% | MEM+ |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 17.4 MiB | 1.012 | 4.1% / 3.5% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 14.8 MiB | 0.992 | 1.8% / 5.7% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.7 MiB | 13.0 MiB | 1.020 | 1.5% / 3.4% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.8 MiB | 62.7 MiB | 0.999 | 1.4% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.2 MiB | 24.3 MiB | 1.002 | 6.6% / 5.7% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.3 MiB | 37.5 MiB | 1.004 | 3.3% / 3.5% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 37.4 MiB | 37.2 MiB | 0.996 | 4.8% / 4.0% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 232.4 MiB | 233.4 MiB | 1.004 | 2.2% / 3.5% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 331.3 MiB | 328.8 MiB | 0.992 | 1.1% / 1.3% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 23.8 MiB | 24.0 MiB | 1.009 | 4.4% / 3.3% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 37.8 MiB | 37.5 MiB | 0.994 | 4.6% / 3.9% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.2 MiB | 37.0 MiB | 0.993 | 2.5% / 4.5% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 14.9 MiB | 1.000 | 3.5% / 1.8% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.0 MiB | 1.003 | 1.6% / 1.0% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.1 MiB | 1.008 | 2.5% / 0.7% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 13.0 MiB | 1.008 | 7.1% / 2.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.2 MiB | 24.5 MiB | 1.012 | 6.4% / 5.3% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.8 MiB | 12.0 MiB | 1.009 | 3.3% / 2.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.1 MiB | 12.1 MiB | 0.999 | 1.9% / 2.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.8 MiB | 11.9 MiB | 1.007 | 4.1% / 2.4% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.6 MiB | 19.7 MiB | 1.005 | 2.6% / 2.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 12.9 MiB | 0.969 | 3.6% / 4.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.9 MiB | 1.004 | 2.9% / 2.0% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.3 MiB | 13.2 MiB | 0.995 | 1.6% / 1.7% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.0 MiB | 16.1 MiB | 1.006 | 2.5% / 2.4% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.3 MiB | 17.7 MiB | 1.023 | 4.9% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.7 MiB | 12.8 MiB | 1.009 | 1.4% / 2.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.3 MiB | 12.3 MiB | 1.004 | 0.8% / 2.2% | ~ |

Peak memory (common cases): geomean B/A = 1.028; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 363.7 MiB -> 619.9 MiB (1.704x); 3 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 255, 255, 255, 255, 255 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1276s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
