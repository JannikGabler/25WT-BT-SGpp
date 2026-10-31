A/B `e2e`: A=`WP-29c-before` B=`WP-29c-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 175.99ms | 176.64ms | 0.993 | [0.974, 1.011] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 175.61ms | 176.27ms | 0.992 | [0.974, 1.011] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 208.61us | 222.72us | 0.959 | [0.862, 1.066] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 244.20ms | 244.70ms | 1.002 | [0.982, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.06ms | 244.55ms | 1.002 | [0.982, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 74.03us | 75.50us | 0.997 | [0.836, 1.190] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.445s | 1.447s | 0.998 | [0.991, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.445s | 1.447s | 0.998 | [0.991, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 101.55us | 109.72us | 0.933 | [0.744, 1.170] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.266s | 1.262s | 1.006 | [0.993, 1.019] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.266s | 1.262s | 1.006 | [0.993, 1.019] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 65.30us | 67.19us | 0.953 | [0.862, 1.054] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.971s | 2.007s | 0.987 | [0.971, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.971s | 2.007s | 0.987 | [0.971, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.29us | 34.85us | 0.733 | [0.300, 1.790] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.07ms | 180.85ms | 1.005 | [1.000, 1.011] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.96ms | 180.77ms | 1.005 | [1.000, 1.011] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.26us | 28.15us | 1.101 | [0.769, 1.575] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 344.03ms | 338.24ms | 1.018 | [1.005, 1.031] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 343.92ms | 338.13ms | 1.018 | [1.005, 1.031] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.36us | 34.85us | 1.090 | [0.847, 1.402] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.21ms | 51.19ms | 0.999 | [0.987, 1.012] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.12ms | 51.12ms | 0.999 | [0.987, 1.012] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.53us | 21.37us | 1.100 | [0.764, 1.585] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.36ms | 108.76ms | 1.016 | [0.978, 1.055] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 105.29ms | 102.98ms | 1.017 | [0.976, 1.060] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.68ms | 1.65ms | 1.029 | [0.926, 1.145] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 444.07ms | 444.07ms | 1.019 | [0.966, 1.074] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 441.33ms | 441.31ms | 1.019 | [0.966, 1.075] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.04ms | 2.20ms | 0.949 | [0.835, 1.079] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.532s | 1.525s | 1.004 | [0.982, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.529s | 1.522s | 1.004 | [0.982, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.23ms | 2.14ms | 0.989 | [0.654, 1.497] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.469s | 2.481s | 1.001 | [0.980, 1.021] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.466s | 2.479s | 1.001 | [0.980, 1.021] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.82ms | 1.77ms | 1.054 | [0.898, 1.237] | ~ (noise) |

Sum of medians (total, common cases): A = 10.235s, B = 10.266s, saved = -30805076.0ns (0.997x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.02e-15 -> B 1.92e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 14.8 MiB | 0.995 | 3.0% / 2.6% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.0 MiB | 0.994 | 2.3% / 1.6% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.1 MiB | 1.003 | 2.3% / 1.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 13.0 MiB | 1.002 | 5.5% / 2.3% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.5 MiB | 25.0 MiB | 1.019 | 4.6% / 5.5% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 11.9 MiB | 1.003 | 3.4% / 1.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 12.0 MiB | 1.008 | 0.3% / 0.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.8 MiB | 0.992 | 2.8% / 1.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.5 MiB | 0.990 | 0.5% / 3.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.9 MiB | 13.0 MiB | 1.010 | 2.8% / 2.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.8 MiB | 1.010 | 3.1% / 3.7% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.3 MiB | 1.009 | 6.7% / 4.0% | ~ |

Peak memory (common cases): geomean B/A = 1.003; largest B/A: `interp/sg/cc/b0/d10/L2/p8` 24.5 MiB -> 25.0 MiB (1.019x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.4%, 0.3%, 0.2%, 0.2%
Round durations (s): 44, 44, 44, 44, 44 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 219s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
