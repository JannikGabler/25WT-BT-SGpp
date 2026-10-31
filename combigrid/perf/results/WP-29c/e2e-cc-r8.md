A/B `e2e`: A=`WP-29c-before` B=`WP-29c-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d5/L6/p64 | total | 1.451s | 1.449s | 1.001 | [0.996, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.450s | 1.449s | 1.001 | [0.996, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 117.30us | 107.59us | 1.046 | [0.897, 1.219] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.261s | 1.262s | 0.998 | [0.989, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.260s | 1.262s | 0.998 | [0.989, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 64.64us | 65.86us | 0.971 | [0.937, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.999s | 2.009s | 0.994 | [0.986, 1.002] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.999s | 2.008s | 0.994 | [0.986, 1.002] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.27us | 35.06us | 0.868 | [0.202, 3.737] | ~ (noise) |

Sum of medians (total, common cases): A = 4.710s, B = 4.721s, saved = -10889313.0ns (0.998x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d7/L4/p16`: A 1.25e-04 -> B 1.25e-04 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.2 MiB | 1.007 | 0.9% / 2.0% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 12.9 MiB | 0.999 | 4.5% / 4.9% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 25.0 MiB | 25.0 MiB | 0.999 | 3.3% / 3.9% | ~ |

Peak memory (common cases): geomean B/A = 1.002; largest B/A: `interp/sg/cc/b0/d5/L6/p64` 12.1 MiB -> 12.2 MiB (1.007x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 19, 19, 19, 19, 19, 19, 19, 19 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 155s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
