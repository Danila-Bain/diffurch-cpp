#pragma once

#include "../events.hpp"
#include "expression.hpp"

namespace State {

template <IsNotStateExpression T = double> struct Constant : StateExpression {
  T value;

  Constant(const T &val) : value(val) {}
  operator T() const { return value; }

  T operator()([[maybe_unused]] const auto &state) const { return value; }
  T prev([[maybe_unused]] const auto &state) const { return value; }
  T operator()([[maybe_unused]] const auto &state,
               [[maybe_unused]] double t) const {
    return value;
  }
  T operator()([[maybe_unused]] double t) const { return value; }
  static auto get_events() { return Events(); }
};

struct TimeVariable : StateExpression {
  static auto operator()(const auto &state) { return state.t_curr; }
  static auto prev(const auto &state) { return state.t_prev; }
  static auto operator()([[maybe_unused]] const auto &state, double t) {
    return t;
  }
  static auto operator()(double t) { return t; }
  static auto get_events() { return Events(); }
};

template <size_t coordinate, IsStateExpression Arg, size_t derivative = 0>
struct VariableAt : StateExpression {
  Arg arg;
  VariableAt(Arg arg_) : arg(arg_) {}
  auto operator()(const auto &state) const {
    return state.template eval<derivative, coordinate>(arg(state));
  }
  auto prev(const auto &state) const {
    return state.template eval<derivative, coordinate>(arg.prev(state));
  }
  auto operator()(const auto &state, double t) const {
    return state.template eval<derivative, coordinate>(arg(state, t));
  }
  auto get_events() const { return arg.get_events(); }
};

template <size_t coordinate = -1> struct Variable : StateExpression {
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
    return state.template eval<0, coordinate>(t);
  }

  template <IsStateExpression Arg> static auto operator()(Arg arg) {
    return VariableAt<coordinate, Arg, 0>(arg);
  }

  static auto get_events() { return Events(); }
};

template <size_t derivative = 1, size_t coordinate = -1>
constexpr auto D(const Variable<coordinate> &var) {
  return VariableAt<coordinate, TimeVariable, derivative>(TimeVariable());
}

template <size_t N, size_t from = 0, size_t to = N> constexpr auto Variables() {
  if constexpr (from == to) {
    return std::tuple<>();
  } else {
    return std::tuple_cat(std::make_tuple(Variable<from>()),
                          Variables<N, from + 1, to>());
  }
}

} // namespace State
