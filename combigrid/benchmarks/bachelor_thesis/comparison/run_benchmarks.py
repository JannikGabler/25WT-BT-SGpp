#!/usr/bin/env python3
"""Runs a compiled comparison benchmark for several dimensions and increasing levels.

For every number of dimensions d the level starts at START_LEVEL and grows by one after each
successful level. A level consists of three executions of the benchmark binary:

  1. probe:   one round (no warmup) inside a cgroup with the memory limit, killed after the
              maximum duration; /usr/bin/time -v records the peak memory. If it is killed
              (timeout, out of memory) or fails, the script continues with the next d.
  2. runtime: the actual measurement; the binary chooses its warmup / round counts from the
              MIN_* settings itself. No time limit; only the memory limit (if enabled).
  3. perf:    one round under `perf stat` (CPU utilization, page faults, ...).

The binary is called as
  <binary> <min_warmup_rounds> <min_rounds> <min_warmup_time> <min_round_time> <nDimensions> <level>
with times in seconds.

Usage:
  ./run_benchmarks.py <binary> <max duration per round> <max memory> <dimensions> [-o <dir>]
  ./run_benchmarks.py ./uq_electron_beam 30min "8 GiB" "[2, 5, 10]"

Output directory (default: <binary dir>/results/<binary name>_<timestamp>/):
  results.csv    one row per (d, level): status, peak memory, runtime statistics, perf metrics
  results.jsonl  the same plus all checkpoints and perf events (one JSON object per line)
  meta.json      arguments, configuration, binary checksum, machine information
  benchmark.log  everything printed to the console plus the complete output of every execution
  raw/           unprocessed output files of every execution

Only the Python standard library is used. While a benchmark runs, the script blocks in wait()
and a single thread wakes up once per second to redraw the progress bar.
"""

import argparse
import csv
import dataclasses
import datetime
import hashlib
import json
import os
import platform
import re
import shutil
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path

# ---------------------------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------------------------

MIN_WARMUP_ROUNDS = 1
MIN_ROUNDS = 2
MIN_WARMUP_TIME = 15.0  # seconds
MIN_ROUND_TIME = 45.0  # seconds

START_LEVEL = 3
MAX_LEVEL = None  # None: increase until a limit is hit

RUN_PERF_STAT = True
# Applies the memory limit to the runtime execution too (no measurable effect on the runtime;
# protects the machine if the probe was not representative)
LIMIT_MEMORY_OF_RUNTIME_RUN = True
# The perf execution may take longer than the probe; its time limit is this factor times the max.
# duration (a failed perf execution is recorded but does not end the dimension)
PERF_TIMEOUT_FACTOR = 1.1
# Exported to the benchmark when set (e.g. 8 to restrict OpenMP to 8 threads)
OMP_NUM_THREADS = None

PROGRESS_REFRESH_SECONDS = 10.0

TIME_BINARY = "/usr/bin/time"

# ---------------------------------------------------------------------------------------------
# Argument parsing
# ---------------------------------------------------------------------------------------------

DURATION_UNITS = {
    "ms": 1e-3, "s": 1, "sec": 1, "m": 60, "min": 60, "h": 3600, "hr": 3600, "d": 86400,
}
MEMORY_UNITS = {
    "": 1, "b": 1,
    "kb": 1000, "mb": 1000**2, "gb": 1000**3, "tb": 1000**4,
    "k": 1024, "m": 1024**2, "g": 1024**3, "t": 1024**4,  # like systemd
    "kib": 1024, "mib": 1024**2, "gib": 1024**3, "tib": 1024**4,
}


def parse_duration(text):
    """'30min', '1h30min', '90s', '45' (seconds) -> seconds."""
    text = text.strip().lower().replace(" ", "")
    if re.fullmatch(r"[\d.]+", text):
        return float(text)
    parts = re.findall(r"([\d.]+)([a-z]+)", text)
    if not parts or "".join(n + u for n, u in parts) != text:
        raise argparse.ArgumentTypeError(f"invalid duration '{text}' (e.g. 30min, 1h, 90s)")
    seconds = 0.0
    for number, unit in parts:
        if unit not in DURATION_UNITS:
            raise argparse.ArgumentTypeError(f"unknown time unit '{unit}' in '{text}'")
        seconds += float(number) * DURATION_UNITS[unit]
    return seconds


