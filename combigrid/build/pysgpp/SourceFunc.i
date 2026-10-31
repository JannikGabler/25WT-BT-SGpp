// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Python bindings for sgpp::combigrid::SourceFunc.
//
// In C++, a SourceFunc wraps a std::function<double(const base::DataVector&)>. From Python it is
// constructed from any Python callable instead:
//
//     def f(point):            # point is a tuple of floats (one entry per dimension)
//         return point[0] ** 2
//     sourceFunc = pysgpp.SourceFunc(f)
//
// This follows the callback approach of base's OperationQuadratureMC.i (a Python callable held as
// PyObject*, called with a tuple of coordinates while holding the GIL). Differences to that file
// are due to how the CT operators call the function:
//
// 1. Threads: interpolate / interpolateLinear / quadrature evaluate the source function inside
//    OpenMP parallel regions, and pysgpp is built with 'swig -threads', which releases the GIL for
//    the duration of every wrapped call. Every call into Python therefore acquires the GIL itself
//    (PyGILState_Ensure). As a consequence, Python source functions are evaluated serially.
//
// 2. Exceptions: a Python exception cannot be propagated through an OpenMP region. The first
//    exception raised by the callable is stored, the failing evaluation and all subsequent ones
//    return NaN without calling Python again, and the stored exception is re-raised as soon as the
//    wrapped C++ call (operator or SourceFunc::evaluate*) returns to Python. See
//    COMBIGRID_SOURCE_FUNC_EXCEPTION below.
//
// 3. Lifetime: the Python callable is reference-counted by a PyCallableHolder that is shared by all
//    copies of the std::function (and thus of the SourceFunc). Because SourceFunc::func is private,
//    the wrapper keeps a registry SourceFunc* -> holder, so that a stored exception can be found
//    again after an operator call; entries are removed in the (extended) destructor.
//
// The vector-valued VectorSourceFunc (VectorSourceFunc.i) uses the same holder (callVector), its
// own registry VectorSourceFunc* -> holder and the same exception handler. The cached variants
// (CachedSourceFunc.i) are registered under their base class pointer.

