
`quad/dsg/cc-uq/b0/d10/L7` (16 t, 1 warm-up + 1 sample), median of 3 processes:
| event | rp-4 | WP-05-before | WP-05-after | rp-5 |
|---|---|---|---|---|
| instructions | 9.844e+11 | 9.853e+11 | 9.854e+11 | 9.853e+11 |
| cycles | 4.361e+11 | 4.386e+11 | 4.504e+11 | 4.385e+11 |
| op_cache_hit_miss.all_op_cache_accesses | 1.315e+11 | 1.339e+11 | 1.337e+11 | 1.338e+11 |
| op_cache_hit_miss.op_cache_miss | 2.989e+08 | 3.171e+08 | 3.775e+09 | 3.116e+08 |
| miss / access | 0.0023 | 0.0024 | 0.0282 | 0.0023 |

`quad/dsg/equi-exp/b0/d10/L7` (16 t, 1 warm-up + 1 sample), median of 3 processes:
| event | rp-4 | WP-05-before | WP-05-after | rp-5 |
|---|---|---|---|---|
| instructions | 7.788e+11 | 7.797e+11 | 7.798e+11 | 7.798e+11 |
| cycles | 3.43e+11 | 3.473e+11 | 3.475e+11 | 3.473e+11 |
| op_cache_hit_miss.all_op_cache_accesses | 1.208e+11 | 1.231e+11 | 1.231e+11 | 1.231e+11 |
| op_cache_hit_miss.op_cache_miss | 2.435e+08 | 2.589e+08 | 2.529e+08 | 2.515e+08 |
| miss / access | 0.0020 | 0.0021 | 0.0021 | 0.0020 |

op-cache misses by symbol, rp-4 (cc d10, sampled every 20000 misses):
    12.68%  [unknown]                [k] 0xffffffffb48076a3
     6.90%  [unknown]                [k] 0xffffffffb44011b9
     3.88%  [unknown]                [k] 0xffffffffb4807612
     3.73%  libsgppcombigrid.so      [.] void sgpp::combigrid::tools::dsg_weighted_sum::forEachGPWithGlobalWeight<sgpp::combigrid::tools::dsg_weighted_sum::(anonymous namespace)::sumWithGlobalWeights<sgpp::combigrid::misc::ScalarValues>(sgpp::combigrid::misc::GenericDSGOperatorContext const&, sgpp::combigrid::misc::ScalarValues, sgpp::combigrid::TensorGridCTData const&, sgpp::combigrid::misc::DeltaWeightedSumScratch&)::{lambda(sgpp::base::DataVector const&, double)#1}>(sgpp::combigrid::misc::GenericDSGOperatorContext const&, sgpp::combigrid::TensorGridCTData const&, sgpp::combigrid::misc::DeltaWeightedSumScratch&, sgpp::combigrid::tools::dsg_weighted_sum::(anonymous namespace)::sumWithGlobalWeights<sgpp::combigrid::misc::ScalarValues>(sgpp::combigrid::misc::GenericDSGOperatorContext const&, sgpp::combigrid::misc::ScalarValues, sgpp::combigrid::TensorGridCTData const&, sgpp::combigrid::misc::DeltaWeightedSumScratch&)::{lambda(sgpp::base::DataVector const&, double)#1}&&) [clone .lto_priv.0]
     2.75%  libsgppcombigrid.so      [.] sgpp::combigrid::tools::ProjectedSubTrieBuilder::addChildren(unsigned long, unsigned long, unsigned long, unsigned int, sgpp::combigrid::tools::ProjectedSubTrieBuilder::Path const&)
     2.18%  [unknown]                [k] 0xffffffffb480b324
     1.86%  [unknown]                [k] 0xffffffffb480765a
     1.62%  perf_e2e                 [.] sgpp::combigrid::perf::uqIntegrand(sgpp::base::DataVector const&)
     1.42%  libsgppcombigrid.so      [.] sgpp::combigrid::tools::ProjectedSubTrieBuilder::appendProjectedChildren(unsigned long, unsigned long, unsigned int)
     1.41%  libm.so.6                [.] __ieee754_exp_fma
     1.24%  [unknown]                [k] 0xffffffffb49938cf
     1.20%  [unknown]                [k] 0xffffffffb4807696
     1.05%  [unknown]                [k] 0xffffffffb48bd664
     1.04%  [unknown]                [k] 0xffffffffb480769c

op-cache misses by symbol, rp-5 (cc d10, sampled every 20000 misses):
    12.66%  [unknown]            [k] 0xffffffffb48076a3
     7.63%  libsgppcombigrid.so  [.] void sgpp::combigrid::tools::dsg_weighted_sum::forEachGPWithGlobalWeight<sgpp::combigrid::tools::dsg_weighted_sum::(anonymous namespace)::sumWithGlobalWeights<sgpp::combigrid::misc::ScalarValues>(sgpp::combigrid::misc::GenericDSGOperatorContext const&, sgpp::combigrid::misc::ScalarValues, sgpp::combigrid::TensorGridCTDataView const&, sgpp::combigrid::misc::DeltaWeightedSumScratch&)::{lambda(sgpp::base::DataVector const&, double)#1}>(sgpp::combigrid::misc::GenericDSGOperatorContext const&, sgpp::combigrid::TensorGridCTDataView const&, sgpp::combigrid::misc::DeltaWeightedSumScratch&, sgpp::combigrid::tools::dsg_weighted_sum::(anonymous namespace)::sumWithGlobalWeights<sgpp::combigrid::misc::ScalarValues>(sgpp::combigrid::misc::GenericDSGOperatorContext const&, sgpp::combigrid::misc::ScalarValues, sgpp::combigrid::TensorGridCTDataView const&, sgpp::combigrid::misc::DeltaWeightedSumScratch&)::{lambda(sgpp::base::DataVector const&, double)#1}&&) [clone .lto_priv.0]
     6.63%  [unknown]            [k] 0xffffffffb44011b9
     3.70%  [unknown]            [k] 0xffffffffb4807612
     3.14%  perf_e2e             [.] sgpp::combigrid::perf::uqIntegrand(sgpp::base::DataVector const&)
     2.63%  libm.so.6            [.] __ieee754_exp_fma
     2.11%  libsgppcombigrid.so  [.] sgpp::combigrid::tools::ProjectedSubTrieBuilder::addChildren(unsigned long, unsigned long, unsigned long, unsigned int, sgpp::combigrid::tools::ProjectedSubTrieBuilder::Path const&)
     1.96%  [unknown]            [k] 0xffffffffb480b324
     1.57%  [unknown]            [k] 0xffffffffb480765a
     1.24%  [unknown]            [k] 0xffffffffb49938cf
     1.23%  libsgppcombigrid.so  [.] sgpp::combigrid::tools::ProjectedSubTrieBuilder::appendProjectedChildren(unsigned long, unsigned long, unsigned int)
     1.14%  [unknown]            [k] 0xffffffffb48bd664
     1.09%  [unknown]            [k] 0xffffffffb4807696
