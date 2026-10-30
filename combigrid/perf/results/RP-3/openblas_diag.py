#!/usr/bin/env python3
"""WP-10 diagnostic (no code change): fresh e2e processes with and without the OpenBLAS pool.

Runs the snapshot's perf_e2e directly (same OpenMP env as perf.py, 16 threads), alternating
default vs. OPENBLAS_NUM_THREADS=1 per round. Reports the process wall time (fork to exit) and
the in-process `total` (1 warm-up + 1 sample, as `ab e2e`).
"""
import json
import os
import statistics
import subprocess
import sys
import time

snap = sys.argv[1]
rounds = int(sys.argv[2])
cases = sys.argv[3:]
exe = f"{snap}/bin/perf_e2e"
base = dict(os.environ, LD_LIBRARY_PATH=f"{snap}/lib", OMP_NUM_THREADS="16",
            OMP_PROC_BIND="close", OMP_PLACES="threads")
variants = {"default": base, "openblas1": dict(base, OPENBLAS_NUM_THREADS="1")}

res = {(c, v): {"wall": [], "total": [], "first": []} for c in cases for v in variants}
for r in range(rounds):
    for i, c in enumerate(cases):
        order = list(variants) if (r + i) % 2 == 0 else list(variants)[::-1]
        for v in order:
            t0 = time.perf_counter()
            out = subprocess.run([exe, "--json", "--filter", f"^{c}$", "--samples", "1",
                                  "--warmups", "1"], env=variants[v], capture_output=True,
                                 text=True).stdout
            wall = time.perf_counter() - t0
            rec = [json.loads(l) for l in out.splitlines() if l.startswith("{")][-1]
            res[(c, v)]["wall"].append(wall)
            res[(c, v)]["total"].append(rec["metrics"]["total"][0])

print("| case | process wall default | OPENBLAS_NUM_THREADS=1 | ratio | in-process total default |"
      " OPENBLAS_NUM_THREADS=1 | ratio |")
print("|---|---|---|---|---|---|---|")
for c in cases:
    d, o = res[(c, "default")], res[(c, "openblas1")]
    wd, wo = statistics.median(d["wall"]), statistics.median(o["wall"])
    td, to = statistics.median(d["total"]), statistics.median(o["total"])
    print(f"| {c} | {1000*wd:.1f} ms | {1000*wo:.1f} ms | {wd/wo:.3f} | {1000*td:.2f} ms |"
          f" {1000*to:.2f} ms | {td/to:.3f} |")
print("\nraw:", json.dumps({f"{c}|{v}": x for (c, v), x in res.items()}))
