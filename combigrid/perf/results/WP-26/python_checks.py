#!/usr/bin/env python3
"""WP-26: checks of the Python bindings of the source-function cache.

Usage (from the repo root, through the repo's .venv):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=<t> \
      .venv/bin/python combigrid/perf/results/WP-26/python_checks.py

- Results: CachedSourceFunc / CachedVectorSourceFunc give the results of SourceFunc /
  VectorSourceFunc for quadrature, interpolate and interpolateLinear (bitwise at one thread, else
  within round-off), cold and warm; the Python callable is called on misses only.
- Cross-operator reuse on a shifted domain: interpolate after quadrature calls Python 0 times.
- Constructors, CacheConfig, statistics, sharing one SourceFuncCache, cache().
- Exceptions inside the callable are re-raised after the operator, failed points are not cached
  (retried next call); wrong vector length -> ValueError; misuse -> RuntimeError.
- Reference counts of the callable and lifetime of a shared cache.
- Built without the libraries of the cache (SourceFuncCache.isAvailable() is False): every
  constructor raises RuntimeError.
Exit code 1 if any check fails.
"""
import gc
import math
import os
import sys

import pysgpp

failures = []
THREADS = int(os.environ.get("OMP_NUM_THREADS", "0") or 0)


def check(cond, msg):
    if not cond:
        failures.append(msg)
        print("FAIL:", msg)


def tolist(v):
    return [v[i] for i in range(len(v))]


def same(a, b):
    """Bitwise at one thread, else within round-off (reduction order varies)."""
    if THREADS == 1:
        return a == b
    return abs(a - b) <= 1e-12 * max(1.0, abs(a), abs(b))


def make_grid(nodes, boundary, n_dim, level, domain):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain(domain)
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen, pysgpp.SparseGrid(gen)


class Counted:
    """Python callable counting its calls."""

    def __init__(self, fail_below=None):
        self.calls = 0
        self.fail_below = fail_below

    def __call__(self, p):
        self.calls += 1
        if self.fail_below is not None and p[0] < self.fail_below:
            raise KeyError("expected failure")
        s = 0.0
        for j, x in enumerate(p):
            s += 0.3 * (j + 1) * x
        return math.cos(s) + 0.1 * sum(x * x for x in p)


def vector_f(p):
    s = sum(p)
    return [math.cos(s), math.sin(s), s]


def domains(n_dim):
    return [[(0.0, 1.0)] * n_dim, [(-0.5 + 0.1 * k, 1.0 + 0.2 * k) for k in range(n_dim)]]


def points(domain):
    pts = []
    for t in (0.0, 0.37, 1.0):
        pts.append(pysgpp.DataVector([lo + ((t + 0.11 * k) % 1.0) * (hi - lo)
                                      for k, (lo, hi) in enumerate(domain)]))
    return pts


def run_scalar(f, grid, pts):
    r = [pysgpp.quadrature(f, grid)]
    for p in pts:
        r.append(pysgpp.interpolate(f, p, grid))
        r.append(pysgpp.interpolateLinear(f, p, grid))
    return r


def run_vector(f, grid, pts):
    r = tolist(pysgpp.quadrature(f, grid))
    for p in pts:
        r += tolist(pysgpp.interpolate(f, p, grid))
        r += tolist(pysgpp.interpolateLinear(f, p, grid))
    return r


