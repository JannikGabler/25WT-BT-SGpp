"""WP-61: round-off of the SG quadrature's combination sum_l c_l Q_l per summation variant.

Reads the dumps of combination_variants.cpp (argv[1]) and prints per case the relative error
of each variant against the exact (rational) sum of the same per-tensor-grid estimates, the
condition number sum|c Q| / |sum c Q|, the discretization error |exact sum - integral| and
which variant the library result equals bitwise. Run with the repo's .venv/bin/python.
"""
import struct
import sys
from fractions import Fraction

VARIANTS = ["plain", "plainfma", "split16", "pairwise", "neumaier"]


def load(path):
    raw = open(path, "rb").read()
    n, exact, library = struct.unpack_from("<qdd", raw, 0)
    off = 24
    coeffs = struct.unpack_from(f"<{n}i", raw, off)
    off += 4 * n
    q = struct.unpack_from(f"<{n}d", raw, off)
    off += 8 * n
    variants = struct.unpack_from(f"<{len(VARIANTS)}d", raw, off)
    return n, exact, library, coeffs, q, variants


def main():
    outdir = sys.argv[1]
    names = open(f"{outdir}/cases.txt").read().split("\n")
    print(f"| case | nTG | cond | discr. rel. err | " + " | ".join(VARIANTS) + " | library = |")
    print("|---|---|---|---|" + "---|" * len(VARIANTS) + "---|")
    for idx, name in enumerate(n for n in names if n):
        n, exact, library, coeffs, q, variants = load(f"{outdir}/{idx}.bin")
        terms = [Fraction(c) * Fraction(x) for c, x in zip(coeffs, q)]
        s = sum(terms)
        cond = float(sum(abs(t) for t in terms) / abs(s))
        discr = abs(float(s) - exact) / abs(exact)
        errs = [abs(Fraction(v) - s) / abs(s) for v in variants]
        same = [VARIANTS[k] for k, v in enumerate(variants) if v == library] or ["-"]
        print(f"| {name} | {n} | {cond:.1e} | {discr:.1e} | "
              + " | ".join(f"{float(e):.1e}" for e in errs) + f" | {'/'.join(same)} |")


main()
