#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""Toolchain variants for the combigrid performance suite (WP-11, WP-14 added gcc-sa; Python
standard library only).

Builds base + combigrid with a compiler/flag variant of the judged configuration and snapshots
it, reusing the machinery of the protected driver perf.py (imported, never modified): the SCons
invocation and log handling (`run_scons`), the build stamp format, and the snapshot layout
(`copy_build`). Measurements on the snapshots are done with perf.py itself (`ab`, `refcheck`,
`profile`, `test`), so the A/B, noise-model and refcheck semantics are those of perf.py.

Truthful labels: the build stamp's `config` is the variant name (`normal` only for the variant
`gcc`, whose SCons arguments are exactly perf.py's `normal` ones), so `perf.py snapshot` and the
`current` snapshot refuse variant builds; use `toolchain.py snapshot`. Every snapshot's meta.json
records the variant, its SCons arguments, the compiler/linker found in the libraries
(`readelf -p .comment`), the OpenMP/BLAS runtimes actually loaded (`ldd`), and the full compile
and link command lines taken from the build log.

Commands (run from the repo root):
  variants                     list the variants and their SCons arguments
  build VARIANT [--clean]      (clean and) build a variant; writes the build stamp
  snapshot NAME                snapshot the current variant build with toolchain metadata
  timing VARIANT               build-time protocol: clean build, edit/revert one .cpp and one
                               widely included header (each rebuild timed); records to
                               --record (JSON lines). The clean keeps lib/ entries that the
                               build does not recreate (other modules' libraries, lib/pysgpp).
  warnings LOG                 count compiler warnings in a build log (unique locations)
  info SNAPSHOT                print the toolchain metadata of a snapshot
  overview REPORT.json ...     condensed tables of saved A/B reports (groups, d >= 7, geomean)

SCons never reads buildVars.out (it is written at exit only; variables come from the command
line and custom.py), so variants cannot leak into later builds; the file is still backed up
before the first variant build and can be restored with `restore-buildvars`.
"""

import argparse
import importlib.util
import json
import math
import os
import re
import shutil
import statistics
import subprocess
import sys
import time
from pathlib import Path

PERF_PY = Path(__file__).resolve().parent / "perf.py"
_spec = importlib.util.spec_from_file_location("perf", PERF_PY)
perf = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(perf)

REPO = perf.REPO
WORK = perf.WORK
BUILDVARS = REPO / "buildVars.out"
BUILDVARS_BACKUP = WORK / "buildVars.out.toolchain-backup"
# relative to the repo root (SCons runs the linker there); the repo path contains spaces, which
# SCons' flag converter would split
THINLTO_CACHE = "combigrid/perf/.work/thinlto-cache"

NORMAL = perf.BUILD_CONFIGS["normal"]
DEBUG = perf.BUILD_CONFIGS["debug"]
GCC_CPP = "-march=native -flto=auto"
GCC_LINK = "-march=native -flto=auto"
# Without -O at link time lld runs the LTO backend at its default O2; GCC uses the highest -O of
# the compile step. -O3 in LINKFLAGS makes the Clang driver pass -plugin-opt=O3.
CLANG_LINK = "-fuse-ld=lld -O3"
NOSI = "-fno-semantic-interposition"
# SGppConfigure.py appends -fno-strict-aliasing after the user's CPPFLAGS, so -fstrict-aliasing in
# CPPFLAGS loses (the last -f wins). The wrapper g++-sa appends it after all arguments instead;
# build() prepends WRAPPERS to PATH for the variants in PATH_VARIANTS (SCons passes os.environ to
# the commands). The objects' DW_AT_producer records the effective flag order.
WRAPPERS = Path(__file__).resolve().parent / "wrappers"
SA = "-fstrict-aliasing"
PATH_VARIANTS = {"gcc-sa"}


# SConstruct runs its configure checks (e.g. for the Eigen headers) with $CXX before
# configureClangCompiler sets CXX=clang++, i.e. with g++, which rejects -flto=thin; the explicit
# overrides make the checks use Clang as well.
CLANG = ["COMPILER=clang", "CC=clang", "CXX=clang++", "LINK=clang++"]


def _with_flags(cpp, link, compiler=None):
    base = [a for a in NORMAL if not a.startswith(("CPPFLAGS=", "LINKFLAGS="))]
    return (CLANG if compiler == "clang" else []) + base + \
        [f"CPPFLAGS={cpp}", f"LINKFLAGS={link}"]


VARIANTS = {
    # name: (stamp config, scons arguments, description)
    "gcc": ("normal", NORMAL, "judged configuration (perf.py build normal), GCC WHOPR LTO"),
    "gcc-nosi": ("gcc-nosi", _with_flags(f"{GCC_CPP} {NOSI}", f"{GCC_LINK} {NOSI}"),
                 "gcc + -fno-semantic-interposition"),
    "gcc-nosi-bsym": ("gcc-nosi-bsym",
                      _with_flags(f"{GCC_CPP} {NOSI} -fno-plt",
                                  f"{GCC_LINK} {NOSI} -fno-plt -Wl,-Bsymbolic-functions"),
                      "gcc-nosi + -fno-plt + -Wl,-Bsymbolic-functions"),
    "gcc-sa": ("gcc-sa", ["CXX=g++-sa"] + _with_flags(GCC_CPP, f"{GCC_LINK} {SA}"),
               "gcc + -fstrict-aliasing (WP-14; last on every compile line via wrappers/g++-sa)"),
    "clang-thin": ("clang-thin",
                   _with_flags("-march=native -flto=thin",
                               f"-march=native -flto=thin {CLANG_LINK}", "clang"),
                   "Clang ThinLTO, lld"),
    "clang-thin-cache": ("clang-thin-cache",
                         _with_flags("-march=native -flto=thin",
                                     f"-march=native -flto=thin {CLANG_LINK} "
                                     f"-Wl,--thinlto-cache-dir={THINLTO_CACHE}", "clang"),
                         "clang-thin + ThinLTO cache (build times only)"),
    "clang-full": ("clang-full",
                   _with_flags("-march=native -flto", f"-march=native -flto {CLANG_LINK}",
                               "clang"),
                   "Clang full LTO, lld"),
    "clang-thin-nosi": ("clang-thin-nosi",
                        _with_flags(f"-march=native -flto=thin {NOSI}",
                                    f"-march=native -flto=thin {CLANG_LINK} {NOSI}", "clang"),
                        "clang-thin + -fno-semantic-interposition"),
    "clang-full-nosi": ("clang-full-nosi",
                        _with_flags(f"-march=native -flto {NOSI}",
                                    f"-march=native -flto {CLANG_LINK} {NOSI}", "clang"),
                        "clang-full + -fno-semantic-interposition"),
    "clang-thin-fpfast": ("clang-thin-fpfast",
                          _with_flags("-march=native -flto=thin -ffp-contract=fast",
                                      f"-march=native -flto=thin {CLANG_LINK}", "clang"),
                          "clang-thin + -ffp-contract=fast (GCC's default; refcheck diagnostic)"),
    "gcc-debug": ("debug", DEBUG, "perf.py build debug (build times only)"),
    "clang-debug": ("clang-debug", CLANG + DEBUG,
                    "debug with Clang (build times only)"),
}

TIMING_CPP = "combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.cpp"
TIMING_HPP = "combigrid/src/sgpp/combigrid/type_defs.hpp"
PROBE_LINE = "// toolchain.py build-time probe (temporary edit, reverted automatically)\n"
BANNER_LINES = 4


def scons_args(variant, tests, jobs, extra=()):
    args = ["scons", f"-j{jobs}", *perf.SCONS_COMMON, *VARIANTS[variant][1], *extra]
    if tests == "skip":
        return args + ["COMPILE_BOOST_TESTS=0"]
    return args + ["COMPILE_BOOST_TESTS=1",
                   "RUN_BOOST_TESTS=1" if tests == "run" else "RUN_BOOST_TESTS=0"]


def backup_buildvars():
    if BUILDVARS.exists() and not BUILDVARS_BACKUP.exists():
        WORK.mkdir(parents=True, exist_ok=True)
        shutil.copy2(BUILDVARS, BUILDVARS_BACKUP)
        print(f"buildVars.out backed up to {BUILDVARS_BACKUP}")


def preserve_lib():
    """`scons -c` removes all of lib/ (SConstruct: env.Clean("clean", ["lib/", ...])), including
    libraries of modules outside SG_BASE/SG_COMBIGRID and an installed lib/pysgpp, which an
    SG_ALL=0 build does not recreate. Copies everything except base/combigrid's libraries aside;
    restore_lib() puts back what the build did not recreate."""
    keep = WORK / "lib-preserved"
    shutil.rmtree(keep, ignore_errors=True)
    keep.mkdir(parents=True)
    for entry in (REPO / "lib").iterdir() if (REPO / "lib").exists() else []:
        if entry.name in perf.LIBS:
            continue
        if entry.is_dir() and not entry.is_symlink():
            shutil.copytree(entry, keep / entry.name, symlinks=True)
        else:
            shutil.copy2(entry, keep / entry.name, follow_symlinks=False)
    return keep


def restore_lib(keep):
    for entry in keep.iterdir():
        dest = REPO / "lib" / entry.name
        if dest.exists() or dest.is_symlink():
            continue
        if entry.is_dir() and not entry.is_symlink():
            shutil.copytree(entry, dest, symlinks=True)
        else:
            shutil.copy2(entry, dest, follow_symlinks=False)
        print(f"  restored lib/{entry.name} (removed by scons -c)")
    shutil.rmtree(keep, ignore_errors=True)


def build(variant, tests="compile", jobs=16, clean=False, tag="", extra=()):
    """Builds a variant via perf.run_scons (with its SCons arguments plus @p extra, e.g.
    SG_PYTHON=1) and writes the build stamp. Returns (wall-clock seconds of the build, log path);
    a preceding clean is not timed."""
    backup_buildvars()
    if variant in PATH_VARIANTS and not os.environ["PATH"].startswith(f"{WRAPPERS}:"):
        os.environ["PATH"] = f"{WRAPPERS}:{os.environ['PATH']}"
    (WORK / "logs").mkdir(parents=True, exist_ok=True)
    stem = f"build-{variant}{'-' + tag if tag else ''}-{perf.now_tag()}"
    keep = None
    if clean:
        keep = preserve_lib()
        clean_log = WORK / "logs" / f"{stem}-clean.log"
        with open(clean_log, "w") as f:
            result = subprocess.run(scons_args(variant, tests, jobs, extra) + ["-c"], cwd=REPO,
                                    stdout=f, stderr=subprocess.STDOUT)
        if result.returncode != 0:
            restore_lib(keep)
            perf.die(f"scons -c failed, see {clean_log}")
    config, _, _ = VARIANTS[variant]
    if extra:
        config += "+" + "+".join(extra)
    perf.BUILD_CONFIGS[config] = VARIANTS[variant][1] + list(extra)
    log = WORK / "logs" / f"{stem}.log"
    start = time.time()
    try:
        args = perf.run_scons(REPO, config, tests, jobs, log)
    finally:
        if keep:
            restore_lib(keep)
    seconds = time.time() - start
    assert args == scons_args(variant, tests, jobs, extra)
    perf.STAMP.write_text(json.dumps({"config": config, "variant": variant, "scons": args,
                                      "time": time.time(), "log": str(log),
                                      "git": perf.git_state()}, indent=1))
    return seconds, log


# --------------------------------------------------------------------------------------------
# toolchain metadata
# --------------------------------------------------------------------------------------------

def readelf_comment(path):
    out = subprocess.run(["readelf", "-p", ".comment", str(path)], capture_output=True,
                         text=True).stdout
    return sorted({m.group(1).strip() for m in re.finditer(r"\]\s+(.*)", out)})


def loaded_runtimes(exe, lib):
    env = dict(os.environ, LD_LIBRARY_PATH=str(lib))
    out = subprocess.run(["ldd", str(exe)], env=env, capture_output=True, text=True).stdout
    keep = ("omp", "blas", "armadillo", "lapack", "sgpp", "stdc++", "libm.", "libgcc")
    return [line.strip() for line in out.splitlines() if any(k in line for k in keep)]


def commands_from_log(log):
    """Full command lines of one combigrid compile and of the combigrid library link."""
    found = {}
    for line in perf.read_file(log).splitlines():
        if re.search(r"\s-c\s.*operators/quadrature/quadrature\.cpp$", line):
            found["compile_quadrature_cpp"] = line
        elif re.match(r"^\S+ -o combigrid/libsgppcombigrid\.so .*-shared", line):
            # object file list shortened
            found["link_libsgppcombigrid"] = line.split(" combigrid/src/", 1)[0] + " <objects>"
    return found


def toolchain_meta(src_lib, src_exe, stamp):
    meta = {"variant": stamp.get("variant", "gcc" if stamp["config"] == "normal" else "?"),
            "scons": stamp["scons"],
            "comment_libsgppcombigrid": readelf_comment(src_lib / "libsgppcombigrid.so"),
            "comment_libsgppbase": readelf_comment(src_lib / "libsgppbase.so"),
            "comment_perf_e2e": readelf_comment(src_exe),
            "ldd_perf_e2e": loaded_runtimes(src_exe, src_lib)}
    log = stamp.get("log")
    commands = commands_from_log(log) if log else {}
    meta["commands"] = commands or "not in the last build log (incremental build)"
    return meta


# --------------------------------------------------------------------------------------------
# commands
# --------------------------------------------------------------------------------------------

def cmd_variants(args):
    for name, (config, scons, description) in VARIANTS.items():
        print(f"{name:17} stamp config '{config}': {description}\n    {' '.join(scons)}")


def cmd_build(args):
    seconds, log = build(args.variant, args.tests, args.jobs, args.clean, extra=args.extra)
    print(f"variant '{args.variant}' built in {seconds:.0f}s (log: {log})")
    if args.record:
        record(args.record, args.variant, "clean" if args.clean else "build", seconds, log)


def cmd_snapshot(args):
    _, lib, stamp = perf.current_build()
    if stamp["config"] not in [c for c, _, _ in VARIANTS.values()] or \
            stamp["config"] in ("debug", "clang-debug"):
        perf.die(f"current build '{stamp['config']}' is not an optimized toolchain variant")
    dest = WORK / "snapshots" / args.name
    if dest.exists() and not args.force:
        perf.die(f"snapshot '{args.name}' exists (use --force to replace it)")
    perf.copy_build(REPO, dest, {"name": args.name, "source": "working tree",
                                 "time": time.time(), "stamp": stamp, "git": perf.git_state()})
    meta = json.loads((dest / "meta.json").read_text())
    meta["toolchain"] = toolchain_meta(dest / "lib", dest / "bin" / perf.EXES["e2e"], stamp)
    (dest / "meta.json").write_text(json.dumps(meta, indent=1))
    print(f"snapshot '{args.name}' saved in {dest}")
    print(f"  build info: {json.dumps(meta.get('build_info'))}")
    print_toolchain(meta["toolchain"])


def print_toolchain(tc):
    print(f"  variant: {tc['variant']}")
    print(f"  libsgppcombigrid .comment: {tc['comment_libsgppcombigrid']}")
    print(f"  perf_e2e .comment: {tc['comment_perf_e2e']}")
    for line in tc["ldd_perf_e2e"]:
        print(f"  ldd: {line}")


def cmd_info(args):
    meta = json.loads((WORK / "snapshots" / args.snapshot / "meta.json").read_text())
    print(f"build info: {json.dumps(meta.get('build_info'))}")
    if "toolchain" in meta:
        print_toolchain(meta["toolchain"])
        commands = meta["toolchain"]["commands"]
        if isinstance(commands, dict):
            for key, line in commands.items():
                print(f"  {key}: {line}")
    else:
        root = WORK / "snapshots" / args.snapshot
        print(f"  (no toolchain metadata) .comment: {readelf_comment(root / 'lib' / 'libsgppcombigrid.so')}")
        for line in loaded_runtimes(root / "bin" / perf.EXES["e2e"], root / "lib"):
            print(f"  ldd: {line}")


def record(path, variant, kind, seconds, log):
    entry = {"variant": variant, "kind": kind, "seconds": round(seconds, 1), "log": str(log),
             "time": perf.now_tag()}
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    with open(path, "a") as f:
        f.write(json.dumps(entry) + "\n")
    print(f"  recorded {variant} {kind}: {seconds:.1f}s")


def probe_edit(rel):
    """Inserts a comment line after the copyright banner (shifts all line numbers, so the debug
    info and thus the object files change, as with a real edit). Returns the original bytes."""
    path = REPO / rel
    original = path.read_bytes()
    lines = original.decode().splitlines(keepends=True)
    path.write_text("".join(lines[:BANNER_LINES]) + PROBE_LINE + "".join(lines[BANNER_LINES:]))
    return original


def cmd_timing(args):
    if not perf.envcheck(quiet=True):
        perf.die("machine not idle (see above)")
    seconds, log = build(args.variant, args.tests, args.jobs, clean=True, tag="clean")
    record(args.record, args.variant, "clean", seconds, log)
    for kind, rel in (("edit-cpp", args.cpp), ("edit-hpp", args.hpp)):
        original = probe_edit(rel)
        try:
            seconds, log = build(args.variant, args.tests, args.jobs, tag=kind)
            record(args.record, args.variant, kind, seconds, log)
        finally:
            (REPO / rel).write_bytes(original)
        seconds, log = build(args.variant, args.tests, args.jobs, tag=kind + "-revert")
        record(args.record, args.variant, kind + "-revert", seconds, log)
    diff = perf.git("status", "--porcelain", "--", args.cpp, args.hpp)
    if diff.strip():
        perf.die(f"probe files not restored: {diff}")
    print("timing done; probe files restored (git status clean for them)")


WARNING_RX = re.compile(r"^(\S+?):(\d+):(\d+): warning: (.*?)(?: \[(-W[^\]]+)\])?$")


def count_warnings(log):
    """Unique compiler warnings (file, line, column, flag) per source area and flag."""
    unique = {}
    for line in perf.read_file(log).splitlines():
        m = WARNING_RX.match(re.sub(r"\x1b\[[0-9;]*[mK]", "", line))
        if not m:
            continue
        file, row, col, _, flag = m.groups()
        file = os.path.normpath(file)
        unique[(file, row, col, flag or "-")] = True
    areas, flags = {}, {}
    for (file, _, _, flag) in unique:
        area = next((a for a in ("combigrid/src", "combigrid/tests", "combigrid/examples",
                                 "base/src") if file.startswith(a)), "other")
        areas[area] = areas.get(area, 0) + 1
        flags[flag] = flags.get(flag, 0) + 1
    return len(unique), areas, flags


def cmd_warnings(args):
    for log in args.log:
        total, areas, flags = count_warnings(log)
        print(f"{log}: {total} unique warnings; by area {json.dumps(areas, sort_keys=True)}")
        top = sorted(flags.items(), key=lambda kv: -kv[1])[: args.top]
        print("  by flag: " + ", ".join(f"{k} {v}" for k, v in top))


def e2e_group(case):
    parts = case.split("/")
    return "/".join(parts[:2]) if parts[0] == "quad" else parts[0]


def load_ab(path):
    """Per common case: {metric: (median A, median B, speedup, lo, hi)} with perf.py's medians
    and paired speedups (noise model included)."""
    report = json.loads(Path(path).read_text())
    suite, data = report["suite"], report["data"]
    noise = json.loads(perf.NOISE_FILES[suite].read_text())
    cases = {}
    for key in data:
        side, case = key.split("|", 1)
        if side != "A" or "B|" + case not in data:
            continue
        recs_a, recs_b = data[key], data["B|" + case]
        for metric in recs_a[0]["metrics"]:
            sa = [r["metrics"][metric] for r in recs_a]
            sb = [r["metrics"][metric] for r in recs_b]
            point, lo, hi, _ = perf.paired_speedup(sa, sb, noise.get(case, {}).get(metric, 0.0))
            med_a = statistics.median([x for r in sa for x in r])
            med_b = statistics.median([x for r in sb for x in r])
            cases.setdefault(case, {})[metric] = (med_a, med_b, point, lo, hi)
    return suite, report["b"], cases


def verdict_counts(rows):
    counts = {"faster": 0, "SLOWER": 0, "~ (noise)": 0}
    for _, _, _, lo, hi in rows:
        counts[perf.verdict(lo, hi)] += 1
    return f"{counts['faster']}/{counts['SLOWER']}/{counts['~ (noise)']}"


def cmd_overview(args):
    """Condensed tables of saved A/B reports: e2e sums of medians per operator group and for the
    d >= 7 cases (total, generation, operator), micro geometric mean of the speedups."""
    for path in args.report:
        suite, b, cases = load_ab(path)
        print(f"\n### {Path(path).stem} (B = {b}; verdicts faster/SLOWER/noise)")
        if suite == "micro":
            rows = [m["call"] for m in cases.values()]
            geo = math.exp(statistics.fmean(math.log(r[2]) for r in rows))
            print(f"micro: {len(rows)} cases, geomean speedup {geo:.3f}, min "
                  f"{min(r[2] for r in rows):.3f}, max {max(r[2] for r in rows):.3f}, verdicts "
                  f"{verdict_counts(rows)}")
            for case, m in sorted(cases.items(), key=lambda kv: kv[1]["call"][2]):
                if perf.verdict(*m["call"][3:]) != "~ (noise)" or args.all:
                    a, bb, p, lo, hi = m["call"]
                    print(f"  {case}: {perf.fmt_time(a)} -> {perf.fmt_time(bb)} {p:.3f} "
                          f"[{lo:.3f}, {hi:.3f}]")
            continue
        groups = {}
        for case in cases:
            groups.setdefault(e2e_group(case), []).append(case)
            if int(re.search(r"/d(\d+)/", case).group(1)) >= 7:
                groups.setdefault("d >= 7", []).append(case)
            groups.setdefault("all", []).append(case)
        print("| group | cases | total A | total B | A/B | verdicts | generation A/B | "
              "operator A/B |\n|---|---|---|---|---|---|---|---|")
        order = ["quad/sg", "quad/dsg", "interp", "interplin", "mivec", "d >= 7", "all"]
        for group, names in sorted(groups.items(), key=lambda kv: order.index(kv[0])
                                   if kv[0] in order else len(order)):
            cells = []
            for metric in ("total", "generation", "operator"):
                rows = [cases[c][metric] for c in names if metric in cases[c]]
                sa, sb = sum(r[0] for r in rows), sum(r[1] for r in rows)
                cells.append((sa, sb, rows))
            (ta, tb, trows), (ga, gb, _), (oa, ob, _) = cells
            print(f"| {group} | {len(names)} | {perf.fmt_time(ta)} | {perf.fmt_time(tb)} | "
                  f"{ta / tb:.3f} | {verdict_counts(trows)} | {ga / gb:.3f} | {oa / ob:.3f} |")
        extremes = sorted(cases.items(), key=lambda kv: kv[1]["total"][2])
        shown = extremes[:args.extremes] + extremes[-args.extremes:] if args.extremes > 0 else []
        for case, m in shown:
            a, bb, p, lo, hi = m["total"]
            print(f"  {case}: total {perf.fmt_time(a)} -> {perf.fmt_time(bb)} {p:.3f} "
                  f"[{lo:.3f}, {hi:.3f}] {perf.verdict(lo, hi)}")


def cmd_restore_buildvars(args):
    if not BUILDVARS_BACKUP.exists():
        perf.die("no backup of buildVars.out")
    shutil.copy2(BUILDVARS_BACKUP, BUILDVARS)
    print(f"buildVars.out restored from {BUILDVARS_BACKUP}")


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("variants").set_defaults(func=cmd_variants)

    p = sub.add_parser("build")
    p.add_argument("variant", choices=list(VARIANTS))
    p.add_argument("--clean", action="store_true", help="scons -c first (not timed)")
    p.add_argument("--tests", choices=["compile", "run", "skip"], default="compile")
    p.add_argument("--jobs", type=int, default=16)
    p.add_argument("--record", help="append the build time to this JSON-lines file")
    p.add_argument("--extra", nargs="*", default=[],
                   help="additional SCons arguments (e.g. SG_PYTHON=1); the stamp config gets "
                        "them appended, so benchmarks never mistake the build for a variant")
    p.set_defaults(func=cmd_build)

    p = sub.add_parser("snapshot")
    p.add_argument("name")
    p.add_argument("--force", action="store_true")
    p.set_defaults(func=cmd_snapshot)

    p = sub.add_parser("timing")
    p.add_argument("variant", choices=list(VARIANTS))
    p.add_argument("--record", required=True, help="JSON-lines file for the build times")
    p.add_argument("--cpp", default=TIMING_CPP)
    p.add_argument("--hpp", default=TIMING_HPP)
    p.add_argument("--tests", choices=["compile", "run", "skip"], default="compile")
    p.add_argument("--jobs", type=int, default=16)
    p.set_defaults(func=cmd_timing)

    p = sub.add_parser("warnings")
    p.add_argument("log", nargs="+")
    p.add_argument("--top", type=int, default=12)
    p.set_defaults(func=cmd_warnings)

    p = sub.add_parser("info")
    p.add_argument("snapshot")
    p.set_defaults(func=cmd_info)

    p = sub.add_parser("overview")
    p.add_argument("report", nargs="+", help="saved A/B reports (.json) of perf.py ab")
    p.add_argument("--extremes", type=int, default=3, help="e2e: show the N best/worst cases")
    p.add_argument("--all", action="store_true", help="micro: list all cases")
    p.set_defaults(func=cmd_overview)

    sub.add_parser("restore-buildvars").set_defaults(func=cmd_restore_buildvars)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
