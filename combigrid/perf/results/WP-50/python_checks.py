#!/usr/bin/env python3
"""WP-50: checks of the Python bindings of DeltasSparseGrid and its three operators.

Usage (from the repo root, through the repo's .venv):
  PYTHONPATH=<pysgpp> LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-50/python_checks.py

Checks, over equi / cc nodes x boundary level offset 0 / 1 x unit / shifted domain x complete
grids (d = 1, 2, 3) and an MIVec grid:
- DSG operator == SparseGrid operator within round-off (tolerance: 64 units of eps * scale,
  scale = max(1, vol) * sum|c| * max|f| for quadrature and interpolateLinear; for interpolate
  sum over the tensor grids of |c| * product of the 1D Lebesgue sums sum_i |L_i(x_k)| * max|f|,
  as the C++ test of the DSG global interpolation: equidistant nodes of high degree are
  ill-conditioned) for quadrature, interpolate, interpolateLinear with
  SourceFunc, VectorSourceFunc (m = 3; every component also vs. the scalar DSG run),
  CachedSourceFunc and CachedVectorSourceFunc (cached == uncached DSG; bitwise at 1 thread);
- evaluation counts and points: DSG quadrature / interpolate evaluate every grid point once (the
  distinct points of all SparseGrid tensor grids, bitwise); DSG interpolateLinear evaluates every
  distinct corner the SparseGrid operator evaluates, once;
- grid queries (nDim, nTG, getTensorGrid by index / LvlMI, getTensorGrids, len, [], iteration, ==,
  getGenInstr, getMaxTG*), delta tensor grids partition the grid points, coefficients match;
- errors: non-nested nodes (cheb1) -> RuntimeError; wrong vector result length -> ValueError;
  exception of the callable propagates (same object) and the function is reusable; plain callable
  -> TypeError; internals (getOperatorData) not exposed;
- keep-alive: a DSG built from an MIVecSGGenInstr keeps the LvlMIVec alive (all other references
  dropped); TensorGridCTData keeps its grid alive; reference count of the callable restored;
- cached function: a DSG operator after the SparseGrid operator on the same points calls Python
  zero times (values are shared between both grid types).
Exit code 1 if any check fails.
"""
import gc
import math
import os
import sys

import pysgpp

EPS = 2.0 ** -52
TOL_UNITS = 64.0
THREADS = os.environ.get("OMP_NUM_THREADS", "?")
failures = []
stats = {"cmp": 0, "bitwise": 0, "max_units": 0.0}


def tolist(v):
    # DataVector has no __iter__ (list(v) reads past the end): index explicitly.
    return [v[i] for i in range(len(v))]


def check(cond, msg):
    if not cond:
        failures.append(msg)
        print("FAIL:", msg)


def raises(exc_type, fn):
    try:
        fn()
    except exc_type as e:
        return e
    except Exception as e:  # noqa: BLE001
        print("  unexpected", type(e).__name__, e)
        return None
    return None


def configure(gen, nodes, boundary, domain):
    gen.setDomain(domain)
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen


def complete_gen(nodes, boundary, n_dim, level, domain):
    return configure(pysgpp.CompleteSGGenInstr(level, n_dim), nodes, boundary, domain)


def mivec_gen(nodes, boundary, domain):
    return configure(pysgpp.MIVecSGGenInstr(pysgpp.LvlMIVec([[0, 3], [2, 1], [1, 2]])), nodes,
                     boundary, domain)


def f0(p):
    return math.exp(0.3 * sum(p)) + math.cos(sum((k + 1) * x for k, x in enumerate(p)))


def vec_values(p):
    s = f0(p)
    return [s, 2.0 * s - 1.0, math.sin(sum(p)) + 0.5]


def scalar_k(k):
    return lambda p: vec_values(p)[k]


def tolerance_units(a, b, scale):
    return abs(a - b) / (EPS * scale)


def compare(a, b, scale, msg):
    stats["cmp"] += 1
    if a == b:
        stats["bitwise"] += 1
        return
    units = tolerance_units(a, b, scale)
    stats["max_units"] = max(stats["max_units"], units)
    check(units <= TOL_UNITS, f"{msg}: {a!r} vs {b!r} ({units:.1f} units)")


def scale_of(sg, gen, fmax):
    coeff_sum = sum(abs(sg.getTensorGrid(i).coefficient) for i in range(sg.nTG()))
    return coeff_sum * fmax * max(1.0, gen.getVolumeOfDomain())


