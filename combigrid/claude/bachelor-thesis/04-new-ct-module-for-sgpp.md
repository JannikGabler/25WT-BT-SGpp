---
source: bachelor thesis/thesis/chapters/a_new_ct_module_for_sgpp.tex (+ "chapters/sections/a new ct module for sgpp/{requirements_and_approach,basic_usage,features,overview_over_the_architecture,functionality}.tex")
chapter: 4
title: A new Combination Technique Module for SG++
---

# 4 A new Combination Technique Module for SG++ {#sec:a_new_ct_module_for_sgpp}

The analysis of existing sparse grid frameworks has shown that a well-designed implementation of the combination technique (CT) must balance clarity, extensibility, and performance.
Building on these observations, the following sections first formulate the requirements that guided the design and implementation, then present the basic usage from a user's perspective, and finally provide a structured overview of the most important features, architectural principles, and internal data structures.
The internal algorithmic and design choices, as well as the specific implementation details, are discussed in Section 5 (Algorithmic and Design Choices, file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md)), since this chapter is intended to serve as a starting point for users.

## 4.1 Requirements and Approach {#sec:a_new_ct_module_for_sgpp:requirements_and_approach}

Based on the motivation outlined in the previous chapter, the development of a new combination technique module for SG++ is guided by a set of requirements that should address the weaknesses of the existing implementation while preserving its strengths.
At this stage of the thesis, the module is not yet described as a finished product.
Instead, this section defines the goals, constraints, and conceptual approach that shape its design and implementation in the following chapters.

The first requirement emerging from the framework analysis is a clear internal structure.
The module should represent the sparse grid workflow in a way that is easy to understand, easy to maintain, and suitable for further extension.
This implies that the major steps of the workflow should be separated conceptually and technically, rather than being intertwined in a single monolithic implementation.
A structured design also makes it easier to reason about the responsibilities of the individual components and to adapt them independently if necessary.

A second requirement concerns performance.
Since the combination technique is used in computationally demanding settings, the implementation must avoid unnecessary overhead.
The architecture should therefore allow efficient data handling, controlled memory usage, and targeted optimization of the most relevant parts of the workflow.
At the same time, performance should not be achieved at the expense of clarity or extensibility.

Another important requirement is extensibility.
The module should not force all users into one fixed workflow, but instead provide a flexible architecture that can support different use cases.
Standard functionality should be available through sensible defaults, while advanced users should still be able to exchange or customize individual parts of the workflow.
This balance between convenience and flexibility is central to the intended design.

A fourth requirement is compatibility with SG++.
The new module is intended to become part of the existing framework ecosystem and should therefore integrate naturally with the surrounding code base.
Wherever appropriate, existing SG++ data structures and concepts should be reused, so that the module fits into the overall architecture instead of forming an isolated subsystem.
Such integration is important both for practical usability and for long-term maintainability.

To satisfy these requirements, the approach taken in this thesis is based on a modular representation of the sparse grid workflow.
In its conceptual form, the workflow is divided into four steps: the definition of the source function, the specification of a blueprint describing the sparse grid configuration, the generation of the sparse grid itself, and the application of operators to the resulting grid.
This decomposition provides a clean conceptual model that mirrors the mathematical structure of the combination technique while also supporting a practical software design.
Listing 4.1 below shows a draft of this workflow from an early design phase.

<!-- label: lst:a_new_ct_module_for_sgpp:requirements_and_approach:pseudocode -->
**Listing 4.1.** Draft from an early design stage of a basic workflow for the new combigrid module.

```python
def f(point) = ...  # source function

blueprint = ([0, 1], [CC_gp, Uniform_gp], ...) # Specifies how the sparse grid is constructed

sg = create_sparse_grid(instr)

result = quadrature(sg, f)
```

The intended architecture follows the principle of separation of concerns.
Each part of the workflow should be responsible for one clearly defined task, which makes the overall system easier to understand and opens the possibility of optimizing individual parts independently.
Grid generation, point evaluation, combination handling, and operator application can then be developed as separate components that interact through well-defined interfaces.

