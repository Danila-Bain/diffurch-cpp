#pragma once

#include "expression.hpp"
#include <cstddef> // for size_t
#include <tuple>

namespace diffurch {

template <IsNotStateExpression T = double> struct Constant : StateExpression {
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
  static auto get_events() { return std::make_tuple(); }
};

template <size_t derivative = 1, IsNotStateExpression T = double>
constexpr auto D(const Constant<T> &c) {
  if constexpr (derivative == 0) {
    return c;
  } else {
    return Constant(0.);
  }
}

struct TimeVariable : StateExpression {
  static auto operator()(const auto &state) { return state.t_curr; }
  static auto prev(const auto &state) { return state.t_prev; }
  static auto operator()([[maybe_unused]] const auto &state, double t) {
    return t;
  }
  static auto operator()(double t) { return t; }
  static auto get_events() { return std::make_tuple(); }
};

template <size_t derivative = 1> constexpr auto D(const TimeVariable &t) {
  if constexpr (derivative == 0) {
    return t;
  } else if constexpr (derivative == 1) {
    return Constant(1.);
  } else {
    return Constant(0.);
  }
}

template <size_t coordinate, IsStateExpression Arg, size_t derivative = 0>
struct VariableAt : StateExpression {
  Arg arg;
  VariableAt(Arg arg_) : arg(arg_) {}
  auto operator()(const auto &state) const {
    return state.template eval<derivative>(arg(state))[coordinate];
  }
  auto prev(const auto &state) const {
    return state.template eval<derivative>(arg.prev(state))[coordinate];
  }
  auto operator()(const auto &state, double t) const {
    return state.template eval<derivative>(arg(state, t))[coordinate];
  }
  auto get_events() { return arg.get_events(); }
};

template <size_t derivative = 1, size_t var_coordinate = -1,
          IsStateExpression VarArg = TimeVariable, size_t var_derivative = 0>
constexpr auto
D(const VariableAt<var_coordinate, VarArg, var_derivative> &var_at) {
  if constexpr (derivative == 0) {
    return var_at;
  } else {
    return D<derivative - 1>(
        Variable<var_coordinate, var_derivative + 1>(var_at.arg) *
        D(var_at.arg));
  }
}

template <size_t coordinate = -1, size_t derivative = 0>
struct Variable : StateExpression {
  static auto operator()(const IsNotStateExpression auto &state, double t) {
    return state.template eval<derivative>(t)[coordinate];
  }

  template <IsStateExpression Arg> static auto operator()(Arg arg) {
    return VariableAt<coordinate, Arg, derivative>(arg);
  }

  static auto get_events() { return std::make_tuple(); }
};

template <size_t coordinate> struct Variable<coordinate, 0> : StateExpression {
  static constexpr auto operator()(const IsNotStateExpression auto &state) {
    if constexpr (coordinate == -1)
      return state.x_curr;
    else
      return state.x_curr[coordinate];
  }
  static auto prev(const IsNotStateExpression auto &state) {
    if constexpr (coordinate == -1)
      return state.x_prev;
    else
      return state.x_prev[coordinate];
  }
  static auto operator()(const IsNotStateExpression auto &state, double t) {
    return state.template eval<0>(t)[coordinate];
  }

  template <IsStateExpression Arg> static auto operator()(Arg arg) {
    return VariableAt<coordinate, Arg, 0>(arg);
  }

  static auto get_events() { return std::make_tuple(); }
};

template <size_t derivative = 1, size_t var_coordinate = -1,
          size_t var_derivative = 0>
constexpr auto D(const Variable<var_coordinate, var_derivative> &var) {
  return Variable<var_coordinate, var_derivative + derivative>();
}

template <size_t N, size_t from = 0, size_t to = N> constexpr auto Variables() {
  if constexpr (from == to) {
    return std::tuple<>();
  } else {
    return std::tuple_cat(std::make_tuple(Variable<from>()),
                          Variables<N, from + 1, to>());
  }
}

} // namespace diffurch
