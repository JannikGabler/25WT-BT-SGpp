// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Python bindings for the multi-index types MI<T> and MIVec<T>.
//
// Both templates are instantiated exactly once, for T = unsigned int. In C++
// this single instantiation carries two aliases, LvlMI (level multi-index) and
// GPMI (grid-point multi-index); in Python the class is called LvlMI and GPMI
// is provided as an alias of it (see the %pythoncode at the end of this file).

namespace std {
  // Element storage of MI<unsigned int>. Needed so that Python lists can be passed
  // wherever a const std::vector<unsigned int>& is expected (MI constructor, MIVec::setMI).
  %template(UnsignedIntVector) vector<unsigned int>;
  // Needed for MIVec(const std::vector<std::vector<unsigned int>>&), i.e. the Python
  // equivalent of the C++ brace initialisation LvlMIVec{{0, 1}, {1, 0}}.
  %template(UnsignedIntVectorVector) vector<vector<unsigned int> >;
}

// ---------------------------------------------------------------------------
// MI<T>
// ---------------------------------------------------------------------------

// std::initializer_list has no Python equivalent; use MI([0, 1, 2]) instead.
%ignore sgpp::combigrid::MI::MI(std::initializer_list<value_type>);
// Takes a *non-const* std::vector&, for which SWIG only accepts an UnsignedIntVector proxy but
// not a Python list. Replaced by the const& constructor in the %extend block below.
%ignore sgpp::combigrid::MI::MI(std::vector<value_type>&);
// Implicit conversions to std::vector are a C++-only convenience.
%ignore sgpp::combigrid::MI::operator std::vector<value_type>&;
%ignore sgpp::combigrid::MI::operator const std::vector<value_type>&;
// The non-const overloads return 'unsigned int&', which SWIG would expose as an opaque pointer
// object. Ignoring them lets SWIG pick the const overloads, which return plain Python ints.
%ignore sgpp::combigrid::MI::at(size_type);
%ignore sgpp::combigrid::MI::front();
%ignore sgpp::combigrid::MI::back();
// Raw pointers and iterators cannot be used from Python.
%ignore sgpp::combigrid::MI::data;
%ignore sgpp::combigrid::MI::begin;
%ignore sgpp::combigrid::MI::cbegin;
%ignore sgpp::combigrid::MI::end;
%ignore sgpp::combigrid::MI::cend;
%ignore sgpp::combigrid::MI::rbegin;
%ignore sgpp::combigrid::MI::rend;
%ignore sgpp::combigrid::MI::insert;
%ignore sgpp::combigrid::MI::erase;
// operator+= / operator-= return MI&. SWIG would wrap the returned reference in a second,
// non-owning proxy that Python then binds to the left-hand name, destroying the owning proxy
// (and with it the C++ object). They are re-exposed safely as __iadd__ / __isub__ below.
%ignore sgpp::combigrid::MI::operator+=;
%ignore sgpp::combigrid::MI::operator-=;

%extend sgpp::combigrid::MI<unsigned int> {
  // Replacement for the ignored MI(std::vector<value_type>&) constructor.
  MI(const std::vector<unsigned int>& data) {
    std::vector<unsigned int> copy(data);
    return new sgpp::combigrid::MI<unsigned int>(copy);
  }

  // The comparison and arithmetic operators are friend functions in C++, which SWIG cannot
  // wrap on its own (warning 503). Only the MI-with-MI variants are exposed.
  // NOTE: comparisons are the *component-wise partial order* of the C++ code, not Python's
  // lexicographic sequence order. Size mismatches raise RuntimeError (std::logic_error).
  bool __eq__(const sgpp::combigrid::MI<unsigned int>& other) const { return *$self == other; }
  bool __ne__(const sgpp::combigrid::MI<unsigned int>& other) const { return *$self != other; }
  bool __lt__(const sgpp::combigrid::MI<unsigned int>& other) const { return *$self < other; }
  bool __le__(const sgpp::combigrid::MI<unsigned int>& other) const { return *$self <= other; }
  bool __gt__(const sgpp::combigrid::MI<unsigned int>& other) const { return *$self > other; }
  bool __ge__(const sgpp::combigrid::MI<unsigned int>& other) const { return *$self >= other; }

  sgpp::combigrid::MI<unsigned int> __add__(const sgpp::combigrid::MI<unsigned int>& other) const {
    return *$self + other;
  }
  sgpp::combigrid::MI<unsigned int> __sub__(const sgpp::combigrid::MI<unsigned int>& other) const {
    return *$self - other;
  }

  // Helpers for the Python-side __iadd__ / __isub__ / __getitem__ / __setitem__ below.
  // Named with two leading underscores, following DataVector::__array in base.
  void __iaddImpl(const sgpp::combigrid::MI<unsigned int>& other) { *$self += other; }
  void __isubImpl(const sgpp::combigrid::MI<unsigned int>& other) { *$self -= other; }
  unsigned int __getElem(size_t pos) const { return (*$self)[pos]; }
  void __setElem(size_t pos, unsigned int value) { (*$self)[pos] = value; }

  %pythoncode
  %{
    def __iadd__(self, other):
        self.__iaddImpl(other)
        return self

    def __isub__(self, other):
        self.__isubImpl(other)
        return self

    # Same set of conveniences as base's DataVector (__len__, __getitem__, __setitem__,
    # __str__). __getitem__ raises IndexError so that iteration terminates.
    def __len__(self):
        return self.size()

    def __getitem__(self, pos):
        if pos < 0 or pos >= self.size():
            raise IndexError("multi-index position out of range")
        return self.__getElem(pos)

    def __setitem__(self, pos, value):
        if pos < 0 or pos >= self.size():
            raise IndexError("multi-index position out of range")
        self.__setElem(pos, value)

    def __str__(self):
        return "[" + ", ".join(str(v) for v in self) + "]"
  %}
}

