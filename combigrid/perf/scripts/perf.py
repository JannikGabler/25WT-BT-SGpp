#!/usr/bin/env python3
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org
"""Driver of the combigrid performance suite (Python standard library only).

PROTECTED FILE (see combigrid/perf/README.md): it defines how measurements, A/B comparisons and
the reference check are performed. Changes only on explicit instruction of the project owner.

Commands (run `perf.py <command> -h` for options):
  envcheck   check that the machine is in benchmarking state (idle, governor, power, ...)
  calibrate  run fixed baseline workloads and compare with the recorded baseline (machine state)
  build      build base + combigrid in a fixed configuration (normal = judged, debug, asan)
  test       run the combigrid Boost tests of the current build
  snapshot   save the current build (or a build of a commit) for later comparisons
  run        run a benchmark suite (e2e | micro) once and print a table
  ab         interleaved A/B comparison of two snapshots with confidence intervals
  refcheck   compare the reference outputs of a build against the protected reference
  profile    perf-record one benchmark case and print the hottest functions
  verify     check that no protected file was modified
  reject     revert the working tree to HEAD except work-session summaries/results (dry run)
  baseline   record the protected baseline (only once, setup session)
  protect    rehash the protected-file manifest (only on explicit instruction of the owner)
"""

import argparse
import datetime
import fnmatch
import hashlib
import json
import math
import os
import re
import resource
import shutil
import statistics
import subprocess
import sys
import time
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
PERF = REPO / "combigrid" / "perf"
WORK = PERF / ".work"
BENCH_SRC = REPO / "combigrid" / "examples" / "c++" / "benchmarks" / "perf"
TEST_EXE = REPO / "combigrid" / "tests" / "test_combigrid_boost"
LIBS = ["libsgppbase.so", "libsgppcombigrid.so"]
EXES = {"e2e": "perf_e2e", "micro": "perf_micro", "reference": "perf_reference"}
REFERENCE = PERF / "reference" / "reference.jsonl"
TOLERANCES = PERF / "reference" / "tolerances.json"
MANIFEST = PERF / "protected.sha256"
STAMP = WORK / "build_stamp.json"

SCONS_COMMON = ["SG_ALL=0", "SG_BASE=1", "SG_COMBIGRID=1"]
# The judged configuration: compileNormal.sh + Eigen/Armadillo (see combigrid/perf/README.md)
BUILD_CONFIGS = {
    "normal": ["OPT=1", "ARCH=avx2", "USE_EIGEN=1", "USE_ARMADILLO=1",
               "CPPFLAGS=-march=native -flto=auto", "LINKFLAGS=-march=native -flto=auto"],
    "debug": ["OPT=0", "USE_EIGEN=1", "USE_ARMADILLO=1"],
    "asan": ["OPT=0", "USE_EIGEN=1", "USE_ARMADILLO=1",
             "CPPFLAGS=-fsanitize=address -fno-omit-frame-pointer",
             "LINKFLAGS=-fsanitize=address -fno-omit-frame-pointer"],
}

# Files that later sessions must not change. Directories protect every file below them
# (including new ones).
PROTECTED_DIRS = [
    "combigrid/perf/baseline/",
    "combigrid/perf/reference/",
]
PROTECTED_GLOBS = [
    "combigrid/perf/scripts/perf.py",
    "combigrid/examples/c++/benchmarks/perf/perf_harness.hpp",
    "combigrid/examples/c++/benchmarks/perf/perf_problems.hpp",
    "combigrid/examples/c++/benchmarks/perf/perf_e2e.cpp",
    "combigrid/examples/c++/benchmarks/perf/perf_reference.cpp",
    "combigrid/examples/c++/benchmarks/quadrature/*.?pp",
    "combigrid/examples/c++/benchmarks/dsg_quadrature/*.?pp",
    "combigrid/examples/c++/benchmarks/multiindex_vector/*.?pp",
    "combigrid/examples/c++/benchmarks/source_function/*.?pp",
    "combigrid/examples/c++/thesis/comparison/*.?pp",
    "combigrid/src/sgpp/combigrid/tools/benchmarking/benchmarker.hpp",
    ".claude/hooks/protect_perf.py",
    ".claude/settings.json",
]
# Paths kept by `reject` (the record of a rejected attempt must survive). Untracked files are only
# deleted below REJECT_DELETE_UNDER (work sessions create files there; the owner's own untracked
# files elsewhere, e.g. compile scripts, are left alone).
REJECT_KEEP = ["combigrid/perf/summaries/", "combigrid/perf/results/"]
REJECT_DELETE_UNDER = "combigrid/"

DEFAULTS = {  # per suite: threads, rounds, samples, warmups
    # Process-to-process variation dominates the noise (bimodal cases up to ~9 %), so A/B
    # comparisons use many short processes (rounds) instead of many samples per process.
    "e2e": {"threads": 16, "rounds": 5, "samples": 1, "warmups": 1},
    "micro": {"threads": 1, "rounds": 5, "samples": 2, "warmups": 0},
}
# Noise model: per case and metric the standard deviation of the per-round log speedup measured
# in self-comparisons of the baseline (setup session). Used as a lower bound of the spread.
NOISE_FILES = {"e2e": PERF / "baseline" / "noise_e2e.json",
               "micro": PERF / "baseline" / "noise_micro.json"}
NOISE_FLOOR = 0.003  # minimum std of the per-round log speedup
T_QUANTILE = {1: 12.706, 2: 4.303, 3: 3.182, 4: 2.776, 5: 2.571, 6: 2.447, 7: 2.365, 8: 2.306,
              9: 2.262, 10: 2.228, 12: 2.179, 15: 2.131, 20: 2.086, 30: 2.042}


# --------------------------------------------------------------------------------------------
# helpers
# --------------------------------------------------------------------------------------------

def now_tag():
    return datetime.datetime.now().strftime("%Y%m%d-%H%M%S")


def die(message, code=1):
    print(f"ERROR: {message}", file=sys.stderr)
    sys.exit(code)


def git(*args, cwd=REPO, check=True):
    result = subprocess.run(["git", *args], cwd=cwd, capture_output=True, text=True)
    if check and result.returncode != 0:
        die(f"git {' '.join(args)} failed: {result.stderr.strip()}")
    return result.stdout


def git_state():
    rev = git("rev-parse", "HEAD").strip()
    status = git("status", "--porcelain")
    diff = git("diff", "HEAD")
    return {"rev": rev, "dirty": bool(status.strip()),
            "diff_sha256": hashlib.sha256(diff.encode()).hexdigest()[:16],
            "changed_files": [line[3:] for line in status.splitlines()]}


