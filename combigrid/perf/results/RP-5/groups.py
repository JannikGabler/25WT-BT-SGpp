#!/usr/bin/env python3
"""Per-group progress for RP-5 (adapted from RP-4/groups.py).

Usage: groups.py CUM.json DIRECT.json
  CUM.json    = `ab e2e baseline rp-5` (A = baseline, B = rp-5; full suite)
  DIRECT.json = `ab e2e rp-4 rp-5`     (A = rp-4,     B = rp-5; same session)
Groups: quad/sg, quad/dsg (b0, b1), interp/sg, interp/dsg, interplin/sg, interplin/dsg, mivec.
The DSG interpolation cases have no baseline values (absolute times and shares only).
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


C, D = table(sys.argv[1]), table(sys.argv[2])
groups = ["quad/sg", "quad/dsg", "interp/sg", "interp/dsg", "interplin/sg", "interplin/dsg", "mivec"]
agg = {g: dict(a=0, b=0, r4=0, b4=0, a7=0, b7=0, n=0, lr=[], lr4=[]) for g in groups + ["common", "all"]}
for case, s in C.items():
    g = group(case)
    b = med(s["B"])
    a = med(s["A"]) if "A" in s else None
    d = D.get(case)
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
        if d is not None:
            x["r4"] += med(d["A"])
            x["b4"] += med(d["B"])
            x["lr4"].append(math.log(rss(d["B"]) / rss(d["A"])))
total = agg["all"]["b"]
print("| group | cases | baseline | rp-4 (direct A) | rp-5 (direct B) | rp-5 (cum. B) | baseline → rp-5 "
      "| rp-4 → rp-5 | d ≥ 7: baseline → rp-5 | peak RSS geomean baseline → rp-5 | rp-4 → rp-5 "
      "| share of rp-5 suite |")
print("|---|---|---|---|---|---|---|---|---|---|---|---|")
for g in groups + ["common", "all"]:
    x = agg[g]
    sp = f"{x['a'] / x['b']:.3f}x" if x["a"] else "–"
    s4 = f"{x['r4'] / x['b4']:.3f}x" if x["b4"] else "–"
    d7 = f"{x['a7']:.2f} → {x['b7']:.2f} s ({x['a7'] / x['b7']:.2f}x)" if x["a7"] else "–"
    gm = f"{math.exp(statistics.mean(x['lr'])):.3f}" if x["lr"] else "–"
    gm4 = f"{math.exp(statistics.mean(x['lr4'])):.3f}" if x["lr4"] else "–"
    print(f"| {g} | {x['n']} | {x['a']:.3f} s | {x['r4']:.3f} s | {x['b4']:.3f} s | {x['b']:.3f} s "
          f"| {sp} | {s4} | {d7} | {gm} | {gm4} | {100 * x['b'] / total:.1f} % |")
b0 = sum(med(s["B"]) for c, s in C.items() if c.startswith("quad/dsg") and "/b0/" in c)
b1 = sum(med(s["B"]) for c, s in C.items() if c.startswith("quad/dsg") and "/b1/" in c)
print(f"\nquad/dsg b0 {b0:.3f} s ({100*b0/total:.1f} %), b1 {b1:.3f} s ({100*b1/total:.1f} %)")
print("\n| case | rp-4 | rp-5 | rp-4 → rp-5 (direct, ratio of medians) | gen rp-5 | op rp-5 | peak RSS baseline → rp-4 → rp-5 |")
print("|---|---|---|---|---|---|---|")
for case, s in sorted(C.items(), key=lambda kv: -med(kv[1]["B"])):
    d = D.get(case)
    b = med(s["B"])
    r4 = med(d["A"]) if d else None
    b4 = med(d["B"]) if d else None
    ra = f"{rss(s['A']):.1f} → " if "A" in s else ""
    rr = f"{rss(d['A']):.1f} → " if d else ""
    print(f"| {case} | {f'{r4:.3f}' if r4 else '–'} | {b:.3f} | {f'{r4 / b4:.3f}x' if d else '–'} "
          f"| {med(s['B'], 'generation') * 1e3:.2f} ms | {med(s['B'], 'operator'):.3f} "
          f"| {ra}{rr}{rss(s['B']):.1f} MiB |")
