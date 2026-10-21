---
source: bachelor thesis/thesis/chapters/appendix.tex
chapter: Appendix (A-D)
title: Appendix
---

# Appendix

## Appendix A: Use of AI Tools {#sec:appendix:use_of_ai_tools}

<!-- label: tab:appendix:use_of_ai_tools -->
**Table A.1.** Overview of all AI tools and their respective use cases.

| AI Tool | Used for | Used where |
|---|---|---|
| DeepL Write | Grammar and spelling correction, as well as rephrasing of individual sentences and paragraphs | Entire thesis |
| ChatGPT5 | Assisting with questions related to the English language, LaTeX, C++, and programming in general | Entire thesis & SG++ implementation |
| ChatGPT5 | Generation of test cases, benchmarking procedures, and plotting scripts | SG++ implementation |

## Appendix B: Additional Information about SGMK's Architecture and Capabilities {#sec:appendix:sgmk_uml}

The UML diagram shown in Figures B.1 and B.2 below provides an overview over the architecture of the Sparse Grids Matlab Kit (SGMK).
It was used during the framework analysis in Section 3 (Existing Frameworks and Motivation for a new Approach) in file [03-existing-frameworks-and-motivation.md](03-existing-frameworks-and-motivation.md).
Figure B.2 is a more compact variant intended for the print version of this work.

<!-- label: fig:appendix:sgmk_uml:original -->
**Figure B.1.** Original variant for the digital PDF.
*Image: `figures/appendix/SGMK UML Original.pdf`*

<!-- label: fig:appendix:sgmk_uml:compacted -->
**Figure B.2.** Compacted variant for the print.
*Image: `figures/appendix/SGMK UML Compacted.pdf`*

<!-- TODO: conversion unclear -->
Both figures are rendered UML class diagrams supplied as PDF files; no vector/text source (e.g., PlantUML) was found in the repository, so the class structure itself could not be transcribed here. A future session needing the concrete class relationships of the SGMK should open the PDFs directly.

## Appendix C: Implementations of the Comparison Example for the different Frameworks {#sec:appendix:implementation_comparison_example_for_different_frameworks}

This appendix lists the full source code of the forward uncertainty quantification (UQ) benchmark example (see Section 7, Comparison between the Frameworks, in file [07-comparison-between-frameworks.md](07-comparison-between-frameworks.md)) for all four compared frameworks. All four implement the integrand

$$
f(\vec{x}) = \prod_{i=1}^d \left( 4 (1 - x_i)^3 \, e^{-x_i/d} \right)
$$

and then set up and evaluate a sparse grid quadrature for it, each using that framework's own API.

<!-- label: lst:appendix:implementation_comparison_example_for_different_frameworks:sgpp -->
**Listing C.1.** SG++.

```cpp
// Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
double integrant(const DataVector& x) {
  double prod = 1.0;
  double sum = 0.0;

  for (size_t dim = 0; dim < x.size(); dim++) {
    const double v = (1 - x[dim]);
    prod *= 4 * v * v * v;
    sum += x[dim];
  }

  return prod * std::exp(-sum / x.size());
}

double run(const size_t nDim, const size_t maxLvl) {
  // === 1. Source func ===
  const SourceFunc sourceFunc(integrant);

  // === 2. Sparse Grid Generation Instruction ===
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

  // === 3. Sparse Grid Generation ===
  const SparseGrid sg(genInstr);

  // === 4. Quadrature ===
  const double result = quadrature(sourceFunc, sg);

  return result;
}
```

<!-- label: lst:appendix:implementation_comparison_example_for_different_frameworks:sgmk -->
**Listing C.2.** Sparse Grids Matlab Kit.

