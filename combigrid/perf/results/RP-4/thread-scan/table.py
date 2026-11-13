#!/usr/bin/env python3
"""Thread scan of the low-d e2e cases (rp-4, `perf.py run e2e --threads 1|4|16`, 1 warm-up + 1 sample)."""
import json
import pathlib

d = pathlib.Path(__file__).parent
runs = {t: {r["case"]: r for r in json.load(open(d / f"t{t}.json"))["records"]} for t in (1, 4, 16)}
print("| case | 1 t | 4 t | 16 t | 1 → 16 t | best | 16 t − best |\n|---|---|---|---|---|---|---|")
for c in runs[16]:
    v = {t: runs[t][c]["metrics"]["total"][0] for t in (1, 4, 16)}
    bt = min(v, key=v.get)
    print(f"| {c} | {v[1]*1e3:.1f} ms | {v[4]*1e3:.1f} ms | {v[16]*1e3:.1f} ms | {v[1]/v[16]:.2f}x "
          f"| {bt} t | {(v[16]-v[bt])*1e3:.1f} ms |")
