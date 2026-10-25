# WP-13 — checks besides the A/B benchmarks (raw outputs)

## All modules: Boost tests (SG_ALL=1 SG_PYTHON=0 + judged flags, -j10; before = HEAD, after = with the change)
```
before (build 347 s; base/combigrid objects reused: SCons excludes include paths from the signature):
base: exit 0 (6s) 
combigrid: exit 0 (4s) 
datadriven: exit 0 (40s) 
misc: exit 0 (1s) 
optimization: exit 0 (1s) 
pde: exit 0 (86s) 
quadrature: exit 0 (0s) 
solver: exit 0 (8s) 
  base: 190 test cases out of 190 passed
  combigrid: 390 test cases out of 390 passed
  datadriven: 135 test cases out of 135 passed
  misc: 1 test case out of 1 passed
  optimization: 45 test cases out of 45 passed
  pde: 48 test cases out of 48 passed
  quadrature: 2 test cases out of 2 passed
  solver: 5 test cases out of 5 passed
after (build 520 s, full rebuild):
base: exit 0 (7s) 
combigrid: exit 0 (2s) 
datadriven: exit 0 (43s) 
misc: exit 0 (0s) 
optimization: exit 0 (1s) 
pde: exit 0 (88s) 
quadrature: exit 0 (0s) 
solver: exit 0 (7s) 
  base: 190 test cases out of 190 passed
  combigrid: 390 test cases out of 390 passed
  datadriven: 135 test cases out of 135 passed
  misc: 1 test case out of 1 passed
  optimization: 45 test cases out of 45 passed
  pde: 48 test cases out of 48 passed
  quadrature: 2 test cases out of 2 passed
  solver: 5 test cases out of 5 passed
flag on compile/link lines of the after build:
  base: compiled 425 (with flag 425), links 15 (with flag 15)
  combigrid: compiled 118 (with flag 118), links 32 (with flag 32)
  datadriven: compiled 334 (with flag 334), links 16 (with flag 16)
  misc: compiled 3 (with flag 3), links 2 (with flag 2)
  optimization: compiled 123 (with flag 123), links 5 (with flag 5)
  pde: compiled 105 (with flag 105), links 2 (with flag 2)
  quadrature: compiled 9 (with flag 9), links 2 (with flag 2)
  solver: compiled 19 (with flag 19), links 3 (with flag 3)
compiler warnings after: 7 (the 7 known base/combigrid ones; other modules: 0 before and after)

Differences in the logs (outcomes identical):
  optimization TestIterativeGridGenerators: 20093 -> 20100 assertions, deterministic (2 runs each, after test binary with before/after libs).
  probe (same loop as the test, prints n per grid and generator), diff before-libs vs after-libs:
    < grid 1 (bsplineBoundary) LS: n=198
    > grid 1 (bsplineBoundary) LS: n=199
    < grid 9 (weaklyFundamentalNakSplineBoundary) LS: n=198
    > grid 9 (weaklyFundamentalNakSplineBoundary) LS: n=199
    < grid 10 (weaklyFundamentalSplineBoundary) LS: n=198
    > grid 10 (weaklyFundamentalSplineBoundary) LS: n=199
    < grid 13 (linearBoundary) LS: n=198
    > grid 13 (linearBoundary) LS: n=199
    < grid 18 (naturalBsplineBoundary) LS: n=198
    > grid 18 (naturalBsplineBoundary) LS: n=199
    < grid 19 (nakBsplineBoundary) LS: n=198
    > grid 19 (nakBsplineBoundary) LS: n=199
    < grid 21 (waveletBoundary) LS: n=198
    > grid 21 (waveletBoundary) LS: n=199
  combigrid assertion counts and datadriven 'observed error' lines differ between any two runs (time / random_device seeded).
```

