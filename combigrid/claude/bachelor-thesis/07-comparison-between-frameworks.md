---
source: bachelor thesis/thesis/chapters/comparison_between_the_frameworks.tex (+ chapters/sections/comparison_between_the_frameworks/{benchmarking_procedure,results,discussion}.tex)
chapter: 7
title: Comparison between the Frameworks
---

# 7 Comparison between the Frameworks {#sec:comparison_between_the_frameworks}

This chapter compares the newly developed sparse grid combination technique (CT) module for SG++ with the three other frameworks from the initial analysis.
Namely, these are the Sparse Grids Matlab Kit (SGMK), Tasmanian, and SparseSpACE.
The comparison is based on the previous forward uncertainty quantification (UQ) example, Equation (6.8) in file [06-application.md](06-application.md), introduced in Section 6.3 (Forward Uncertainty Quantification of an Electron Beam Through One-Dimensional Layers) of the same file.
This example is chosen because all four frameworks support quadrature and therefore allows for a meaningful comparison under a common setting.

To ensure a fair comparison, the same problem formulation and the same configuration parameters are used in all frameworks.
Complete sparse grids are employed with Chebyshev nodes of the second kind in all dimensions, boundary points are included, the doubling level conversion (Equation (2.26) in file [02-sparse-grids.md](02-sparse-grids.md)) is used, and the computational domain is the unit hypercube $[0,1]^d$.
These settings are selected because they are available in all four frameworks and do not favor any specific implementation.

The comparison focuses on four aspects.
First, the user code required to implement the example is examined, since the programming effort and the resulting usability are important practical criteria.
Second, the runtime of the quadrature computation is analyzed.
Third, the peak memory consumption is considered.
Fourth, the CPU utilization is discussed in terms of the number of actively used cores.

As already noted earlier in Section 3.1.3 (SG++) in file [03-existing-frameworks-and-motivation.md](03-existing-frameworks-and-motivation.md), the old SG++ combination technique module has only limited functionality and does not provide a direct quadrature implementation.
While quadrature is realizable with that module, it would require the user to implement the complete quadrature logic manually.
For the present comparison, this approach was deliberately not pursued.
Doing so would have been outside the scope of the work, would have required considerable additional effort, and would not have added much value to the comparison, since our quadrature design and implementation is already integrated into the new module.

The following sections describe the benchmarking procedure, present the results, and discuss the observed differences between the frameworks.

## 7.1 Benchmarking Procedure {#sec:comparison_between_the_frameworks:benchmarking_procedure}

The UQ example is implemented in each framework following the stated setting.
The number of dimensions $d$ and the maximum level $l_{\max}$ of the full sparse grid are treated as variable input parameters.

An automated Bash script is used to execute each implementation for all values $d \in \{2, 3, 5, 7, 10\}$.
For each number of dimensions, the script starts with $l_{\max} = 3$ and increases this value incrementally by one after each successful execution.
An execution is considered successful if it completes within one hour and does not exceed a memory consumption of 8 GB.

More precisely, for each parameter pair $(d, l_{\max})$, the script first performs a single execution to determine the peak memory usage using the Linux command `time -v` [Kerrisk 2025, @man7_time].
This initial run also serves to verify compliance with the predefined resource constraints.
The execution is carried out within a Linux cgroup that enforces both a memory limit of 8 GB and a time limit of one hour [Heo 2015, @kerneldocs_cgroups].
If either constraint is violated, the execution is terminated and marked as failed, and the script proceeds with the next number of dimensions $d$.

If the constraints are satisfied, the measured peak memory usage is recorded and the runtime evaluation phase begins.
In this phase, the implementation is executed multiple times to obtain reliable runtime measurements.
The reported runtime corresponds to the average over several runs following a warm-up period.
The warm-up phase includes at least one full execution and lasts for a minimum of 15 seconds.
The subsequent measurement phase consists of at least three full executions and has a minimum duration of 45 seconds.

The runtime measurement is implemented within the program itself and is triggered only once by the Bash script.
In contrast to the initial memory measurement, this execution is not externally monitored to avoid any interference with the runtime results.

Finally, an additional execution is performed to collect further performance metrics, such as CPU utilization and page faults, using the Linux command `perf stat` [Kerrisk 2024, @man7_perf].
The full Bash script can be seen in Appendix D (Bash Benchmarking Script, Listing D.1) in file [09-appendix.md](09-appendix.md).

The benchmarks are conducted on a standard desktop system equipped with an AMD Ryzen 5 1600X CPU and 16 GB of main memory.
Up to 8 cores and 8 GB of memory were allowed to be used by the implementations.
The system runs a fresh installation of the Debian operating system (Trixie, version 13.4).
To ensure consistent and reproducible results, no additional user applications are executed during the benchmarking process beyond the operating system and the benchmarking workloads themselves.

