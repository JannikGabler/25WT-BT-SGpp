# WP-24 vector-valued source functions: prototype measurements (micro/vec/...)

Snapshot WP-24-before (judged build, 1 thread, OMP_PLACES={4}), 3 runs (`perf.py run micro --filter '^micro/vec/'`, separate processes), raw: `vector-prototypes-run-*.json`. Per case: median ns per grid point (and output) over all samples of the 3 runs; ratios per run, median [min, max]. Prototypes: `combigrid/benchmarks/perf/perf_vector_proto.hpp`. Analysis: `perf_vector_proto.hpp` header comment + WP-24 summary.

## Result reproduction (ulp distance per tensor grid, vector component k vs. scalar run k)

Judged build (FMA contraction on):
```
checked 222
  dsg/batch                    cases  10  nonzero   4  max 1
  dsg/batch_direct             cases   2  nonzero   0  max 0
  dsg/rt                       cases  10  nonzero   0  max 0
  dsg/rt_direct                cases   2  nonzero   0  max 0
  dsg/scalar                   cases   2  nonzero   0  max 0
  interp/inner                 cases  20  nonzero  20  max 36815
  interp/inner_direct          cases   4  nonzero   4  max 27
  interp/innerk                cases  20  nonzero   0  max 0
  interp/innerk_direct         cases   4  nonzero   0  max 0
  interp/outer                 cases  20  nonzero   0  max 0
  interp/outer_direct          cases   4  nonzero   0  max 0
  interp/scalar                cases   4  nonzero   0  max 0
  quad/batch                   cases  15  nonzero  13  max 14
  quad/batch_direct            cases   5  nonzero   2  max 1
  quad/batch_nested            cases   5  nonzero   2  max 1
  quad/ct                      cases  15  nonzero  13  max 14
  quad/ct_direct               cases   5  nonzero   2  max 1
  quad/rowbuf                  cases  15  nonzero  13  max 14
  quad/rowbuf_direct           cases   5  nonzero   2  max 1
  quad/rowbufk                 cases  15  nonzero  13  max 14
  quad/rowbufk_direct          cases   5  nonzero   2  max 1
  quad/rt                      cases  15  nonzero  13  max 14
  quad/rt_direct               cases   5  nonzero   2  max 1
  quad/rt_nested               cases   5  nonzero   2  max 1
  quad/scalar                  cases   5  nonzero   2  max 1
  quad/tvt_scalar              cases   5  nonzero   2  max 1
```

Same benchmark TU compiled with `-ffp-contract=off` (libraries unchanged; the remaining 3 nonzero cases compare the uncontracted copy with the contracted production library):
```
checked 222
  dsg/batch                    cases  10  nonzero   0  max 0
  dsg/batch_direct             cases   2  nonzero   0  max 0
  dsg/rt                       cases  10  nonzero   0  max 0
  dsg/rt_direct                cases   2  nonzero   0  max 0
  dsg/scalar                   cases   2  nonzero   0  max 0
  interp/inner                 cases  20  nonzero   0  max 0
  interp/inner_direct          cases   4  nonzero   0  max 0
  interp/innerk                cases  20  nonzero   0  max 0
  interp/innerk_direct         cases   4  nonzero   0  max 0
  interp/outer                 cases  20  nonzero   0  max 0
  interp/outer_direct          cases   4  nonzero   0  max 0
  interp/scalar                cases   4  nonzero   1  max 2
  quad/batch                   cases  15  nonzero   0  max 0
  quad/batch_direct            cases   5  nonzero   0  max 0
  quad/batch_nested            cases   5  nonzero   0  max 0
  quad/ct                      cases  15  nonzero   0  max 0
  quad/ct_direct               cases   5  nonzero   0  max 0
  quad/rowbuf                  cases  15  nonzero   0  max 0
  quad/rowbuf_direct           cases   5  nonzero   0  max 0
  quad/rowbufk                 cases  15  nonzero   0  max 0
  quad/rowbufk_direct          cases   5  nonzero   0  max 0
  quad/rt                      cases  15  nonzero   0  max 0
  quad/rt_direct               cases   5  nonzero   0  max 0
  quad/rt_nested               cases   5  nonzero   0  max 0
  quad/scalar                  cases   5  nonzero   2  max 1
  quad/tvt_scalar              cases   5  nonzero   0  max 0
```

