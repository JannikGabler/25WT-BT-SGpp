"""WP-77: makespan model of the largest-first order with and without splitting large tensor grids.

Cost of a TG = its points (RP-6 model, results/RP-6/schedule_sim.py); 16 threads, chunk 1, greedy.
  order    size-class order (bit width + 3 mantissa bits, stable), as implemented
  split K  TGs with more than total / K points split into ceil(points / (total / K)) equal parts
           (thread-count independent rule; row granularity ignored), then the same order
Efficiency = max(total / T, largest part) / makespan; 'vs order' = makespan(order) / makespan(split).
Usage: .venv/bin/python split_sim.py ../RP-6/tg-sizes.jsonl
"""
import heapq
import json
import math
import sys

T = 16


def greedy(costs):
    h = [0.0] * T
    for c in costs:
        heapq.heappush(h, heapq.heappop(h) + c)
    return max(h)


def size_class(n):
    if n == 0:
        return 0
    e = n.bit_length() - 1
    lead = n >> (e - 3) if e >= 3 else n << (3 - e)
    return ((e + 1) << 3) | (lead & 7)


def ordered(costs):
    return sorted(costs, key=lambda n: -size_class(int(n)))


def split(costs, k):
    share = sum(costs) / k
    out = []
    for c in costs:
        parts = max(1, math.ceil(c / share))
        out += [c / parts] * parts
    return out


print("| case | nTG | largest / share | eff. order | eff. split 16 | eff. split 32 | split 16 vs order | split 32 vs order |")
print("|---|---|---|---|---|---|---|---|")
for line in open(sys.argv[1]):
    r = json.loads(line)
    c = r["nGP"]
    tot = sum(c)
    bound = tot / T
    o = greedy(ordered(c))
    s16 = greedy(ordered(split(c, 16)))
    s32 = greedy(ordered(split(c, 32)))
    print(f"| {r['case']} | {len(c)} | {max(c) / bound:.2f} | {max(bound, max(c)) / o:.3f} "
          f"| {bound / s16:.3f} | {bound / s32:.3f} | {o / s16:.3f} | {o / s32:.3f} |")
