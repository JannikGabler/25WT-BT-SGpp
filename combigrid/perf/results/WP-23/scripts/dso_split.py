#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""WP-23: DSO split of a perf.data (self samples): libsgppbase.so, base PLT stubs in the
callers, libc allocator / mem* functions. Usage: dso_split.py PERF.data
"""
import re, subprocess, sys, collections
data = sys.argv[1]
out = subprocess.run(["perf", "report", "-i", data, "--sort", "dso,sym", "--stdio", "--no-children", "-n",
                      "-g", "none", "--percent-limit", "0"], capture_output=True, text=True).stdout
BASE_PLT = re.compile(r"(operator new|operator delete|sgpp::base::|_Znwm|_Znam|_ZdlPv|_ZdaPv|_ZN4sgpp4base).*@plt")
ALLOC = re.compile(r"^(_int_malloc|_int_free\w*|_int_memalign|_mid_memalign|__posix_memalign|posix_memalign|malloc|cfree\S*|free|__libc_malloc|__libc_free|malloc_consolidate|unlink_chunk\S*|tcache\w*|_int_realloc|realloc|__libc_calloc|calloc|__memset\w*|__memcpy\w*|__memmove\w*)$")
dso_tot = collections.Counter(); base_syms = collections.Counter(); plt = collections.Counter(); alloc = collections.Counter()
total = 0
for line in out.splitlines():
    m = re.match(r"^\s+[\d.]+%\s+(\d+)\s+(\S+)\s+\[(.)\]\s+(.*)$", line)
    if not m: continue
    pct, dso, _, sym = int(m.group(1)), m.group(2), m.group(3), m.group(4).strip()
    dso_tot[dso] += pct; total += pct
    if dso == "libsgppbase.so": base_syms[sym] += pct
    if BASE_PLT.search(sym): plt[f"{dso}: {sym}"] += pct
    if dso.startswith("libc.so") and ALLOC.match(sym): alloc[sym] += pct
f = 100.0 / max(total, 1)
for c in (dso_tot, base_syms, plt, alloc):
    for k in c: c[k] *= f
print(f"samples: {total}")
print("DSO shares:", ", ".join(f"{d} {p:.3f}%" for d, p in dso_tot.most_common(8)))
print(f"libsgppbase.so total: {sum(base_syms.values()):.3f}%  ", "; ".join(f"{s[:60]} {p:.3f}%" for s, p in base_syms.most_common(6)))
print(f"base PLT stubs: {sum(plt.values()):.3f}%  ", "; ".join(f"{s[:70]} {p:.3f}%" for s, p in plt.most_common(6)))
print(f"libc alloc/mem*: {sum(alloc.values()):.3f}%  ", "; ".join(f"{s} {p:.3f}%" for s, p in alloc.most_common(8)))