def parse_memory(text):
    """'8 GiB' (2^30), '8GB' (10^9), '8G' (2^30, like systemd), '512MiB' -> bytes."""
    match = re.fullmatch(r"\s*([\d.]+)\s*([a-zA-Z]*)\s*", text)
    if not match or match.group(2).lower() not in MEMORY_UNITS:
        raise argparse.ArgumentTypeError(f"invalid memory size '{text}' (e.g. 8GiB, 8GB, 512M)")
    return int(float(match.group(1)) * MEMORY_UNITS[match.group(2).lower()])


def parse_dimensions(tokens):
    """'[2, 5, 10]', '2,5,10' or '2 5 10' (possibly split by the shell) -> [2, 5, 10]."""
    text = " ".join(tokens)
    if not re.fullmatch(r"[\s\[\]\d,;]*", text):
        raise argparse.ArgumentTypeError(f"invalid dimension list '{text}' (e.g. \"[2, 5, 10]\")")
    dimensions = [int(token) for token in re.findall(r"\d+", text)]
    if not dimensions or 0 in dimensions:
        raise argparse.ArgumentTypeError(f"invalid dimension list '{text}'")
    return dimensions


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Benchmarks a comparison binary for increasing levels per dimension.")
    parser.add_argument("binary", type=Path, help="compiled benchmark binary")
    parser.add_argument("max_duration", type=parse_duration,
                        help="time limit of one round, e.g. 30min, 1h, 90s")
    parser.add_argument("max_memory", type=parse_memory, help="memory limit, e.g. '8 GiB', 8GB")
    parser.add_argument("dimensions", nargs="+", help="dimensions, e.g. \"[2, 5, 10]\"")
    parser.add_argument("-o", "--output-dir", type=Path, help="directory for the results")
    args = parser.parse_args()
    try:
        args.dimensions = parse_dimensions(args.dimensions)
    except argparse.ArgumentTypeError as error:
        parser.error(str(error))
    args.binary = args.binary.resolve()
    if not (args.binary.is_file() and os.access(args.binary, os.X_OK)):
        parser.error(f"'{args.binary}' is not an executable file")
    return args


# ---------------------------------------------------------------------------------------------
# Formatting
# ---------------------------------------------------------------------------------------------


def format_duration(seconds):
    if seconds is None:
        return "-"
    if seconds < 60:
        return f"{seconds:.1f}s"
    minutes, secs = divmod(int(round(seconds)), 60)
    hours, minutes = divmod(minutes, 60)
    return f"{hours}:{minutes:02d}:{secs:02d}" if hours else f"{minutes}:{secs:02d}"


def format_bytes(count):
    if count is None:
        return "-"
    for unit, factor in (("GiB", 1024**3), ("MiB", 1024**2), ("KiB", 1024)):
        if count >= factor:
            return f"{count / factor:.2f} {unit}"
    return f"{count} B"


def format_number(value):
    """Number as passed to the binary: 15.0 -> '15', 0.5 -> '0.5'."""
    return f"{value:g}"


# ---------------------------------------------------------------------------------------------
# Console: terminal output with a progress bar at the bottom, everything mirrored to the log
# ---------------------------------------------------------------------------------------------


@dataclasses.dataclass
class Progress:
    dimensions: list
    dimension_index: int = 0
    level: int = None
    phase: str = "starting"
    phase_start: float = dataclasses.field(default_factory=time.monotonic)
    last_level_text: str = "-"  # duration of the previous level
    finished: bool = False


