A/B `e2e`: A=`WP-06-before` B=`WP-06-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 435.09ms | 431.07ms | 1.003 | [0.899, 1.120] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 355.30ms | 361.68ms | 0.961 | [0.861, 1.074] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.35ms | 31.69ms | 0.995 | [0.893, 1.107] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 265.61ms | 266.47ms | 0.994 | [0.982, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 262.99ms | 264.06ms | 0.996 | [0.985, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 2.05ms | 0.862 | [0.496, 1.499] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 517.55ms | 515.27ms | 1.004 | [0.993, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 514.97ms | 512.87ms | 1.003 | [0.993, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.79ms | 1.82ms | 1.000 | [0.942, 1.062] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 914.53ms | 911.01ms | 1.019 | [0.974, 1.066] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 910.95ms | 908.72ms | 1.019 | [0.974, 1.066] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.19ms | 1.96ms | 1.039 | [0.618, 1.748] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 298.42ms | 299.25ms | 1.004 | [0.977, 1.031] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 290.07ms | 290.67ms | 1.005 | [0.977, 1.033] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.11ms | 5.25ms | 0.931 | [0.770, 1.126] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 517.65ms | 519.50ms | 0.996 | [0.983, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 494.81ms | 495.69ms | 0.998 | [0.984, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 13.65ms | 15.37ms | 0.907 | [0.810, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 229.39ms | 233.09ms | 0.992 | [0.953, 1.032] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 202.92ms | 202.40ms | 1.000 | [0.971, 1.029] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 16.92ms | 18.52ms | 0.933 | [0.679, 1.280] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.407s | 2.408s | 0.995 | [0.980, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.405s | 2.405s | 0.995 | [0.980, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.90ms | 1.90ms | 0.998 | [0.898, 1.108] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.197s | 1.253s | 0.976 | [0.896, 1.062] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.112s | 1.171s | 0.973 | [0.881, 1.074] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 73.62ms | 81.80ms | 0.966 | [0.807, 1.156] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.03ms | 86.42ms | 0.994 | [0.980, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.25ms | 84.18ms | 0.991 | [0.979, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.11ms | 1.013 | [0.945, 1.085] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 323.51ms | 322.95ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 320.57ms | 320.22ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.27ms | 2.25ms | 1.013 | [0.941, 1.090] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 697.40ms | 694.24ms | 1.005 | [0.999, 1.011] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 694.91ms | 691.64ms | 1.005 | [0.999, 1.011] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.86ms | 1.99ms | 0.946 | [0.877, 1.020] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.316s | 1.321s | 0.998 | [0.989, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.313s | 1.319s | 0.998 | [0.989, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.19ms | 2.21ms | 1.007 | [0.758, 1.338] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 174.32ms | 130.73ms | 1.321 | [1.276, 1.368] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 173.96ms | 130.48ms | 1.320 | [1.275, 1.367] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 219.37us | 203.80us | 1.040 | [0.952, 1.136] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 243.31ms | 186.14ms | 1.306 | [1.280, 1.333] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 243.17ms | 186.00ms | 1.307 | [1.280, 1.333] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.75us | 73.33us | 1.135 | [0.708, 1.820] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.442s | 1.126s | 1.280 | [1.267, 1.294] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.442s | 1.126s | 1.281 | [1.267, 1.295] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 109.02us | 143.66us | 0.705 | [0.562, 0.883] | SLOWER |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.255s | 1.004s | 1.241 | [1.217, 1.266] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.255s | 1.004s | 1.242 | [1.219, 1.265] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 63.62us | 123.69us | 0.353 | [0.071, 1.754] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.978s | 1.595s | 1.230 | [1.195, 1.266] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.978s | 1.595s | 1.230 | [1.195, 1.266] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.06us | 42.88us | 0.725 | [0.590, 0.892] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.73ms | 34.23ms | 5.330 | [5.252, 5.408] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 182.63ms | 34.16ms | 5.339 | [5.264, 5.416] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.92us | 25.77us | 1.078 | [0.726, 1.602] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 344.51ms | 116.54ms | 2.955 | [2.917, 2.994] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 344.40ms | 116.42ms | 2.957 | [2.919, 2.996] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 35.27us | 39.32us | 0.883 | [0.650, 1.199] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.02ms | 11.90ms | 4.295 | [4.252, 4.339] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.95ms | 11.83ms | 4.314 | [4.270, 4.358] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.28us | 24.30us | 0.944 | [0.772, 1.155] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.46ms | 52.43ms | 1.003 | [0.993, 1.014] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 52.18ms | 52.14ms | 1.004 | [0.993, 1.014] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 136.89us | 132.56us | 1.029 | [1.010, 1.049] | faster |
| interp/dsg/cc/b0/d3/L9/p64 | total | 42.89ms | 42.93ms | 1.012 | [0.979, 1.046] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.66ms | 42.69ms | 1.012 | [0.979, 1.046] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 66.70us | 66.63us | 1.005 | [0.852, 1.186] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 170.45ms | 172.25ms | 0.989 | [0.940, 1.041] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 170.18ms | 172.01ms | 0.989 | [0.940, 1.041] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 78.01us | 76.76us | 1.007 | [0.978, 1.037] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 148.93ms | 150.36ms | 0.975 | [0.940, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 148.76ms | 150.19ms | 0.975 | [0.940, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 58.53us | 57.69us | 1.055 | [0.892, 1.247] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 394.46ms | 392.59ms | 1.006 | [1.000, 1.012] | faster |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 394.38ms | 392.52ms | 1.006 | [1.000, 1.012] | faster |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 34.85us | 32.41us | 1.051 | [0.944, 1.170] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.49ms | 6.50ms | 1.001 | [0.982, 1.021] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.42ms | 6.43ms | 1.001 | [0.982, 1.021] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 23.33us | 20.53us | 1.067 | [0.957, 1.190] | ~ (noise) |

Sum of medians (total, common cases): A = 15.691s, B = 14.282s, saved = 1.409s (1.099x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.47e-15 -> B 5.42e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.1 MiB | 620.0 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 16.0 MiB | 15.8 MiB | 0.986 | 2.5% / 2.4% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.7 MiB | 14.9 MiB | 1.017 | 3.2% / 1.9% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.0 MiB | 13.0 MiB | 0.998 | 4.6% / 3.8% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 25.8 MiB | 0.995 | 6.5% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 51.8 MiB | 53.2 MiB | 1.027 | 2.6% / 3.7% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 56.3 MiB | 58.1 MiB | 1.032 | 13.3% / 11.9% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.9 MiB | 14.8 MiB | 0.998 | 3.5% / 4.3% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1071.1 MiB | 1078.2 MiB | 1.007 | 1.7% / 3.2% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 41.4 MiB | 41.1 MiB | 0.994 | 3.9% / 7.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.8 MiB | 18.0 MiB | 1.008 | 3.4% / 3.7% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.3 MiB | 15.2 MiB | 0.997 | 6.6% / 5.6% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.2 MiB | 13.2 MiB | 1.004 | 3.4% / 2.4% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.2 MiB | 16.1 MiB | 1.060 | 2.0% / 1.6% | MEM+ |
| interp/sg/cc/b0/d3/L9/p64 | 12.4 MiB | 12.2 MiB | 0.984 | 3.1% / 1.6% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.2 MiB | 0.994 | 2.6% / 1.8% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.2 MiB | 12.1 MiB | 0.916 | 4.7% / 1.2% | MEM- |
| interp/sg/cc/b0/d10/L2/p8 | 24.3 MiB | 12.1 MiB | 0.496 | 5.1% / 2.0% | MEM- |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.1 MiB | 12.2 MiB | 1.010 | 2.2% / 1.3% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.3 MiB | 1.007 | 2.1% / 1.1% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.2 MiB | 0.996 | 2.7% / 1.7% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.1 MiB | 16.2 MiB | 1.006 | 2.7% / 2.7% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.7 MiB | 12.6 MiB | 0.997 | 1.4% / 2.0% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.6 MiB | 12.5 MiB | 0.992 | 1.7% / 1.1% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.6 MiB | 12.5 MiB | 0.993 | 1.6% / 3.0% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 1.002 | 1.0% / 1.7% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.2 MiB | 0.998 | 1.8% / 3.3% | ~ |

Peak memory (common cases): geomean B/A = 0.975; largest B/A: `interp/sg/cc/b0/d2/L12/p64` 15.2 MiB -> 16.1 MiB (1.060x); 1 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.2%, 0.3%, 0.3%, 0.2%
Round durations (s): 66, 66, 66, 66, 66 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 330s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
