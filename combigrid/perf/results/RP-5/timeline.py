#!/usr/bin/env python3
"""RP-5: per-thread timeline of one e2e case from `perf record` samples (no instrumentation).

Records `perf record -F FREQ` (cycles, all threads) of `perf_e2e --filter CASE --warmups 1
--samples 1` on a snapshot, then bins every thread's samples into BIN ms slots and classifies
them: F = CC weights (FFT: kissfft / Eigen FFT / genWeights / sin / cos / sincos / cexp),
I = integrand (perf_e2e + exp), S = combigrid operator code (sumOverRows, processTensorGrid, ...),
G = libgomp, B = OpenBLAS, K = kernel (page faults, zeroing), o = other, '.' = no samples
(sleeping: futex wait in std::call_once, idle pool thread). One character per bin, dominant
class. Thread busy time per class and the idle time inside the operator window are summed for
the last operator call (window = from the first sample after the warm-up call's last FFT/S gap,
determined by the largest gap of the main thread's samples) — see the printed window.

Usage (repo root): .venv/bin/python timeline.py SNAPSHOT CASE OUTDIR [FREQ] [BIN_MS] [WSTART WEND]
(WSTART / WEND: manual window in s from the first sample, overrides the automatic one)
"""
import collections
import os
import pathlib
import re
import subprocess
import sys

snap, case, outdir = sys.argv[1], sys.argv[2], pathlib.Path(sys.argv[3])
freq = int(sys.argv[4]) if len(sys.argv) > 4 else 4000
bin_ms = float(sys.argv[5]) if len(sys.argv) > 5 else 10.0
root = pathlib.Path("combigrid/perf/.work/snapshots") / snap
outdir.mkdir(parents=True, exist_ok=True)
stem = re.sub(r"[^A-Za-z0-9]+", "_", case) + f"-{snap}"
data = outdir / f"{stem}.data"

env = dict(os.environ, OMP_NUM_THREADS="16", OMP_PROC_BIND="close", OMP_PLACES="threads",
           LD_LIBRARY_PATH=str(root / "lib"))
if not (data.exists() and os.environ.get("REUSE")):  # REUSE=1: analyse an existing recording
  subprocess.run(["perf", "record", "-q", "-F", str(freq), "-o", str(data), "--",
                str(root / "bin/perf_e2e"), "--filter", f"^{re.escape(case)}$", "--warmups", "1",
                  "--samples", "1"], env=env, check=True, stdout=subprocess.DEVNULL)
out = subprocess.run(["perf", "script", "-i", str(data), "-F", "tid,time,ip,sym,dso"],
                     capture_output=True, text=True, check=True).stdout


def classify(sym, dso):
    if re.search(r"kiss|Eigen::FFT|genWeights|forwardDFT|buildSymmetric|QuadWeightTable|computeRow"
                 r"|^(__)?(sin|cos|sincos|cexp)", sym):
        return "F"
    if "perf_e2e" in dso or ("libm" in dso and "exp" in sym):
        return "I"
    if "libsgpp" in dso:
        return "S"
    if "gomp" in dso:
        return "G"
    if "openblas" in dso:
        return "B"
    if "kernel" in dso or dso == "[unknown]":
        return "K"
    return "o"


samples = []
line = re.compile(r"^\s*(\d+)\s+([0-9.]+):\s+[0-9a-f]+\s+(\S.*?)\s+\((.*)\)\s*$")
for ln in out.splitlines():
    m = line.match(ln)
    if not m:
        continue
    tid, t, sym, dso = int(m.group(1)), float(m.group(2)), m.group(3), m.group(4)
    samples.append((tid, t, classify(sym, dso)))
t0 = min(s[1] for s in samples)
tids = sorted({s[0] for s in samples}, key=lambda x: min(s[1] for s in samples if s[0] == x))
main = tids[0]

# Operator calls: windows in which the pool threads (not the main thread) carry S/I/F samples.
pool_t = sorted(s[1] for s in samples if s[0] != main and s[2] in "SIF")
calls, start, prev = [], pool_t[0], pool_t[0]
for t in pool_t[1:]:
    if t - prev > 0.05:  # > 50 ms without pool work: next phase
        calls.append((start, prev))
        start = t
    prev = t
calls.append((start, prev))
# The operator also starts on the main thread: extend each window back to the main thread's first
# F/S/I sample after the previous window.
print(f"case {case} snapshot {snap}: {len(samples)} samples, {len(tids)} threads, "
      f"{freq} Hz, bins {bin_ms} ms")
print("pool-work windows (s from first sample):",
      ", ".join(f"[{a - t0:.3f}, {b - t0:.3f}]" for a, b in calls))
ws, we = calls[-1]
prev_end = calls[-2][1] if len(calls) > 1 else t0
ms = [s[1] for s in samples if s[0] == main and prev_end < s[1] < we and s[2] in "SIF"]
if ms:
    ws = min(ws, min(ms))
if len(sys.argv) > 7:  # manual window (s from the first sample), e.g. read off a first run
    ws, we = t0 + float(sys.argv[6]), t0 + float(sys.argv[7])
print(f"analysed window (last operator call): [{ws - t0:.3f}, {we - t0:.3f}] = "
      f"{1e3 * (we - ws):.1f} ms")

nb = int((we - ws) * 1e3 / bin_ms) + 1
dt = 1.0 / freq
tot = collections.Counter()
print("\nthread  timeline (one char per bin; F fft, I integrand, S combigrid, G gomp, B openblas, "
      "K kernel, o other, . none)   busy ms by class")
for tid in tids:
    bins = [collections.Counter() for _ in range(nb)]
    cls = collections.Counter()
    for s_tid, t, c in samples:
        if s_tid == tid and ws <= t <= we:
            bins[int((t - ws) * 1e3 / bin_ms)][c] += 1
            cls[c] += 1
    if not cls:
        continue
    row = "".join(b.most_common(1)[0][0] if b else "." for b in bins)
    busy = " ".join(f"{k}{1e3 * v * dt:.0f}" for k, v in sorted(cls.items(), key=lambda kv: -kv[1]))
    idle = sum(1 for b in bins if not b) * bin_ms
    tot.update(cls)
    print(f"{tid:7d} {row}  {busy} | idle≈{idle:.0f}")
w = 1e3 * (we - ws)
print(f"\nall threads in window: " + ", ".join(f"{k} {1e3 * v * dt:.0f} ms" for k, v in
                                               sorted(tot.items(), key=lambda kv: -kv[1])))
print(f"thread-time capacity 16 x {w:.0f} ms = {16 * w:.0f} ms; sampled busy "
      f"{1e3 * sum(tot.values()) * dt:.0f} ms")