class Console:
    BAR_WIDTH = 12

    def __init__(self, log_path, progress):
        self.log = open(log_path, "a", encoding="utf-8")
        self.progress = progress
        self.lock = threading.Lock()
        self.show_bar = sys.stdout.isatty()
        self.stop = threading.Event()
        self.thread = threading.Thread(target=self._refresh_loop, daemon=True)
        if self.show_bar:
            self.thread.start()

    def info(self, message=""):
        """Prints to terminal and log."""
        with self.lock:
            self._clear_bar()
            print(message, flush=True)
            self._write_log(message + "\n")
            self._draw_bar()

    def log_only(self, text):
        with self.lock:
            self._write_log(text if text.endswith("\n") else text + "\n")

    def set_phase(self, phase, level=None):
        with self.lock:
            if level is not None:
                self.progress.level = level
            self.progress.phase = phase
            self.progress.phase_start = time.monotonic()
            self._clear_bar()
            self._draw_bar()

    def close(self):
        self.stop.set()
        with self.lock:
            self.progress.finished = True
            self._clear_bar()
        self.log.close()

    def _write_log(self, text):
        self.log.write(text)
        self.log.flush()

    def _refresh_loop(self):
        while not self.stop.wait(PROGRESS_REFRESH_SECONDS):
            with self.lock:
                self._clear_bar()
                self._draw_bar()

    def _clear_bar(self):
        if self.show_bar:
            sys.stdout.write("\r\x1b[2K")

    def _draw_bar(self):
        if not self.show_bar or self.progress.finished:
            return
        sys.stdout.write(self._bar_text()[: shutil.get_terminal_size().columns - 1])
        sys.stdout.flush()

    def _bar_text(self):
        """[████░░░░] d=5 l=7 runtime 1:12 │ left: 10, 12 │ last d5 l6: rounds 3:20, total 3:41"""
        p = self.progress
        total = len(p.dimensions)
        index = min(p.dimension_index, total - 1)
        filled = round(self.BAR_WIDTH * p.dimension_index / total)
        bar = "█" * filled + "░" * (self.BAR_WIDTH - filled)
        left = ", ".join(map(str, p.dimensions[index + 1:])) or "none"
        level = "-" if p.level is None else p.level
        elapsed = format_duration(time.monotonic() - p.phase_start)
        return (f"[{bar}] d={p.dimensions[index]} l={level} {p.phase} {elapsed} │ "
                f"left: {left} │ last {p.last_level_text}")


# ---------------------------------------------------------------------------------------------
# Executing commands
# ---------------------------------------------------------------------------------------------


@dataclasses.dataclass
class CommandResult:
    returncode: int
    wall_seconds: float
    timed_out: bool
    output: str


def kill_process_group(process):
    try:
        os.killpg(process.pid, signal.SIGKILL)
    except ProcessLookupError:
        pass


def run_command(command, output_path, timeout=None, env=None):
    """Runs command in its own process group, stdout + stderr into output_path.

    The calling thread blocks in wait() (no polling); a timer kills the process group after
    timeout seconds. Ctrl+C kills the process group as well.
    """
    with open(output_path, "wb") as output:
        start = time.monotonic()
        process = subprocess.Popen(command, stdout=output, stderr=subprocess.STDOUT,
                                   stdin=subprocess.DEVNULL, start_new_session=True, env=env)
    timed_out = threading.Event()

    def on_timeout():
        timed_out.set()
        kill_process_group(process)

    timer = threading.Timer(timeout, on_timeout) if timeout else None
    if timer:
        timer.daemon = True
        timer.start()
    try:
        process.wait()
    except BaseException:
        kill_process_group(process)
        process.wait()
        raise
    finally:
        if timer:
            timer.cancel()
    wall_seconds = time.monotonic() - start
    text = Path(output_path).read_text(encoding="utf-8", errors="replace")
    return CommandResult(process.returncode, wall_seconds, timed_out.is_set(), text)


def memory_limited(command, max_memory):
    # OOMPolicy=continue: after an OOM kill, systemd would otherwise SIGTERM the whole scope
    # (including /usr/bin/time, which then reports nothing)
    return ["systemd-run", "--user", "--quiet", "--scope", "-p", f"MemoryMax={max_memory}",
            "-p", "MemorySwapMax=0", "-p", "OOMPolicy=continue", *command]


def killed_by_oom(returncode, time_output):
    # The cgroup OOM killer sends SIGKILL; /usr/bin/time reports it as exit status 137
    return returncode in (137, -signal.SIGKILL) or "terminated by signal 9" in time_output


