#!/usr/bin/env python3
"""RP-4 placement check: perf stat (instructions, cycles, op-cache accesses / misses) of rp-4 vs.
rp-4-sor-a64 (sumOverRows [[gnu::aligned(64)]]), 3 alternating processes per snapshot and case.
e2e cases at 16 threads (1 warm-up + 2 samples), micro cases at 1 thread (perf.py's OpenMP env).
Usage (repo root): .venv/bin/python combigrid/perf/results/RP-4/opcache_stat.py
"""
import os
import statistics
import subprocess

SNAP = "combigrid/perf/.work/snapshots"
EV = ["instructions", "cycles", "op_cache_hit_miss.all_op_cache_accesses",
      "op_cache_hit_miss.op_cache_miss"]
CASES = [("e2e", "interp/sg/cc/b0/d10/L2/p8", 16), ("e2e", "quad/sg/equi-exp/b0/d10/L4", 16),
         ("e2e", "quad/sg/equi-exp/b1/d10/L7", 16), ("micro", "interp/sg/cc/b0/d8/L2", 1),
         ("micro", "quad/process_tg/equi/b0/d10/L4", 1)]


def env(threads):
    e = dict(os.environ, OMP_NUM_THREADS=str(threads))
    e.update({"OMP_PROC_BIND": "true", "OMP_PLACES": "{4}"} if threads == 1 else
             {"OMP_PROC_BIND": "close", "OMP_PLACES": "threads"})
    return e


def stat(snap, suite, case, threads):
    e = env(threads)
    e["LD_LIBRARY_PATH"] = f"{SNAP}/{snap}/lib"
    name = case if suite == "e2e" else "micro/" + case
    cmd = ["perf", "stat", "-x", ",", "-e", ",".join(EV), "--", f"{SNAP}/{snap}/bin/perf_{suite}",
           "--filter", f"^{name}$", "--samples", "2", "--warmups", "1"]
    err = subprocess.run(cmd, env=e, capture_output=True, text=True).stderr
    out = {}
    for line in err.splitlines():
        p = line.split(",")
        if len(p) > 2 and p[2] in EV:
            out[p[2]] = float(p[0])
    return out


for suite, case, t in CASES:
    res = {"rp-4": [], "rp-4-sor-a64": []}
    for i in range(3):
        for s in (("rp-4", "rp-4-sor-a64") if i % 2 == 0 else ("rp-4-sor-a64", "rp-4")):
            res[s].append(stat(s, suite, case, t))
    print(f"\n{suite} `{case}` ({t} t), median of 3 processes:")
    print("| event | rp-4 | rp-4-sor-a64 | B/A |\n|---|---|---|---|")
    for ev in EV:
        a = statistics.median(r[ev] for r in res["rp-4"])
        b = statistics.median(r[ev] for r in res["rp-4-sor-a64"])
        print(f"| {ev} | {a:.4g} | {b:.4g} | {b / a:.3f} |")
