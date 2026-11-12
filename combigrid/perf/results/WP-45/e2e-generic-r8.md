A/B `e2e`: A=`WP-45-before` B=`WP-45-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.79ms | 181.78ms | 0.999 | [0.959, 1.041] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.71ms | 181.70ms | 1.000 | [0.960, 1.040] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 27.45us | 27.94us | 0.626 | [0.201, 1.952] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 335.86ms | 343.37ms | 0.980 | [0.971, 0.988] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.75ms | 343.26ms | 0.980 | [0.971, 0.988] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 33.94us | 34.68us | 0.967 | [0.816, 1.146] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.90ms | 51.26ms | 0.992 | [0.984, 1.001] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.83ms | 51.19ms | 0.992 | [0.984, 1.001] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 19.80us | 20.71us | 0.933 | [0.758, 1.148] | ~ (noise) |

Sum of medians (total, common cases): A = 568.55ms, B = 576.41ms, saved = -7856126.5ns (0.986x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cheb1/b0/d5/L4/p64`: A 2.28e-04 -> B 2.28e-04 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.2 MiB | 0.998 | 3.4% / 1.3% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.1 MiB | 12.2 MiB | 1.005 | 2.3% / 1.3% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.2 MiB | 1.000 | 2.5% / 3.7% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `interp/sg/cheb1/b0/d5/L4/p64` 12.1 MiB -> 12.2 MiB (1.005x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.6%, 0.2%, 0.1%, 0.2%, 0.1%, 0.2%, 0.2%
Round durations (s): 3, 3, 3, 3, 3, 3, 3, 3 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 24s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
