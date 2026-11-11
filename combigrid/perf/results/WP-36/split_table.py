#!/usr/bin/env python3
"""Groups the per-symbol shares of the WP-36 inline-chain splits (profiles/*-split.txt) into the
categories of the exploration report; prints one Markdown row per profile."""
import glob
import re
import sys

CATS = [
    ("integrand", r"__cos_fma|perf::cosProd|cos@plt|_Function_handler"),
    ("operator kernel", r"processTensorGridInternal|computeNeighborGPWithWeight|interpolateLinear|"
                        r"getNeighborsForLinInterpolation|lin_interpolation::processTensorGrid"),
    ("opt. barycentric", r"OptBarycentricFormula"),
    ("generic weights", r"barycentricFormulaWeights|log|exp|BarycentricFormula::interpolate|"
                        r"hasTrivialResult"),
    ("allocation", r"memalign|cfree|operator new|operator delete|malloc|_int_free|free@"),
    ("libgomp", r"libgomp"),
    ("OpenBLAS", r"openblas"),
]


def parse(path):
    shares, helpers, samples = {}, {}, 0
    section = None
    for line in open(path):
        if line.startswith("samples:"):
            samples = int(line.split()[1])
        elif line.startswith("## "):
            section = line.strip()
        elif section == "## per symbol" and re.match(r"\s*[0-9.]+ %", line):
            pct = float(line.split()[0])
            name = line.split("%", 1)[1].strip()
            shares[name] = shares.get(name, 0.0) + pct
        elif section and section.startswith("## per symbol /") and "<=" in line:
            pct = float(line.split()[0])
            helper = line.split("<=")[1].strip()
            helpers[helper] = helpers.get(helper, 0.0) + pct
    return shares, helpers, samples


def categorize(shares):
    out = {c: 0.0 for c, _ in CATS}
    out["other"] = 0.0
    for name, pct in shares.items():
        for cat, rx in CATS:
            if re.search(rx, name, re.IGNORECASE):
                out[cat] += pct
                break
        else:
            out["other"] += pct
    return out


print("| profile | samples | " + " | ".join(c for c, _ in CATS) + " | other | kernel helpers "
      "(getGridPoint / call / mapToDomain / nearlyEqual) |")
print("|---|---|" + "---|" * (len(CATS) + 2))
for path in sorted(glob.glob(sys.argv[1] + "/*-split.txt")):
    if "/micro_" in path:
        continue
    shares, helpers, samples = parse(path)
    cats = categorize(shares)
    h = lambda k: sum(v for n, v in helpers.items() if k in n)
    name = path.rsplit("/", 1)[1].replace("-split.txt", "")
    print(f"| {name} | {samples} | " + " | ".join(f"{cats[c]:.1f}" for c, _ in CATS) +
          f" | {cats['other']:.1f} | {h('getGridPoint'):.1f} / {h('SourceFunc::evaluate'):.1f} / "
          f"{h('mapToDomain'):.1f} / {h('nearlyEqual'):.1f} |")