def lebesgue_1d(nodes, x):
    total = 0.0
    for i, xi in enumerate(nodes):
        basis = 1.0
        for j, xj in enumerate(nodes):
            if j != i:
                basis *= (x - xj) / (xi - xj)
        total += abs(basis)
    return total


def interp_scale(sg, gen, point, fmax):
    domain = gen.getDomain()
    x = [(point[k] - domain[k][0]) / (domain[k][1] - domain[k][0]) for k in range(sg.nDim())]
    scale = 0.0
    for tg_data in sg:
        tg = tg_data.tensorGrid
        counts = tolist(tg.getGPCntPerDim())
        nodes = tolist(tg.getNodesPerDim())
        product, offset = 1.0, 0
        for k, n in enumerate(counts):
            product *= lebesgue_1d(nodes[offset:offset + n], x[k])
            offset += n
        scale += abs(tg_data.coefficient) * product
    return 1.0 + fmax * scale


def points_for(domain):
    lo = [a for a, _ in domain]
    hi = [b for _, b in domain]
    rel = [(0.31, 0.47, 0.73), (0.9, 0.05, 0.5)]
    return [pysgpp.DataVector([lo[k] + r[k] * (hi[k] - lo[k]) for k in range(len(domain))])
            for r in rel]


def operators(point):
    return {
        "quad": lambda f, g: pysgpp.quadrature(f, g),
        "interp": lambda f, g: pysgpp.interpolate(f, point, g),
        "interplin": lambda f, g: pysgpp.interpolateLinear(f, point, g),
    }


def recording(fn):
    seen = []

    def wrapped(p):
        seen.append(tuple(p))
        return fn(p)
    return wrapped, seen


def check_grid_queries(name, gen, sg, dsg):
    check(dsg.nDim() == sg.nDim(), f"{name}: nDim")
    check(len(dsg) == dsg.nTG() and dsg.nTG() >= sg.nTG(), f"{name}: nTG")
    tgs = dsg.getTensorGrids()
    check(isinstance(tgs, tuple) and len(tgs) == dsg.nTG(), f"{name}: getTensorGrids")
    check(len([tg for tg in dsg]) == dsg.nTG(), f"{name}: iteration")
    check(raises(IndexError, lambda: dsg[dsg.nTG()]) is not None, f"{name}: IndexError")
    sg_coeffs = {tuple(tg.mi[k] for k in range(sg.nDim())): tg.coefficient for tg in sg}
    n_gp = 0
    max_gp = 0
    for i, tg in enumerate(dsg):
        mi = tuple(tg.mi[k] for k in range(dsg.nDim()))
        check(tg.coefficient == sg_coeffs.get(mi, 0), f"{name}: coefficient of {mi}")
        found = dsg.getTensorGrid(pysgpp.LvlMI(list(mi)))
        check(found is not None and found.coefficient == tg.coefficient, f"{name}: lookup {mi}")
        check(dsg.getTensorGrid(i).mi == tg.mi, f"{name}: getTensorGrid({i})")
        n_gp += tg.tensorGrid.nGP()
        max_gp = max(max_gp, tg.tensorGrid.nGP())
    check(dsg.getMaxTGGPCnt() == max_gp, f"{name}: getMaxTGGPCnt")
    check(dsg.getMaxTGSumOverGPCntsPerDim() > 0 or dsg.nDim() == 0, f"{name}: getMaxTGSum")
    absent = pysgpp.LvlMI([50] * dsg.nDim())
    check(dsg.getTensorGrid(absent) is None, f"{name}: absent MI -> None")
    check(dsg == pysgpp.DeltasSparseGrid(gen), f"{name}: == same instruction")
    check(dsg.getGenInstr().nDim() == gen.nDim(), f"{name}: getGenInstr")
    check(not hasattr(dsg, "getOperatorData"), f"{name}: getOperatorData exposed")
    return n_gp


def scales(sg, gen, point, fmax):
    scale = scale_of(sg, gen, fmax)
    return {"quad": scale, "interp": interp_scale(sg, gen, point, fmax), "interplin": scale}


