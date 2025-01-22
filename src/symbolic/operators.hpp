#pragma once

#include "../events.hpp"
#include "expression.hpp"
#include <math.h>
#include <utility>

namespace State {

#define STATE_OPERATOR_OVERLOAD(op, op_name, base_class)                       \
  template <IsStateExpression L, IsStateExpression R>                          \
  struct op_name : base_class {                                                \
    L l;                                                                       \
    R r;                                                                       \
                                                                               \
    op_name(L l_, R r_) : l(l_), r(r_) {};                                     \
                                                                               \
    auto operator()(const auto &state) const { return l(state) op r(state); }  \
    auto prev(const auto &state) const {                                       \
      return l.prev(state) op r.prev(state);                                   \
    }                                                                          \
    auto operator()(const auto &state, double t) const {                       \
      return l(state, t) op r(state, t);                                       \
    }                                                                          \
    auto operator()(double t) const { return l(t) op r(t); }                   \
    auto get_events() const { return Events(l.get_events(), r.get_events()); } \
  };                                                                           \
  template <IsStateExpression L, IsStateExpression R>                          \
  auto operator op(L l, R r) {                                                 \
    return op_name(l, r);                                                      \
  }                                                                            \
  template <IsNotStateExpression L, IsStateExpression R>                       \
  auto operator op(L &&l, R r) {                                               \
    return op_name(Constant(std::forward<L>(l)), r);                           \
  }                                                                            \
  template <IsStateExpression L, IsNotStateExpression R>                       \
  auto operator op(L l, R &&r) {                                               \
    return op_name(l, Constant(std::forward<R>(r)));                           \
  }

STATE_OPERATOR_OVERLOAD(+, Add, StateExpression);
STATE_OPERATOR_OVERLOAD(-, Sub, StateExpression);
STATE_OPERATOR_OVERLOAD(*, Mul, StateExpression);
STATE_OPERATOR_OVERLOAD(/, Div, StateExpression);

STATE_OPERATOR_OVERLOAD(<, Less, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(>, Greater, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(<=, LessEq, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(>=, GreaterEq, StateBoolExpression);

#define STATE_UNARY_OPERATOR_OVERLOAD(op, op_name)                             \
  template <IsStateExpression Arg> struct op_name : StateExpression {          \
    Arg arg;                                                                   \
    op_name(Arg arg_) : arg(arg_) {}                                           \
    auto operator()(const auto &state) const { return op(arg(state)); }        \
    auto operator()(const auto &state, double t) const {                       \
      return op(arg(state, t));                                                \
    }                                                                          \
    auto operator()(double t) const { return op(arg(t)); }                     \
    auto prev(const auto &state) const { return op(arg.prev(state)); }         \
    auto get_events() const { return arg.get_events(); }                       \
  };                                                                           \
  template <IsStateExpression Arg> auto operator op(Arg arg) {                 \
    return op_name(arg);                                                       \
  }

STATE_UNARY_OPERATOR_OVERLOAD(-, Neg);
STATE_UNARY_OPERATOR_OVERLOAD(+, UnaryPlus);

} // namespace State
