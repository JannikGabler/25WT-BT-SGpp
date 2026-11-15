import sys, numpy as np
def load(p): return np.fromfile(p,dtype=np.float64)
ref=load(sys.argv[1])
for p in sys.argv[2:]:
    x=load(p); off=0; worst=0; worstulp=0
    for l in range(21):
        N=(2<<l)+1; a=ref[off:off+N]; b=x[off:off+N]; off+=N
        worst=max(worst, np.max(np.abs(a-b))*(N-1))   # abs diff relative to typical weight 1/m
        worstulp=max(worstulp, np.max(np.abs(a.view(np.int64)-b.view(np.int64))))
    print(f"{sys.argv[1]} vs {p}: max |diff|*m = {worst:.3e}, max ulp = {worstulp}")
