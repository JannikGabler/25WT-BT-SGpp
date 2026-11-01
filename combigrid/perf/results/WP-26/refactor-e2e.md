A/B `e2e`: A=`WP-26-before` B=`WP-26-after-r`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 391.04ms | 400.76ms | 0.976 | [0.875, 1.089] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 318.80ms | 325.03ms | 0.981 | [0.878, 1.096] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.47ms | 30.67ms | 1.028 | [0.921, 1.148] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 245.60ms | 247.64ms | 0.995 | [0.983, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 243.20ms | 245.11ms | 0.995 | [0.984, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.88ms | 1.98ms | 0.952 | [0.834, 1.087] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 500.19ms | 497.10ms | 1.005 | [0.994, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 497.80ms | 494.76ms | 1.005 | [0.994, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.77ms | 1.74ms | 1.020 | [0.964, 1.079] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 923.95ms | 918.15ms | 1.007 | [1.001, 1.013] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 920.21ms | 914.16ms | 1.006 | [0.999, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 3.77ms | 2.20ms | 1.553 | [1.190, 2.028] | faster |
| quad/sg/equi-exp/b1/d10/L7 | total | 301.41ms | 301.27ms | 0.998 | [0.976, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 293.10ms | 292.82ms | 0.997 | [0.978, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.19ms | 5.12ms | 1.038 | [0.849, 1.269] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 518.50ms | 523.99ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 494.38ms | 495.21ms | 0.999 | [0.985, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.51ms | 16.15ms | 0.935 | [0.834, 1.047] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 235.98ms | 230.99ms | 1.013 | [0.990, 1.036] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 206.76ms | 205.17ms | 1.007 | [0.994, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.31ms | 17.65ms | 1.067 | [0.788, 1.444] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.409s | 2.406s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.406s | 2.403s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.82ms | 1.88ms | 0.982 | [0.884, 1.091] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.198s | 1.208s | 0.986 | [0.929, 1.046] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.115s | 1.127s | 0.996 | [0.939, 1.056] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 78.86ms | 82.58ms | 0.937 | [0.811, 1.083] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 85.69ms | 87.05ms | 0.992 | [0.976, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.44ms | 84.17ms | 0.996 | [0.983, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.07ms | 2.06ms | 0.990 | [0.937, 1.046] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.89ms | 320.64ms | 1.004 | [0.995, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.17ms | 318.15ms | 1.004 | [0.995, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.25ms | 2.25ms | 1.016 | [0.871, 1.186] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 696.57ms | 690.39ms | 1.006 | [1.000, 1.012] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 693.87ms | 687.86ms | 1.006 | [1.001, 1.012] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.99ms | 2.03ms | 1.016 | [0.908, 1.137] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.309s | 1.305s | 1.011 | [0.989, 1.034] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.306s | 1.302s | 1.012 | [0.989, 1.034] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.38ms | 2.37ms | 1.017 | [0.626, 1.653] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 91.20ms | 92.89ms | 0.982 | [0.969, 0.995] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 89.85ms | 91.54ms | 0.982 | [0.969, 0.995] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 969.32us | 1.02ms | 1.023 | [0.836, 1.252] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.64ms | 201.97ms | 0.994 | [0.984, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.10ms | 199.61ms | 0.992 | [0.984, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.12ms | 1.03ms | 1.051 | [0.815, 1.356] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.133s | 1.135s | 0.998 | [0.994, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.126s | 1.128s | 0.999 | [0.994, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.00ms | 2.99ms | 0.886 | [0.659, 1.191] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.395s | 5.409s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.387s | 5.401s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.43ms | 3.34ms | 0.809 | [0.492, 1.332] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.220s | 1.223s | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.148s | 1.147s | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.19ms | 44.76ms | 0.943 | [0.808, 1.101] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.232s | 1.227s | 1.007 | [0.999, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.127s | 1.125s | 1.007 | [0.994, 1.019] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 62.70ms | 60.35ms | 1.022 | [0.958, 1.091] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.60ms | 248.34ms | 0.994 | [0.982, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 243.80ms | 244.84ms | 0.993 | [0.983, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.29ms | 2.22ms | 1.047 | [0.899, 1.219] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.443s | 1.442s | 0.998 | [0.977, 1.020] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.433s | 1.431s | 0.999 | [0.978, 1.020] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.47ms | 4.93ms | 0.892 | [0.708, 1.125] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.775s | 6.783s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.765s | 6.773s | 1.000 | [0.995, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.48ms | 4.55ms | 1.030 | [0.674, 1.575] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 175.86ms | 173.81ms | 1.009 | [0.990, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 175.52ms | 173.40ms | 1.009 | [0.990, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 208.82us | 206.59us | 1.015 | [0.929, 1.108] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 247.76ms | 241.84ms | 1.019 | [0.995, 1.044] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 247.62ms | 241.70ms | 1.019 | [0.995, 1.044] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 71.59us | 74.24us | 1.012 | [0.900, 1.137] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.446s | 1.441s | 1.005 | [1.000, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.445s | 1.441s | 1.005 | [1.000, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 115.59us | 107.34us | 1.074 | [0.857, 1.347] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.264s | 1.255s | 1.005 | [0.992, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.264s | 1.255s | 1.005 | [0.992, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.00us | 67.81us | 0.992 | [0.940, 1.046] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.990s | 1.987s | 1.003 | [0.993, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.990s | 1.987s | 1.003 | [0.993, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.57us | 35.27us | 1.027 | [0.696, 1.514] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.61ms | 187.88ms | 0.970 | [0.940, 1.002] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.53ms | 187.80ms | 0.970 | [0.939, 1.002] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.63us | 27.24us | 1.468 | [0.566, 3.804] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 335.58ms | 335.68ms | 0.997 | [0.979, 1.015] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.47ms | 335.57ms | 0.997 | [0.979, 1.015] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.85us | 34.36us | 1.020 | [0.793, 1.313] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.76ms | 51.07ms | 0.985 | [0.968, 1.003] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.69ms | 51.00ms | 0.985 | [0.967, 1.003] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.44us | 20.60us | 1.040 | [0.894, 1.210] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 108.14ms | 110.38ms | 0.982 | [0.945, 1.020] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.00ms | 104.42ms | 0.980 | [0.941, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.61ms | 1.68ms | 0.966 | [0.869, 1.074] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 438.69ms | 440.37ms | 0.999 | [0.974, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 435.94ms | 437.36ms | 0.999 | [0.973, 1.026] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.22ms | 2.13ms | 1.016 | [0.894, 1.155] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.538s | 1.540s | 1.004 | [0.982, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.536s | 1.537s | 1.005 | [0.982, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.16ms | 2.23ms | 0.981 | [0.648, 1.484] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.479s | 2.439s | 1.019 | [0.998, 1.040] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.477s | 2.437s | 1.018 | [0.998, 1.039] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.80ms | 1.66ms | 1.196 | [0.584, 2.453] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 735.85ms | 730.16ms | 1.006 | [0.954, 1.062] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 732.14ms | 726.53ms | 1.006 | [0.954, 1.062] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.00ms | 2.99ms | 1.007 | [0.992, 1.021] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 484.51ms | 485.47ms | 1.019 | [0.963, 1.078] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 476.15ms | 477.05ms | 1.018 | [0.960, 1.080] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.50ms | 7.53ms | 1.078 | [0.939, 1.238] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 27.52ms | 26.96ms | 1.000 | [0.962, 1.039] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.82ms | 24.27ms | 1.000 | [0.950, 1.051] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.42ms | 1.006 | [0.999, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.13ms | 3.64ms | 0.867 | [0.773, 0.973] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.11ms | 2.15ms | 0.915 | [0.809, 1.034] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 797.93us | 962.69us | 0.761 | [0.521, 1.111] | ~ (noise) |

Sum of medians (total, common cases): A = 38.379s, B = 38.348s, saved = 30.91ms (1.001x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d7/L7`: A 7.52e-12 -> B 1.50e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.9 MiB | 619.9 MiB | 1.000 | 0.0% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.6 MiB | 15.6 MiB | 0.999 | 4.2% / 3.9% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.5 MiB | 14.6 MiB | 1.012 | 4.1% / 2.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.006 | 2.0% / 3.9% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.1 MiB | 25.8 MiB | 0.990 | 2.9% / 2.3% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 52.7 MiB | 53.7 MiB | 1.017 | 1.7% / 6.2% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 59.5 MiB | 57.7 MiB | 0.969 | 12.4% / 12.2% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.7 MiB | 1.007 | 3.2% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1079.7 MiB | 1075.3 MiB | 0.996 | 1.9% / 2.5% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 41.0 MiB | 41.1 MiB | 1.003 | 8.7% / 4.2% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.6 MiB | 17.4 MiB | 0.986 | 2.2% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.8 MiB | 15.0 MiB | 1.017 | 2.8% / 2.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.1 MiB | 1.005 | 3.1% / 1.2% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 61.4 MiB | 62.7 MiB | 1.020 | 3.1% / 2.5% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.9 MiB | 26.2 MiB | 1.010 | 2.3% / 5.0% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 41.2 MiB | 42.7 MiB | 1.038 | 5.3% / 6.5% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.1 MiB | 42.0 MiB | 0.998 | 3.8% / 3.7% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.5 MiB | 151.4 MiB | 1.006 | 1.6% / 3.7% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.2 MiB | 217.2 MiB | 1.000 | 0.2% / 0.3% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.3 MiB | 25.6 MiB | 1.011 | 3.4% / 3.6% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.9 MiB | 42.3 MiB | 0.986 | 3.1% / 6.3% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.5 MiB | 42.3 MiB | 0.995 | 4.8% / 3.8% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.7 MiB | 14.8 MiB | 1.012 | 3.5% / 4.1% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.1 MiB | 1.005 | 2.8% / 1.8% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.1 MiB | 1.005 | 0.7% / 1.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.0 MiB | 12.9 MiB | 0.995 | 3.8% / 4.9% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 25.1 MiB | 24.5 MiB | 0.977 | 2.5% / 7.0% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 12.1 MiB | 1.008 | 2.5% / 2.5% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.0 MiB | 0.999 | 1.6% / 3.3% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.0 MiB | 1.013 | 1.2% / 2.3% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 19.5 MiB | 1.004 | 2.0% / 2.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 13.0 MiB | 0.985 | 3.8% / 2.2% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.8 MiB | 0.994 | 3.0% / 5.1% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.5 MiB | 13.4 MiB | 0.993 | 6.3% / 3.0% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.2 MiB | 16.3 MiB | 1.005 | 2.2% / 1.9% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.4 MiB | 17.3 MiB | 0.991 | 5.0% / 4.2% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 12.9 MiB | 1.004 | 1.9% / 1.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.3 MiB | 1.002 | 2.1% / 2.2% | ~ |

Peak memory (common cases): geomean B/A = 1.002; largest B/A: `quad/dsg/equi-exp/b0/d7/L10` 41.2 MiB -> 42.7 MiB (1.038x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.2%, 0.3%
Round durations (s): 162, 162, 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 810s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
