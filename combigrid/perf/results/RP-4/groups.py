#!/usr/bin/env python3
"""Per-group progress from `ab e2e baseline rp-4` (RP-4 e2e.json) and the RP-3 report (B = rp-3).

Usage: groups.py RP-4/e2e.json RP-3/e2e.json
Groups: quad/sg, quad/dsg (b0, b1), interp/sg, interp/dsg, interplin/sg, interplin/dsg, mivec.
The DSG interpolation cases exist only in rp-4 (absolute times and shares only).
"""
import json
import math
import re
import statistics
import sys


def table(path):
    out = {}
    for key, recs in json.load(open(path))["data"].items():
        side, case = key.split("|", 1)
        out.setdefault(case, {})[side] = recs
    return out


def med(recs, metric="total"):
    return statistics.median(r["metrics"][metric][0] for r in recs)


def rss(recs):
    return statistics.median(r["peak_rss_kib"] for r in recs) / 1024


def group(case):
    p = case.split("/")
    return "mivec" if p[0] == "mivec" else f"{p[0]}/{p[1]}"


def dim(case):
    return int(re.search(r"/d(\d+)/", case).group(1))


C, P = table(sys.argv[1]), table(sys.argv[2])
groups = ["quad/sg", "quad/dsg", "interp/sg", "interp/dsg", "interplin/sg", "interplin/dsg", "mivec"]
agg = {g: dict(a=0, b=0, r3=0, a7=0, b7=0, n=0, lr=[]) for g in groups + ["common", "all"]}
for case, s in C.items():
    g = group(case)
    b = med(s["B"])
    a = med(s["A"]) if "A" in s else None
    r3 = med(P[case]["B"]) if case in P else None
    for key in (g, "all") + (("common",) if a is not None else ()):
        x = agg[key]
        x["n"] += 1
        x["b"] += b
        if a is not None:
            x["a"] += a
            x["lr"].append(math.log(rss(s["B"]) / rss(s["A"])))
            if dim(case) >= 7:
                x["a7"] += a
                x["b7"] += b
        if r3 is not None:
            x["r3"] += r3
total = agg["all"]["b"]
print("| group | cases | baseline | rp-3 | rp-4 | baseline → rp-4 | rp-3 → rp-4 | d ≥ 7: baseline → rp-4 "
      "| peak RSS geomean B/A | share of rp-4 suite |")
print("|---|---|---|---|---|---|---|---|---|---|")
for g in groups + ["common", "all"]:
    x = agg[g]
    sp = f"{x['a'] / x['b']:.3f}x" if x["a"] else "–"
    s3 = f"{x['r3'] / x['b']:.3f}x" if x["r3"] and x["a"] else "–"
    d7 = f"{x['a7']:.2f} → {x['b7']:.2f} s ({x['a7'] / x['b7']:.2f}x)" if x["a7"] else "–"
    gm = f"{math.exp(statistics.mean(x['lr'])):.3f}" if x["lr"] else "–"
    print(f"| {g} | {x['n']} | {x['a']:.3f} s | {x['r3']:.3f} s | {x['b']:.3f} s | {sp} | {s3} | {d7} "
          f"| {gm} | {100 * x['b'] / total:.1f} % |")
b0 = sum(med(s["B"]) for c, s in C.items() if c.startswith("quad/dsg") and "/b0/" in c)
b1 = sum(med(s["B"]) for c, s in C.items() if c.startswith("quad/dsg") and "/b1/" in c)
print(f"\nquad/dsg b0 {b0:.3f} s ({100*b0/total:.1f} %), b1 {b1:.3f} s ({100*b1/total:.1f} %)")
print("\n| case | rp-3 | rp-4 | rp-3 → rp-4 | gen rp-4 | op rp-4 | peak RSS baseline → rp-4 |")
print("|---|---|---|---|---|---|---|")
for case, s in sorted(C.items(), key=lambda kv: -med(kv[1]["B"])):
    b = med(s["B"])
    r3 = med(P[case]["B"]) if case in P else None
    ra = f"{rss(s['A']):.1f} → " if "A" in s else ""
    print(f"| {case} | {f'{r3:.3f}' if r3 else '–'} | {b:.3f} | {f'{r3 / b:.3f}x' if r3 else 'new'} "
          f"| {med(s['B'], 'generation') * 1e3:.2f} ms | {med(s['B'], 'operator'):.3f} | {ra}{rss(s['B']):.1f} MiB |")
