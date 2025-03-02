#pragma once

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

} // namespace diffurch
