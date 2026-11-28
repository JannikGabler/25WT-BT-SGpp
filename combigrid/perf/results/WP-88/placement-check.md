# WP-88 — `quad/dsg/equi-exp/b0/d5/L12` 0.993 / 0.996 SLOWER: code placement

`perf stat -e instructions:u,cycles:u,op_cache_hit_miss.op_cache_miss,op_cache_hit_miss.all_op_cache_accesses`
on `perf_e2e --filter '^quad/dsg/equi-exp/b0/d5/L12$' --threads 16 --samples 5 --warmups 1`, two runs each:

| snapshot | instructions | cycles | op-cache misses | op-cache accesses |
|---|---|---|---|---|
| WP-88-before | 87.108e9 / 87.082e9 | 39.32e9 / 39.31e9 | 73.2e6 / 73.6e6 | 13.75e9 |
| WP-88-after  | 87.105e9 / 87.110e9 | 39.59e9 / 39.64e9 | 143.4e6 / 144.6e6 | 13.75e9 |

Same instruction count (the DSG quadrature path is untouched by WP-88), op-cache misses x1.96, cycles +0.7 %:
code placement of the unchanged hot loop (README "Pitfalls"). A hybrid snapshot (same libraries, the other
benchmark binary) is impossible here: the renamed symbols make the old binaries unlinkable against the new
libraries and vice versa.
