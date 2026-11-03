A/B `e2e`: A=`WP-04-before` B=`WP-04-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.74ms | 91.14ms | 1.023 | [1.010, 1.037] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 92.20ms | 89.67ms | 1.025 | [1.012, 1.038] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.01ms | 1.15ms | 0.919 | [0.725, 1.165] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.99ms | 195.17ms | 1.036 | [1.029, 1.042] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.67ms | 192.27ms | 1.037 | [1.032, 1.042] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.12ms | 1.33ms | 0.823 | [0.608, 1.113] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.136s | 1.077s | 1.060 | [1.048, 1.073] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.128s | 1.070s | 1.060 | [1.047, 1.073] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.07ms | 3.08ms | 0.993 | [0.959, 1.028] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.406s | 5.125s | 1.055 | [1.051, 1.059] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.397s | 5.116s | 1.055 | [1.052, 1.059] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.23ms | 3.55ms | 0.808 | [0.563, 1.160] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.222s | 1.160s | 1.052 | [1.042, 1.062] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.150s | 1.088s | 1.055 | [1.050, 1.060] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.86ms | 43.40ms | 0.973 | [0.833, 1.135] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225s | 1.177s | 1.040 | [1.035, 1.045] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.122s | 1.077s | 1.043 | [1.039, 1.047] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.87ms | 60.50ms | 0.990 | [0.959, 1.021] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.44ms | 239.67ms | 1.037 | [1.032, 1.042] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.38ms | 236.04ms | 1.037 | [1.031, 1.042] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.44ms | 2.42ms | 1.020 | [0.908, 1.146] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.440s | 1.369s | 1.051 | [1.047, 1.055] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.429s | 1.359s | 1.050 | [1.046, 1.054] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.95ms | 4.06ms | 1.132 | [0.962, 1.333] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.785s | 6.535s | 1.040 | [1.032, 1.048] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.777s | 6.526s | 1.040 | [1.032, 1.049] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.34ms | 4.59ms | 0.990 | [0.648, 1.514] | ~ (noise) |

Sum of medians (total, common cases): A = 17.758s, B = 16.969s, saved = 789.22ms (1.047x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b0/d5/L12`: A 3.54e-12 -> B 3.62e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.7 MiB | 62.5 MiB | 0.997 | 2.2% / 2.3% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.0 MiB | 26.7 MiB | 1.028 | 3.6% / 3.8% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.0 MiB | 43.4 MiB | 1.033 | 4.1% / 3.6% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.0 MiB | 41.7 MiB | 0.993 | 3.1% / 3.5% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.4 MiB | 150.3 MiB | 0.993 | 2.3% / 2.3% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.4 MiB | 215.5 MiB | 0.991 | 0.9% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.8 MiB | 26.1 MiB | 1.013 | 2.8% / 3.6% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 43.1 MiB | 42.6 MiB | 0.990 | 2.2% / 2.1% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.4 MiB | 42.3 MiB | 0.998 | 1.2% / 3.3% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `quad/dsg/equi-exp/b0/d7/L10` 42.0 MiB -> 43.4 MiB (1.033x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 72, 71, 72, 72, 72 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 358s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
