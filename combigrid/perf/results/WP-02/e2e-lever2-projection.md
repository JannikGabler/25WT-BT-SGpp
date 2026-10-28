A/B `e2e`: A=`WP-02-v2` B=`WP-02-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.472s | 5.482s | 0.997 | [0.993, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.463s | 5.474s | 0.996 | [0.993, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.71ms | 3.35ms | 1.051 | [0.792, 1.395] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 2.137s | 1.228s | 1.732 | [1.704, 1.761] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 2.077s | 1.155s | 1.789 | [1.763, 1.816] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.26ms | 44.45ms | 0.800 | [0.685, 0.933] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | total | 2.332s | 1.230s | 1.894 | [1.887, 1.901] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 2.242s | 1.128s | 1.984 | [1.976, 1.991] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 49.81ms | 60.68ms | 0.823 | [0.798, 0.847] | SLOWER |

Sum of medians (total, common cases): A = 9.942s, B = 7.940s, saved = 2.002s (1.252x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d20/L6`: A 6.12e-09 -> B 7.40e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | 37.6 MiB | 42.1 MiB | 1.120 | 1.4% / 2.8% | MEM+ |
| quad/dsg/equi-exp/b1/d15/L7 | 236.7 MiB | 153.4 MiB | 0.648 | 0.9% / 3.2% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 336.8 MiB | 217.4 MiB | 0.646 | 3.2% / 1.0% | MEM- |

Peak memory (common cases): geomean B/A = 0.777; largest B/A: `quad/dsg/equi-exp/b0/d10/L7` 37.6 MiB -> 42.1 MiB (1.120x); 1 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.6%, 0.5%, 0.5%, 0.5%
Round durations (s): 37, 37, 37, 37, 37 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 183s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
