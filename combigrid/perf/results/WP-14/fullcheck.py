#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""WP-14 complete-project check (Python standard library only; run from the repo root).

One state (TAG, e.g. head / after) = the working tree as it is, built and checked with OPT=1 and
the judged flags:
  all    SG_ALL=1 SG_PYTHON=1 SG_JAVA=1: one `scons -k` build (libraries, pysgpp, jsgpp, Boost
         tests, examples), then the Boost test binaries directly (--report_level=short:
         deterministic test-case counts), then one `scons -k -j1` pass per RUN_CPP_EXAMPLES /
         RUN_PYTHON_EXAMPLES / RUN_PYTHON_TESTS. Separate passes because SCons chains the run
         targets (finalStepDependencies), so with -k one failure would skip all later kinds.
         jsgpp builds with combigrid only with the owner's working-tree edit of
         combigrid/build/jsgpp/combigrid.i (its includes of old-module headers commented out).
  java   the Java examples of the modules and JsgppSmoke.java (next to this script) against the
         jsgpp of the `all` build.
  summary / compare   condensed results (per module and per run target) of one or two states.
Logs: combigrid/perf/.work/logs/wp14/<TAG>/; summaries: combigrid/perf/results/WP-14/ (both in
the main tree, also for --repo).
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

MAIN = Path(__file__).resolve().parents[4]
HERE = Path(__file__).resolve().parent
LOGS = MAIN / "combigrid/perf/.work/logs/wp14"
REPO = MAIN  # the tree that is built and checked (--repo, e.g. a git worktree of HEAD)
MODULES = ["base", "combigrid", "datadriven", "misc", "optimization", "pde", "quadrature",
           "solver"]
JUDGED = ["OPT=1", "ARCH=avx2", "USE_EIGEN=1", "USE_ARMADILLO=1",
          "CPPFLAGS=-march=native -flto=auto", "LINKFLAGS=-march=native -flto=auto"]
ALL = ["SG_ALL=1", "SG_PYTHON=1", "SG_JAVA=1"]
NO_RUN = ["RUN_BOOST_TESTS=0", "RUN_CPP_EXAMPLES=0", "RUN_PYTHON_EXAMPLES=0",
          "RUN_PYTHON_TESTS=0"]
RUN_PASSES = {"cpp": "RUN_CPP_EXAMPLES=1", "python": "RUN_PYTHON_EXAMPLES=1",
              "pytests": "RUN_PYTHON_TESTS=1"}
JAVA_EXAMPLES = [("base/examples", "tutorial.java", "tutorial"),
                 ("base/examples", "refinement.java", "refinement"),
                 ("optimization/examples", "optimization.java ExampleFunction.java",
                  "optimization"),
                 ("datadriven/examples", "example_learnerSGDE.java", "example_learnerSGDE")]


def scons(tag, name, args, jobs, extra):
    LOGS.joinpath(tag).mkdir(parents=True, exist_ok=True)
    log = LOGS / tag / f"{name}.log"
    cmd = ["scons", "-k", f"-j{jobs}", *args, *JUDGED, *extra]
    start = time.time()
    with open(log, "w") as f:
        f.write("# " + " ".join(cmd) + "\n")
        f.flush()
        code = subprocess.run(cmd, cwd=REPO, stdout=f, stderr=subprocess.STDOUT).returncode
    seconds = time.time() - start
    print(f"[{tag}] {name}: scons exit {code} in {seconds:.0f}s (log {log})", flush=True)
    return {"name": name, "exit": code, "seconds": round(seconds), "log": str(log)}


def run(cmd, cwd, log, env=None, timeout=3600):
    start = time.time()
    with open(log, "w") as f:
        try:
            code = subprocess.run(cmd, cwd=cwd, stdout=f, stderr=subprocess.STDOUT, env=env,
                                  timeout=timeout).returncode
        except subprocess.TimeoutExpired:
            code = "timeout"
    return code, round(time.time() - start, 1)


def lib_env():
    env = dict(os.environ)
    env["LD_LIBRARY_PATH"] = os.pathsep.join([str(REPO / "lib"), str(REPO / "lib/jsgpp"),
                                              env.get("LD_LIBRARY_PATH", "")])
    return env


def boost_tests(tag):
    """Runs every module's Boost test binary as SCons' BoostTest builder does (from the repo
    root, --log_level=test_suite) plus --report_level=short for the counts."""
    results = {}
    for module in MODULES:
        exe = REPO / module / "tests" / f"test_{module}_boost"
        if not exe.exists():
            results[module] = {"exit": "missing"}
            continue
        log = LOGS / tag / f"boost-{module}.log"
        code, seconds = run([f"./{module}/tests/test_{module}_boost", "--log_level=test_suite",
                             "--report_level=short"], REPO, log, lib_env())
        results[module] = {"exit": code, "seconds": seconds, **boost_counts(log)}
        print(f"[{tag}] boost {module}: exit {code} ({seconds}s) {results[module]}", flush=True)
    return results


