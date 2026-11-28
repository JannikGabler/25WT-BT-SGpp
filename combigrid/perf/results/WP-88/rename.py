#!/usr/bin/env python3
"""WP-88 rename tool: NodeGenFunc -> NodeFamily, sg_gen_instruction -> sparse_grid_generation_instruction.

Usage (from the repo root):
  rename.py moves              git mv of all renamed files (run once)
  rename.py apply FILE...      rewrite the given files in place
  rename.py print FILE         print the rewritten content of FILE to stdout (for patches)

The substitutions are ordered (longest / most specific first). Prose replacements cover the
common forms of "node-generation function(s)"; remaining prose is fixed by hand.
"""
import re
import subprocess
import sys

SRC = "combigrid/src/sgpp/combigrid"
NGF = f"{SRC}/functions/node_generation_functions"
NF = f"{SRC}/node_families"
GI = f"{SRC}/sparse_grid_generation_instructions"

MOVES = [
    (f"{GI}/sg_gen_instruction.hpp", f"{GI}/sparse_grid_generation_instruction.hpp"),
    (f"{GI}/sg_gen_instruction.cpp", f"{GI}/sparse_grid_generation_instruction.cpp"),
    (f"{GI}/complete_sg_gen_instruction.hpp", f"{GI}/complete_sparse_grid_generation_instruction.hpp"),
    (f"{GI}/complete_sg_gen_instruction.cpp", f"{GI}/complete_sparse_grid_generation_instruction.cpp"),
    (f"{GI}/multiindex_vector_sg_gen_instruction.hpp",
     f"{GI}/multiindex_vector_sparse_grid_generation_instruction.hpp"),
    (f"{GI}/multiindex_vector_sg_gen_instruction.cpp",
     f"{GI}/multiindex_vector_sparse_grid_generation_instruction.cpp"),
    (f"{NGF}/node_generation_function.hpp", f"{NF}/node_family.hpp"),
    (f"{NGF}/node_generation_function.cpp", f"{NF}/node_family.cpp"),
]
for fam in ["equidistant", "first_type_chebyshev", "second_type_chebyshev"]:
    for ext in ["hpp", "cpp"]:
        MOVES.append((f"{NGF}/default_functions/{fam}_node_generation_function.{ext}",
                      f"{NF}/{fam}_nodes.{ext}"))
for fam in ["clenshaw_curtis", "equidistant", "first_type_chebyshev", "second_type_chebyshev"]:
    for ext in ["hpp", "cpp"]:
        MOVES.append((f"{NGF}/getter/{fam}_node_generation_function_getter.{ext}",
                      f"{NF}/getter/{fam}_nodes_getter.{ext}"))
for ext in ["hpp", "cpp"]:
    MOVES.append((f"{SRC}/tools/functions/node_generation_functions/"
                  f"equidistant_node_generation_function_utilities.{ext}",
                  f"{SRC}/tools/node_families/equidistant_nodes_utilities.{ext}"))
MOVES += [
    ("combigrid/tests/node_generation_functions/node_generation_functions_test.cpp",
     "combigrid/tests/node_families/node_families_test.cpp"),
    ("combigrid/tests/node_generation_functions/delta_node_generation_functions_test.cpp",
     "combigrid/tests/node_families/delta_node_families_test.cpp"),
]

# (pattern, replacement); applied in order with re.sub.
SUBS = [
    # include paths / file names
    (r"tools/functions/node_generation_functions/equidistant_node_generation_function_utilities",
     "tools/node_families/equidistant_nodes_utilities"),
    (r"functions/node_generation_functions/default_functions/", "node_families/"),
    (r"functions/node_generation_functions/getter/", "node_families/getter/"),
    (r"functions/node_generation_functions/node_generation_function\b", "node_families/node_family"),
    (r"tools/functions/node_generation_functions/", "tools/node_families/"),
    (r"functions/node_generation_functions/", "node_families/"),
    (r"\bnode_generation_functions/", "node_families/"),
    (r"_node_generation_function_getter\b", "_nodes_getter"),
    (r"_node_generation_function_utilities\b", "_nodes_utilities"),
    (r"\b(equidistant|first_type_chebyshev|second_type_chebyshev)_node_generation_function\b",
     r"\1_nodes"),
    (r"\bdelta_node_generation_functions_test\b", "delta_node_families_test"),
    (r"\bnode_generation_functions_test\b", "node_families_test"),
    (r"\bnode_generation_function\.(hpp|cpp)\b", r"node_family.\1"),
    (r"\bnode_generation_functions\b", "node_families"),
    (r"sg_gen_instruction\b", "sparse_grid_generation_instruction"),
    # namespaces
    (r"\bequidistant_node_gen_function\b", "equidistant_nodes"),
    (r"\bnode_gen_funcs\b", "node_families"),
    # getters and classes
    (r"\bget(ClenshawCurtis|Equidistant|FirstTypeChebyshev|SecondTypeChebyshev)NodeGenFunc\b",
     r"get\1Nodes"),
    (r"\b(Equidistant|FirstTypeChebyshev|SecondTypeChebyshev)NodeGenFunc\b", r"\1Nodes"),
    (r"\b(clenshawCurtis|equidistant|firstTypeChebyshev|secondTypeChebyshev)NodeGenFunc\b",
     r"\1Nodes"),
    (r"NodeGenFuncs\b", "NodeFamilies"),
    (r"nodeGenFuncs\b", "nodeFamilies"),
    (r"NodeGenFunc", "NodeFamily"),
    (r"nodeGenFunc", "nodeFamily"),
    # prose
    (r"\b([Nn])ode[- ]generation functions\b", r"\1ode families"),
    (r"\b([Nn])ode[- ]generation function\b", r"\1ode family"),
    (r"\b([Nn])ode[- ]generation rules\b", r"\1ode families"),
    (r"\b([Nn])ode[- ]generation rule\b", r"\1ode family"),
]


def rewrite(text: str) -> str:
    for pattern, repl in SUBS:
        text = re.sub(pattern, repl, text)
    return text


def main() -> None:
    cmd = sys.argv[1]
    if cmd == "moves":
        for old, new in MOVES:
            subprocess.run(["mkdir", "-p", new.rsplit("/", 1)[0]], check=True)
            subprocess.run(["git", "mv", old, new], check=True)
    elif cmd == "apply":
        for path in sys.argv[2:]:
            with open(path, encoding="utf-8") as f:
                old = f.read()
            new = rewrite(old)
            if new != old:
                with open(path, "w", encoding="utf-8") as f:
                    f.write(new)
                print(path)
    elif cmd == "print":
        with open(sys.argv[2], encoding="utf-8") as f:
            sys.stdout.write(rewrite(f.read()))
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
