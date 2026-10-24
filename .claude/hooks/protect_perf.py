#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""PreToolUse hook: blocks modifications of the protected files of the combigrid performance
suite (baseline, reference results and tolerances, protected benchmarks, measurement scripts).

The protected paths are read from combigrid/perf/protected.sha256 ("#dir" / "#glob" lines and
the hashed files). Blocks Edit/Write/MultiEdit/NotebookEdit on them and Bash commands that
obviously write to them (output redirection, sed/perl -i, rm, mv, tee, truncate, chmod, the
destination of cp/install/rsync/ln, git checkout/restore/rm/mv/stash). `perf.py verify`
(checksums) catches everything else. Exit code 2 = block (the reason goes to stderr).

Protected files change only on explicit instruction of the project owner: the owner edits them
outside of Claude Code (or temporarily disables this hook) and then runs
`perf.py protect --authorized-by-owner`.
"""

import fnmatch
import json
import os
import re
import shlex
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
MANIFEST = REPO / "combigrid" / "perf" / "protected.sha256"
MANIFEST_REL = "combigrid/perf/protected.sha256"

MODIFY_ALL = {"rm", "mv", "tee", "truncate", "chmod", "chown", "shred", "unlink"}
MODIFY_DEST = {"cp", "install", "rsync", "ln"}
GIT_MODIFY = {"checkout", "restore", "rm", "mv", "stash", "clean"}


def load_patterns():
    dirs, globs, files = [], [], {MANIFEST_REL}
    try:
        lines = MANIFEST.read_text().splitlines()
    except OSError:
        return dirs, globs, files
    for line in lines:
        if line.startswith("#dir "):
            dirs.append(line[5:].strip())
        elif line.startswith("#glob "):
            globs.append(line[6:].strip())
        elif line.strip() and not line.startswith("#"):
            files.add(line.split(None, 1)[1].strip())
    return dirs, globs, files


def to_rel(path_str, cwd):
    path = Path(path_str).expanduser()
    if not path.is_absolute():
        path = Path(cwd) / path
    norm = os.path.normpath(str(path))
    root = str(REPO) + os.sep
    return norm[len(root):].replace(os.sep, "/") if norm.startswith(root) else None


def is_protected(rel, patterns):
    if rel is None:
        return False
    dirs, globs, files = patterns
    return (rel in files or any(rel.startswith(d) or rel + "/" == d for d in dirs)
            or any(fnmatch.fnmatch(rel, g) for g in globs))


def block(reason):
    print(f"BLOCKED by .claude/hooks/protect_perf.py: {reason}. Protected files of the "
          "combigrid performance suite may only change on explicit instruction of the project "
          "owner (see combigrid/perf/README.md, section 'Protection').", file=sys.stderr)
    sys.exit(2)


def check_paths(paths, cwd, patterns, what):
    for p in paths:
        rel = to_rel(p, cwd)
        if is_protected(rel, patterns):
            block(f"{what} would modify protected file '{rel}'")


def check_bash(command, cwd, patterns):
    for target in re.findall(r"(?<![<0-9&])>{1,2}\|?\s*([^\s;&|<>]+)", command):
        check_paths([target], cwd, patterns, "output redirection")
    for segment in re.split(r"&&|\|\||;|\||\n", command):
        try:
            tokens = shlex.split(segment, posix=True)
        except ValueError:
            tokens = segment.split()
        while tokens and (re.match(r"^\w+=", tokens[0]) or tokens[0] in ("sudo", "env", "command")):
            tokens.pop(0)
        if not tokens:
            continue
        cmd = os.path.basename(tokens[0])
        args = [t for t in tokens[1:] if not t.startswith("-")]
        if cmd in MODIFY_ALL:
            check_paths(args, cwd, patterns, cmd)
        elif cmd in MODIFY_DEST and args:
            check_paths(args[-1:], cwd, patterns, cmd)
        elif cmd in ("sed", "perl") and any(t.startswith("-i") or t == "--in-place"
                                            for t in tokens[1:]):
            check_paths(args, cwd, patterns, f"{cmd} -i")
        elif cmd == "dd":
            check_paths([t[3:] for t in tokens[1:] if t.startswith("of=")], cwd, patterns, "dd")
        elif cmd == "git" and len(tokens) > 1 and tokens[1] in GIT_MODIFY:
            check_paths(args[1:], cwd, patterns, f"git {tokens[1]}")


def main():
    try:
        data = json.load(sys.stdin)
    except ValueError:
        return 0
    tool = data.get("tool_name", "")
    tool_input = data.get("tool_input") or {}
    cwd = data.get("cwd") or os.getcwd()
    patterns = load_patterns()

    if tool in ("Edit", "Write", "MultiEdit", "NotebookEdit"):
        path = tool_input.get("file_path") or tool_input.get("notebook_path")
        if path:
            check_paths([path], cwd, patterns, tool)
    elif tool == "Bash":
        check_bash(tool_input.get("command", ""), cwd, patterns)
    return 0


if __name__ == "__main__":
    sys.exit(main())