The goal of this thesis is to lay the foundation for such a module.
The approach presented in this section defines the core requirements, the central data flow, and the most important concepts needed for a combination technique-based sparse grid implementation.
It does not aim to reproduce the full feature set of the existing frameworks in a single step.
Instead, it provides a sustainable basis on which later extensions can be built.
Additional functionality may be added in subsequent work or by advanced users who require specialized behavior.
In this sense, the present thesis should be understood as the starting point of a new combigrid architecture rather than as its final completion.

## 4.2 Basic usage {#sec:a_new_ct_module_for_sgpp:basic_usage}

Using the new CT module is as easy as Listing 4.2 below shows.
In the following, we will explain each of the four instructions used and introduce the basic usage of the overall module alongside it.

<!-- label: lst:a_new_ct_module_for_sgpp:basic_usage:intro -->
**Listing 4.2.** Simple quadrature script to demonstrate the basic usage.

```cpp
SourceFunc sourceFunc(func);

CompleteSGGenInstr genInstr(3 /*maxLvl*/, 2 /*nDim*/);

SparseGrid sg(genInstr);

double result = quadrature(sourceFunc, sg);
```

The first line creates a new `SourceFunc` object called `sourceFunc` by taking a C++ function `func` as an argument.
A `SourceFunc` represents a mathematical function $f(\vec{x})$ which can be later used as a source function for operations like quadrature or interpolation.
It can be created by providing a C++ function (representable by `std::function`) capable of evaluating $f$ for a given point $\vec{x}$.
This C++ function is represented by the variable `func` in line 1.

Such a function must have the signature `DataVector&` $\rightarrow$ `double`, where `DataVector` is just a wrapper around a `double` vector.
The function $f(\vec{x}) = 1$ could be, for example, specified as shown in Listing 4.3 below.

<!-- label: lst:a_new_ct_module_for_sgpp:basic_usage:cpp_specification_of_const_function -->
**Listing 4.3.** Implementation of $f(\vec{x}) = 1$ for a `SourceFunc`.

```cpp
double func(const DataVector& point) {
  return 1.0;
}
```

After specifying our source function, we now prepare the generation of a sparse grid.
Sparse grids are generated using so-called "sparse grid generation instructions".
A sparse grid generation instruction is a blueprint for constructing a sparse grid in a certain way.
There are different types of such blueprints (see Section 4.5.5, Sparse Grid Generation Instructions, below).
The second instruction of Listing 4.2 above creates exactly such a sparse grid generation instruction into the variable `genInstr`.
The most basic type of sparse grid generation instruction, a `CompleteSGGenInstr` for complete sparse grids, is used.
`CompleteSGGenInstr` takes the maximum level $l_{max} \in \mathbb{N}_0$ and the number of dimensions as parameters (note that all indices start from $0$).
To conclude, `CompleteSGGenInstr genInstr(3, 2)` is a blueprint for a complete two-dimensional sparse grid with a maximum level of $3$.

Using the sparse grid generation instruction, we can now actually generate our sparse grid in the third instruction.
This is done by creating a new `SparseGrid` object called `sg` using `genInstr`.
Figure 4.1 below shows the generated sparse grid.

<!-- label: fig:a_new_ct_module_for_sgpp:basic_usage:sg_generated_by_intro -->
**Figure 4.1.** Sparse grid generated by Listing 4.2 above.
*Image: `figures/grids/sparse grids/2d_full_3m.png`*

The final fourth instruction uses the sparse grid `sg` to approximate the integral of our function `sourceFunc` over $[0, 1]^2$.

This completes the short example to demonstrate the basic usage.
The most important takeaway is the structure consisting of the 4 steps "Source function", "Sparse grid generation instruction", "Sparse grid generation" and "Operator application".
For a more detailed overview over the functionality refer to Section 4.5 (Functionality) below and for the architecture see Section 4.4 (Overview over the architecture) below.

## 4.3 Features {#sec:a_new_ct_module_for_sgpp:features}

