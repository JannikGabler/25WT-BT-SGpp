// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-20: does a struct-pointer store force reloads of a container's array pointers?
// g++ -std=c++23 -O3 -fPIC -shared [-fno-strict-aliasing|-flto] alias_probe.cpp; count the loads
// of t->off/nodes/coeffs in f(): -fno-strict-aliasing 8, strict aliasing 2, + -flto 2.
#include <cstddef>
struct TrieNode { const double* w; std::size_t end; double c; };
struct Node { unsigned lvl; std::size_t childEnd; };
struct Trie { std::size_t* off; Node* nodes; double* coeffs; };
__attribute__((noinline)) double f(const Trie* t, TrieNode** finish, std::size_t i) {
  double x = t->nodes[i].childEnd + t->off[i] + t->coeffs[i];
  *finish = *finish + 1;          // like vector::push_back: store of TrieNode*
  double y = t->nodes[i + 1].childEnd + t->off[i + 1] + t->coeffs[i + 1];  // reload t->nodes/off/coeffs?
  (*finish)[-1].end = 5;          // size_t store via TrieNode access path
  double z = t->nodes[i + 2].childEnd + t->off[i + 2];  // reload of values?
  return x + y + z;
}
double g(const Trie* t, TrieNode** f2, std::size_t i) { return f(t, f2, i); }