def check_results():
    for nodes in (pysgpp.getEquidistantNodeGenFunc(), pysgpp.getClenshawCurtisNodeGenFunc()):
        for boundary in (0, 1):
            for n_dim in (1, 3):
                for domain in domains(n_dim):
                    tag = f"b{boundary} d{n_dim} {domain[0]}"
                    _, grid = make_grid(nodes, boundary, n_dim, 3, domain)
                    pts = points(domain)
                    plain_f = Counted()
                    plain = run_scalar(pysgpp.SourceFunc(plain_f), grid, pts)
                    counted = Counted()
                    cached_f = pysgpp.CachedSourceFunc(counted)
                    cold = run_scalar(cached_f, grid, pts)
                    misses_cold = counted.calls
                    warm = run_scalar(cached_f, grid, pts)
                    stats = cached_f.statistics()
                    check(all(same(a, b) for a, b in zip(plain, cold)), f"scalar cold {tag}")
                    check(all(same(a, b) for a, b in zip(plain, warm)), f"scalar warm {tag}")
                    check(counted.calls == misses_cold, f"warm calls Python {tag}")
                    check(stats.lookups == 2 * plain_f.calls, f"lookups {tag}")
                    check(stats.lookups - stats.hits == counted.calls, f"misses {tag}")
                    check(stats.operatorCalls == 2 * (1 + 2 * len(pts)), f"operator calls {tag}")

                    plain_v = run_vector(pysgpp.VectorSourceFunc(vector_f, 3), grid, pts)
                    cached_v = pysgpp.CachedVectorSourceFunc(vector_f, 3)
                    cold_v = run_vector(cached_v, grid, pts)
                    warm_v = run_vector(cached_v, grid, pts)
                    check(all(same(a, b) for a, b in zip(plain_v, cold_v)), f"vector cold {tag}")
                    check(all(same(a, b) for a, b in zip(plain_v, warm_v)), f"vector warm {tag}")
                    check(cached_v.cache().nOutputs() == 3, f"vector nOutputs {tag}")


def check_cross_operator():
    domain = domains(3)[1]
    _, grid = make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 3, 4, domain)
    counted = Counted()
    f = pysgpp.CachedSourceFunc(counted)
    pysgpp.quadrature(f, grid)
    after_quadrature = counted.calls
    for p in points(domain):
        pysgpp.interpolate(f, p, grid)
        pysgpp.interpolateLinear(f, p, grid)
    check(counted.calls == after_quadrature, "cross-operator: interpolation after quadrature "
          f"called Python {counted.calls - after_quadrature} times")


def check_api():
    cfg = pysgpp.CacheConfig()
    check(cfg.memoryLimit == 2 ** 30, "default memory limit 1 GiB")
    cfg.memoryLimit = 0
    cache = pysgpp.SourceFuncCache(cfg)
    check(cache.config().memoryLimit == 0, "config of the cache")
    check(cache.nOutputs() == 0, "nOutputs before use")
    counted = Counted()
    f = pysgpp.CachedSourceFunc(counted, cache)
    g = pysgpp.CachedSourceFunc(counted, pysgpp.CacheConfig())
    check(isinstance(f, pysgpp.SourceFunc), "CachedSourceFunc is a SourceFunc")
    check(isinstance(pysgpp.CachedVectorSourceFunc(vector_f, 3), pysgpp.VectorSourceFunc),
          "CachedVectorSourceFunc is a VectorSourceFunc")
    p = pysgpp.DataVector([0.2, 0.4])
    f.evaluate(p)
    f.evaluate(p)
    check(counted.calls == 2 and f.statistics().rejectedInserts == 2, "limit 0: nothing stored")
    g.evaluate(p)
    g.evaluate(p)
    check(counted.calls == 3 and g.statistics().hits == 1, "evaluate uses the cache")
    g.cache().clear()
    check(g.statistics().entries == 0, "clear")

    shared = pysgpp.SourceFuncCache()
    a = pysgpp.CachedSourceFunc(counted, shared)
    b = pysgpp.CachedSourceFunc(counted, shared)
    calls = counted.calls
    a.evaluate(p)
    b.evaluate(p)
    check(counted.calls == calls + 1 and shared.statistics().hits == 1, "shared cache")
    check(a.cache().statistics().entries == 1, "cache() returns the shared cache")


