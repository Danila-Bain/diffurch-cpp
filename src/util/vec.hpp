#pragma once

#include <array>
#include <functional>
#include <math.h>

using namespace std;

/*template <size_t N>*/
/*using Vec = typename conditional<N == 1, double, array<double, N>>::type;*/
/*template <size_t N>*/
/*using VecArg = typename conditional<N == 1, Vec<1>, const Vec<N> &>::type;*/

template <size_t N> using Vec = array<double, N>;
template <size_t N> using VecArg = const Vec<N> &;

template <size_t N, size_t M> using VecMap = function<Vec<M>(VecArg<N>)>;

template <size_t N1, size_t N2, size_t M>
using VecMap2 = function<Vec<M>(VecArg<N1>, VecArg<N2>)>;

namespace std {

template <typename T, std::size_t N>
std::array<T, N> operator+(const std::array<T, N> &lhs,
                           const std::array<T, N> &rhs) {
  std::array<T, N> result;
  for (std::size_t i = 0; i < N; ++i) {
    result[i] = lhs[i] + rhs[i];
  }
  return result;
}

template <typename T, std::size_t N>
std::array<T, N> operator-(const std::array<T, N> &lhs,
                           const std::array<T, N> &rhs) {
  std::array<T, N> result;
  for (std::size_t i = 0; i < N; ++i) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

template <typename T, std::size_t N>
std::array<T, N> operator*(const std::array<T, N> &lhs, T rhs) {
  std::array<T, N> result;
  for (std::size_t i = 0; i < N; ++i) {
    result[i] = lhs[i] * rhs;
  }
  return result;
}

/**
 * @brief Product of a scalar and array.
 */
template <typename T, std::size_t N>
std::array<T, N> operator*(T lhs, const std::array<T, N> &rhs) {
  std::array<T, N> result;
  for (std::size_t i = 0; i < N; ++i) {
    result[i] = lhs * rhs[i];
  }
  return result;
}

/**
 * @brief Divide an array by a scalar.
 */
template <typename T, std::size_t N>
std::array<T, N> operator/(const std::array<T, N> &lhs, T rhs) {
  return lhs *
         (1 / rhs); // Is it more efficient to multiply by reciprocal that is
                    // calculated once, than perform a multiple divisions?
}

/**
 * @brief Dot product of two arrays.
 */
template <typename T, std::size_t N>
T operator*(const std::array<T, N> &lhs, const std::array<T, N> &rhs) {
  T result;
  for (std::size_t i = 0; i < N; ++i) {
    result += lhs[i] * rhs[i];
  }
  return result;
}
} // namespace std

template <typename ContainerL, typename ContainerR>
decltype(ContainerL{}[0] * ContainerR{}[0])
dot(const ContainerL &lhs, const ContainerR &rhs, size_t size) {
  decltype(lhs[0] * rhs[0]) result{};
  for (std::size_t i = 0; i < size; ++i) {
    result = result + lhs[i] * rhs[i];
  }
  return result;
}

template <typename T, std::size_t N, std::size_t M>
std::array<T, N + M> concatenate(const std::array<T, N> &arr1,
                                 const std::array<T, M> &arr2) {
  std::array<T, N + M> result;
  std::copy(arr1.begin(), arr1.end(), result.begin());
  std::copy(arr2.begin(), arr2.end(), result.begin() + N);
  return result;
}

inline void VecCopy(double from, double &to, size_t = 0) { to = from; }

template <size_t n, typename T>
inline void VecCopy(T from, array<T, n> &to, size_t i = 0) {
  to[i] = from;
}

template <size_t n, size_t m, typename T>
inline void VecCopy(const array<T, n> &from, array<T, m> &to, size_t i = 0) {
  copy(from.begin(), from.end(), to.begin() + i);
}

template <size_t n, typename T>
inline void VecCopy(const array<T, n> &from, vector<T> &to, size_t i = 0) {
  copy(from.begin(), from.end(), to.begin() + i);
}

inline void VecCopy(double from, vector<double> &to, size_t i = 0) {
  to[i] = from;
}

template <size_t n, typename T> inline double norm(const array<T, n> &v) {
  return sqrt(v * v);
}

inline double norm(double v) { return abs(v); }

std::vector<double> linspace(auto start_in, auto end_in, int num_in) {
  std::vector<double> result(num_in);

  double start = static_cast<double>(start_in);
  double end = static_cast<double>(end_in);
  double num = static_cast<double>(num_in);

  if (num == 0) {
    return result;
  }
  if (num == 1) {
    result[0] = start;
    return result;
  }

  double delta = (end - start);

  for (int i = 0; i < num - 1; ++i) {
    result[i] = start + delta * i / (num - 1);
  }
  result[num - 1] = end; // I want to ensure that start and end
                         // are exactly the same as the input
  return result;
}

std::vector<double> logspace(auto start_in, auto end_in, int num_in) {
  std::vector<double> result(num_in);

  double start = static_cast<double>(start_in);
  double end = static_cast<double>(end_in);
  double num = static_cast<double>(num_in);

  if (num == 0) {
    return result;
  }
  if (num == 1) {
    result[0] = start;
    return result;
  }

  double delta = log(end / start) / (num - 1);

  for (int i = 0; i < num - 1; ++i) {
    result[i] = start * exp(i * delta);
  }
  result[num - 1] = end; // I want to ensure that start and end
                         // are exactly the same as the input
  return result;
}