def check_scalar(name, gen, sg, dsg, point, n_gp):
    scale_by_op = scales(sg, gen, point, 3.5)
    for op_name, op in operators(point).items():
        scale = scale_by_op[op_name]
        rec_sg, seen_sg = recording(f0)
        rec_dsg, seen_dsg = recording(f0)
        a = op(pysgpp.SourceFunc(rec_sg), sg)
        b = op(pysgpp.SourceFunc(rec_dsg), dsg)
        check(isinstance(b, float), f"{name}/{op_name}: scalar DSG result type {type(b)}")
        compare(a, b, scale, f"{name}/{op_name}: DSG vs SG")
        distinct = set(seen_dsg)
        check(len(distinct) == len(seen_dsg), f"{name}/{op_name}: DSG evaluates a point twice")
        check(distinct == set(seen_sg), f"{name}/{op_name}: DSG points != SG points")
        if op_name != "interplin":
            check(len(seen_dsg) == n_gp, f"{name}/{op_name}: {len(seen_dsg)} evals != {n_gp} GP")


def check_vector(name, gen, sg, dsg, point):
    scale_by_op = scales(sg, gen, point, 7.0)
    for op_name, op in operators(point).items():
        scale = scale_by_op[op_name]
        vsf = pysgpp.VectorSourceFunc(vec_values, 3)
        r_dsg = op(vsf, dsg)
        r_sg = op(vsf, sg)
        check(isinstance(r_dsg, pysgpp.DataVector) and len(r_dsg) == 3,
              f"{name}/{op_name}: vector DSG result")
        for k in range(3):
            compare(r_sg[k], r_dsg[k], scale, f"{name}/{op_name}: vector DSG vs SG [{k}]")
            compare(op(pysgpp.SourceFunc(scalar_k(k)), dsg), r_dsg[k], scale,
                    f"{name}/{op_name}: vector DSG [{k}] vs scalar DSG")


def check_cached(name, gen, sg, dsg, point):
    scale_by_op = scales(sg, gen, point, 7.0)
    for op_name, op in operators(point).items():
        scale = scale_by_op[op_name]
        calls = [0]

        def counted(p):
            calls[0] += 1
            return f0(p)
        csf = pysgpp.CachedSourceFunc(counted)
        compare(op(pysgpp.SourceFunc(f0), dsg), op(csf, dsg), scale,
                f"{name}/{op_name}: cached DSG vs DSG")
        # SparseGrid first, then the DSG on the same points: every value is a cache hit.
        csf2 = pysgpp.CachedSourceFunc(counted)
        op(csf2, sg)
        before = calls[0]
        compare(op(csf, sg), op(csf2, dsg), scale, f"{name}/{op_name}: cached SG vs cached DSG")
        check(calls[0] == before, f"{name}/{op_name}: DSG after SG called Python "
              f"{calls[0] - before} times")
        cvsf = pysgpp.CachedVectorSourceFunc(vec_values, 3)
        r = op(cvsf, dsg)
        r_ref = op(pysgpp.VectorSourceFunc(vec_values, 3), dsg)
        for k in range(3):
            compare(r_ref[k], r[k], scale, f"{name}/{op_name}: cached vector DSG [{k}]")


def cache_available():
    try:
        pysgpp.CachedSourceFunc(f0)
        return True
    except RuntimeError as e:
        print("cache not available, cached checks skipped:", e)
        return False


def grid_matrix():
    nodes = {"equi": pysgpp.getEquidistantNodeGenFunc(), "cc": pysgpp.getClenshawCurtisNodeGenFunc()}
    domains = {"unit": lambda d: [(0.0, 1.0)] * d,
               "shifted": lambda d: [(-1.0 + 0.5 * k, 2.0 + 0.25 * k) for k in range(d)]}
    for node_name, node in nodes.items():
        for boundary in (0, 1):
            for dom_name, dom in domains.items():
                for n_dim, level in ((1, 5), (2, 4), (3, 3)):
                    name = f"{node_name}/b{boundary}/{dom_name}/d{n_dim}/L{level}"
                    yield name, complete_gen(node, boundary, n_dim, level, dom(n_dim)), dom(n_dim)
                yield (f"{node_name}/b{boundary}/{dom_name}/mivec",
                       mivec_gen(node, boundary, dom(2)), dom(2))


