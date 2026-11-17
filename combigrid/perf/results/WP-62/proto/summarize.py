#!/usr/bin/env python3
"""Prints, for JSON traces on stdin, the error at fixed evaluation budgets (last trace point
with evals <= budget) and the final point."""
import json
import sys

budgets = [int(float(b)) for b in sys.argv[1].split(",")] if len(sys.argv) > 1 else [100, 1000, 10000, 100000]
rows = [json.loads(line) for line in sys.stdin if line.startswith("{")]
if rows:
    cells = []
    for b in budgets:
        best = [r for r in rows if r["evals"] <= b]
        cells.append("%.1e" % best[-1]["err"] if best else "   -   ")
    last = rows[-1]
    print(" ".join(cells), "| final: evals=%d nI=%d nOld=%d err=%.2e eta=%.2e wall=%.3fs eval=%.3fs" % (
        last["evals"], last["nI"], last["nOld"], last["err"], last["eta"], last["wall"], last["evalTime"]))
