"""WP-61: re-pins the SG quadrature hashes of tests/grids/grid_storage_pinning_test.cpp.

Usage: repin.py <SGPP_PRINT_GOLDEN output> <field: debug|opt> <log>
Replaces the bitwise hash (debug: 2nd, opt: 3rd value of a GOLDEN entry) of every " sg" entry
whose value differs; fails if a " dsg" entry or a point hash differs. Appends old -> new to <log>.
"""
import re
import sys

TEST = "combigrid/tests/grids/grid_storage_pinning_test.cpp"
printed, field, log = sys.argv[1], sys.argv[2], sys.argv[3]
current = {}
for line in open(printed):
    name, rest = line.split(": ", 1)
    point, bitwise = rest.split()[:2]
    current[name.strip()] = (point, bitwise)

src = open(TEST).read()
pattern = re.compile(r"(    // (?P<name>[^\n]+)\n    \{)(?P<point>0x[0-9a-f]+ULL), (?P<dbg>0x[0-9a-f]+ULL), (?P<opt>0x[0-9a-f]+ULL)")
changes = []


def repl(m):
    name = m.group("name")
    point, bitwise = current[name]
    assert point == m.group("point"), f"point hash changed: {name}"
    old = m.group("dbg" if field == "debug" else "opt")
    if old == bitwise:
        return m.group(0)
    assert name.endswith(" sg"), f"non-SG entry changed: {name}"
    changes.append(f"{field} | {name} | {old} -> {bitwise}")
    dbg = bitwise if field == "debug" else m.group("dbg")
    opt = bitwise if field == "opt" else m.group("opt")
    return f"{m.group(1)}{point}, {dbg}, {opt}"


new, n = pattern.subn(repl, src)
assert n == len(current), (n, len(current))
open(TEST, "w").write(new)
with open(log, "a") as f:
    for c in changes:
        f.write(c + "\n")
print(f"{field}: {len(changes)} of {n} entries re-pinned")
