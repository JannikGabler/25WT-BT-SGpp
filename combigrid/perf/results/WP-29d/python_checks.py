#!/usr/bin/env python3
"""WP-29d: checks of the Python bindings of VectorSourceFunc (design T).

Usage (from the repo root, through the repo's .venv):
  PYTHONPATH=<pysgpp> LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-29d/python_checks.py \
      [--dump scalar.json] [--compare scalar-before.json]

- Scalar part (works with the bindings before and after WP-29d): quadrature / interpolate /
  interpolateLinear of SourceFunc over a matrix of grids; --dump writes the results (repr of the
  floats, i.e. bitwise), --compare reports any difference to an earlier dump (bitwise at
  OMP_NUM_THREADS=1, else within 1e-13 relative; --tolerant: always with the tolerance, for a
  comparison across library builds, e.g. OPT=0 vs. OPT=1). Also records how a plain callable
  passed to the operators is rejected.
- Vector part (only if pysgpp has VectorSourceFunc): component k of every vector operator equals
  the scalar operator on output k (tolerance in units of eps * sum|c| * max|f| * max(1, vol));
  error handling (wrong return length / type, exceptions inside the callable, nOutputs = 0,
  non-callable), overload selection, reuse after an error and reference counts of the callable.
Exit code 1 if any check fails.
"""
import argparse
import gc
import json
import math
import os
import sys

import pysgpp

EPS = 2.0 ** -52
TOL_UNITS = 64.0
failures = []


def tolist(v):
    # Base's DataVector has __len__ and an unchecked __getitem__ but no __iter__: list(v) reads
    # past the end (Python's legacy iteration stops only at an IndexError).
    return [v[i] for i in range(len(v))]


def check(cond, msg):
    if not cond:
        failures.append(msg)
        print("FAIL:", msg)


def make_grid(nodes, boundary, n_dim, level, domain):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain(domain)
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen, pysgpp.SparseGrid(gen)


def make_mivec_grid(nodes, boundary, domain):
    mis = pysgpp.LvlMIVec([[0, 3], [2, 1], [1, 2]])
    gen = pysgpp.MIVecSGGenInstr(mis)
    gen.setDomain(domain)
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen, pysgpp.SparseGrid(gen)


def scalar_f(k):
    def f(p):
        s = 0.3 * (k + 1)
        for j, x in enumerate(p):
            s += 0.2 * (j + 1 + k) * x
        return math.cos(s) + 0.1 * k
    return f


def vector_f(m):
    fs = [scalar_f(k) for k in range(m)]

    def f(p):
        return [fk(p) for fk in fs]
    return f


NODES = {
    "equi": pysgpp.getEquidistantNodeGenFunc(),
    "cc": pysgpp.getClenshawCurtisNodeGenFunc(),
    "cheb1": pysgpp.getFirstTypeChebyshevNodeGenFunc(),
}
DOMAINS = {"unit": (0.0, 1.0), "shifted": (-1.0, 2.0)}


def cases():
    for node_name, nodes in NODES.items():
        for b in (0, 1):
            for dom_name, dom in DOMAINS.items():
                for d in (1, 2, 3):
                    gen, sg = make_grid(nodes, b, d, 3, dom)
                    yield f"complete/{node_name}/b{b}/{dom_name}/d{d}/L3", nodes, dom, d, sg
                gen, sg = make_mivec_grid(nodes, b, dom)
                yield f"mivec/{node_name}/b{b}/{dom_name}/d2", nodes, dom, 2, (gen, sg)


def points(dom, d):
    lo, hi = dom
    generic = [lo + (hi - lo) * (0.137 + 0.21 * j) for j in range(d)]
    corner = [lo] * d
    return {"generic": generic, "corner": corner}


def unpack(sg):
    # MIVec grids keep (gen, sg) together so that the LvlMIVec stays referenced explicitly.
    return sg[1] if isinstance(sg, tuple) else sg


def scalar_results():
    out = {}
    for name, nodes, dom, d, sg in cases():
        sg = unpack(sg)
        for k in (0, 2):
            sf = pysgpp.SourceFunc(scalar_f(k))
            if nodes is not NODES["cheb1"]:
                out[f"quad/{name}/k{k}"] = repr(pysgpp.quadrature(sf, sg))
            for pname, p in points(dom, d).items():
                dv = pysgpp.DataVector(p)
                out[f"interp/{name}/{pname}/k{k}"] = repr(pysgpp.interpolate(sf, dv, sg))
                out[f"interplin/{name}/{pname}/k{k}"] = repr(
                    pysgpp.interpolateLinear(sf, dv, sg))
    return out


def plain_callable_behavior():
    _, sg = make_grid(NODES["cc"], 0, 2, 2, (0.0, 1.0))
    dv = pysgpp.DataVector([0.3, 0.4])
    res = {}
    for op, args in (("quadrature", (sg,)), ("interpolate", (dv, sg)),
                     ("interpolateLinear", (dv, sg))):
        try:
            getattr(pysgpp, op)(lambda p: 1.0, *args)
            res[op] = "accepted"
        except Exception as e:  # noqa: BLE001
            res[op] = type(e).__name__
    return res


