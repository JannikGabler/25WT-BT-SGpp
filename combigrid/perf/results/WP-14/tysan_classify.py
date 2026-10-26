#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""WP-14: classifies Clang TypeSanitizer reports of the Boost test logs (stdlib only).

Every report ("ERROR: TypeSanitizer: type-aliasing-violation ...") is reduced to
(access, accessed type, existing type, first frame) and counted. Its area is that of the first
stack frame: SG++ library code (<module>/src), tests (<module>/tests), or a third-party header
(Eigen, Boost, Armadillo, libstdc++, ...); the first SG++ frame is shown as well, since a report
inside a library header is often caused by the SG++ code calling it.

  tysan_classify.py LOG... [--root WORKTREE] [--frames N]
"""

import argparse
import os
import re
from pathlib import Path

HEAD_RX = re.compile(r"ERROR: TypeSanitizer: (\S+) on address")
ACCESS_RX = re.compile(r"(READ|WRITE) of size (\d+) at \S+ with type (.+?) accesses an existing "
                       r"object of type (.+)$")
FRAME_RX = re.compile(r"^\s+#(\d+) 0x[0-9a-f]+ in (.+?) (/\S+?)(?::(\d+)(?::\d+)?)?$")
MODULES = ("base", "combigrid", "datadriven", "misc", "optimization", "pde", "quadrature",
           "solver")


def area(path, root):
    rel = os.path.relpath(path, root) if root else path
    parts = Path(rel).parts
    if not rel.startswith("..") and parts and parts[0] in MODULES and len(parts) > 1:
        return f"{parts[0]}/{parts[1]}"
    for key, name in (("eigen", "Eigen"), ("boost", "Boost"), ("armadillo", "Armadillo"),
                      ("/c++/", "libstdc++"), ("omp", "OpenMP")):
        if key in path:
            return name
    return "other (" + "/".join(Path(path).parts[:4]) + ")"


def parse(log, root, n_frames):
    reports, current = [], None
    for line in Path(log).read_text(errors="replace").splitlines():
        if HEAD_RX.search(line):
            current = {"kind": HEAD_RX.search(line).group(1), "frames": []}
            reports.append(current)
        elif current is not None and (m := ACCESS_RX.search(line)):
            current["access"] = m.groups()
        elif current is not None and (m := FRAME_RX.match(line)):
            if len(current["frames"]) < 40:
                current["frames"].append((m.group(2), m.group(3), m.group(4) or "?"))
        elif current is not None and current["frames"] and not line.strip():
            current = None
    out = []
    for r in reports:
        frames = r["frames"]
        first = frames[0] if frames else ("?", "?", "?")
        sgpp = next((f for f in frames if area(f[1], root).split("/")[0] in MODULES), None)
        out.append({"kind": r["kind"], "access": r.get("access"), "first": first,
                    "area": area(first[1], root),
                    "sgpp": sgpp, "sgpp_area": area(sgpp[1], root) if sgpp else "-",
                    "frames": frames[:n_frames]})
    return out


def short(frame, root):
    func, path, row = frame
    rel = os.path.relpath(path, root) if root and not os.path.relpath(path, root).startswith("..") \
        else path
    return f"{func[:90]} ({rel}:{row})"


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("log", nargs="+")
    parser.add_argument("--root", help="worktree root (paths are shown relative to it)")
    parser.add_argument("--frames", type=int, default=4)
    args = parser.parse_args()
    for log in args.log:
        reports = parse(log, args.root, args.frames)
        unique = {}
        for r in reports:
            key = (r["access"], r["first"])
            unique.setdefault(key, {**r, "count": 0})["count"] += 1
        by_area = {}
        for u in unique.values():
            k = f"{u['area']} <- {u['sgpp_area']}"
            by_area[k] = by_area.get(k, 0) + 1
        print(f"\n## {log}: {len(reports)} reports, {len(unique)} unique "
              f"(first frame <- first SG++ frame: {by_area})")
        for u in sorted(unique.values(), key=lambda u: (u["area"], -u["count"])):
            acc = u["access"] or ("?", "?", "?", "?")
            print(f"- [{u['area']}] {u['count']}x {acc[0]} size {acc[1]} as `{acc[2]}` of "
                  f"`{acc[3]}`")
            for f in u["frames"]:
                print(f"    {short(f, args.root)}")
            if u["sgpp"] and u["sgpp"] not in u["frames"]:
                print(f"    first SG++ frame: {short(u['sgpp'], args.root)}")


if __name__ == "__main__":
    main()
