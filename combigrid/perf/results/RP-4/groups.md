| group | cases | baseline | rp-3 | rp-4 | baseline → rp-4 | rp-3 → rp-4 | d ≥ 7: baseline → rp-4 | peak RSS geomean B/A | share of rp-4 suite |
|---|---|---|---|---|---|---|---|---|---|
| quad/sg | 13 | 22.281 s | 9.204 s | 9.242 s | 2.411x | 0.996x | 18.50 → 6.89 s (2.68x) | 1.083 | 24.2 % |
| quad/dsg | 9 | 26.718 s | 17.746 s | 17.826 s | 1.499x | 0.995x | 26.12 → 17.27 s (1.51x) | 0.984 | 46.7 % |
| interp/sg | 8 | 6.596 s | 5.698 s | 4.236 s | 1.557x | 1.345x | 3.70 → 2.63 s (1.41x) | 0.935 | 11.1 % |
| interp/dsg | 6 | 0.000 s | 0.000 s | 0.815 s | – | – | – | – | 2.1 % |
| interplin/sg | 4 | 4.973 s | 4.547 s | 3.516 s | 1.414x | 1.293x | 4.36 → 3.05 s (1.43x) | 1.016 | 9.2 % |
| interplin/dsg | 4 | 0.000 s | 0.000 s | 1.197 s | – | – | – | – | 3.1 % |
| mivec | 4 | 9.327 s | 1.257 s | 1.337 s | 6.974x | 0.940x | 7.12 → 0.51 s (13.88x) | 1.024 | 3.5 % |
| common | 38 | 69.894 s | 38.451 s | 36.157 s | 1.933x | 1.063x | 59.80 → 30.36 s (1.97x) | 1.014 | 94.7 % |
| all | 48 | 69.894 s | 38.451 s | 38.170 s | 1.831x | 1.007x | 59.80 → 30.36 s (1.97x) | 1.014 | 100.0 % |

quad/dsg b0 15.388 s (40.3 %), b1 2.439 s (6.4 %)