def omp_env(threads):
    env = dict(os.environ)
    env["OMP_NUM_THREADS"] = str(threads)
    if threads == 1:
        env["OMP_PROC_BIND"] = "true"
        env["OMP_PLACES"] = "{4}"
    elif threads <= 8:
        env["OMP_PROC_BIND"] = "close"
        env["OMP_PLACES"] = "cores"
    else:
        env["OMP_PROC_BIND"] = "close"
        env["OMP_PLACES"] = "threads"
    return env


def read_file(path, default=""):
    try:
        return Path(path).read_text().strip()
    except OSError:
        return default


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


class BenchLock:
    """Ensures that only one benchmark process runs at a time (machine must be idle)."""

    def __init__(self):
        self.path = WORK / "bench.lock"

    def __enter__(self):
        WORK.mkdir(parents=True, exist_ok=True)
        if self.path.exists():
            pid = read_file(self.path)
            if pid.isdigit() and Path(f"/proc/{pid}").exists():
                die(f"another benchmark run is active (pid {pid}); run one session at a time")
        self.path.write_text(str(os.getpid()))
        return self

    def __exit__(self, *args):
        try:
            self.path.unlink()
        except OSError:
            pass


# --------------------------------------------------------------------------------------------
# envcheck
# --------------------------------------------------------------------------------------------

def cpu_busy_fraction(interval=1.0):
    def snapshot():
        fields = Path("/proc/stat").read_text().splitlines()[0].split()[1:]
        values = [int(v) for v in fields]
        idle = values[3] + values[4]
        return sum(values), idle
    total0, idle0 = snapshot()
    time.sleep(interval)
    total1, idle1 = snapshot()
    total = total1 - total0
    return 0.0 if total <= 0 else 1.0 - (idle1 - idle0) / total


class ForeignLoad:
    """Measures the CPU time used by other processes while we benchmark: busy time of all CPUs
    (/proc/stat) minus the CPU time of this process and its (waited-for) children. A background
    job that starts in the middle of a long run (e.g. unattended upgrades after a reboot) slows
    multi-threaded runs without envcheck noticing; this makes it visible."""

    THRESHOLD = 0.02  # fraction of the machine's capacity

    def __init__(self):
        self.ticks = os.sysconf("SC_CLK_TCK")
        self.n_cpus = os.cpu_count() or 1
        self.start()

    def _busy(self):
        values = [int(v) for v in Path("/proc/stat").read_text().splitlines()[0].split()[1:]]
        return (sum(values) - values[3] - values[4]) / self.ticks

    @staticmethod
    def _own():
        own = resource.getrusage(resource.RUSAGE_SELF)
        children = resource.getrusage(resource.RUSAGE_CHILDREN)
        return own.ru_utime + own.ru_stime + children.ru_utime + children.ru_stime

    def start(self):
        self.t0, self.busy0, self.own0 = time.time(), self._busy(), self._own()

    def fraction(self):
        """Foreign CPU time since start() as a fraction of the machine's capacity."""
        wall = max(time.time() - self.t0, 1e-9)
        foreign = (self._busy() - self.busy0) - (self._own() - self.own0)
        return max(foreign, 0.0) / (wall * self.n_cpus)


def environment_info():
    cpus = sorted(Path("/sys/devices/system/cpu").glob("cpu[0-9]*"))
    governors = {read_file(c / "cpufreq/scaling_governor", "?") for c in cpus}
    epps = {read_file(c / "cpufreq/energy_performance_preference", "?") for c in cpus}
    ac = [read_file(p / "online") for p in Path("/sys/class/power_supply").glob("*")
          if read_file(p / "type") == "Mains"]
    model = ""
    for line in read_file("/proc/cpuinfo").splitlines():
        if line.startswith("model name"):
            model = line.split(":", 1)[1].strip()
            break
    return {
        "cpu": model,
        "n_cpus": len(cpus),
        "governor": ",".join(sorted(governors)),
        "epp": ",".join(sorted(epps)),
        "boost": read_file("/sys/devices/system/cpu/cpufreq/boost", "?"),
        "platform_profile": read_file("/sys/firmware/acpi/platform_profile", "?"),
        "ac_online": ",".join(ac) if ac else "?",
        "perf_event_paranoid": read_file("/proc/sys/kernel/perf_event_paranoid", "?"),
        "kernel": os.uname().release,
        "compiler": subprocess.run(["g++", "-dumpfullversion"], capture_output=True,
                                   text=True).stdout.strip(),
        "load_avg": read_file("/proc/loadavg").split()[:3],
    }


def baseline_env():
    path = PERF / "baseline" / "environment.json"
    if path.exists():
        return json.loads(path.read_text()).get("environment", {})
    return {}


def envcheck(quiet=False):
    """Returns True if the machine is fit for benchmarking. Prints the findings."""
    info = environment_info()
    problems, warnings = [], []
    busy = cpu_busy_fraction()
    if busy > 0.10:
        problems.append(f"CPU not idle: {busy:.0%} busy (close other applications / wait)")
    elif busy > 0.03:
        warnings.append(f"CPU {busy:.0%} busy")
    if info["governor"] != "performance":
        problems.append(f"governor is '{info['governor']}', expected 'performance'")
    if "1" not in info["ac_online"].split(","):
        problems.append(f"not on AC power ({info['ac_online']})")
    base = baseline_env()
    if base and base.get("boost") != info["boost"]:
        problems.append(f"boost={info['boost']} differs from the baseline (boost={base.get('boost')})")
    if base and base.get("platform_profile", info["platform_profile"]) != info["platform_profile"]:
        problems.append(f"ACPI platform profile '{info['platform_profile']}' differs from the "
                        f"baseline ('{base.get('platform_profile')}')")
    if info["epp"] not in ("performance", "?"):
        warnings.append(f"energy performance preference is '{info['epp']}'")
    if info["perf_event_paranoid"] not in ("-1", "0", "1"):
        warnings.append("perf_event_paranoid > 1: `perf.py profile` will not work")
    if not quiet or problems or warnings:
        print(f"envcheck: busy={busy:.1%} governor={info['governor']} boost={info['boost']} "
              f"ac={info['ac_online']} load={'/'.join(info['load_avg'])}")
        for w in warnings:
            print(f"  WARN: {w}")
        for p in problems:
            print(f"  FAIL: {p}")
    return not problems


def require_env(args):
    if getattr(args, "no_envcheck", False):
        print("envcheck skipped (--no-envcheck): results are not valid for acceptance")
        return
    if not envcheck(quiet=True):
        die("machine not in benchmarking state (see above); fix it or pass --no-envcheck "
            "for exploratory runs only")


# --------------------------------------------------------------------------------------------
# build / test / snapshot
# --------------------------------------------------------------------------------------------

