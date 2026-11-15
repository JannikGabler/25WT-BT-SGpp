#!/usr/bin/env python3
"""WP-56: re-pins the bitwise hashes of grid_storage_pinning_test.cpp after the round-off change of
the Clenshaw-Curtis weights (owner decision 2026-11-15).

Usage (repo root): SGPP_PRINT_GOLDEN=1 ./combigrid/tests/test_combigrid_boost \
    --run_test=grid_storage_pinning > golden.txt
  python3 repin_grid_storage.py golden.txt debug|opt
Replaces only the bitwise hash of the given build (column 2 = debug, 3 = optimized) and refuses if
a point hash differs (the grid structure must not change). Prints the configurations it changed.
"""
import re
import sys

path = "combigrid/tests/grids/grid_storage_pinning_test.cpp"
golden_txt, build = sys.argv[1], sys.argv[2]
col = {"debug": 1, "opt": 2}[build]
printed = []
for line in open(golden_txt):
    m = re.match(r"^(.+?): (0x[0-9a-f]{16})ULL (0x[0-9a-f]{16})ULL", line)
    if m:
        printed.append((m.group(1), m.group(2), m.group(3)))

src = open(path).read()
entry = re.compile(r"(    // (?P<name>[^\n]+)\n    \{)(?P<h0>0x[0-9a-f]{16})ULL, (?P<h1>0x[0-9a-f]{16})ULL, "
                   r"(?P<h2>0x[0-9a-f]{16})ULL")
matches = list(entry.finditer(src))
assert len(matches) == len(printed), (len(matches), len(printed))
out, pos, changed = [], 0, []
for m, (name, point, bitwise) in zip(matches, printed):
    assert m.group("name") == name, (m.group("name"), name)
    assert m.group("h0") == point, f"point hash changed: {name}"
    hashes = [m.group("h0"), m.group("h1"), m.group("h2")]
    if hashes[col] != bitwise:
        changed.append(name)
        hashes[col] = bitwise
    out.append(src[pos:m.start()])
    out.append(f"{m.group(1)}{hashes[0]}ULL, {hashes[1]}ULL, {hashes[2]}ULL")
    pos = m.end()
out.append(src[pos:])
open(path, "w").write("".join(out))
print(f"{build}: {len(changed)} of {len(printed)} bitwise hashes re-pinned")
for name in changed:
    print("  " + name)
