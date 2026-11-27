# Classifies the reports of tsan.sh: pairs with both accesses inside one parallel region of the
# refinement are real races; a pair with one access outside the region (main thread before / after
# it, or inside an earlier region such as the grid construction) is ordered only by libgomp's
# fork / join, which ThreadSanitizer does not see (libgomp is not instrumented).
import collections
import re
import sys

text = open(sys.argv[1]).read()
header = re.compile(r"^  (Previous )?(atomic )?(Atomic )?(read|write|Read|Write) of size \d+ at \S+ "
                    r"by (main thread|thread T\d+)")
pairs = collections.Counter()
for report in text.split("WARNING: ThreadSanitizer")[1:]:
    accesses, current = [], None
    for line in report.split("\n"):
        if header.match(line):
            current = [line]
            accesses.append(current)
        elif current is not None:
            current = None if line.strip() == "" else (current.append(line) or current)
    kinds = []
    for access in accesses[:2]:
        frames = "\n".join(access)
        region = "refinement region" if "RefinementEngine::run" in frames and "_omp_fn." in frames \
            else ("other region" if "_omp_fn." in frames else "outside regions")
        kinds.append(region)
    pairs[tuple(kinds)] += 1
for kinds, count in pairs.most_common():
    print(count, " / ".join(kinds))
