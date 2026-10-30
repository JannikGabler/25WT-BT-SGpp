// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Python bindings of the combination technique (combigrid) module.
//
// This file is %include'd by pysgpp/pysgpp.i after base.i, so all base types (DataVector,
// DataMatrix, IntVector, SizeVector, ...) and the global %exception handler are available.
// The C++ headers are made visible to the generated wrapper through the umbrella header
// sgpp_combigrid.hpp, which pysgpp.i includes when SG_COMBIGRID is set.
//
// Ownership conventions used throughout this file:
//  - Objects returned by value are owned by Python.
//  - Objects returned by (const) reference or pointer into another object's memory are non-owning
//    proxies. To prevent dangling references, the Python proxy of the owner is stored in the
//    attribute '_owner' of the returned proxy (%pythonappend), which keeps the owner alive.
//  - MIVecSGGenInstr stores a C++ reference to its LvlMIVec; see the section "Sparse grid
//    generation instructions".

// According to the SWIG documentation, shared pointers have to be declared before the classes
// themselves. The SGGenInstr hierarchy is wrapped as shared_ptr proxies, so that clone (see
// "Sparse grid generation instructions") returns an owning proxy of the polymorphic copy.
%shared_ptr(sgpp::combigrid::SGGenInstr)
%shared_ptr(sgpp::combigrid::CompleteSGGenInstr)
%shared_ptr(sgpp::combigrid::MIVecSGGenInstr)

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

%include "MultiIndex.i"

// Only needed so that SWIG resolves LvlType, GPCntType, CTCoeffType, LvlMI, GPMI, LvlMIVec,
// HyperCubeArea and Lvl2GPCntFunc in the signatures below; the aliases are not wrapped themselves.
%include "combigrid/src/sgpp/combigrid/type_defs.hpp"

namespace std {
  // Element of HyperCubeArea; allows setDomain((0, 1)) and setDomainForDim((0, 1), dim).
  %template(DoubleDoublePair) pair<double, double>;
  // HyperCubeArea = std::vector<std::pair<double, double>>; allows setDomain([(0, 1), (-1, 1)]).
  %template(HyperCubeArea) vector<pair<double, double> >;
}

// ---------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------

%include "Functions.i"
%include "SourceFunc.i"

// ---------------------------------------------------------------------------
// Sparse grid generation instructions
// ---------------------------------------------------------------------------

// genReducedMIVecWithCoeffs returns std::pair<LvlMIVec, std::vector<CTCoeffType>>. std_pair.i
// cannot be instantiated for it: MIVec is not copy-assignable (const member), which the
// generated 'first' / 'second' setters of a pair proxy require. The pair is returned as the
// Python tuple (LvlMIVec, tuple of ints) instead, which is also what std_pair.i returns for a
// pair.
%typemap(out) std::pair<sgpp::combigrid::LvlMIVec, std::vector<sgpp::combigrid::CTCoeffType> > {
  const std::pair<sgpp::combigrid::LvlMIVec, std::vector<sgpp::combigrid::CTCoeffType> >& pairRef = $1;
  PyObject* coeffs = PyTuple_New(static_cast<Py_ssize_t>(pairRef.second.size()));
  for (size_t i = 0; i < pairRef.second.size(); i++) {
    PyTuple_SET_ITEM(coeffs, static_cast<Py_ssize_t>(i), PyLong_FromLong(pairRef.second[i]));
  }
  $result = PyTuple_New(2);
  PyTuple_SET_ITEM($result, 0,
                   SWIG_NewPointerObj(new sgpp::combigrid::LvlMIVec(pairRef.first),
                                      $descriptor(sgpp::combigrid::MIVec<unsigned int>*),
                                      SWIG_POINTER_OWN));
  PyTuple_SET_ITEM($result, 1, coeffs);
}

// MIVecSGGenInstr stores a *reference* to the LvlMIVec passed to its constructor, and clone()
// copies that reference (e.g. into every SparseGrid built from it). The Python LvlMIVec is therefore
// kept alive by the instruction (_miVec), by its clones (_owner, see clone below) and, transitively,
// by sparse grids built from it (_genInstr, see SparseGrid below).
%pythonappend sgpp::combigrid::MIVecSGGenInstr::MIVecSGGenInstr %{
  self._miVec = miVec
%}
// clone() returns a std::unique_ptr, but the hierarchy is wrapped as shared_ptr proxies (a class
// cannot be wrapped as both). It is replaced by a helper converting the clone to a std::shared_ptr,
// which is renamed to clone and inherited by the subclasses. (A differently named helper plus
// %rename is needed because an %ignore of clone would also hide an %extend method of that name.)
%ignore sgpp::combigrid::SGGenInstr::clone;
%ignore sgpp::combigrid::CompleteSGGenInstr::clone;
%ignore sgpp::combigrid::MIVecSGGenInstr::clone;
%rename(clone) sgpp::combigrid::SGGenInstr::cloneShared;
%pythonappend sgpp::combigrid::SGGenInstr::cloneShared %{
  val._owner = self
%}
%extend sgpp::combigrid::SGGenInstr {
  std::shared_ptr<sgpp::combigrid::SGGenInstr> cloneShared() const { return $self->clone(); }
}