Table 4.1 below gives an overview of the most important features of the new SG++ CT module.
The module is implemented in C++ and is designed for efficient execution on central processing unit (CPU) architectures.
Its major operations are parallelized and optimized for balanced workload distribution, while distributed execution is not supported.
In terms of functionality, the module currently supports complete sparse grids as well as arbitrary non-adaptive sparse grids based on sets of multi-indices.
It provides pre-defined equidistant and second-kind Chebyshev (Clenshaw-Curtis) nodes, and includes mathematical operators for quadrature, global interpolation, and linear interpolation.
In addition, source function value caching is available to improve performance.
The module operates on arbitrary hyperrectangular domains and offers several customization options, such as a boundary level offset, anisotropic sparse and tensor grids with different node types per dimension, and custom conversions from levels to numbers of grid points.

<!-- label: tab:a_new_ct_module_for_sgpp:features -->
**Table 4.1.** Overview over all relevant features.

| Name | Description |
|---|---|
| Supported languages | C++ |
| Parallelism | All major operations are multi-threaded and optimized for equal workload distributions. |
| Distributed | No |
| Supported hardware devices | CPU |
| Sparse grid generation methods | Complete sparse grids; arbitrary non-adaptive sparse grids based on set of multi-indices; no pre-defined adaptive methods yet but user-defined ones can be easily integrated. |
| Nodes | Equidistant, sec. kind Chebyshev (Clenshaw-Curtis) |
| Mathematical operators | Quadrature, global interpolation, linear interpolation |
| Caching | Source function value caching |
| Supported domains | Arbitrary hyperrectangle |
| Special customizabilities | Boundary level offset; anisotropic sparse and tensor grids (different node type per dimension); custom conversions from levels to number of grid points. |

## 4.4 Overview over the architecture {#sec:a_new_ct_module_for_sgpp:overview_over_the_architecture}

This section provides a high-level overview over the architecture of the module.
All major components of the module can be found in the following UML diagram depicted by Figure 4.2 below.
The illustrated structure can be used as a mental model while working with it, but it also matches the real architecture found in the code.
Details about each component follow in the subsequent Section 4.5 (Functionality) below.

<!-- label: fig:a_new_ct_module_for_sgpp:overview_over_the_architecture:uml_diagram -->
**Figure 4.2.** High-level overview over the architecture.
*Image: `figures/architecture_overview.pdf`*

<!-- TODO: conversion unclear -->
This UML diagram is a rendered PDF (`figures/architecture_overview.pdf`); its component structure could not be transcribed as text because it is a graphic, not vector source. A future session that needs the concrete class relationships should open the PDF (or the corresponding SG++ combigrid source code) directly.

## 4.5 Functionality {#sec:a_new_ct_module_for_sgpp:functionality}

The functionality of the module is centered around a small set of abstractions that are intended to make sparse grid construction and usage both intuitive and efficient.
To this end, the module provides dedicated representations for multi-indices, source functions, node generation, sparse grid generation instructions, and the sparse and tensor grid data structures themselves.
In addition, it offers basic mathematical operators and a configurable choice of primitive data types to adapt the implementation to different use cases.
The following subsections introduce these components from a user-oriented perspective and explain how they interact to form the public interface of the module.

### 4.5.1 Multi-indices {#sec:a_new_ct_module_for_sgpp:functionality:multi-indices}

Multi-indices $\vec{i} \in \mathbb{N}_0^d$ are represented by the class `MI<T>`, where `T` is a type parameter specifying the type of the indices.
`MI<T>` is a wrapper around `std::vector<T>` with added functionality to improve performance and ease of use.
There are two pre-defined template instantiations `LvlMI` and `GPMI`.
`LvlMI`s (level multi indices) are used to represent level multi-indices.
One example of their use is in `SparseGrid`, where they are employed for the level multi-index of a tensor grid that is contained.
`GPMI` (grid point multi indices) is used to represent the index of a specific grid point in a tensor grid.
For example, the first grid point in a 2D tensor grid is represented by the multi-index $(0, 0)$ and the first successor in x dimension by $(1, 0)$.
The two specializations should always be preferred over the general `MI<T>` if feasible.
This allows for an easy change of the primitive data type used for indices (see Section 4.5.9, Primitive data types, below).