| case | rp-3 | rp-4 | rp-3 → rp-4 | gen rp-4 | op rp-4 | peak RSS baseline → rp-4 |
|---|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.785 | 6.848 | 0.991x | 4.57 ms | 6.838 | 37.0 → 43.1 MiB |
| quad/dsg/equi-exp/b0/d10/L7 | 5.396 | 5.368 | 1.005x | 3.17 ms | 5.359 | 37.2 → 42.0 MiB |
| quad/sg/equi-sin/b0/d7/L7 | 2.418 | 2.404 | 1.006x | 1.90 ms | 2.401 | 14.5 → 14.8 MiB |
| interplin/sg/equi/b1/d10/L5/p32 | 2.468 | 1.850 | 1.334x | 1.77 ms | 1.848 | 13.4 → 13.5 MiB |
| interp/sg/cc/b0/d10/L2/p8 | 1.990 | 1.627 | 1.224x | 0.06 ms | 1.626 | 23.7 → 12.2 MiB |
| quad/dsg/cc-uq/b0/d7/L10 | 1.441 | 1.441 | 1.000x | 4.07 ms | 1.432 | 36.9 → 42.6 MiB |
| quad/sg/cc-uq/b0/d10/L4 | 1.312 | 1.317 | 0.996x | 2.21 ms | 1.315 | 13.0 → 13.3 MiB |
| quad/sg/cc-uq/b0/d2/L20 | 1.201 | 1.235 | 0.972x | 84.87 ms | 1.139 | 799.1 → 1078.7 MiB |
| quad/dsg/equi-exp/b1/d15/L7 | 1.220 | 1.225 | 0.996x | 43.39 ms | 1.151 | 231.4 → 150.6 MiB |
| quad/dsg/equi-exp/b1/d20/L6 | 1.227 | 1.214 | 1.010x | 61.23 ms | 1.112 | 331.1 → 217.6 MiB |
| interplin/sg/equi/b1/d8/L5/p256 | 1.527 | 1.200 | 1.273x | 2.33 ms | 1.196 | 12.8 → 13.2 MiB |
| quad/dsg/equi-exp/b0/d7/L10 | 1.135 | 1.172 | 0.968x | 3.02 ms | 1.165 | 37.2 → 42.8 MiB |
| interp/sg/cc/b0/d5/L6/p64 | 1.449 | 1.123 | 1.290x | 0.13 ms | 1.123 | 12.1 → 12.3 MiB |
| interp/sg/cc/b0/d7/L4/p16 | 1.261 | 1.007 | 1.252x | 0.10 ms | 1.007 | 13.0 → 12.2 MiB |
| quad/sg/equi-exp/b0/d10/L4 | 0.935 | 0.910 | 1.027x | 2.26 ms | 0.906 | 12.8 → 13.1 MiB |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 0.743 | 0.825 | 0.900x | 3.00 ms | 0.821 | 16.0 → 16.6 MiB |
| quad/sg/cc-uq/b0/d7/L7 | 0.702 | 0.695 | 1.010x | 2.00 ms | 0.692 | 15.0 → 15.1 MiB |
| quad/sg/equi-exp/b1/d15/L6 | 0.523 | 0.519 | 1.007x | 15.68 ms | 0.495 | 54.3 → 54.1 MiB |
| quad/sg/equi-exp/b0/d7/L7 | 0.501 | 0.515 | 0.973x | 1.78 ms | 0.513 | 14.4 → 14.7 MiB |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 0.484 | 0.482 | 1.003x | 7.51 ms | 0.474 | 17.4 → 17.5 MiB |
| quad/sg/equi-exp/b0/d2/L20 | 0.405 | 0.440 | 0.920x | 32.56 ms | 0.364 | 363.8 → 620.0 MiB |
| interplin/dsg/equi/b1/d10/L5/p32 | – | 0.435 | new | 2.92 ms | 0.433 | 17.0 MiB |
| interp/dsg/cc/b0/d10/L2/p8 | – | 0.393 | new | 0.03 ms | 0.393 | 12.3 MiB |
| interplin/dsg/equi/b1/d8/L5/p256 | – | 0.392 | new | 1.98 ms | 0.390 | 13.2 MiB |
| interplin/sg/equi/b1/d5/L8/p1024 | 0.444 | 0.354 | 1.251x | 2.14 ms | 0.351 | 13.1 → 13.4 MiB |
| quad/sg/cc-uq/b0/d5/L10 | 0.326 | 0.323 | 1.008x | 2.28 ms | 0.320 | 17.0 → 17.5 MiB |
| quad/sg/equi-exp/b1/d10/L7 | 0.311 | 0.303 | 1.026x | 5.18 ms | 0.295 | 25.9 → 26.2 MiB |
| quad/sg/equi-exp/b0/d5/L10 | 0.253 | 0.265 | 0.954x | 2.10 ms | 0.262 | 15.2 → 15.9 MiB |
| quad/dsg/cc-uq/b0/d5/L12 | 0.248 | 0.250 | 0.993x | 2.40 ms | 0.246 | 24.2 → 26.0 MiB |
| quad/sg/equi-exp/b1/d20/L5 | 0.232 | 0.229 | 1.015x | 18.22 ms | 0.202 | 60.2 → 57.5 MiB |
| quad/dsg/equi-exp/b0/d5/L12 | 0.202 | 0.212 | 0.952x | 1.23 ms | 0.209 | 24.4 → 26.3 MiB |
| interp/sg/cc/b0/d3/L9/p64 | 0.246 | 0.187 | 1.318x | 0.07 ms | 0.186 | 12.1 → 12.3 MiB |
| interplin/dsg/equi/b1/d5/L8/p1024 | – | 0.186 | new | 2.20 ms | 0.183 | 13.3 MiB |
| interplin/dsg/equi/b1/d2/L16/p16384 | – | 0.184 | new | 1.20 ms | 0.179 | 20.7 MiB |
| interp/dsg/cc/b0/d5/L6/p64 | – | 0.171 | new | 0.08 ms | 0.171 | 12.5 MiB |
| interp/dsg/cc/b0/d7/L4/p16 | – | 0.149 | new | 0.06 ms | 0.149 | 12.5 MiB |
| interp/sg/cc/b0/d2/L12/p64 | 0.176 | 0.129 | 1.364x | 0.21 ms | 0.129 | 14.9 → 16.1 MiB |
| interp/sg/cheb1/b0/d5/L4/p64 | 0.343 | 0.117 | 2.929x | 0.04 ms | 0.117 | 12.0 → 12.2 MiB |
| interplin/sg/equi/b1/d2/L16/p16384 | 0.108 | 0.111 | 0.974x | 1.65 ms | 0.105 | 19.5 → 19.7 MiB |
| quad/dsg/equi-exp/b0/d3/L16 | 0.093 | 0.096 | 0.965x | 1.00 ms | 0.095 | 62.3 → 62.6 MiB |
| quad/sg/cc-uq/b0/d3/L14 | 0.087 | 0.087 | 0.997x | 2.07 ms | 0.084 | 38.2 → 40.9 MiB |
| interp/dsg/cc/b0/d2/L12/p64 | – | 0.052 | new | 0.13 ms | 0.052 | 16.2 MiB |
| interp/dsg/cc/b0/d3/L9/p64 | – | 0.043 | new | 0.06 ms | 0.043 | 12.6 MiB |
| interp/sg/cheb1/b0/d3/L6/p64 | 0.182 | 0.034 | 5.291x | 0.03 ms | 0.034 | 11.9 → 12.2 MiB |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 0.028 | 0.027 | 1.006x | 2.43 ms | 0.025 | 12.7 → 13.1 MiB |
| interp/sg/equi/b0/d3/L5/p64 | 0.052 | 0.012 | 4.307x | 0.02 ms | 0.012 | 11.8 → 12.1 MiB |
| interp/dsg/equi/b0/d3/L5/p64 | – | 0.006 | new | 0.02 ms | 0.006 | 12.2 MiB |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 0.003 | 0.003 | 0.999x | 0.81 ms | 0.002 | 12.1 → 12.5 MiB |