```matlab
% Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
function result = integrant(x)
    d = numel(x); % Determines the length of x
    result = prod( 4 * (1 - x).^3 .* exp(-x / d) );
end

function result = run(nDim, maxLvl)
  % === 1. Nodes Specification ===
  knots=@(n) knots_CC(n,0,1,'nonprob');

  % === 2. Sparse Grid Generation ===
  S = create_sparse_grid(nDim, maxLvl+1, knots, @lev2knots_doubling); % Levels are shifted by 1

  % === 3. Sparse Grid Reduction ===
  Sr = reduce_sparse_grid(S);

  % === 4. Quadrature ===
  result = quadrature_on_sparse_grid(@integrant, S, Sr, [], [], [], 0);
end
```

<!-- label: lst:appendix:implementation_comparison_example_for_different_frameworks:tasmanian -->
**Listing C.3.** Tasmanian.

```cpp
// Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
double integrant(const size_t pointIdx, const std::vector<double>& points) {
    double prod = 1.0;
    double sum = 0.0;

    for (size_t dim = 0; dim < nDim; dim++) {
        const double v = (1 - points[pointIdx + dim]);
        prod *= 4 * v * v * v;
        sum += points[pointIdx + dim];
    }

    return prod * std::exp(-sum * invDimCnt);
}

double run(const size_t nDim, const size_t maxLvl) {
    // === 1. Sparse Grid Generation ===
    TasmanianSparseGrid grid;
    grid.makeGlobalGrid(nDim, 1, maxLvl, type_level, rule_clenshawcurtis);

    // === 2. Sparse Grid Configuration ===
    const std::vector<double> lowerBounds(nDim, 0.0);
    const std::vector<double> upperBounds(nDim, 1.0);
    grid.setDomainTransform(lowerBounds, upperBounds);

    // === 3. Source Function Evaluation ===
    const size_t gridCnt = grid.getNumNeeded();
    const auto points = grid.getNeededPoints(); // Returns all sparse grid points
    std::vector<double> values(gridCnt);

    for (size_t i = 0; i < gridCnt; i++) {
        values[i] = integrant(i * nDim, points);
    }

    grid.loadNeededValues(values);

    // === 4. Quadrature ===
    return grid.integrate()[0];
}
```

<!-- label: lst:appendix:implementation_comparison_example_for_different_frameworks:sparseSpACE -->
**Listing C.4.** SparseSpACE.

```python
# Class that realizes the integrant function
class Integrant(Function):
    def __init__(self, n_dim: int):
        super().__init__()
        self.n_dim = n_dim

    # Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
    def eval(self, coordinates: np.ndarray) -> float:
        arr = np.pow(1 - coordinates, 3)
        factor1 = (4 ** self.n_dim) * np.prod(arr1)

        factor2 = -np.sum(coordinates)
        factor2 /= self.n_dim
        factor2 = math.exp(factor2)

        return factor1 * factor2

    # Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
    def eval_vectorized(self, coordinates) -> np.ndarray:
        # coordinates: shape (n_points, n_dim)
        arr1 = (1 - coordinates) ** 3
        factor1 = (4 ** self.n_dim) * np.prod(arr1, axis=1)

        factor2 = -np.sum(coordinates, axis=1) / self.n_dim
        factor2 = np.exp(factor2)

        return factor1 * factor


def run(n_dim: int, max_level: int):
    # === 1. Preparation phase ===
    lower_domain_bounds = np.zeros(n_dim)
    upper_domain_nounds = np.ones(n_dim)

    # Only specifies what type of grid to use. Does not generate the sparse grid.
    grid = ClenshawCurtisGrid(a=lower_domain_bounds , b=upper_domain_nounds, \
                              boundary=True)

    operation = Integration(f=Integrant(n_dim), grid=grid, dim=n_dim)

    # === 2. Sparse Grid Generation ===
    combiObject = StandardCombi(a, b, operation=operation)

    # === 3. Quadrature phase ===
    _, _, result = combiObject.perform_operation(1, max_level+1) # Levels are shifted by 1

    return result
```

