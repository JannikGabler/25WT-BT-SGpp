A/B `e2e`: A=`WP-26-before` B=`WP-26-after-r`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.55ms | 93.53ms | 0.991 | [0.982, 1.000] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.18ms | 91.99ms | 0.992 | [0.983, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 1.20ms | 0.916 | [0.800, 1.049] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.18ms | 3.18ms | 0.890 | [0.670, 1.183] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.08ms | 2.06ms | 0.971 | [0.746, 1.263] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 855.59us | 910.79us | 0.788 | [0.500, 1.240] | ~ (noise) |

Sum of medians (total, common cases): A = 95.73ms, B = 96.70ms, saved = -974279.5ns (0.990x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b0/d3/L16`: A 1.14e-12 -> B 1.15e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 62.6 MiB | 1.001 | 3.3% / 1.7% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.3 MiB | 1.010 | 3.6% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 1.006; largest B/A: `mivec/sg/cc-exp/b1/d20/S7/n300` 12.2 MiB -> 12.3 MiB (1.010x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.2%, 0.5%, 0.3%, 0.0%, 0.3%, 0.2%, 0.1%
Round durations (s): 1, 1, 1, 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 7s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
