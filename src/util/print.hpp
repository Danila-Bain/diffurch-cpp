#pragma once

#include <array>
#include <iostream>
#include <tuple>
#include <vector>
namespace diffurch {
#define FILE_IS_EXECUTED                                                       \
  std::cout << "~~~ " << __FILE__ << " is executed ~~~" << std::endl;
#define FILE_IS_FINISHED                                                       \
  std::cout << "~~~ " << __FILE__ << " is finished ~~~" << std::endl;

// cout  << array<T, N>
template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<T, N> &arr) {
  os << '[';
  for (std::size_t i = 0; i < N; ++i) {
    os << arr[i];
    if (i < N - 1) {
      os << ", ";
    }
  }
  os << ']';
  return os;
}

// cout << vector<T>
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &arr) {
  os << '[';
  for (std::size_t i = 0; i < arr.size(); ++i) {
    os << arr[i];
    if (i < arr.size() - 1) {
      os << ", ";
    }
  }
  os << ']';
  return os;
}

// cout << pair<T, U>
template <typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::pair<T, U> &p) {
  os << "(" << p.first << ", " << p.second << ")";
  return os;
}

// Helper to print tuple elements recursively
template <typename Tuple, std::size_t... Indices>
void print_tuple_impl(const Tuple &tpl, std::ostream &os,
                      std::index_sequence<Indices...>) {
  ((os << (Indices == 0 ? "" : ", ") << std::get<Indices>(tpl)),
   ...); // Format: comma-separated
}

// Overload `<<` for tuples
template <typename... Types>
std::ostream &operator<<(std::ostream &os, const std::tuple<Types...> &tpl) {
  os << "("; // Start of tuple
  print_tuple_impl(tpl, os, std::index_sequence_for<Types...>{});
  os << ")"; // End of tuple
  return os;
}
} // namespace diffurch
