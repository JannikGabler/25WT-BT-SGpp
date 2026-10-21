---
source: bachelor thesis/thesis/chapters/introduction.tex
chapter: 1
title: Introduction
---

# 1 Introduction {#sec:introduction}

The curse of dimensionality remains one of the central obstacles in the numerical treatment of high-dimensional problems.
Whenever a method based on a full tensor grid is extended from one to several dimensions, the number of grid points grows exponentially, and the computational effort quickly becomes prohibitive.
This affects many areas of scientific computing in which high-dimensional interpolation, quadrature, or uncertainty quantification are required.
Sparse grids offer a way to reduce this growth while retaining much of the attractive approximation behavior of full grid methods [Pflüger 2010, @pflueger_diss; Garcke 2012, @sparse_grids_in_a_nutshell].

Within sparse grid methods, the combination technique (CT) has become an especially important approach because it builds sparse grid approximations from tensor-grid solutions.
This makes it possible to reuse existing full-grid algorithms and to exploit the inherent independence of the tensor-grid subproblems.
As a result, the combination technique provides a conceptually elegant and practically useful bridge between classical tensor-grid methods and sparse grid computation [Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations].

The present thesis is concerned with the design and implementation of a new combination technique module for SG++ [SG++ Website, @sgpp].
The motivation for this work arises from several observations regarding both the existing implementation and the broader development of sparse grid software.

The combination technique module in SG++ has evolved over multiple development iterations.
As is common in such cases, this evolution has led to the accumulation of legacy structures, including outdated documentation and design decisions that no longer align well with current requirements.
While the existing module provides a functional basis, these aspects motivate a reconsideration of its structure and a more systematic design [SG++ GitHub, @sgpp_github; SG++ Doxygen, @sgpp_doxygen].

At the same time, the surrounding ecosystem of sparse grid frameworks has continued to progress.
In particular, frameworks such as the Sparse Grids Matlab Kit [Piazzola & Tamellini, @sgmk_website] and sparseSpACE [Obersteiner, @sparseSpACE] have introduced new ideas and implementation strategies in recent years.
These developments highlight alternative design approaches and emphasize the importance of modularity, clarity, and extensibility in modern sparse grid software.

In addition, practical experience with earlier implementations has revealed limitations of certain design choices.
For example, approaches such as a purely header-based implementation in C++ can complicate maintainability and extensibility as the code base grows.
Reflecting on such experiences provides valuable guidance for the design of a revised module with a clearer structure and improved long-term sustainability.
Against this background, this thesis aims to develop a new combination technique module for SG++ that emphasizes a modular and comprehensible structure.

To support this goal, the thesis first establishes the mathematical and conceptual foundations of sparse grids and the combination technique.
Section 2 (Sparse Grids, file [02-sparse-grids.md](02-sparse-grids.md)) introduces the relevant concepts and terminology that form the basis for the subsequent implementation.
Section 3 (Existing Frameworks and Motivation for a new Approach, file [03-existing-frameworks-and-motivation.md](03-existing-frameworks-and-motivation.md)) discusses existing sparse grid frameworks and outlines the motivation for a revised approach in SG++.
Building on this, Section 4 (A new Combination Technique Module for SG++, file [04-new-ct-module-for-sgpp.md](04-new-ct-module-for-sgpp.md)) presents the newly developed module from a user-oriented perspective, while Section 5 (Algorithmic and Design Choices, file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md)) examines the key algorithmic and design decisions.
Section 6 (Application, file [06-application.md](06-application.md)) illustrates the use of the module in representative application scenarios, Section 7 (Comparison between the Frameworks, file [07-comparison-between-frameworks.md](07-comparison-between-frameworks.md)) provides a comparison with other frameworks, and Section 8 (Conclusion and Outlook, file [08-conclusion-and-outlook.md](08-conclusion-and-outlook.md)) concludes the thesis and discusses possible directions for future work.

Overall, this work addresses the question of how a combination technique module for SG++ can be structured in a way that balances mathematical rigor, practical usability, and long-term maintainability.
By focusing on both conceptual clarity and implementation aspects, the thesis contributes to the development of a flexible and extensible foundation for sparse grid methods within SG++.

## References cited in this chapter

- **[Pflüger 2010, @pflueger_diss]** Dirk Pflüger. *Spatially Adaptive Sparse Grids for High-Dimensional Problems*. Verlag Dr. Hut, 2010. ISBN 978-3-86853-555-6.
- **[Garcke 2012, @sparse_grids_in_a_nutshell]** Jochen Garcke. "Sparse grids in a nutshell." In *Sparse grids and applications*, pp. 57–80. Springer, 2012.
- **[Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations]** Markus Hegland, Jochen Garcke, Vivien Challis. "The combination technique and some generalisations." *Linear Algebra and its Applications*, 420(2-3):249–275, Elsevier, 2007.
- **[SG++ Website, @sgpp]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library Website." <https://sgpp.sparsegrids.org>. Accessed 2026-04-18.
- **[SG++ GitHub, @sgpp_github]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library GitHub." <https://github.com/SGpp/SGpp>. Accessed 2026-04-18.
- **[SG++ Doxygen, @sgpp_doxygen]** Scientific Computing group of Prof. Dr. Dirk Pflüger, University of Stuttgart. "SG++ Sparse Grids Library Doxygen Documentation." <https://sgpp.github.io/SGpp/>. Accessed 2026-04-18.
- **[Piazzola & Tamellini, @sgmk_website]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit Website." <https://sites.google.com/view/sparse-grids-kit>. Accessed 2026-04-18.
- **[Obersteiner, @sparseSpACE]** Dr. rer. nat. Michael Obersteiner. "Sparse Grid spatially adaptive combination environment." <https://github.com/obersteiner/sparseSpACE>. Accessed 2026-04-18.
