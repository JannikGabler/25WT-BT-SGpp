A/B `e2e`: A=`WP-47-before` B=`WP-47-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.99ms | 111.04ms | 1.055 | [0.809, 1.375] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.96ms | 105.14ms | 1.057 | [0.800, 1.396] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.70ms | 1.71ms | 1.034 | [0.896, 1.195] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 355.58ms | 363.93ms | 0.989 | [0.964, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 352.75ms | 358.99ms | 0.991 | [0.965, 1.018] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.12ms | 2.30ms | 0.769 | [0.485, 1.221] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.195s | 1.205s | 0.988 | [0.966, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.192s | 1.201s | 0.989 | [0.967, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.33ms | 3.42ms | 0.852 | [0.563, 1.291] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.849s | 1.889s | 0.987 | [0.962, 1.013] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.847s | 1.887s | 0.987 | [0.962, 1.013] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.79ms | 1.70ms | 1.077 | [0.997, 1.164] | ~ (noise) |

Sum of medians (total, common cases): A = 3.511s, B = 3.569s, saved = -57807757.0ns (0.984x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d8/L5/p256`: A 3.19e-02 -> B 3.19e-02 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.7 MiB | 1.002 | 3.0% / 1.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.4 MiB | 13.3 MiB | 0.997 | 0.9% / 3.8% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.1 MiB | 13.1 MiB | 1.002 | 4.2% / 3.2% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.6 MiB | 13.4 MiB | 0.982 | 3.8% / 5.0% | ~ |

Peak memory (common cases): geomean B/A = 0.996; largest B/A: `interplin/sg/equi/b1/d8/L5/p256` 13.1 MiB -> 13.1 MiB (1.002x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 15, 15, 15, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 76s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
