Total time per case [s], 16 threads, median over the rounds (spread max/min − 1 of the rounds in the last column for head); ratio = baseline (or first state) / head.

| case | baseline | rp-1 | rp-2 | rp-3 | rp-4 | rp-5 | head | first/head | head spread |
|---|---|---|---|---|---|---|---|---|---|
| quad/sg/cc-exp/b1/d10/L4/c100us | 1.570 | 1.577 | 1.579 | 1.608 | 1.586 | 1.574 | 1.535 | 1.02x (baseline) | 0.2% |
| quad/dsg/cc-exp/b1/d10/L4/c100us | 0.574 | 0.574 | 0.571 | 0.573 | 0.572 | 0.571 | 0.573 | 1.00x (baseline) | 2.6% |
| quad/sg/cc-exp/b1/d20/L3/c100us | 1.042 | 1.040 | 1.039 | 1.039 | 1.040 | 1.041 | 1.038 | 1.00x (baseline) | 0.6% |
| quad/dsg/cc-exp/b1/d20/L3/c100us | 0.525 | 0.525 | 0.521 | 0.522 | 0.521 | 0.521 | 0.520 | 1.01x (baseline) | 0.2% |
| mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 2.210 | 2.231 | 2.214 | 2.059 | 2.059 | 2.060 | 2.156 | 1.02x (baseline) | 0.1% |
| mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 1.030 | 1.079 | 1.032 | 0.891 | 0.882 | 0.880 | 0.881 | 1.17x (baseline) | 0.4% |
| interp/sg/cc-exp/b1/d10/L3/p8/c100us | 1.074 | 1.074 | 1.074 | 1.073 | 1.072 | 1.074 | 1.074 | 1.00x (baseline) | 0.1% |
| interp/dsg/cc-exp/b1/d10/L3/p8/c100us | – | – | – | – | 0.503 | 0.503 | 0.503 | 1.00x (rp-4) | 0.1% |
| interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 0.825 | 0.829 | 0.900 | 0.825 | 0.833 | 0.835 | 0.827 | 1.00x (baseline) | 9.2% |
| interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | – | – | – | – | 0.346 | 0.345 | 0.346 | 1.00x (rp-4) | 8.3% |
| quad/sg/cc-exp/b1/d15/L5/c1us | 1.344 | 1.195 | 1.201 | 1.193 | 1.194 | 1.179 | 1.156 | 1.16x (baseline) | 2.4% |
| quad/dsg/cc-exp/b1/d15/L5/c1us | 0.422 | 0.423 | 0.355 | 0.357 | 0.356 | 0.350 | 0.349 | 1.21x (baseline) | 0.2% |
| mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 29.106 | 31.075 | 27.757 | 0.764 | 0.756 | 0.767 | 0.661 | 44.03x (baseline) | 1.3% |
| mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 28.498 | 30.610 | 27.300 | 0.335 | 0.335 | 0.333 | 0.333 | 85.56x (baseline) | 0.2% |
| interp/sg/cc-exp/b1/d10/L4/p16/c1us | 0.297 | 0.280 | 0.280 | 0.280 | 0.257 | 0.259 | 0.260 | 1.14x (baseline) | 30.1% |
| interp/dsg/cc-exp/b1/d10/L4/p16/c1us | – | – | – | – | 0.099 | 0.099 | 0.099 | 1.00x (rp-4) | 5.1% |
| interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 1.206 | 1.181 | 1.189 | 1.266 | 1.077 | 1.068 | 1.070 | 1.13x (baseline) | 1.5% |
| interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | – | – | – | – | 0.232 | 0.234 | 0.207 | 1.12x (rp-4) | 0.2% |
| mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 2.551 | 2.570 | 2.550 | 2.399 | 2.398 | 2.400 | 2.475 | 1.03x (baseline) | 0.2% |
| interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 0.399 | 0.398 | 0.401 | 0.399 | 0.398 | 0.400 | 0.398 | 1.00x (baseline) | 0.2% |
| interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 1.294 | 1.288 | 1.288 | 1.293 | 1.288 | 1.288 | 1.287 | 1.00x (baseline) | 1.4% |
| quad/sg/cc-exp/b1/d10/L4/c100us-cached | – | – | – | – | 0.599 | 0.613 | 0.576 | 1.04x (rp-4) | 0.4% |
| interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | – | – | – | – | 0.070 | 0.070 | 0.070 | 1.00x (rp-4) | 0.5% |
| interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | – | – | – | – | 0.245 | 0.243 | 0.244 | 1.00x (rp-4) | 9.9% |
| interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | – | – | – | – | 0.153 | 0.153 | 0.153 | 1.00x (rp-4) | 3.0% |

Sum over 17 cases present at all states: baseline 73.97 s, rp-1 77.95 s, rp-2 71.25 s, rp-3 16.88 s, rp-4 16.62 s, rp-5 16.60 s, head 16.60 s

Sum over 8 cases from rp-4 on: rp-4 2.25 s, rp-5 2.26 s, head 2.20 s

States: baseline = fed2cdb4e (snapshot `baseline`), rp-1 … rp-5 = the "Accept RP-N" commits
(b0b7faac7, 50a8737e0, e1257538d, cf698f31d, d426bddcf; snapshots `rp-N`), head = 59f2899e4
(snapshot `WP-76-after`). Driver: `perf_e2e_history.cpp` (the group's block of the patched
perf_e2e.cpp, built by `build_history.sh` against each state's headers and snapshot libraries;
DSG interpolation and the cache exist from rp-4 on). `run_history.sh 3`: 3 interleaved rounds,
`perf.py run e2e` (16 threads, one process per case, warm-up + sample), 2026-11-17 evening,
`calibrate` 1.019 / 1.010 (e2e), foreign load ≤ 0.5 %. Raw data: `runs/`.

Notes:
- The step at rp-3 is **generation**, not the operator: `MIVecSGGenInstr` at d20 S10 n300 took
  28.1 s (baseline) / 27.0 s (rp-2) → 3 ms (rp-3: WP-03, downwards closure / Pareto maxima);
  d15 S6 0.15 s → 1 ms. Operator phases of mivec d20 S10 c1us: SG 0.87 → 0.74 → 0.77 → 0.66 s,
  DSG 0.33 s throughout.
- Evaluation-bound operators are unchanged since the baseline (quad / interp at c100us ±1 %):
  the optimization phase so far targeted the operators' own work, which matters at c1us only
  (quad/sg d15 L5 c1us 1.344 → 1.156 s, mostly WP-01 before rp-1; quad/dsg 0.422 → 0.349 s,
  WP-02 at rp-2; interplin/sg d10 c1us 1.21 → 1.07 s at rp-4; interplin/dsg d10 c1us 0.232 →
  0.207 s: WP-49 at head).
- **Shift rp-5 → head on mivec/sg** (the SG quadrature driver changed in WP-56): operator c100us
  2.060 → 2.155 s (+4.6 %), c100us-ramp 2.397 → 2.474 s (+3.2 %), c1us 0.769 → 0.658 s
  (−14 %); spreads ≤ 1.3 %. The load balance of the SG quadrature's tensor-grid loop changed;
  the cheap suite cannot show this (its mivec cases are 3–750 ms of non-evaluation work).
- Interpolation spreads up to 30 % (interp/sg c1us) are single slow processes (bimodal, see
  `../e2e-expensive-r8.md`); the medians are stable.