def scons_args(config, tests, jobs):
    args = ["scons", f"-j{jobs}", *SCONS_COMMON, *BUILD_CONFIGS[config]]
    if tests == "skip":
        args.append("COMPILE_BOOST_TESTS=0")
    else:
        args.append("COMPILE_BOOST_TESTS=1")
        args.append("RUN_BOOST_TESTS=1" if tests == "run" else "RUN_BOOST_TESTS=0")
    return args


def run_scons(cwd, config, tests, jobs, log):
    args = scons_args(config, tests, jobs)
    print(f"building '{config}' in {cwd} (log: {log}) ...", flush=True)
    start = time.time()
    with open(log, "w") as f:
        result = subprocess.run(args, cwd=cwd, stdout=f, stderr=subprocess.STDOUT)
    elapsed = time.time() - start
    if result.returncode != 0:
        tail = read_file(log).splitlines()[-40:]
        print("\n".join(tail))
        die(f"build failed after {elapsed:.0f}s (exit {result.returncode}), see {log}")
    # [build/include_order] is ignored: the repository's include convention always triggers it
    warnings = [line for line in read_file(log).splitlines()
                if "warning:" in line and "[build/include_order]" not in line
                and ("combigrid/src" in line or "benchmarks/perf" in line)]
    print(f"build '{config}' finished in {elapsed:.0f}s; {len(warnings)} compiler/style warnings "
          f"in combigrid sources or perf benchmarks")
    for line in warnings[:20]:
        print(f"  {line}")
    return args


def cmd_build(args):
    WORK.mkdir(parents=True, exist_ok=True)
    (WORK / "logs").mkdir(exist_ok=True)
    log = WORK / "logs" / f"build-{args.config}-{now_tag()}.log"
    scons = run_scons(REPO, args.config, args.tests, args.jobs, log)
    STAMP.write_text(json.dumps({"config": args.config, "scons": scons, "time": time.time(),
                                 "git": git_state()}, indent=1))


def current_build(require_config=None):
    """Returns (bin_dir_map, lib_dir, stamp) of the current in-tree build."""
    if not STAMP.exists():
        die("no build stamp: build with `perf.py build normal` first")
    stamp = json.loads(STAMP.read_text())
    lib = REPO / "lib" / "libsgppcombigrid.so"
    if lib.exists() and lib.stat().st_mtime > stamp["time"] + 1:
        die("lib/ was rebuilt outside of perf.py after the last `perf.py build`; "
            "run `perf.py build <config>` again")
    if require_config and stamp["config"] != require_config:
        die(f"current build is '{stamp['config']}', expected '{require_config}' "
            f"(run `perf.py build {require_config}`)")
    bins = {suite: BENCH_SRC / exe for suite, exe in EXES.items()}
    return bins, REPO / "lib", stamp


def snapshot_paths(name):
    if name in (None, "current"):
        bins, lib, _ = current_build(require_config="normal")
        return bins, lib
    root = WORK / "snapshots" / name
    if not root.exists():
        die(f"snapshot '{name}' does not exist (create it with `perf.py snapshot {name}`)")
    return {suite: root / "bin" / exe for suite, exe in EXES.items()}, root / "lib"


def build_info(exe, lib):
    env = dict(os.environ, LD_LIBRARY_PATH=str(lib))
    result = subprocess.run([str(exe), "--build-info"], env=env, capture_output=True, text=True)
    return json.loads(result.stdout) if result.returncode == 0 else None


def copy_build(src_repo, dest, meta):
    if dest.exists():
        shutil.rmtree(dest)
    (dest / "lib").mkdir(parents=True)
    (dest / "bin").mkdir()
    for lib in LIBS:
        shutil.copy2(src_repo / "lib" / lib, dest / "lib" / lib)
    src_bench = src_repo / "combigrid" / "examples" / "c++" / "benchmarks" / "perf"
    for exe in EXES.values():
        if (src_bench / exe).exists():
            shutil.copy2(src_bench / exe, dest / "bin" / exe)
    meta["build_info"] = build_info(dest / "bin" / EXES["e2e"], dest / "lib")
    (dest / "meta.json").write_text(json.dumps(meta, indent=1))


def cmd_snapshot(args):
    dest = WORK / "snapshots" / args.name
    if dest.exists() and not args.force:
        die(f"snapshot '{args.name}' exists (use --force to replace it)")
    if args.commit:
        worktree = WORK / "worktrees" / args.name
        if worktree.exists():
            git("worktree", "remove", "--force", str(worktree), check=False)
            shutil.rmtree(worktree, ignore_errors=True)
        worktree.parent.mkdir(parents=True, exist_ok=True)
        rev = git("rev-parse", args.commit).strip()
        git("worktree", "add", "--detach", str(worktree), rev)
        try:
            (WORK / "logs").mkdir(parents=True, exist_ok=True)
            log = WORK / "logs" / f"build-snapshot-{args.name}-{now_tag()}.log"
            run_scons(worktree, "normal", "skip", args.jobs, log)
            copy_build(worktree, dest, {"name": args.name, "source": "commit", "rev": rev,
                                        "time": time.time()})
        finally:
            if not args.keep_worktree:
                git("worktree", "remove", "--force", str(worktree), check=False)
    else:
        _, _, stamp = current_build(require_config="normal")
        copy_build(REPO, dest, {"name": args.name, "source": "working tree", "time": time.time(),
                                "stamp": stamp, "git": git_state()})
    meta = json.loads((dest / "meta.json").read_text())
    info = meta.get("build_info") or {}
    print(f"snapshot '{args.name}' saved in {dest}")
    print(f"  build info: {json.dumps(info)}")
    if not (info.get("ndebug") and info.get("optimize") and info.get("use_eigen")):
        print("  WARN: snapshot is not in the judged configuration")


def cmd_test(args):
    _, lib, stamp = current_build()
    env = dict(os.environ, LD_LIBRARY_PATH=str(lib))
    failures = 0
    for i in range(args.repeat):
        cmd = [str(TEST_EXE)] + (["--run_test=" + args.run_test] if args.run_test else [])
        result = subprocess.run(cmd, env=env, capture_output=True, text=True)
        out = (result.stdout + result.stderr).strip().splitlines()
        summary = [line for line in out if "error" in line.lower() or "No errors" in line]
        print(f"[{stamp['config']}] run {i + 1}/{args.repeat}: exit {result.returncode}; " +
              (summary[-1] if summary else (out[-1] if out else "")))
        if result.returncode != 0:
            failures += 1
            print("\n".join(out[-40:]))
    if failures:
        die(f"{failures} of {args.repeat} test runs failed")


# --------------------------------------------------------------------------------------------
# running benchmarks
# --------------------------------------------------------------------------------------------

