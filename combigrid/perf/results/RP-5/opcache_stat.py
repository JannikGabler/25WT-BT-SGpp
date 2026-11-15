#!/usr/bin/env python3
"""RP-5 question 3: is WP-05's `quad/dsg/cc-uq/b0/d10/L7` op-cache effect still present at HEAD?

perf stat (instructions, cycles, op-cache accesses / misses) of several snapshots on the DSG b0
d10 cases, N alternating processes per snapshot and case (16 threads, 1 warm-up + 1 sample, the
OpenMP environment of perf.py). Then (`--record`) samples the op-cache-miss event per symbol for
the first and the last snapshot (which code misses).
Usage (repo root): .venv/bin/python opcache_stat.py [--record] SNAP [SNAP ...]
"""
import os
import re
import statistics
import subprocess
import sys

SNAP = "combigrid/perf/.work/snapshots"
OUT = "combigrid/perf/.work/profiles/RP-5"
EV = ["instructions", "cycles", "op_cache_hit_miss.all_op_cache_accesses",
      "op_cache_hit_miss.op_cache_miss"]
CASES = ["quad/dsg/cc-uq/b0/d10/L7", "quad/dsg/equi-exp/b0/d10/L7"]
N = 3

args = sys.argv[1:]
record = "--record" in args
snaps = [a for a in args if a != "--record"]


def env(snap):
    e = dict(os.environ, OMP_NUM_THREADS="16", OMP_PROC_BIND="close", OMP_PLACES="threads")
    e["LD_LIBRARY_PATH"] = f"{SNAP}/{snap}/lib"
    return e


def cmd(snap, case):
    return [f"{SNAP}/{snap}/bin/perf_e2e", "--filter", f"^{re.escape(case)}$", "--samples", "1",
            "--warmups", "1"]


def stat(snap, case):
    err = subprocess.run(["perf", "stat", "-x", ",", "-e", ",".join(EV), "--"] + cmd(snap, case),
                         env=env(snap), capture_output=True, text=True).stderr
    out = {}
    for line in err.splitlines():
        p = line.split(",")
        if len(p) > 2 and p[2] in EV:
            out[p[2]] = float(p[0])
    return out


for case in CASES:
    res = {s: [] for s in snaps}
    for i in range(N):
        for s in (snaps if i % 2 == 0 else list(reversed(snaps))):
            res[s].append(stat(s, case))
    print(f"\n`{case}` (16 t, 1 warm-up + 1 sample), median of {N} processes:")
    print("| event | " + " | ".join(snaps) + " |\n|---|" + "---|" * len(snaps))
    for ev in EV:
        print(f"| {ev} | " + " | ".join(f"{statistics.median(r[ev] for r in res[s]):.4g}"
                                         for s in snaps) + " |")
    print("| miss / access | " + " | ".join(
        f"{statistics.median(r[EV[3]] / r[EV[2]] for r in res[s]):.4f}" for s in snaps) + " |")

if record:
    os.makedirs(OUT, exist_ok=True)
    for s in (snaps[0], snaps[-1]):
        data = f"{OUT}/dsg_cc_d10-{s}.data"
        subprocess.run(["perf", "record", "-q", "-e", "op_cache_hit_miss.op_cache_miss", "-c",
                        "20000", "-o", data, "--"] + cmd(s, CASES[0]), env=env(s), check=True,
                       stdout=subprocess.DEVNULL)
        rep = subprocess.run(["perf", "report", "-i", data, "--stdio", "--sort", "dso,sym",
                              "--percent-limit", "1"], capture_output=True, text=True).stdout
        print(f"\nop-cache misses by symbol, {s} (cc d10, sampled every 20000 misses):")
        print("\n".join(l for l in rep.splitlines() if l and not l.startswith("#"))[:4000])