## Strong exported symbols defined in more than one lib/libsgpp*.so (nm -D --defined-only, not W/V/u)
```
before: 10448 strong exported symbols, 94 defined in more than one lib; after: 10446, 94 (same set)
kinds (none is a function):
        1 std::piecewise_construct
        1 std::_Sp_make_shared_tag::_S_ti()::__tag
       26 typeinfo for
       66 typeinfo name for
type letters per lib combination:
       29  base(R) datadriven(R) optimization(R)
        9  base(R) datadriven(R)
        8  base(R) datadriven(R) optimization(R) pde(R)
        7  base(D) datadriven(D)
        4  base(R) pde(R)
        4  base(R) datadriven(R) pde(R)
        4  base(R) combigrid(R) datadriven(R) optimization(R)
        4  base(D) datadriven(D) optimization(D)
        2  datadriven(R) optimization(R)
        2  base(R) optimization(R)
        2  base(R) datadriven(R) pde(R) solver(R)
        2  base(R) combigrid(R) datadriven(R)
        2  base(D) optimization(D)
        2  base(D) datadriven(D) pde(D) solver(D)
        2  base(D) datadriven(D) pde(D)
        2  base(D) datadriven(D) optimization(D) pde(D)
        2  base(D) combigrid(D) datadriven(D) optimization(D)
        1  datadriven(R) solver(R)
        1  datadriven(D) solver(D)
        1  datadriven(D) pde(D)
        1  datadriven(D) optimization(D)
        1  base(R) quadrature(R)
        1  base(D) quadrature(D)
        1  base(D) combigrid(D) datadriven(D)
relocations against these symbols (count lib type), before | after:
  201  libsgppbase          R_X86_64_64        201  libsgppbase          R_X86_64_64
  13   libsgppbase          R_X86_64_GLOB_DAT  13   libsgppbase          R_X86_64_GLOB_DAT
  8    libsgppcombigrid     R_X86_64_64        8    libsgppcombigrid     R_X86_64_64
  3    libsgppcombigrid     R_X86_64_GLOB_DAT  3    libsgppcombigrid     R_X86_64_GLOB_DAT
  162  libsgppdatadriven    R_X86_64_64        161  libsgppdatadriven    R_X86_64_64
  12   libsgppdatadriven    R_X86_64_GLOB_DAT  12   libsgppdatadriven    R_X86_64_GLOB_DAT
  97   libsgppoptimization  R_X86_64_64        97   libsgppoptimization  R_X86_64_64
  5    libsgppoptimization  R_X86_64_GLOB_DAT  5    libsgppoptimization  R_X86_64_GLOB_DAT
  83   libsgpppde           R_X86_64_64        83   libsgpppde           R_X86_64_64
  6    libsgpppde           R_X86_64_GLOB_DAT  6    libsgpppde           R_X86_64_GLOB_DAT
  2    libsgppquadrature    R_X86_64_64        2    libsgppquadrature    R_X86_64_64
  7    libsgppsolver        R_X86_64_64        7    libsgppsolver        R_X86_64_64
  1    libsgppsolver        R_X86_64_GLOB_DAT  1    libsgppsolver        R_X86_64_GLOB_DAT
exported symbols that changed: only two vague-linkage RTTI names no longer exported by datadriven (no undefined reference in any lib, test, example or _pysgpp_swig.so):
  < libsgppdatadriven.so R _ZTSN4sgpp10datadriven25DataTransformationBuilderE
  < libsgppdatadriven.so R _ZTSSt23_Sp_counted_ptr_inplaceIN4sgpp10datadriven16BlacsProcessGridESaIvELN9__gnu_cxx12_Lock_policyE2EE
```

## Dynamic relocations per library (before -> after)
```
lib                             JUMP_SLOT        R_X86_64_64           RELATIVE size
libsgppbase.so              855 -> 164         3265 -> 1619        1219 -> 2871    74895976 -> 76124376
libsgppcombigrid.so         179 -> 59           112 -> 74            51 -> 89      6852496 -> 6956720
libsgppdatadriven.so       1376 -> 385         2356 -> 1483        1091 -> 1951    67618216 -> 68189520
libsgppmisc.so                0 -> 0              0 -> 0              3 -> 3       52864 -> 52920
libsgppoptimization.so      600 -> 231         1779 -> 932          522 -> 1369    24528928 -> 24682208
libsgpppde.so               513 -> 180          864 -> 493           25 -> 395     13374416 -> 13307208
libsgppquadrature.so         37 -> 20            43 -> 26             5 -> 22      645400 -> 678904
libsgppsolver.so            102 -> 74           156 -> 90            11 -> 77      2298512 -> 2376592
```