# ---------------------------------------------------------------------------------------------
# Parsing the outputs
# ---------------------------------------------------------------------------------------------

NUMBER = r"[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?|[-+]?(?:nan|inf)"


def key_values(line):
    """'  min = 1.0 s    mean = 2.0 s    count = 3' -> {'min': 1.0, 'mean': 2.0, 'count': 3.0}"""
    return {key: float(value) for key, value in re.findall(rf"(\w+)\s*=\s*({NUMBER})", line)}


def parse_time_output(text):
    """Peak memory and elapsed time from `/usr/bin/time -v`."""
    result = {"peak_rss_bytes": None, "elapsed_seconds": None}
    match = re.search(r"Maximum resident set size \(kbytes\):\s*(\d+)", text)
    if match:
        result["peak_rss_bytes"] = int(match.group(1)) * 1024
    match = re.search(r"Elapsed \(wall clock\) time.*?:\s*([\d:.]+)\s*$", text, re.MULTILINE)
    if match:
        seconds = 0.0
        for part in match.group(1).split(":"):
            seconds = seconds * 60 + float(part)
        result["elapsed_seconds"] = seconds
    return result


def parse_benchmarker_output(text):
    """Results printed by tools::Benchmarker::print_results (current and master format)."""
    lines = text.replace("\r", "\n").splitlines()
    result = {"runs": None, "warmups": None, "result": None, "total": {}, "checkpoints": {},
              "absolute_error": {}, "relative_error": {}}
    for index, line in enumerate(lines):
        match = re.search(r"Runs:\s*(\d+)\s+Warmups:\s*(\d+)", line)
        if match:
            result["runs"], result["warmups"] = int(match.group(1)), int(match.group(2))
        elif line.startswith("Total per-run") and index + 1 < len(lines):
            result["total"] = key_values(lines[index + 1])
        elif match := re.search(rf"Result of the 1\. run:\s*({NUMBER})", line):
            result["result"] = float(match.group(1))
        elif "Absolute error:" in line:
            result["absolute_error"] = key_values(line)
        elif "Relative error:" in line:
            result["relative_error"] = key_values(line)
        elif line.startswith("Checkpoints"):
            result["checkpoints"] = parse_checkpoint_table(lines[index + 1:])
    return result


def parse_checkpoint_table(lines):
    """Header 'Name  min  mean  stddev  count', a dashed line, then one row per checkpoint."""
    if len(lines) < 2:
        return {}
    columns = lines[0].split()[1:]
    checkpoints = {}
    for line in lines[2:]:
        parts = line.rsplit(None, len(columns))
        if len(parts) != len(columns) + 1:
            break
        name, values = parts[0].strip(), parts[1:]
        try:
            checkpoints[name] = {column: float(v) for column, v in zip(columns, values)}
        except ValueError:
            break
    return checkpoints


def parse_perf_csv(text):
    """Events of `perf stat -x,` -> {event: value}, plus the CPU utilization metric."""
    events = {}
    cpus_utilized = None
    for line in text.splitlines():
        fields = line.split(",")
        if line.startswith("#") or len(fields) < 3 or not fields[2]:
            continue
        value, event = fields[0], fields[2]
        try:
            events[event] = float(value)
        except ValueError:  # <not counted>, <not supported>
            events[event] = None
        if event.startswith("task-clock") and len(fields) > 6 and "CPUs" in fields[6]:
            cpus_utilized = float(fields[5])
    return {"events": events, "cpus_utilized": cpus_utilized}


# ---------------------------------------------------------------------------------------------
# Benchmarking one (d, level)
# ---------------------------------------------------------------------------------------------

CSV_COLUMNS = [
    "dimensions", "level", "status", "peak_rss_mib", "probe_seconds", "runtime_run_seconds",
    "level_seconds", "warmups", "runs", "time_min_s", "time_mean_s", "time_stddev_s", "result",
    "abs_error_mean", "rel_error_mean", "cpus_utilized", "task_clock_ms", "page_faults",
    "context_switches", "cpu_migrations",
]


