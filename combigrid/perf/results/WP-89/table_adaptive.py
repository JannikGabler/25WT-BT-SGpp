# Markdown table of run_adaptive.sh output: per case the median refine time at 16 threads, the
# speedup over 1 thread (sync) and over the sync schedule at 16 threads, evaluations, tensor grids,
# decisions, idle share of the workers, tensor grids evaluated ahead (unused), relative error.
import json
import statistics
import sys

out = sys.argv[1]
rows16 = [json.loads(line) for line in open(f"{out}/adaptive_t16.jsonl")]
rows1 = {json.loads(line)["case"]: json.loads(line) for line in open(f"{out}/adaptive_t1.jsonl")}
med = lambda row: statistics.median(row["metrics"]["refine"])
base = lambda case: case.rsplit("/", 1)[0]
sync16 = {base(r["case"]): med(r) for r in rows16 if r["case"].endswith("/sync")}
print("| case | refine 16 t [s] | vs 1 t | vs sync | evaluations | TGs | decisions | idle | ahead (unused) | rel. error |")
print("|---|---|---|---|---|---|---|---|---|---|")
for r in rows16:
    c = r["counters"]
    one = rows1.get(base(r["case"]) + "/sync")
    t1 = f"{med(one) / med(r):.2f}x" if one else "–"
    vs = f"{sync16[base(r['case'])] / med(r):.2f}x"
    print(f"| {r['case']} | {med(r):.4f} | {t1} | {vs} | {c['evaluations']:.0f} | {c['tensorGrids']:.0f} | "
          f"{c['decisions']:.0f} | {c['idleShare']:.2f} | {c['ahead']:.0f} ({c['aheadUnused']:.0f}) | "
          f"{c['relError']:.1e} |")
print()
print("1 thread (sync):")
for case, r in rows1.items():
    print(f"- {case}: {med(r):.4f} s, {r['counters']['evaluations']:.0f} evaluations, rel. error "
          f"{r['counters']['relError']:.1e}")
