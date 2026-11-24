A/B `e2e`: A=`WP-48-after` B=`WP-48-vector`, 16 thread(s), 3 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 128.33ms | 128.20ms | 1.000 | [0.964, 1.038] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.07ms | 127.95ms | 1.000 | [0.964, 1.038] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 214.69us | 211.34us | 0.980 | [0.821, 1.168] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 185.95ms | 185.68ms | 0.999 | [0.959, 1.041] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 185.81ms | 185.53ms | 0.999 | [0.959, 1.041] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 88.14us | 88.77us | 0.990 | [0.853, 1.147] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.136s | 1.134s | 1.007 | [0.991, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.136s | 1.133s | 1.007 | [0.991, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 83.60us | 71.94us | 1.184 | [0.752, 1.862] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.009s | 1.009s | 1.001 | [0.976, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.009s | 1.009s | 1.001 | [0.976, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 70.75us | 60.48us | 1.217 | [0.918, 1.614] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.613s | 1.610s | 1.003 | [0.993, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.613s | 1.609s | 1.003 | [0.993, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 62.93us | 53.01us | 1.140 | [0.775, 1.676] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 33.95ms | 34.27ms | 0.993 | [0.965, 1.022] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 33.86ms | 34.18ms | 0.993 | [0.965, 1.022] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 41.14us | 43.44us | 0.950 | [0.464, 1.946] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.99ms | 117.18ms | 1.005 | [0.973, 1.038] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.90ms | 117.10ms | 1.005 | [0.973, 1.038] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 31.50us | 31.50us | 1.017 | [0.614, 1.685] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 12.23ms | 11.85ms | 1.185 | [0.578, 2.432] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 12.15ms | 11.78ms | 1.187 | [0.577, 2.441] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 34.29us | 38.62us | 0.870 | [0.627, 1.207] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 100.96ms | 102.70ms | 0.981 | [0.910, 1.058] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 94.18ms | 95.94ms | 0.978 | [0.901, 1.063] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.39ms | 2.38ms | 1.025 | [0.867, 1.211] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 368.28ms | 367.12ms | 0.999 | [0.901, 1.108] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 367.76ms | 366.56ms | 0.999 | [0.901, 1.108] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 79.90us | 84.86us | 0.937 | [0.725, 1.211] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.196s | 1.203s | 0.994 | [0.949, 1.041] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.193s | 1.201s | 0.994 | [0.950, 1.039] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.05ms | 2.04ms | 1.051 | [0.459, 2.407] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.837s | 1.843s | 0.985 | [0.924, 1.050] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.835s | 1.841s | 0.985 | [0.924, 1.050] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.42ms | 1.55ms | 0.927 | [0.770, 1.115] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | 1.073s | 1.073s | 0.998 | [0.971, 1.026] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | operator | 1.073s | 1.073s | 0.998 | [0.971, 1.026] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | generation | 75.01us | 58.04us | 1.163 | [0.625, 2.162] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 829.55ms | 846.42ms | 0.984 | [0.931, 1.039] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 829.48ms | 846.36ms | 0.984 | [0.931, 1.039] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 51.47us | 38.55us | 1.342 | [1.076, 1.673] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | 399.12ms | 397.87ms | 1.015 | [0.958, 1.075] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | operator | 399.07ms | 397.79ms | 1.015 | [0.958, 1.075] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | generation | 31.01us | 38.20us | 0.775 | [0.529, 1.137] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.291s | 1.291s | 1.001 | [0.939, 1.068] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.291s | 1.291s | 1.001 | [0.939, 1.068] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 53.85us | 61.95us | 0.928 | [0.702, 1.226] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | 257.76ms | 257.89ms | 1.022 | [0.686, 1.523] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | operator | 255.63ms | 255.74ms | 1.025 | [0.696, 1.509] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.08ms | 2.11ms | 0.815 | [0.292, 2.269] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.075s | 1.082s | 0.996 | [0.782, 1.270] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.073s | 1.077s | 0.998 | [0.781, 1.274] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 1.98ms | 3.65ms | 0.596 | [0.207, 1.715] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | 70.83ms | 70.50ms | 1.004 | [0.997, 1.012] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | operator | 70.73ms | 70.40ms | 1.004 | [0.997, 1.012] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | generation | 44.98us | 46.30us | 0.943 | [0.651, 1.366] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 243.30ms | 243.26ms | 1.004 | [0.835, 1.208] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 243.17ms | 243.16ms | 1.004 | [0.835, 1.207] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 33.87us | 36.18us | 0.938 | [0.695, 1.267] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 155.61ms | 155.21ms | 0.970 | [0.797, 1.180] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 146.54ms | 146.37ms | 0.972 | [0.793, 1.191] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 56.43us | 56.22us | 1.094 | [0.750, 1.596] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 7.738s, B = 7.745s, saved = -7140600.0ns (0.999x)
Sum of medians (total, common cases, group expensive/): A = 4.925s, B = 4.948s, saved = -22529827.0ns (0.995x)
Sum of medians (total, common cases, group expensive/ cached): A = 469.75ms, B = 468.98ms, saved = 773.91us (1.002x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d2/L12/p64`: A 1.90e-15 -> B 1.90e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.7 MiB | 0.991 | 1.5% / 1.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.5 MiB | 12.5 MiB | 1.001 | 1.3% / 0.3% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.4 MiB | 12.3 MiB | 0.995 | 0.7% / 0.8% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.3 MiB | 0.993 | 1.3% / 1.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.4 MiB | 12.4 MiB | 1.000 | 1.9% / 1.8% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.4 MiB | 12.4 MiB | 0.999 | 1.8% / 0.6% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.4 MiB | 12.4 MiB | 0.996 | 0.9% / 0.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.4 MiB | 1.004 | 0.5% / 1.6% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 20.0 MiB | 21.1 MiB | 1.056 | 6.8% / 0.6% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 12.8 MiB | 0.984 | 0.5% / 1.1% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 12.6 MiB | 0.980 | 0.1% / 1.6% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.9 MiB | 12.7 MiB | 0.982 | 1.9% / 1.6% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 12.4 MiB | 12.2 MiB | 0.984 | 1.8% / 1.5% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.3 MiB | 12.2 MiB | 0.992 | 0.8% / 2.2% | ~ |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 12.3 MiB | 12.5 MiB | 1.017 | 1.7% / 1.1% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.3 MiB | 12.2 MiB | 0.987 | 3.9% / 1.0% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 13.0 MiB | 12.9 MiB | 0.989 | 3.0% / 1.6% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 13.0 MiB | 12.8 MiB | 0.991 | 1.8% / 1.7% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 13.8 MiB | 13.6 MiB | 0.987 | 2.2% / 2.0% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.4 MiB | 13.4 MiB | 0.997 | 0.7% / 1.7% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.6 MiB | 59.4 MiB | 0.996 | 0.5% / 0.3% | ~ |

Peak memory (common cases): geomean B/A = 0.996; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 20.0 MiB -> 21.1 MiB (1.056x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%
Round durations (s): 57, 57, 57 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 171s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
