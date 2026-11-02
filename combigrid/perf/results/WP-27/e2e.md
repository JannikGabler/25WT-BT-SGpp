A/B `e2e`: A=`WP-27-before` B=`WP-27-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 390.75ms | 390.43ms | 1.012 | [0.907, 1.129] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 322.73ms | 323.68ms | 1.029 | [0.921, 1.149] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.08ms | 30.94ms | 0.998 | [0.897, 1.112] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 248.09ms | 247.57ms | 1.007 | [0.992, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.51ms | 244.98ms | 1.004 | [0.992, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.04ms | 2.00ms | 1.269 | [0.702, 2.294] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.67ms | 496.08ms | 1.000 | [0.990, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 494.12ms | 493.59ms | 1.000 | [0.990, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.82ms | 1.84ms | 1.014 | [0.934, 1.100] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 914.38ms | 915.62ms | 0.996 | [0.986, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 911.91ms | 912.63ms | 0.997 | [0.988, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.00ms | 3.80ms | 0.640 | [0.405, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 301.85ms | 301.04ms | 1.001 | [0.984, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 293.08ms | 292.60ms | 1.000 | [0.983, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.40ms | 5.10ms | 1.039 | [0.719, 1.502] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 523.47ms | 519.17ms | 1.006 | [0.994, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 498.95ms | 494.63ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.71ms | 15.54ms | 1.035 | [0.882, 1.215] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 231.54ms | 231.85ms | 1.000 | [0.980, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.36ms | 206.43ms | 0.992 | [0.978, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.03ms | 17.21ms | 1.068 | [0.790, 1.443] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.402s | 2.405s | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.400s | 2.402s | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.83ms | 1.87ms | 0.963 | [0.867, 1.070] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.204s | 1.216s | 0.986 | [0.924, 1.053] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.130s | 1.128s | 0.992 | [0.926, 1.064] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 74.20ms | 82.53ms | 0.929 | [0.804, 1.074] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.55ms | 85.37ms | 1.012 | [0.980, 1.045] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.80ms | 82.93ms | 1.008 | [0.980, 1.037] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.03ms | 2.05ms | 1.043 | [0.955, 1.139] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.24ms | 321.03ms | 0.999 | [0.990, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 317.39ms | 317.94ms | 1.001 | [0.991, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.21ms | 2.35ms | 0.871 | [0.593, 1.281] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 692.08ms | 692.46ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 689.13ms | 689.80ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.98ms | 1.91ms | 1.178 | [0.715, 1.942] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.301s | 1.307s | 0.982 | [0.932, 1.035] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.299s | 1.304s | 0.983 | [0.932, 1.036] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.24ms | 2.64ms | 0.728 | [0.502, 1.057] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.10ms | 94.56ms | 0.972 | [0.961, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.70ms | 93.24ms | 0.971 | [0.961, 0.981] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.19ms | 1.02ms | 1.056 | [0.863, 1.293] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.80ms | 202.28ms | 0.996 | [0.988, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.29ms | 199.82ms | 0.996 | [0.990, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.08ms | 1.16ms | 0.941 | [0.624, 1.420] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.135s | 1.136s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.127s | 1.128s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.01ms | 3.04ms | 1.004 | [0.979, 1.030] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.403s | 5.407s | 0.999 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.395s | 5.398s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.35ms | 3.68ms | 0.936 | [0.705, 1.243] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.219s | 1.220s | 1.006 | [1.000, 1.011] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.147s | 1.143s | 1.005 | [0.997, 1.013] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.14ms | 44.33ms | 0.963 | [0.825, 1.124] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.227s | 1.225s | 0.995 | [0.975, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.122s | 0.993 | [0.969, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.64ms | 61.36ms | 1.018 | [0.954, 1.086] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.66ms | 248.38ms | 1.004 | [0.994, 1.015] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.09ms | 244.69ms | 1.004 | [0.994, 1.015] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.27ms | 2.32ms | 1.027 | [0.927, 1.138] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.440s | 1.441s | 1.007 | [0.995, 1.019] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.430s | 1.431s | 1.006 | [0.995, 1.017] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.84ms | 4.87ms | 1.107 | [0.950, 1.290] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.788s | 6.788s | 0.998 | [0.989, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.779s | 6.778s | 0.998 | [0.989, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.18ms | 3.98ms | 1.011 | [0.661, 1.545] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 173.33ms | 173.15ms | 0.995 | [0.944, 1.048] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 172.97ms | 172.75ms | 0.995 | [0.944, 1.048] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 204.35us | 205.61us | 0.978 | [0.896, 1.068] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 241.57ms | 241.54ms | 0.999 | [0.979, 1.019] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 241.43ms | 241.39ms | 0.999 | [0.979, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 72.50us | 73.89us | 0.944 | [0.817, 1.090] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.446s | 1.446s | 1.002 | [0.990, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.446s | 1.446s | 1.002 | [0.990, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.53us | 111.95us | 1.103 | [0.829, 1.466] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.265s | 1.258s | 1.007 | [0.994, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.265s | 1.257s | 1.007 | [0.994, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 62.93us | 63.21us | 0.999 | [0.947, 1.054] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.995s | 1.972s | 1.004 | [0.984, 1.025] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.995s | 1.969s | 1.005 | [0.984, 1.025] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 33.38us | 35.34us | 0.407 | [0.034, 4.900] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.34ms | 181.80ms | 1.060 | [0.898, 1.251] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.26ms | 181.72ms | 1.058 | [0.901, 1.242] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 34.29us | 30.03us | 2.548 | [0.220, 29.484] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 336.01ms | 334.15ms | 1.010 | [0.988, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.90ms | 334.04ms | 1.010 | [0.988, 1.032] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.78us | 34.78us | 1.052 | [0.817, 1.354] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.83ms | 50.92ms | 1.119 | [0.814, 1.539] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.75ms | 50.85ms | 1.119 | [0.814, 1.539] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.79us | 20.46us | 1.017 | [0.748, 1.383] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.22ms | 110.12ms | 0.984 | [0.948, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.10ms | 104.21ms | 0.984 | [0.944, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.66ms | 1.65ms | 0.983 | [0.896, 1.079] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 441.26ms | 446.27ms | 1.022 | [0.929, 1.124] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 438.36ms | 443.33ms | 1.023 | [0.929, 1.127] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.19ms | 2.26ms | 0.810 | [0.555, 1.182] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.507s | 1.517s | 0.996 | [0.973, 1.019] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.504s | 1.515s | 0.996 | [0.974, 1.019] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.22ms | 2.21ms | 1.028 | [0.679, 1.555] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.435s | 2.458s | 0.983 | [0.963, 1.003] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.432s | 2.456s | 0.983 | [0.963, 1.003] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.75ms | 1.77ms | 0.981 | [0.875, 1.099] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 741.21ms | 726.89ms | 1.004 | [0.951, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 737.52ms | 723.19ms | 1.003 | [0.951, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.98ms | 2.99ms | 1.003 | [0.984, 1.022] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 493.42ms | 500.11ms | 1.001 | [0.947, 1.058] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 484.75ms | 491.57ms | 1.001 | [0.946, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.53ms | 7.54ms | 0.994 | [0.975, 1.013] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 28.15ms | 26.16ms | 1.232 | [0.904, 1.678] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 25.45ms | 23.45ms | 1.101 | [0.955, 1.270] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.44ms | 1.598 | [0.429, 5.958] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.17ms | 3.14ms | 1.120 | [0.838, 1.497] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.05ms | 2.04ms | 1.002 | [0.926, 1.086] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 883.63us | 869.03us | 1.264 | [0.711, 2.246] | ~ (noise) |

Sum of medians (total, common cases): A = 38.316s, B = 38.335s, saved = -19177153.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 6.40e-13 -> B 1.01e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 620.0 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.9 MiB | 16.0 MiB | 1.005 | 4.5% / 1.3% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 14.8 MiB | 1.012 | 2.0% / 3.1% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.0 MiB | 13.1 MiB | 1.011 | 1.9% / 1.6% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.3 MiB | 26.1 MiB | 0.991 | 3.0% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 54.0 MiB | 53.6 MiB | 0.992 | 7.7% / 0.9% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 57.9 MiB | 57.0 MiB | 0.984 | 15.1% / 10.6% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 14.6 MiB | 0.996 | 4.0% / 3.3% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1073.5 MiB | 1069.8 MiB | 0.997 | 2.5% / 1.2% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 41.3 MiB | 40.3 MiB | 0.977 | 11.6% / 2.9% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.7 MiB | 17.5 MiB | 0.990 | 2.8% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.0 MiB | 15.3 MiB | 1.017 | 1.4% / 4.6% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.2 MiB | 1.013 | 3.5% / 2.5% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.3 MiB | 62.9 MiB | 1.009 | 1.2% / 1.3% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.9 MiB | 26.4 MiB | 1.019 | 4.4% / 3.9% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.0 MiB | 42.7 MiB | 1.017 | 5.4% / 4.9% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.2 MiB | 42.0 MiB | 0.995 | 4.0% / 2.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 152.2 MiB | 150.2 MiB | 0.987 | 2.4% / 3.5% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.7 MiB | 217.5 MiB | 0.999 | 0.6% / 0.5% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.2 MiB | 25.6 MiB | 1.016 | 3.4% / 4.0% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.2 MiB | 42.9 MiB | 1.018 | 6.9% / 3.5% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.4 MiB | 41.9 MiB | 0.989 | 3.9% / 4.5% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 15.1 MiB | 1.013 | 4.2% / 4.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.3 MiB | 1.009 | 1.2% / 2.7% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.4 MiB | 1.017 | 2.1% / 1.4% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.2 MiB | 13.2 MiB | 0.999 | 5.9% / 4.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.8 MiB | 24.5 MiB | 0.988 | 3.6% / 3.4% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.1 MiB | 12.1 MiB | 1.001 | 1.8% / 2.5% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.2 MiB | 1.013 | 2.7% / 0.7% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.0 MiB | 12.2 MiB | 1.022 | 2.7% / 0.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.8 MiB | 19.8 MiB | 0.999 | 3.8% / 0.8% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.4 MiB | 1.003 | 3.5% / 3.0% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 13.1 MiB | 1.020 | 3.2% / 4.1% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.6 MiB | 13.6 MiB | 0.998 | 6.9% / 5.3% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.4 MiB | 16.4 MiB | 0.999 | 2.7% / 3.6% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.4 MiB | 17.6 MiB | 1.012 | 4.0% / 2.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.1 MiB | 13.0 MiB | 0.990 | 2.5% / 2.0% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.4 MiB | 12.6 MiB | 1.017 | 2.4% / 1.9% | ~ |

Peak memory (common cases): geomean B/A = 1.003; largest B/A: `interp/sg/equi/b0/d3/L5/p64` 12.0 MiB -> 12.2 MiB (1.022x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 163, 162, 162, 163, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 811s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
