#!/usr/bin/env python3
"""WP-56: accuracy of the Clenshaw-Curtis weights before (complex FFT of length 2m) and after (real
FFT of length m), from the dumps of cc_weights_dump.cpp.

Reference: Waldvogel's closed form w_k = c_k/m (1 - sum_{j=1}^{m/2} b_j/(4j^2-1) cos(2 pi j k/m)) on
[-1, 1], mapped to [0, 1] (w01[i] = w[m-i]/2): with mpmath (35 digits) at all indices for n <= 129,
and in 80-bit long double (NumPy, exact angle reduction jk mod m) at all indices for n <= 4097 and
at 257 indices (0..63, the last 64, 129 spread evenly) above; the long-double reference is checked
against mpmath where both exist. Usage: .venv/bin/python cc_weights_accuracy.py DUMPDIR
"""
import sys

import mpmath
import numpy as np

d = sys.argv[1]
NS = [3, 9, 64, 65, 129, 1025, 4097, 32769, 131073, 1048577, 2097153]
mpmath.mp.dps = 35


def load(tag, n):
    return np.fromfile(f"{d}/w_{tag}_{n}.bin", dtype=np.float64)


def ref_longdouble(n, idx):
    m = n - 1
    j = np.arange(1, m // 2 + 1, dtype=np.int64)
    bj = np.where(2 * j == m, 1.0, 2.0).astype(np.longdouble)
    jl = j.astype(np.longdouble)
    f = bj / (4 * jl * jl - 1)
    two_pi = np.longdouble(2) * np.longdouble("3.14159265358979323846264338327950288")
    out = []
    for i in idx:
        k = m - i  # w01[i] = w[m - i] / 2
        s = np.sum(f * np.cos(two_pi * ((j * k) % m).astype(np.longdouble) / m))
        ck = 1 if k in (0, m) else 2
        out.append(np.longdouble(ck) / m * (1 - s) / 2)
    return np.array(out, dtype=np.longdouble)


def ref_mp(n):
    m = n - 1
    res = []
    for i in range(n):
        k = m - i
        s = mpmath.mpf(0)
        for j in range(1, m // 2 + 1):
            bj = 1 if 2 * j == m else 2
            s += mpmath.mpf(bj) / (4 * j * j - 1) * mpmath.cos(2 * mpmath.pi * ((j * k) % m) / m)
        ck = 1 if k in (0, m) else 2
        res.append(mpmath.mpf(ck) / m * (1 - s) / 2)
    return res


print("| n | indices | max abs new-old | max rel new-old | max abs err old | max abs err new | "
      "max rel err old | max rel err new | weight scale 1/m |")
print("|---|---|---|---|---|---|---|---|---|")
for n in NS:
    old, new = load("before", n), load("after", n)
    m = n - 1
    if n <= 4097:
        idx = np.arange(n)
    else:
        idx = np.unique(np.concatenate([np.arange(64), np.arange(n - 64, n),
                                        np.linspace(0, n - 1, 129).astype(np.int64)]))
    ref = ref_longdouble(n, idx)
    if n <= 129:
        mp = ref_mp(n)
        dev = max(abs(float(ref[t] - np.longdouble(str(mp[i])))) for t, i in enumerate(idx))
        assert dev < 1e-18 / m, (n, dev)  # long double: ~1e-19 relative to s_k = O(1)
    o, w = old[idx].astype(np.longdouble), new[idx].astype(np.longdouble)
    dabs = np.abs(new - old)
    drel = dabs / np.abs(old)
    eo, en = np.abs(o - ref), np.abs(w - ref)
    print(f"| {n} | {'all' if n <= 4097 else len(idx)} | {dabs.max():.2e} | {drel.max():.2e} | "
          f"{float(eo.max()):.2e} | {float(en.max()):.2e} | {float((eo / ref).max()):.2e} | "
          f"{float((en / ref).max()):.2e} | {1 / m:.2e} |")
