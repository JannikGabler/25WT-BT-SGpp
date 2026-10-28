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

HEAD_RX = re.compile(r"ERROR: TypeSanitizer: (\S+) on address (0x[0-9a-f]+)")
ACCESS_RX = re.compile(r"(READ|WRITE) of size (\d+) at \S+ with type (.+?) accesses "
                       r"(?:part of )?an existing object of type (.+)$")
# paths may contain spaces (the repository path does), function names do not contain " /";
# uninstrumented frames look like "in func (/usr/lib/libx.so+0x123)"
FRAME_RX = re.compile(r"^\s+#(\d+) 0x[0-9a-f]+ in (.*?) \(?(/[^:()]+?)(?:\+0x[0-9a-f]+\))?"
                      r"(?::(\d+)(?::\d+)?)?$")
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


def pattern(report, sgpp):
    """Known TySan false-positive classes; only "heap/other" needs a manual review.
    - vptr: base-class constructor/destructor writes its vtable pointer into an object TySan
      typed as the derived class;
    - static-init: member stores while a global is constructed (its shadow holds the whole type);
    - stack: stack slot that held another object before (TySan does not reset the shadow when a
      lifetime ends; uninstrumented writers such as libomp's loop bounds are not seen)."""
    access = report.get("access") or ("?",) * 4
    # substring: reports of concurrent OpenMP threads interleave, which garbles the lines
    if "vtable pointer" in access[2] or "vtable pointer" in access[3]:
        return "vptr"
    if sgpp and sgpp[0].startswith(("__cxx_global_var_init", "_GLOBAL__sub_I")):
        return "static-init"
    if report["address"] >= 0x7ff000000000:
        return "stack"
    return "heap/other"


def summarize(report, root, n_frames):
    frames = report["frames"]
    first = frames[0] if frames else ("?", "?", "?")
    sgpp = next((f for f in frames if area(f[1], root).split("/")[0] in MODULES), None)
    return {"kind": report["kind"], "access": report.get("access"), "first": first,
            "pattern": pattern(report, sgpp), "area": area(first[1], root), "sgpp": sgpp,
            "sgpp_area": area(sgpp[1], root) if sgpp else "-", "frames": frames[:n_frames]}


def parse(log, root, n_frames):
    """Streams the log (TySan does not deduplicate: logs reach several GB) and aggregates the
    reports per access/object type pair (offsets removed) and triggering location (the first
    SG++ frame, else the first frame). Returns (number of reports, {key: entry with count})."""
    unique, total, current = {}, 0, None

    def finish(report):
        r = summarize(report, root, n_frames)
        acc = tuple(re.sub(r" (that starts )?at offset -?\d+", "", a)
                    for a in (r["access"] or ("?",) * 4))
        where = r["sgpp"] or r["first"]
        key = (r["pattern"], acc[0], acc[2], acc[3], where[1], where[2])
        unique.setdefault(key, {**r, "count": 0})["count"] += 1

    with open(log, errors="replace") as f:
        for line in f:
            line = line.rstrip("\n")
            if m := HEAD_RX.search(line):
                if current is not None:
                    finish(current)
                total += 1
                current = {"kind": m.group(1), "address": int(m.group(2), 16), "frames": []}
            elif current is not None and (m := ACCESS_RX.search(line)):
                current["access"] = m.groups()
            elif current is not None and (m := FRAME_RX.match(line)):
                if len(current["frames"]) < 40:
                    current["frames"].append((m.group(2), m.group(3), m.group(4) or "?"))
    if current is not None:
        finish(current)
    return total, unique


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
        total, unique = parse(log, args.root, args.frames)
        by_area = {}
        for u in unique.values():
            k = f"{u['pattern']}: {u['area']} <- {u['sgpp_area']}"
            by_area[k] = by_area.get(k, 0) + 1
        print(f"\n## {log}: {total} reports, {len(unique)} unique "
              f"(first frame <- first SG++ frame: {by_area})")
        for u in sorted(unique.values(), key=lambda u: (u["pattern"], u["area"], -u["count"])):
            acc = u["access"] or ("?", "?", "?", "?")
            print(f"- [{u['pattern']}; {u['area']}] {u['count']}x {acc[0]} size {acc[1]} as "
                  f"`{acc[2]}` of `{acc[3]}`")
            for f in u["frames"]:
                print(f"    {short(f, args.root)}")
            if u["sgpp"] and u["sgpp"] not in u["frames"]:
                print(f"    first SG++ frame: {short(u['sgpp'], args.root)}")


if __name__ == "__main__":
    main()
