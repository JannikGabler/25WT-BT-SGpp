---
source: bachelor thesis/thesis/chapters/conclusion_and_outlook.tex (+ chapters/sections/conclusion_and_outlook/{conclusion,outlook}.tex)
chapter: 8
title: Conclusion and Outlook
---

# 8 Conclusion and Outlook {#sec:conclusion_and_outlook}

This final chapter summarizes the main contributions of this thesis and revisits the extent to which the initial goals have been achieved.
It then outlines several directions for future work that could further extend and improve the newly developed SG++ combination technique (CT) module.

## 8.1 Conclusion {#sec:conclusion_and_outlook:conclusion}

This thesis presented the design and implementation of a new sparse grid combination technique module for SG++.
The work was motivated by the limitations of the existing SG++ combigrid module and by a comparative analysis of three additional sparse grid frameworks, namely Tasmanian, the Sparse Grids Matlab Kit (SGMK), and sparseSpACE.
This analysis revealed several shortcomings of the previous approach: an internal structure and outdated documentation that had grown over time, a lack of features, a largely single-threaded core, and an architecture with optimization potential.
At the same time, it highlighted the requirements for a new module, namely a clearer and more composable architecture, improved performance, better usability, and a design that is easier to extend and integrates into the broader SG++ code base.
Based on these findings, the thesis developed an implementation that addresses the most important weaknesses of the former module while establishing a more modern and flexible foundation for combination-technique-based sparse grid computations.

One major contribution of this thesis is the comprehensive, self-contained presentation of the Combination Technique.
Compared to many existing introductions, the explanation was deliberately tailored to the combination technique itself and supported by many illustrations and new explanatory viewpoints.
This allowed the mathematical background, the sparse grid structure, and the role of tensor grids and combination coefficients to be developed in a form that is directly useful for the implementation discussion later in the thesis.
(See Section 2, Sparse Grids, in file [02-sparse-grids.md](02-sparse-grids.md).)

A second major contribution is the framework analysis and comparison in Section 3 (Existing Frameworks and Motivation for a new Approach, file [03-existing-frameworks-and-motivation.md](03-existing-frameworks-and-motivation.md)) and Section 7 (Comparison between the Frameworks, file [07-comparison-between-frameworks.md](07-comparison-between-frameworks.md)).
The investigation of the old SG++ module, Tasmanian, the SGMK, and sparseSpACE clarified which design decisions are advantageous and which limitations should be addressed in a new implementation.
In particular, the analysis showed that the old SG++ combigrid module already provided a solid foundation, but lacked a clear internal structure, direct quadrature support, and a sufficiently modern architecture.
The comparison also showed that Tasmanian and the SGMK offer strong performance in different regimes, while sparseSpACE provides useful ideas such as spatial adaptivity but suffers from comparatively weak performance as evident in the benchmarked setting.
These observations directly shaped the requirements for the new module.

The third major contribution is the design and implementation of new algorithms and design choices for the SG++ module.
Most notably, this includes the new on-demand tensor grid representation, which avoids storing all tensor grids explicitly and thereby reduces memory consumption substantially (see Section 5.3, Tensor Grid Representation, in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md)).
Another central contribution is the newly developed sparse grid generation algorithm for determining the Combination Technique coefficients for general downwards-closed multi-index sets, referred to in this thesis as the *dimension-separated algorithm for the combination technique coefficients* (see Section 5.6, Sparse Grid Generation, in the same file).
Together, these components form a more modular and more efficient computational core for combination-technique-based sparse grid computations.

A fourth contribution is the application of the module in three representative example scenarios (see Section 6, Application, in file [06-application.md](06-application.md)).
The examples of numerical quadrature, function interpolation, and forward uncertainty quantification demonstrate that the module is not limited to a single use case, but can be applied to a range of sparse-grid-based numerical tasks.
These examples also show how the new interface can be used in practice and how the module supports the workflow from source function definition to sparse grid generation and operator application.

Finally, the benchmark comparison between the four frameworks (Section 7, Comparison between the Frameworks, file [07-comparison-between-frameworks.md](07-comparison-between-frameworks.md)) confirms that the new SG++ module fulfills the goals set out in the motivation and requirements chapters to a substantial degree.
The module provides a clear internal structure, fits naturally into the broader SG++ code base, and offers strong memory efficiency.
Its runtime performance is particularly good for low-dimensional problems, while its CPU utilization is the best among the compared frameworks.
At the same time, the comparison also showed that the module does not always outperform every alternative in every regime: Tasmanian and the SGMK can become faster for higher-dimensional problems due to their reduced sparse-grid representation.
This result is not a weakness of the implementation itself, but rather reflects different design trade-offs between direct combination-technique processing and reduced-grid quadrature.

Taken together, the thesis achieves the goals formulated in the motivation and requirements chapters.
The new module addresses the structural weaknesses of the old SG++ combigrid implementation, improves memory usage substantially, provides direct quadrature support, and lays the foundation for further extensions.
In this sense, the work does not only present a new implementation, but also establishes a more coherent and extensible basis for future sparse grid development in SG++.

## 8.2 Outlook {#sec:conclusion_and_outlook:outlook}

Although the new module already covers the most important core functionality, several promising directions remain for future work.

A natural next step is the implementation of dimension-adaptive refinement.
This is the standard refinement strategy for the Combination Technique and is supported by most combination technique frameworks [Pflüger 2010, @pflueger_diss; Piazzola & Tamellini 2024, @sparse_grids_matlab_kit; Stoyanov 2019, @stoyanov2015tasmanian; SG++ Website, @sgpp].
Adding it to the new SG++ module would significantly broaden its applicability, especially for problems where the most efficient sparse grid cannot be determined a priori.

