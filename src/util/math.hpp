#pragma once

namespace diffurch {
template <typename T> T sign(T x) { return (x > 0) - (x < 0); }
template <typename T> T step(T x) { return (x > 0); }
} // namespace diffurch