def boost_counts(log):
    text = Path(log).read_text(errors="replace")
    counts = {}
    for key, rx in (("cases", r"(\d+) test cases? out of (\d+) passed"),
                    ("assertions", r"(\d+) assertions? out of (\d+) passed")):
        m = re.search(rx, text)
        if m:
            counts[key] = f"{m.group(1)}/{m.group(2)}"
    for key, rx in (("cases_failed", r"(\d+) test cases? out of \d+ failed"),
                    ("assertions_failed", r"(\d+) assertions? out of \d+ failed"),
                    ("cases_skipped", r"(\d+) test cases? out of \d+ skipped")):
        m = re.search(rx, text)
        if m:
            counts[key] = int(m.group(1))
    return counts


def cmd_all(args):
    state = {"tag": args.tag, "git": git_state(), "steps": []}
    if not args.skip_build:
        state["steps"].append(scons(args.tag, "build", ALL + ["COMPILE_BOOST_TESTS=1"] + NO_RUN,
                                    args.jobs, args.extra))
    state["boost"] = boost_tests(args.tag)
    for name, flag in RUN_PASSES.items():
        flags = [f for f in NO_RUN if f.split("=")[0] != flag.split("=")[0]] + [flag]
        state["steps"].append(scons(args.tag, f"run-{name}", ALL + ["COMPILE_BOOST_TESTS=1"] +
                                    flags, 1, args.extra))
    save(args.tag, "all", state)


def cmd_java(args):
    state = {"tag": args.tag, "git": git_state(), "steps": [], "java": {}}
    jar = REPO / "lib/jsgpp/jsgpp.jar"
    out = LOGS / args.tag / "java-classes"
    shutil.rmtree(out, ignore_errors=True)
    programs = [(REPO / folder, sources, main) for folder, sources, main in JAVA_EXAMPLES] + \
        [(HERE / "java", "JsgppSmoke.java", "JsgppSmoke")]
    for folder, sources, main in programs:
        classes = out / main
        classes.mkdir(parents=True)
        log = LOGS / args.tag / f"java-{main}.log"
        code, _ = run(["javac", "-cp", str(jar), "-d", str(classes), *sources.split()],
                      folder, log)
        entry = {"javac": code}
        if code == 0:
            # run in the example's folder (some examples read data files relative to it)
            code, seconds = run(["java", "-cp", f"{jar}{os.pathsep}{classes}", main],
                                folder, log, lib_env())
            entry.update({"exit": code, "seconds": seconds})
        state["java"][main] = entry
        print(f"[{args.tag}] java {main}: {entry}", flush=True)
    save(args.tag, "java", state)


def git_state():
    head = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=REPO,
                          capture_output=True, text=True).stdout.strip()
    diff = subprocess.run(["git", "diff", "--stat", "--", "site_scons", "jsgpp", "pysgpp",
                           "combigrid/build"],
                          cwd=REPO, capture_output=True, text=True).stdout.strip()
    return {"repo": str(REPO), "head": head, "diff_build_files": diff}


def save(tag, kind, state):
    path = HERE / f"fullcheck-{tag}-{kind}.json"
    path.write_text(json.dumps(state, indent=1))
    print(f"saved {path}")


# --------------------------------------------------------------------------------------------
# log analysis
# --------------------------------------------------------------------------------------------

ERROR_RX = re.compile(r"^scons: \*\*\* \[(.+?)\] (.*)$")
WARNING_RX = re.compile(r"^(\S.*?):(\d+):(\d+): warning: (.*?)(?: \[(-W[^\]]+)\])?$")


def scons_failures(log):
    return sorted({f"{m.group(1)}: {m.group(2)}" for line in read(log).splitlines()
                   if (m := ERROR_RX.match(line))})


def read(path):
    return Path(path).read_text(errors="replace") if Path(path).exists() else ""


def compiler_warnings(log):
    """Unique compiler warnings (file, line, flag) per module; cpplint lines are skipped."""
    found = {}
    for line in read(log).splitlines():
        line = re.sub(r"\x1b\[[0-9;]*[mK]", "", line)
        m = WARNING_RX.match(line)
        if not m or "[build/" in line or "[whitespace/" in line or "[readability/" in line:
            continue
        file = os.path.normpath(m.group(1)).replace(str(REPO) + os.sep, "")
        found[(file, m.group(2), m.group(5) or "-")] = m.group(4)
    return found


def flag_placement(log, flags=("-fno-strict-aliasing", "-fstrict-aliasing")):
    """Per module/binding: compile lines and how many carry each flag (the last of the two
    wins)."""
    table = {}
    for line in read(log).splitlines():
        m = re.match(r"^(\S+) -o (\S+?)\.o?s? -c ", line)
        if not m:
            continue
        area = m.group(2).split("/")[0]
        row = table.setdefault(area, {"compiles": 0, **{f: 0 for f in flags}, "effective-no": 0})
        row["compiles"] += 1
        tokens = line.split()
        for f in flags:
            row[f] += f in tokens
        last = [t for t in tokens if t in flags]
        row["effective-no"] += bool(last) and last[-1] == "-fno-strict-aliasing"
    return table