def run_exe(exe, lib, threads, extra, timeout=None):
    env = omp_env(threads)
    env["LD_LIBRARY_PATH"] = str(lib)
    cmd = [str(exe), "--json", *extra]
    result = subprocess.run(cmd, env=env, capture_output=True, text=True, timeout=timeout)
    if result.returncode != 0:
        die(f"{exe.name} {' '.join(extra)} failed (exit {result.returncode}):\n{result.stderr}")
    return [json.loads(line) for line in result.stdout.splitlines() if line.startswith("{")]


def list_cases(exe, lib, pattern):
    env = dict(os.environ, LD_LIBRARY_PATH=str(lib))
    out = subprocess.run([str(exe), "--list"], env=env, capture_output=True, text=True).stdout
    cases = [c for c in out.splitlines() if c.strip()]
    if pattern:
        rx = re.compile(pattern)
        cases = [c for c in cases if rx.search(c)]
    return cases


def suite_args(suite, args):
    d = DEFAULTS[suite]
    threads = args.threads or d["threads"]
    samples = args.samples or d["samples"]
    warmups = d["warmups"] if args.warmups is None else args.warmups
    extra = ["--samples", str(samples), "--warmups", str(warmups)]
    if suite == "micro":
        extra += ["--min-sample-time", str(args.min_sample_time)]
    return threads, extra


def fmt_time(seconds):
    if seconds is None or math.isnan(seconds):
        return "-"
    if seconds >= 1:
        return f"{seconds:.3f}s"
    if seconds >= 1e-3:
        return f"{seconds * 1e3:.2f}ms"
    if seconds >= 1e-6:
        return f"{seconds * 1e6:.2f}us"
    return f"{seconds * 1e9:.1f}ns"


def cmd_run(args):
    require_env(args)
    bins, lib = snapshot_paths(args.snapshot)
    threads, extra = suite_args(args.suite, args)
    if args.filter:
        extra += ["--filter", args.filter]
    meter = ForeignLoad()
    with BenchLock():
        records = run_exe(bins[args.suite], lib, threads, extra)
    foreign = meter.fraction()
    rows = []
    for rec in records:
        metrics = rec["metrics"]
        med = {k: statistics.median(v) for k, v in metrics.items()}
        if rec["kind"] == "micro":
            per_item = med["call"] / rec.get("items", 1.0)
            rows.append(f"| {rec['case']} | {fmt_time(med['call'])} | {per_item * 1e9:.2f} ns |")
        else:
            rel = abs(rec["result"] - rec["exact"]) / abs(rec["exact"]) if rec.get("exact") else float("nan")
            rows.append(f"| {rec['case']} | {fmt_time(med['total'])} | "
                        f"{fmt_time(med.get('generation'))} | {fmt_time(med.get('operator'))} | "
                        f"{rel:.2e} |")
    if args.suite == "micro":
        print("| case | time/call | time/item |\n|---|---|---|")
    else:
        print(f"| case ({threads} threads) | total | generation | operator | rel. error |\n"
              "|---|---|---|---|---|")
    print("\n".join(rows))
    print(f"\nforeign CPU load during the run: {foreign:.1%}" +
          (" - WARN: distorted, repeat" if foreign > ForeignLoad.THRESHOLD else ""))
    save = Path(args.save) if args.save else WORK / "runs" / f"{now_tag()}-{args.suite}.json"
    save.parent.mkdir(parents=True, exist_ok=True)
    save.write_text(json.dumps({"suite": args.suite, "snapshot": args.snapshot, "threads": threads,
                                "environment": environment_info(), "records": records}, indent=1))
    print(f"\nraw results: {save}")


# --------------------------------------------------------------------------------------------
# A/B comparison
# --------------------------------------------------------------------------------------------

def t_quantile(df):
    keys = sorted(T_QUANTILE)
    for k in keys:
        if df <= k:
            return T_QUANTILE[k]
    return 1.96


def per_round_logs(rounds_a, rounds_b):
    return [math.log(statistics.median(a) / statistics.median(b))
            for a, b in zip(rounds_a, rounds_b)]


def robust_std(values):
    """1.4826 * median absolute deviation: a std estimate that ignores single outliers (e.g. a
    round at the onset of throttling); used for the noise model."""
    if len(values) < 2:
        return 0.0
    center = statistics.median(values)
    return 1.4826 * statistics.median(abs(v - center) for v in values)


def noise_model_from(data, suite):
    """Noise model (per case and metric: robust std of the per-round log speedups) from the raw
    data of a self-comparison (`ab ... baseline baseline`)."""
    model = {}
    for key, recs_a in data.items():
        side, case = key.split("|", 1)
        if side != "A" or ("B|" + case) not in data:
            continue
        recs_b = data["B|" + case]
        for metric in recs_a[0]["metrics"]:
            logs = per_round_logs([r["metrics"][metric] for r in recs_a],
                                  [r["metrics"][metric] for r in recs_b])
            model.setdefault(case, {})[metric] = robust_std(logs)
    return model


def paired_speedup(rounds_a, rounds_b, noise_std):
    """Speedup median(A)/median(B) from paired rounds: geometric mean of the per-round ratios and
    a 95 % t-interval of the mean log ratio whose spread is at least the noise model's.
    Returns (point, lo, hi, observed std of the log ratios)."""
    logs = per_round_logs(rounds_a, rounds_b)
    mean = statistics.fmean(logs)
    observed = statistics.stdev(logs) if len(logs) > 1 else 0.0
    spread = max(observed, noise_std, NOISE_FLOOR)
    half = t_quantile(max(len(logs) - 1, 1)) * spread / math.sqrt(len(logs))
    return math.exp(mean), math.exp(mean - half), math.exp(mean + half), observed


def verdict(lo, hi):
    if lo > 1.0:
        return "faster"
    if hi < 1.0:
        return "SLOWER"
    return "~ (noise)"


