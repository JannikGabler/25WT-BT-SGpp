# WP-14 — checks besides the A/B benchmarks (raw results)

Scripts (in this folder): `fullcheck.py` (complete-project builds and runs), `sascan.py`
(`-Wstrict-aliasing` scan), `tysan_classify.py` (TypeSanitizer reports), `java/JsgppSmoke.java`.
Logs: `combigrid/perf/.work/logs/wp14/` (git-ignored, several GB of TySan logs).

## Part A: flag order and the variant `gcc-sa`
```
SGppConfigure appends -fno-strict-aliasing AFTER the user's CPPFLAGS:
  g++ -o .../quadrature.os -c -fPIC -march=native -flto=auto -O3 ... -fno-strict-aliasing -funroll-loops ...
=> -fstrict-aliasing in CPPFLAGS would lose. Variant gcc-sa (toolchain.py): CXX=g++-sa
   (scripts/wrappers/g++-sa = `exec g++ "$@" -fstrict-aliasing`), LINKFLAGS += -fstrict-aliasing.
gcc-sa build: 538 compile lines, all via g++-sa; DW_AT_producer of all 59 combigrid C++ CUs:
  "... -funroll-loops -fstrict-aliasing ..." (WP-14-before: "-fno-strict-aliasing -funroll-loops")
gcc-sa warnings: the 7 known ones (base 1, combigrid src 2, tests 4), 0 -Wstrict-aliasing
WP-14-after (= site_scons change, perf.py build normal) vs WP-14-sa: .text/.rodata/.data/.data.rel.ro
  of libsgppcombigrid.so, libsgppbase.so, perf_e2e, perf_micro, perf_reference bitwise identical
  (0 of 59 combigrid CUs carry a strict-aliasing flag; GCC's default at -O2+ is -fstrict-aliasing)
```

## Part A: result changes (perf_reference, bitwise, 1 thread, deterministic)
```
before vs sa: 6 of 104 cases differ, all Clenshaw-Curtis quadrature (sg and dsg):
  quad/sg/cc/b0/d5/L5/sin-unit   0.020528708434641853 -> ...857  (1 ulp), error 2.15e-16 -> 2.12e-16
  quad/sg/cc/b1/d5/L5/sin-unit   1 ulp, error 7.742e-14 -> 7.743e-14
  quad/sg/cc/b0/d2/L10/uq-unit   9 ulp, error 9.99e-16 -> 0
  quad/dsg/cc/b0/d2/L10/uq-unit  4 ulp, error 1.44e-15 -> 9.99e-16
  quad/sg/cc/b0/d5/L6/uq-unit    1 ulp, error 2.0739e-13 -> 2.0750e-13
  quad/dsg/cc/b0/d5/L6/uq-unit   2 ulp, error 1.321e-14 -> 1.343e-14
Source: ClenshawCurtisQuadRule::genWeights (Eigen kissfft): 504 of 2048 node counts n differ by
  1-2 ulp, all with n-1 not a power of 2 (mixed-radix butterflies).
Standalone compile of clenshaw_curtis_quadrature_rule.cpp (n = 2..2049, all weights %a):
  -fno-strict-aliasing vs -fstrict-aliasing, -ffp-contract=fast (GCC default): 13277 weights differ
  -fno-strict-aliasing vs -fstrict-aliasing, -ffp-contract=off:                    0 weights differ
=> the change is FMA contraction placement (TBAA lets GCC restructure the butterflies), not
   type punning. refcheck OK at 1 and 16 threads (refcheck-sa.txt, refcheck.txt).
```

## Part A: why mivec generation and dsg are slower (WP-14-before vs WP-14-sa)
```
mivec/sg/cc-exp/b1/d15/S10/n300, 16 threads: 96 % self time in genMIVecDownwardsClosure (both)
perf stat (2 runs each, warmup 1 + 3 samples):
  before: cycles 2.189e11 / 2.190e11, instructions 4.529e11, branch-misses 6.7e8  (IPC 2.07)
  sa:     cycles 2.689e11 / 2.690e11, instructions 4.704e11, branch-misses 6.8e8  (IPC 1.75)
  => +3.9 % instructions, +23 % cycles: same hot loop structure (unrolled scalar compares,
     same vector code), different address computation/scheduling (`shl`+`lea` vs scaled `lea`).
quad/dsg/equi-exp/b1/d20/L6: profile shape unchanged (appendSubTrieChildren 64 %,
  forEachGPWithGlobalWeight 23 %), both ~5 % slower.
```

