// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Python bindings for the level-to-grid-point-count (growth) functions and the node generation
// functions of the combigrid module.

// ---------------------------------------------------------------------------
// Lvl2GPCntFunc = GPCntType (*)(LvlType)
// ---------------------------------------------------------------------------

// Lvl2GPCntFunc is a plain C function pointer. %callback("%s") exposes the two predefined growth
// functions as function pointer constants under their C++ names, so that
//     genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
// reads exactly like the C++ code. Pointers returned by the getters compare equal (==) to these
// constants.
// Limitations (inherent to a context-free C function pointer):
//  - The constants are not callable from Python.
//  - Custom growth functions written in Python cannot be passed.
%callback("%s") sgpp::combigrid::linearLvl2GPCntFunction;
%callback("%s") sgpp::combigrid::doublingLvl2GPCntFunction;
%include "combigrid/src/sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp"
%nocallback;

// std_vector.i cannot instantiate std::vector for function pointer element types (the generated
// traits do not compile), so SGGenInstr::setLvl2GPCntFuncs / getLvl2GPCntFuncs get hand-written
// typemaps: a Python sequence of function pointer constants in, a tuple of them out.
%typemap(in) const std::vector<sgpp::combigrid::Lvl2GPCntFunc>& (std::vector<sgpp::combigrid::Lvl2GPCntFunc> temp) {
  if (!PySequence_Check($input)) {
    SWIG_exception_fail(SWIG_TypeError, "expected a sequence of Lvl2GPCntFunc function pointers");
  }
  const Py_ssize_t n = PySequence_Size($input);
  for (Py_ssize_t i = 0; i < n; i++) {
    PyObject* item = PySequence_GetItem($input, i);
    void* ptr = nullptr;
    const int res = SWIG_ConvertFunctionPtr(item, &ptr, $descriptor(sgpp::combigrid::Lvl2GPCntFunc));
    Py_XDECREF(item);
    if (!SWIG_IsOK(res)) {
      SWIG_exception_fail(SWIG_TypeError, "sequence element is not a Lvl2GPCntFunc function pointer");
    }
    temp.push_back(reinterpret_cast<sgpp::combigrid::Lvl2GPCntFunc>(ptr));
  }
  $1 = &temp;
}

%typemap(out) const std::vector<sgpp::combigrid::Lvl2GPCntFunc>& {
  $result = PyTuple_New(static_cast<Py_ssize_t>($1->size()));
  for (size_t i = 0; i < $1->size(); i++) {
    PyTuple_SET_ITEM($result, static_cast<Py_ssize_t>(i),
                     SWIG_NewFunctionPtrObj(reinterpret_cast<void*>((*$1)[i]),
                                            $descriptor(sgpp::combigrid::Lvl2GPCntFunc)));
  }
}

// ---------------------------------------------------------------------------
// NodeGenFunc
// ---------------------------------------------------------------------------

// NodeGenFunc is exposed without a director: it is used only through the singletons returned by
// the get*NodeGenFunc() getters below. Those return raw pointers to process-lifetime static
// objects; SWIG proxies for them are non-owning (no %newobject), so Python never deletes them.
// The quadrature rule / interpolation method accessors are extension machinery (their return
// types are not wrapped).
%ignore sgpp::combigrid::NodeGenFunc::getQuadRule;
%ignore sgpp::combigrid::NodeGenFunc::getInterpolationMethod;
%include "combigrid/src/sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp"

%include "combigrid/src/sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp"
%include "combigrid/src/sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp"
%include "combigrid/src/sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp"
%include "combigrid/src/sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp"

namespace std {
  // SGGenInstr::getNodeGenFuncs / setNodeGenFuncs / getUniqueNodeGenFuncs (elements non-owning).
  %template(NodeGenFuncVector) vector<sgpp::combigrid::NodeGenFunc*>;
}
