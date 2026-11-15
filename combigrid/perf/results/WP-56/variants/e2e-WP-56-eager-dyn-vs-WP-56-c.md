A/B `e2e`: A=`WP-56-eager-dyn` B=`WP-56-c`, 16 thread(s), 4 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 353.58ms | 283.15ms | 1.237 | [1.075, 1.424] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 315.13ms | 244.53ms | 1.277 | [1.108, 1.472] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.26ms | 38.59ms | 0.985 | [0.858, 1.130] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 433.58ms | 376.34ms | 1.140 | [1.049, 1.239] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 357.98ms | 304.10ms | 1.188 | [1.131, 1.248] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 70.72ms | 71.23ms | 0.969 | [0.805, 1.167] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.16ms | 76.23ms | 0.999 | [0.981, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.88ms | 74.91ms | 0.999 | [0.983, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.25ms | 1.26ms | 1.000 | [0.983, 1.017] | ~ (noise) |

Sum of medians (total, common cases): A = 863.32ms, B = 735.73ms, saved = 127.59ms (1.173x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d3/L14`: A 1.17e-13 -> B 1.21e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 112.5 MiB | 114.4 MiB | 1.017 | 13.8% / 15.9% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 208.2 MiB | 194.5 MiB | 0.934 | 11.1% / 4.7% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.2 MiB | 15.0 MiB | 0.992 | 4.4% / 5.4% | ~ |

Peak memory (common cases): geomean B/A = 0.980; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 112.5 MiB -> 114.4 MiB (1.017x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.7%, 0.2%
Round durations (s): 4, 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 16s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
