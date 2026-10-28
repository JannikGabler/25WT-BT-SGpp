A/B `e2e`: A=`WP-02-after` B=`WP-02-split-hidden`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.95ms | 94.95ms | 0.987 | [0.980, 0.993] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 92.43ms | 93.48ms | 0.987 | [0.979, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 1.03ms | 1.001 | [0.802, 1.248] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 205.28ms | 212.93ms | 0.964 | [0.946, 0.981] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 202.64ms | 210.26ms | 0.963 | [0.945, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.17ms | 1.40ms | 0.887 | [0.674, 1.167] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.164s | 1.202s | 0.966 | [0.953, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.156s | 1.194s | 0.966 | [0.954, 0.977] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.07ms | 3.03ms | 0.993 | [0.539, 1.829] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.478s | 5.653s | 0.968 | [0.965, 0.972] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.470s | 5.644s | 0.968 | [0.965, 0.972] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.23ms | 3.49ms | 0.929 | [0.700, 1.234] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.223s | 1.252s | 0.978 | [0.974, 0.982] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.151s | 1.181s | 0.976 | [0.972, 0.981] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.54ms | 42.22ms | 0.988 | [0.846, 1.153] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.223s | 1.239s | 0.993 | [0.978, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.138s | 0.993 | [0.977, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.50ms | 62.20ms | 0.979 | [0.951, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 251.68ms | 261.98ms | 0.960 | [0.944, 0.976] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 247.49ms | 257.83ms | 0.962 | [0.947, 0.978] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.35ms | 2.28ms | 0.998 | [0.925, 1.077] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.468s | 1.511s | 0.969 | [0.962, 0.976] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.457s | 1.501s | 0.969 | [0.962, 0.976] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.04ms | 4.55ms | 1.094 | [0.831, 1.441] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.834s | 7.032s | 0.972 | [0.966, 0.978] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.825s | 7.023s | 0.972 | [0.967, 0.976] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.11ms | 4.03ms | 1.028 | [0.673, 1.572] | ~ (noise) |

Sum of medians (total, common cases): A = 17.941s, B = 18.459s, saved = -517703105.0ns (0.972x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 3.70e-10 -> B 4.36e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.0 MiB | 62.4 MiB | 1.008 | 1.7% / 3.0% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.2 MiB | 26.1 MiB | 0.997 | 2.7% / 3.2% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.2 MiB | 42.6 MiB | 1.009 | 4.0% / 5.7% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.4 MiB | 41.9 MiB | 0.990 | 4.0% / 3.6% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.1 MiB | 150.5 MiB | 0.996 | 1.1% / 3.3% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.4 MiB | 217.3 MiB | 1.000 | 0.8% / 0.8% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.9 MiB | 25.5 MiB | 0.984 | 3.4% / 6.1% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.5 MiB | 43.3 MiB | 1.020 | 8.3% / 5.0% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.9 MiB | 42.4 MiB | 0.987 | 2.9% / 4.2% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `quad/dsg/cc-uq/b0/d7/L10` 42.5 MiB -> 43.3 MiB (1.020x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.4%, 0.4%, 0.4%
Round durations (s): 75, 75, 75, 75, 75 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 374s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