## Equivalence with WP-11-gcc-nosi (snapshot files, ELF sections compared by SHA-256)
```
WP-13-before libsgppcombigrid.so: plt call/jmp sites 2139, localalias 7, SourceFunc::evaluate@plt 10, size 6852496
WP-11-gcc libsgppcombigrid.so: plt call/jmp sites 2139, localalias 7, SourceFunc::evaluate@plt 10, size 6852496
WP-11-gcc-nosi libsgppcombigrid.so: plt call/jmp sites 1739, localalias 85, SourceFunc::evaluate@plt 0, size 6956720
WP-13-after libsgppcombigrid.so: plt call/jmp sites 1739, localalias 85, SourceFunc::evaluate@plt 0, size 6956720
lib/libsgppcombigrid.so:
  .debug_str               differs: size 1097060 vs 1097060
  .debug_info              differs: size 3912592 vs 3912592
  .note.gnu.build-id       differs: size 36 vs 36
lib/libsgppbase.so:
  .debug_str               differs: size 12215501 vs 12215501
  .debug_info              differs: size 47173664 vs 47173664
  .note.gnu.build-id       differs: size 36 vs 36
bin/perf_e2e:
  .debug_str               differs: size 1047574 vs 1047574
  .debug_info              differs: size 1503487 vs 1503487
  .note.gnu.build-id       differs: size 36 vs 36
bin/perf_micro:
  .debug_info              differs: size 1828719 vs 1828719
  .debug_str               differs: size 1246587 vs 1246587
  .note.gnu.build-id       differs: size 36 vs 36
bin/perf_reference:
  .debug_info              differs: size 1570440 vs 1570440
  .debug_str               differs: size 1129544 vs 1129544
  .note.gnu.build-id       differs: size 36 vs 36
producer strings (DW_AT_producer, -march expansion removed):
  WP-11-gcc-nosi: GNU C++23 15.2.0 -D_FORTIFY_SOURCE=3 -mavx2 -mfma -mfpmath=sse -g -O3 -std=c++23 -flto -fno-semantic-interposition -fno-omit-frame-pointer -fno-strict-aliasing -funroll-loops -fopenmp -fPIC -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-pr
  WP-13-after: GNU C++23 15.2.0 -D_FORTIFY_SOURCE=3 -mavx2 -mfma -mfpmath=sse -g -O3 -std=c++23 -flto -fno-omit-frame-pointer -fno-strict-aliasing -funroll-loops -fno-semantic-interposition -fopenmp -fPIC -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-pr
compile line (quadrature.cpp), WP-11-gcc-nosi: g++ -o combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.os -c -fPIC -march=native -flto=auto -fno-semantic-interposition -O3 -DNDEBUG -fno-omit-frame-pointer -g -Wall -Wextra -Wcast-qual -
compile line (quadrature.cpp), WP-13 judged:   g++ -o combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.os -c -fPIC -march=native -flto=auto -O3 -DNDEBUG -fno-omit-frame-pointer -g -Wall -Wextra -Wcast-qual -Wconversion -Wformat=2 -Wformat-nonliteral -Wformat-security -Winit-self -Wmissing-format-attribute -Wmissing-include-dirs -Wpacked -Wunreachable-code -Wunuse
```

## Python (SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 SG_PYTHON=1 + judged flags, COMPILE_BOOST_TESTS=0 RUN_PYTHON_TESTS=0; 136 s)
```
g++ -o pysgpp/_pysgpp_swig.so -march=native -flto=auto -fno-semantic-interposition -fopenmp -shared pysgpp/pys ...
extended_const_func_full_sg_interpolation.py: exit 0
extended_const_func_full_sg_quadrature.py: exit 0
simple_const_func_full_sg_interpolation.py: exit 0
simple_const_func_full_sg_quadrature.py: exit 0
sin_func_full_sg_interpolation.py: exit 0
sin_func_full_sg_quadrature.py: exit 0
sin_func_multiindex_vector_sg_interpolation.py: exit 0
sin_func_multiindex_vector_sg_quadrature.py: exit 0
```

## Debug build (perf.py build debug, OPT=0)
```
(compile db of the debug build was checked during the session: 540 entries, -O0 in 540, flag in 0)
debug build log: flag on 0 lines; perf.py test --repeat 3: 3/3 exit 0
```
