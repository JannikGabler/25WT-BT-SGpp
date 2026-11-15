A/B `e2e`: A=`baseline` B=`rp-5`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 730.71ms | 332.78ms | 2.215 | [1.985, 2.472] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 697.77ms | 296.58ms | 2.397 | [2.146, 2.678] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.54ms | 38.12ms | 0.839 | [0.754, 0.934] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | total | 704.36ms | 262.32ms | 2.718 | [2.636, 2.802] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 701.86ms | 260.46ms | 2.729 | [2.638, 2.823] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.87ms | 1.59ms | 1.028 | [0.751, 1.407] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.623s | 511.97ms | 3.147 | [3.045, 3.252] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.620s | 510.30ms | 3.152 | [3.050, 3.257] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.92ms | 1.50ms | 1.338 | [1.201, 1.492] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.305s | 918.15ms | 3.604 | [3.576, 3.633] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.302s | 915.06ms | 3.614 | [3.588, 3.640] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.21ms | 3.49ms | 0.803 | [0.421, 1.533] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.127s | 295.18ms | 3.847 | [3.742, 3.956] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.119s | 292.97ms | 3.846 | [3.741, 3.953] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.90ms | 1.52ms | 3.653 | [3.020, 4.417] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.367s | 505.43ms | 4.635 | [4.491, 4.784] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.342s | 496.25ms | 4.678 | [4.558, 4.801] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.43ms | 6.55ms | 2.324 | [1.645, 3.283] | faster |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.111s | 212.81ms | 5.199 | [5.106, 5.293] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.081s | 201.07ms | 5.351 | [5.262, 5.441] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 21.70ms | 8.41ms | 2.372 | [1.755, 3.205] | faster |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.526s | 2.404s | 1.465 | [1.453, 1.477] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.523s | 2.403s | 1.464 | [1.453, 1.476] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.95ms | 1.43ms | 1.228 | [0.897, 1.680] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.398s | 777.74ms | 1.779 | [1.724, 1.836] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.296s | 706.86ms | 1.822 | [1.761, 1.885] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 89.48ms | 69.72ms | 1.207 | [1.044, 1.395] | faster |
| quad/sg/cc-uq/b0/d3/L14 | total | 181.26ms | 76.98ms | 2.355 | [2.322, 2.388] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 179.09ms | 75.74ms | 2.366 | [2.336, 2.395] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 1.23ms | 1.744 | [1.479, 2.057] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 779.55ms | 318.81ms | 2.439 | [2.416, 2.463] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 776.36ms | 317.13ms | 2.443 | [2.419, 2.467] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.49ms | 1.60ms | 1.504 | [1.330, 1.701] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.818s | 688.92ms | 2.640 | [2.625, 2.655] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.815s | 687.09ms | 2.644 | [2.629, 2.658] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.03ms | 1.47ms | 1.201 | [0.792, 1.822] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.624s | 1.310s | 2.771 | [2.751, 2.791] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.620s | 1.307s | 2.775 | [2.755, 2.795] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.57ms | 1.94ms | 1.043 | [0.597, 1.823] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 105.99ms | 94.81ms | 1.121 | [1.101, 1.143] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 104.70ms | 93.84ms | 1.115 | [1.096, 1.135] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.16ms | 828.59us | 1.551 | [1.074, 2.240] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.86ms | 211.87ms | 1.052 | [1.043, 1.062] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.83ms | 211.29ms | 1.042 | [1.034, 1.050] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.38ms | 466.54us | 2.971 | [2.299, 3.839] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.236s | 1.191s | 1.038 | [1.032, 1.044] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.229s | 1.190s | 1.032 | [1.027, 1.036] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.62ms | 179.00us | 23.094 | [15.656, 34.065] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.666s | 5.436s | 1.044 | [1.036, 1.051] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.657s | 5.435s | 1.042 | [1.035, 1.049] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.83ms | 489.23us | 11.036 | [6.686, 18.217] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.975s | 1.169s | 3.395 | [3.381, 3.408] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.898s | 1.160s | 3.358 | [3.346, 3.371] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 48.54ms | 4.42ms | 10.616 | [9.095, 12.391] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.766s | 1.139s | 5.941 | [5.919, 5.963] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.651s | 1.120s | 5.941 | [5.919, 5.963] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.12ms | 11.08ms | 6.414 | [5.668, 7.258] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.65ms | 247.99ms | 1.086 | [1.081, 1.091] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.88ms | 247.23ms | 1.074 | [1.070, 1.078] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.54ms | 649.17us | 3.361 | [2.025, 5.580] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.520s | 1.447s | 1.050 | [1.046, 1.054] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.509s | 1.446s | 1.043 | [1.039, 1.047] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.88ms | 239.76us | 20.385 | [17.800, 23.346] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.973s | 6.894s | 1.011 | [1.005, 1.017] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.963s | 6.893s | 1.010 | [1.004, 1.016] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.32ms | 502.36us | 10.798 | [6.808, 17.128] | faster |
| interp/sg/cc/b0/d2/L12/p64 | total | 234.03ms | 127.78ms | 1.816 | [1.755, 1.880] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 233.65ms | 127.50ms | 1.817 | [1.756, 1.880] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 215.53us | 220.91us | 0.977 | [0.895, 1.067] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 309.42ms | 186.78ms | 1.668 | [1.630, 1.707] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 309.27ms | 186.63ms | 1.668 | [1.630, 1.708] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 79.13us | 89.54us | 0.869 | [0.805, 0.937] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.741s | 1.129s | 1.545 | [1.532, 1.559] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.740s | 1.129s | 1.545 | [1.532, 1.559] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.11us | 70.96us | 1.507 | [1.181, 1.924] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.467s | 1.007s | 1.457 | [1.439, 1.476] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.467s | 1.007s | 1.457 | [1.439, 1.476] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 80.11us | 69.98us | 1.141 | [1.068, 1.219] | faster |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.230s | 1.611s | 1.384 | [1.359, 1.410] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.230s | 1.611s | 1.384 | [1.359, 1.410] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 40.44us | 48.61us | 1.824 | [0.430, 7.743] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.15ms | 34.13ms | 5.487 | [5.438, 5.536] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.02ms | 34.06ms | 5.495 | [5.444, 5.547] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.50us | 34.78us | 1.324 | [0.445, 3.940] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 368.88ms | 116.82ms | 3.172 | [3.107, 3.239] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 368.76ms | 116.73ms | 3.174 | [3.108, 3.240] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 40.65us | 38.27us | 1.117 | [0.868, 1.438] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.26ms | 11.88ms | 4.510 | [4.453, 4.568] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.19ms | 11.81ms | 4.532 | [4.475, 4.589] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.84us | 31.71us | 0.720 | [0.545, 0.951] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.77ms | 110.12ms | 1.103 | [0.870, 1.397] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 105.73ms | 103.24ms | 1.114 | [0.868, 1.429] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.70ms | 2.32ms | 0.766 | [0.704, 0.834] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 524.20ms | 364.20ms | 1.416 | [1.369, 1.465] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 521.33ms | 363.65ms | 1.410 | [1.363, 1.459] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.24ms | 76.61us | 29.088 | [25.587, 33.068] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.670s | 1.189s | 1.398 | [1.366, 1.430] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.668s | 1.186s | 1.398 | [1.367, 1.430] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.41ms | 2.05ms | 1.137 | [0.624, 2.072] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.706s | 1.845s | 1.462 | [1.419, 1.506] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.703s | 1.844s | 1.462 | [1.419, 1.506] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.80ms | 1.48ms | 1.220 | [1.104, 1.349] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.242s | 755.73ms | 2.925 | [2.773, 3.085] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.236s | 752.56ms | 2.930 | [2.777, 3.091] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.69ms | 2.64ms | 1.773 | [1.752, 1.794] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.260s | 481.80ms | 6.525 | [4.814, 8.845] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.881s | 474.13ms | 3.832 | [2.814, 5.220] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.377s | 7.13ms | 191.035 | [153.079, 238.402] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.886s | 26.19ms | 73.838 | [70.203, 77.662] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 103.37ms | 23.62ms | 4.347 | [4.107, 4.601] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.782s | 2.37ms | 760.701 | [693.374, 834.566] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.909s | 4.72ms | 488.434 | [342.680, 696.184] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 10.14ms | 2.03ms | 3.890 | [2.323, 6.513] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.898s | 760.15us | 1970.011 | [989.850, 3920.737] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | total | (not in A) | 52.01ms | - | - | new case |
| interp/dsg/cc/b0/d3/L9/p64 | total | (not in A) | 43.39ms | - | - | new case |
| interp/dsg/cc/b0/d5/L6/p64 | total | (not in A) | 174.75ms | - | - | new case |
| interp/dsg/cc/b0/d7/L4/p16 | total | (not in A) | 151.02ms | - | - | new case |
| interp/dsg/cc/b0/d10/L2/p8 | total | (not in A) | 398.87ms | - | - | new case |
| interp/dsg/equi/b0/d3/L5/p64 | total | (not in A) | 6.54ms | - | - | new case |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | (not in A) | 177.77ms | - | - | new case |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | (not in A) | 179.96ms | - | - | new case |
| interplin/dsg/equi/b1/d8/L5/p256 | total | (not in A) | 374.69ms | - | - | new case |
| interplin/dsg/equi/b1/d10/L5/p32 | total | (not in A) | 434.48ms | - | - | new case |

