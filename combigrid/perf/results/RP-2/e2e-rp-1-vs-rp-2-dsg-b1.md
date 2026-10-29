A/B `e2e`: A=`rp-1` B=`rp-2`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d15/L7 | total | 4.087s | 1.220s | 3.362 | [3.337, 3.387] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 4.026s | 1.145s | 3.517 | [3.491, 3.543] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.56ms | 42.72ms | 0.851 | [0.729, 0.993] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | total | 7.025s | 1.223s | 5.742 | [5.715, 5.769] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.934s | 1.122s | 6.176 | [6.144, 6.207] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.86ms | 60.90ms | 0.843 | [0.818, 0.868] | SLOWER |

Sum of medians (total, common cases): A = 11.112s, B = 2.443s, saved = 8.669s (4.548x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d20/L6`: A 6.42e-09 -> B 6.77e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d15/L7 | 234.7 MiB | 151.6 MiB | 0.646 | 1.7% / 4.4% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 328.1 MiB | 217.2 MiB | 0.662 | 1.2% / 0.1% | MEM- |

Peak memory (common cases): geomean B/A = 0.654; largest B/A: `quad/dsg/equi-exp/b1/d20/L6` 328.1 MiB -> 217.2 MiB (0.662x); 0 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.5%, 0.2%, 0.2%, 0.2%
Round durations (s): 28, 28, 28, 28, 28 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 139s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
