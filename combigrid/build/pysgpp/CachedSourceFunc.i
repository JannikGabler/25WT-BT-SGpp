// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Python bindings for the source-function cache: sgpp::combigrid::SourceFuncCacheConfig,
// CacheStatistics, SourceFuncCache, CachedSourceFunc and CachedVectorSourceFunc. Always wrapped; if
// SG++ was built without Boost >= 1.81 or xxHash >= 0.8, the constructors raise RuntimeError
// (base::dependency_exception, the message names the reason).
//
//     cache = pysgpp.SourceFuncCache()                    # or SourceFuncCache(config)
//     f = pysgpp.CachedSourceFunc(func, cache)            # or CachedSourceFunc(func[, config])
//     pysgpp.quadrature(f, sparseGrid)                    # a CachedSourceFunc is a SourceFunc
//     pysgpp.interpolate(f, point, sparseGrid)            # values of the quadrature are hits
//     f.statistics().hits
//     g = pysgpp.CachedVectorSourceFunc(vfunc, m[, cache | config])
//     config = pysgpp.SourceFuncCacheConfig()             # values kept in a file across runs
//     config.policy = pysgpp.CachePolicy_PersistentStorage
//     config.file = "values.cache"
//     f = pysgpp.CachedSourceFunc(func, config)
//     config = pysgpp.SourceFuncCacheConfig()             # caches only if it pays (WP-27)
//     config.policy = pysgpp.CachePolicy_Adaptive
//
// The storage policies need LMDB at build time (else RuntimeError, base::dependency_exception).
//
// The Python callable is held exactly as for SourceFunc / VectorSourceFunc (SourceFunc.i); the
// cache sits in front of it in C++, so hits neither call Python nor take the GIL. An evaluation
// that raises returns NaN, which the cache does not store, and the exception is re-raised after
// the call: the cached functions are registered under their SourceFunc / VectorSourceFunc base
// (the operators and the inherited evaluate receive the base pointer, which has the same
// address). The cache is wrapped as std::shared_ptr proxy (%shared_ptr in combigrid.i): it lives
// as long as a Python reference or a cached function uses it.

// The end-of-operator hook is an internal step of the operators.
%ignore sgpp::combigrid::SourceFuncCache::finishOperatorCall;
%ignore sgpp::combigrid::CachedSourceFunc::finishOperatorCall;
%ignore sgpp::combigrid::CachedVectorSourceFunc::finishOperatorCall;

// CachePolicy is an enum class: pysgpp.CachePolicy_MemoryOnly, CachePolicy_TemporaryStorage,
// CachePolicy_PersistentStorage, CachePolicy_Adaptive (WP-27; the current mode of an adaptive
// cache: statistics().adaptiveMode == pysgpp.AdaptiveCacheMode_None / _MemoryOnly /
// _TemporaryStorage, plus the measured costs functionCostNs, memoryHitCostNs, ...). The config's
// file is a Python str; clear(deletePersistentFile) and flush() are wrapped as declared (file
// errors raise RuntimeError, base::file_exception).
// 'None' is a Python keyword, so SWIG would name the enumerator AdaptiveCacheMode__None; the
// flattened attribute pysgpp.AdaptiveCacheMode_None is none. The keyword rule (pythonkw.swg) is
// lifted for this header only and restored afterwards.
%namewarn("") "None";
%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp"
%keywordwarn("'None' is a python keyword", rename="_%s") "None";
%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/cache_policy.hpp"
%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp"
%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp"

%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp"

// The std::function constructors cannot be called from Python; they are replaced by the
// PyObject* constructors below ('pyfunc' activates the callable-checking 'in' typemap, see
// SourceFunc.i). The signatures include the default arguments, so that the overloads SWIG
// generates for them are ignored as well. A null cache (None) or a cache of a function with
// another number of outputs raises RuntimeError (std::invalid_argument).
%ignore sgpp::combigrid::CachedSourceFunc::CachedSourceFunc(
    std::function<double(const base::DataVector&)>, const std::shared_ptr<SourceFuncCache>&);
