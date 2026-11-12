A/B `e2e`: A=`WP-50-before` B=`WP-50-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.08ms | 96.37ms | 1.002 | [0.980, 1.025] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.70ms | 95.04ms | 1.001 | [0.979, 1.024] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.15ms | 1.02ms | 1.112 | [0.909, 1.361] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.34ms | 210.56ms | 1.004 | [0.997, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.74ms | 208.08ms | 1.004 | [0.997, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 997.68us | 1.14ms | 0.900 | [0.614, 1.319] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.173s | 1.173s | 1.002 | [0.994, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.166s | 1.166s | 1.003 | [0.994, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.03ms | 3.05ms | 0.996 | [0.940, 1.054] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.379s | 5.363s | 1.008 | [0.993, 1.023] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.371s | 5.355s | 1.008 | [0.993, 1.023] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.43ms | 3.29ms | 1.053 | [0.793, 1.398] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.234s | 1.226s | 1.007 | [0.998, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.159s | 1.152s | 1.006 | [0.998, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.81ms | 44.92ms | 0.978 | [0.838, 1.142] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.215s | 1.217s | 1.004 | [0.987, 1.022] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.112s | 1.114s | 1.002 | [0.984, 1.019] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 63.46ms | 60.61ms | 1.054 | [0.972, 1.143] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 250.26ms | 249.57ms | 1.012 | [0.981, 1.045] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.67ms | 245.99ms | 1.011 | [0.983, 1.041] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.42ms | 2.26ms | 1.109 | [0.912, 1.349] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.446s | 1.442s | 1.004 | [0.999, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.436s | 1.431s | 1.004 | [0.999, 1.010] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.91ms | 4.88ms | 0.987 | [0.812, 1.199] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.871s | 6.846s | 1.005 | [0.997, 1.013] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.862s | 6.837s | 1.005 | [0.997, 1.013] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.97ms | 4.61ms | 0.967 | [0.632, 1.478] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 173.75ms | 174.72ms | 0.996 | [0.978, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 173.38ms | 174.38ms | 0.996 | [0.978, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 213.37us | 220.49us | 1.012 | [0.926, 1.105] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 242.32ms | 242.20ms | 1.001 | [0.981, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 242.18ms | 242.05ms | 1.001 | [0.981, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 74.17us | 72.91us | 1.020 | [0.947, 1.098] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.440s | 1.438s | 1.001 | [0.996, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.440s | 1.438s | 1.001 | [0.996, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 98.62us | 87.30us | 1.057 | [0.843, 1.326] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.256s | 1.255s | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.255s | 1.255s | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 63.00us | 63.49us | 0.962 | [0.905, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.975s | 1.974s | 0.999 | [0.994, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.975s | 1.974s | 0.999 | [0.994, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.29us | 34.85us | 1.453 | [0.472, 4.474] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.26ms | 181.81ms | 1.000 | [0.981, 1.020] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 182.18ms | 181.73ms | 1.000 | [0.981, 1.020] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 33.87us | 26.75us | 1.115 | [0.779, 1.596] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 343.83ms | 343.45ms | 1.002 | [0.989, 1.016] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 343.72ms | 343.33ms | 1.002 | [0.989, 1.016] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 35.34us | 36.32us | 0.989 | [0.768, 1.272] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.29ms | 51.19ms | 1.025 | [0.972, 1.082] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.22ms | 51.13ms | 1.026 | [0.972, 1.082] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.02us | 21.37us | 1.013 | [0.799, 1.283] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.38ms | 52.50ms | 0.995 | [0.984, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 52.09ms | 52.20ms | 0.995 | [0.984, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 137.45us | 140.52us | 0.990 | [0.949, 1.033] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 42.97ms | 43.04ms | 0.999 | [0.991, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.75ms | 42.86ms | 0.999 | [0.991, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 65.37us | 67.68us | 0.948 | [0.836, 1.075] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 171.10ms | 171.25ms | 0.998 | [0.992, 1.004] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 170.83ms | 171.01ms | 0.998 | [0.992, 1.004] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 78.57us | 76.76us | 1.055 | [0.988, 1.128] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 149.13ms | 149.08ms | 1.004 | [0.994, 1.014] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 148.96ms | 148.91ms | 1.004 | [0.994, 1.014] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 59.51us | 58.53us | 0.970 | [0.833, 1.131] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 394.02ms | 394.02ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 393.93ms | 393.94ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 34.43us | 37.78us | 0.851 | [0.670, 1.082] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.56ms | 6.53ms | 1.011 | [0.988, 1.034] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.48ms | 6.46ms | 1.011 | [0.988, 1.034] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 22.56us | 21.79us | 1.041 | [0.871, 1.246] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 108.41ms | 110.75ms | 0.991 | [0.954, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.37ms | 104.52ms | 0.991 | [0.951, 1.033] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.72ms | 1.68ms | 1.014 | [0.933, 1.103] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 360.28ms | 358.89ms | 1.019 | [0.958, 1.084] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 357.54ms | 356.02ms | 1.019 | [0.956, 1.086] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.04ms | 2.16ms | 1.060 | [0.788, 1.426] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.210s | 1.202s | 1.009 | [0.986, 1.032] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.206s | 1.199s | 1.009 | [0.987, 1.032] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.20ms | 2.24ms | 0.913 | [0.527, 1.584] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.843s | 1.849s | 1.001 | [0.981, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.840s | 1.847s | 1.001 | [0.981, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.76ms | 1.70ms | 1.036 | [0.959, 1.119] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 185.05ms | 184.08ms | 1.037 | [0.938, 1.146] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 180.12ms | 179.12ms | 1.038 | [0.937, 1.149] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.21ms | 1.22ms | 0.932 | [0.740, 1.173] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 185.51ms | 188.73ms | 0.988 | [0.972, 1.003] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 182.66ms | 183.85ms | 0.993 | [0.980, 1.006] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 2.07ms | 3.36ms | 0.687 | [0.479, 0.985] | SLOWER |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 375.81ms | 374.51ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 371.94ms | 372.03ms | 1.002 | [0.996, 1.008] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.30ms | 2.19ms | 0.963 | [0.668, 1.388] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 433.67ms | 431.45ms | 1.012 | [0.970, 1.055] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 431.51ms | 429.03ms | 1.012 | [0.973, 1.054] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.61ms | 1.56ms | 0.921 | [0.340, 2.498] | ~ (noise) |

Sum of medians (total, common cases): A = 29.057s, B = 29.000s, saved = 57.23ms (1.002x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d2/L12/p64`: A 1.90e-15 -> B 2.31e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 63.6 MiB | 62.8 MiB | 0.987 | 2.8% / 1.8% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 27.1 MiB | 26.2 MiB | 0.966 | 2.8% / 4.4% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 43.6 MiB | 41.6 MiB | 0.956 | 4.4% / 2.7% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 42.7 MiB | 42.3 MiB | 0.990 | 4.1% / 0.9% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 153.9 MiB | 150.5 MiB | 0.978 | 4.3% / 3.0% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 218.4 MiB | 217.6 MiB | 0.996 | 1.3% / 0.1% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 27.1 MiB | 25.7 MiB | 0.948 | 2.6% / 1.8% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 43.8 MiB | 43.4 MiB | 0.991 | 7.7% / 6.6% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 43.5 MiB | 43.0 MiB | 0.988 | 4.0% / 2.4% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.7 MiB | 15.1 MiB | 0.962 | 2.1% / 3.5% | MEM- |
| interp/sg/cc/b0/d3/L9/p64 | 13.2 MiB | 12.2 MiB | 0.923 | 2.8% / 2.4% | MEM- |
| interp/sg/cc/b0/d5/L6/p64 | 13.2 MiB | 12.4 MiB | 0.934 | 2.1% / 1.0% | MEM- |
| interp/sg/cc/b0/d7/L4/p16 | 14.3 MiB | 13.4 MiB | 0.937 | 3.0% / 2.5% | MEM- |
| interp/sg/cc/b0/d10/L2/p8 | 26.0 MiB | 24.0 MiB | 0.923 | 5.2% / 6.4% | MEM- |
| interp/sg/cheb1/b0/d3/L6/p64 | 13.1 MiB | 12.3 MiB | 0.936 | 1.4% / 1.7% | MEM- |
| interp/sg/cheb1/b0/d5/L4/p64 | 13.0 MiB | 12.2 MiB | 0.933 | 1.2% / 1.4% | MEM- |
| interp/sg/equi/b0/d3/L5/p64 | 13.1 MiB | 12.2 MiB | 0.933 | 1.1% / 2.2% | MEM- |
| interp/dsg/cc/b0/d2/L12/p64 | 17.1 MiB | 16.2 MiB | 0.946 | 2.9% / 2.3% | MEM- |
| interp/dsg/cc/b0/d3/L9/p64 | 13.6 MiB | 12.6 MiB | 0.927 | 2.3% / 2.4% | MEM- |
| interp/dsg/cc/b0/d5/L6/p64 | 13.4 MiB | 12.5 MiB | 0.933 | 0.8% / 3.7% | MEM- |
| interp/dsg/cc/b0/d7/L4/p16 | 13.3 MiB | 12.3 MiB | 0.929 | 2.1% / 2.0% | MEM- |
| interp/dsg/cc/b0/d10/L2/p8 | 13.1 MiB | 12.3 MiB | 0.940 | 1.9% / 2.9% | MEM- |
| interp/dsg/equi/b0/d3/L5/p64 | 13.0 MiB | 12.2 MiB | 0.941 | 1.8% / 3.2% | MEM- |
| interplin/sg/equi/b1/d2/L16/p16384 | 20.7 MiB | 19.8 MiB | 0.954 | 2.5% / 2.7% | MEM- |
| interplin/sg/equi/b1/d5/L8/p1024 | 14.2 MiB | 13.3 MiB | 0.938 | 3.1% / 3.3% | MEM- |
| interplin/sg/equi/b1/d8/L5/p256 | 14.1 MiB | 13.1 MiB | 0.929 | 4.9% / 1.9% | MEM- |
| interplin/sg/equi/b1/d10/L5/p32 | 14.4 MiB | 13.5 MiB | 0.935 | 6.5% / 7.6% | MEM- |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.4 MiB | 20.6 MiB | 0.964 | 2.3% / 1.8% | MEM- |
| interplin/dsg/equi/b1/d5/L8/p1024 | 14.1 MiB | 13.2 MiB | 0.937 | 0.8% / 2.6% | MEM- |
| interplin/dsg/equi/b1/d8/L5/p256 | 14.1 MiB | 13.2 MiB | 0.935 | 3.2% / 5.0% | MEM- |
| interplin/dsg/equi/b1/d10/L5/p32 | 18.1 MiB | 17.3 MiB | 0.957 | 2.8% / 4.2% | MEM- |

Peak memory (common cases): geomean B/A = 0.950; largest B/A: `quad/dsg/equi-exp/b1/d20/L6` 218.4 MiB -> 217.6 MiB (0.996x); 0 MEM+, 24 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 11.2 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 1.1%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 124, 124, 124, 124, 123 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 619s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
