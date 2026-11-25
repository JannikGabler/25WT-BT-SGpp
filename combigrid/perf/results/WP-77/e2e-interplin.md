A/B `e2e`: A=`WP-77-before` B=`WP-77-after`, 16 thread(s), 12 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 101.30ms | 104.19ms | 0.959 | [0.923, 0.997] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 94.58ms | 97.36ms | 0.956 | [0.918, 0.997] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.29ms | 2.25ms | 1.027 | [0.981, 1.074] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 364.46ms | 369.85ms | 0.987 | [0.974, 0.999] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 363.93ms | 369.30ms | 0.987 | [0.974, 1.000] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 81.19us | 96.59us | 0.843 | [0.790, 0.900] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.198s | 1.195s | 1.006 | [0.994, 1.018] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.196s | 1.192s | 1.006 | [0.994, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.05ms | 2.06ms | 1.041 | [0.838, 1.293] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.840s | 1.817s | 1.013 | [1.003, 1.024] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.839s | 1.816s | 1.013 | [1.003, 1.024] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.54ms | 1.52ms | 0.989 | [0.836, 1.171] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 824.77ms | 819.65ms | 1.007 | [0.993, 1.021] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 824.71ms | 819.57ms | 1.007 | [0.993, 1.021] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 36.25us | 48.29us | 0.730 | [0.673, 0.792] | SLOWER |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.288s | 1.092s | 1.180 | [1.161, 1.200] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.288s | 1.092s | 1.180 | [1.161, 1.200] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 48.68us | 52.90us | 0.908 | [0.837, 0.986] | SLOWER |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.072s | 1.060s | 1.013 | [0.953, 1.077] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.070s | 1.058s | 1.013 | [0.953, 1.078] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.10ms | 2.14ms | 0.914 | [0.713, 1.173] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 243.45ms | 166.21ms | 1.468 | [1.401, 1.538] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 243.33ms | 166.11ms | 1.468 | [1.401, 1.539] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 36.04us | 37.61us | 0.965 | [0.894, 1.041] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 152.19ms | 142.21ms | 1.069 | [1.041, 1.099] | faster |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 143.76ms | 133.27ms | 1.079 | [1.049, 1.110] | faster |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 57.34us | 62.09us | 0.922 | [0.872, 0.976] | SLOWER |

Sum of medians (total, common cases, group suite): A = 3.504s, B = 3.486s, saved = 18.45ms (1.005x)
Sum of medians (total, common cases, group expensive/): A = 3.186s, B = 2.972s, saved = 213.32ms (1.072x)
Sum of medians (total, common cases, group expensive/ cached): A = 395.64ms, B = 308.43ms, saved = 87.21ms (1.283x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d2/L16/p16384`: A 1.92e-10 -> B 1.92e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 20.3 MiB | 20.8 MiB | 1.023 | 8.9% / 9.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.9 MiB | 13.0 MiB | 1.007 | 3.1% / 2.9% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.8 MiB | 0.999 | 3.0% / 3.1% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.7 MiB | 12.8 MiB | 1.008 | 3.8% / 4.6% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.2 MiB | 12.3 MiB | 1.010 | 2.8% / 3.5% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.2 MiB | 12.3 MiB | 1.008 | 3.4% / 3.2% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.7 MiB | 12.8 MiB | 1.008 | 2.4% / 2.4% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.5 MiB | 13.7 MiB | 1.011 | 3.7% / 4.1% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.6 MiB | 59.5 MiB | 0.999 | 1.0% / 0.9% | ~ |

Peak memory (common cases): geomean B/A = 1.008; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 20.3 MiB -> 20.8 MiB (1.023x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.6 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 356s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
