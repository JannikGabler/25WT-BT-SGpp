# A/A memory section re-rendered with the final flag rule

`aa-e2e.md` was produced while the provisional rule (ratio beyond ±2 % only) was in place; this is the same data (`aa-e2e.json`) rendered by the final `memory_section` of perf.py.

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b1/d20/L5 | 57.7 MiB | 57.1 MiB | 0.989 | 12.1% / 12.5% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 14.7 MiB | 0.999 | 6.5% / 5.4% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1067.2 MiB | 1067.1 MiB | 1.000 | 4.8% / 1.4% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 328.7 MiB | 326.5 MiB | 0.993 | 1.9% / 1.9% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.9 MiB | 37.4 MiB | 0.988 | 4.8% / 3.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.6 MiB | 24.3 MiB | 0.990 | 2.7% / 6.3% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.9 MiB | 1.002 | 3.3% / 1.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.0 MiB | 0.983 | 4.8% / 3.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.0 MiB | 12.1 MiB | 1.004 | 2.1% / 3.8% | ~ |

Peak memory (common cases): geomean B/A = 0.994; largest B/A: `mivec/sg/cc-exp/b1/d20/S7/n300` 12.0 MiB -> 12.1 MiB (1.004x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.1 MiB
