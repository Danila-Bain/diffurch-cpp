#pragma once

#include "symbol_types.hpp"
#include <cstddef> // for size_t
#include <tuple>

namespace diffurch {

template <IsNotSymbol T = double> struct Constant : Symbol {
  T value;

  Constant(T val) : value(val) {}

  operator T() const { return value; }

  T operator()([[maybe_unused]] const auto &state) const { return value; }
  T prev([[maybe_unused]] const auto &state) const { return value; }
  T operator()([[maybe_unused]] const auto &state,
               [[maybe_unused]] double t) const {
    return value;
  }
  T operator()([[maybe_unused]] double t) const { return value; }
  template <size_t coordinate = -1> static auto get_events() {
    return std::make_tuple();
  }
};

template <size_t derivative_order = 1, IsNotSymbol T = double>
constexpr Constant<T> D(const Constant<T> &c) {
  if constexpr (derivative_order == 0) {
    return c;
  } else {
    return Constant<T>(0);
  }
}

struct TimeVariable : Symbol {
  static auto operator()(const auto &state) { return state.t_curr; }
  static auto prev(const auto &state) { return state.t_prev; }
  static auto operator()([[maybe_unused]] const auto &state, double t) {
    return t;
  }
  static auto operator()(double t) { return t; }
  template <size_t coordinate = size_t(-1)> static auto get_events() {
    return std::make_tuple();
  }
};

template <size_t derivative_order = 1> constexpr auto D(const TimeVariable &t) {
  if constexpr (derivative_order == 0) {
    return t;
  } else if constexpr (derivative_order == 1) {
    return Constant(1.);
  } else {
    return Constant(0.);
  }
}

template <size_t coordinate, IsSymbol Arg, size_t derivative_order = 0>
struct VariableAt : Symbol {
  Arg arg;
  VariableAt(Arg arg_) : arg(arg_) {}
  auto operator()(const auto &state) const {
    return state.template eval<derivative_order>(arg(state))[coordinate];
  }
  auto prev(const auto &state) const {
    return state.template eval<derivative_order>(arg.prev(state))[coordinate];
  }
  auto operator()(const auto &state, double t) const {
    return state.template eval<derivative_order>(arg(state, t))[coordinate];
  }
  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return arg.template get_events<current_coordinate>();
  }
};

template <size_t derivative_order = 1, size_t var_coordinate = -1,
          IsSymbol VarArg = TimeVariable, size_t var_derivative = 0>
constexpr auto
D(const VariableAt<var_coordinate, VarArg, var_derivative> &var_at) {
  if constexpr (derivative_order == 0) {
    return var_at;
  } else {
    return D<derivative_order - 1>(
        Variable<var_coordinate, var_derivative + 1>(var_at.arg) *
        D(var_at.arg));
  }
}

template <size_t coordinate = -1, size_t derivative_order = 0>
struct Variable : Symbol {
  static auto operator()(const IsNotSymbol auto &state, double t) {
    return state.template eval<derivative_order>(t)[coordinate];
  }

  static auto operator()(IsSymbol auto arg) {
    return VariableAt<coordinate, decltype(arg), derivative_order>(arg);
  }

  template <size_t current_coordinate = size_t(-1)> static auto get_events() {
    return std::make_tuple();
  }
};

template <size_t coordinate> struct Variable<coordinate, 0> : Symbol {
  static constexpr auto operator()(const IsNotSymbol auto &state) {
    if constexpr (coordinate == -1)
      return state.x_curr;
    else
      return state.x_curr[coordinate];
  }
  static auto prev(const IsNotSymbol auto &state) {
    if constexpr (coordinate == -1)
      return state.x_prev;
    else
      return state.x_prev[coordinate];
  }
  static auto operator()(const IsNotSymbol auto &state, double t) {
    if constexpr (coordinate == -1)
      return state.template eval<0>(t);
    else
      return state.template eval<0>(t)[coordinate];
  }

  static auto operator()(IsSymbol auto arg) {
    return VariableAt<coordinate, decltype(arg), 0>(arg);
  }

  template <size_t current_coordinate = size_t(-1)> static auto get_events() {
    return std::make_tuple();
  }
};

template <size_t derivative_order = 1, size_t var_coordinate = -1,
          size_t var_derivative = 0>
constexpr auto D(const Variable<var_coordinate, var_derivative> &var) {
  return Variable<var_coordinate, var_derivative + derivative_order>();
}

template <size_t N, size_t from = 0, size_t to = N, size_t derivative_order = 0>
constexpr auto Variables() {
  if constexpr (from == to) {
    return std::tuple<>();
  } else {
    return std::tuple_cat(std::make_tuple(Variable<from, derivative_order>()),
                          Variables<N, from + 1, to>());
  }
}

} // namespace diffurch
