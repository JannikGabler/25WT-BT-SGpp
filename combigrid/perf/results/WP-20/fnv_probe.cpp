// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Build: g++ -std=c++23 -O3 -DNDEBUG -march=native -mavx2 -mfma -flto=auto -funroll-loops
//        -I combigrid/src [-fno-strict-aliasing] fnv_probe.cpp; run pinned (taskset -c 4).
// Variant Index = uint32_t: FlatNestedVec<double, 3> -> FlatNestedVec<double, 3, std::uint32_t>.
// WP-20 probe: chained FlatNestedVec access vs. a local view/span vs. nested std::vector,
// writes (like the 1D weights) and reads (like the trie), with/without -fno-strict-aliasing.
#include <chrono>
#include <cstdio>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <vector>
using sgpp::combigrid::misc::FlatNestedVec;
using sgpp::combigrid::misc::NestedStdVec;

constexpr std::size_t N1 = 2000, N2 = 10, N3 = 33;

template <class F> double best(F&& f) {
  double b = 1e30;
  for (int r = 0; r < 15; r++) {
    auto t0 = std::chrono::steady_clock::now();
    f();
    double t = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    if (t < b) b = t;
  }
  return b * 1e9 / (N1 * N2 * N3);
}

__attribute__((noinline)) void writeChained(FlatNestedVec<double, 3>& fv) {
  for (std::size_t i = 0; i < N1; i++)
    for (std::size_t j = 0; j < N2; j++)
      for (std::size_t k = 0; k < N3; k++) fv[i][j][k] = 0.5 * fv[i][j][k] + double(k);
}
__attribute__((noinline)) void writeLocal(FlatNestedVec<double, 3>& fv) {
  for (std::size_t i = 0; i < N1; i++) {
    const auto v = fv[i];
    for (std::size_t j = 0; j < N2; j++) {
      const std::span<double> s = v[j];
      for (std::size_t k = 0; k < N3; k++) s[k] = 0.5 * s[k] + double(k);
    }
  }
}
__attribute__((noinline)) void writeNested(NestedStdVec<double, 3>& nv) {
  for (std::size_t i = 0; i < N1; i++)
    for (std::size_t j = 0; j < N2; j++)
      for (std::size_t k = 0; k < N3; k++) nv[i][j][k] = 0.5 * nv[i][j][k] + double(k);
}
// Reads interleaved with stores into another vector of pointers (like push_back of TrieNodes).
struct Rec { const double* w; std::size_t end; double c; };
__attribute__((noinline)) double readChainedPush(const FlatNestedVec<double, 3>& fv, std::vector<Rec>& out) {
  out.clear();
  double s = 0;
  for (std::size_t i = 0; i < N1; i++)
    for (std::size_t j = 0; j < N2; j++)
      for (std::size_t k = 0; k < N3; k += 3) {
        out.push_back({fv[i][j].data() + k, k, fv[i][j][k]});
        s += out.back().c;
      }
  return s;
}
__attribute__((noinline)) double readLocalPush(const FlatNestedVec<double, 3>& fv, std::vector<Rec>& out) {
  out.clear();
  double s = 0;
  for (std::size_t i = 0; i < N1; i++) {
    const auto v = fv[i];
    for (std::size_t j = 0; j < N2; j++) {
      const std::span<const double> sp = v[j];
      for (std::size_t k = 0; k < N3; k += 3) {
        out.push_back({sp.data() + k, k, sp[k]});
        s += out.back().c;
      }
    }
  }
  return s;
}

int main() {
  NestedStdVec<double, 3> nv(N1, std::vector<std::vector<double>>(N2, std::vector<double>(N3, 1.0)));
  FlatNestedVec<double, 3> fv(nv);
  std::vector<Rec> out;
  out.reserve(N1 * N2 * N3);
  double sink = 0;
  const double wc = best([&] { writeChained(fv); });
  const double wl = best([&] { writeLocal(fv); });
  const double wn = best([&] { writeNested(nv); });
  const double rc = best([&] { sink += readChainedPush(fv, out); });
  const double rl = best([&] { sink += readLocalPush(fv, out); });
  std::printf("write chained %.3f  local %.3f  nested %.3f | read+push chained %.3f  local %.3f ns/elem (sink %g)\n",
              wc, wl, wn, rc, rl, sink);
}