%include "combigrid/src/sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp"
%include "combigrid/src/sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp"
%include "combigrid/src/sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp"

// ---------------------------------------------------------------------------
// Grids
// ---------------------------------------------------------------------------

// Rvalue overload, shadowed by the const& overload anyway (SWIG warning 509).
%ignore sgpp::combigrid::TensorGrid::TensorGrid(GPMI&&, base::DataVector&&);
// Internal helper of the linear interpolation operator.
%ignore sgpp::combigrid::TensorGrid::getNeighborsForLinInterpolation;
// Both return a const reference into the tensor grid.
%pythonappend sgpp::combigrid::TensorGrid::getGPCntPerDim %{
  val._owner = self
%}
%pythonappend sgpp::combigrid::TensorGrid::getNodesPerDim %{
  val._owner = self
%}
// getGridPointAndMI(idx) returns std::pair<DataVector, GPMI>; returned as the Python tuple
// (DataVector, LvlMI) of owned copies, consistent with the pair typemap above.
%typemap(out) std::pair<sgpp::base::DataVector, sgpp::combigrid::GPMI> {
  const std::pair<sgpp::base::DataVector, sgpp::combigrid::GPMI>& pairRef = $1;
  $result = PyTuple_New(2);
  PyTuple_SET_ITEM($result, 0,
                   SWIG_NewPointerObj(new sgpp::base::DataVector(pairRef.first),
                                      $descriptor(sgpp::base::DataVector*), SWIG_POINTER_OWN));
  PyTuple_SET_ITEM($result, 1,
                   SWIG_NewPointerObj(new sgpp::combigrid::GPMI(pairRef.second),
                                      $descriptor(sgpp::combigrid::MI<unsigned int>*),
                                      SWIG_POINTER_OWN));
}
%include "combigrid/src/sgpp/combigrid/grids/tensor_grid.hpp"

// TensorGridCTData is read-only in Python: it is only obtained from a SparseGrid.
%nodefaultctor sgpp::combigrid::TensorGridCTData;
%immutable sgpp::combigrid::TensorGridCTData::coefficient;
// SWIG wraps class-type data members as non-owning pointers into the struct. mi and tensorGrid
// are therefore re-exposed as read-only properties that keep the TensorGridCTData alive.
%ignore sgpp::combigrid::TensorGridCTData::mi;
%ignore sgpp::combigrid::TensorGridCTData::tensorGrid;
%extend sgpp::combigrid::TensorGridCTData {
  const sgpp::combigrid::LvlMI& __getMI() const { return $self->mi; }
  const sgpp::combigrid::TensorGrid& __getTensorGrid() const { return $self->tensorGrid; }

  %pythoncode %{
    @property
    def mi(self):
        val = self.__getMI()
        val._owner = self
        return val

    @property
    def tensorGrid(self):
        val = self.__getTensorGrid()
        val._owner = self
        return val
  %}
}
%include "combigrid/src/sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp"

namespace std {
  // SparseGrid::getTensorGrids; returned as a tuple of owned copies.
  %template(TensorGridCTDataVector) vector<sgpp::combigrid::TensorGridCTData>;
}

