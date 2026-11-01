# WP-28: checks of the Python bindings of the storage policies of the source-function cache.
# Run from the repo root after an SG_PYTHON=1 build:
#   LD_LIBRARY_PATH=lib PYTHONPATH=lib:lib/pysgpp .venv/bin/python \
#       combigrid/perf/results/WP-28/python_checks.py
import math
import os
import tempfile

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
    directory = tempfile.mkdtemp(prefix="sgpp-wp28-py-")
    path = os.path.join(directory, "values.cache")

    config = pysgpp.SourceFuncCacheConfig()
    check(config.policy == pysgpp.CachePolicy_MemoryOnly and config.file == "",
          "defaults: MemoryOnly, empty file")
    config.policy = pysgpp.CachePolicy_PersistentStorage
    config.file = path

    calls = [0]

    def counted(x):
        calls[0] += 1
        return func(x)

    f = pysgpp.CachedSourceFunc(counted, config)
    cold = pysgpp.quadrature(f, sg)
    stats = f.statistics()
    check(close(cold, reference), "persistent cold run equals the uncached quadrature")
    check(stats.diskEntries == stats.entries > 0 and stats.fileBytes > 0,
          "file holds every point after the operator (%d)" % stats.diskEntries)
    try:
        pysgpp.SourceFuncCache(config)
        check(False, "second cache on the locked file raises")
    except RuntimeError as error:
        check("locked" in str(error), "second cache on the locked file raises: %s" % error)
    del f

    calls[0] = 0
    g = pysgpp.CachedSourceFunc(counted, config)
    check(g.statistics().loadedEntries == stats.entries, "reloaded every point")
    check(close(pysgpp.quadrature(g, sg), reference) and calls[0] == 0,
          "warm run: same result, no Python call")
    g.cache().clear()
    check(g.statistics().entries == 0 and g.statistics().diskEntries == stats.entries,
          "clear() keeps the file")
    g.cache().clear(True)
    check(g.statistics().diskEntries == 0, "clear(True) empties the file")
    g.cache().flush()
    try:
        pysgpp.CachedVectorSourceFunc(lambda x, out: None, 2, g.cache())
        check(False, "another number of outputs raises")
    except RuntimeError:
        check(True, "another number of outputs raises")
    del g

    temporary = pysgpp.SourceFuncCacheConfig()
    temporary.policy = pysgpp.CachePolicy_TemporaryStorage
    temporary.memoryLimit = 0
    t = pysgpp.CachedSourceFunc(func, temporary)
    check(t.cache().config().file.startswith("/proc/self/fd/"),
          "temporary file: %s" % t.cache().config().file)
    check(close(pysgpp.quadrature(t, sg), reference) and close(pysgpp.quadrature(t, sg), reference),
          "temporary storage (memory limit 0): same results")
    check(t.statistics().diskHits > 0 and t.statistics().entries == 0, "hits come from disk")

    bad = pysgpp.SourceFuncCacheConfig()
    bad.policy = pysgpp.CachePolicy_PersistentStorage
    try:
        pysgpp.SourceFuncCache(bad)
        check(False, "persistent without file raises")
    except RuntimeError:
        check(True, "persistent without file raises")
    with open(os.path.join(directory, "foreign"), "w") as handle:
        handle.write("x" * 20000)
    bad.file = os.path.join(directory, "foreign")
    try:
        pysgpp.SourceFuncCache(bad)
        check(False, "foreign file raises")
    except RuntimeError as error:
        check("no file of a source-function cache" in str(error), "foreign file raises: %s" % error)

    for name in os.listdir(directory):
        os.remove(os.path.join(directory, name))
    os.rmdir(directory)
    print("all checks passed")


if __name__ == "__main__":
    main()
