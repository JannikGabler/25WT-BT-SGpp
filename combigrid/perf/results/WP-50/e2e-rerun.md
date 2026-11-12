A/B `e2e`: A=`WP-50-before` B=`WP-50-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.75ms | 96.93ms | 1.000 | [0.985, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.15ms | 95.58ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.10ms | 1.07ms | 0.985 | [0.805, 1.205] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.22ms | 211.30ms | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.33ms | 208.25ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.49ms | 1.38ms | 0.959 | [0.590, 1.559] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.172s | 1.182s | 0.993 | [0.989, 0.998] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.164s | 1.174s | 0.993 | [0.989, 0.998] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.04ms | 3.04ms | 1.013 | [0.992, 1.034] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.363s | 5.363s | 1.003 | [0.996, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.354s | 5.355s | 1.003 | [0.996, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.22ms | 3.31ms | 0.988 | [0.744, 1.311] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.222s | 1.226s | 0.997 | [0.993, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.151s | 1.153s | 0.998 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 45.64ms | 43.47ms | 1.036 | [0.888, 1.209] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.212s | 1.215s | 1.002 | [0.993, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.112s | 1.112s | 1.005 | [0.993, 1.018] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.99ms | 62.52ms | 0.960 | [0.855, 1.077] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.69ms | 249.73ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.91ms | 246.09ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.46ms | 2.38ms | 0.997 | [0.831, 1.197] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.441s | 1.443s | 0.998 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.431s | 1.433s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.02ms | 4.95ms | 0.882 | [0.707, 1.100] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.849s | 6.846s | 1.003 | [0.994, 1.012] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.839s | 6.837s | 1.003 | [0.994, 1.012] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.61ms | 3.95ms | 1.097 | [0.717, 1.676] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 175.33ms | 173.91ms | 1.032 | [0.981, 1.085] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 174.93ms | 173.57ms | 1.032 | [0.981, 1.085] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 219.09us | 207.64us | 1.029 | [0.942, 1.124] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 242.66ms | 242.71ms | 0.988 | [0.952, 1.026] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 242.52ms | 242.56ms | 0.988 | [0.952, 1.026] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 72.98us | 74.10us | 0.988 | [0.918, 1.064] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.437s | 1.440s | 0.999 | [0.994, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.436s | 1.439s | 0.999 | [0.994, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 85.91us | 96.73us | 0.973 | [0.776, 1.220] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.258s | 1.254s | 1.002 | [0.989, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.258s | 1.254s | 1.002 | [0.989, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 61.53us | 62.37us | 0.993 | [0.928, 1.061] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.978s | 1.985s | 0.998 | [0.983, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.978s | 1.984s | 0.998 | [0.983, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 33.17us | 33.32us | 1.007 | [0.952, 1.066] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.18ms | 181.56ms | 1.002 | [0.996, 1.009] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 182.09ms | 181.49ms | 1.002 | [0.996, 1.009] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 29.54us | 27.59us | 1.142 | [0.784, 1.663] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 343.00ms | 344.16ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 342.90ms | 344.05ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 35.83us | 34.78us | 1.052 | [0.818, 1.354] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.86ms | 51.18ms | 0.973 | [0.912, 1.039] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.79ms | 51.11ms | 0.973 | [0.912, 1.039] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.02us | 22.35us | 0.899 | [0.772, 1.046] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.25ms | 52.33ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.93ms | 52.02ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 138.91us | 136.82us | 1.034 | [0.967, 1.106] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 42.83ms | 43.23ms | 0.989 | [0.972, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.64ms | 43.02ms | 0.989 | [0.972, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 66.98us | 66.63us | 0.953 | [0.838, 1.084] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 171.21ms | 172.08ms | 0.997 | [0.983, 1.010] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 170.96ms | 171.83ms | 0.997 | [0.983, 1.010] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 78.22us | 77.80us | 0.977 | [0.880, 1.084] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 149.36ms | 149.22ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 149.19ms | 149.06ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 59.57us | 58.53us | 1.151 | [0.969, 1.367] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 395.70ms | 393.43ms | 1.003 | [0.996, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 395.62ms | 393.34ms | 1.003 | [0.996, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 34.29us | 42.81us | 0.567 | [0.160, 2.017] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.61ms | 6.50ms | 1.027 | [0.983, 1.072] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.54ms | 6.42ms | 1.027 | [0.983, 1.073] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 21.37us | 22.70us | 0.964 | [0.867, 1.072] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 107.55ms | 110.88ms | 0.977 | [0.934, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 101.36ms | 104.60ms | 0.975 | [0.929, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.67ms | 1.70ms | 0.998 | [0.918, 1.085] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 357.63ms | 354.23ms | 1.009 | [0.984, 1.035] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 354.76ms | 350.34ms | 1.010 | [0.983, 1.036] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.25ms | 2.24ms | 0.922 | [0.511, 1.661] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.208s | 1.224s | 0.981 | [0.952, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.205s | 1.222s | 0.981 | [0.951, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.21ms | 2.13ms | 1.166 | [0.771, 1.765] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.841s | 1.852s | 0.995 | [0.975, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.839s | 1.850s | 0.995 | [0.975, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.73ms | 1.56ms | 1.093 | [0.989, 1.207] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 185.28ms | 184.77ms | 1.002 | [0.995, 1.010] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 180.40ms | 179.84ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.25ms | 1.17ms | 1.020 | [0.926, 1.124] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 186.55ms | 186.76ms | 0.998 | [0.984, 1.012] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 183.49ms | 183.83ms | 0.994 | [0.978, 1.011] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 2.01ms | 2.10ms | 1.258 | [0.836, 1.891] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 374.81ms | 374.37ms | 1.002 | [0.995, 1.008] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 372.22ms | 371.98ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.23ms | 2.01ms | 1.078 | [0.969, 1.200] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 432.55ms | 431.64ms | 1.003 | [0.995, 1.012] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 428.38ms | 428.22ms | 1.002 | [0.993, 1.012] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.76ms | 1.57ms | 1.188 | [0.745, 1.894] | ~ (noise) |

Sum of medians (total, common cases): A = 28.995s, B = 29.041s, saved = -45876745.0ns (0.998x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 3.95e-10 -> B 8.16e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.8 MiB | 62.4 MiB | 0.994 | 2.5% / 1.1% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.5 MiB | 26.4 MiB | 0.996 | 2.2% / 1.0% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.8 MiB | 42.7 MiB | 0.998 | 4.6% / 5.2% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.8 MiB | 42.5 MiB | 1.016 | 3.0% / 1.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.5 MiB | 152.0 MiB | 1.010 | 4.0% / 4.4% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.8 MiB | 217.6 MiB | 0.999 | 0.4% / 1.3% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.9 MiB | 25.8 MiB | 0.993 | 2.8% / 2.1% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.4 MiB | 42.5 MiB | 1.002 | 2.7% / 4.4% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.8 MiB | 43.1 MiB | 1.007 | 2.2% / 1.9% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.1 MiB | 15.4 MiB | 1.019 | 4.4% / 4.4% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.3 MiB | 1.002 | 2.9% / 2.3% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.3 MiB | 0.997 | 3.9% / 2.4% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.2 MiB | 13.2 MiB | 0.999 | 2.8% / 2.8% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.6 MiB | 24.2 MiB | 0.984 | 3.3% / 4.0% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.1 MiB | 12.1 MiB | 1.004 | 3.4% / 2.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.1 MiB | 0.994 | 1.0% / 2.1% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.1 MiB | 12.2 MiB | 1.005 | 2.4% / 0.6% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.2 MiB | 16.2 MiB | 1.000 | 3.8% / 4.6% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.6 MiB | 12.6 MiB | 0.996 | 1.1% / 1.5% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.4 MiB | 12.5 MiB | 1.011 | 2.9% / 1.6% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.5 MiB | 1.003 | 1.6% / 2.4% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.2 MiB | 0.995 | 2.0% / 0.9% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.1 MiB | 12.2 MiB | 1.002 | 1.4% / 2.3% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.7 MiB | 1.002 | 2.8% / 2.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 13.1 MiB | 0.991 | 4.1% / 4.0% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 12.8 MiB | 0.994 | 2.3% / 3.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.5 MiB | 13.6 MiB | 1.009 | 4.2% / 3.2% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 20.7 MiB | 20.6 MiB | 0.992 | 1.6% / 2.1% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.0 MiB | 0.993 | 2.7% / 2.6% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 13.1 MiB | 13.2 MiB | 1.009 | 2.1% / 2.5% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 16.9 MiB | 17.1 MiB | 1.012 | 4.1% / 2.1% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `interp/sg/cc/b0/d2/L12/p64` 15.1 MiB -> 15.4 MiB (1.019x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.0 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.1%, 0.2%, 0.2%
Round durations (s): 123, 123, 123, 123, 123 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 616s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
