# WP-27: checks of the Python bindings of the adaptive policy of the source-function cache.
# Run from the repo root after an SG_PYTHON=1 build:
#   LD_LIBRARY_PATH=lib PYTHONPATH=lib:lib/pysgpp .venv/bin/python \
#       combigrid/perf/results/WP-27/python_checks.py
import math
import os

import pysgpp


def func(x):
    return math.exp(-x[0] * x[0]) + math.sin(x[1] + x[2])


def grid():
    gen = pysgpp.CompleteSGGenInstr(5, 3)
    gen.setDomain([(-1.0, 1.0), (0.0, 2.0), (0.5, 1.5)])
    gen.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(1)
    return pysgpp.SparseGrid(gen)


def close(a, b):
    # Bitwise at 1 thread; at several threads the reduction order varies from call to call.
    return a == b if os.environ.get("OMP_NUM_THREADS") == "1" else abs(a - b) <= 1e-12 * abs(b)


def check(condition, what):
    print(("ok    " if condition else "FAIL  ") + what)
    if not condition:
        raise SystemExit(1)


def main():
    sg = grid()
    reference = pysgpp.quadrature(pysgpp.SourceFunc(func), sg)

    check(pysgpp.CachePolicy_Adaptive != pysgpp.CachePolicy_MemoryOnly,
          "enum CachePolicy_Adaptive")
    check(len({pysgpp.AdaptiveCacheMode_None, pysgpp.AdaptiveCacheMode_MemoryOnly,
               pysgpp.AdaptiveCacheMode_TemporaryStorage}) == 3, "enum AdaptiveCacheMode")
    config = pysgpp.SourceFuncCacheConfig()
    config.policy = pysgpp.CachePolicy_Adaptive

    calls = [0]

    def counted(x):
        calls[0] += 1
        return func(x)

    f = pysgpp.CachedSourceFunc(counted, config)
    stats = f.statistics()
    check(stats.adaptiveMode == pysgpp.AdaptiveCacheMode_MemoryOnly and stats.modeSwitches == 0,
          "starts in MemoryOnly")
    cold = pysgpp.quadrature(f, sg)
    check(close(cold, reference), "adaptive quadrature equals the uncached one")
    stats = f.statistics()
    check(stats.adaptiveMode == pysgpp.AdaptiveCacheMode_MemoryOnly,
          "a Python callable stays cached (function %.0f ns, hit %.0f ns)"
          % (stats.functionCostNs, stats.memoryHitCostNs))
    check(stats.functionCostNs > stats.memoryHitCostNs > 0, "costs measured")
    check(calls[0] == stats.lookups - stats.hits, "Python called on misses only (%d)" % calls[0])
    before = calls[0]
    point = pysgpp.DataVector([0.1, 1.2, 0.9])
    value = pysgpp.interpolate(f, point, sg)
    check(close(value, pysgpp.interpolate(pysgpp.SourceFunc(func), point, sg)),
          "interpolation equals the uncached one")
    check(calls[0] == before, "interpolation after the quadrature: all hits")

    vconfig = pysgpp.SourceFuncCacheConfig()
    vconfig.policy = pysgpp.CachePolicy_Adaptive
    g = pysgpp.CachedVectorSourceFunc(lambda x: [func(x), 2.0 * func(x)], 2, vconfig)
    values = pysgpp.quadrature(g, sg)
    check(close(values[0], reference) and close(values[1], 2.0 * reference),
          "vector-valued adaptive quadrature")

    f.cache().clear()
    stats = f.statistics()
    check(stats.lookups == 0 and stats.functionCostNs == 0.0, "clear() resets the measurements")
    print("all checks passed")


if __name__ == "__main__":
    main()
