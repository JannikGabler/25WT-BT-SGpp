---
source: bachelor thesis/thesis/chapters/abstract.tex
chapter: front matter
title: Abstract
---

# Abstract

High-dimensional numerical problems are often limited by the curse of dimensionality, which makes full grid methods computationally expensive.
Sparse grids mitigate this issue by reducing the number of grid points while retaining favorable approximation properties, and the combination technique is a particularly useful sparse grid approach because it constructs sparse-grid solutions from tensor-grid subproblems.
This thesis presents the design and implementation of a new combination technique module for SG++, motivated by limitations of the previous SG++ combigrid implementation and by a comparative analysis of existing sparse grid frameworks, including Tasmanian, the Sparse Grids Matlab Kit, and sparseSpACE.
Based on this analysis, the new module was developed with a clearer and more composable architecture, improved performance, and a more maintainable implementation.
A central contribution is the on-demand tensor grid assembly, which constructs tensor grids only when required, as well as a new algorithm for generating combination technique coefficients for general downwards-closed multi-index sets efficiently.
The module is evaluated through representative applications in quadrature, interpolation, and forward uncertainty quantification, and benchmarked against the selected frameworks.
The results show that the new SG++ module provides a robust and extensible foundation for combination-technique-based sparse grid computations, with particularly strong memory efficiency and good performance in low-dimensional settings.

## Kurzfassung (German abstract)

Hochdimensionale numerische Probleme werden häufig durch den sogenannten Fluch der Dimensionalität begrenzt, da sich Verfahren auf vollen Gittern mit steigender Dimension schnell als rechnerisch zu aufwendig erweisen.
Sparse Grids begegnen diesem Problem, indem sie die Anzahl der Gitterpunkte deutlich reduzieren und dennoch gute Approximationseigenschaften aufweisen.
Die Combination Technique ist dabei ein besonders nützlicher Ansatz, weil sie Sparse-Grid-Lösungen aus Tensorgitter-Teilproblemen zusammensetzt.
Diese Arbeit präsentiert die Konzeption und Implementierung eines neuen Combination-Technique-Moduls für SG++, motiviert durch Einschränkungen der bisherigen SG++-Combigrid-Implementierung sowie durch eine vergleichende Analyse bestehender Sparse-Grid-Frameworks wie Tasmanian, dem Sparse Grids Matlab Kit und sparseSpACE.
Auf Basis dieser Analyse wurde das neue Modul mit einer klareren, modulareren Architektur, verbesserter Performanz und einer besser wartbaren Implementierung entwickelt.
Ein zentraler Beitrag ist die On-Demand Assemblierung der Tensorgitter, durch die diese nur bei Bedarf konstruiert werden, sowie ein neuer Algorithmus zur effizienten Erzeugung der Combination-Technique-Koeffizienten für allgemeine nach unten abgeschlossene Multiindex-Mengen.
Das Modul wird anhand exemplarischer Anwendungen in der Quadratur, Interpolation und Forward Uncertainty Quantification evaluiert und zusätzlich mit ausgewählten Frameworks verglichen.
Die Ergebnisse zeigen, dass das neue SG++-Modul eine leistungsfähige und erweiterbare Grundlage für Sparse-Grid-Berechnungen auf Basis der Combination Technique bietet, insbesondere mit sehr guter Speichereffizienz und guter Laufzeit in niedrigen Dimensionen.

## References cited in this chapter

None.