class LevelBenchmark:
    def __init__(self, args, console, raw_dir, env):
        self.args = args
        self.console = console
        self.raw_dir = raw_dir
        self.env = env

    def run(self, dimensions, level):
        """Returns the result record; record['status'] == 'ok' means the next level may run."""
        start = time.monotonic()
        record = {"dimensions": dimensions, "level": level, "status": "ok"}
        prefix = self.raw_dir / f"d{dimensions}_l{level}"

        record.update(self._probe(dimensions, level, prefix))
        if record["status"] == "ok":
            record.update(self._runtime(dimensions, level, prefix))
        if record["status"] == "ok" and RUN_PERF_STAT and shutil.which("perf"):
            record.update(self._perf(dimensions, level, prefix))

        record["level_seconds"] = time.monotonic() - start
        return record

    def _execute(self, phase, dimensions, level, command, output_path, timeout=None):
        self.console.set_phase(phase, level)
        self.console.log_only(f"\n----- d={dimensions} level={level} {phase}: "
                              f"{' '.join(map(str, command))}")
        result = run_command(command, output_path, timeout, self.env)
        self.console.log_only(self.filter_progress_lines(result.output))
        self.console.log_only(f"----- exit status {result.returncode}"
                              f"{' (timeout)' if result.timed_out else ''}, "
                              f"{format_duration(result.wall_seconds)}")
        return result

    def _binary_command(self, dimensions, level, warmup_rounds, rounds, warmup_time, round_time):
        return [str(self.args.binary), str(warmup_rounds), str(rounds),
                format_number(warmup_time), format_number(round_time), str(dimensions), str(level)]

    def filter_progress_lines(self, text):
        """Removes Warmup/Run progress lines from the logged output."""
        progress_pattern = re.compile(
            r"^\s*(?:Warmup|Run)\s+\d+/\d+,\s*"
            r"\d+(?:\.\d+)?/\d+(?:\.\d+)?\s*s\s+"
            r"\(\d+(?:\.\d+)?%\)\s*$"
        )

        # Some binaries use carriage returns (\r) instead of newlines
        # to update their progress display.
        lines = text.replace("\r\n", "\n").replace("\r", "\n").splitlines(keepends=True)

        return "".join(
            line for line in lines
            if not progress_pattern.fullmatch(line.rstrip("\n"))
        )

    def _probe(self, dimensions, level, prefix):
        time_path = prefix.with_name(prefix.name + "_probe_time.txt")
        command = memory_limited(
            [TIME_BINARY, "-v", "-o", str(time_path), "--",
             *self._binary_command(dimensions, level, 0, 1, 0, 0)],
            self.args.max_memory)
        result = self._execute("probe", dimensions, level, command,
                               prefix.with_name(prefix.name + "_probe.txt"),
                               timeout=self.args.max_duration)
        time_output = time_path.read_text(errors="replace") if time_path.exists() else ""
        self.console.log_only(time_output)
        measured = parse_time_output(time_output)

        record = {"peak_rss_bytes": measured["peak_rss_bytes"],
                  "probe_seconds": measured["elapsed_seconds"] or result.wall_seconds}
        if result.timed_out:
            record["status"] = "timeout"
        elif killed_by_oom(result.returncode, time_output):
            record["status"] = "out_of_memory"
        elif result.returncode != 0:
            record["status"] = f"probe_error({result.returncode})"
        return record

    def _runtime(self, dimensions, level, prefix):
        command = self._binary_command(dimensions, level, MIN_WARMUP_ROUNDS, MIN_ROUNDS,
                                       MIN_WARMUP_TIME, MIN_ROUND_TIME)
        if LIMIT_MEMORY_OF_RUNTIME_RUN:
            command = memory_limited(command, self.args.max_memory)
        result = self._execute("runtime", dimensions, level, command,
                               prefix.with_name(prefix.name + "_runtime.txt"))
        record = {"runtime_run_seconds": result.wall_seconds,
                  "benchmark": parse_benchmarker_output(result.output)}
        if result.returncode != 0:
            record["status"] = f"runtime_error({result.returncode})"
        return record

    def _perf(self, dimensions, level, prefix):
        perf_path = prefix.with_name(prefix.name + "_perf.csv")
        command = memory_limited(
            ["perf", "stat", "-x", ",", "-o", str(perf_path), "--",
             *self._binary_command(dimensions, level, 0, 1, 0, 0)],
            self.args.max_memory)
        result = self._execute("perf", dimensions, level, command,
                               prefix.with_name(prefix.name + "_perf.txt"),
                               timeout=PERF_TIMEOUT_FACTOR * self.args.max_duration)
        perf_output = perf_path.read_text(errors="replace") if perf_path.exists() else ""
        self.console.log_only(perf_output)
        if result.returncode != 0:
            return {"perf_status": "timeout" if result.timed_out else f"error({result.returncode})"}
        return {"perf_status": "ok", "perf": parse_perf_csv(perf_output)}


