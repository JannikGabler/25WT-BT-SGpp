"""WP-77: bitwise equality of the perf_reference results of two snapshots at several thread counts.

Usage: python3 reference_bits_ab.py SNAPSHOT_A SNAPSHOT_B [THREADS ...]   (default threads 1 2 3 16)
Runs perf_reference of both snapshots (as WP-48's reference_bits.py: results only, "info" =
evaluation counts excluded) and reports the cases whose results differ bitwise between A and B
at the same thread count, and between thread counts within B.
"""
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
SNAPSHOTS = ROOT / "combigrid/perf/.work/snapshots"


def run(snapshot, threads):
    env = dict(os.environ, OMP_NUM_THREADS=str(threads), OMP_PROC_BIND="close",
               OMP_PLACES="threads", LD_LIBRARY_PATH=str(SNAPSHOTS / snapshot / "lib"))
    out = subprocess.run([str(SNAPSHOTS / snapshot / "bin/perf_reference")], env=env,
                         capture_output=True, text=True, check=True).stdout
    records = [json.loads(line) for line in out.splitlines() if line]
    return {r["case"]: json.dumps({k: v for k, v in r.items() if k not in ("info", "case")},
                                  sort_keys=True) for r in records}


def differing(x, y):
    return sorted(case for case in x if x[case] != y.get(case))


a, b = sys.argv[1], sys.argv[2]
threads = [int(t) for t in sys.argv[3:]] or [1, 2, 3, 16]
resB1 = None
for t in threads:
    ra, rb = run(a, t), run(b, t)
    print(f"{t:2d} t: {a} vs {b}: {len(differing(ra, rb))} of {len(ra)} cases differ {differing(ra, rb)}")
    if resB1 is None:
        resB1 = rb
    else:
        d = differing(resB1, rb)
        print(f"      {b} {threads[0]} t vs {t} t: {len(d)} cases differ {d}")
