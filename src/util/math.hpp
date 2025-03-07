#pragma once
#include <math.h>

namespace diffurch {

template <typename T> constexpr T sign(T x) { return (x > 0) - (x < 0); }
template <typename T> constexpr T step(T x) { return (x >= 0); }
template <typename T> constexpr T step(T x, T low, T high) {
  return low + (x >= 0) * (high - low);
}
template <typename T> constexpr T relu(T x) { return x * (x >= 0); }

template <typename T> constexpr T clip(T x, T min, T max) {
  return x < min ? min : x > max ? max : x;
}

template <typename T> constexpr auto periodic_continuation(T a, T b, auto F) {
  return [a, b, tau = b - a, tau_inv = 1 / (b - a), F](T x) {
    T xx;
    xx = (x - a) * tau_inv;
    xx = xx - std::floor(xx);
    xx = xx * tau + a;
    return F(xx);
  };
}

} // namespace diffurch
