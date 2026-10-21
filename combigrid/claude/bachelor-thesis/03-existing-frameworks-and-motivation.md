---
source: bachelor thesis/thesis/chapters/existing_frameworks_and_motivation_for_a_new_approach.tex (+ "chapters/sections/existing frameworks and motivation for a new approach/{existing_frameworks,motivation}.tex")
chapter: 3
title: Existing Frameworks and Motivation for a new Approach
---

# 3 Existing Frameworks and Motivation for a new Approach {#sec:existing_frameworks_and_motiviation_for_a_new_approach}

A number of sparse grid frameworks have been proposed to facilitate the implementation of high-dimensional numerical methods.
While these frameworks share common theoretical foundations, they differ significantly in terms of design choices, capabilities, employed algorithms, performance characteristics, and usability.

To guide the development of a new sparse grid framework, this work first examines selected existing approaches.
This chapter introduces these frameworks and analyzes their key concepts, providing the basis for the motivation and design decisions discussed in the subsequent sections.

## 3.1 Existing Sparse Grid Frameworks {#sec:existing_frameworks_and_motiviation_for_a_new_approach:existing_frameworks}

In this work, four representative and widely used open-source frameworks are selected for analysis: SG++ [SG++ Doxygen, @sgpp_doxygen; SG++ Website, @sgpp; SG++ GitHub, @sgpp_github], Sparse Grids Matlab Kit (SGMK) [Piazzola & Tamellini 2024, @sparse_grids_matlab_kit; SGMK GitHub, @sgmk_github; Piazzola & Tamellini, @sgmk_website; Piazzola & Tamellini, @sgmk_manual], Tasmanian [Stoyanov, @tasmanian_github; Stoyanov, @tasmanian_doxygen; Stoyanov 2019, @stoyanov2015tasmanian], and sparseSpACE [Obersteiner, @sparseSpACE].
The selection covers a diverse range of implementation strategies and levels of abstraction, allowing for a comprehensive comparison of existing approaches.
As the combination technique (CT) is the central focus of the thesis, the main attention is given to the CT parts.

The purpose of this section is not merely to list features, but to analyze how the individual design choices influence the usability and technical properties of the frameworks.
Some systems are optimized for a high level of accessibility and extensive documentation, whereas others are designed to give experienced users more direct control over the numerical workflow.
Likewise, some frameworks provide a very broad range of operators and grid types, while others concentrate on a smaller number of methods but implement them in a particularly elegant or efficient manner.
These differences are relevant because they illustrate the trade-offs that arise when sparse grid methods are turned into software.

Each framework is introduced individually in the following sections, highlighting its core concepts, design decisions, and relevant features.

### 3.1.1 Sparse Grids Matlab Kit {#sec:existing_frameworks_and_motiviation_for_a_new_approach:existing_frameworks:sgmk}

The Sparse Grids Matlab Kit (SGMK) is an extensive sparse grid library for MATLAB that is fully based on the combination technique.
It offers source function evaluation, quadrature, global polynomial interpolation, and plotting as basic operations, while also providing more advanced operators such as polynomial chaos expansion (PCE), gradient and Hessian computation, and Sobol index estimation.
Main developers of the kit are Lorenzo Tamellini from the CNR-IMATI in Pavia and Chiara Piazzola from the Technical University of Munich [Piazzola & Tamellini, @sgmk_website; Piazzola & Tamellini, @sgmk_manual].

One of the key strengths of the SGMK is its comprehensive documentation and explanations.
This makes it incredibly easy to get started with, as well as to understand the theoretical background.
The documentation consists of a manual, around 15 thousand lines of examples and documentation of the actual code.
Almost every functionality is covered in the examples.
Additionally, the kit also offers many visualization capabilities [Piazzola & Tamellini, @sgmk_website; SGMK GitHub, @sgmk_github; Piazzola & Tamellini, @sgmk_manual].

MATLAB and the structure of the code play a very important role in making the approaches taken understandable.
The code for the core concepts is, besides some handling of edge cases, very compact and on a high-level.
This makes it possible to provide explanations and insights at an almost pseudocode level of abstraction and compactness.
Abstraction here refers to the abstraction from low-level code and implementation details, not theoretical concepts.
Precisely these are what is kept.