Listing 4.4 below illustrates the basic usage of `MI<T>` for accessing tensor grids and grid points via multi-indices.
First, a level multi-index `LvlMI` is created to retrieve the tensor grid with level vector $(2,3,0)$ from the sparse grid.
Then, a grid point multi-index `GPMI` is used to access a specific grid point of that tensor grid.

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:mi_example -->
**Listing 4.4.** Example demonstrating `MI<T>`.

```cpp
SparseGrid sg(...);

// Retrieve the tensor grid with the multi-index (2, 3, 0) from sparse grid
LvlMI lvlMI{2, 3, 0};
TensorGrid tg = *getTensorGrid(lvlMI);

// Get the first grid point in the tensor grid
GPMI gpMI{0, 0, 0};
DataVector gridPoint = tg.getGridPoint(gpMI);
```

In addition to individual multi-indices, collections of multi-indices are often necessary as well.
The class `MIVec<T>` is used for this purpose.
It behaves similar to a `std::vector<MI<T>>` but with additional optimizations like contiguous memory allocation and further functionality.
Analogously to `MI<T>`, there are the same two template instantiations, `LvlMIVec` and `GPMIVec`.

Listing 4.5 below demonstrates how `MIVec<T>` is used to store and pass collections of multi-indices.
In the example, a set of level multi-indices is assembled and then wrapped into a sparse grid generation instruction.
The constructor of `SparseGrid` subsequently uses this instruction to generate a sparse grid based on the given multi-index set.

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:mi_vec_example -->
**Listing 4.5.** Example demonstrating `MIVec<T>`.

```cpp
// Level multi-indices of the tensor grids, which should be used for the SG generation
LvlMIVec mis{{2, 0}, {1, 1}, {2, 0}, {1, 0}, {0, 1}, {0, 0}};

// SG generation instruction for a SG using the six multi-indices
MIVecSGGenInstr genInstr(mis);

SparseGrid sg(genInstr);
```

### 4.5.2 Source Functions {#sec:a_new_ct_module_for_sgpp:functionality:source_functions}

Source functions, as defined in Section 2.1 (Basics) of file [02-sparse-grids.md](02-sparse-grids.md), are represented by the class `SourceFunc`.
Such a `SourceFunc` can be created by providing a `std::function` capable of evaluating $f$ for a given point $\vec{x}$.
The `std::function` must have the signature `DataVector&` $\rightarrow$ `double`, where `DataVector` is just a wrapper around a `std::vector<double>`.
A `std::function` can wrap any compatible regular C++ function, lambda expression, functor, or `std::bind` expression.

The wrapper class `SourceFunc` itself provides functionality like caching and the linear transformation from $[0,1]^d$ to $[a_1, b_1] \times \ldots \times [a_d, b_d]$.
A `SourceFunc` needs to be provided in any kind of functionality where source function values are required.
Typical examples are mathematical operators and adaptive refinement.

Listing 4.6 below shows how a source function can be provided to the module in the form of a C++ function and wrapped into a `SourceFunc` object.
The example defines a simple function that computes the product of cosine values over all coordinates of a given point.
This wrapped function is then passed to the `quadrature` operator together with a `SparseGrid`, illustrating how source functions are integrated into the workflow of the module.

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:source_func_example -->
**Listing 4.6.** Example demonstrating `SourceFunc`.

```cpp
double func(const DataVector& point) {
  double result = 1;

  for (size_t dim = 0; dim < point.size(); dim++) {
    result *= std::cos(point[dim]);
  }

  return result;
}

SourceFunc sourceFunc(func);

SparseGrid sg(...);
double result = quadrature(sourceFunc, sg);
```

### 4.5.3 Nodes and Grid Points {#sec:a_new_ct_module_for_sgpp:functionality:nodes_and_grid_points}