## Strict-aliasing scan (all modules, HEAD build log = every compile line incl. wrappers)
```
sascan.py: each of the 1134 compile lines + -fsyntax-only -fstrict-aliasing -Wstrict-aliasing=N
-Wstrict-aliasing=3 (what -Wall enables): 0 warnings in all 1134 TUs (wrappers still with -w)
-Wstrict-aliasing=2: 4 warnings, all datadriven/src, all REAL type punning (int64 bit mask read
  through double*; __AVX__ paths, compiled with -march=native / ARCH=avx2):
  datadriven/.../OperationMultiEvalStreaming/OperationMultiEvalStreaming_multImpl.cpp:175
  datadriven/.../OperationMultiEvalStreaming/OperationMultiEvalStreaming_multTransposeImpl.cpp:140
  datadriven/.../OperationMultipleEvalSubspace/combined/OperationMultipleEvalSubspaceCombined.hpp:119, :244
    int64_t imask = 0x7FFFFFFFFFFFFFFF; double* fmask = reinterpret_cast<double*>(&imask);
    __m256d mask = _mm256_broadcast_sd(fmask);
  (the SSE3 branch, line 53-65 of multImpl.cpp, has the same pattern; not compiled with AVX)
  GCC 15.2 -O3 -march=native, standalone reproduction: the mask is folded into a constant
  (vandpd .LC1) with and without -fstrict-aliasing, identical results => no miscompilation today;
  formally UB. datadriven Boost tests: identical with and without the flag (below).
Wrappers (--no-w, both levels): jsgpp_wrap.cc =3: 8237, =2: 8239 warnings
  (`*(sgpp::base::DataVector **)&jx = ...`, SWIG java.swg jlong<->pointer typemaps)
  pysgpp_wrap.cc =3: 0, =2: 226, all the void** idiom SWIG_ConvertPtr(o, (void**)&ptr, ...) /
  (void**)&fnptr: GCC gives void* an alias set that conflicts with every pointer type, so this is
  well-defined for GCC; + 1 in numpy's ndarraytypes.h:2024 (third-party, level 2)
```

## TypeSanitizer (Clang 21.1.8, -fsanitize=type, OPT=1, all modules, shared libraries; diagnostic)
```
Build: worktree wp14-tysan (HEAD a16733d65; Clang never adds the flag), 35 min, works with the
  shared-library build (libs reference __tysan_*, the test executables carry the runtime).
Memory: Armadillo pulls in OpenBLAS, whose per-thread buffers get 8 bytes of TySan shadow per byte:
  every test binary (even --list_content) needed > 12 GB and crashed the laptop on 2026-10-26
  (global OOM, journald watchdog). Fixed by OPENBLAS_NUM_THREADS=1 (startup 1.1 GB) and a cgroup
  cap per process (systemd-run --user --scope -p MemoryMax=6G -p MemorySwapMax=0).
Per module (reports are not deduplicated by TySan; classes = tysan_classify.py, then manual review):
  misc          complete, 1/1 cases, 0 reports
  quadrature    complete, 2/2, 319 reports, 6 classes
  solver        complete, 5/5, 22 reports, 5 classes
  optimization  killed by the 6 GB cap after 817 s (in TestFileIO*), 3.1 M reports, 105 classes
  base          killed by the 6 GB cap after 1076 s, 3.6 M reports, 24 classes
  combigrid     one process: TySan runtime crash after 105 cases; per suite (74 processes):
                52 complete, 21 aborted by the same runtime crash, 1 timeout (600 s), 4.2 M reports
  datadriven, pde: not run (budget)
Runtime crash: SIGSEGV in __tysan_check -> isAliasingLegal (TySan's own code) at addresses
  -4/-8/-12 (16x), e.g. at a plain double store in genWeightsFFT (heap reused after Eigen's
  complex<double> FFT buffers) and in a Boost EQ assertion (50 == 50); not reproducible when the
  test runs alone => corrupted shadow state, a TySan bug.
Classes (all modules): no class indicates type punning in SG++ library code:
  vptr         base-class ctor/dtor/virtual call on singletons (QuadRule1D/GaussLegendreQuadRule1D,
               NodeGenFunc, OptBarycentricFormula, ClenshawCurtisQuadRule): TySan vtable-pointer FP
  static-init  std::mt19937_64 global in quadrature/src/.../Random.cpp:14 (member stores at init)
  stack        stack slot reused (Boost basic_cstring -> std::string) and OpenMP loop bounds written
               by uninstrumented libomp (#pragma omp for in downwards_closedness.hpp:63/150,
               AlgorithmDGEMV.hpp:143, LassoFunction.hpp:41)
  heap/other   (1) std::mt19937 state array (unsigned long _M_x[624], accessed as long) inside the
                   base::RandomNumberGenerator singleton: TySan's array-in-class limitation
               (2) ClenshawCurtisTable::maxLevel (plain member read/write, no cast) attributed by
                   TySan to the preceding DataVector member: layout artifact
               (3) garbled lines of concurrent OpenMP threads = vptr reads (see above)
```

