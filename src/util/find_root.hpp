#pragma once

#include <algorithm>
#include <utility>

namespace diffurch {

#ifdef DEBUG
#define DEBUG_ASSERT(x) assert(x)
#else
#define DEBUG_ASSERT(x)
#endif

template <typename F, typename T>
inline T root_by_bisection(const F &f, T l, T r) {
  DEBUG_ASSERT(
      f(l) * f(h) <= 0 &&
      "Endpoints don't have the opposite signes in root_by_bisection_method");

  if (f(l) > 0)
    std::swap(l, r); // such that f(l) < 0 < f(r)

  T m;
  for (int i = 0; i < 50; i++) {
    m = (l + r) * 0.5;
    if (f(m) < 0) {
      l = m;
    } else {
      r = m;
    }
  }
  // return m;
  return std::max(r, l);
}

template <typename F, typename T>
inline T bool_change_by_bisection(const F &f, T l, T r) {

  if (f(l))
    std::swap(l, r); // such that f(l) == false, f(r) == true

  T m;
  for (int i = 0; i < 50; i++) {
    m = (l + r) * 0.5;
    if (f(m)) {
      r = m;
    } else {
      l = m;
    }
  }
  // return m;
  return std::max(r, l);
}

} // namespace diffurch
