A/B `e2e`: A=`WP-61-rework-A` B=`WP-61-rework-B`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | total | 452.03ms | 442.15ms | 1.013 | [0.963, 1.067] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 372.91ms | 364.04ms | 1.025 | [0.984, 1.069] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 75.33ms | 76.02ms | 0.998 | [0.899, 1.109] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.76ms | 26.37ms | 1.014 | [0.995, 1.034] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.21ms | 23.80ms | 1.015 | [0.981, 1.051] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.38ms | 2.38ms | 1.005 | [0.991, 1.019] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.63ms | 344.94ms | 1.003 | [1.000, 1.005] | faster |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.56ms | 344.87ms | 1.003 | [1.000, 1.005] | faster |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 37.68us | 35.20us | 1.075 | [1.025, 1.128] | faster |

Sum of medians (total, common cases, group suite): A = 478.80ms, B = 468.53ms, saved = 10.27ms (1.022x)
Sum of medians (total, common cases, group expensive/): A = 345.63ms, B = 344.94ms, saved = 693.03us (1.002x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d2/L20`: A 3.64e-13 -> B 3.64e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 190.5 MiB | 189.1 MiB | 0.993 | 11.5% / 3.2% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.3 MiB | 1.004 | 1.7% / 1.5% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.4 MiB | 12.5 MiB | 1.005 | 2.1% / 1.9% | ~ |

Peak memory (common cases): geomean B/A = 1.000; largest B/A: `expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us` 12.4 MiB -> 12.5 MiB (1.005x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.6%, 0.2%, 0.1%, 0.2%, 0.2%, 0.3%, 0.2%, 0.3%
Round durations (s): 4, 4, 4, 4, 4, 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 32s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
