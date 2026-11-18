#!/usr/bin/env python3
"""RP-6: makespan of a parallel TG loop under cost = grid points (evaluation-bound model).

Input: tg-sizes.jsonl (tg_sizes.cpp). Per case: total points, largest TG, bound
max(total / T, largest), and the simulated makespan (16 threads, chunk 1, greedy: each next TG
goes to the thread that becomes free first) for
  dynamic   TG order as stored (`schedule(dynamic)`, what SG / DSG quadrature use since WP-56),
  lpt       largest TG first (sorted descending, then dynamic),
  static    `schedule(static)` blocks of ceil(n / T) consecutive TGs (SG interplin),
  guided    OpenMP guided (chunk = ceil(remaining / T), min 1) in stored order.
Efficiency = bound / makespan. Optional per-TG overhead `--tg-cost` in point units; `--filter` = substring.
Usage: .venv/bin/python schedule_sim.py tg-sizes.jsonl [--threads 16] [--tg-cost 0]
"""
import argparse
import heapq
import json
import math

ap = argparse.ArgumentParser()
ap.add_argument("file")
ap.add_argument("--threads", type=int, default=16)
ap.add_argument("--tg-cost", type=float, default=0.0)
ap.add_argument("--filter", default="")
a = ap.parse_args()
T = a.threads


def greedy(chunks):
    h = [0.0] * T
    for c in chunks:
        t = heapq.heappop(h)
        heapq.heappush(h, t + c)
    return max(h)


def guided(costs):
    chunks, i, n = [], 0, len(costs)
    while i < n:
        k = max(1, math.ceil((n - i) / T))
        chunks.append(sum(costs[i:i + k]))
        i += k
    return greedy(chunks)


def static(costs):
    n = len(costs)
    k = math.ceil(n / T)
    return max(sum(costs[i:i + k]) for i in range(0, n, k))


print("| case | nTG | points | largest TG | largest / (points / T) | bound | dynamic | lpt | static | guided | eff. dynamic | eff. lpt |")
print("|---|---|---|---|---|---|---|---|---|---|---|---|")
for line in open(a.file):
    r = json.loads(line)
    if a.filter not in r["case"]:
        continue
    costs = [n + a.tg_cost for n in r["nGP"]]
    tot = sum(costs)
    big = max(costs)
    bound = max(tot / T, big)
    dyn = greedy(costs)
    lpt = greedy(sorted(costs, reverse=True))
    print(f"| {r['case']} | {len(costs)} | {int(tot)} | {int(big)} | {big / (tot / T):.2f} | {bound:.0f} "
          f"| {dyn:.0f} | {lpt:.0f} | {static(costs):.0f} | {guided(costs):.0f} "
          f"| {bound / dyn:.3f} | {bound / lpt:.3f} |")