<!-- TODO: conversion unclear -->
Listing C.4 (SparseSpACE) contains apparent typos/bugs carried over verbatim from the thesis source (`chapters/appendix.tex`): `eval` references an undefined `arr1` (the array was assigned to `arr`), `eval_vectorized` returns `factor1 * factor` (an undefined name, likely meant to be `factor2`), and `run` uses undefined names `a`, `b`, and `upper_domain_nounds` (likely meant to be `upper_domain_bounds`) instead of `lower_domain_bounds`/the correctly spelled upper bound variable. These are preserved as-is rather than silently fixed, since the instructions require never inventing content; treat this listing as illustrative pseudocode rather than as runnable Python.

## Appendix D: Bash Benchmarking Script {#sec:appendix:bash_benchmarking_script}

<!-- label: lst:appendix:bash_benchmarking_script -->
**Listing D.1.** Benchmarking Script.

```bash
#!/usr/bin/env bash
# Configuration
file="$1"

MAX_RAM_MB=8000;
MAX_RAM_KiB=$((MAX_RAM_MB * 977))

MAX_RAM_DURATION_SEC=4000

MIN_WARMUPS=1;
MIN_RUNS=3;
MIN_WARMUP_DURATION_SEC=15;
MIN_RUN_DURATION_SEC=45

# Main loop
for dim in 2 3 5 7 10; do
  maxLvl=3
  while true; do
    echo "d: $dim, lmax: $maxLvl"

    ram_start=$EPOCHREALTIME
    (
        systemd-run --user --quiet --scope -p MemoryMax=${MAX_RAM_KiB}K -p MemorySwapMax=0 timeout --kill-after=5s "${MAX_RAM_DURATION_SEC}s" /usr/bin/time -v "$file" --warmups 0 --runs 1 --d $dim --lmax $maxLvl > /dev/null 2> time.tmp
    )
    exit_code=$?
    ram_end=$EPOCHREALTIME; ram_duration_micro_seconds=$(awk -v start="$ram_start" -v end="$ram_end" 'BEGIN {
        print int((end - start) * 1000000)
    }')

    if [ $exit_code -eq 124 ] || [ $ram_duration_micro_seconds -gt $((MAX_RAM_DURATION_SEC * 1000000)) ]; then
        echo "Timeout in the RAM run through after $ram_duration_micro_seconds microseconds."
        [ -f time.tmp ] && cat time.tmp
        break
    else
        if [ $exit_code -ne 0 ]; then
            echo "Out of memory or error."
            cat time.tmp
            break
        else
          ceil=$(( (MIN_WARMUP_DURATION_SEC * 1000000 + ram_duration_micro_seconds - 1) / ram_duration_micro_seconds ))
          warmups=$(( MIN_WARMUPS > ceil ? MIN_WARMUPS : ceil ))
          ceil=$(( (MIN_RUN_DURATION_SEC * 1000000 + ram_duration_micro_seconds - 1) / ram_duration_micro_seconds ))
          runs=$(( MIN_RUNS > ceil ? MIN_RUNS : ceil ))

          echo "\n=== Run 1: Runtime ==="
          "$file" --warmups $warmups --runs $runs --d $dim --lmax $maxLvl

          echo "\n=== Run 2: RAM ==="
          peak_kb=$(grep "Maximum resident set size" time.tmp | awk '{print $6}')
          peak_mb=$(awk -v kb="$peak_kb" 'BEGIN { printf "%.2f", kb/1024 }')

          echo "Peak RAM: ${peak_mb} MB"

          echo "\n=== Run 3: Perf ==="
          perf stat "$file" --warmups 0 --runs 1 --d $dim --lmax $maxLvl | awk '/Performance counter stats for/{flag=1} flag'
        fi
    fi
    rm -f time.tmp

    ((maxLvl++))
  done
done
```

(Note: the original LaTeX source escapes every `$` in this listing as `$\$$` and one literal `microseconds` as `$\mu$s` because it is typeset via `lstlisting` inside a LaTeX document; both have been unescaped here to their plain-Bash form, since this file is meant to be read as actual shell script content, not typeset LaTeX.)

## References cited in this appendix

None.