%{
#include <algorithm>
#include <atomic>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <span>
#include <unordered_map>

namespace sgpp {
namespace combigrid {
namespace python {

// Owns a reference to a Python callable and the first exception it raised.
class PyCallableHolder {
 public:
  explicit PyCallableHolder(PyObject* callable) : callable_(callable) {
    // The constructor wrapper may run with the GIL released ('swig -threads').
    PyGILState_STATE gil = PyGILState_Ensure();
    Py_INCREF(callable_);
    PyGILState_Release(gil);
  }

  ~PyCallableHolder() {
    // If the interpreter is already finalised, leak the references instead of crashing.
    if (!Py_IsInitialized()) {
      return;
    }
    PyGILState_STATE gil = PyGILState_Ensure();
    Py_XDECREF(callable_);
    Py_XDECREF(errType_);
    Py_XDECREF(errValue_);
    Py_XDECREF(errTraceback_);
    PyGILState_Release(gil);
  }

  PyCallableHolder(const PyCallableHolder&) = delete;
  PyCallableHolder& operator=(const PyCallableHolder&) = delete;

  // Calls the Python callable with the coordinates of point as a tuple of floats and returns the
  // float it returns (NaN if the call fails). May be called from any (OpenMP) thread, with or
  // without the GIL.
  double call(const sgpp::base::DataVector& point) {
    double value = 0.0;
    const bool ok = invoke(point, [&value](PyObject* ret) { value = PyFloat_AsDouble(ret); });
    return ok ? value : std::numeric_limits<double>::quiet_NaN();
  }

  // Calls the Python callable like call and writes the values of the sequence it returns to out
  // (all NaN if the call fails). A result that is not a sequence of exactly out.size() numbers
  // raises a Python exception (ValueError for a wrong length, TypeError otherwise), which is
  // recorded like an exception of the callable itself.
  void callVector(const sgpp::base::DataVector& point, std::span<double> out) {
    if (!invoke(point, [out](PyObject* ret) { storeSequence(ret, out); })) {
      std::ranges::fill(out, std::numeric_limits<double>::quiet_NaN());
    }
  }

  // Moves a recorded exception into the Python error indicator and resets the holder.
  // Must be called with the GIL held. Returns true iff an exception was restored.
  bool restorePendingError() {
    if (!failed_.load()) {
      return false;
    }
    PyErr_Restore(errType_, errValue_, errTraceback_);  // steals all three references
    errType_ = nullptr;
    errValue_ = nullptr;
    errTraceback_ = nullptr;
    failed_.store(false);
    return true;
  }

 private:
  // Calls the Python callable with point as a tuple of floats and passes its return value to
  // convert (which may set a Python exception). Returns false iff the call failed, i.e. an
  // exception has been recorded now or before.
  template <typename Convert>
  bool invoke(const sgpp::base::DataVector& point, Convert convert) {
    // Fast path without the GIL once an exception has been recorded.
    if (failed_.load()) {
      return false;
    }

    PyGILState_STATE gil = PyGILState_Ensure();
    bool ok = false;

    // Re-check under the GIL: another thread may have failed in the meantime.
    if (!failed_.load()) {
      const Py_ssize_t n = static_cast<Py_ssize_t>(point.getSize());
      PyObject* coords = PyTuple_New(n);

      for (Py_ssize_t i = 0; (coords != nullptr) && (i < n); i++) {
        PyObject* coord = PyFloat_FromDouble(point[static_cast<size_t>(i)]);
        if (coord == nullptr) {
          Py_CLEAR(coords);
          break;
        }
        PyTuple_SET_ITEM(coords, i, coord);  // steals the reference to coord
      }

      if (coords != nullptr) {
        PyObject* ret = PyObject_CallFunctionObjArgs(callable_, coords, nullptr);
        Py_DECREF(coords);

        if (ret != nullptr) {
          convert(ret);
          Py_DECREF(ret);
        }
      }

      if (PyErr_Occurred() != nullptr) {
        PyErr_Fetch(&errType_, &errValue_, &errTraceback_);
        failed_.store(true);
      } else {
        ok = true;
      }
    }

    PyGILState_Release(gil);
    return ok;
  }

  // Writes the numbers of the sequence ret to out; sets a Python exception if ret is not a
  // sequence of exactly out.size() numbers. Must be called with the GIL held.
  static void storeSequence(PyObject* ret, std::span<double> out) {
    // A new reference to ret itself for lists and tuples; other iterables are copied to a list.
    PyObject* seq = PySequence_Fast(ret, "VectorSourceFunc: the callable must return a sequence");
    if (seq == nullptr) {
      return;
    }

    const Py_ssize_t n = PySequence_Fast_GET_SIZE(seq);
    if (static_cast<size_t>(n) != out.size()) {
      PyErr_Format(PyExc_ValueError,
                   "VectorSourceFunc: the callable returned %zd values instead of nOutputs = %zu",
                   n, out.size());
    } else {
      PyObject** items = PySequence_Fast_ITEMS(seq);
      for (Py_ssize_t i = 0; i < n; i++) {
        const double value = PyFloat_AsDouble(items[i]);
        if ((value == -1.0) && (PyErr_Occurred() != nullptr)) {
          break;
        }
        out[static_cast<size_t>(i)] = value;
      }
    }
    Py_DECREF(seq);
  }

  PyObject* callable_;
  std::atomic<bool> failed_{false};
  PyObject* errType_ = nullptr;
  PyObject* errValue_ = nullptr;
  PyObject* errTraceback_ = nullptr;
};

// The callable object stored inside the SourceFunc's std::function.
struct PyCallableFunctor {
  std::shared_ptr<PyCallableHolder> holder;

  double operator()(const sgpp::base::DataVector& point) const { return holder->call(point); }
};

// The callable object stored inside the VectorSourceFunc's std::function.
struct PyVectorCallableFunctor {
  std::shared_ptr<PyCallableHolder> holder;

  void operator()(const sgpp::base::DataVector& point, std::span<double> out) const {
    holder->callVector(point, out);
  }
};

// Registries Func* -> holder for Func = SourceFunc and VectorSourceFunc (see point 3 at the top of
// SourceFunc.i), guarded by one mutex.
std::mutex& sourceFuncRegistryMutex() {
  static std::mutex mutex;
  return mutex;
}

template <typename Func>
std::unordered_map<const Func*, std::shared_ptr<PyCallableHolder>>& sourceFuncRegistry() {
  static std::unordered_map<const Func*, std::shared_ptr<PyCallableHolder>> registry;
  return registry;
}

template <typename Func>
void registerSourceFunc(const Func* sourceFunc, std::shared_ptr<PyCallableHolder> holder) {
  std::lock_guard<std::mutex> lock(sourceFuncRegistryMutex());
  sourceFuncRegistry<Func>()[sourceFunc] = std::move(holder);
}

template <typename Func>
void unregisterSourceFunc(const Func* sourceFunc) {
  std::lock_guard<std::mutex> lock(sourceFuncRegistryMutex());
  sourceFuncRegistry<Func>().erase(sourceFunc);
}

// Re-raises the exception recorded for sourceFunc (a SourceFunc or VectorSourceFunc), if any. Must
// be called with the GIL held. Returns true iff a Python exception is now set.
template <typename Func>
bool raisePendingSourceFuncError(const Func* sourceFunc) {
  std::shared_ptr<PyCallableHolder> holder;
  {
    std::lock_guard<std::mutex> lock(sourceFuncRegistryMutex());
    auto it = sourceFuncRegistry<Func>().find(sourceFunc);
    if (it != sourceFuncRegistry<Func>().end()) {
      holder = it->second;
    }
  }
  return (holder != nullptr) && holder->restorePendingError();
}

}  // namespace python
}  // namespace combigrid
}  // namespace sgpp
%}

