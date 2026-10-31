# WP-26 Python callables with pysgpp.CachedSourceFunc (judged build + SG_PYTHON=1, lib byte-identical to snapshot WP-26-after)

threads: 1

| case | callable | variant | ns/eval | speedup | Python calls / evals |
|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | math | none | 534 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | cold | 191 | 2.80 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | warm | 71 | 7.53 | 0 / 1031790 |
| quad/sg/cc/b0/d5/L4 | numpy | none | 4748 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | cold | 949 | 5.00 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | warm | 69 | 68.47 | 0 / 1031790 |
| quad/sg/equi/b1/d10/L4 | math | none | 652 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | cold | 399 | 1.63 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | warm | 85 | 7.70 | 0 / 1221805 |
| quad/sg/equi/b1/d10/L4 | numpy | none | 5102 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | cold | 2097 | 2.43 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | warm | 88 | 58.30 | 0 / 1221805 |
| interp/sg/cc/b0/d5/L3/p8-shifted | math | none | 591 | 1.00 | 440784 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | math | cold | 110 | 5.38 | 12033 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | math | warm (by quadrature) | 92 | 6.40 | 0 / 2203920 |
| interp/sg/cc/b0/d5/L3/p8-shifted | numpy | none | 4768 | 1.00 | 440784 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | numpy | cold | 227 | 20.97 | 12033 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | numpy | warm (by quadrature) | 91 | 52.19 | 0 / 2203920 |

threads: 16

| case | callable | variant | ns/eval | speedup | Python calls / evals |
|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | math | none | 23929 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | cold | 4409 | 5.43 | 36102 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | warm | 18 | 1309.55 | 0 / 1031790 |
| quad/sg/cc/b0/d5/L4 | numpy | none | 29939 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | cold | 5464 | 5.48 | 36086 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | warm | 12 | 2504.66 | 0 / 1031790 |
| quad/sg/equi/b1/d10/L4 | math | none | 23454 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | cold | 9145 | 2.56 | 90587 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | warm | 13 | 1843.31 | 0 / 1221805 |
| quad/sg/equi/b1/d10/L4 | numpy | none | 29806 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | cold | 11383 | 2.62 | 90587 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | warm | 12 | 2447.71 | 0 / 1221805 |
| interp/sg/cc/b0/d5/L3/p8-shifted | math | none | 23777 | 1.00 | 440784 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | math | cold | 690 | 34.45 | 12074 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | math | warm (by quadrature) | 14 | 1720.44 | 0 / 2203920 |
| interp/sg/cc/b0/d5/L3/p8-shifted | numpy | none | 29650 | 1.00 | 440784 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | numpy | cold | 823 | 36.04 | 12078 / 440784 |
| interp/sg/cc/b0/d5/L3/p8-shifted | numpy | warm (by quadrature) | 14 | 2089.23 | 0 / 2203920 |
(The "Python calls / evals" column of the warm rows counts misses / lookups over the 5 timed calls.)
