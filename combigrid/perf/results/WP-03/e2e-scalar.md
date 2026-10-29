A/B `e2e`: A=`WP-03-before` B=`WP-03-scalar`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 746.77ms | 755.67ms | 0.985 | [0.934, 1.039] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 741.57ms | 750.63ms | 0.985 | [0.934, 1.039] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.59ms | 4.31ms | 1.036 | [0.942, 1.139] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.904s | 1.311s | 1.431 | [1.392, 1.471] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 482.33ms | 477.07ms | 0.993 | [0.930, 1.061] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.418s | 833.06ms | 1.685 | [1.644, 1.726] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.864s | 1.289s | 1.443 | [1.415, 1.471] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.37ms | 23.59ms | 1.013 | [0.954, 1.077] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.840s | 1.265s | 1.451 | [1.423, 1.480] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.832s | 1.144s | 1.598 | [1.568, 1.628] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 2.08ms | 0.837 | [0.513, 1.366] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.830s | 1.142s | 1.600 | [1.569, 1.631] | faster |

Sum of medians (total, common cases): A = 6.347s, B = 4.500s, saved = 1.847s (1.410x)
Result sanity (rel. error vs exact): worst growth in `mivec/sg/cc-exp/b1/d10/S12/n1000`: A 1.24e-08 -> B 1.24e-08 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.2 MiB | 1.005 | 2.9% / 1.3% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.6 MiB | 17.5 MiB | 0.995 | 1.9% / 4.0% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.9 MiB | 1.010 | 2.3% / 5.3% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.4 MiB | 12.3 MiB | 0.997 | 0.8% / 1.4% | ~ |

Peak memory (common cases): geomean B/A = 1.002; largest B/A: `mivec/sg/cc-exp/b1/d15/S10/n300` 12.8 MiB -> 12.9 MiB (1.010x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.5%, 0.5%, 0.4%
Round durations (s): 23, 22, 23, 23, 23 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 113s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