## 7.2 Results {#sec:comparison_between_the_frameworks:results}

Having established the benchmarking procedure, this results section first discusses the implementation itself before turning to the measured performance.
Since the code listings are comparatively large, they are provided in Appendix C (Implementations of the Comparison Example for the different Frameworks) in file [09-appendix.md](09-appendix.md) rather than in the main text.
For this reason, the discussion below focuses on the structure of the implementations and on the interfaces exposed by the four frameworks, with particular emphasis on their understandability and structure in the context of the present example.

At a high level, all four implementations follow the same conceptual workflow: first, the integrand is defined, then the sparse grid is set up, and finally the quadrature is performed.
This common structure makes the example well suited for an interface comparison, because the essential mathematical task is identical across all frameworks.
Nevertheless, the amount of explicit user code and the degree of control required differ noticeably.
The SG++ implementation is comparatively compact, modular and closely mirrors the logical steps of the example: a source function is created, the sparse grid generation instruction is configured, the grid is generated, and the quadrature routine is called directly.
This gives the implementation a clear structure and makes the individual responsibilities easy to identify.

The SGMK, Tasmanian, and SparseSpACE examples are similarly based on a small set of steps, but their interfaces expose different levels of abstraction.
In the SGMK example, the user first creates the sparse grid and then explicitly reduces it before the quadrature is computed.
Tasmanian follows a related pattern in the sense that the grid is created, the relevant grid points are obtained, the integrand is evaluated at these points, and the resulting values are loaded back into the grid before integration is performed.
SparseSpACE also combines sparse grid construction and operation execution through a higher-level object that bundles the relevant setup steps.

A notable difference is that Tasmanian and the MATLAB-based SGMK example do not use the combination technique in the same way as described in this thesis, where the problem is solved on each tensor grid and the results are combined afterwards.
Instead, the sparse grid is reduced before the quadrature step.
This reduction collects all grid points of the resulting sparse grid without duplicates and assigns each point a weight given by the sum of the combination technique coefficients from all tensor grids in which that point would occur.
From the perspective of the user interface, this means that the quadrature is performed on the reduced sparse grid representation rather than directly on the individual tensor grids.

Overall, the code examples show that all four frameworks support the same mathematical example, but they differ in the underlying structure and in the level of abstraction exposed to the user.
In particular, SparseSpACE and Tasmanian show a more extensive and complex code than the new SG++ module and the Sparse Grid Matlab Kit.
The latter two provide a more modular workflow that is easier to understand.

### 7.2.1 Runtime

The complete runtime results are shown in Figures 7.3–7.7 below (at the end of this chapter).
However, to improve readability, a shortened overview over the runtime results is shown in Figure 7.1 below.
SparseSpACE performs worst in all tested configurations.
For every number of dimensions and every maximum level, its runtime is usually one to two orders of magnitude above the next best framework.
This gap is already visible for the two-dimensional case in Figure 7.3 below and remains present throughout the benchmark range, including the higher-dimensional cases in Figures 7.5–7.6 below.

The new SG++ module achieves the best runtime for low-dimensional problems.
For dimensions up to four, it is the fastest framework in the benchmark.
This is visible already in the two- and three-dimensional results in Figures 7.3–7.4 below.
However, the situation changes as the dimensionality increases.
For higher numbers of dimensions, Tasmanian and, in some configurations, the SGMK reach comparable or better runtimes.
Tasmanian in particular shows very strong scaling behavior and becomes the fastest framework for the higher-dimensional cases, as can be seen in Figures 7.5 and 7.7 below.
The SGMK also remains competitive and is often close to Tasmanian and the new SG++ module, although its runtime exhibits noticeably stronger fluctuations.

### 7.2.2 Memory consumption

The peak memory consumption is shown alongside the runtime in Figures 7.3, 7.4, 7.5, and 7.7 below.
The new SG++ module uses by far the least memory in all benchmark cases.
Its memory consumption remains low even when the dimensionality and the sparse-grid level increase, and none of the tested cases approach the 8 GB limit.
This is already evident in the two- and three-dimensional plots and remains true for the larger cases in Figures 7.5 and 7.7 below.
This makes memory usage the least restrictive resource for the new implementation.

Tasmanian lies in the middle of the spectrum.
Its memory consumption is clearly higher than that of the new SG++ module, but substantially lower than that of SparseSpACE and especially the SGMK.
In the benchmark, the memory limit becomes the primary stopping criterion for Tasmanian in most cases.

