import json,sys,math
a=[json.loads(l) for l in open(sys.argv[1])]
b=[json.loads(l) for l in open(sys.argv[2])]
ratios=[];worse=[]
for x,y in zip(a,b):
    if x["values"]==y["values"]: continue
    ea=max(abs(p-e) for p,e in zip(x["values"],x["exact"]))
    eb=max(abs(p-e) for p,e in zip(y["values"],y["exact"]))
    # round-off unit of the combination as in the refcheck: 2^-52 * magnitude * (sum|c| + sqrt(#points))
    unit=2**-52*x["magnitude"]*(x["sumAbsCoeffs"]+math.sqrt(x["sumGP"]))
    r=(eb+1e-300)/(ea+1e-300); ratios.append(r)
    if eb>1.1*ea: worse.append((x["case"],ea,eb,(eb-ea)/unit))
print(f"{len(ratios)} differing cases; error ratio after/before: geomean {math.exp(sum(map(math.log,ratios))/len(ratios)):.3f}, max {max(ratios):.2f}, min {min(ratios):.2f}")
print("cases with error > 1.1x before (growth in round-off units of the combination):")
for c,ea,eb,u in sorted(worse,key=lambda t:-t[3]): print(f"  {c:45s} {ea:.2e} -> {eb:.2e}  (+{u:.2f} units)")