def rounded(seconds):
    return round(seconds, 3) if seconds is not None else None


def csv_row(record):
    benchmark = record.get("benchmark", {})
    events = record.get("perf", {}).get("events", {})
    peak = record.get("peak_rss_bytes")
    return {
        "dimensions": record["dimensions"],
        "level": record["level"],
        "status": record["status"],
        "peak_rss_mib": f"{peak / 1024**2:.3f}" if peak is not None else "",
        "probe_seconds": rounded(record.get("probe_seconds")),
        "runtime_run_seconds": rounded(record.get("runtime_run_seconds")),
        "level_seconds": rounded(record.get("level_seconds")),
        "warmups": benchmark.get("warmups"),
        "runs": benchmark.get("runs"),
        "time_min_s": benchmark.get("total", {}).get("min"),
        "time_mean_s": benchmark.get("total", {}).get("mean"),
        "time_stddev_s": benchmark.get("total", {}).get("stddev"),
        "result": benchmark.get("result"),
        "abs_error_mean": benchmark.get("absolute_error", {}).get("mean"),
        "rel_error_mean": benchmark.get("relative_error", {}).get("mean"),
        "cpus_utilized": record.get("perf", {}).get("cpus_utilized"),
        "task_clock_ms": next((v for k, v in events.items() if k.startswith("task-clock")), None),
        "page_faults": events.get("page-faults"),
        "context_switches": events.get("context-switches"),
        "cpu_migrations": events.get("cpu-migrations"),
    }


def level_summary(record):
    """One line per level for the terminal."""
    row = csv_row(record)
    parts = [f"d={record['dimensions']} level={record['level']}: {record['status']}"]
    if row["time_mean_s"] is not None:
        parts.append(f"mean {row['time_mean_s']:.6f} s ({row['runs']} runs, "
                     f"{row['warmups']} warmups)")
    parts.append(f"peak {format_bytes(record.get('peak_rss_bytes'))}")
    if row["cpus_utilized"] is not None:
        parts.append(f"CPUs {row['cpus_utilized']:.2f}")
    if record.get("perf_status", "ok") != "ok":
        parts.append(f"perf {record['perf_status']}")
    parts.append(f"took {format_duration(record['level_seconds'])}")
    return ", ".join(parts)


# ---------------------------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------------------------


def check_prerequisites(args):
    for tool in ("systemd-run", TIME_BINARY):
        if not shutil.which(tool):
            sys.exit(f"Error: '{tool}' not found.")
    test = subprocess.run(memory_limited(["true"], args.max_memory), capture_output=True,
                          text=True)
    if test.returncode != 0:
        sys.exit(f"Error: cannot create a memory-limited cgroup scope:\n{test.stderr}")