Nodes are represented using `double`s.
They are generated by so-called node generation functions, implementing the node distributions.
Node generation functions are represented by the abstract class `NodeGenFunc` in the code.

The module currently offers two pre-defined node generation function for equidistant and second kind Chebyshev nodes.
They are obtainable via the operations `getEquidistantNodeGenFunc()` and `getSecondTypeChebyshevNodeGenFunc()`.
Custom node distributions can be easily added by creating a new inheriting class of `NodeGenFunc` and overwriting the abstract methods.
For more details, refer to Section 5.2 (Node Generation) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md) or the two pre-defined ones.

Grid points are conceptually node vectors, one node per dimension, and can be found exactly like this in the module.
They are represented as `DataVector`s.

### 4.5.4 Levels {#sec:a_new_ct_module_for_sgpp:functionality:levels}

Levels $\ell$ are in $\mathbb{N}_0$, i.e., they start at $0$.
This definition differs from the regular mathematical definition, where levels start at $1$.

Semantically, levels indirectly describe a certain number of grid points used in a certain dimension.
The typical conversion used is

<!-- label: eq:a_new_ct_module_for_sgpp:functionality:levels:doubling_lvl_2_gpc_func -->
$$
\#gp(\ell) = 2^{\ell+1} - 1, \tag{4.1}
$$

which corresponds to a doubling of the grid point count on every level.
However, this conversion can be freely chosen by the user.
Note that this number of grid points does not include the grid points on the boundary.
It only specifies how many inner grid points are used for a dimension.

Internally, a function performing this conversion is referred to as `Lvl2GPCntFunc`.
It is simply a function which takes a level as an input and returns the number of grid points the level should have.
There are currently two pre-defined `Lvl2GPCntFunc`s, as shown in Table 4.2 below.

<!-- label: tab:a_new_ct_module_for_sgpp:functionality:levels:pre-defined_lvl_to_gp_cnt_functions -->
**Table 4.2.** Pre-defined `Lvl2GPCntFunc`s.

| Name | `Lvl2GPCntFunc` | Definition |
|---|---|---|
| Linear | `linearLvl2GPCntFunction` | $\#gp(\ell) = \ell + 1$ |
| Doubling | `doublingLvl2GPCntFunction` | $\#gp(\ell) = 2^{\ell+1} - 1$ |

### 4.5.5 Sparse Grid Generation Instructions {#sec:a_new_ct_module_for_sgpp:functionality:sg_gen_instr}

`SparseGrid`s are constructed by the module using blueprints, which we call sparse grid generation instructions.
The basic attributes required for every sparse grid generation, are covered by the abstract base class `SGGenInstr`.
They are listed and described in Table 4.3 below.
For additional explanations and examples, refer to Section 5.6 (Sparse Grid Generation) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md).

The actual method and further type-specific attributes are defined in the specializing subclasses.
There are currently two pre-defined specializations, `CompleteSGGenInstr` and `MIVecSGGenInstr`.
Custom sparse grid generation instructions, like a dimension-adaptive approach, can be easily implemented by defining new subclasses.

`CompleteSGGenInstr` is the most basic sparse grid generation instruction.
It can be used to generate complete sparse grids, given the desired max. level $\ell_{max} \in \mathbb{N}_0$ and the number of dimensions (as specified by Equation (2.29) in file [02-sparse-grids.md](02-sparse-grids.md)).

`MIVecSGGenInstr` is a very primitive instruction which allows the construction of sparse grids of any form.
Input is a `LvlMIVec` which represents a collection of level multi-indices.
The sparse grid generation instruction computes the minimal downwards-closed multi-index set that contains all specified multi-indices.
This ensures that the `LvlMIVec` is downwards-closed, which is a necessary property for the sparse grid generation.
The computed downwards-closed set is then taken and passed to the sparse grid generation.
To drastically optimize this step, `MIVecSGGenInstr` offers a flag (`assumeDownwardsClosedness`) that completely skips the computation.
Keep in mind that setting this flag will deactivate all correctness checks.