// Exception handler for every wrapped function that evaluates a SourceFunc or VectorSourceFunc
// passed as its first argument (arg1; for the classes' own methods, arg1 is 'this'; the registry
// is chosen by the type of arg1, so one handler serves the scalar and the vector overloads of an
// operator). Replaces the global handler of
// pysgpp.i for these functions: C++ exceptions are translated to RuntimeError as usual, and an
// exception raised by the Python callable during the call is re-raised afterwards.
// $action contains SWIG's RAII GIL release, so the GIL is held again when the check runs.
%define COMBIGRID_SOURCE_FUNC_EXCEPTION(function)
%exception function {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
  if (sgpp::combigrid::python::raisePendingSourceFuncError(arg1)) {
    SWIG_fail;
  }
}
%enddef

// The std::function constructors cannot be called from Python; they are replaced by the
// PyObject* constructor below. (The parameter name 'pyfunc' activates the callable-checking
// 'in' typemap defined in base/build/pysgpp/OperationQuadratureMC.i.)
%ignore sgpp::combigrid::SourceFunc::SourceFunc(const std::function<double(const base::DataVector&)>&);
%ignore sgpp::combigrid::SourceFunc::SourceFunc(std::function<double(const base::DataVector&)>&&);

%extend sgpp::combigrid::SourceFunc {
  SourceFunc(PyObject* pyfunc) {
    auto holder = std::make_shared<sgpp::combigrid::python::PyCallableHolder>(pyfunc);
    std::function<double(const sgpp::base::DataVector&)> func(
        sgpp::combigrid::python::PyCallableFunctor{holder});
    auto* sourceFunc = new sgpp::combigrid::SourceFunc(std::move(func));
    sgpp::combigrid::python::registerSourceFunc(sourceFunc, std::move(holder));
    return sourceFunc;
  }

  ~SourceFunc() {
    sgpp::combigrid::python::unregisterSourceFunc($self);
    delete $self;
  }
}

// The end-of-operator hook (e.g. of a CachedSourceFunc) is an internal step of the operators.
%ignore sgpp::combigrid::SourceFunc::finishOperatorCall;

COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::SourceFunc::evaluate)
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::SourceFunc::evaluateNormalizedInPlace)
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::SourceFunc::evaluateNormalizedOutOfPlace)

%include "combigrid/src/sgpp/combigrid/functions/source_functions/source_function.hpp"
