#!/usr/bin/env python3
"""WP-61 rework check: per-round e2e results of A and B from an `ab e2e` JSON.

Usage: .venv/bin/python combigrid/perf/results/WP-61/rework/e2e_results.py <e2e.json>
Prints, per case, the distinct result bit patterns of A and B over the rounds and whether they
are identical. SG quadrature / mivec cases must give one identical value in A and B; the other
operators (DSG quadrature, interpolation) use thread reductions and are not run-to-run
deterministic in either version.
"""
import json
import struct
import sys


def bits(x):
    return struct.pack("<d", x).hex()


data = json.load(open(sys.argv[1]))["data"]
cases = sorted({k.split("|", 1)[1] for k in data})
print("| case | A distinct | B distinct | A == B (sets) | max abs(B - A) |")
print("|---|---|---|---|---|")
summary = {"sgquad_same": 0, "sgquad_total": 0, "other_same": 0, "other_total": 0}
for case in cases:
    a = [r["result"] for r in data.get("A|" + case, []) if "result" in r]
    b = [r["result"] for r in data.get("B|" + case, []) if "result" in r]
    if not a or not b:
        continue
    sa, sb = {bits(x) for x in a}, {bits(x) for x in b}
    same = sa == sb
    diff = max(abs(x - y) for x in a for y in b)
    sg_quad = "/sg/" in case and ("quad/" in case or "mivec/" in case)
    key = "sgquad" if sg_quad else "other"
    summary[key + "_total"] += 1
    summary[key + "_same"] += same
    print(f"| {case} | {len(sa)} | {len(sb)} | {'yes' if same else 'no'} | {diff:.2e} |")
print()
print(f"SG quad / mivec cases: {summary['sgquad_same']} / {summary['sgquad_total']} with identical "
      "result sets in A and B")
print(f"other cases: {summary['other_same']} / {summary['other_total']} with identical result sets "
      "(not deterministic in A or B)")