%include "combigrid/src/sgpp/combigrid/multiindices/multiindex.hpp"

// LvlMI = GPMI = MI<unsigned int> (see type_defs.hpp).
%template(LvlMI) sgpp::combigrid::MI<unsigned int>;

namespace std {
  // Needed for MIVec(const std::vector<MI<T>>&), i.e. LvlMIVec([LvlMI([0, 1]), LvlMI([1, 0])]).
  %template(LvlMIVector) vector<sgpp::combigrid::MI<unsigned int> >;
}

// ---------------------------------------------------------------------------
// MIVec<T>
// ---------------------------------------------------------------------------

// std::initializer_list has no Python equivalent; use LvlMIVec([[0, 1], [1, 0]]) instead.
%ignore sgpp::combigrid::MIVec::MIVec(const std::initializer_list<MI<T>>);
// Raw pointer to the internal storage.
%ignore sgpp::combigrid::MIVec::data;
// The non-const operator() returns an MIVecElemProxy (C++ assignment proxy). Ignoring it leaves
// the const overload, which SWIG exposes as __call__ returning a plain int: miVec(miIdx, dim).
// Element writes go through setMI().
%ignore sgpp::combigrid::MIVec::operator()(size_t, size_t);
// These return std::shared_ptr to internal caches. componentWiseMax and paretoMaxima are
// re-exposed under the same Python name by the copy-returning helpers in the %extend below;
// the lookup table and cache invalidation are internals.
%ignore sgpp::combigrid::MIVec::componentWiseMax;
%ignore sgpp::combigrid::MIVec::paretoMaxima;
%ignore sgpp::combigrid::MIVec::lookup;
%ignore sgpp::combigrid::MIVec::clearCachedValues;
%rename(componentWiseMax) sgpp::combigrid::MIVec<unsigned int>::componentWiseMaxCopy;
%rename(paretoMaxima) sgpp::combigrid::MIVec<unsigned int>::paretoMaximaCopy;

%extend sgpp::combigrid::MIVec<unsigned int> {
  // Same semantics as the C++ methods (including caching), but returning copies instead of
  // std::shared_ptr. Avoids %shared_ptr on MI and, in particular, on std::vector<size_t>, which
  // would change how base's SizeVector is wrapped.
  sgpp::combigrid::MI<unsigned int> componentWiseMaxCopy() const {
    return *($self->componentWiseMax());
  }
  std::vector<size_t> paretoMaximaCopy(const bool isDownwardsClosed = false) const {
    return *($self->paretoMaxima(isDownwardsClosed));
  }

  // Helper for __getitem__ below (C++ operator[] is not wrapped by SWIG, warning 389).
  sgpp::combigrid::MI<unsigned int> __getMI(size_t miIdx) const { return (*$self)[miIdx]; }

  %pythoncode
  %{
    def __len__(self):
        return self.nMI()

    # Returns a copy of the miIdx-th multi-index, like C++ operator[].
    def __getitem__(self, miIdx):
        if miIdx < 0 or miIdx >= self.nMI():
            raise IndexError("multi-index vector index out of range")
        return self.__getMI(miIdx)
  %}
}

// SWIG warns that MIVec(const std::vector<std::vector<T>>&) is shadowed by
// MIVec(const std::vector<MI<T>>&). This is a false positive: the generated overload dispatch checks
// the sequence elements at runtime, so LvlMIVec([[0, 1]]) and LvlMIVec([LvlMI([0, 1])]) both work.
%warnfilter(509) sgpp::combigrid::MIVec<unsigned int>::MIVec;

%include "combigrid/src/sgpp/combigrid/multiindices/multiindex_vector.hpp"

// LvlMIVec = MIVec<unsigned int> (see type_defs.hpp).
%template(LvlMIVec) sgpp::combigrid::MIVec<unsigned int>;

%pythoncode
%{
# In C++, GPMI and LvlMI are aliases of the same type MI<unsigned int>.
GPMI = LvlMI
%}
