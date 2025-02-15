#pragma once

namespace diffurch {

#ifdef DEBUG
#define DEBUG_ASSERT(x) assert(x)
#else
#define DEBUG_ASSERT(x)
#endif

template <typename F, typename T>
inline T root_by_bisection(const F &f, T l, T h) {
  DEBUG_ASSERT(
      f(l) * f(h) <= 0 &&
      "Endpoints don't have the opposite signes in root_by_bisection_method");

  if (f(l) > 0)
    swap(l, h); // such that f(l) < 0 < f(r)
  T m;
  for (int i = 0; i < 50; i++) {
    m = (l + h) * 0.5;
    if (f(m) < 0) {
      l = m;
    } else {
      h = m;
    }
  }
  return m;
}
} // namespace diffurch
