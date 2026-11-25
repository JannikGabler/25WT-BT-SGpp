A/B `e2e`: A=`WP-77-after` B=`WP-77-interplin-dyn`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 100.20ms | 104.08ms | 0.977 | [0.909, 1.049] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 93.43ms | 97.22ms | 0.974 | [0.903, 1.051] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.36ms | 2.26ms | 1.045 | [0.961, 1.136] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 364.98ms | 371.14ms | 0.981 | [0.957, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 364.43ms | 370.61ms | 0.981 | [0.956, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 95.40us | 97.78us | 0.991 | [0.872, 1.127] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.202s | 1.195s | 1.009 | [0.986, 1.032] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.200s | 1.192s | 1.009 | [0.986, 1.031] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.01ms | 2.23ms | 0.921 | [0.609, 1.394] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.851s | 1.818s | 1.020 | [0.999, 1.041] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.849s | 1.816s | 1.020 | [0.999, 1.041] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.64ms | 1.58ms | 1.208 | [0.799, 1.826] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 830.79ms | 820.30ms | 1.012 | [0.984, 1.040] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 830.71ms | 820.22ms | 1.012 | [0.984, 1.040] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 49.87us | 49.17us | 0.957 | [0.808, 1.135] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.299s | 1.094s | 1.186 | [1.149, 1.225] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.299s | 1.094s | 1.186 | [1.149, 1.225] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 53.08us | 51.89us | 1.034 | [0.919, 1.164] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.076s | 1.061s | 1.011 | [0.896, 1.142] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.072s | 1.059s | 1.011 | [0.895, 1.143] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.24ms | 2.25ms | 1.066 | [0.648, 1.754] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 243.99ms | 166.00ms | 1.465 | [1.336, 1.607] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 243.84ms | 165.86ms | 1.466 | [1.337, 1.607] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 38.69us | 37.09us | 1.046 | [0.900, 1.216] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 152.80ms | 142.06ms | 1.074 | [1.043, 1.105] | faster |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 144.27ms | 133.72ms | 1.081 | [1.056, 1.107] | faster |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 63.41us | 61.53us | 1.023 | [0.984, 1.064] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 3.518s, B = 3.488s, saved = 30.25ms (1.009x)
Sum of medians (total, common cases, group expensive/): A = 3.206s, B = 2.975s, saved = 230.49ms (1.077x)
Sum of medians (total, common cases, group expensive/ cached): A = 396.80ms, B = 308.05ms, saved = 88.75ms (1.288x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d2/L16/p16384`: A 1.92e-10 -> B 1.92e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 19.8 MiB | 20.4 MiB | 1.031 | 6.3% / 7.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.9 MiB | 12.7 MiB | 0.989 | 0.9% / 2.6% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.8 MiB | 1.004 | 0.9% / 3.4% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.8 MiB | 12.7 MiB | 0.986 | 2.7% / 2.3% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.2 MiB | 12.2 MiB | 1.000 | 1.0% / 2.1% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.3 MiB | 12.1 MiB | 0.986 | 1.8% / 2.8% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.8 MiB | 12.7 MiB | 0.995 | 4.1% / 2.3% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.5 MiB | 13.7 MiB | 1.019 | 3.4% / 5.6% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.3 MiB | 59.5 MiB | 1.003 | 0.5% / 1.0% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 19.8 MiB -> 20.4 MiB (1.031x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 30, 30, 30, 30, 30 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 149s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
