"""WP-40: compares the machine code of every function of two libsgppcombigrid.so builds.

Disassembles both (objdump -d, no raw bytes), drops addresses and absolute targets (code moves
when other functions change size) and reports the functions whose normalized instructions
differ, plus those only in one library.
Usage: python3 compare_text.py A/libsgppcombigrid.so B/libsgppcombigrid.so
"""
import re
import subprocess
import sys


def functions(lib):
    out = subprocess.run(["objdump", "-d", "--no-show-raw-insn", "-C", lib], capture_output=True,
                         text=True, check=True).stdout
    funcs, name = {}, None
    for line in out.splitlines():
        m = re.match(r"^[0-9a-f]+ <(.*)>:$", line)
        if m:
            name = m.group(1)
            funcs[name] = []
        elif name and line.startswith(" "):
            ins = line.split(":", 1)[1].strip() if ":" in line else line.strip()
            ins = ins.split("#", 1)[0].strip()  # objdump's comments: resolved addresses
            ins = re.sub(r"-?0x[0-9a-f]+\(%rip\)", "RIP(%rip)", ins)  # data moves with layout
            ins = re.sub(r"\b[0-9a-f]{4,}\b", "ADDR", ins)  # absolute branch / call targets
            ins = re.sub(r"\+0x[0-9a-f]+>$", "+off>", ins)  # branch targets inside a function
            funcs[name].append(ins)
    return funcs


a, b = functions(sys.argv[1]), functions(sys.argv[2])
changed = sorted(n for n in a.keys() & b.keys() if a[n] != b[n])
print(f"functions: A {len(a)}, B {len(b)}, common {len(a.keys() & b.keys())}, "
      f"identical {len(a.keys() & b.keys()) - len(changed)}, changed {len(changed)}")
for n in changed:
    print(f"  changed ({len(a[n])} -> {len(b[n])} instructions): {n[:160]}")
for n in sorted(a.keys() - b.keys()):
    print(f"  only in A: {n[:160]}")
for n in sorted(b.keys() - a.keys()):
    print(f"  only in B: {n[:160]}")