def check_errors():
    gen = complete_gen(pysgpp.getFirstTypeChebyshevNodeGenFunc(), 0, 2, 3, [(0, 1)] * 2)
    e = raises(RuntimeError, lambda: pysgpp.DeltasSparseGrid(gen))
    check(e is not None and "nested" in str(e), f"cheb1: RuntimeError expected, got {e!r}")
    gen = complete_gen(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 2, 3, [(0, 1)] * 2)
    dsg = pysgpp.DeltasSparseGrid(gen)
    point = pysgpp.DataVector([0.3, 0.6])
    for op_name, op in operators(point).items():
        for bad in (lambda p: [1.0, 2.0], lambda p: [1.0] * 4, lambda p: []):
            e = raises(ValueError, lambda: op(pysgpp.VectorSourceFunc(bad, 3), dsg))
            check(e is not None, f"{op_name}: wrong vector length -> ValueError")
        e = raises(TypeError, lambda: op(pysgpp.VectorSourceFunc(lambda p: 1.0, 3), dsg))
        check(e is not None, f"{op_name}: non-sequence -> TypeError")
        marker = ZeroDivisionError("marker")
        calls = [0]

        def failing(p):
            calls[0] += 1
            if calls[0] == 5:
                raise marker
            return f0(p)
        sf = pysgpp.SourceFunc(failing)
        e = raises(ZeroDivisionError, lambda: op(sf, dsg))
        check(e is marker, f"{op_name}: exception of the callable not propagated ({e!r})")
        compare(op(pysgpp.SourceFunc(f0), dsg), op(sf, dsg), 1.0,
                f"{op_name}: SourceFunc reusable after an exception")
        e = raises(TypeError, lambda: op(f0, dsg))
        check(e is not None, f"{op_name}: plain callable -> TypeError")
    check(raises(TypeError, lambda: pysgpp.DeltasSparseGrid(5)) is not None, "ctor(5) TypeError")


def check_lifetimes():
    nodes = pysgpp.getClenshawCurtisNodeGenFunc()
    ref_gen = mivec_gen(nodes, 0, [(0, 1)] * 2)
    ref = pysgpp.quadrature(pysgpp.SourceFunc(f0), pysgpp.SparseGrid(ref_gen))

    def build():
        return pysgpp.DeltasSparseGrid(mivec_gen(nodes, 0, [(0, 1)] * 2))
    dsg = build()
    for _ in range(3):
        gc.collect()
        [pysgpp.LvlMIVec([[7, 7], [1, 9]]) for _ in range(100)]  # reuse freed memory
    compare(ref, pysgpp.quadrature(pysgpp.SourceFunc(f0), dsg), 64.0,
            "keep-alive: DSG of a temporary MIVecSGGenInstr / LvlMIVec")
    check(dsg.getGenInstr().nDim() == 2, "keep-alive: getGenInstr")
    tg = dsg.getTensorGrid(1)
    mi = [tg.mi[k] for k in range(2)]
    del dsg
    gc.collect()
    check([tg.mi[k] for k in range(2)] == mi and tg.tensorGrid.nGP() > 0,
          "keep-alive: TensorGridCTData after the grid was deleted")
    # Reference count of the callable: +1 while a SourceFunc holds it, restored after del.
    func = lambda p: f0(p)  # noqa: E731
    count = sys.getrefcount(func)
    gen = complete_gen(nodes, 0, 2, 3, [(0, 1)] * 2)
    dsg = pysgpp.DeltasSparseGrid(gen)
    sf = pysgpp.SourceFunc(func)
    for op in operators(pysgpp.DataVector([0.2, 0.7])).values():
        op(sf, dsg)
    check(sys.getrefcount(func) == count + 1, "refcount +1 while held")
    del sf
    gc.collect()
    check(sys.getrefcount(func) == count, "refcount restored")
    # SparseGrid and DeltasSparseGrid are unrelated proxies: comparing them is not an error.
    check(not (dsg == pysgpp.SparseGrid(gen)), "DSG == SparseGrid is False")


def main():
    print(f"threads: {THREADS}")
    cached = cache_available()
    n_grids = 0
    for name, gen, domain in grid_matrix():
        sg = pysgpp.SparseGrid(gen)
        dsg = pysgpp.DeltasSparseGrid(gen)
        n_gp = check_grid_queries(name, gen, sg, dsg)
        for point in points_for(domain):
            check_scalar(name, gen, sg, dsg, point, n_gp)
            check_vector(name, gen, sg, dsg, point)
            if cached:
                check_cached(name, gen, sg, dsg, point)
        n_grids += 1
    check_errors()
    check_lifetimes()
    print(f"grids: {n_grids}; comparisons: {stats['cmp']}, bitwise equal {stats['bitwise']}, "
          f"max {stats['max_units']:.2f} units of eps*scale "
          f"(limit {TOL_UNITS:g})")
    print(f"RESULT: {'OK' if not failures else 'FAIL'} ({len(failures)} failures)")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