Listing 4.7 and Listing 4.8 below illustrate two typical ways of using `CompleteSGGenInstr` for sparse grid construction.
In the first example, the generation instruction is initialized with a maximal level of $2$ and two dimensions, and the sparse grid is then created directly from this minimal configuration.
The corresponding Figure 4.3 below shows the resulting sparse grid in the standard domain $[0,1]^2$.
The second example extends this basic setup by explicitly setting the domain to $[-1,1]^2$, choosing different node generation functions for the two dimensions, switching to the doubling level-to-grid-point conversion, and adding a boundary level offset.
Figure 4.4 below depicts the sparse grid obtained from this customized configuration.

<!-- label: tab:a_new_ct_module_for_sgpp:functionality:sg_gen_instr:attributes -->
**Table 4.3.** Attributes of the different `SGGenInstr`s.

| Class | Attribute | Description |
|---|---|---|
| `SGGenInstr` (base class) | `boundaryLevelOffset` | Offsets the level of the boundary compared to the level of the main axis by the set amount. |
| `SGGenInstr` (base class) | `domain` | The domain over which the `SparseGrid` will span. Has to be a hyperrectangle. |
| `SGGenInstr` (base class) | `nodeGenFuncs` | `NodeGenFunc`s used for the grid points. Each dimension has its own function, which allows for different node types in different dimensions. |
| `SGGenInstr` (base class) | `lvl2GPCntFuncs` | `Lvl2GPCntFunc`s used to determine the number of inner grid points a level corresponds to. Each dimension has its own function. |
| `CompleteSGGenInstr` (inherits all base class attributes) | `maxLvl` | Specifies the maximum level $\ell_{max}$ used to generate the complete sparse grid according to $\|\vec{i}\|_1 \le \ell_{max}$. |
| `MIVecSGGenInstr` (inherits all base class attributes) | `miVec` | `std::shared_ptr` to a `LvlMIVec`, representing a selection of tensor grids that should be used for the sparse grid generation. Note that this attribute is only a pointer. |
| `MIVecSGGenInstr` (inherits all base class attributes) | `assumeDownwardsClosedness` | Whether downwards-closedness of the `miVec` should be assumed. Helps optimizing. |

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:sg_gen_instr:simple_full_sg_gen_instr_example -->
**Listing 4.7.** Simple example demonstrating `CompleteSGGenInstr`.

```cpp
CompleteSGGenInstr genInstr(2 /*maxLvl*/, 2 /*nDim*/);

SparseGrid sg(genInstr);
```

<!-- label: fig:a_new_ct_module_for_sgpp:functionality:sg_gen_instr:simple_full_sg_gen_instr_example -->
**Figure 4.3.** Sparse grid generated by Listing 4.7 above.
*Image: `figures/grids/sparse grids/2d_full_2m_0,1domain.png`*

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:sg_gen_instr:advanced_full_sg_gen_instr_example -->
**Listing 4.8.** Advanced example demonstrating `CompleteSGGenInstr`.

```cpp
CompleteSGGenInstr genInstr(2 /*maxLvl*/, 2 /*nDim*/);
genInstr.setDomain({-1, 1}); // Domain [-1, 1]^d
// Equidistant node in x dimension and sec. kind Chebyshev nodes in y dimension
genInstr.setNodeGenFuncs({ getEquidistantNodeGenFunc(), getSecondTypeChebyshevNodeGenFunc() });
// Doubling Lvl2GPCntFunc in all dimensions
genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
genInstr.setBoundaryLevelOffset(1);

SparseGrid sg(genInstr);
```

<!-- label: fig:a_new_ct_module_for_sgpp:functionality:sg_gen_instr:advanced_full_sg_gen_instr_example -->
**Figure 4.4.** Sparse grid generated by Listing 4.8 above.
*Image: `figures/grids/sparse grids/2d_full_2m_equi_cc_1blo_-1,1domain.png`*

### Sparse Grids

Sparse grids are the core data structure of the module and implemented by the class `SparseGrid`.
A `SparseGrid` object essentially consists only of a `std::vector`, storing the data for all contained tensor grids.
Each element in the vector stores the following attributes corresponding to one tensor grid:

