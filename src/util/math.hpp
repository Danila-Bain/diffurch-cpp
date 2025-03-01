#pragma once

namespace diffurch {

template <typename T> constexpr T sign(T x) { return (x > 0) - (x < 0); }
template <typename T> constexpr T step(T x) { return (x >= 0); }
template <typename T> constexpr T step(T x, T low, T high) {
  return low + (x >= 0) * (high - low);
}

} // namespace diffurch