def cmd_ab(args):
    require_env(args)
    suite = args.suite
    bins_a, lib_a = snapshot_paths(args.a)
    bins_b, lib_b = snapshot_paths(args.b)
    threads, extra = suite_args(suite, args)
    cases_a = list_cases(bins_a[suite], lib_a, args.filter)
    cases_b = list_cases(bins_b[suite], lib_b, args.filter)
    common = [c for c in cases_a if c in cases_b]
    only_a = [c for c in cases_a if c not in cases_b]
    only_b = [c for c in cases_b if c not in cases_a]
    if not common and not only_b:
        die("no cases to compare")

    data = {}  # (side, case) -> list of records (one per round)
    start = time.time()
    foreign, durations = [], []
    meter = ForeignLoad()
    with BenchLock():
        for rnd in range(args.rounds):
            meter.start()
            for i, case in enumerate(common + only_b):
                sides = ["A", "B"] if (rnd + i) % 2 == 0 else ["B", "A"]
                if case in only_b:
                    sides = ["B"]
                for side in sides:
                    bins, lib = (bins_a, lib_a) if side == "A" else (bins_b, lib_b)
                    recs = run_exe(bins[suite], lib, threads,
                                   extra + ["--filter", "^" + re.escape(case) + "$"])
                    data.setdefault((side, case), []).append(recs[0])
            foreign.append(meter.fraction())
            durations.append(time.time() - meter.t0)
            print(f"  round {rnd + 1}/{args.rounds} done ({time.time() - start:.0f}s, "
                  f"foreign CPU load {foreign[-1]:.1%})", flush=True)

    metrics_shown = ["call"] if suite == "micro" else args.metrics.split(",")
    noise = json.loads(NOISE_FILES[suite].read_text()) if NOISE_FILES[suite].exists() else {}
    lines = [f"A/B `{suite}`: A=`{args.a}` B=`{args.b}`, {threads} thread(s), {args.rounds} rounds "
             f"(interleaved processes) x {extra[1]} sample(s), speedup = median(A)/median(B) "
             f"(> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the "
             f"spread bounded below by the noise model ({'baseline/' + NOISE_FILES[suite].name if noise else 'none: floor only'})",
             "",
             "| case | metric | A | B | speedup | 95% CI | verdict |",
             "|---|---|---|---|---|---|---|"]
    total_a = total_b = 0.0
    result_dev = []
    for case in common:
        recs_a, recs_b = data[("A", case)], data[("B", case)]
        for metric in metrics_shown:
            if metric not in recs_a[0]["metrics"] or metric not in recs_b[0]["metrics"]:
                continue
            sa = [r["metrics"][metric] for r in recs_a]
            sb = [r["metrics"][metric] for r in recs_b]
            point, lo, hi, _ = paired_speedup(sa, sb, noise.get(case, {}).get(metric, 0.0))
            med_a = statistics.median([x for r in sa for x in r])
            med_b = statistics.median([x for r in sb for x in r])
            if metric in ("total", "call"):
                total_a += med_a
                total_b += med_b
            lines.append(f"| {case} | {metric} | {fmt_time(med_a)} | {fmt_time(med_b)} | "
                         f"{point:.3f} | [{lo:.3f}, {hi:.3f}] | {verdict(lo, hi)} |")
        if suite == "e2e":
            ra, rb = recs_a[-1]["result"], recs_b[-1]["result"]
            ex = recs_a[-1]["exact"]
            if ra is not None and rb is not None and ex:
                result_dev.append((case, abs(ra - ex) / abs(ex), abs(rb - ex) / abs(ex)))
    for case in only_b:
        recs_b = data[("B", case)]
        key = "call" if suite == "micro" else "total"
        med_b = statistics.median([x for r in recs_b for x in r["metrics"][key]])
        lines.append(f"| {case} | {key} | (not in A) | {fmt_time(med_b)} | - | - | new case |")
    for case in only_a:
        lines.append(f"| {case} | - | present | (not in B) | - | - | removed case |")
    if total_b > 0:
        lines += ["", f"Sum of medians ({'call' if suite == 'micro' else 'total'}, common cases): "
                      f"A = {fmt_time(total_a)}, B = {fmt_time(total_b)}, saved = "
                      f"{fmt_time(total_a - total_b)} ({total_a / total_b:.3f}x)"]
    if result_dev:
        worst = max(result_dev, key=lambda t: t[2] / max(t[1], 1e-300))
        lines.append(f"Result sanity (rel. error vs exact): worst growth in `{worst[0]}`: "
                     f"A {worst[1]:.2e} -> B {worst[2]:.2e} (the binding check is `refcheck`)")
    lines.append("Foreign CPU load per round (other processes, share of all CPUs): " +
                 ", ".join(f"{f:.1%}" for f in foreign))
    lines.append("Round durations (s): " + ", ".join(f"{d:.0f}" for d in durations) +
                 " (rounds getting slower = the laptop throttles under sustained load; A and B are "
                 "affected alike)")
    if max(foreign) > ForeignLoad.THRESHOLD:
        lines.append(f"**WARN: other processes used > {ForeignLoad.THRESHOLD:.0%} of the CPUs in "
                     "some rounds; the comparison may be distorted - find the cause and repeat.**")
    lines.append(f"Duration: {time.time() - start:.0f}s. Environment: "
                 f"{json.dumps({k: v for k, v in environment_info().items() if k in ('governor', 'boost', 'ac_online')})}")
    report = "\n".join(lines)
    print("\n" + report)
    if args.noise_out:
        raw_data = {f"{side}|{case}": recs for (side, case), recs in data.items()}
        Path(args.noise_out).write_text(json.dumps(noise_model_from(raw_data, suite), indent=1,
                                                   sort_keys=True))
        print(f"noise model (robust std of the per-round log speedups) written to {args.noise_out}")

    save_dir = Path(args.save) if args.save else WORK / "ab"
    save_dir.mkdir(parents=True, exist_ok=True)
    stem = args.name or f"{now_tag()}-{suite}-{args.a}-vs-{args.b}"
    (save_dir / f"{stem}.md").write_text(report + "\n")
    raw = {f"{side}|{case}": recs for (side, case), recs in data.items()}
    (save_dir / f"{stem}.json").write_text(json.dumps(
        {"suite": suite, "a": args.a, "b": args.b, "threads": threads, "rounds": args.rounds,
         "environment": environment_info(), "data": raw}))
    print(f"\nsaved: {save_dir / (stem + '.md')} (+ .json)")


# Fixed workloads to detect changes of the machine state that envcheck cannot see (power limits,
# thermal state). The laptop's firmware limits sustained multi-core power differently between
# boots (observed in the setup session: identical settings, short bursts equally fast, sustained
# 16-thread load 1.6x slower), so the e2e workload runs for about a minute and its first and last
# samples are compared separately. The baseline command records the same procedure in
# baseline/calibration.json.
CALIBRATION = [("e2e", "quad/sg/equi-exp/b0/d5/L10", 16, "total", 120),
               ("micro", "micro/sol/quad_tg/equi/b0/d10/L4", 1, "call", 10)]
CALIBRATION_FILE = PERF / "baseline" / "calibration.json"
CALIBRATION_TOLERANCE = 0.07


