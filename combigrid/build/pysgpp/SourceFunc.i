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

%{
#include <atomic>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
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

  // Calls the Python callable with the coordinates of point as a tuple of floats.
  // May be called from any (OpenMP) thread, with or without the GIL.
  double call(const sgpp::base::DataVector& point) {
    const double nan = std::numeric_limits<double>::quiet_NaN();

    // Fast path without the GIL once an exception has been recorded.
    if (failed_.load()) {
      return nan;
    }

    PyGILState_STATE gil = PyGILState_Ensure();
    double result = nan;

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
          const double value = PyFloat_AsDouble(ret);
          Py_DECREF(ret);
          if (PyErr_Occurred() == nullptr) {
            result = value;
          }
        }
      }

      if (PyErr_Occurred() != nullptr) {
        PyErr_Fetch(&errType_, &errValue_, &errTraceback_);
        failed_.store(true);
      }
    }

    PyGILState_Release(gil);
    return result;
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

// Registry SourceFunc* -> holder (see point 3 at the top of SourceFunc.i).
std::mutex& sourceFuncRegistryMutex() {
  static std::mutex mutex;
  return mutex;
}

std::unordered_map<const sgpp::combigrid::SourceFunc*, std::shared_ptr<PyCallableHolder>>&
sourceFuncRegistry() {
  static std::unordered_map<const sgpp::combigrid::SourceFunc*, std::shared_ptr<PyCallableHolder>>
      registry;
  return registry;
}

// Re-raises the exception recorded for sourceFunc, if any. Must be called with the GIL held.
// Returns true iff a Python exception is now set.
bool raisePendingSourceFuncError(const sgpp::combigrid::SourceFunc* sourceFunc) {
  std::shared_ptr<PyCallableHolder> holder;
  {
    std::lock_guard<std::mutex> lock(sourceFuncRegistryMutex());
    auto it = sourceFuncRegistry().find(sourceFunc);
    if (it != sourceFuncRegistry().end()) {
      holder = it->second;
    }
  }
  return (holder != nullptr) && holder->restorePendingError();
}

}  // namespace python
}  // namespace combigrid
}  // namespace sgpp
%}

// Exception handler for every wrapped function that evaluates a SourceFunc passed as its first
// argument (arg1; for SourceFunc's own methods, arg1 is 'this'). Replaces the global handler of
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

    std::lock_guard<std::mutex> lock(sgpp::combigrid::python::sourceFuncRegistryMutex());
    sgpp::combigrid::python::sourceFuncRegistry()[sourceFunc] = holder;
    return sourceFunc;
  }

  ~SourceFunc() {
    {
      std::lock_guard<std::mutex> lock(sgpp::combigrid::python::sourceFuncRegistryMutex());
      sgpp::combigrid::python::sourceFuncRegistry().erase($self);
    }
    delete $self;
  }
}

COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::SourceFunc::evaluate)
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::SourceFunc::evaluateNormalizedInPlace)
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::SourceFunc::evaluateNormalizedOutOfPlace)

%include "combigrid/src/sgpp/combigrid/functions/source_functions/source_function.hpp"