## Scalar cost (m = 1, e2e integrand): time / today (> 1: slower)

| op | config | today ns/pt | variant | ns/pt | variant / today [min, max] |
|---|---|---|---|---|---|
| dsg | equi/b0/d10/L7 | 26.64 | rt_direct | 30.52 | 1.140 [1.123, 1.151] |
| dsg | equi/b0/d10/L7 | 26.64 | batch_direct | 30.38 | 1.140 [1.135, 1.181] |
| dsg | equi/b1/d20/L6 | 29.99 | rt_direct | 38.12 | 1.268 [1.268, 1.284] |
| dsg | equi/b1/d20/L6 | 29.99 | batch_direct | 34.90 | 1.164 [1.163, 1.164] |
| interp | cc/b0/d10/L2 | 259.76 | inner_direct | 271.26 | 1.034 [1.032, 1.044] |
| interp | cc/b0/d10/L2 | 259.76 | innerk_direct | 270.41 | 1.035 [1.034, 1.041] |
| interp | cc/b0/d10/L2 | 259.76 | outer_direct | 267.95 | 1.028 [1.026, 1.032] |
| interp | cc/b0/d2/L12 | 56.22 | inner_direct | 65.38 | 1.162 [1.135, 1.163] |
| interp | cc/b0/d2/L12 | 56.22 | innerk_direct | 63.64 | 1.132 [1.102, 1.134] |
| interp | cc/b0/d2/L12 | 56.22 | outer_direct | 60.60 | 1.078 [1.052, 1.078] |
| interp | cc/b0/d5/L6 | 109.18 | inner_direct | 118.82 | 1.087 [1.083, 1.088] |
| interp | cc/b0/d5/L6 | 109.18 | innerk_direct | 117.90 | 1.080 [1.067, 1.080] |
| interp | cc/b0/d5/L6 | 109.18 | outer_direct | 116.31 | 1.061 [1.060, 1.067] |
| interp | cheb1/b0/d5/L4 | 198.18 | inner_direct | 208.42 | 1.055 [1.047, 1.189] |
| interp | cheb1/b0/d5/L4 | 198.18 | innerk_direct | 195.84 | 0.989 [0.984, 0.991] |
| interp | cheb1/b0/d5/L4 | 198.18 | outer_direct | 198.10 | 1.001 [0.997, 1.001] |
| quad | cc/b0/d10/L4 | 21.52 | tvt_scalar | 21.42 | 0.994 [0.987, 1.001] |
| quad | cc/b0/d10/L4 | 21.52 | rt_nested | 29.82 | 1.386 [1.338, 1.393] |
| quad | cc/b0/d10/L4 | 21.52 | rt_direct | 28.95 | 1.345 [1.302, 1.357] |
| quad | cc/b0/d10/L4 | 21.52 | ct_direct | 24.78 | 1.133 [1.117, 1.151] |
| quad | cc/b0/d10/L4 | 21.52 | rowbuf_direct | 32.05 | 1.492 [1.444, 1.493] |
| quad | cc/b0/d10/L4 | 21.52 | rowbufk_direct | 32.93 | 1.529 [1.484, 1.538] |
| quad | cc/b0/d10/L4 | 21.52 | batch_nested | 28.39 | 1.319 [1.279, 1.334] |
| quad | cc/b0/d10/L4 | 21.52 | batch_direct | 27.99 | 1.298 [1.261, 1.310] |
| quad | equi/b0/d10/L4 | 22.01 | tvt_scalar | 22.00 | 1.000 [0.995, 1.004] |
| quad | equi/b0/d10/L4 | 22.01 | rt_nested | 29.84 | 1.358 [1.354, 1.362] |
| quad | equi/b0/d10/L4 | 22.01 | rt_direct | 29.02 | 1.318 [1.314, 1.328] |
| quad | equi/b0/d10/L4 | 22.01 | ct_direct | 24.93 | 1.137 [1.121, 1.148] |
| quad | equi/b0/d10/L4 | 22.01 | rowbuf_direct | 31.32 | 1.418 [1.413, 1.437] |
| quad | equi/b0/d10/L4 | 22.01 | rowbufk_direct | 32.86 | 1.496 [1.492, 1.499] |
| quad | equi/b0/d10/L4 | 22.01 | batch_nested | 28.59 | 1.300 [1.295, 1.304] |
| quad | equi/b0/d10/L4 | 22.01 | batch_direct | 27.90 | 1.267 [1.263, 1.275] |
| quad | equi/b0/d2/L16 | 15.21 | tvt_scalar | 15.22 | 1.001 [0.989, 1.003] |
| quad | equi/b0/d2/L16 | 15.21 | rt_nested | 22.53 | 1.484 [1.463, 1.486] |
| quad | equi/b0/d2/L16 | 15.21 | rt_direct | 22.01 | 1.449 [1.427, 1.452] |
| quad | equi/b0/d2/L16 | 15.21 | ct_direct | 18.39 | 1.214 [1.200, 1.240] |
| quad | equi/b0/d2/L16 | 15.21 | rowbuf_direct | 25.54 | 1.685 [1.658, 1.708] |
| quad | equi/b0/d2/L16 | 15.21 | rowbufk_direct | 26.59 | 1.749 [1.728, 1.749] |
| quad | equi/b0/d2/L16 | 15.21 | batch_nested | 21.79 | 1.433 [1.420, 1.437] |
| quad | equi/b0/d2/L16 | 15.21 | batch_direct | 21.41 | 1.409 [1.386, 1.413] |
| quad | equi/b0/d5/L10 | 18.26 | tvt_scalar | 18.19 | 0.997 [0.975, 0.997] |
| quad | equi/b0/d5/L10 | 18.26 | rt_nested | 25.39 | 1.393 [1.352, 1.592] |
| quad | equi/b0/d5/L10 | 18.26 | rt_direct | 25.00 | 1.371 [1.340, 1.372] |
| quad | equi/b0/d5/L10 | 18.26 | ct_direct | 21.53 | 1.162 [1.154, 1.188] |
| quad | equi/b0/d5/L10 | 18.26 | rowbuf_direct | 28.07 | 1.540 [1.502, 1.544] |
| quad | equi/b0/d5/L10 | 18.26 | rowbufk_direct | 29.86 | 1.634 [1.601, 1.641] |
| quad | equi/b0/d5/L10 | 18.26 | batch_nested | 24.97 | 1.367 [1.339, 1.368] |
| quad | equi/b0/d5/L10 | 18.26 | batch_direct | 24.52 | 1.343 [1.315, 1.344] |
| quad | equi/b1/d20/L4/all | 27.94 | tvt_scalar | 27.24 | 0.974 [0.970, 0.977] |
| quad | equi/b1/d20/L4/all | 27.94 | rt_nested | 36.46 | 1.305 [1.297, 1.307] |
| quad | equi/b1/d20/L4/all | 27.94 | rt_direct | 35.73 | 1.277 [1.272, 1.281] |
| quad | equi/b1/d20/L4/all | 27.94 | ct_direct | 30.90 | 1.105 [1.104, 1.106] |
| quad | equi/b1/d20/L4/all | 27.94 | rowbuf_direct | 36.33 | 1.299 [1.295, 1.300] |
| quad | equi/b1/d20/L4/all | 27.94 | rowbufk_direct | 34.70 | 1.242 [1.236, 1.244] |
| quad | equi/b1/d20/L4/all | 27.94 | batch_nested | 32.54 | 1.165 [1.157, 1.166] |
| quad | equi/b1/d20/L4/all | 27.94 | batch_direct | 32.08 | 1.148 [1.142, 1.148] |