SparseSpACE shows a significantly higher memory footprint than Tasmanian and the new SG++ module.
Although it is still below the SGMK for almost all tested dimensions, its memory usage is already substantial for small inputs, as visible in Figures 7.3–7.4 below.
For lower dimensions, runtime is the limiting factor, whereas for higher dimensions the peak memory consumption eventually becomes the restricting resource.

The SGMK exhibits by far the largest memory consumption.
In many cases, its memory usage is already higher than that of SparseSpACE by a factor in the single-digit range and exceeds the memory usage of the new SG++ module by several orders of magnitude.
For all tested configurations, memory is the limiting resource, and the available 8 GB are reached long before runtime would become the dominant constraint.

<!-- label: fig:comparison_between_the_frameworks:results:overview -->
**Figure 7.1.** Runtime and memory comparison between UQ implementations for two dimensions.
*Image: `figures/benchmarking results/runtime_ram_subplots.pdf`*

### 7.2.3 CPU utilization

The CPU utilization results are shown in Figure 7.2 below.
The new SG++ module shows the best overall core usage.
Although the utilization is still modest for small dimensionalities, it increases steadily with the number of dimensions and approaches full utilization of the available 8 cores for the larger benchmark cases.
This indicates that the implementation is able to make increasingly effective use of the available hardware as the problem size grows.

Tasmanian also shows a clear tendency toward better parallel utilization at higher dimensions.
For small numbers of dimensions, only about 2 to 4 cores are used on average.
As the dimensionality increases, the number of active cores rises noticeably, and the framework appears to level off at around 6 of the 8 available cores.

The SGMK has a relatively stable core usage of roughly 3 to 4 cores.
This behavior is largely independent of the dimensionality and suggests that the available hardware is not exploited as effectively as in the new SG++ module or Tasmanian.

SparseSpACE performs worst together with the SGMK.
For low-dimensional cases, it often does not fully utilize even a single core.
Only for higher dimensions does it begin to use multiple cores more consistently, but even then the average utilization remains limited and typically reaches only around 6 cores at most.

<!-- label: fig:comparison_between_the_frameworks:results:cpu_utilization -->
**Figure 7.2.** Average CPU utilization for different number of dimensions.
*Image: `figures/benchmarking results/cpu_utilization_over_dimensions.pdf`*

## 7.3 Discussion {#sec:comparison_between_the_frameworks:discussion}

The benchmark results reveal several characteristic differences between the four frameworks that can be traced back to their implementation strategies and design choices.

SparseSpACE is the weakest framework in this comparison with respect to runtime, memory consumption, and CPU utilization.
Although it is implemented in Python and relies heavily on NumPy, it already incorporates several optimizations, such as batched evaluation of function values.
Nevertheless, the benchmark results show that these measures are not sufficient to compensate for the overall overhead of the implementation approach.
In particular, the runtime remains consistently high, the memory footprint is substantial, and the available CPU resources are only used to a limited extent.
This indicates that the framework's design does not scale well for the considered sparse grid quadrature task, especially once the dimensionality increases.

The new SG++ module achieves the best runtime for low-dimensional problems, which is especially visible in Figures 7.3–7.4 below.
This advantage can be attributed to the stronger parallelization and optimization of the new implementation.
The module is able to make efficient use of the available hardware even for relatively small problem sizes, which explains its good performance in the low-dimensional regime.
At higher dimensions, however, Tasmanian and the SGMK become increasingly competitive and can overtake the new SG++ module in runtime.
This is consistent with the usage of the direct combination-technique-based workflow, while the other two frameworks use a reduced sparse grid representation that is optimized for the subsequent quadrature.

For Tasmanian and the SGMK, the reduced representation of the sparse grid becomes increasingly beneficial as the dimensionality grows.
Instead of applying the quadrature directly on the tensor grids of the regular combination technique, both frameworks first construct a reduced sparse grid that contains all grid points of the final sparse grid exactly once, together with the accumulated combination coefficients.
This introduces additional overhead during sparse grid construction, since the reduction itself must be computed.
However, once the reduction has been completed, the actual quadrature becomes significantly cheaper because the number of points that must be processed is much smaller and the data are stored in a compact array-based representation that can be handled very efficiently.
This trade-off becomes particularly worthwhile in higher dimensions, where a large share of the points generated by the regular combination technique would otherwise appear as duplicates across multiple tensor grids.
As a result, the initial extra cost of the reduction is increasingly compensated by the faster quadrature phase.

The low memory consumption of the new SG++ module is mainly a consequence of the on-demand assembly of tensor grids, which was introduced and explained in Section 5.3 (Tensor Grid Representation) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md).
This design avoids storing all tensor grids of the sparse grid simultaneously and therefore keeps the memory footprint very small.
The effect of this decision becomes especially clear in comparison with the SGMK, which stores all contained tensor grids fully assembled in memory.
Thus, the memory results confirm that the new module is significantly more efficient in terms of storage requirements, even though this does not always translate into the best runtime for larger dimensions.