def write_metadata(path, args, start_time):
    binary_hash = hashlib.sha256(args.binary.read_bytes()).hexdigest()
    meta = {
        "command_line": sys.argv,
        "binary": str(args.binary),
        "binary_sha256": binary_hash,
        "binary_mtime": datetime.datetime.fromtimestamp(args.binary.stat().st_mtime).isoformat(),
        "max_duration_seconds": args.max_duration,
        "max_memory_bytes": args.max_memory,
        "dimensions": args.dimensions,
        "configuration": {
            "MIN_WARMUP_ROUNDS": MIN_WARMUP_ROUNDS, "MIN_ROUNDS": MIN_ROUNDS,
            "MIN_WARMUP_TIME": MIN_WARMUP_TIME, "MIN_ROUND_TIME": MIN_ROUND_TIME,
            "START_LEVEL": START_LEVEL, "MAX_LEVEL": MAX_LEVEL, "RUN_PERF_STAT": RUN_PERF_STAT,
            "LIMIT_MEMORY_OF_RUNTIME_RUN": LIMIT_MEMORY_OF_RUNTIME_RUN,
            "OMP_NUM_THREADS": OMP_NUM_THREADS,
        },
        "host": platform.node(),
        "platform": platform.platform(),
        "cpu_count": os.cpu_count(),
        "start_time": start_time.isoformat(timespec="seconds"),
    }
    path.write_text(json.dumps(meta, indent=2))


def benchmark_all(args, console, output_dir):
    env = dict(os.environ)
    if OMP_NUM_THREADS is not None:
        env["OMP_NUM_THREADS"] = str(OMP_NUM_THREADS)
    raw_dir = output_dir / "raw"
    raw_dir.mkdir()
    benchmark = LevelBenchmark(args, console, raw_dir, env)
    progress = console.progress

    with open(output_dir / "results.csv", "w", newline="") as csv_file, \
            open(output_dir / "results.jsonl", "w") as jsonl_file:
        writer = csv.DictWriter(csv_file, fieldnames=CSV_COLUMNS)
        writer.writeheader()
        for index, dimensions in enumerate(args.dimensions):
            progress.dimension_index = index
            console.info(f"=== d={dimensions} ===")
            level = START_LEVEL
            while MAX_LEVEL is None or level <= MAX_LEVEL:
                record = benchmark.run(dimensions, level)
                writer.writerow(csv_row(record))
                csv_file.flush()
                jsonl_file.write(json.dumps(record) + "\n")
                jsonl_file.flush()
                progress.last_level_text = (
                    f"d{dimensions} l{level}: "
                    f"rounds {format_duration(record.get('runtime_run_seconds'))}, "
                    f"total {format_duration(record['level_seconds'])}")
                console.info(level_summary(record))
                if record["status"] != "ok":
                    break
                level += 1
        progress.dimension_index = len(args.dimensions)


def raise_keyboard_interrupt(signum, frame):
    """SIGTERM / SIGHUP end the script like Ctrl+C (kills the running benchmark)."""
    raise KeyboardInterrupt


def main():
    args = parse_arguments()
    check_prerequisites(args)

    start_time = datetime.datetime.now()
    output_dir = args.output_dir or (
        args.binary.parent / "results"
        / f"{args.binary.name}_{start_time.strftime('%Y%m%d-%H%M%S')}")
    output_dir.mkdir(parents=True, exist_ok=False)
    write_metadata(output_dir / "meta.json", args, start_time)

    console = Console(output_dir / "benchmark.log", Progress(args.dimensions))
    for sig in (signal.SIGTERM, signal.SIGHUP):
        signal.signal(sig, raise_keyboard_interrupt)

    console.info(f"Binary:       {args.binary}")
    console.info(f"Dimensions:   {args.dimensions}")
    console.info(f"Max. round:   {format_duration(args.max_duration)}")
    console.info(f"Max. memory:  {format_bytes(args.max_memory)} ({args.max_memory} bytes)")
    console.info(f"Min. warmup:  {MIN_WARMUP_ROUNDS} rounds, {MIN_WARMUP_TIME:g} s")
    console.info(f"Min. rounds:  {MIN_ROUNDS} rounds, {MIN_ROUND_TIME:g} s")
    if RUN_PERF_STAT and not shutil.which("perf"):
        console.info("Warning: perf not found, skipping the perf executions")
    console.info(f"Results:      {output_dir}")
    console.info()

    exit_code = 0
    try:
        benchmark_all(args, console, output_dir)
        duration = (datetime.datetime.now() - start_time).total_seconds()
        console.info(f"\nFinished after {format_duration(duration)}.")
    except KeyboardInterrupt:
        console.info("\nAborted; the running benchmark was killed. Results so far are kept.")
        exit_code = 130
    finally:
        console.close()
    print(f"Results: {output_dir}")
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