def run_targets(log):
    """Commands SCons executed in a run pass (one per example / Python test)."""
    targets = []
    for line in read(log).splitlines():
        if re.match(r"^(cd .* && )?(\./\S+|python3 \S+)( |$)", line) and "scons" not in line:
            targets.append(line.strip())
    return targets


def cmd_summary(args):
    for tag in args.tag:
        print(f"\n## {tag}")
        for kind in ("all", "java"):
            path = HERE / f"fullcheck-{tag}-{kind}.json"
            if not path.exists():
                continue
            state = json.loads(path.read_text())
            print(f"{kind}: git {state['git']}")
            for step in state["steps"]:
                fails = scons_failures(step["log"])
                print(f"  {step['name']}: exit {step['exit']}, {step['seconds']}s, "
                      f"{len(fails)} failed targets")
                for f in fails:
                    print(f"    FAILED {f}")
                if step["name"].startswith("run-"):
                    print(f"    {len(run_targets(step['log']))} run commands")
                if step["name"].endswith("build"):
                    warnings = compiler_warnings(step["log"])
                    by_flag = {}
                    for (_, _, flag) in warnings:
                        by_flag[flag] = by_flag.get(flag, 0) + 1
                    print(f"    {len(warnings)} unique compiler warnings by flag: {by_flag}")
                    for (file, row, flag), text in sorted(warnings.items()):
                        if "aliasing" in flag:
                            print(f"    {file}:{row} {flag}: {text}")
                    for area, row in sorted(flag_placement(step["log"]).items()):
                        print(f"    flags {area}: {row}")
            for module, res in state.get("boost", {}).items():
                print(f"  boost {module}: {res}")
            for main, res in state.get("java", {}).items():
                print(f"  java {main}: {res}")


def cmd_compare(args):
    """Pass/fail of every run target and Boost module, and the run commands' outputs (only
    lines that differ, timing lines are noise)."""
    a, b = args.a, args.b
    for kind in ("all", "java"):
        pa, pb = HERE / f"fullcheck-{a}-{kind}.json", HERE / f"fullcheck-{b}-{kind}.json"
        if not (pa.exists() and pb.exists()):
            continue
        sa, sb = json.loads(pa.read_text()), json.loads(pb.read_text())
        print(f"\n## {kind}: {a} vs {b}")
        for step_a, step_b in zip(sa["steps"], sb["steps"]):
            fa, fb = scons_failures(step_a["log"]), scons_failures(step_b["log"])
            same = "identical" if fa == fb else "DIFFERENT"
            print(f"  {step_a['name']}: failed targets {len(fa)} vs {len(fb)} ({same})")
            for f in sorted(set(fa) ^ set(fb)):
                print(f"    only in {'A' if f in fa else 'B'}: {f}")
            if step_a["name"].startswith("run-"):
                ra, rb = run_targets(step_a["log"]), run_targets(step_b["log"])
                print(f"    run commands {len(ra)} vs {len(rb)}"
                      f" ({'identical' if ra == rb else 'DIFFERENT'})")
        for module in sa.get("boost", {}):
            ra, rb = sa["boost"][module], sb["boost"].get(module, {})
            keys = ("exit", "cases", "cases_failed", "cases_skipped", "assertions")
            diff = {k: (ra.get(k), rb.get(k)) for k in keys if ra.get(k) != rb.get(k)}
            print(f"  boost {module}: exit {ra.get('exit')}/{rb.get('exit')} cases "
                  f"{ra.get('cases')}/{rb.get('cases')}" + (f" differences {diff}" if diff else ""))
        for main in sa.get("java", {}):
            ra, rb = sa["java"][main], sb["java"].get(main)
            la, lb = LOGS / a / f"java-{main}.log", LOGS / b / f"java-{main}.log"
            print(f"  java {main}: {ra} vs {rb}; output "
                  f"{'identical' if read(la) == read(lb) else 'DIFFERENT'}")


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    p = sub.add_parser("all")
    p.add_argument("tag")
    p.add_argument("--jobs", type=int, default=16)
    p.add_argument("--skip-build", action="store_true")
    p.add_argument("--extra", nargs="*", default=[], help="additional SCons arguments")
    p.set_defaults(func=cmd_all)
    p = sub.add_parser("java")
    p.add_argument("tag")
    p.set_defaults(func=cmd_java)
    p = sub.add_parser("summary")
    p.add_argument("tag", nargs="+")
    p.set_defaults(func=cmd_summary)
    p = sub.add_parser("compare")
    p.add_argument("a")
    p.add_argument("b")
    p.set_defaults(func=cmd_compare)
    parser.add_argument("--repo", type=Path, default=MAIN, help="tree to build and check")
    args = parser.parse_args()
    global REPO
    REPO = args.repo.resolve()
    args.func(args)


if __name__ == "__main__":
    main()