The comparatively high variance between the different $l_{max}$ values in the SGMK performance results is largely caused by the MATLAB Runtime Environment.
This is visible especially in the two-dimensional benchmark in Figure 7.3 below, where the runtime is still relatively high for smaller maximum levels but improves noticeably as the level increases.
This behavior is consistent with MATLAB's dynamic optimization mechanisms and JIT compilation, which can lead to substantial runtime fluctuations.
The same runtime environment is also responsible for the high baseline memory consumption.
Even before the actual sparse grid computations dominate the resource usage, the MATLAB runtime already imposes a comparatively large fixed memory overhead.

The lower CPU utilization of the SGMK and Tasmanian compared to the new SG++ module is also related to the generation process of the reduced sparse grid representation.
Although the final quadrature is highly parallelized, the reduction itself appears to offer only moderate parallelization.
This may explain why both frameworks do not exploit the available cores as effectively as the new SG++ module, especially in the low-dimensional benchmark cases.
At present, it is unclear whether this behavior is inherent to the reduction-based formulation or whether it could be improved by a more sophisticated parallel implementation.
This question is therefore a natural topic for future work.

The decrease in CPU utilization for SparseSpACE at ten dimensions compared to five dimensions can be explained by the structure of the benchmark itself.
The reported CPU utilization is averaged over all tested maximum levels.
For ten dimensions, SparseSpACE can only execute the smallest maximum levels before the memory limit is reached, namely levels 2 and 3.
These small instances generate very limited work per execution and therefore do not provide enough parallel workload to keep many cores busy.
As a consequence, the average CPU utilization shown in Figure 7.2 above decreases for ten dimensions, even though the framework uses somewhat more cores for the remaining admissible cases.

Overall, the benchmark shows a clear division of strengths.
The new SG++ module offers the most balanced combination of low memory usage, strong parallel utilization, and good runtime for low-dimensional problems.
Tasmanian is particularly attractive for higher-dimensional cases where its reduced sparse grid representation pays off in runtime.
The SGMK remains competitive in runtime but is limited by memory consumption and the overhead of the MATLAB runtime.
SparseSpACE, despite employing some useful optimizations, performs worst in all relevant metrics for the considered example.

<!-- label: fig:comparison_between_the_frameworks:results:runtime_and_memory_2d -->
**Figure 7.3.** Runtime and memory comparison between UQ implementations for two dimensions.
*Images: `figures/benchmarking results/runtime_2d.pdf` and `figures/benchmarking results/peak_memory_usage_2d.pdf`*

<!-- label: fig:comparison_between_the_frameworks:results:runtime_and_memory_3d -->
**Figure 7.4.** Runtime and memory comparison between UQ implementations for three dimensions.
*Images: `figures/benchmarking results/runtime_3d.pdf` and `figures/benchmarking results/peak_memory_usage_3d.pdf`*

<!-- label: fig:comparison_between_the_frameworks:results:runtime_and_memory_5d -->
**Figure 7.5.** Runtime and memory comparison between UQ implementations for five dimensions.
*Images: `figures/benchmarking results/runtime_5d.pdf` and `figures/benchmarking results/peak_memory_usage_5d.pdf`*

<!-- label: fig:comparison_between_the_frameworks:results:runtime_and_memory_7d -->
**Figure 7.6.** Runtime and memory comparison between UQ implementations for seven dimensions.
*Images: `figures/benchmarking results/runtime_7d.pdf` and `figures/benchmarking results/peak_memory_usage_7d.pdf`*

<!-- label: fig:comparison_between_the_frameworks:results:runtime_and_memory_10d -->
**Figure 7.7.** Runtime and memory comparison between UQ implementations for ten dimensions.
*Images: `figures/benchmarking results/runtime_10d.pdf` and `figures/benchmarking results/peak_memory_usage_10d.pdf`*

## References cited in this chapter

- **[Kerrisk 2025, @man7_time]** Michael Kerrisk. "time(1) — Linux manual page." 2025. <https://man7.org/linux/man-pages/man1/time.1.html>. Accessed 2026-04-18.
- **[Heo 2015, @kerneldocs_cgroups]** Tejun Heo. "Control Group v2." 2015. <https://docs.kernel.org/admin-guide/cgroup-v2.html>. Accessed 2026-04-16.
- **[Kerrisk 2024, @man7_perf]** Michael Kerrisk. "perf(1) — Linux manual page." 2024. <https://man7.org/linux/man-pages/man1/perf.1.html>. Accessed 2026-04-18.