def sum_abs_coeffs(sg):
    return sum(abs(sg.getTensorGrid(i).coefficient) for i in range(sg.nTG()))


def vector_checks():
    m_values = (1, 3)
    n_compared = 0
    n_bitwise = 0
    max_units = 0.0
    for name, nodes, dom, d, sg in cases():
        sg = unpack(sg)
        vol = (dom[1] - dom[0]) ** d
        scale = EPS * sum_abs_coeffs(sg) * 1.5 * max(1.0, vol)  # |f_k| <= 1.2
        for m in m_values:
            vsf = pysgpp.VectorSourceFunc(vector_f(m), m)
            check(vsf.nOutputs() == m, f"nOutputs {name} m{m}")
            sfs = [pysgpp.SourceFunc(scalar_f(k)) for k in range(m)]
            runs = []
            if nodes is not NODES["cheb1"]:
                runs.append(("quad", pysgpp.quadrature(vsf, sg),
                             [pysgpp.quadrature(sf, sg) for sf in sfs]))
            for pname, p in points(dom, d).items():
                dv = pysgpp.DataVector(p)
                runs.append((f"interp/{pname}", pysgpp.interpolate(vsf, dv, sg),
                             [pysgpp.interpolate(sf, dv, sg) for sf in sfs]))
                runs.append((f"interplin/{pname}", pysgpp.interpolateLinear(vsf, dv, sg),
                             [pysgpp.interpolateLinear(sf, dv, sg) for sf in sfs]))
            for op, vec, scal in runs:
                check(isinstance(vec, pysgpp.DataVector) and vec.getSize() == m,
                      f"{op}/{name}/m{m}: result type/size")
                for k in range(m):
                    n_compared += 1
                    diff = abs(vec[k] - scal[k])
                    n_bitwise += diff == 0.0
                    units = diff / scale
                    max_units = max(max_units, units)
                    check(units <= TOL_UNITS, f"{op}/{name}/m{m}/k{k}: {vec[k]!r} vs {scal[k]!r} "
                                              f"({units:.1f} units)")
    print(f"vector vs scalar: {n_compared} components, {n_bitwise} bitwise equal, "
          f"max {max_units:.2f} units of eps*sum|c|*max|f|*max(1,vol) (limit {TOL_UNITS:.0f})")


def expect_raises(exc_type, fn, msg):
    try:
        fn()
    except exc_type as e:
        return e
    except Exception as e:  # noqa: BLE001
        check(False, f"{msg}: raised {type(e).__name__} ({e}) instead of {exc_type.__name__}")
        return None
    check(False, f"{msg}: no exception")
    return None


class CallableError(Exception):
    pass