- *Multi-index*: The multi-index $\vec{i}$ describing the tensor grid.
- *Coefficient*: The CT coefficient corresponding to the tensor grid.
- *Tensor grid*: The tensor grid itself.

For performance reasons, only tensor grids with a non-zero CT coefficient are stored.
`SparseGrid`s can be constructed using the constructor of the class.
This requires a `SGGenInstr` of arbitrary type.
See Listing 4.9 and Figure 4.5 below for an illustration.

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:sparse_grids:example -->
**Listing 4.9.** Example demonstrating the structure of a `SparseGrid`.

```cpp
CompleteSGGenInstr genInstr(2 /*maxLvl*/, 2 /*nDim*/);

SparseGrid sg(genInstr);
```

<!-- label: fig:a_new_ct_module_for_sgpp:functionality:sparse_grids:example -->
**Figure 4.5.** Contents of the sparse grid `sg` from Listing 4.9 above.
*Image: `figures/tensor grid tableaus/full_sg_2_2 + coefficients and mis.pdf`*

### Tensor Grids

Similar to sparse grids, tensor grids are also implemented in a class of the same name, `TensorGrid`.
From a user's standpoint, a tensor grid can be imagined as a $d$-dimensional `std::vector` containing all the grid points in the tensor grid.
The class semantically behaves exactly like this.
Internally, grid points are only stored implicitly and assembled on demand.
Refer to Section 5.3 (Tensor Grid Representation) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md) for detailed information about the internal functionality.

Listing 4.10 below illustrates the basic structure and usage of a `TensorGrid`.
A sparse grid is first generated from a `CompleteSGGenInstr` with maximal level $1$ in two dimensions, and the tensor grid with multi-index $(0,0)$ is then retrieved from it.
The example shows that the corresponding tensor grid can be accessed through the stored `TensorGridCTData` object and that its grid points can be obtained as a collection of `DataVector`s.
The commented output demonstrates how the tensor grid contains all grid points of the underlying full tensor product structure.

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:tensor_grids:example -->
**Listing 4.10.** Example demonstrating the structure of a `TensorGrid`.

```cpp
CompleteSGGenInstr genInstr(1 /*maxLvl*/, 2 /*nDim*/);
SparseGrid sg(genInstr);

/* We retrieve the tensor grid (0, 0) */
TensorGridCTData tgData = *sg.getTensorGrid({0, 0});
TensorGrid tg = tgData.tensorGrid;

std::vector<DataVector> gridPoints = tg.getGridPoints();
/* gridPoints equals {{0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0},
                      {0.0, 0.5}, {0.5, 0.5}, {1.0, 0.5},
                      {0.0, 1.0}, {0.5, 1.0}, {1.0, 1.0}} */
```

### Mathematical Operators

The module currently offers two mathematical operators that can be applied to a source function using a sparse grid.
These are quadrature and global polynomial interpolation.
They can be used by calling the corresponding functions `quadrature` and `interpolate`.
New custom operators can be added easily.

*Quadrature* allows for the approximation of the integral

$$
\int_{\Omega} f(\vec{x}) \, d\vec{x} \approx \sum_{\vec{i}} w_{\vec{i}} \cdot f(\vec{x}_{\vec{i}}),
$$

with $f: \mathbb{R}^d \rightarrow \mathbb{R}$ being the source function, $\vec{x}_{\vec{i}}$ the grid points, $w_{\vec{i}}$ the quadrature weights, and $\Omega$ the domain of the sparse grid.
The corresponding static function `quadrature` takes a `SourceFunc` and a `SparseGrid` as inputs and returns a `double` as the quadrature result.
The quadrature rules utilized depend on the chosen node types for the sparse grid.
Table 4.4 below shows the employed quadrature rules for the different node types.

<!-- label: tab:a_new_ct_module_for_sgpp:functionality:mathematical_operators:quadrature_rules -->
**Table 4.4.** Employed quadrature rules for the different node types.