Sum of medians (total, common cases): A = 69.927s, B = 35.446s, saved = 34.481s (1.973x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 2.26e-16 -> B 5.76e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 364.0 MiB | 124.4 MiB | 0.342 | 0.2% / 20.9% | MEM- |
| quad/sg/equi-exp/b0/d5/L10 | 15.0 MiB | 12.7 MiB | 0.844 | 2.3% / 2.9% | MEM- |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 12.8 MiB | 0.880 | 4.0% / 2.1% | MEM- |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.8 MiB | 0.998 | 2.1% / 1.0% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 13.3 MiB | 0.511 | 2.2% / 1.8% | MEM- |
| quad/sg/equi-exp/b1/d15/L6 | 54.5 MiB | 18.4 MiB | 0.337 | 5.7% / 0.8% | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 59.5 MiB | 20.2 MiB | 0.339 | 13.3% / 1.0% | MEM- |
| quad/sg/equi-sin/b0/d7/L7 | 14.4 MiB | 13.0 MiB | 0.898 | 3.7% / 3.1% | MEM- |
| quad/sg/cc-uq/b0/d2/L20 | 806.5 MiB | 309.1 MiB | 0.383 | 5.5% / 11.6% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 38.2 MiB | 17.4 MiB | 0.455 | 3.8% / 4.2% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 13.1 MiB | 0.761 | 3.8% / 1.9% | MEM- |
| quad/sg/cc-uq/b0/d7/L7 | 14.7 MiB | 13.0 MiB | 0.881 | 2.8% / 0.5% | MEM- |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 12.9 MiB | 0.990 | 2.7% / 1.4% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.3 MiB | 43.9 MiB | 0.705 | 1.7% / 4.9% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 24.0 MiB | 18.0 MiB | 0.747 | 4.6% / 2.8% | MEM- |
| quad/dsg/equi-exp/b0/d7/L10 | 37.1 MiB | 28.3 MiB | 0.763 | 1.6% / 3.2% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 36.5 MiB | 29.8 MiB | 0.816 | 3.8% / 4.0% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 231.4 MiB | 51.6 MiB | 0.223 | 1.9% / 0.4% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 332.0 MiB | 82.8 MiB | 0.249 | 1.5% / 0.2% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 24.2 MiB | 18.0 MiB | 0.742 | 2.0% / 1.7% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 37.5 MiB | 29.1 MiB | 0.777 | 2.6% / 6.6% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 37.4 MiB | 29.8 MiB | 0.797 | 3.1% / 4.3% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.3 MiB | 0.968 | 4.6% / 2.1% | MEM- |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.3 MiB | 1.021 | 3.7% / 1.7% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.2 MiB | 1.005 | 3.2% / 2.2% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.1 MiB | 12.2 MiB | 0.934 | 2.3% / 1.2% | MEM- |
| interp/sg/cc/b0/d10/L2/p8 | 24.8 MiB | 12.3 MiB | 0.495 | 9.1% / 1.6% | MEM- |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.8 MiB | 12.3 MiB | 1.036 | 3.6% / 2.6% | MEM+ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.8 MiB | 12.2 MiB | 1.034 | 1.8% / 1.4% | MEM+ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.3 MiB | 1.031 | 2.5% / 1.0% | MEM+ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 19.9 MiB | 1.020 | 1.1% / 4.6% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 12.8 MiB | 0.975 | 0.8% / 2.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.7 MiB | 0.979 | 4.0% / 2.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.3 MiB | 12.7 MiB | 0.955 | 4.8% / 1.4% | MEM- |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.2 MiB | 13.5 MiB | 0.833 | 2.7% / 1.8% | MEM- |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.6 MiB | 15.1 MiB | 0.858 | 1.1% / 2.5% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 13.1 MiB | 1.028 | 1.2% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.6 MiB | 1.032 | 2.6% / 1.9% | MEM+ |
| interp/dsg/cc/b0/d2/L12/p64 | (not in A) | 15.3 MiB | - | - | new case |
| interp/dsg/cc/b0/d3/L9/p64 | (not in A) | 12.6 MiB | - | - | new case |
| interp/dsg/cc/b0/d5/L6/p64 | (not in A) | 12.4 MiB | - | - | new case |
| interp/dsg/cc/b0/d7/L4/p16 | (not in A) | 12.4 MiB | - | - | new case |
| interp/dsg/cc/b0/d10/L2/p8 | (not in A) | 12.2 MiB | - | - | new case |
| interp/dsg/equi/b0/d3/L5/p64 | (not in A) | 12.4 MiB | - | - | new case |
| interplin/dsg/equi/b1/d2/L16/p16384 | (not in A) | 21.3 MiB | - | - | new case |
| interplin/dsg/equi/b1/d5/L8/p1024 | (not in A) | 12.9 MiB | - | - | new case |
| interplin/dsg/equi/b1/d8/L5/p256 | (not in A) | 12.6 MiB | - | - | new case |
| interplin/dsg/equi/b1/d10/L5/p32 | (not in A) | 15.3 MiB | - | - | new case |

Peak memory (common cases): geomean B/A = 0.724; largest B/A: `interp/sg/cheb1/b0/d3/L6/p64` 11.8 MiB -> 12.3 MiB (1.036x); 4 MEM+, 26 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.9%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 226, 225, 224, 224, 224 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1124s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
