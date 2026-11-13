A/B `e2e`: A=`rp-4` B=`rp-4-binWP45`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | total | 1.281s | 1.221s | 1.013 | [0.906, 1.133] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.181s | 1.145s | 1.006 | [0.900, 1.126] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 81.41ms | 79.70ms | 1.001 | [0.809, 1.238] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.99ms | 96.76ms | 1.005 | [0.991, 1.018] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.72ms | 95.26ms | 1.005 | [0.991, 1.019] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.11ms | 1.04ms | 0.979 | [0.800, 1.198] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.80ms | 203.11ms | 1.036 | [1.029, 1.044] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.00ms | 200.70ms | 1.037 | [1.029, 1.045] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.30ms | 1.06ms | 1.102 | [0.854, 1.422] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.175s | 1.149s | 1.022 | [1.016, 1.027] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.167s | 1.141s | 1.022 | [1.016, 1.028] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.03ms | 3.05ms | 0.982 | [0.936, 1.031] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.851s | 6.845s | 1.003 | [0.997, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.841s | 6.835s | 1.003 | [0.998, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.53ms | 3.94ms | 1.152 | [0.754, 1.761] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 787.50ms | 726.75ms | 1.063 | [1.008, 1.121] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 783.32ms | 722.95ms | 1.063 | [1.008, 1.122] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.04ms | 3.01ms | 1.032 | [0.957, 1.113] | ~ (noise) |

Sum of medians (total, common cases): A = 10.402s, B = 10.242s, saved = 159.50ms (1.016x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b0/d5/L12`: A 3.53e-12 -> B 3.60e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 1071.0 MiB | 1061.3 MiB | 0.991 | 2.3% / 3.6% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.4 MiB | 62.5 MiB | 1.002 | 2.5% / 2.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.3 MiB | 26.8 MiB | 1.020 | 1.7% / 5.0% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.5 MiB | 41.9 MiB | 0.987 | 4.2% / 6.3% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 43.0 MiB | 42.4 MiB | 0.986 | 4.2% / 4.2% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.3 MiB | 16.5 MiB | 1.011 | 4.0% / 4.1% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `quad/dsg/equi-exp/b0/d5/L12` 26.3 MiB -> 26.8 MiB (1.020x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 43, 43, 43, 42, 43 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 213s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