def error_checks():
    _, sg = make_grid(NODES["cc"], 0, 3, 4, (0.0, 1.0))
    dv = pysgpp.DataVector([0.3, 0.4, 0.5])
    ops = {
        "quadrature": lambda f: pysgpp.quadrature(f, sg),
        "interpolate": lambda f: pysgpp.interpolate(f, dv, sg),
        "interpolateLinear": lambda f: pysgpp.interpolateLinear(f, dv, sg),
        "evaluate": lambda f: f.evaluate(dv),
    }
    good = vector_f(2)
    reference = {op: tolist(run(pysgpp.VectorSourceFunc(good, 2))) for op, run in ops.items()}

    bad_returns = {
        "too long": (ValueError, lambda p: [1.0, 2.0, 3.0]),
        "too short": (ValueError, lambda p: [1.0]),
        "empty": (ValueError, lambda p: []),
        "not a sequence": (TypeError, lambda p: 1.0),
        "None": (TypeError, lambda p: None),
        "element not a float": (TypeError, lambda p: [1.0, "a"]),
    }
    for what, (exc, func) in bad_returns.items():
        vsf = pysgpp.VectorSourceFunc(func, 2)
        for op, run in ops.items():
            expect_raises(exc, lambda: run(vsf), f"{op}: callable returns {what}")

    # Tuples, generators of the right length and NumPy arrays are sequences of floats too.
    try:
        import numpy as np
        accepted = {"tuple": lambda p: tuple(good(p)), "numpy": lambda p: np.array(good(p)),
                    "ints": lambda p: [int(v > 0) for v in good(p)]}
    except ImportError:
        accepted = {"tuple": lambda p: tuple(good(p))}
    for what, func in accepted.items():
        vsf = pysgpp.VectorSourceFunc(func, 2)
        res = tolist(ops["quadrature"](vsf))
        if what != "ints":
            check(all(abs(a - b) <= 1e-13 for a, b in zip(res, reference["quadrature"])),
                  f"quadrature: callable returns {what}")

    # An exception raised inside the callable propagates (same object), at any evaluation, and
    # the function is usable again afterwards.
    for op, run in ops.items():
        calls = [0]
        raised = CallableError("raised inside the callable")

        def failing(p, calls=calls, raised=raised):
            calls[0] += 1
            if calls[0] == 5 or op == "evaluate":
                raise raised
            return good(p)

        vsf = pysgpp.VectorSourceFunc(failing, 2)
        e = expect_raises(CallableError, lambda: run(vsf), f"{op}: exception in callable")
        check(e is raised, f"{op}: the propagated exception is the raised object")
        if op != "evaluate":
            res = tolist(run(vsf))
            ref = reference[op]
            check(all(abs(a - b) <= 1e-13 * max(1.0, abs(b)) for a, b in zip(res, ref)),
                  f"{op}: reuse after an exception gives {res} instead of {ref}")

    # The scalar path keeps its behavior.
    sf_calls = [0]

    def scalar_failing(p):
        sf_calls[0] += 1
        if sf_calls[0] == 3:
            raise CallableError("scalar")
        return 1.0
    expect_raises(CallableError, lambda: pysgpp.quadrature(pysgpp.SourceFunc(scalar_failing), sg),
                  "scalar quadrature: exception in callable")

    expect_raises(RuntimeError, lambda: pysgpp.VectorSourceFunc(good, 0), "nOutputs = 0")
    expect_raises(TypeError, lambda: pysgpp.VectorSourceFunc(42, 2), "non-callable")
    expect_raises((TypeError, OverflowError), lambda: pysgpp.VectorSourceFunc(good, -1),
                  "nOutputs = -1")
    expect_raises(TypeError, lambda: pysgpp.VectorSourceFunc(good), "missing nOutputs")

    # Overload selection: SourceFunc -> float, VectorSourceFunc -> DataVector (also for m = 1).
    sf = pysgpp.SourceFunc(scalar_f(0))
    vsf1 = pysgpp.VectorSourceFunc(vector_f(1), 1)
    for op, run in ops.items():
        if op == "evaluate":
            check(isinstance(run(sf), float), "SourceFunc.evaluate returns a float")
        else:
            check(isinstance(run(sf), float), f"{op}(SourceFunc) returns a float")
        r = run(vsf1)
        check(isinstance(r, pysgpp.DataVector) and r.getSize() == 1,
              f"{op}(VectorSourceFunc m=1) returns a DataVector of size 1")
        check(abs(r[0] - run(sf)) <= 1e-13, f"{op}: m = 1 equals scalar")

    # Reference counts / lifetime of the callable.
    def counted(p):
        return [1.0, 2.0]
    before = sys.getrefcount(counted)
    vsf = pysgpp.VectorSourceFunc(counted, 2)
    check(sys.getrefcount(counted) == before + 1, "the VectorSourceFunc holds one reference")
    del vsf
    gc.collect()
    check(sys.getrefcount(counted) == before, "deleting the VectorSourceFunc releases it")

    vsf = pysgpp.VectorSourceFunc(lambda p: [sum(p), 2.0], 2)  # only referenced by vsf
    gc.collect()
    res = tolist(pysgpp.quadrature(vsf, sg))
    check(abs(res[0] - 1.5) <= 1e-12 and abs(res[1] - 2.0) <= 1e-12,
          f"temporary lambda stays alive: {res}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dump")
    parser.add_argument("--compare")
    parser.add_argument("--tolerant", action="store_true",
                        help="compare with a tolerance also at 1 thread (other library build)")
    args = parser.parse_args()

    scalar = scalar_results()
    scalar["plain_callable"] = plain_callable_behavior()
    print(f"scalar results: {len(scalar) - 1}; plain callable: {scalar['plain_callable']}")
    if args.dump:
        with open(args.dump, "w") as fh:
            json.dump(scalar, fh, indent=0, sort_keys=True)
    if args.compare:
        with open(args.compare) as fh:
            old = json.load(fh)
        diff = [k for k in old if old[k] != scalar.get(k)]
        print(f"scalar results vs {args.compare}: {len(old) - len(diff)} of {len(old)} identical")
        # With several threads, the OpenMP reduction order varies from run to run (C++ as well):
        # bitwise only at 1 thread with the same library build, otherwise relative to the
        # magnitude.
        check(len(old) == len(scalar), "scalar result keys differ")
        if os.environ.get("OMP_NUM_THREADS") == "1" and not args.tolerant:
            check(not diff, f"scalar results differ from {args.compare}: {diff[:10]}")
        else:
            rel = max((abs(float(old[k]) - float(scalar[k])) / max(1.0, abs(float(old[k])))
                       for k in diff if k != "plain_callable"), default=0.0)
            check(rel <= 1e-13 and old["plain_callable"] == scalar["plain_callable"],
                  f"scalar results differ from {args.compare} by up to {rel:.2e} (relative)")
            print(f"  max difference {rel:.2e} (relative to max(1, |value|))")

    if hasattr(pysgpp, "VectorSourceFunc"):
        vector_checks()
        error_checks()
    else:
        print("pysgpp has no VectorSourceFunc: vector checks skipped")

    print("RESULT:", "FAIL" if failures else "OK", f"({len(failures)} failures)")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