// Both getTensorGrid overloads return a reference into the sparse grid.
%pythonappend sgpp::combigrid::SparseGrid::getTensorGrid %{
  if val is not None:
    val._owner = self
%}
// The sparse grid holds a clone of the generation instruction. The clone of an MIVecSGGenInstr
// references the caller's LvlMIVec, so the instruction's Python proxy (which keeps that vector
// alive) is kept alive as well.
%pythonappend sgpp::combigrid::SparseGrid::SparseGrid %{
  self._genInstr = genInstruction
%}
// Returns a reference to the sparse grid's internal instruction; keep the sparse grid alive.
// Note: SWIG drops the const of the returned reference, so setters called on the returned object
// modify the instruction the sparse grid's operators use.
%pythonappend sgpp::combigrid::SparseGrid::getGenInstr %{
  val._owner = self
%}
// SparseGridBase (the abstract base class of SparseGrid and DeltasSparseGrid) is not wrapped, as
// it is irrelevant for Python users. SWIG therefore does not see the methods SparseGrid inherits
// from it: its public methods that are meaningful in Python are declared here again and forward to
// the inherited C++ methods. Not declared again are the iterators (iteration is provided by
// __len__ / __getitem__ below) and the protected methods for manual assembly (a SparseGrid without
// generation instruction or with unset size caches crashes the operators).
%warnfilter(SWIGWARN_TYPE_UNDEFINED_CLASS) sgpp::combigrid::SparseGrid;
%extend sgpp::combigrid::SparseGrid {
  size_t nDim() const { return $self->nDim(); }

  size_t nTG() const { return $self->nTG(); }

  const sgpp::combigrid::TensorGridCTData& getTensorGrid(size_t idx) const {
    return $self->getTensorGrid(idx);
  }

  // The C++ overload returns a const_iterator, which cannot be used from Python. This one returns
  // the matching TensorGridCTData, or None if the sparse grid contains no tensor grid with this
  // level multi-index.
  const sgpp::combigrid::TensorGridCTData* getTensorGrid(const sgpp::combigrid::LvlMI& mi) const {
    auto it = $self->getTensorGrid(mi);
    return (it == $self->end()) ? nullptr : &(*it);
  }

  // The C++ method returns a std::span, for which SWIG has no typemap. A std::vector is returned
  // as a tuple of owned copies (TensorGridCTDataVector).
  std::vector<sgpp::combigrid::TensorGridCTData> getTensorGrids() const {
    const auto tgs = $self->getTensorGrids();
    return std::vector<sgpp::combigrid::TensorGridCTData>(tgs.begin(), tgs.end());
  }

  size_t getMaxTGGPCnt() const { return $self->getMaxTGGPCnt(); }

  size_t getMaxTGSumOverGPCntsPerDim() const { return $self->getMaxTGSumOverGPCntsPerDim(); }

  bool operator==(const sgpp::combigrid::SparseGrid& other) const { return *$self == other; }

  %pythoncode %{
    # Same kind of conveniences as base's DataVector. __getitem__ raises IndexError so that
    # 'for tgData in sparseGrid' terminates.
    def __len__(self):
        return self.nTG()

    def __getitem__(self, idx):
        if idx < 0 or idx >= self.nTG():
            raise IndexError("tensor grid index out of range")
        return self.getTensorGrid(idx)
  %}
}
%include "combigrid/src/sgpp/combigrid/grids/sparse_grid.hpp"

// ---------------------------------------------------------------------------
// Operators
// ---------------------------------------------------------------------------

// Internal per-tensor-grid helpers. SWIG flattens namespaces, so without these %ignores they would
// be exposed as public functions (global_interpolation::interpolate and
// linear_interpolation::interpolate would even be merged into the public interpolate /
// interpolateLinear functions as additional overloads).
%ignore sgpp::combigrid::global_interpolation::interpolate;
%ignore sgpp::combigrid::global_interpolation::interpolateFirstDim;
%ignore sgpp::combigrid::global_interpolation::interpolateLaterDim;
%ignore sgpp::combigrid::global_interpolation::getInterpolationCntPerDim;
%ignore sgpp::combigrid::linear_interpolation::interpolate;
%ignore sgpp::combigrid::linear_interpolation::computeNeighborGPWithWeight;
%ignore sgpp::combigrid::quadrature_operator::processTensorGrid;
%ignore sgpp::combigrid::quadrature_operator::getWeights;

// Vector-valued source functions (VectorSourceFunc) are not exposed yet (WP-29d): hide the
// operator overloads that take one (VectorSourceFunc has no proxy, std::span no typemap).
%ignore sgpp::combigrid::quadrature(const VectorSourceFunc&, const SparseGrid&, std::span<double>);
%ignore sgpp::combigrid::quadrature(const VectorSourceFunc&, const SparseGrid&);

// Re-raise exceptions of Python source functions after the operator returns (see SourceFunc.i).
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::interpolate)
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::interpolateLinear)
COMBIGRID_SOURCE_FUNC_EXCEPTION(sgpp::combigrid::quadrature)

%include "combigrid/src/sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp"
%include "combigrid/src/sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp"
%include "combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.hpp"

// ---------------------------------------------------------------------------
// Tools
// ---------------------------------------------------------------------------

// Reference implementation used by the unit tests only.
%ignore sgpp::combigrid::tools::computeCTCoeffsNaive;
// Internal helpers of the CT coefficient algorithm (internalComputeCTCoeffSingle takes internal
// types such as MIView and misc::MIVecLookup, which are not wrapped).
%ignore sgpp::combigrid::tools::ct_coeff_calc::internalComputeCTCoeffSingle;
%ignore sgpp::combigrid::tools::ct_coeff_calc::computeParityOfMI;
%include "combigrid/src/sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp"
