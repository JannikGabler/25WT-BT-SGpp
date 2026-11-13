#!/usr/bin/env python3
"""Condenses the RP-4 profile splits (profiles/*-split.txt) into one line per case.

Integrand = all samples in perf_e2e (integrand, its PLT stubs, `_M_invoke`) + libm. Combigrid and
base symbols >= 0.8 % are listed by short name; the row loops get their inline-helper split.
Usage: summarize_profiles.py PROFILE_DIR
"""
import pathlib
import re
import sys

d = pathlib.Path(sys.argv[1])
line_sym = re.compile(r"^\s*([0-9.]+) %\s+(.*?)\s+\[(.*)\]\s*$")
line_help = re.compile(r"^\s*([0-9.]+) %\s+(.*?)\s+<=\s+(.*)$")


def short(s):
    s = re.sub(r"<.*", "", s)
    return s.split("::")[-1] or s


for f in sorted(d.glob("*-split.txt")):
    sec, cats, syms, helpers = None, {}, {}, {}
    for l in f.read_text().splitlines():
        if l.startswith("## per symbol /"):
            sec = "help"
            continue
        if l.startswith("## per symbol"):
            sec = "sym"
            continue
        if sec == "sym" and (m := line_sym.match(l)):
            p, s, dso = float(m.group(1)), m.group(2), m.group(3)
            if dso in ("perf_e2e", "libm.so.6"):
                k = "integrand"
            elif "gomp" in dso:
                k = "libgomp"
            elif "openblas" in dso:
                k = "OpenBLAS"
            elif dso == "libc.so.6":
                k = "libc"
            elif dso.startswith("libsgpp"):
                syms[short(s)] = syms.get(short(s), 0) + p
                continue
            else:
                k = "other"
            cats[k] = cats.get(k, 0) + p
        elif sec == "help" and (m := line_help.match(l)):
            p, s, h = float(m.group(1)), short(m.group(2)), short(m.group(3))
            if s in ("forEachGPWithGlobalWeight", "sumOverRows", "processTensorGrid") and h != s:
                helpers.setdefault(s, []).append(f"{h} {p:.1f}")
    case = f.name.replace("-t16-split.txt", "")
    c = " · ".join(f"{k} {v:.1f}" for k, v in sorted(cats.items(), key=lambda kv: -kv[1]) if v >= 0.5)
    s = " · ".join(f"`{k}` {v:.1f}" + (f" ({', '.join(helpers[k])})" if k in helpers else "")
                   for k, v in sorted(syms.items(), key=lambda kv: -kv[1]) if v >= 0.8)
    print(f"| {case} | {c} | {s} | {sum(syms.values()):.1f} |")