| Grid point type | Quadrature rule |
|---|---|
| Equidistant | Simpson quadrature rule (Equation (5.12) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md)) if the number of grid points is odd. Trapezoidal quadrature rule (Equation (5.14) in the same file) otherwise. |
| Chebyshev sec. kind | Clenshaw-Curtis quadrature rule (Equation (5.16) in the same file) |

*Global polynomial interpolation* approximates the source function $f: \mathbb{R}^d \rightarrow \mathbb{R}$ through a $d$-dimensional global polynomial that matches the function values $f(\vec{x}_i)$ at all grid points $\vec{x}_i$.
The corresponding static function `interpolate` takes a `SourceFunc`, a point to evaluate the polynomial, and a `SparseGrid` as inputs.
It returns the evaluation of the interpolant at the specified point as a `double`.
As with the quadrature, the interpolation methods used depend on the type of nodes in the sparse grid.
The actual pairings can be seen in Table 4.5 below.
Currently, computing the polynomial interpolant itself is not supported by default.

<!-- label: tab:a_new_ct_module_for_sgpp:functionality:mathematical_operators:interpolation_methods -->
**Table 4.5.** Employed interpolation methods for the different node types.

| Grid point type | Interpolation method |
|---|---|
| Equidistant | Barycentric formula of the second kind (Equation (5.4) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md)) |
| Chebyshev sec. kind | Optimized Barycentric formula of the second kind for Chebyshev nodes of the second kind |

*Linear interpolation* approximates the source function by a piecewise multilinear interpolant.
In one dimension, linear interpolation matches the source function at neighboring grid points and connects these values by a straight line.
In higher dimensions, the corresponding generalization is multilinear interpolation, which is obtained by tensor products of one-dimensional linear basis functions.
The static function `interpolateLinear` takes a `SourceFunc`, a point to evaluate the interpolant, and a `SparseGrid` as inputs.
It returns the value of the multilinear interpolant at the specified point as a `double`.
Unlike quadrature and global polynomial interpolation, no distinction between different node types is made for this operator.

### 4.5.9 Primitive data types {#sec:a_new_ct_module_for_sgpp:functionality:primitive_data_types}

Table 4.6 below lists the most important usages where the choice of the primitive data types matters the most, defaults and whether they can be modified.
Changeable primitive types can be adjusted in the `type_defs` file.
The relevant excerpt is shown in Listing 4.11 below.

<!-- label: tab:a_new_ct_module_for_sgpp:functionality:primitive_data_types -->
**Table 4.6.** Usages, defaults, and modifiability of primitive data types.

| Usage | Default | Modifiable? | Notes |
|---|---|---|---|
| Levels | `unsigned int` | Yes | |
| Grid point counts | `unsigned int` | Yes | |
| CT coefficients | `int` | Yes | |
| Source function values | `double` | No | The data type is hard-coded into the SG++ base. |
| Quadrature weights | `double` | Yes/No | Weights need to be cast to `double` for usage, but they may be of arbitrary type beforehand. |
| Interpolation weights (Barycentric formula) | `double` | Yes/No | Weights need to be cast to `double` for usage, but they may be of arbitrary type beforehand. |

<!-- label: lst:a_new_ct_module_for_sgpp:functionality:type_defs_file -->
**Listing 4.11.** Section of the `type_defs` file showing the primitive data type choices.

```cpp
/*****************************************************************************
-Variable types-
Can be changed to better suit specific needs.
Larger data types are necessary when dealing with higher levels, coefficients,
or number of gridpoints. Smaller data types may increase performance.
*****************************************************************************/
using LvlType = unsigned int;
using LvlMI = MI<LvlType>;
using LvlMIVec = MIVec<LvlType>;

using GPCntType = unsigned int;
using GPMI = MI<GPCntType>;

using CTCoeffType = int;
```

## References cited in this chapter

None (this chapter contains no `\cite` commands in the source; all mentions of SG++ and other frameworks refer back to citations resolved in earlier chapters, see [03-existing-frameworks-and-motivation.md](03-existing-frameworks-and-motivation.md)).
