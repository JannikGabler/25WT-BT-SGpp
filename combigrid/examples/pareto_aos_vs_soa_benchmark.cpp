// #include <immintrin.h>  // AVX2
// #include <algorithm>
// #include <chrono>
// #include <iostream>
// #include <random>
// #include <vector>

// using namespace std;
// using namespace std::chrono;

// // ----------- AoS -----------
// struct VectorAoS {
//   vector<double> values;
// };

// bool dominatesAoS(const VectorAoS& a, const VectorAoS& b) {
//   bool strictly_better = false;
//   for (size_t i = 0; i < a.values.size(); ++i) {
//     if (a.values[i] < b.values[i]) return false;
//     if (a.values[i] > b.values[i]) strictly_better = true;
//   }
//   return strictly_better;
// }

// vector<VectorAoS> paretoAoS(const vector<VectorAoS>& vectors) {
//   vector<VectorAoS> result;
//   for (const auto& v : vectors) {
//     bool dominated = false;
//     for (const auto& u : vectors) {
//       if (&v != &u && dominatesAoS(u, v)) {
//         dominated = true;
//         break;
//       }
//     }
//     if (!dominated) result.push_back(v);
//   }
//   return result;
// }

// // ----------- SoA -----------
// struct VectorSoA {
//   vector<vector<double>> values;  // values[dim][index]
// };

// bool dominatesSoA(const VectorSoA& vecs, size_t i, size_t j) {
//   bool strictly_better = false;
//   size_t dim = vecs.values.size();
//   for (size_t d = 0; d < dim; ++d) {
//     if (vecs.values[d][i] < vecs.values[d][j]) return false;
//     if (vecs.values[d][i] > vecs.values[d][j]) strictly_better = true;
//   }
//   return strictly_better;
// }

// vector<size_t> paretoSoA(const VectorSoA& vecs) {
//   size_t n = vecs.values[0].size();
//   vector<size_t> result;
//   for (size_t i = 0; i < n; ++i) {
//     bool dominated = false;
//     for (size_t j = 0; j < n; ++j) {
//       if (i != j && dominatesSoA(vecs, j, i)) {
//         dominated = true;
//         break;
//       }
//     }
//     if (!dominated) result.push_back(i);
//   }
//   return result;
// }

// // ----------- SoA + SIMD -----------
// bool dominatesSIMD(const VectorSoA& vecs, size_t i, size_t j) {
//   bool strictly_better = false;
//   size_t dim = vecs.values.size();

//   size_t d = 0;
//   for (; d + 3 < dim; d += 4) {  // 4 doubles pro AVX-Register
//     __m256d a = _mm256_loadu_pd(&vecs.values[d][i]);
//     __m256d b = _mm256_loadu_pd(&vecs.values[d][j]);

//     __m256d cmp_lt = _mm256_cmp_pd(a, b, _CMP_LT_OQ);
//     if (!_mm256_testz_pd(cmp_lt, cmp_lt)) return false;  // a < b in irgendeiner Dimension

//     __m256d cmp_gt = _mm256_cmp_pd(a, b, _CMP_GT_OQ);
//     if (!_mm256_testz_pd(cmp_gt, cmp_gt)) strictly_better = true;
//   }

//   // Reste, falls dim % 4 != 0
//   for (; d < dim; ++d) {
//     if (vecs.values[d][i] < vecs.values[d][j]) return false;
//     if (vecs.values[d][i] > vecs.values[d][j]) strictly_better = true;
//   }

//   return strictly_better;
// }

// vector<size_t> paretoSIMD(const VectorSoA& vecs) {
//   size_t n = vecs.values[0].size();
//   vector<size_t> result;
//   for (size_t i = 0; i < n; ++i) {
//     bool dominated = false;
//     for (size_t j = 0; j < n; ++j) {
//       if (i != j && dominatesSIMD(vecs, j, i)) {
//         dominated = true;
//         break;
//       }
//     }
//     if (!dominated) result.push_back(i);
//   }
//   return result;
// }

// // ----------- Main Benchmark -----------
// int main() {
//   size_t n_vectors = 200000;  // für O(n^2) nicht zu groß, sonst dauert es ewig
//   size_t dim = 4;

//   // Random generator
//   random_device rd;
//   mt19937 gen(rd());
//   uniform_real_distribution<> dis(0.0, 100.0);

//   // --- AoS ---
//   vector<VectorAoS> vectorsAoS(n_vectors);
//   for (auto& v : vectorsAoS) {
//     v.values.resize(dim);
//     for (auto& x : v.values) x = dis(gen);
//   }

//   auto start = high_resolution_clock::now();
//   auto resultAoS = paretoAoS(vectorsAoS);
//   auto end = high_resolution_clock::now();
//   cout << "AoS: " << duration_cast<milliseconds>(end - start).count() << " ms, " <<
//   resultAoS.size()
//        << " Pareto-Vektoren" << endl;

//   // --- SoA ---
//   VectorSoA vectorsSoA;
//   vectorsSoA.values.resize(dim, vector<double>(n_vectors));
//   for (size_t d = 0; d < dim; ++d) {
//     for (size_t i = 0; i < n_vectors; ++i) {
//       vectorsSoA.values[d][i] = dis(gen);
//     }
//   }

//   start = high_resolution_clock::now();
//   auto resultSoA = paretoSoA(vectorsSoA);
//   end = high_resolution_clock::now();
//   cout << "SoA: " << duration_cast<milliseconds>(end - start).count() << " ms, " <<
//   resultSoA.size()
//        << " Pareto-Vektoren" << endl;

//   vectorsSoA.values.clear();
//   vectorsSoA.values.resize(dim, vector<double>(n_vectors));
//   for (size_t d = 0; d < dim; ++d)
//     for (size_t i = 0; i < n_vectors; ++i) vectorsSoA.values[d][i] = dis(gen);

//   start = high_resolution_clock::now();
//   auto resultSIMD = paretoSIMD(vectorsSoA);
//   end = high_resolution_clock::now();

//   cout << "SoA + SIMD: " << duration_cast<milliseconds>(end - start).count() << " ms, "
//        << resultSIMD.size() << " Pareto-Vektoren" << endl;

//   return 0;
// }

int main() { return 0; }