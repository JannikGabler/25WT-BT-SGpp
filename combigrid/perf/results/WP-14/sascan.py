#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""WP-14 strict-aliasing scan (Python standard library only).

Re-runs every entry of a compile_commands.json (or the given source files of it) with
`-fsyntax-only -fstrict-aliasing -Wstrict-aliasing=LEVEL` appended (the last -f wins over the
-fno-strict-aliasing of pre-WP-14 builds; GCC emits -Wstrict-aliasing in the front end only, so
-fsyntax-only sees all of them) and lists the unique warnings per area and file.
`--no-w` removes `-w` (the SWIG wrappers are compiled with it).

  sascan.py COMPILE_DB --level 2 [--level 3] [--match RE] [--no-w] [--save STEM]
"""

import argparse
import json
import os
import re
import shlex
import subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

WARN_RX = re.compile(r"^(.*?):(\d+):(\d+): warning: (.*?) \[-Wstrict-aliasing(?:=\d)?\]$")


def arguments(entry):
    args = entry.get("arguments") or shlex.split(entry["command"])
    out, skip = [], False
    for a in args:
        if skip:
            skip = False
            continue
        if a == "-o":
            skip = True
            continue
        if a == "-c" or a.startswith("-fdiagnostics-color"):
            continue
        out.append(a)
    return out


def scan(entry, level, no_w):
    args = [a for a in arguments(entry) if not (no_w and a == "-w")]
    args += ["-fsyntax-only", "-fdiagnostics-color=never", "-fstrict-aliasing",
             f"-Wstrict-aliasing={level}"]
    result = subprocess.run(args, cwd=entry["directory"], capture_output=True, text=True)
    warnings = []
    for line in (result.stdout + result.stderr).splitlines():
        m = WARN_RX.match(line)
        if m:
            path = os.path.normpath(os.path.join(entry["directory"], m.group(1)))
            warnings.append((path, int(m.group(2)), int(m.group(3)), m.group(4)))
    return entry["file"], result.returncode, warnings


def area(path, root):
    rel = os.path.relpath(path, root)
    if rel.startswith(".."):
        return "third-party (" + "/".join(Path(path).parts[1:4]) + ")"
    parts = Path(rel).parts
    kind = parts[1] if len(parts) > 2 else ""
    if parts[0] in ("pysgpp", "jsgpp"):
        return parts[0] + " (SWIG wrapper)"
    return f"{parts[0]}/{kind}"


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("db", type=Path)
    parser.add_argument("--level", type=int, action="append", required=True)
    parser.add_argument("--match", default=".", help="regex on the source file path")
    parser.add_argument("--no-w", action="store_true", help="drop -w (SWIG wrappers)")
    parser.add_argument("--jobs", type=int, default=16)
    parser.add_argument("--save", help="write STEM-L<level>.json with all warnings")
    args = parser.parse_args()
    entries = [e for e in json.loads(args.db.read_text()) if re.search(args.match, e["file"])]
    root = args.db.resolve().parent
    for level in args.level:
        with ThreadPoolExecutor(args.jobs) as pool:
            results = list(pool.map(lambda e: scan(e, level, args.no_w), entries))
        failed = [f for f, code, _ in results if code != 0]
        unique = {}
        for tu, _, warnings in results:
            for path, row, col, text in warnings:
                unique.setdefault((path, row, col), (text, tu))
        by_area = {}
        for (path, _, _) in unique:
            by_area[area(path, root)] = by_area.get(area(path, root), 0) + 1
        print(f"-Wstrict-aliasing={level}: {len(entries)} TUs ({len(failed)} with compile "
              f"errors), {len(unique)} unique warnings; by area {json.dumps(by_area)}")
        for (path, row, col), (text, tu) in sorted(unique.items()):
            print(f"  {os.path.relpath(path, root)}:{row}:{col}: {text}"
                  f"  (first TU: {os.path.relpath(os.path.join(root, tu), root)})")
        for f in failed[:10]:
            print(f"  compile error in {f}")
        if args.save:
            Path(f"{args.save}-L{level}.json").write_text(json.dumps(
                {"level": level, "tus": len(entries), "failed": failed,
                 "warnings": [[os.path.relpath(p, root), r, c, t, tu]
                              for (p, r, c), (t, tu) in sorted(unique.items())]}, indent=1))


if __name__ == "__main__":
    main()
