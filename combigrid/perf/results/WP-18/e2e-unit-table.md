A/B `e2e`: A=`WP-18-lazyunit` B=`WP-18-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 343.53ms | 367.93ms | 0.916 | [0.821, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 324.33ms | 322.99ms | 0.982 | [0.879, 1.097] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 19.17ms | 38.82ms | 0.539 | [0.415, 0.701] | SLOWER |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.138s | 1.131s | 0.988 | [0.915, 1.066] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.089s | 1.071s | 0.998 | [0.921, 1.082] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 48.70ms | 60.73ms | 0.797 | [0.664, 0.956] | SLOWER |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.29ms | 127.85ms | 0.999 | [0.981, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.01ms | 127.52ms | 0.999 | [0.981, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 207.43us | 224.96us | 0.944 | [0.865, 1.031] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 187.09ms | 186.51ms | 0.999 | [0.979, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 186.86ms | 186.37ms | 0.999 | [0.979, 1.020] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 172.51us | 89.53us | 1.943 | [1.804, 2.092] | faster |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.131s | 1.148s | 0.987 | [0.966, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.131s | 1.147s | 0.987 | [0.966, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 92.33us | 70.05us | 1.311 | [1.046, 1.644] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.003s | 1.010s | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.002s | 1.010s | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 68.58us | 69.00us | 0.992 | [0.868, 1.133] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.592s | 1.603s | 0.998 | [0.992, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.592s | 1.603s | 0.998 | [0.992, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 37.16us | 52.10us | 0.676 | [0.552, 0.828] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.30ms | 34.82ms | 0.995 | [0.951, 1.041] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.21ms | 34.74ms | 0.994 | [0.950, 1.040] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 47.14us | 34.36us | 1.345 | [0.940, 1.925] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.51ms | 120.54ms | 0.912 | [0.770, 1.080] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.43ms | 120.44ms | 0.912 | [0.770, 1.080] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 36.81us | 37.37us | 1.005 | [0.781, 1.293] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.87ms | 12.80ms | 0.822 | [0.552, 1.223] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.81ms | 12.73ms | 0.821 | [0.551, 1.224] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.65us | 32.90us | 0.707 | [0.587, 0.852] | SLOWER |
| interp/dsg/cc/b0/d2/L12/p64 | total | 51.87ms | 52.02ms | 1.009 | [0.981, 1.038] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.62ms | 51.68ms | 1.011 | [0.983, 1.040] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 119.71us | 216.16us | 0.555 | [0.543, 0.567] | SLOWER |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.21ms | 43.68ms | 1.023 | [0.843, 1.240] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 43.03ms | 43.54ms | 1.022 | [0.842, 1.240] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 98.83us | 61.53us | 1.576 | [1.375, 1.806] | faster |
| interp/dsg/cc/b0/d5/L6/p64 | total | 174.46ms | 174.06ms | 1.029 | [0.878, 1.206] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 174.35ms | 173.95ms | 1.029 | [0.878, 1.206] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 58.25us | 50.84us | 1.173 | [1.091, 1.262] | faster |
| interp/dsg/cc/b0/d7/L4/p16 | total | 151.20ms | 152.01ms | 0.984 | [0.937, 1.033] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 151.12ms | 151.93ms | 0.984 | [0.937, 1.033] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 46.02us | 45.12us | 1.012 | [0.911, 1.124] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 402.88ms | 399.15ms | 1.011 | [0.987, 1.035] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 402.81ms | 399.08ms | 1.011 | [0.987, 1.035] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 34.57us | 37.71us | 0.924 | [0.831, 1.028] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.57ms | 6.55ms | 1.192 | [0.747, 1.903] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.51ms | 6.48ms | 1.195 | [0.747, 1.911] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 22.00us | 26.47us | 0.864 | [0.713, 1.047] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 113.62ms | 111.54ms | 1.020 | [0.980, 1.061] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 107.88ms | 104.60ms | 1.031 | [0.985, 1.079] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.26ms | 2.27ms | 0.559 | [0.490, 0.637] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 362.32ms | 360.75ms | 1.010 | [0.974, 1.048] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 361.72ms | 360.21ms | 1.010 | [0.973, 1.048] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 92.89us | 95.75us | 0.992 | [0.870, 1.131] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.208s | 1.237s | 0.978 | [0.956, 1.001] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.206s | 1.235s | 0.978 | [0.956, 1.000] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.23ms | 2.21ms | 1.092 | [0.721, 1.652] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.883s | 1.884s | 0.999 | [0.974, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.882s | 1.883s | 0.999 | [0.974, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.56ms | 1.47ms | 0.972 | [0.850, 1.112] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 181.28ms | 178.66ms | 1.008 | [0.984, 1.034] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 176.35ms | 173.30ms | 1.011 | [0.985, 1.037] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 907.23us | 1.23ms | 0.735 | [0.639, 0.846] | SLOWER |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 182.57ms | 183.31ms | 1.029 | [0.879, 1.205] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 181.99ms | 182.77ms | 1.029 | [0.878, 1.205] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 83.25us | 88.63us | 1.073 | [0.797, 1.445] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 379.92ms | 379.79ms | 0.994 | [0.920, 1.074] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 375.65ms | 377.42ms | 0.993 | [0.920, 1.072] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.10ms | 2.10ms | 1.164 | [0.761, 1.782] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 442.67ms | 429.42ms | 1.046 | [0.982, 1.115] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 441.14ms | 427.94ms | 1.048 | [0.981, 1.119] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.40ms | 1.47ms | 0.804 | [0.474, 1.364] | ~ (noise) |

Sum of medians (total, common cases): A = 11.270s, B = 11.334s, saved = -64289203.0ns (0.994x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d2/L20`: A 7.18e-14 -> B 7.28e-14 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 339.8 MiB | 336.1 MiB | 0.989 | 8.4% / 7.2% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 845.4 MiB | 864.8 MiB | 1.023 | 4.3% / 2.8% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.6 MiB | 0.986 | 1.7% / 3.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.5 MiB | 12.4 MiB | 0.992 | 2.7% / 1.3% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.3 MiB | 1.001 | 0.9% / 1.9% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.3 MiB | 12.2 MiB | 0.991 | 1.5% / 1.1% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 0.997 | 2.6% / 1.6% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.5 MiB | 12.4 MiB | 0.988 | 1.8% / 3.8% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.5 MiB | 12.4 MiB | 0.994 | 2.1% / 2.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.4 MiB | 1.003 | 1.9% / 0.9% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 15.3 MiB | 15.5 MiB | 1.010 | 3.8% / 2.3% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.7 MiB | 12.7 MiB | 1.002 | 0.8% / 2.1% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.5 MiB | 12.5 MiB | 1.001 | 2.4% / 1.7% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.6 MiB | 12.5 MiB | 0.994 | 1.2% / 1.2% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.5 MiB | 12.5 MiB | 1.000 | 1.6% / 0.6% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.3 MiB | 0.992 | 0.8% / 1.9% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 22.5 MiB | 20.4 MiB | 0.905 | 5.7% / 10.0% | MEM- |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 12.8 MiB | 0.986 | 2.5% / 4.6% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.7 MiB | 0.996 | 3.5% / 2.0% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.9 MiB | 12.6 MiB | 0.977 | 2.1% / 2.6% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 20.9 MiB | 21.5 MiB | 1.029 | 3.1% / 1.7% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.0 MiB | 0.999 | 3.8% / 4.0% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.8 MiB | 1.008 | 4.0% / 4.8% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 15.3 MiB | 15.3 MiB | 0.999 | 3.3% / 3.3% | ~ |

Peak memory (common cases): geomean B/A = 0.994; largest B/A: `interplin/dsg/equi/b1/d2/L16/p16384` 20.9 MiB -> 21.5 MiB (1.029x); 0 MEM+, 1 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.4%, 0.4%, 0.4%
Round durations (s): 52, 51, 51, 52, 51 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 258s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
