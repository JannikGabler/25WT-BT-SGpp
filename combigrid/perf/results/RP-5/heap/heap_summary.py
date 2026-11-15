#!/usr/bin/env python3
"""RP-5: condensed heaptrack peak consumers: size + the first SG++ / Eigen frame of each top block.
Usage: heap_summary.py FILE.zst [N]"""
import re, subprocess, sys
f, n = sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 12
out = subprocess.run(["heaptrack_print", "-f", f, "--print-peaks", "1", "--print-allocators", "0",
                      "--print-temporary", "0", "--print-leaks", "0", "--peak-limit", str(n),
                      "--sub-peak-limit", "0"], capture_output=True, text=True).stdout
lines = out.splitlines()
for i, l in enumerate(lines):
    m = re.match(r"^([0-9.]+[KMG]?) peak memory consumed over (\d+) calls from", l)
    if m:
        frame = next((x.strip() for x in lines[i + 1:i + 40]
                      if re.match(r"\s*(sgpp::|Eigen::)", x) and "allocat" not in x), "?")
        print(f"{m.group(1):>8}  {m.group(2):>6} calls  {re.sub(r'<.*?>', '<>', frame)[:150]}")
for l in lines:
    if l.startswith(("peak heap memory consumption", "peak RSS", "total memory leaked", "calls to allocation")):
        print(l)