def run_calibration():
    """Returns {case: {"burst": median of the first 5 samples, "sustained": median of the second
    half, "series": all samples}}."""
    bins, lib = snapshot_paths("baseline")
    result = {}
    for suite, case, threads, metric, samples in CALIBRATION:
        extra = ["--filter", "^" + re.escape(case) + "$", "--samples", str(samples),
                 "--warmups", "1" if suite == "e2e" else "0"]
        series = run_exe(bins[suite], lib, threads, extra)[0]["metrics"][metric]
        result[case] = {"threads": threads, "burst": statistics.median(series[:5]),
                        "sustained": statistics.median(series[len(series) // 2:]),
                        "series": series}
        print(f"calibrate: {case} ({threads} thread(s), {samples} samples): burst "
              f"{fmt_time(result[case]['burst'])}, sustained {fmt_time(result[case]['sustained'])}")
    return result


def cmd_calibrate(args):
    require_env(args)
    if not CALIBRATION_FILE.exists():
        die("no baseline/calibration.json")
    recorded = json.loads(CALIBRATION_FILE.read_text())
    meter = ForeignLoad()
    with BenchLock():
        now = run_calibration()
    worst = 0.0
    for case, values in now.items():
        for key in ("burst", "sustained"):
            ratio = values[key] / recorded[case][key]
            worst = max(worst, abs(ratio - 1.0))
            print(f"  {case} {key}: ratio to baseline {ratio:.3f}")
    foreign = meter.fraction()
    print(f"  foreign CPU load: {foreign:.1%}")
    if worst > CALIBRATION_TOLERANCE or foreign > ForeignLoad.THRESHOLD:
        print(f"  WARN: the machine runs the baseline binaries up to {worst:.0%} off the recorded "
              "baseline (power limit, thermal state, background load?). A/B comparisons stay "
              "valid (both sides run in the same state), but absolute numbers are not comparable "
              "with baseline/. State the ratios in the summary; tell the owner if sustained "
              "differs.")
    else:
        print("calibrate: OK (within 7 % of the recorded baseline)")


# --------------------------------------------------------------------------------------------
# reference check
# --------------------------------------------------------------------------------------------

def load_jsonl(path):
    return {r["case"]: r for r in (json.loads(l) for l in Path(path).read_text().splitlines() if l.strip())}


def compare_reference(ref, new, tol):
    """Returns a list of (status, case, message). status in OK, INFO, WARN, FAIL."""
    eps = 2.0 ** -52
    out = []
    for case, r in ref.items():
        n = new.get(case)
        if n is None:
            out.append(("FAIL", case, "case missing in new output"))
            continue
        over = tol.get("overrides", {}).get(case, {})
        ok_ratio = over.get("ok_ratio", tol["ok_ratio"])
        fail_ratio = over.get("fail_ratio", tol["fail_ratio"])
        noise_units = over.get("noise_units", tol["noise_units"])
        status, messages = "OK", []

        for key, value in r["ints"].items():
            if n["ints"].get(key) != value:
                status = "FAIL"
                messages.append(f"{key}: {value} -> {n['ints'].get(key)} (structure changed)")
        for key, value in r["floats"].items():
            nv = n["floats"].get(key)
            if nv is None or abs(nv - value) > tol["float_rel_tol"] * max(abs(value), 1e-300):
                status = "FAIL"
                messages.append(f"{key}: {value!r} -> {nv!r}")

        # Round-off unit of the combination; reassociation (thread count, summation order) moves
        # the results by up to ~1.5 units (measured in the setup session)
        unit = eps * r["magnitude"] * (r["sumAbsCoeffs"] + math.sqrt(r["sumGP"]))
        noise = noise_units * unit
        worst_level, worst_ratio, worst_errors = 0, 0.0, (0.0, 0.0, -1)
        if len(n["values"]) != len(r["values"]):
            status = "FAIL"
            messages.append("number of values changed")
        else:
            for i, (rv, nv, ex) in enumerate(zip(r["values"], n["values"], r["exact"])):
                e_ref = abs(rv - ex)
                e_new = float("inf") if nv is None or math.isnan(nv) else abs(nv - ex)
                level = 0 if e_new <= ok_ratio * e_ref + noise else (
                    1 if e_new <= fail_ratio * e_ref + noise else 2)
                ratio = e_new / max(e_ref, noise)
                if (level, ratio) > (worst_level, worst_ratio):
                    worst_level, worst_ratio, worst_errors = level, ratio, (e_ref, e_new, i)
            if worst_level == 2:
                status = "FAIL"
            elif worst_level == 1 and status == "OK":
                status = "WARN"
            if worst_level > 0:
                e_ref, e_new, i = worst_errors
                messages.append(
                    f"error grew at value {i}: |ref-exact|={e_ref:.3e} |new-exact|={e_new:.3e} "
                    f"(x{worst_ratio:.2f}, noise allowance {noise:.1e})")
        for key, value in r.get("info", {}).items():
            if n.get("info", {}).get(key) != value:
                if status == "OK":
                    status = "INFO"
                messages.append(f"{key}: {value:g} -> {n.get('info', {}).get(key)}")
        out.append((status, case, "; ".join(messages) if messages else
                    f"max error ratio {worst_ratio:.3f}"))
    for case in new:
        if case not in ref:
            out.append(("INFO", case, "new case (not in reference)"))
    return out


def cmd_refcheck(args):
    tol = json.loads(TOLERANCES.read_text())
    ref = load_jsonl(args.reference or REFERENCE)
    bins, lib = snapshot_paths(args.snapshot)
    worst = "OK"
    order = ["OK", "INFO", "WARN", "FAIL"]
    report = []
    for threads in [int(t) for t in args.threads.split(",")]:
        env = omp_env(threads)
        env["LD_LIBRARY_PATH"] = str(lib)
        extra = ["--filter", args.filter] if args.filter else []
        result = subprocess.run([str(bins["reference"]), *extra], env=env, capture_output=True,
                                text=True)
        if result.returncode != 0:
            die(f"perf_reference failed: {result.stderr}")
        new = {r["case"]: r for r in (json.loads(l) for l in result.stdout.splitlines() if l)}
        ref_sel = {k: v for k, v in ref.items() if not args.filter or re.search(args.filter, k)}
        results = compare_reference(ref_sel, new, tol)
        counts = {s: sum(1 for r in results if r[0] == s) for s in order}
        report.append(f"refcheck ({threads} thread(s)): " +
                      ", ".join(f"{counts[s]} {s}" for s in order))
        for status, case, msg in results:
            if status != "OK" or args.verbose:
                report.append(f"  {status:4} {case}: {msg}")
            if order.index(status) > order.index(worst):
                worst = status
    text = "\n".join(report)
    print(text)
    if args.save:
        Path(args.save).parent.mkdir(parents=True, exist_ok=True)
        Path(args.save).write_text(text + "\n")
    print(f"\nrefcheck result: {worst}"
          + (" (WARN: errors grew slightly - must be stated in the summary)" if worst == "WARN" else "")
          + (" (INFO: e.g. evaluation counts changed - state it in the summary)" if worst == "INFO" else ""))
    if worst == "FAIL":
        sys.exit(1)


# --------------------------------------------------------------------------------------------
# profiling
# --------------------------------------------------------------------------------------------

def cmd_profile(args):
    require_env(args)
    bins, lib = snapshot_paths(args.snapshot)
    threads = args.threads or DEFAULTS[args.suite]["threads"]
    out_dir = WORK / "profiles"
    out_dir.mkdir(parents=True, exist_ok=True)
    stem = f"{now_tag()}-{re.sub(r'[^A-Za-z0-9]+', '_', args.case)}-t{threads}"
    data = out_dir / f"{stem}.data"
    env = omp_env(threads)
    env["LD_LIBRARY_PATH"] = str(lib)
    cmd = ["perf", "record", "-F", str(args.freq), "-g", "-o", str(data), "--",
           str(bins[args.suite]), "--filter", args.case, "--samples", str(args.samples),
           "--warmups", "1"]
    with BenchLock():
        result = subprocess.run(cmd, env=env, capture_output=True, text=True)
    if result.returncode != 0:
        die(f"perf record failed: {result.stderr[-2000:]}")
    report_cmd = ["perf", "report", "-i", str(data), "--stdio", "--percent-limit",
                  str(args.percent_limit), "--no-children" if not args.children else "--children",
                  "-g", "none"]
    report = subprocess.run(report_cmd, capture_output=True, text=True).stdout
    lines = [l for l in report.splitlines() if re.match(r"^\s+[0-9.]+%", l)]
    # drop parameter lists, keep "(anonymous namespace)"
    lines = [re.sub(r"\(.*", "", l.replace("(anonymous namespace)", "{anon}"))
             for l in lines][: args.top]
    text = (f"profile of `{args.case}` ({args.suite}, {threads} threads, "
            f"{'children' if args.children else 'self'} time)\n" + "\n".join(lines))
    print(text)
    (out_dir / f"{stem}.txt").write_text(text + "\n")
    print(f"\nperf data: {data} (inspect with `perf report -i ...` / `perf annotate -i ...`)")


# --------------------------------------------------------------------------------------------
# protection
# --------------------------------------------------------------------------------------------

def protected_files():
    files = set()
    for d in PROTECTED_DIRS:
        for p in (REPO / d).rglob("*"):
            if p.is_file():
                files.add(p.relative_to(REPO).as_posix())
    for pattern in PROTECTED_GLOBS:
        for p in REPO.glob(pattern):
            if p.is_file():
                files.add(p.relative_to(REPO).as_posix())
    return sorted(files)


def is_protected(rel):
    rel = rel.replace("\\", "/")
    if any(rel.startswith(d) for d in PROTECTED_DIRS):
        return True
    return any(fnmatch.fnmatch(rel, g) for g in PROTECTED_GLOBS) or rel == MANIFEST.relative_to(REPO).as_posix()


def cmd_verify(args):
    if not MANIFEST.exists():
        die("no manifest (combigrid/perf/protected.sha256)")
    expected = {}
    for line in MANIFEST.read_text().splitlines():
        if line.startswith("#") or not line.strip():
            continue
        digest, rel = line.split(None, 1)
        expected[rel.strip()] = digest
    problems = []
    for rel, digest in expected.items():
        path = REPO / rel
        if not path.exists():
            problems.append(f"missing: {rel}")
        elif sha256(path) != digest:
            problems.append(f"modified: {rel}")
    for rel in protected_files():
        if rel not in expected:
            problems.append(f"new file in protected location: {rel}")
    status = git("status", "--porcelain", "--", *PROTECTED_DIRS, check=False)
    for line in status.splitlines():
        rel = line[3:]
        if f"modified: {rel}" not in problems and f"new file in protected location: {rel}" not in problems:
            problems.append(f"git: {line.strip()}")
    if problems:
        print("PROTECTED FILES CHANGED:")
        for p in problems:
            print(f"  {p}")
        die("protected files were changed; revert them (they may only change on explicit "
            "instruction of the project owner)")
    print(f"verify: {len(expected)} protected files unchanged")


def cmd_protect(args):
    if not args.authorized_by_owner:
        die("rehashing the manifest legitimizes changes to protected files; pass "
            "--authorized-by-owner only on explicit instruction of the project owner")
    lines = ["# sha256 manifest of the protected files of the combigrid performance suite.",
             "# Generated by `perf.py protect`; checked by `perf.py verify` and by the",
             "# .claude/hooks/protect_perf.py hook. Do not edit by hand.",
             *[f"#dir {d}" for d in PROTECTED_DIRS],
             *[f"#glob {g}" for g in PROTECTED_GLOBS]]
    for rel in protected_files():
        lines.append(f"{sha256(REPO / rel)}  {rel}")
    MANIFEST.write_text("\n".join(lines) + "\n")
    print(f"manifest written: {len(protected_files())} files")


def cmd_reject(args):
    keep = REJECT_KEEP + (args.keep or [])
    status = git("status", "--porcelain", "--untracked-files=all")
    restore, delete, untouched = [], [], []
    for line in status.splitlines():
        code, rel = line[:2], line[3:]
        if " -> " in rel:
            rel = rel.split(" -> ")[1]
        if any(rel.startswith(k) for k in keep):
            continue
        if code != "??":
            restore.append(rel)
        elif rel.startswith(REJECT_DELETE_UNDER):
            delete.append(rel)
        else:
            untouched.append(rel)
    for rel in untouched:
        print(f"left alone (untracked outside {REJECT_DELETE_UNDER}): {rel}")
    if not restore and not delete:
        print("nothing to revert")
        return
    print("would restore to HEAD:" if not args.apply else "restoring to HEAD:")
    for rel in restore:
        print(f"  {rel}")
    print("would delete (untracked):" if not args.apply else "deleting (untracked):")
    for rel in delete:
        print(f"  {rel}")
    print(f"kept: {', '.join(keep)}")
    if not args.apply:
        print("\ndry run; pass --apply to revert")
        return
    if restore:
        git("restore", "--source=HEAD", "--staged", "--worktree", "--", *restore)
    for rel in delete:
        path = REPO / rel
        if path.is_file() or path.is_symlink():
            path.unlink()
    print("done")


# --------------------------------------------------------------------------------------------
# baseline
# --------------------------------------------------------------------------------------------

def cmd_baseline(args):
    base = PERF / "baseline"
    if (base / "e2e_t16.jsonl").exists() and not args.authorized_by_owner:
        die("the baseline exists and is protected")
    require_env(args)
    bins, lib = snapshot_paths(args.snapshot)
    base.mkdir(parents=True, exist_ok=True)
    meta = json.loads((WORK / "snapshots" / args.snapshot / "meta.json").read_text()) \
        if args.snapshot not in (None, "current") else {}
    (base / "environment.json").write_text(json.dumps(
        {"environment": environment_info(), "snapshot": meta, "recorded": time.time(),
         "omp_binding": {t: {k: omp_env(t)[k] for k in ("OMP_PROC_BIND", "OMP_PLACES")}
                         for t in (1, 16)}}, indent=1))
    # Calibration first, after an idle period (like the start of a session, where `calibrate`
    # runs): its burst samples must not inherit a depleted power budget
    print(f"idle for {args.settle}s before the calibration ...", flush=True)
    time.sleep(args.settle)
    with BenchLock():
        CALIBRATION_FILE.write_text(json.dumps(run_calibration(), indent=1))
    loads = {}
    with BenchLock():
        # micro (1 thread, low power) before the long 16-thread e2e run, which leaves the laptop
        # throttled for minutes
        for suite, threads, samples, warmups in (("micro", 1, 5, 0), ("e2e", 16, 5, 1)):
            meter = ForeignLoad()
            extra = ["--samples", str(samples), "--warmups", str(warmups)]
            if suite == "micro":
                extra += ["--min-sample-time", "0.05"]
            print(f"recording {suite} ({threads} threads) ...", flush=True)
            recs = run_exe(bins[suite], lib, threads, extra)
            loads[suite] = meter.fraction()
            print(f"  foreign CPU load: {loads[suite]:.1%}")
            with open(base / f"{suite}_t{threads}.jsonl", "w") as f:
                for r in recs:
                    f.write(json.dumps(r) + "\n")
    env_file = base / "environment.json"
    env = json.loads(env_file.read_text())
    env["foreign_cpu_load"] = loads
    env_file.write_text(json.dumps(env, indent=1))
    if max(loads.values()) > ForeignLoad.THRESHOLD:
        print("WARN: foreign CPU load above the threshold - re-record the baseline")
    print(f"baseline written to {base}")


# --------------------------------------------------------------------------------------------
# main
# --------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    p = sub.add_parser("envcheck", help="check benchmarking state of the machine")
    p.set_defaults(func=lambda a: sys.exit(0 if envcheck() else 1))

    p = sub.add_parser("build", help="build base + combigrid in a fixed configuration")
    p.add_argument("config", choices=list(BUILD_CONFIGS))
    p.add_argument("--tests", choices=["compile", "run", "skip"], default="compile",
                   help="compile (default), compile and run via SCons, or skip the Boost tests")
    p.add_argument("--jobs", type=int, default=16)
    p.set_defaults(func=cmd_build)

    p = sub.add_parser("test", help="run the combigrid Boost tests of the current build")
    p.add_argument("--repeat", type=int, default=1, help="repetitions (tests use time seeds)")
    p.add_argument("--run-test", help="Boost.Test filter, e.g. 'equidistant_nodes/*'")
    p.set_defaults(func=cmd_test)

    p = sub.add_parser("snapshot", help="save the current build or the build of a commit")
    p.add_argument("name")
    p.add_argument("--commit", help="build this commit in a temporary git worktree instead")
    p.add_argument("--force", action="store_true")
    p.add_argument("--keep-worktree", action="store_true")
    p.add_argument("--jobs", type=int, default=16)
    p.set_defaults(func=cmd_snapshot)

    for name, func in (("run", cmd_run), ("ab", cmd_ab)):
        p = sub.add_parser(name, help="run a suite" if name == "run" else "interleaved A/B comparison")
        p.add_argument("suite", choices=["e2e", "micro"])
        if name == "run":
            p.add_argument("--snapshot", default="current", help="snapshot name or 'current'")
            p.add_argument("--save")
        else:
            p.add_argument("a", help="snapshot A (before), or 'current'")
            p.add_argument("b", help="snapshot B (after), or 'current'")
            p.add_argument("--rounds", type=int, default=None)
            p.add_argument("--metrics", default="total,operator,generation",
                           help="pipeline metrics to report (default: total,operator,generation)")
            p.add_argument("--save", help="directory for the report (e.g. combigrid/perf/results/WP-03)")
            p.add_argument("--name", help="file stem of the report")
            p.add_argument("--noise-out", help="write the per-case std of the per-round log "
                           "speedups (self-comparison -> noise model, setup only)")
        p.add_argument("--filter", help="regex on case names")
        p.add_argument("--threads", type=int)
        p.add_argument("--samples", type=int)
        p.add_argument("--warmups", type=int)
        p.add_argument("--min-sample-time", type=float, default=0.05)
        p.add_argument("--no-envcheck", action="store_true")
        p.set_defaults(func=func)

    p = sub.add_parser("calibrate", help="run fixed baseline workloads, compare with baseline/")
    p.add_argument("--no-envcheck", action="store_true")
    p.set_defaults(func=cmd_calibrate)

    p = sub.add_parser("refcheck", help="check reference outputs against the protected reference")
    p.add_argument("--snapshot", default="current")
    p.add_argument("--threads", default="1,16")
    p.add_argument("--filter")
    p.add_argument("--reference", help="alternative reference file (setup/validation only)")
    p.add_argument("--save")
    p.add_argument("--verbose", action="store_true")
    p.set_defaults(func=cmd_refcheck)

    p = sub.add_parser("profile", help="perf record one case and print the hottest functions")
    p.add_argument("case", help="regex selecting exactly one case")
    p.add_argument("--suite", choices=["e2e", "micro"], default="e2e")
    p.add_argument("--snapshot", default="current")
    p.add_argument("--threads", type=int)
    p.add_argument("--samples", type=int, default=2)
    p.add_argument("--freq", type=int, default=999)
    p.add_argument("--top", type=int, default=30)
    p.add_argument("--percent-limit", type=float, default=0.5)
    p.add_argument("--children", action="store_true", help="inclusive instead of self time")
    p.add_argument("--no-envcheck", action="store_true")
    p.set_defaults(func=cmd_profile)

    p = sub.add_parser("verify", help="check that no protected file was modified")
    p.set_defaults(func=cmd_verify)

    p = sub.add_parser("reject", help="revert a rejected attempt (dry run unless --apply)")
    p.add_argument("--apply", action="store_true")
    p.add_argument("--keep", nargs="*", help="additional paths to keep")
    p.set_defaults(func=cmd_reject)

    p = sub.add_parser("baseline", help="record the protected baseline (setup session only)")
    p.add_argument("--snapshot", default="baseline")
    p.add_argument("--authorized-by-owner", action="store_true")
    p.add_argument("--settle", type=int, default=120, help="idle seconds before the calibration")
    p.add_argument("--no-envcheck", action="store_true")
    p.set_defaults(func=cmd_baseline)

    p = sub.add_parser("protect", help="rehash the protected-file manifest")
    p.add_argument("--authorized-by-owner", action="store_true")
    p.set_defaults(func=cmd_protect)

    args = parser.parse_args()
    if getattr(args, "suite", None) in DEFAULTS and hasattr(args, "rounds") and args.rounds is None:
        args.rounds = DEFAULTS[args.suite]["rounds"]
    args.func(args)


if __name__ == "__main__":
    main()
