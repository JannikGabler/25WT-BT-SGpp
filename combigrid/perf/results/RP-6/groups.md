| group | cases | baseline | rp-5 (direct A) | rp-6 (direct B) | rp-6 (cum. B) | baseline → rp-6 | rp-5 → rp-6 | d ≥ 7: baseline → rp-6 | peak RSS geomean baseline → rp-6 | rp-5 → rp-6 | share of rp-6 suite |
|---|---|---|---|---|---|---|---|---|---|---|---|
| quad/sg | 13 | 22.302 s | 8.617 s | 8.135 s | 8.134 s | 2.742x | 1.059x | 18.49 → 6.74 s (2.74x) | 0.577 | 0.971 | 22.4 % |
| quad/dsg | 9 | 26.744 s | 17.817 s | 17.715 s | 17.702 s | 1.511x | 1.006x | 26.14 → 17.16 s (1.52x) | 0.594 | 1.001 | 48.7 % |
| interp/sg | 8 | 6.600 s | 4.216 s | 4.234 s | 4.234 s | 1.559x | 0.996x | 3.71 → 2.62 s (1.41x) | 0.927 | 1.011 | 11.7 % |
| interp/dsg | 6 | 0.000 s | 0.826 s | 0.828 s | 0.828 s | – | 0.998x | – | – | 1.019 | 2.3 % |
| interplin/sg | 4 | 4.967 s | 3.507 s | 3.544 s | 3.536 s | 1.405x | 0.990x | 4.36 → 3.06 s (1.42x) | 0.999 | 1.006 | 9.7 % |
| interplin/dsg | 4 | 0.000 s | 1.163 s | 0.786 s | 0.785 s | – | 1.479x | – | – | 1.011 | 2.2 % |
| mivec | 4 | 9.275 s | 1.297 s | 1.090 s | 1.093 s | 8.486x | 1.190x | 7.01 → 0.40 s (17.44x) | 0.945 | 1.007 | 3.0 % |
| common | 38 | 69.888 s | 35.455 s | 34.718 s | 34.699 s | 2.014x | 1.021x | 59.71 → 29.98 s (1.99x) | 0.716 | 0.994 | 95.6 % |
| all | 48 | 69.888 s | 37.443 s | 36.332 s | 36.312 s | 1.925x | 1.031x | 59.71 → 29.98 s (1.99x) | 0.716 | 0.999 | 100.0 % |
| expensive | 21 | 0.000 s | 17.746 s | 17.766 s | 17.748 s | – | 0.999x | – | – | 1.015 | – |
| expensive cached | 4 | 0.000 s | 1.066 s | 1.045 s | 1.044 s | – | 1.020x | – | – | 1.009 | – |

quad/dsg b0 15.405 s (42.4 %), b1 2.297 s (6.3 %)

