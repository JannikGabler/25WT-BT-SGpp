A/B `e2e`: A=`WP-45-before` B=`WP-45-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 97.73ms | 97.46ms | 1.004 | [0.999, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 96.15ms | 96.14ms | 1.004 | [0.998, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.13ms | 1.07ms | 1.004 | [0.821, 1.229] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.96ms | 202.08ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.44ms | 199.39ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.14ms | 1.19ms | 0.948 | [0.735, 1.224] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.146s | 1.146s | 1.002 | [0.996, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.139s | 1.137s | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.20ms | 3.19ms | 1.047 | [0.641, 1.709] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.409s | 5.410s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.400s | 5.402s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.40ms | 3.45ms | 0.985 | [0.742, 1.307] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.238s | 1.237s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.163s | 1.165s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 44.79ms | 43.45ms | 1.031 | [0.884, 1.204] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.220s | 1.223s | 0.999 | [0.992, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.118s | 1.121s | 1.000 | [0.994, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.02ms | 61.49ms | 0.973 | [0.881, 1.074] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.47ms | 248.20ms | 0.996 | [0.992, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 243.35ms | 244.54ms | 0.996 | [0.992, 1.000] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.30ms | 2.38ms | 0.902 | [0.764, 1.065] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.433s | 1.435s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.423s | 1.426s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.07ms | 4.99ms | 0.994 | [0.781, 1.264] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.833s | 6.836s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.824s | 6.828s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.95ms | 4.60ms | 0.893 | [0.584, 1.365] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 173.62ms | 174.71ms | 0.976 | [0.937, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 173.25ms | 174.32ms | 0.976 | [0.937, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 210.71us | 206.03us | 1.018 | [0.932, 1.112] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 242.46ms | 242.69ms | 0.982 | [0.940, 1.026] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 242.31ms | 242.55ms | 0.982 | [0.940, 1.026] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.33us | 72.64us | 0.981 | [0.863, 1.114] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.443s | 1.440s | 1.007 | [0.991, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.442s | 1.440s | 1.007 | [0.991, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.04us | 86.81us | 1.173 | [0.918, 1.498] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.257s | 1.255s | 1.003 | [0.980, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.257s | 1.255s | 1.003 | [0.980, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 62.23us | 63.28us | 1.024 | [0.885, 1.184] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.973s | 1.971s | 1.002 | [0.985, 1.019] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.973s | 1.971s | 1.002 | [0.985, 1.019] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.13us | 35.27us | 0.984 | [0.931, 1.041] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.45ms | 181.31ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.29ms | 181.22ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.43us | 29.12us | 1.291 | [0.595, 2.799] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 336.40ms | 343.00ms | 0.977 | [0.964, 0.990] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 336.29ms | 342.88ms | 0.977 | [0.964, 0.990] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 35.06us | 33.80us | 1.017 | [0.790, 1.309] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.72ms | 51.02ms | 0.998 | [0.987, 1.010] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.65ms | 50.95ms | 0.998 | [0.987, 1.010] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.35us | 21.37us | 1.030 | [0.885, 1.198] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.31ms | 109.26ms | 1.040 | [0.933, 1.161] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.31ms | 103.08ms | 1.043 | [0.929, 1.171] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.62ms | 1.66ms | 0.981 | [0.903, 1.067] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 365.99ms | 358.10ms | 1.035 | [0.990, 1.082] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 363.09ms | 355.10ms | 1.036 | [0.992, 1.083] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.14ms | 2.28ms | 0.862 | [0.555, 1.338] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.209s | 1.203s | 1.002 | [0.979, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.206s | 1.199s | 1.002 | [0.980, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.26ms | 2.20ms | 0.931 | [0.615, 1.409] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.858s | 1.846s | 1.010 | [0.990, 1.031] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.855s | 1.843s | 1.010 | [0.990, 1.031] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.72ms | 1.052 | [0.973, 1.136] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 185.42ms | 183.78ms | 1.007 | [0.989, 1.026] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 180.45ms | 178.85ms | 1.007 | [0.988, 1.026] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.21ms | 1.16ms | 1.061 | [0.838, 1.343] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 188.67ms | 185.02ms | 1.020 | [0.977, 1.065] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 185.84ms | 182.37ms | 1.018 | [0.977, 1.061] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 2.11ms | 2.11ms | 1.110 | [0.766, 1.607] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 374.44ms | 376.84ms | 0.986 | [0.945, 1.028] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 371.78ms | 374.12ms | 0.984 | [0.945, 1.024] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.23ms | 1.96ms | 1.283 | [0.680, 2.421] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 432.20ms | 430.94ms | 1.009 | [0.991, 1.028] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 428.95ms | 428.89ms | 1.006 | [0.990, 1.022] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 3.64ms | 1.49ms | 1.729 | [0.963, 3.104] | ~ (noise) |

Sum of medians (total, common cases): A = 28.206s, B = 28.187s, saved = 19.25ms (1.001x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.13e-15 -> B 1.81e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.7 MiB | 62.6 MiB | 0.997 | 3.4% / 2.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.2 MiB | 26.4 MiB | 1.004 | 4.7% / 3.9% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.9 MiB | 41.9 MiB | 0.976 | 2.6% / 6.0% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.4 MiB | 42.5 MiB | 1.001 | 1.3% / 4.0% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 152.7 MiB | 150.4 MiB | 0.985 | 3.2% / 2.6% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.4 MiB | 217.5 MiB | 1.001 | 0.3% / 0.7% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.9 MiB | 26.0 MiB | 1.004 | 3.0% / 5.8% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.4 MiB | 42.4 MiB | 1.002 | 3.4% / 6.8% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 43.2 MiB | 42.7 MiB | 0.988 | 4.3% / 2.7% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 15.1 MiB | 1.020 | 2.2% / 3.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.3 MiB | 1.003 | 0.7% / 0.8% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.4 MiB | 12.4 MiB | 1.001 | 1.9% / 1.7% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.2 MiB | 13.1 MiB | 0.991 | 3.2% / 2.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.8 MiB | 24.4 MiB | 0.982 | 3.7% / 6.5% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.2 MiB | 0.993 | 3.8% / 1.6% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.1 MiB | 12.3 MiB | 1.012 | 1.8% / 2.0% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.3 MiB | 1.003 | 1.3% / 2.0% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.7 MiB | 0.996 | 2.1% / 1.7% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.3 MiB | 0.997 | 6.0% / 3.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.2 MiB | 13.0 MiB | 0.985 | 2.8% / 2.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.8 MiB | 13.7 MiB | 0.995 | 7.0% / 4.3% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 20.7 MiB | 20.7 MiB | 1.002 | 1.9% / 1.8% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.3 MiB | 1.023 | 2.8% / 1.2% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 13.2 MiB | 13.1 MiB | 0.998 | 3.5% / 3.1% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 16.9 MiB | 17.2 MiB | 1.013 | 2.4% / 3.5% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `interplin/dsg/equi/b1/d5/L8/p1024` 13.0 MiB -> 13.3 MiB (1.023x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.6 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 119, 119, 118, 119, 118 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 593s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
