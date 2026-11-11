A/B `e2e`: A=`WP-08-before` B=`WP-08-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.00ms | 110.96ms | 1.013 | [0.963, 1.066] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.98ms | 104.70ms | 1.014 | [0.961, 1.071] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.69ms | 1.71ms | 0.977 | [0.899, 1.063] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 443.68ms | 362.64ms | 1.251 | [1.163, 1.345] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 440.89ms | 359.62ms | 1.253 | [1.164, 1.349] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.33ms | 2.30ms | 1.021 | [0.601, 1.735] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.517s | 1.195s | 1.267 | [1.238, 1.296] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.514s | 1.193s | 1.268 | [1.239, 1.296] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.30ms | 2.17ms | 0.944 | [0.624, 1.428] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.454s | 1.837s | 1.334 | [1.307, 1.362] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.451s | 1.834s | 1.335 | [1.308, 1.362] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.76ms | 1.76ms | 0.995 | [0.874, 1.133] | ~ (noise) |

Sum of medians (total, common cases): A = 4.525s, B = 3.506s, saved = 1.020s (1.291x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d5/L8/p1024`: A 1.71e-04 -> B 1.71e-04 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.9 MiB | 1.014 | 4.3% / 2.7% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 13.2 MiB | 0.996 | 2.3% / 3.6% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 13.2 MiB | 1.011 | 3.6% / 3.0% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.3 MiB | 13.5 MiB | 1.014 | 2.0% / 5.6% | ~ |

Peak memory (common cases): geomean B/A = 1.009; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.3 MiB -> 13.5 MiB (1.014x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 17, 17, 17, 17, 17 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 86s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