| case | rp-5 | rp-6 | rp-5 → rp-6 (direct, ratio of medians) | gen rp-6 | op rp-6 | peak RSS baseline → rp-5 → rp-6 |
|---|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.890 | 6.859 | 1.003x | 0.65 ms | 6.858 | 37.7 → 30.1 → 29.6 MiB |
| quad/dsg/equi-exp/b0/d10/L7 | 5.428 | 5.406 | 1.004x | 0.47 ms | 5.404 | 36.9 → 29.7 → 30.2 MiB |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 2.400 | 2.476 | 0.969x | 1.17 ms | 2.475 | 12.4 → 12.6 MiB |
| quad/sg/equi-sin/b0/d7/L7 | 2.404 | 2.398 | 1.003x | 1.44 ms | 2.395 | 14.7 → 12.6 → 12.9 MiB |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 2.060 | 2.156 | 0.955x | 1.16 ms | 2.155 | 12.4 → 12.5 MiB |
| interplin/sg/equi/b1/d10/L5/p32 | 1.845 | 1.859 | 0.989x | 1.63 ms | 1.857 | 13.3 → 12.7 → 12.8 MiB |
| interp/sg/cc/b0/d10/L2/p8 | 1.605 | 1.609 | 0.994x | 0.04 ms | 1.609 | 24.6 → 12.2 → 12.1 MiB |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 1.566 | 1.535 | 1.018x | 2.05 ms | 1.534 | 12.8 → 13.0 MiB |
| quad/dsg/cc-uq/b0/d7/L10 | 1.446 | 1.450 | 0.998x | 0.22 ms | 1.449 | 37.4 → 29.3 → 29.6 MiB |
| quad/sg/cc-uq/b0/d10/L4 | 1.310 | 1.306 | 1.001x | 2.04 ms | 1.302 | 13.0 → 12.9 → 13.0 MiB |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 1.289 | 1.292 | 1.000x | 0.05 ms | 1.292 | 12.2 → 12.2 MiB |
| interplin/sg/equi/b1/d8/L5/p256 | 1.196 | 1.205 | 0.991x | 2.19 ms | 1.203 | 12.9 → 12.7 → 12.8 MiB |
| quad/dsg/equi-exp/b1/d15/L7 | 1.169 | 1.162 | 1.007x | 4.09 ms | 1.148 | 233.0 → 51.6 → 51.8 MiB |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 1.179 | 1.153 | 1.023x | 1.87 ms | 1.150 | 13.6 → 13.8 MiB |
| quad/dsg/equi-exp/b0/d7/L10 | 1.191 | 1.145 | 1.037x | 0.18 ms | 1.144 | 37.2 → 29.2 → 29.8 MiB |
| quad/dsg/equi-exp/b1/d20/L6 | 1.138 | 1.135 | 1.002x | 11.33 ms | 1.107 | 332.2 → 82.7 → 82.9 MiB |
| interp/sg/cc/b0/d5/L6/p64 | 1.129 | 1.134 | 0.996x | 0.07 ms | 1.134 | 12.1 → 12.1 → 12.3 MiB |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 1.073 | 1.072 | 1.000x | 0.06 ms | 1.072 | 12.1 → 12.2 MiB |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 1.070 | 1.068 | 0.995x | 2.03 ms | 1.066 | 12.6 → 12.7 MiB |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 1.040 | 1.037 | 1.003x | 2.24 ms | 1.035 | 12.8 → 13.1 MiB |
| interp/sg/cc/b0/d7/L4/p16 | 1.005 | 1.011 | 0.996x | 0.07 ms | 1.011 | 12.7 → 12.1 → 12.2 MiB |
| quad/sg/equi-exp/b0/d10/L4 | 0.915 | 0.904 | 1.014x | 2.08 ms | 0.902 | 12.8 → 12.7 → 13.0 MiB |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 0.881 | 0.881 | 1.000x | 1.13 ms | 0.880 | 12.5 → 12.6 MiB |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 0.827 | 0.825 | 0.992x | 0.04 ms | 0.825 | 12.2 → 12.3 MiB |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 0.777 | 0.691 | 1.127x | 2.66 ms | 0.688 | 16.1 → 13.6 → 13.5 MiB |
| quad/sg/cc-uq/b0/d7/L7 | 0.686 | 0.687 | 0.994x | 1.53 ms | 0.686 | 14.7 → 12.9 → 13.0 MiB |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 0.751 | 0.665 | 1.128x | 3.36 ms | 0.661 | 12.9 → 13.2 MiB |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 0.598 | 0.576 | 1.035x | 2.07 ms | 0.572 | 20.6 → 20.9 MiB |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 0.573 | 0.571 | 1.003x | 2.10 ms | 0.569 | 13.0 → 13.3 MiB |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 0.521 | 0.520 | 1.000x | 1.91 ms | 0.518 | 13.1 → 13.2 MiB |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 0.503 | 0.503 | 1.000x | 0.04 ms | 0.503 | 12.4 → 12.6 MiB |
| quad/sg/equi-exp/b0/d7/L7 | 0.513 | 0.498 | 1.033x | 1.55 ms | 0.496 | 14.4 → 12.7 → 13.0 MiB |
| quad/sg/equi-exp/b1/d15/L6 | 0.503 | 0.491 | 1.024x | 6.69 ms | 0.481 | 53.4 → 18.2 → 18.5 MiB |
| quad/sg/cc-uq/b0/d2/L20 | 0.792 | 0.432 | 1.797x | 70.14 ms | 0.358 | 839.1 → 284.4 → 191.1 MiB |
| interp/dsg/cc/b0/d10/L2/p8 | 0.399 | 0.398 | 0.999x | 0.03 ms | 0.398 | 12.2 → 12.4 MiB |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 0.398 | 0.398 | 1.001x | 0.04 ms | 0.398 | 12.2 → 12.3 MiB |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 0.490 | 0.373 | 1.321x | 7.13 ms | 0.365 | 17.4 → 15.0 → 15.0 MiB |
| interplin/sg/equi/b1/d5/L8/p1024 | 0.357 | 0.361 | 0.986x | 0.08 ms | 0.360 | 13.1 → 12.8 → 12.9 MiB |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 0.350 | 0.350 | 0.999x | 0.32 ms | 0.349 | 15.7 → 16.0 MiB |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 0.345 | 0.345 | 1.001x | 0.03 ms | 0.345 | 12.1 → 12.4 MiB |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 0.333 | 0.333 | 0.996x | 3.23 ms | 0.330 | 13.4 → 13.6 MiB |
| quad/sg/cc-uq/b0/d5/L10 | 0.320 | 0.322 | 0.994x | 1.69 ms | 0.320 | 17.1 → 13.0 → 12.9 MiB |
| quad/sg/equi-exp/b0/d2/L20 | 0.329 | 0.319 | 1.072x | 38.33 ms | 0.280 | 363.9 → 111.4 → 113.7 MiB |
| interplin/dsg/equi/b1/d10/L5/p32 | 0.429 | 0.271 | 1.580x | 1.35 ms | 0.270 | 15.3 → 16.0 MiB |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 0.257 | 0.259 | 0.999x | 2.10 ms | 0.257 | 12.8 → 12.8 MiB |
| quad/sg/equi-exp/b0/d5/L10 | 0.262 | 0.248 | 1.053x | 1.90 ms | 0.246 | 15.4 → 12.6 → 12.9 MiB |
| quad/dsg/cc-uq/b0/d5/L12 | 0.248 | 0.247 | 1.003x | 0.63 ms | 0.246 | 24.0 → 18.1 → 18.5 MiB |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 0.243 | 0.243 | 1.000x | 0.03 ms | 0.243 | 13.5 → 13.4 MiB |
| quad/sg/equi-exp/b1/d10/L7 | 0.293 | 0.241 | 1.214x | 1.56 ms | 0.239 | 25.6 → 13.3 → 13.4 MiB |
| interplin/dsg/equi/b1/d8/L5/p256 | 0.373 | 0.238 | 1.575x | 2.05 ms | 0.235 | 12.7 → 12.4 MiB |
| quad/sg/equi-exp/b1/d20/L5 | 0.213 | 0.213 | 0.999x | 8.56 ms | 0.201 | 60.3 → 20.1 → 20.4 MiB |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 0.232 | 0.209 | 1.110x | 2.09 ms | 0.206 | 13.8 → 14.1 MiB |
| quad/dsg/equi-exp/b0/d5/L12 | 0.212 | 0.203 | 1.044x | 0.48 ms | 0.202 | 24.6 → 18.0 → 18.1 MiB |
| interp/sg/cc/b0/d3/L9/p64 | 0.187 | 0.186 | 1.006x | 0.09 ms | 0.186 | 12.1 → 12.3 → 12.3 MiB |
| interp/dsg/cc/b0/d5/L6/p64 | 0.175 | 0.176 | 0.996x | 0.05 ms | 0.176 | 12.3 → 12.6 MiB |
| interplin/dsg/equi/b1/d2/L16/p16384 | 0.178 | 0.155 | 1.144x | 1.22 ms | 0.150 | 21.4 → 21.2 MiB |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 0.154 | 0.154 | 1.004x | 0.06 ms | 0.144 | 59.2 → 59.5 MiB |
| interp/dsg/cc/b0/d7/L4/p16 | 0.152 | 0.152 | 0.998x | 0.04 ms | 0.152 | 12.4 → 12.5 MiB |
| interp/sg/cc/b0/d2/L12/p64 | 0.128 | 0.128 | 0.993x | 0.22 ms | 0.128 | 14.9 → 14.4 → 14.7 MiB |
| interplin/dsg/equi/b1/d5/L8/p1024 | 0.182 | 0.121 | 1.498x | 0.14 ms | 0.121 | 12.9 → 13.3 MiB |
| interp/sg/cheb1/b0/d5/L4/p64 | 0.116 | 0.119 | 0.993x | 0.04 ms | 0.118 | 11.9 → 12.2 → 12.4 MiB |
| interplin/sg/equi/b1/d2/L16/p16384 | 0.109 | 0.112 | 0.984x | 2.32 ms | 0.105 | 19.3 → 19.7 → 20.5 MiB |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 0.099 | 0.099 | 0.999x | 2.09 ms | 0.097 | 13.0 → 13.0 MiB |
| quad/dsg/equi-exp/b0/d3/L16 | 0.095 | 0.094 | 1.016x | 0.84 ms | 0.091 | 62.1 → 43.8 → 44.3 MiB |
| quad/sg/cc-uq/b0/d3/L14 | 0.076 | 0.076 | 1.013x | 1.26 ms | 0.075 | 37.6 → 17.5 → 14.9 MiB |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 0.071 | 0.071 | 0.997x | 0.04 ms | 0.071 | 13.2 → 13.5 MiB |
| interp/dsg/cc/b0/d2/L12/p64 | 0.052 | 0.052 | 0.993x | 0.22 ms | 0.052 | 15.2 → 15.4 MiB |
| interp/dsg/cc/b0/d3/L9/p64 | 0.043 | 0.043 | 1.000x | 0.06 ms | 0.043 | 12.4 → 12.7 MiB |
| interp/sg/cheb1/b0/d3/L6/p64 | 0.034 | 0.034 | 1.006x | 0.04 ms | 0.034 | 11.9 → 12.1 → 12.4 MiB |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 0.027 | 0.026 | 0.999x | 2.39 ms | 0.024 | 12.7 → 13.1 → 13.4 MiB |
| interp/sg/equi/b0/d3/L5/p64 | 0.012 | 0.012 | 1.003x | 0.03 ms | 0.012 | 11.9 → 12.3 → 12.3 MiB |
| interp/dsg/equi/b0/d3/L5/p64 | 0.007 | 0.007 | 0.994x | 0.03 ms | 0.006 | 12.2 → 12.4 MiB |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 0.003 | 0.003 | 1.000x | 0.76 ms | 0.002 | 12.2 → 12.5 → 12.7 MiB |