Another important extension is the implementation of additional node types.
At present, the module already supports equidistant nodes and second-kind Chebyshev nodes, but further distributions such as Leja nodes [De Marchi 2004, @de2004leja] should be added as well.
In particular, support for non-nested node families would improve flexibility and would make the module more useful for a wider class of approximation and quadrature problems.

The results of the framework comparison also suggest that a reduced sparse grid representation deserves further investigation.
Tasmanian and the SGMK showed that such a representation can be highly beneficial, especially in higher dimensions, where many tensor-grid points are duplicates across the regular Combination Technique decomposition.
A future version of the SG++ module could therefore explore whether a reduced representation can be integrated without sacrificing the advantages of the current on-demand tensor-grid design.

Related to this is further experimentation with the tensor grid representation itself.
The implementation developed in this thesis uses an on-demand assembly approach, but there may be alternative assembly strategies with different trade-offs between construction cost, memory usage, and evaluation speed.
A systematic comparison of such strategies would help determine whether the current design is optimal or whether additional optimization potential remains.

The boundary handling can also be refined further.
At present, the boundary level offset is configured as a single constant for all dimensions.
Allowing dimension-wise boundary level offsets would increase flexibility and would make it possible to adapt the grid structure more precisely to anisotropic problems.

On the algorithmic side, the module can be extended with additional operators.
Besides quadrature and global interpolation, operations such as uncertainty quantification workflows [Smith 2024, @smith2024uncertainty], polynomial chaos expansion [Xiu & Karniadakis 2002, @xiu2002wiener], or other sparse-grid-based analysis methods would make the framework substantially more versatile.

A further important extension concerns source functions with multi-dimensional codomains, that is, mappings from $\mathbb{R}^d$ to $\mathbb{R}^m$ instead of to $\mathbb{R}$.
Such functions can already be handled manually in principle, but native support would improve usability and could also enable more efficient implementations of vector-valued workflows.

Other promising directions include opticom (optimal combination technique) integration [Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations], graphics processing unit (GPU) support, and Python bindings.
Each of these would increase the practical relevance of the module in different ways: opticom and GPU support could open the door to additional performance gains, and Python bindings would make the module more accessible to users who prefer a higher-level interface.

Finally, the caching mechanisms for quadrature weights and source functions may be improved further.
The new module already supports source function caching.
However, there is still considerable room for improvement.
In particular, caching can currently only be enabled or disabled statically for all functions at once.
A more flexible approach, such as the one used in the SGMK, would be to activate or deactivate the cache dynamically depending on the evaluation time [Piazzola & Tamellini 2024, @sparse_grids_matlab_kit].
Since repeated evaluation is common in sparse grid computations, more aggressive or specialized caching strategies could reduce overhead and improve performance in both adaptive and non-adaptive settings.

In a different direction, spatial adaptivity would also be an interesting extension, especially in light of the ideas introduced by sparseSpACE [Obersteiner, @sparseSpACE].
Combining spatial and dimension-adaptive refinement could lead to a highly flexible framework for sparse grid computations on difficult anisotropic or localized problems.

Overall, the work presented in this thesis provides a solid foundation for future development.
The new SG++ combination technique module already offers a clear architecture, strong memory efficiency, and a practical implementation of the essential core functionality.
The extensions outlined above would turn it into an even more complete and powerful sparse grid framework in the long term.

## References cited in this chapter

- **[Pflüger 2010, @pflueger_diss]** Dirk Pflüger. *Spatially Adaptive Sparse Grids for High-Dimensional Problems*. Verlag Dr. Hut, 2010. ISBN 978-3-86853-555-6.
- **[Piazzola & Tamellini 2024, @sparse_grids_matlab_kit]** C. Piazzola, L. Tamellini. "Algorithm 1040: The Sparse Grids Matlab Kit — a Matlab implementation of sparse grids for high-dimensional function approximation and uncertainty quantification." *ACM Transactions on Mathematical Software*, 50(1), 2024. DOI: 10.1145/3630023.
- **[Stoyanov 2019, @stoyanov2015tasmanian]** M. Stoyanov. "User Manual: TASMANIAN Sparse Grids." Technical Report ORNL/TM-2015/596, Oak Ridge National Laboratory, 2019.
- **[SG++ Website, @sgpp]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library Website." <https://sgpp.sparsegrids.org>. Accessed 2026-04-18.
- **[De Marchi 2004, @de2004leja]** Stefano De Marchi. "On Leja sequences: some results and applications." *Applied mathematics and computation*, 152(3):621–647, Elsevier, 2004.
- **[Smith 2024, @smith2024uncertainty]** Ralph C. Smith. *Uncertainty quantification: theory, implementation, and applications*. SIAM, 2024.
- **[Xiu & Karniadakis 2002, @xiu2002wiener]** Dongbin Xiu, George Em Karniadakis. "The Wiener–Askey polynomial chaos for stochastic differential equations." *SIAM Journal on Scientific Computing*, 24(2):619–644, SIAM, 2002.
- **[Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations]** Markus Hegland, Jochen Garcke, Vivien Challis. "The combination technique and some generalisations." *Linear Algebra and its Applications*, 420(2-3):249–275, Elsevier, 2007.
- **[Obersteiner, @sparseSpACE]** Dr. rer. nat. Michael Obersteiner. "Sparse Grid spatially adaptive combination environment." <https://github.com/obersteiner/sparseSpACE>. Accessed 2026-04-18.
