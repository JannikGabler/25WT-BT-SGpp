A/B `e2e`: A=`WP-56-a` B=`WP-56-eager-dyn`, 16 thread(s), 3 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 334.71ms | 364.34ms | 0.927 | [0.745, 1.155] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 296.16ms | 323.44ms | 0.924 | [0.740, 1.154] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.82ms | 39.88ms | 0.953 | [0.769, 1.182] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 564.64ms | 447.68ms | 1.280 | [1.186, 1.381] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 495.17ms | 372.36ms | 1.361 | [1.217, 1.522] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 69.32ms | 71.32ms | 0.935 | [0.700, 1.249] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.40ms | 75.99ms | 0.996 | [0.969, 1.025] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.01ms | 74.70ms | 0.996 | [0.971, 1.021] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.27ms | 1.30ms | 0.982 | [0.918, 1.050] | ~ (noise) |

Sum of medians (total, common cases): A = 974.74ms, B = 888.01ms, saved = 86.73ms (1.098x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d2/L20`: A 3.64e-13 -> B 3.64e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 120.3 MiB | 115.2 MiB | 0.958 | 17.6% / 24.8% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 199.7 MiB | 195.7 MiB | 0.980 | 18.8% / 4.8% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 14.9 MiB | 15.1 MiB | 1.015 | 3.2% / 3.7% | ~ |

Peak memory (common cases): geomean B/A = 0.984; largest B/A: `quad/sg/cc-uq/b0/d3/L14` 14.9 MiB -> 15.1 MiB (1.015x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%
Round durations (s): 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 13s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
