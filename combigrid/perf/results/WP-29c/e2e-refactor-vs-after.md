A/B `e2e`: A=`WP-29c-after` B=`WP-29c-refactor`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 176.05ms | 175.52ms | 0.994 | [0.962, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 175.66ms | 175.14ms | 0.994 | [0.961, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 208.61us | 206.73us | 1.002 | [0.918, 1.095] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 244.20ms | 244.59ms | 1.002 | [0.982, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.05ms | 244.44ms | 1.002 | [0.982, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 75.29us | 72.42us | 1.047 | [0.916, 1.197] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.453s | 1.483s | 0.988 | [0.953, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.453s | 1.482s | 0.988 | [0.953, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 116.22us | 110.49us | 1.014 | [0.809, 1.272] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.271s | 1.257s | 1.004 | [0.991, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.271s | 1.257s | 1.004 | [0.991, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.77us | 67.19us | 0.980 | [0.875, 1.098] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.005s | 1.992s | 1.007 | [0.993, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.004s | 1.991s | 1.007 | [0.993, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.92us | 34.99us | 0.981 | [0.907, 1.061] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.55ms | 181.01ms | 0.996 | [0.975, 1.018] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.47ms | 180.92ms | 0.997 | [0.975, 1.019] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 27.66us | 28.43us | 0.617 | [0.163, 2.335] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 336.57ms | 338.76ms | 0.996 | [0.970, 1.022] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 336.47ms | 338.65ms | 0.996 | [0.970, 1.022] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 32.97us | 33.94us | 0.987 | [0.767, 1.271] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.97ms | 51.28ms | 0.992 | [0.967, 1.018] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.90ms | 51.21ms | 0.993 | [0.968, 1.018] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.53us | 22.98us | 0.890 | [0.713, 1.111] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.79ms | 108.56ms | 1.012 | [0.972, 1.053] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.63ms | 102.57ms | 1.012 | [0.971, 1.055] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.66ms | 1.63ms | 1.015 | [0.886, 1.164] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 448.46ms | 444.21ms | 1.009 | [0.983, 1.034] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 445.63ms | 440.34ms | 1.010 | [0.984, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.15ms | 2.33ms | 0.825 | [0.611, 1.114] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.541s | 1.642s | 0.981 | [0.890, 1.082] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.536s | 1.640s | 0.981 | [0.889, 1.082] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.44ms | 2.15ms | 1.176 | [0.777, 1.779] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.552s | 2.546s | 1.007 | [0.987, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.549s | 2.543s | 1.007 | [0.987, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.76ms | 1.81ms | 0.980 | [0.907, 1.058] | ~ (noise) |

Sum of medians (total, common cases): A = 10.369s, B = 10.463s, saved = -94147789.0ns (0.991x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d2/L12/p64`: A 1.63e-15 -> B 1.77e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 14.8 MiB | 0.991 | 4.0% / 5.9% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.1 MiB | 0.999 | 2.2% / 2.4% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.0 MiB | 0.994 | 1.5% / 2.0% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 12.8 MiB | 0.997 | 3.3% / 1.1% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 25.1 MiB | 24.5 MiB | 0.975 | 3.3% / 7.7% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 11.9 MiB | 0.989 | 2.1% / 1.6% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 11.9 MiB | 0.995 | 1.0% / 1.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.0 MiB | 1.014 | 2.1% / 1.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 19.7 MiB | 1.012 | 1.1% / 2.5% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.2 MiB | 1.020 | 3.7% / 3.1% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.9 MiB | 1.008 | 4.5% / 3.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.4 MiB | 13.6 MiB | 1.021 | 1.6% / 4.1% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.4 MiB -> 13.6 MiB (1.021x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.4%, 0.7%, 0.4%
Round durations (s): 44, 44, 44, 44, 44 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 221s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
