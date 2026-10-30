#!/usr/bin/env python3
"""Inline-chain split of a perf.data file (RP-2 method).

perf script -F ip,sym,symoff,dso -G --no-demangle -> per sample: symbol + offset + dso.
For samples in the given libraries the address (nm address of the symbol + offset) is resolved
with `addr2line -a -i -f -C`; the innermost frame that is not STL / compiler support decides the
"helper". Output: shares per dso, per symbol, per (symbol, helper).
"""
import collections
import re
import subprocess
import sys

data, libdir = sys.argv[1], sys.argv[2]
top = int(sys.argv[3]) if len(sys.argv) > 3 else 40
libs = {"libsgppcombigrid.so": f"{libdir}/libsgppcombigrid.so",
        "libsgppbase.so": f"{libdir}/libsgppbase.so"}

out = subprocess.run(["perf", "script", "-i", data, "-F", "ip,sym,symoff,dso", "-G",
                      "--no-demangle"], capture_output=True, text=True).stdout
line_re = re.compile(r"^\s*([0-9a-f]+)\s+(.*?)\+0x([0-9a-f]+)\s+\((.*)\)\s*$")
line_re2 = re.compile(r"^\s*([0-9a-f]+)\s+(.*?)\s+\((.*)\)\s*$")
samples = []
for l in out.splitlines():
    m = line_re.match(l)
    if m:
        samples.append((m.group(2), int(m.group(3), 16), m.group(4).rsplit("/", 1)[-1]))
        continue
    m = line_re2.match(l)
    if m:
        samples.append((m.group(2), 0, m.group(3).rsplit("/", 1)[-1]))
n = len(samples)

symaddr = {}
for base, path in libs.items():
    nm = subprocess.run(["nm", path], capture_output=True, text=True).stdout
    d = {}
    for l in nm.splitlines():
        p = l.split()
        if len(p) == 3:
            d.setdefault(p[2], int(p[0], 16))
    symaddr[base] = d

# resolve addresses per library
addrs = collections.defaultdict(set)
for sym, off, dso in samples:
    if dso in libs and sym in symaddr[dso]:
        addrs[dso].add(symaddr[dso][sym] + off)
chain = {}
for dso, aset in addrs.items():
    alist = sorted(aset)
    res = subprocess.run(["addr2line", "-a", "-i", "-f", "-C", "-e", libs[dso]],
                         input="\n".join(hex(a) for a in alist), capture_output=True,
                         text=True).stdout.splitlines()
    cur = None
    i = 0
    while i < len(res):
        l = res[i]
        if l.startswith("0x"):
            cur = int(l, 16)
            chain[(dso, cur)] = []
            i += 1
            continue
        fn = l
        i += 2  # skip file:line
        chain[(dso, cur)].append(fn)

skip = re.compile(r"^(std::|__gnu_cxx::|operator|__|\?\?|void std::|bool std::|double std::|"
                  r"unsigned long std::|.*\bstd::(vector|span|function|_Function|__)|"
                  r"sgpp::base::DataVector::operator|sgpp::base::DataVector::size|"
                  r"sgpp::base::DataVector::data|.*\bstd::fill_n|.*\bstd::copy|sgpp::combigrid::MI<.*>::operator\[\]|sgpp::combigrid::MI<.*>::size)")


def short(fn):
    fn = re.sub(r"\(.*", "", fn)
    fn = fn.replace("(anonymous namespace)", "{anon}")
    return fn[-90:]


def demangle(names):
    r = subprocess.run(["c++filt"], input="\n".join(names), capture_output=True, text=True)
    return dict(zip(names, r.stdout.splitlines()))


dem = demangle(sorted({s for s, _, _ in samples}))
by_dso = collections.Counter()
by_sym = collections.Counter()
by_helper = collections.Counter()
for sym, off, dso in samples:
    by_dso[dso] += 1
    s = short(dem.get(sym, sym))
    by_sym[(dso, s)] += 1
    helper = None
    if dso in libs and sym in symaddr[dso]:
        ch = chain.get((dso, symaddr[dso][sym] + off), [])
        for fn in ch:  # innermost first
            if not skip.match(fn):
                helper = short(fn)
                break
    by_helper[(s, helper or "-")] += 1

print(f"samples: {n}")
print("\n## per dso")
for k, v in by_dso.most_common(12):
    print(f"{100*v/n:6.2f} %  {k}")
print("\n## per symbol")
for (d, s), v in by_sym.most_common(top):
    print(f"{100*v/n:6.2f} %  {s}  [{d}]")
print("\n## per symbol / innermost helper (combigrid + base only)")
for (s, h), v in by_helper.most_common(top * 2):
    if h == "-":
        continue
    if 100 * v / n < 0.2:
        break
    print(f"{100*v/n:6.2f} %  {s}  <=  {h}")