%ignore sgpp::combigrid::CachedSourceFunc::CachedSourceFunc(
    std::function<double(const base::DataVector&)>,
    const SourceFuncCacheConfig& config = SourceFuncCacheConfig());
%ignore sgpp::combigrid::CachedVectorSourceFunc::CachedVectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)>, size_t,
    const std::shared_ptr<SourceFuncCache>&);
%ignore sgpp::combigrid::CachedVectorSourceFunc::CachedVectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)>, size_t,
    const SourceFuncCacheConfig& config = SourceFuncCacheConfig());

%extend sgpp::combigrid::CachedSourceFunc {
  CachedSourceFunc(PyObject* pyfunc, const std::shared_ptr<sgpp::combigrid::SourceFuncCache>& cache) {
    auto holder = std::make_shared<sgpp::combigrid::python::PyCallableHolder>(pyfunc);
    auto* sourceFunc = new sgpp::combigrid::CachedSourceFunc(
        sgpp::combigrid::python::PyCallableFunctor{holder}, cache);
    sgpp::combigrid::python::registerSourceFunc(
        static_cast<const sgpp::combigrid::SourceFunc*>(sourceFunc), std::move(holder));
    return sourceFunc;
  }

  CachedSourceFunc(PyObject* pyfunc, const sgpp::combigrid::SourceFuncCacheConfig& config =
                                         sgpp::combigrid::SourceFuncCacheConfig()) {
    auto holder = std::make_shared<sgpp::combigrid::python::PyCallableHolder>(pyfunc);
    auto* sourceFunc = new sgpp::combigrid::CachedSourceFunc(
        sgpp::combigrid::python::PyCallableFunctor{holder}, config);
    sgpp::combigrid::python::registerSourceFunc(
        static_cast<const sgpp::combigrid::SourceFunc*>(sourceFunc), std::move(holder));
    return sourceFunc;
  }

  ~CachedSourceFunc() {
    sgpp::combigrid::python::unregisterSourceFunc(
        static_cast<const sgpp::combigrid::SourceFunc*>($self));
    delete $self;
  }
}

%extend sgpp::combigrid::CachedVectorSourceFunc {
  CachedVectorSourceFunc(PyObject* pyfunc, size_t nOutputs,
                         const std::shared_ptr<sgpp::combigrid::SourceFuncCache>& cache) {
    auto holder = std::make_shared<sgpp::combigrid::python::PyCallableHolder>(pyfunc);
    auto* sourceFunc = new sgpp::combigrid::CachedVectorSourceFunc(
        sgpp::combigrid::python::PyVectorCallableFunctor{holder}, nOutputs, cache);
    sgpp::combigrid::python::registerSourceFunc(
        static_cast<const sgpp::combigrid::VectorSourceFunc*>(sourceFunc), std::move(holder));
    return sourceFunc;
  }

  CachedVectorSourceFunc(PyObject* pyfunc, size_t nOutputs,
                         const sgpp::combigrid::SourceFuncCacheConfig& config =
                             sgpp::combigrid::SourceFuncCacheConfig()) {
    auto holder = std::make_shared<sgpp::combigrid::python::PyCallableHolder>(pyfunc);
    auto* sourceFunc = new sgpp::combigrid::CachedVectorSourceFunc(
        sgpp::combigrid::python::PyVectorCallableFunctor{holder}, nOutputs, config);
    sgpp::combigrid::python::registerSourceFunc(
        static_cast<const sgpp::combigrid::VectorSourceFunc*>(sourceFunc), std::move(holder));
    return sourceFunc;
  }

  ~CachedVectorSourceFunc() {
    sgpp::combigrid::python::unregisterSourceFunc(
        static_cast<const sgpp::combigrid::VectorSourceFunc*>($self));
    delete $self;
  }
}

%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp"
%include "combigrid/src/sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp"
