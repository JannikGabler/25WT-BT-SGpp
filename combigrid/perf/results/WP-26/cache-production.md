## Lookup (ns per lookup incl. hashing; production = XXH3 + Boost index, sharded_spin = WP-25 prototype with an own wyhash; 16 t: wall per item)

| d | n | production 1 t | sharded_spin 1 t | production 16 t | sharded_spin 16 t | bytes/entry production |
|---|---|---|---|---|---|---|
| 2 | 10000 | 50.3 | 56.1 | 10.3 | 12.2 | 62 |
| 2 | 1000000 | 115.1 | 126.8 | 15.5 | 16.9 | 42 |
| 10 | 10000 | 77.2 | 77.4 | 11.3 | 11.3 | 167 |
| 10 | 1000000 | 256.0 | 296.7 | 21.9 | 21.0 | 107 |
| 20 | 10000 | 115.6 | 109.6 | 13.5 | 13.8 | 296 |
| 20 | 1000000 | 333.4 | 350.0 | 30.0 | 30.1 | 187 |

## Insert (ns per insert incl. hashing; production = XXH3 + Boost index, sharded_spin = WP-25 prototype with an own wyhash; 16 t: wall per item)

| d | n | production 1 t | sharded_spin 1 t | production 16 t | sharded_spin 16 t |
|---|---|---|---|---|---|
| 2 | 10000 | 86.2 | 108.1 | 25.6 | 24.4 |
| 2 | 1000000 | 144.7 | 166.3 | 40.5 | 38.6 |
| 10 | 10000 | 99.9 | 102.3 | 26.3 | 25.6 |
| 10 | 1000000 | 183.2 | 259.4 | 60.4 | 58.6 |
| 20 | 10000 | 108.9 | 114.2 | 28.3 | 29.0 |
| 20 | 1000000 | 340.0 | 350.6 | 76.0 | 74.5 |

## Operators with a cached function (speedup = none / variant, > 1: the cache pays; always = new cache per call, warm = filled before; for interp / interplin by a quadrature on the same grid = cross-operator)

| operator (evals / distinct) | cost | none ns/eval 1 t / 16 t | always 1 t / 16 t | always WP-25 proto 1 t / 16 t | warm 1 t / 16 t | warm hit rate |
|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 (5.73) | 0 ns | 28.0 / 3.2 | 0.35 / 0.16 | 0.34 / 0.19 | 0.40 / 0.25 | 1.000 |
| quad/sg/cc/b0/d5/L4 (5.73) | 1000 ns | 950.7 / 72.3 | 3.81 / 2.26 | 3.78 / 2.39 | 13.78 / 5.89 | 1.000 |
| quad/sg/equi/b1/d20/L3 (2.00) | 0 ns | 33.5 / 3.8 | 0.23 / 0.13 | 0.23 / 0.15 | 0.23 / 0.25 | 1.000 |
| quad/sg/equi/b1/d20/L3 (2.00) | 1000 ns | 933.0 / 65.2 | 1.50 / 1.20 | 1.51 / 1.26 | 6.64 / 4.41 | 1.000 |
| quad/sg/equi/b0/d2/L11 (2.73) | 0 ns | 17.9 / 2.8 | 0.21 / 0.09 | 0.20 / 0.09 | 0.29 / 0.19 | 1.000 |
| quad/sg/equi/b0/d2/L11 (2.73) | 1000 ns | 899.8 / 111.2 | 2.05 / 1.40 | 2.01 / 1.35 | 14.78 / 7.26 | 1.000 |
| quad/sg/equi/b1/d10/L4 (2.70) | 0 ns | 23.2 / 2.5 | 0.23 / 0.11 | 0.20 / 0.12 | 0.28 / 0.21 | 1.000 |
| quad/sg/equi/b1/d10/L4 (2.70) | 1000 ns | 919.4 / 65.6 | 2.03 / 1.50 | 1.97 / 1.59 | 10.63 / 5.23 | 1.000 |
| quad/dsg/cc/b0/d5/L5 (1.00) | 0 ns | 37.0 / 6.2 | 0.26 / 0.17 | 0.26 / 0.18 | 0.41 / 0.37 | 1.000 |
| quad/dsg/cc/b0/d5/L5 (1.00) | 1000 ns | 969.3 / 86.9 | 0.89 / 0.78 | 0.88 / 0.77 | 10.92 / 5.06 | 1.000 |
| interp/sg/cc/b0/d5/L3/p8 (36.63) | 0 ns | 112.7 / 11.7 | 1.15 / 0.77 | 1.22 / 0.80 | 1.21 / 0.88 | 1.000 |
| interp/sg/cc/b0/d5/L3/p8 (36.63) | 1000 ns | 1088.2 / 84.9 | 8.76 / 4.74 | 9.15 / 4.90 | 11.66 / 6.00 | 1.000 |
| interplin/sg/equi/b1/d8/L3/p8 (4.43) | 0 ns | 185.7 / 20.1 | 1.13 / 0.70 | 1.11 / 0.74 | 1.49 / 1.39 | 0.993 |
| interplin/sg/equi/b1/d8/L3/p8 (4.43) | 1000 ns | 1153.1 / 88.5 | 2.98 / 1.16 | 2.97 / 1.86 | 9.54 / 5.94 | 0.993 |

## Memory of a cache filled by one quadrature, with and without the limit (quad/sg/equi/b0/d5/L8, expSum; measured: a further quadrature with that cache)

| limit | threads | entries | rejected | cache MiB | heap MiB | hit rate | ns/eval |
|---|---|---|---|---|---|---|---|
| limit1GiB | 1 | 1969665 | 0 | 124.4 | 124.8 | 1.000 | 139.0 |
| limit1GiB | 16 | 1969665 | 0 | 124.4 | 125.0 | 1.000 | 19.8 |
| limit16MiB | 1 | 253952 | 4650773 | 15.9 | 16.0 | 0.753 | 89.0 |
| limit16MiB | 16 | 253952 | 6121348 | 15.9 | 16.1 | 0.675 | 12.7 |

Note: single runs (`perf.py run micro`, snapshot WP-26-after; first 16-thread run repeated because of
3.9 % foreign load). `interplin … c1000` "always" at 16 t was 1.16 in this run; repeated 3x:
production 18.20–18.27 ms vs. WP-25 prototype 18.14–19.12 ms vs. none 30.07–30.63 ms (≈ 1.65x),
i.e. the 1.16 is run-to-run noise of a short 16-thread case.
