A/B `e2e`: A=`WP-49-before` B=`WP-49-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.15ms | 114.04ms | 1.013 | [0.838, 1.225] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.31ms | 107.06ms | 1.014 | [0.830, 1.239] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.39ms | 2.38ms | 0.988 | [0.934, 1.045] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 356.08ms | 359.45ms | 0.981 | [0.958, 1.004] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 355.53ms | 358.89ms | 0.981 | [0.958, 1.004] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 80.39us | 82.90us | 0.976 | [0.895, 1.064] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.192s | 1.202s | 0.995 | [0.979, 1.010] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.189s | 1.199s | 0.994 | [0.980, 1.010] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.06ms | 2.02ms | 1.046 | [0.732, 1.494] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.849s | 1.840s | 1.003 | [0.989, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.847s | 1.838s | 1.003 | [0.989, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.57ms | 1.55ms | 0.902 | [0.560, 1.453] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 178.33ms | 157.50ms | 1.131 | [1.119, 1.143] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 172.92ms | 152.13ms | 1.136 | [1.123, 1.150] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.20ms | 1.23ms | 1.016 | [0.930, 1.111] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 182.69ms | 122.43ms | 1.496 | [1.485, 1.507] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 182.14ms | 121.85ms | 1.499 | [1.487, 1.510] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 82.76us | 126.10us | 0.675 | [0.608, 0.750] | SLOWER |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 370.59ms | 239.93ms | 1.536 | [1.488, 1.586] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 367.59ms | 235.93ms | 1.544 | [1.496, 1.593] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.11ms | 2.60ms | 0.921 | [0.588, 1.442] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 430.25ms | 271.72ms | 1.580 | [1.569, 1.591] | faster |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 428.76ms | 270.22ms | 1.584 | [1.573, 1.594] | faster |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.43ms | 1.41ms | 1.048 | [0.686, 1.602] | ~ (noise) |

Sum of medians (total, common cases): A = 4.670s, B = 4.307s, saved = 363.29ms (1.084x)
Result sanity (rel. error vs exact): worst growth in `interplin/dsg/equi/b1/d2/L16/p16384`: A 1.92e-10 -> B 1.92e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 20.1 MiB | 19.9 MiB | 0.991 | 6.8% / 8.1% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.9 MiB | 12.7 MiB | 0.988 | 2.1% / 2.2% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.6 MiB | 0.990 | 4.0% / 1.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.8 MiB | 12.6 MiB | 0.987 | 2.2% / 2.1% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.4 MiB | 21.2 MiB | 0.992 | 4.0% / 4.7% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.2 MiB | 1.011 | 3.3% / 3.1% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.3 MiB | 0.965 | 2.2% / 3.2% | MEM- |
| interplin/dsg/equi/b1/d10/L5/p32 | 15.3 MiB | 15.9 MiB | 1.040 | 5.3% / 3.9% | MEM+ |

Peak memory (common cases): geomean B/A = 0.995; largest B/A: `interplin/dsg/equi/b1/d10/L5/p32` 15.3 MiB -> 15.9 MiB (1.040x); 1 MEM+, 1 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 20, 20, 20, 20, 20, 20, 20, 20 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 160s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
