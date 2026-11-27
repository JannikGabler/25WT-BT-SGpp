# Bitwise comparison of the reference outputs (WP-89-before vs. WP-89-after)

`perf_reference --threads 1|16` of both snapshots: 137 of 146 cases bitwise identical at both thread
counts; 9 differ by 1–2 ulp, all of them Clenshaw–Curtis quadratures (SG and DSG; the DSG code is
untouched by WP-89). The refcheck is OK for all 9 (only the known WARN
`interp/sg/equi/b0/d2/L5/cos-shifted`, identical before and after).

Cause: the CC weights themselves differ (464 of 8205 weights for n = 1 … 4097, at most 128 ulp of
the weight; deterministic within each build). The source of the CC rule is unchanged and its
function `genWeightsFFTInplace` has identical instructions in both libraries, but Eigen's
header-only kiss FFT that it calls is generated differently under LTO
(`kissfft_impl<double>::get_plan` 1644 → 1265 instructions, `fwd` 1212 → 1002): the unrelated
changes of the LTO unit shift inlining and FMA contraction (`-march=native`, GCC's default
`-ffp-contract=fast`) in the twiddle-factor computation. The last bits of the CC weights therefore
depend on unrelated code of the library (a follow-up for the numerics backlog: e.g. exact twiddle
factors or `-ffp-contract=off` for the FFT path — a flag change needs the owner).
