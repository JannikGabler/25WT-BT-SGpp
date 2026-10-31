// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Python bindings for sgpp::combigrid::VectorSourceFunc (f: R^d -> R^m).
//
// In C++, a VectorSourceFunc wraps a std::function<void(const base::DataVector&,
// std::span<double>)> writing the m values to its second argument. From Python it is constructed
// from a Python callable returning the m values and the number of outputs m:
//
//     def f(point):            # point is a tuple of floats (one entry per dimension)
//         return [point[0] ** 2, point[1]]    # any sequence of m numbers (list, tuple, ndarray)
//     sourceFunc = pysgpp.VectorSourceFunc(f, 2)
//
// The operators interpolate / interpolateLinear / quadrature return the m results as a DataVector.
// Requires SourceFunc.i (PyCallableHolder, registries, COMBIGRID_SOURCE_FUNC_EXCEPTION): threads,
// exceptions and lifetime are handled as for SourceFunc. A result of the callable that is not a
// sequence of exactly m numbers raises ValueError (wrong length) or TypeError after the call, like
// an exception raised by the callable itself.

// The std::function constructors cannot be called from Python; they are replaced by the
// PyObject* constructor below ('pyfunc' activates the callable-checking 'in' typemap, see
// SourceFunc.i). The C++ constructor throws base::data_exception (-> RuntimeError) for
// nOutputs = 0.
%ignore sgpp::combigrid::VectorSourceFunc::VectorSourceFunc(
    const std::function<void(const base::DataVector&, std::span<double>)>&, size_t);
%ignore sgpp::combigrid::VectorSourceFunc::VectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)>&&, size_t);
// std::span has no typemap: evaluate is replaced by an overload returning a DataVector below,
// evaluateNormalizedInPlace (an internal step of the operators) is not exposed.
%ignore sgpp::combigrid::VectorSourceFunc::evaluate(const base::DataVector&, std::span<double>) const;
%ignore sgpp::combigrid::VectorSourceFunc::evaluateNormalizedInPlace;
// The end-of-operator hook (e.g. of a CachedVectorSourceFunc) is an internal step of the
// operators.
%ignore sgpp::combigrid::VectorSourceFunc::finishOperatorCall;

// Before the %extend block: the handler must precede the declaration of the extended evaluate.
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::VectorSourceFunc::evaluate)

%extend sgpp::combigrid::VectorSourceFunc {
  VectorSourceFunc(PyObject* pyfunc, size_t nOutputs) {
    auto holder = std::make_shared<sgpp::combigrid::python::PyCallableHolder>(pyfunc);
    std::function<void(const sgpp::base::DataVector&, std::span<double>)> func(
        sgpp::combigrid::python::PyVectorCallableFunctor{holder});
    auto* sourceFunc = new sgpp::combigrid::VectorSourceFunc(std::move(func), nOutputs);
    sgpp::combigrid::python::registerSourceFunc(sourceFunc, std::move(holder));
    return sourceFunc;
  }

  ~VectorSourceFunc() {
    sgpp::combigrid::python::unregisterSourceFunc($self);
    delete $self;
  }

  // Evaluates the function at point (problem coordinates) and returns the nOutputs() values.
  sgpp::base::DataVector evaluate(const sgpp::base::DataVector& point) const {
    sgpp::base::DataVector out($self->nOutputs());
    $self->evaluate(point, out);
    return out;
  }
}

%include "combigrid/src/sgpp/combigrid/functions/source_functions/vector_source_function.hpp"
