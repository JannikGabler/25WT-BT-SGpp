A/B `e2e`: A=`report-master` B=`rp-1`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.630s | 1.991s | 1.820 | [1.766, 1.876] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 2.598s | 478.24ms | 5.346 | [4.950, 5.774] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.031s | 1.510s | 0.688 | [0.677, 0.700] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.684s | 2.094s | 0.803 | [0.798, 0.808] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 132.68ms | 23.72ms | 5.520 | [5.247, 5.807] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.547s | 2.071s | 0.748 | [0.743, 0.752] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.529s | 2.072s | 0.739 | [0.732, 0.746] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 12.56ms | 2.07ms | 6.003 | [5.544, 6.501] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.516s | 2.070s | 0.733 | [0.726, 0.740] | SLOWER |

Sum of medians (total, common cases): A = 6.843s, B = 6.158s, saved = 684.37ms (1.111x)
Result sanity (rel. error vs exact): worst growth in `mivec/sg/cc-exp/b1/d10/S12/n1000`: A 1.24e-08 -> B 1.24e-08 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 12.8 MiB | 17.7 MiB | 1.383 | 4.8% / 2.8% | MEM+ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 7.4 MiB | 12.9 MiB | 1.731 | 1.4% / 2.0% | MEM+ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 6.8 MiB | 12.4 MiB | 1.825 | 1.9% / 2.4% | MEM+ |

Peak memory (common cases): geomean B/A = 1.635; largest B/A: `mivec/sg/cc-exp/b1/d20/S7/n300` 6.8 MiB -> 12.4 MiB (1.825x); 3 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 5.9 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 26, 26, 26, 27, 26 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 132s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