Another key strength of the SGMK is the magnitude of features.
Besides the already mentioned operations, it supports 15 pre-defined nested and non-nested node types.
They all come with their own quadrature rule and interpolation methods.
Various different types of sparse grid creation methods for the CT are supported.
These range from non-adaptive methods for full and arbitrary regular sparse grids, to adaptive methods supported by different profit indicators and local sensitivity analysis [SGMK GitHub, @sgmk_github; Piazzola & Tamellini, @sgmk_website; Piazzola & Tamellini, @sgmk_manual].
An extensive overview over the features and architecture of SGMK can be found in Appendix B (Additional Information about SGMK's Architecture and Capabilities) in file [09-appendix.md](09-appendix.md).

A typical effect which often comes with this kind of abstraction is the loss of performance.
However, MATLAB optimizations such as just-in-time (JIT) compilation and the parallel computing toolbox ensure a solid runtime performance, as we will see later on.
The real problem comes from the memory usage.
SGMK uses a lot of memory by storing redundancies and by relying on the MATLAB Runtime Environment.
This implies high memory requirements and many cache misses in practice.
Reasons for this will be discussed in the comparison later on [SGMK GitHub, @sgmk_github; Piazzola & Tamellini, @sgmk_manual].

On the downside, the kit also suffers from technical debt.
Earlier versions of the SGMK were primarily focused on quadrature.
As a result, quadrature weights were computed and stored for every generated sparse grid, irrespective of its intended use.
This behavior persists in the current versions [SGMK GitHub, @sgmk_github; Piazzola & Tamellini, @sgmk_manual].

### 3.1.2 Tasmanian {#sec:existing_frameworks_and_motiviation_for_a_new_approach:existing_frameworks:tasmanian}

Tasmanian is a versatile software framework that provides a collection of modules for solving high-dimensional problems.
It employs both the hierarchical sparse grid representation and the combination technique.
Its functionality includes quadrature, interpolation, parameter calibration, and sampling approaches for probability distributions similar to Markov Chain Monte Carlo (MCMC) methods.
The framework is developed by the research group of Miroslav Stoyanov at the Oak Ridge National Laboratory in Tennessee [Stoyanov, @tasmanian_github; Stoyanov 2019, @stoyanov2015tasmanian].

The core functionality is written in C++ which generally allows for increased performance compared to higher-level languages.
Programming interfaces for Python, MATLAB, and Fortran are also supported.
Tasmanian offers a unique feature in the form of a custom command line interface (CLI) that is comparable to the Python CLI.
This feature allows for direct interaction with the Tasmanian features [Stoyanov, @tasmanian_github; Stoyanov, @tasmanian_doxygen].

Another one of Tasmanian's major qualities is the extensive range of implemented sparse grid representation and generation methods.
It features two grid types, global sparse grid for global polynomials and a slightly altered variant for points generated by greedy optimization problems, based on the combination technique.
As well as, two grid types for the hierarchical representation and a special fourier grid [Stoyanov 2019, @stoyanov2015tasmanian].

Tasmanian provides documentation at multiple levels.
At the low-level, comprehensive Doxygen documentation is available for the source code.
At the mid-level, numerous examples illustrate functionality, structure, and design decisions.
At the high-level, a detailed mathematical manual describes the underlying theoretical concepts.
While these artifacts provide valuable documentation individually, their combined coverage is lacking in important aspects.
The main problem is located on the mid-level.
The examples are not sufficient for the size of the framework.
This deficit makes it hard to get started, get an overview over the capabilities, or to understand the approaches taken by the framework.
To obtain this information, manual search on the low-level is required by traversing the structure and examples, or by inferring it from the theoretical concepts.
One specific example we encountered during the research, is the usage of the combination technique.
It is not transparent whether, where and in what form the CT is employed [Stoyanov, @tasmanian_doxygen; Stoyanov, @tasmanian_github; Stoyanov 2019, @stoyanov2015tasmanian].

The framework is designed for advanced users, offering a very open, non-restrictive architecture.
This represents a completely valid design choice.
However, it should be noted that SGMK's architecture offers the same amount of openness and non-restrictiveness in some areas, while providing a superior usability.
This is not the case in every area, such as grid storage handling, but in some.

In conclusion, Tasmanian's main strengths lie in its variety of sparse grid types and its open, non-restrictive architecture.
It is suited to users who are well acquainted with the theoretical concepts and low-level code writing.
The framework offers a high degree of control by shifting responsibilities to the user.

### 3.1.3 SG++ {#sec:existing_frameworks_and_motiviation_for_a_new_approach:existing_frameworks:sgpp}

SG++ is an extensive toolbox for various high-dimensional methods based on sparse grids.
The functionality ranges from basic operation likes interpolation and quadrature to more advanced ones like partial differential equations (PDEs), regression, classification and much more.
It supports various spatially adaptive methods and the combination technique for sparse grids.
The framework is developed and maintained by the Scientific Computing group of Prof. Dr. Dirk Pflüger at the University of Stuttgart [SG++ GitHub, @sgpp_github].

All critical parts are written in C++ for efficient execution.
In addition to C++, SG++ does also support Python, MATLAB, and Java through interfaces.

The framework is composed of different modules that implement the aforementioned features.
In this work, we focus primarily on the combigrid module, which implements the combination technique in SG++.

The module itself supports polynomial interpolation using $d$-linear (nodal basis), global and b-spline polynomials on sparse grids.
Quadrature is not directly supported by the module but the design allows for an implementation by the user or through additional libraries.
The same applies to more advanced operations such as UQ methods which are based on interpolation and quadrature.
In addition to the construction of complete sparse grids and arbitrary ones based on multi-index sets, the combigrid module also supports dimension-adaptive generation [SG++ GitHub, @sgpp_github].

One significant downside of the module is the documentation.
The module's code is documented sufficiently and shows a high code quality.
Two well-annotated examples are also included which makes comprehension easier.
However, the documents besides the mentioned ones are out-dated and address an earlier version of the module.
This includes the GitHub wiki and the Doxygen documentation.
All the stated information is obsolete because the differences between these two versions are so significant [SG++ GitHub, @sgpp_github; SG++ Website, @sgpp].

Similarly to Tasmanian, the module is designed to shift the responsibility in some areas to the user.
One example for this is the source function evaluation.
Both implementations require the user to evaluate the source function at required points.
This is a valid design choice but there are superior approaches to this, as the SGMK shows.
In SGMK, the internal classes take full control on where and when source functions are evaluated, but the way of evaluation is still in full control of the user.
This enables improved internal optimizations, modularity, usability (especially when using adaptive refinement), and much more.
The performance of the evaluation does not significantly suffer from this because batched and parallel evaluations are still possible like before.
Also, the openness of the interface is not worse than before.
Users can still feed in evaluations as desired with all the benefits from this approach [SG++ GitHub, @sgpp_github].

Another weakness of the combigrid module are the optimizations of the code itself.
The most impactful deficiency is the single threaded design of the internal core.
It should be noted that user-written parts can still be parallelized if implemented accordingly, but the core would still be a performance bottleneck [SG++ GitHub, @sgpp_github].

### 3.1.4 sparseSpACE {#sec:existing_frameworks_and_motiviation_for_a_new_approach:existing_frameworks:sparseSpACE}

sparseSpACE is a library written in Python that implements the combination technique with a focus on adaptivity.
The functionality ranges from quadrature and linear interpolation to PDEs, UQ, regression, and density estimation.
Main developer of this library is Dr. Michael Obersteiner from the University of Munich [Obersteiner, @sparseSpACE].

Spatial adaptivity is the key technique that sets sparseSpACE apart from other frameworks.
This approach allows for spatial refinements of sparse grids based on local profit indicators.
It combines concepts from the hierarchical form of sparse grids with the combination technique [Obersteiner, @sparseSpACE; Piazzola & Tamellini, @sgmk_manual; SG++ Website, @sgpp; Stoyanov 2019, @stoyanov2015tasmanian].

Apart from the spatial adaptability, the other features provided by the library are also significant, but some concepts are missing when compared to those of the SGMK or Tasmanian.
It provides no implementation for dimension adaptivity, a slightly more limited selection of node, and no global interpolation [Obersteiner, @sparseSpACE].

The library is build on NumPy [NumPy Developers 2025, @numpy], which significantly improves performance compared to a pure Python implementation.
NumPy provides many optimizations such as vectorization, multi-threading, and ahead-of-time (AOT) compiled code.
However, sparseSpACE still suffers significantly from the disadvantages Python has in terms of runtime and memory.

In terms of usability, the library is ahead of the SG++ module and Tasmanian, as it provides a clearer structure and more support for tasks [Obersteiner, @sparseSpACE].
Unfortunately, this comes with a more restrictive architecture than the others.
It does not achieve the degree of usability and support SGMK offers, while also ensuring openness, modularity, and extensibility [SG++ GitHub, @sgpp_github; Obersteiner, @sparseSpACE; Stoyanov, @tasmanian_github].

Another important weakness of sparseSpACE is the structure and documentation.
It lacks a further documentation or wiki besides the decently commented code.
Each core file contains numerous classes, which makes the navigability and understandability difficult [Obersteiner, @sparseSpACE].

In conclusion, sparseSpACE is an excellent tool.
It offers the unique spatial adaptivity and is easy to use if you are familiar with the theoretical concepts.
It is ideal for quick prototyping and applications where performance is not the limiting factor.
The framework may not be ideal for users who are still becoming familiar with the underlying theoretical concepts.
In such cases, the SGMK may provide a more accessible starting point.

## 3.2 Motivation for a new Approach {#sec:existing_frameworks_and_motiviation_for_a_new_approach:motivation}

The analysis of the existing sparse grid frameworks shows that each of the investigated systems contains valuable concepts and well-suited design decisions.
At the same time, it also reveals that all of them leave room for improvement.
This observation is particularly relevant for the SG++ combigrid module, which is the direct predecessor of the module developed in this thesis.

The existing combigrid module already provides a solid foundation for sparse grid computations based on the combination technique.
It is integrated into the broader SG++ ecosystem and supports several important sparse grid operations.
Nevertheless, the framework analysis in the previous section has shown that the module suffers from a number of structural and technical limitations.
Some of these limitations affect usability and maintainability, while others directly impact runtime performance or restrict the flexibility of the interface.
For this reason, a redesign of the module is not merely an incremental refactoring effort, but a necessary step toward a more robust, extensible, and performant implementation.

A central issue of the old combigrid module is its internal architecture.
The module has grown over time and contains several design decisions that are no longer optimal from a modularity or software engineering perspective.
In particular, the internal structure does not yet expose the sparse grid workflow in a sufficiently clear and composable way.
As a result, certain responsibilities are distributed in a way that is neither especially transparent for users nor especially convenient for extending the code base.
A more structured architecture is therefore required, one that separates the essential steps of sparse grid construction and application more clearly.

The comparison with the SGMK was especially influential in this regard.
Even though the kit is implemented in MATLAB and therefore follows a different technical environment, it demonstrates several design principles that are highly desirable for a new SG++ combigrid module.
Its code is compact, well-structured, and conceptually close to the mathematical formulation of the algorithms.
This makes the implementation easy to understand and provides a clear mapping between theory and software.
At the same time, the kit shows that a sparse grid framework can remain open and extensible even when it offers a comparatively high-level interface.
These properties are particularly valuable for the present work, because they combine usability with a strong potential for future extensions.

Another important motivation arises from performance considerations.
Sparse grid methods are computationally intensive, and the combination technique is often used specifically because it offers a favorable trade-off between accuracy and computational cost in high dimensions.
Consequently, the implementation should not introduce unnecessary overhead.
The analysis of the existing frameworks shows that good performance is possible in different programming environments, provided that the architecture is designed with this goal in mind.
In the case of the new module, the implementation in C++ is therefore a deliberate choice.
It allows fine-grained control over memory management, data structures, and low-level optimizations, which is essential for a framework that is intended to serve as a computational core.

Taken together, these considerations show that the existing SG++ combigrid implementation provides an important starting point, but does not fully meet the requirements of a modern sparse grid framework in terms of structure, extensibility, usability, and performance.
The goal of this thesis is therefore to develop a new module that addresses these weaknesses while preserving the strengths of the existing approaches and the SG++ ecosystem.

## References cited in this chapter

- **[SG++ Doxygen, @sgpp_doxygen]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library Doxygen Documentation." <https://sgpp.github.io/SGpp/>. Accessed 2026-04-18.
- **[SG++ Website, @sgpp]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library Website." <https://sgpp.sparsegrids.org>. Accessed 2026-04-18.
- **[SG++ GitHub, @sgpp_github]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library GitHub." <https://github.com/SGpp/SGpp>. Accessed 2026-04-18.
- **[Piazzola & Tamellini 2024, @sparse_grids_matlab_kit]** C. Piazzola, L. Tamellini. "Algorithm 1040: The Sparse Grids Matlab Kit — a Matlab implementation of sparse grids for high-dimensional function approximation and uncertainty quantification." *ACM Transactions on Mathematical Software*, 50(1), 2024. DOI: 10.1145/3630023.
- **[SGMK GitHub, @sgmk_github]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit GitHub." <https://github.com/lorenzo-tamellini/sparse-grids-matlab-kit>. Accessed 2026-04-18.
- **[Piazzola & Tamellini, @sgmk_website]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit Website." <https://sites.google.com/view/sparse-grids-kit>. Accessed 2026-04-18.
- **[Piazzola & Tamellini, @sgmk_manual]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit Manual." 2023. <https://drive.google.com/file/d/1e0yeJ7IGUq3G014JD0q3-GUQLCjyA44n/view>. Accessed 2026-04-18.
- **[Stoyanov, @tasmanian_github]** M. Stoyanov. "TASMANIAN GitHub." <https://github.com/ORNL/TASMANIAN>. Accessed 2026-04-18.
- **[Stoyanov, @tasmanian_doxygen]** M. Stoyanov. "TASMANIAN Doxygen Documentation." <https://ornl.github.io/TASMANIAN/rolling/group__TasmanianSG.html>. Accessed 2026-04-18.
- **[Stoyanov 2019, @stoyanov2015tasmanian]** M. Stoyanov. "User Manual: TASMANIAN Sparse Grids." Technical Report ORNL/TM-2015/596, Oak Ridge National Laboratory, 2019.
- **[Obersteiner, @sparseSpACE]** Dr. rer. nat. Michael Obersteiner. "Sparse Grid spatially adaptive combination environment." <https://github.com/obersteiner/sparseSpACE>. Accessed 2026-04-18.
- **[NumPy Developers 2025, @numpy]** NumPy Developers. *NumPy*, version 2.4.0, 2025. <https://numpy.org/>.