## Complete project (SG_ALL=1 SG_PYTHON=1 SG_JAVA=1, OPT=1 + judged flags; head = worktree
## a16733d65 + the owner's combigrid/build/jsgpp/combigrid.i edit, after = working tree)
```
Python: the repository's .venv (numpy 2.5.4) first on PATH for both (SConstruct takes the numpy
  include path and whether Python examples/tests run from the python3 on PATH).
build: head exit 0 (650 s), after exit 0 (767 s; base/combigrid objects reused from the judged build)
flag on compile lines (effective = last of -f[no-]strict-aliasing):
  head:  -fno-strict-aliasing on all 1134 (base 423, combigrid 117, datadriven 334, misc 3,
         optimization 123, pde 105, quadrature 8, solver 19, jsgpp 1, pysgpp 1)
  after: only jsgpp_wrap.cc (1 of 597 recompiled lines); DW_AT_producer: libjsgpp.so 1/1 CU with
         the flag; _pysgpp_swig.so 0/1, libsgppdatadriven 0/265, libsgppbase 0/381,
         libsgppcombigrid 0/59
compiler warnings: head 7 (the known base/combigrid ones), after 0 in the recompiled modules
Boost (test cases; assertion counts that are deterministic):
                 head (2 runs)                 after
  base           190/190, 3613802              190/190, 3613802
  combigrid      390/390 (assertions vary)     390/390 (assertions vary)
  datadriven     135/135, 854763               135/135, 854763
  misc           1/1                           1/1
  optimization   45/45, 277725                 45/45, 277725
  pde            48/48, 58929                  48/48, 58929
  quadrature     2/2, 9                        2/2, 9
  solver         5/5, 47                       5/5, 47
  (run times are not comparable: the states ran partly in parallel with each other and TySan)
Java (javac + java against lib/jsgpp of each state):
  tutorial, refinement, JsgppSmoke ("all checks passed"): exit 0, outputs bitwise identical
  optimization (+ExampleFunction, directors): exit 0, identical apart from "Done in N ms"
  example_learnerSGDE: does not compile in both (pre-existing: AdaptivityConfiguration has no
  setNoPoints_)
C++ examples (RUN_CPP_EXAMPLES, scons -k -j1; the PC was shut down on 2026-10-28 00:32 during
  the pass: head had run 55/56 targets, after 28/56; the remaining targets were run as explicit
  SCons targets, steps run-cpp-2):
  56/56 distinct targets executed in both; failed targets identical (2 = pre-existing:
  combigrid thesis/comparison/{sin_example,uq_electron_beam}_template exit 1, "Invalid number of
  arguments! Received 0 but requires 2."); all other 54 exit 0 in both
Python examples (RUN_PYTHON_EXAMPLES): 29/29 targets, 0 failed in both
Python tests (RUN_PYTHON_TESTS): base 7 tests OK, datadriven 21 tests OK, pde 0 tests; identical
`fullcheck.py compare head after`: identical pass/fail for every Boost module, example, Python
  test and Java program (the only differences: combigrid assertion counts, timing lines)
```

## Owner decision 2026-10-28: accepted; datadriven type punning replaced by std::bit_cast
```
Sites (all pointer-cast punning in datadriven/src; 5 files):
  OperationMultiEvalStreaming_{mult,multTranspose}Impl.cpp: abs mask (int64 bits read as double)
    in the SSE3 branch (_mm_set1_pd(*fmask)) and the AVX branch (_mm256_broadcast_sd(fmask))
  OperationMultipleEvalSubspaceCombined.hpp (2x, AVX): same abs mask
  OperationMultiEvalModMaskStreaming_{mult,multTranspose}Impl.cpp, scalar fallback (no SSE3):
    eval | mask via *reinterpret_cast<uint64_t*>, result read back as double
  => const double fmask = std::bit_cast<double>(int64_t{0x7FFFFFFFFFFFFFFF}); (+ #include <bit>)
     std::bit_cast<uint64_t>(eval) | std::bit_cast<uint64_t>(ptrMask[...]); std::bit_cast<double>(...)
  Not changed: the sseUnion/avxUnion reads in OperationMultipleEvalSubspaceCombined.hpp (union
  punning, documented as supported by GCC/Clang). std::bit_cast needs C++20 (default CPP_STD=23).
Build (SG_ALL=1 SG_PYTHON=1 SG_JAVA=1, judged flags, incremental): exit 0, 11 TUs recompiled,
  0 compiler warnings, cpplint OK
-Wstrict-aliasing scan of the 11 TUs (AVX2): level 3: 0, level 2: 0 (before: 4 at level 2)
Branches not compiled with AVX2 (-fsyntax-only, -march=x86-64 -msse3 resp. -march=x86-64):
  SSE3 branch, Streaming mult/multTranspose: HEAD level 2: 1 each -> new: 0
  scalar fallback, ModMask mult/multTranspose: HEAD level 3 (= -Wall): 2 each -> new: 0
    (a build without SSE3 and -Werror would have failed with strict aliasing enabled)
datadriven Boost: 135/135, 854763 assertions (identical; incl. TestStreamingMult(Transpose),
  TestStreamingModMaskMult(Transpose); the SubspaceCombined tests need USE_OCL == 1)
datadriven Python tests: 21 OK
datadriven/ddprobe.cpp (mult + multTranspose of Streaming, ModMask, SubspaceCombined; d = 3, 5;
  517 points; OMP_NUM_THREADS=1): 8568 results bitwise identical before vs after
libsgppdatadriven.so: the affected kernels differ only in register allocation and how the mask
  constant is materialized (movabs immediate on the stack -> load from .rodata); e.g.
  OperationMultiEvalStreaming::multImpl 0x3be -> 0x3bb bytes
```