def check_errors():
    _, grid = make_grid(pysgpp.getEquidistantNodeGenFunc(), 0, 2, 3, domains(2)[1])
    failing = Counted(fail_below=0.0)  # raises for points with x0 < 0 (part of the domain)
    f = pysgpp.CachedSourceFunc(failing)
    for attempt in range(2):
        try:
            pysgpp.quadrature(f, grid)
            check(False, f"exception not re-raised (attempt {attempt})")
        except KeyError:
            pass
    # After the first exception the holder returns NaN without calling Python; the cache must not
    # store these NaNs: the lower domain corner (a grid point, x0 < 0) raises again instead of
    # returning a cached NaN, a valid stored point is still a hit.
    stats = f.statistics()
    check(stats.entries < stats.lookups, "failed points are not stored")
    try:
        f.evaluate(pysgpp.DataVector([-0.5, -0.4]))
        check(False, "evaluate of a failed grid point: exception not re-raised (cached NaN?)")
    except KeyError:
        pass
    good = pysgpp.DataVector([1.0, 1.2])  # upper domain corner, a grid point
    calls = failing.calls
    check(math.isfinite(f.evaluate(good)) and f.evaluate(good) == f.evaluate(good),
          "valid point after errors")
    check(failing.calls in (calls, calls + 1), "valid point evaluated at most once, then cached")

    bad = pysgpp.CachedVectorSourceFunc(lambda p: [1.0, 2.0], 3)
    try:
        pysgpp.quadrature(bad, grid)
        check(False, "wrong vector length not reported")
    except ValueError:
        pass
    check(bad.statistics().entries == 0, "NaN results of a failed vector call not stored")

    scalar_cache = pysgpp.SourceFuncCache()
    pysgpp.CachedSourceFunc(Counted(), scalar_cache)
    for label, make in (
            ("other number of outputs",
             lambda: pysgpp.CachedVectorSourceFunc(vector_f, 3, scalar_cache)),
            ("None cache", lambda: pysgpp.CachedSourceFunc(Counted(), None)),
            ("nOutputs 0", lambda: pysgpp.CachedVectorSourceFunc(vector_f, 0))):
        try:
            make()
            check(False, f"misuse not rejected: {label}")
        except RuntimeError:
            pass
    try:
        pysgpp.CachedSourceFunc(42)
        check(False, "non-callable accepted")
    except (TypeError, ValueError, RuntimeError):
        pass


def check_lifetime():
    func = Counted()
    before = sys.getrefcount(func)
    cache = pysgpp.SourceFuncCache()
    fs = [pysgpp.CachedSourceFunc(func, cache) for _ in range(10)]
    check(sys.getrefcount(func) == before + 10, "one reference per cached function")
    del fs
    gc.collect()
    check(sys.getrefcount(func) == before, "references released")

    f = pysgpp.CachedSourceFunc(func, cache)
    del cache
    gc.collect()
    p = pysgpp.DataVector([0.1, 0.9])
    f.evaluate(p)
    f.evaluate(p)
    check(f.cache().statistics().hits == 1, "cache kept alive by the function")
    keep = f.cache()
    del f
    gc.collect()
    check(keep.statistics().entries == 1, "cache outlives the function")


def check_unavailable():
    """Without the libraries of the cache, every constructor raises RuntimeError."""
    for label, make in (("SourceFuncCache", lambda: pysgpp.SourceFuncCache()),
                        ("CachedSourceFunc", lambda: pysgpp.CachedSourceFunc(Counted())),
                        ("CachedVectorSourceFunc",
                         lambda: pysgpp.CachedVectorSourceFunc(vector_f, 3))):
        try:
            make()
            check(False, f"{label} created although the cache is not available")
        except RuntimeError:
            pass


def main():
    available = pysgpp.SourceFuncCache.isAvailable()
    print(f"pysgpp: {pysgpp.__file__}, OMP_NUM_THREADS={THREADS or 'default'}, "
          f"cache available: {available}")
    checks = ((check_results, check_cross_operator, check_api, check_errors, check_lifetime)
              if available else (check_unavailable,))
    for check_fn in checks:
        n = len(failures)
        check_fn()
        print(f"{check_fn.__name__}: {'OK' if len(failures) == n else 'FAILED'}")
    print("ALL OK" if not failures else f"{len(failures)} FAILURES")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
