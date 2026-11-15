A/B `e2e`: A=`WP-56-lazy-dyn` B=`WP-56-eager-dyn`, 16 thread(s), 4 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 320.26ms | 341.01ms | 0.938 | [0.815, 1.079] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 282.19ms | 303.32ms | 0.935 | [0.812, 1.078] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.04ms | 38.13ms | 1.005 | [0.876, 1.153] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 457.46ms | 437.88ms | 1.093 | [0.929, 1.286] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 382.21ms | 353.61ms | 1.124 | [0.943, 1.340] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 70.51ms | 75.20ms | 0.968 | [0.783, 1.196] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.10ms | 76.04ms | 0.997 | [0.973, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.76ms | 74.77ms | 0.996 | [0.972, 1.021] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.30ms | 1.26ms | 1.031 | [0.996, 1.067] | ~ (noise) |

Sum of medians (total, common cases): A = 853.83ms, B = 854.94ms, saved = -1112075.5ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d3/L14`: A 1.19e-13 -> B 1.20e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 118.0 MiB | 121.2 MiB | 1.027 | 18.5% / 23.8% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 192.5 MiB | 196.0 MiB | 1.018 | 2.7% / 11.2% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 14.8 MiB | 15.5 MiB | 1.045 | 6.8% / 4.8% | ~ |

Peak memory (common cases): geomean B/A = 1.030; largest B/A: `quad/sg/cc-uq/b0/d3/L14` 14.8 MiB -> 15.5 MiB (1.045x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.3%, 0.2%
Round durations (s): 4, 4, 4, 4 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 17s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