## Vector gain: m scalar runs / one vector run (> 1: vector faster); ns per point and output

| op | config | m | m scalar runs | variant | vector | gain [min, max] |
|---|---|---|---|---|---|---|
| dsg | equi/b0/d10/L7 | 1 | 57.53 | rt | 60.53 | 0.949 [0.948, 0.954] |
| dsg | equi/b0/d10/L7 | 1 | 57.53 | batch | 61.56 | 0.934 [0.932, 0.940] |
| dsg | equi/b0/d10/L7 | 2 | 57.44 | rt | 39.69 | 1.447 [1.445, 1.451] |
| dsg | equi/b0/d10/L7 | 2 | 57.44 | batch | 37.34 | 1.538 [1.537, 1.539] |
| dsg | equi/b0/d10/L7 | 4 | 57.47 | rt | 32.50 | 1.768 [1.768, 1.770] |
| dsg | equi/b0/d10/L7 | 4 | 57.47 | batch | 30.83 | 1.864 [1.863, 1.867] |
| dsg | equi/b0/d10/L7 | 8 | 57.47 | rt | 25.04 | 2.295 [2.295, 2.295] |
| dsg | equi/b0/d10/L7 | 8 | 57.47 | batch | 26.00 | 2.210 [2.206, 2.259] |
| dsg | equi/b0/d10/L7 | 32 | 57.47 | rt | 20.76 | 2.765 [2.765, 2.784] |
| dsg | equi/b0/d10/L7 | 32 | 57.47 | batch | 21.77 | 2.646 [2.556, 2.648] |
| dsg | equi/b1/d20/L6 | 1 | 49.52 | rt | 54.12 | 0.915 [0.914, 0.917] |
| dsg | equi/b1/d20/L6 | 1 | 49.52 | batch | 54.20 | 0.913 [0.912, 0.919] |
| dsg | equi/b1/d20/L6 | 2 | 49.53 | rt | 38.13 | 1.300 [1.296, 1.300] |
| dsg | equi/b1/d20/L6 | 2 | 49.53 | batch | 36.03 | 1.374 [1.372, 1.374] |
| dsg | equi/b1/d20/L6 | 4 | 49.48 | rt | 34.53 | 1.434 [1.429, 1.459] |
| dsg | equi/b1/d20/L6 | 4 | 49.48 | batch | 33.30 | 1.488 [1.482, 1.513] |
| dsg | equi/b1/d20/L6 | 8 | 49.41 | rt | 28.96 | 1.706 [1.706, 1.707] |
| dsg | equi/b1/d20/L6 | 8 | 49.41 | batch | 29.02 | 1.702 [1.698, 1.703] |
| dsg | equi/b1/d20/L6 | 32 | 49.52 | rt | 26.15 | 1.894 [1.888, 1.900] |
| dsg | equi/b1/d20/L6 | 32 | 49.52 | batch | 27.18 | 1.822 [1.821, 1.825] |
| interp | cc/b0/d10/L2 | 1 | 270.15 | inner | 285.26 | 0.949 [0.946, 0.954] |
| interp | cc/b0/d10/L2 | 1 | 270.15 | innerk | 284.51 | 0.950 [0.950, 0.957] |
| interp | cc/b0/d10/L2 | 1 | 270.15 | outer | 285.21 | 0.954 [0.945, 0.972] |
| interp | cc/b0/d10/L2 | 2 | 270.05 | inner | 246.49 | 1.096 [1.094, 1.099] |
| interp | cc/b0/d10/L2 | 2 | 270.05 | innerk | 246.41 | 1.096 [1.096, 1.097] |
| interp | cc/b0/d10/L2 | 2 | 270.05 | outer | 251.04 | 1.078 [1.056, 1.081] |
| interp | cc/b0/d10/L2 | 4 | 271.62 | inner | 213.94 | 1.270 [1.266, 1.279] |
| interp | cc/b0/d10/L2 | 4 | 271.62 | innerk | 214.47 | 1.266 [1.262, 1.275] |
| interp | cc/b0/d10/L2 | 4 | 271.62 | outer | 220.98 | 1.229 [1.223, 1.239] |
| interp | cc/b0/d10/L2 | 8 | 274.61 | inner | 203.31 | 1.349 [1.347, 1.354] |
| interp | cc/b0/d10/L2 | 8 | 274.61 | innerk | 206.16 | 1.341 [1.332, 1.342] |
| interp | cc/b0/d10/L2 | 8 | 274.61 | outer | 213.37 | 1.294 [1.287, 1.296] |
| interp | cc/b0/d10/L2 | 32 | 287.30 | inner | 193.78 | 1.482 [1.451, 1.484] |
| interp | cc/b0/d10/L2 | 32 | 287.30 | innerk | 195.34 | 1.471 [1.440, 1.472] |
| interp | cc/b0/d10/L2 | 32 | 287.30 | outer | 204.22 | 1.404 [1.373, 1.408] |
| interp | cc/b0/d2/L12 | 1 | 57.52 | inner | 67.41 | 0.858 [0.844, 0.871] |
| interp | cc/b0/d2/L12 | 1 | 57.52 | innerk | 65.67 | 0.877 [0.870, 0.894] |
| interp | cc/b0/d2/L12 | 1 | 57.52 | outer | 62.89 | 0.913 [0.912, 0.933] |
| interp | cc/b0/d2/L12 | 2 | 57.29 | inner | 50.57 | 1.135 [1.131, 1.136] |
| interp | cc/b0/d2/L12 | 2 | 57.29 | innerk | 50.34 | 1.138 [1.131, 1.141] |
| interp | cc/b0/d2/L12 | 2 | 57.29 | outer | 52.79 | 1.086 [1.083, 1.087] |
| interp | cc/b0/d2/L12 | 4 | 57.34 | inner | 42.69 | 1.343 [1.342, 1.347] |
| interp | cc/b0/d2/L12 | 4 | 57.34 | innerk | 43.39 | 1.322 [1.318, 1.322] |
| interp | cc/b0/d2/L12 | 4 | 57.34 | outer | 48.31 | 1.187 [1.179, 1.187] |
| interp | cc/b0/d2/L12 | 8 | 57.20 | inner | 39.34 | 1.454 [1.453, 1.463] |
| interp | cc/b0/d2/L12 | 8 | 57.20 | innerk | 40.36 | 1.421 [1.411, 1.425] |
| interp | cc/b0/d2/L12 | 8 | 57.20 | outer | 46.68 | 1.228 [1.225, 1.237] |
| interp | cc/b0/d2/L12 | 32 | 58.13 | inner | 36.54 | 1.591 [1.585, 1.595] |
| interp | cc/b0/d2/L12 | 32 | 58.13 | innerk | 38.20 | 1.522 [1.520, 1.528] |
| interp | cc/b0/d2/L12 | 32 | 58.13 | outer | 45.47 | 1.278 [1.276, 1.282] |
| interp | cc/b0/d5/L6 | 1 | 109.92 | inner | 122.29 | 0.901 [0.891, 0.903] |
| interp | cc/b0/d5/L6 | 1 | 109.92 | innerk | 121.78 | 0.907 [0.899, 0.907] |
| interp | cc/b0/d5/L6 | 1 | 109.92 | outer | 119.58 | 0.923 [0.908, 0.924] |
| interp | cc/b0/d5/L6 | 2 | 109.90 | inner | 103.38 | 1.064 [1.057, 1.065] |
| interp | cc/b0/d5/L6 | 2 | 109.90 | innerk | 103.69 | 1.060 [1.059, 1.064] |
| interp | cc/b0/d5/L6 | 2 | 109.90 | outer | 107.25 | 1.027 [1.021, 1.030] |
| interp | cc/b0/d5/L6 | 4 | 110.06 | inner | 96.14 | 1.146 [1.145, 1.157] |
| interp | cc/b0/d5/L6 | 4 | 110.06 | innerk | 96.37 | 1.142 [1.134, 1.148] |
| interp | cc/b0/d5/L6 | 4 | 110.06 | outer | 102.86 | 1.070 [1.066, 1.077] |
| interp | cc/b0/d5/L6 | 8 | 110.06 | inner | 91.43 | 1.207 [1.202, 1.208] |
| interp | cc/b0/d5/L6 | 8 | 110.06 | innerk | 92.92 | 1.188 [1.183, 1.188] |
| interp | cc/b0/d5/L6 | 8 | 110.06 | outer | 100.17 | 1.102 [1.098, 1.102] |
| interp | cc/b0/d5/L6 | 32 | 111.09 | inner | 87.77 | 1.266 [1.264, 1.267] |
| interp | cc/b0/d5/L6 | 32 | 111.09 | innerk | 89.64 | 1.239 [1.238, 1.240] |
| interp | cc/b0/d5/L6 | 32 | 111.09 | outer | 98.34 | 1.130 [1.128, 1.132] |
| interp | cheb1/b0/d5/L4 | 1 | 190.18 | inner | 217.44 | 0.896 [0.827, 0.897] |
| interp | cheb1/b0/d5/L4 | 1 | 190.18 | innerk | 203.51 | 0.934 [0.919, 0.985] |
| interp | cheb1/b0/d5/L4 | 1 | 190.18 | outer | 204.02 | 0.931 [0.897, 0.974] |
| interp | cheb1/b0/d5/L4 | 2 | 190.17 | inner | 148.80 | 1.276 [1.228, 1.279] |
| interp | cheb1/b0/d5/L4 | 2 | 190.17 | innerk | 145.58 | 1.304 [1.299, 1.321] |
| interp | cheb1/b0/d5/L4 | 2 | 190.17 | outer | 191.85 | 0.993 [0.989, 1.007] |
| interp | cheb1/b0/d5/L4 | 4 | 190.22 | inner | 118.90 | 1.600 [1.598, 2.399] |
| interp | cheb1/b0/d5/L4 | 4 | 190.22 | innerk | 118.05 | 1.611 [1.609, 2.479] |
| interp | cheb1/b0/d5/L4 | 4 | 190.22 | outer | 187.44 | 1.015 [1.013, 1.575] |
| interp | cheb1/b0/d5/L4 | 8 | 190.51 | inner | 102.68 | 1.858 [1.853, 2.839] |
| interp | cheb1/b0/d5/L4 | 8 | 190.51 | innerk | 103.31 | 1.848 [1.841, 2.882] |
| interp | cheb1/b0/d5/L4 | 8 | 190.51 | outer | 182.60 | 1.043 [1.042, 1.597] |
| interp | cheb1/b0/d5/L4 | 32 | 190.55 | inner | 91.80 | 2.076 [2.076, 2.200] |
| interp | cheb1/b0/d5/L4 | 32 | 190.55 | innerk | 93.10 | 2.051 [2.040, 2.178] |
| interp | cheb1/b0/d5/L4 | 32 | 190.55 | outer | 180.56 | 1.055 [1.051, 1.125] |
| quad | equi/b0/d10/L4 | 1 | 52.65 | rt | 57.32 | 0.918 [0.915, 0.920] |
| quad | equi/b0/d10/L4 | 1 | 52.65 | ct | 54.72 | 0.962 [0.961, 0.964] |
| quad | equi/b0/d10/L4 | 1 | 52.65 | rowbuf | 58.44 | 0.901 [0.900, 0.902] |
| quad | equi/b0/d10/L4 | 1 | 52.65 | rowbufk | 63.09 | 0.834 [0.833, 0.835] |
| quad | equi/b0/d10/L4 | 1 | 52.65 | batch | 57.94 | 0.909 [0.907, 0.912] |
| quad | equi/b0/d10/L4 | 2 | 52.69 | rt | 38.01 | 1.386 [1.386, 1.388] |
| quad | equi/b0/d10/L4 | 2 | 52.69 | ct | 36.47 | 1.445 [1.443, 1.446] |
| quad | equi/b0/d10/L4 | 2 | 52.69 | rowbuf | 35.56 | 1.482 [1.477, 1.485] |
| quad | equi/b0/d10/L4 | 2 | 52.69 | rowbufk | 37.45 | 1.408 [1.405, 1.409] |
| quad | equi/b0/d10/L4 | 2 | 52.69 | batch | 35.50 | 1.483 [1.483, 1.484] |
| quad | equi/b0/d10/L4 | 4 | 52.68 | rt | 31.57 | 1.669 [1.662, 1.699] |
| quad | equi/b0/d10/L4 | 4 | 52.68 | ct | 30.16 | 1.747 [1.738, 1.748] |
| quad | equi/b0/d10/L4 | 4 | 52.68 | rowbuf | 29.64 | 1.778 [1.736, 1.782] |
| quad | equi/b0/d10/L4 | 4 | 52.68 | rowbufk | 31.23 | 1.686 [1.681, 1.688] |
| quad | equi/b0/d10/L4 | 4 | 52.68 | batch | 30.01 | 1.754 [1.754, 1.755] |
| quad | equi/b0/d10/L4 | 8 | 52.62 | rt | 24.83 | 2.119 [2.067, 2.126] |
| quad | equi/b0/d10/L4 | 8 | 52.62 | ct | 24.20 | 2.173 [2.171, 2.176] |
| quad | equi/b0/d10/L4 | 8 | 52.62 | rowbuf | 24.06 | 2.186 [2.178, 2.189] |
| quad | equi/b0/d10/L4 | 8 | 52.62 | rowbufk | 25.46 | 2.066 [2.064, 2.066] |
| quad | equi/b0/d10/L4 | 8 | 52.62 | batch | 24.71 | 2.128 [2.123, 2.129] |
| quad | equi/b0/d10/L4 | 32 | 52.72 | rt | 20.62 | 2.556 [2.547, 2.562] |
| quad | equi/b0/d10/L4 | 32 | 52.72 | ct | 20.64 | 2.556 [2.542, 2.557] |
| quad | equi/b0/d10/L4 | 32 | 52.72 | rowbuf | 20.56 | 2.564 [2.564, 2.575] |
| quad | equi/b0/d10/L4 | 32 | 52.72 | rowbufk | 21.62 | 2.441 [2.346, 2.449] |
| quad | equi/b0/d10/L4 | 32 | 52.72 | batch | 21.77 | 2.423 [2.420, 2.426] |
| quad | equi/b0/d2/L16 | 1 | 16.96 | rt | 24.79 | 0.685 [0.675, 0.686] |
| quad | equi/b0/d2/L16 | 1 | 16.96 | ct | 21.24 | 0.798 [0.780, 0.800] |
| quad | equi/b0/d2/L16 | 1 | 16.96 | rowbuf | 27.31 | 0.621 [0.621, 0.621] |
| quad | equi/b0/d2/L16 | 1 | 16.96 | rowbufk | 29.39 | 0.578 [0.575, 0.578] |
| quad | equi/b0/d2/L16 | 1 | 16.96 | batch | 23.69 | 0.717 [0.716, 0.717] |
| quad | equi/b0/d2/L16 | 2 | 16.97 | rt | 20.80 | 0.816 [0.813, 0.817] |
| quad | equi/b0/d2/L16 | 2 | 16.97 | ct | 18.34 | 0.925 [0.925, 0.926] |
| quad | equi/b0/d2/L16 | 2 | 16.97 | rowbuf | 20.41 | 0.831 [0.830, 0.833] |
| quad | equi/b0/d2/L16 | 2 | 16.97 | rowbufk | 21.60 | 0.786 [0.785, 0.789] |
| quad | equi/b0/d2/L16 | 2 | 16.97 | batch | 19.50 | 0.870 [0.869, 0.871] |
| quad | equi/b0/d2/L16 | 4 | 16.95 | rt | 16.79 | 1.011 [1.010, 1.012] |
| quad | equi/b0/d2/L16 | 4 | 16.95 | ct | 16.03 | 1.058 [1.054, 1.060] |
| quad | equi/b0/d2/L16 | 4 | 16.95 | rowbuf | 15.83 | 1.070 [0.998, 1.073] |
| quad | equi/b0/d2/L16 | 4 | 16.95 | rowbufk | 17.15 | 0.989 [0.944, 0.991] |
| quad | equi/b0/d2/L16 | 4 | 16.95 | batch | 16.64 | 1.019 [1.017, 1.022] |
| quad | equi/b0/d2/L16 | 8 | 16.92 | rt | 14.29 | 1.187 [1.180, 1.187] |
| quad | equi/b0/d2/L16 | 8 | 16.92 | ct | 13.86 | 1.221 [1.216, 1.224] |
| quad | equi/b0/d2/L16 | 8 | 16.92 | rowbuf | 13.90 | 1.216 [1.215, 1.220] |
| quad | equi/b0/d2/L16 | 8 | 16.92 | rowbufk | 15.09 | 1.120 [1.120, 1.125] |
| quad | equi/b0/d2/L16 | 8 | 16.92 | batch | 14.63 | 1.159 [1.156, 1.159] |
| quad | equi/b0/d2/L16 | 32 | 16.96 | rt | 12.72 | 1.333 [1.333, 1.365] |
| quad | equi/b0/d2/L16 | 32 | 16.96 | ct | 12.60 | 1.347 [1.345, 1.373] |
| quad | equi/b0/d2/L16 | 32 | 16.96 | rowbuf | 12.62 | 1.343 [1.342, 1.373] |
| quad | equi/b0/d2/L16 | 32 | 16.96 | rowbufk | 13.64 | 1.243 [1.242, 1.270] |
| quad | equi/b0/d2/L16 | 32 | 16.96 | batch | 14.17 | 1.197 [1.194, 1.222] |
| quad | equi/b1/d20/L4/all | 1 | 63.25 | rt | 68.17 | 0.928 [0.928, 0.929] |
| quad | equi/b1/d20/L4/all | 1 | 63.25 | ct | 65.46 | 0.966 [0.965, 0.967] |
| quad | equi/b1/d20/L4/all | 1 | 63.25 | rowbuf | 67.40 | 0.938 [0.938, 0.940] |
| quad | equi/b1/d20/L4/all | 1 | 63.25 | rowbufk | 69.97 | 0.904 [0.903, 0.905] |
| quad | equi/b1/d20/L4/all | 1 | 63.25 | batch | 66.99 | 0.944 [0.943, 0.945] |
| quad | equi/b1/d20/L4/all | 2 | 63.24 | rt | 44.08 | 1.435 [1.434, 1.435] |
| quad | equi/b1/d20/L4/all | 2 | 63.24 | ct | 42.61 | 1.483 [1.476, 1.484] |
| quad | equi/b1/d20/L4/all | 2 | 63.24 | rowbuf | 40.47 | 1.563 [1.562, 1.565] |
| quad | equi/b1/d20/L4/all | 2 | 63.24 | rowbufk | 41.83 | 1.511 [1.507, 1.512] |
| quad | equi/b1/d20/L4/all | 2 | 63.24 | batch | 39.94 | 1.583 [1.583, 1.584] |
| quad | equi/b1/d20/L4/all | 4 | 63.31 | rt | 38.08 | 1.664 [1.639, 1.664] |
| quad | equi/b1/d20/L4/all | 4 | 63.31 | ct | 37.57 | 1.686 [1.680, 1.689] |
| quad | equi/b1/d20/L4/all | 4 | 63.31 | rowbuf | 36.10 | 1.753 [1.728, 1.755] |
| quad | equi/b1/d20/L4/all | 4 | 63.31 | rowbufk | 37.11 | 1.704 [1.675, 1.708] |
| quad | equi/b1/d20/L4/all | 4 | 63.31 | batch | 36.54 | 1.732 [1.731, 1.734] |
| quad | equi/b1/d20/L4/all | 8 | 63.28 | rt | 30.85 | 2.053 [1.998, 2.053] |
| quad | equi/b1/d20/L4/all | 8 | 63.28 | ct | 30.44 | 2.079 [2.074, 2.084] |
| quad | equi/b1/d20/L4/all | 8 | 63.28 | rowbuf | 29.85 | 2.122 [2.065, 2.123] |
| quad | equi/b1/d20/L4/all | 8 | 63.28 | rowbufk | 30.73 | 2.060 [2.001, 2.061] |
| quad | equi/b1/d20/L4/all | 8 | 63.28 | batch | 30.57 | 2.070 [2.067, 2.072] |
| quad | equi/b1/d20/L4/all | 32 | 63.29 | rt | 26.70 | 2.370 [2.324, 2.377] |
| quad | equi/b1/d20/L4/all | 32 | 63.29 | ct | 26.70 | 2.371 [2.361, 2.371] |
| quad | equi/b1/d20/L4/all | 32 | 63.29 | rowbuf | 26.53 | 2.386 [2.335, 2.387] |
| quad | equi/b1/d20/L4/all | 32 | 63.29 | rowbufk | 27.27 | 2.320 [2.271, 2.321] |
| quad | equi/b1/d20/L4/all | 32 | 63.29 | batch | 27.66 | 2.288 [2.286, 2.288] |
