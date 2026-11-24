"""WP-48: thread-count dependence of the perf_reference results of snapshots.

Usage: python3 reference_bits.py SNAPSHOT [SNAPSHOT ...]
Runs perf_reference of every snapshot at 1, 2, 3 and 16 threads (16 twice) and reports the cases
whose float results differ bitwise from the 1-thread run (and between the two 16-thread runs).
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
    # Results only (ints / floats / values); "info" (evaluation counts) is not a result
    return {r["case"]: json.dumps({k: v for k, v in r.items() if k not in ("info", "case")},
                                  sort_keys=True) for r in records}


def differing(a, b):
    return sorted(case for case in a if a[case] != b.get(case))


def group(case):
    parts = case.split("/")
    prefix = "expensive/" if parts[0] == "expensive" else ""
    parts = parts[1:] if prefix else parts
    return prefix + parts[0] + "/" + parts[1]


for snapshot in sys.argv[1:]:
    one = run(snapshot, 1)
    print(f"{snapshot}: {len(one)} cases")
    dependent = set()
    for threads in (2, 3, 16):
        diff = differing(one, run(snapshot, threads))
        dependent.update(diff)
        groups = sorted({group(c) for c in diff})
        print(f"  1 t vs {threads} t: {len(diff)} cases differ {groups}")
    diff = differing(run(snapshot, 16), run(snapshot, 16))
    print(f"  16 t run 1 vs run 2: {len(diff)} cases differ {sorted({group(c) for c in diff})}")
    print(f"  thread-count dependent (1/2/3/16 t): {len(dependent)} of {len(one)}")
