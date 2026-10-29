#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""WP-23: PLT call sites of imported symbols per calling function.

Usage: plt_sites.py BINARY IMPORTS.txt [TOP]   (IMPORTS.txt: mangled names, one per line, e.g.
`nm -D --undefined-only BINARY` intersected with `nm -D --defined-only libsgppbase.so`, LC_ALL=C)
"""
import re, subprocess, sys, collections
f = sys.argv[1]
syms = set(open(sys.argv[2]).read().split())
out = subprocess.run(["objdump", "-d", "--no-show-raw-insn", f], capture_output=True, text=True).stdout
func = None
sites = collections.Counter()
per_func = collections.defaultdict(collections.Counter)
for line in out.splitlines():
    m = re.match(r"^[0-9a-f]+ <(.+)>:$", line)
    if m:
        func = m.group(1); continue
    m = re.search(r"(call|jmp)\s+[0-9a-f]+ <(.+?)@plt>", line)
    if m and m.group(2) in syms and not func.endswith("@plt"):
        sites[m.group(2)] += 1
        per_func[m.group(2)][func] += 1
def dem(s): return subprocess.run(["c++filt", s], capture_output=True, text=True).stdout.strip()
for s, n in sites.most_common():
    print(f"{n:5d}  {dem(s)}")
    for fn, k in per_func[s].most_common(int(sys.argv[3]) if len(sys.argv) > 3 else 8):
        print(f"         {k:3d} {dem(fn)[:150]}")
