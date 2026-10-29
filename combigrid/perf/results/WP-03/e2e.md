A/B `e2e`: A=`WP-03-before` B=`WP-03-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 736.80ms | 729.54ms | 1.012 | [0.959, 1.067] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 731.39ms | 725.77ms | 1.009 | [0.956, 1.065] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.65ms | 3.03ms | 1.615 | [1.362, 1.915] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.887s | 483.82ms | 3.870 | [3.754, 3.990] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 475.40ms | 475.27ms | 1.000 | [0.960, 1.042] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.406s | 7.50ms | 188.078 | [185.728, 190.458] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.865s | 26.14ms | 71.044 | [70.304, 71.792] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.67ms | 23.45ms | 1.017 | [0.967, 1.070] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.841s | 2.42ms | 762.168 | [757.463, 766.903] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.831s | 3.61ms | 530.531 | [481.640, 584.385] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.10ms | 2.13ms | 1.355 | [0.576, 3.185] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.829s | 1.30ms | 1619.745 | [1187.248, 2209.794] | faster |

Sum of medians (total, common cases): A = 6.320s, B = 1.243s, saved = 5.077s (5.084x)
Result sanity (rel. error vs exact): worst growth in `mivec/sg/cc-exp/b1/d15/S10/n300`: A 1.92e-03 -> B 1.92e-03 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.0 MiB | 0.995 | 5.5% / 2.2% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.8 MiB | 17.3 MiB | 0.973 | 1.8% / 2.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 12.9 MiB | 1.002 | 2.4% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.3 MiB | 12.3 MiB | 0.999 | 1.4% / 3.3% | ~ |

Peak memory (common cases): geomean B/A = 0.992; largest B/A: `mivec/sg/cc-exp/b1/d15/S10/n300` 12.9 MiB -> 12.9 MiB (1.002x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.8%, 0.8%, 0.2%, 0.2%
Round durations (s): 16, 16, 16, 16, 16 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 81s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
