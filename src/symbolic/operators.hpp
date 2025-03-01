#pragma once

#include "../events.hpp"
#include "expression.hpp"
#include <math.h>
#include <utility>

namespace diffurch {

#define STATE_OPERATOR_OVERLOAD(op, op_name, argument_class, base_class)       \
  template <Is##argument_class L, Is##argument_class R>                        \
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
    template <size_t current_coordinate = size_t(-1)> auto get_events() {      \
      return std::tuple_cat(l.template get_events<current_coordinate>(),       \
                            r.template get_events<current_coordinate>());      \
    }                                                                          \
  };                                                                           \
  template <Is##argument_class L, Is##argument_class R>                        \
  auto operator op(L l, R r) {                                                 \
    return op_name(l, r);                                                      \
  }                                                                            \
  template <IsNot##argument_class L, Is##argument_class R>                     \
  auto operator op(L &&l, R r) {                                               \
    return op_name(Constant(std::forward<L>(l)), r);                           \
  }                                                                            \
  template <Is##argument_class L, IsNot##argument_class R>                     \
  auto operator op(L l, R &&r) {                                               \
    return op_name(l, Constant(std::forward<R>(r)));                           \
  }

#define STATE_UNARY_OPERATOR_OVERLOAD(op, op_name, argument_class, base_class) \
  template <Is##argument_class Arg> struct op_name : base_class {              \
    Arg arg;                                                                   \
    op_name(Arg arg_) : arg(arg_) {}                                           \
    auto operator()(const auto &state) const { return op(arg(state)); }        \
    auto operator()(const auto &state, double t) const {                       \
      return op(arg(state, t));                                                \
    }                                                                          \
    auto operator()(double t) const { return op(arg(t)); }                     \
    auto prev(const auto &state) const { return op(arg.prev(state)); }         \
    template <size_t current_coordinate = size_t(-1)> auto get_events() {      \
      return arg.template get_events<current_coordinate>();                    \
    }                                                                          \
  };                                                                           \
  template <Is##argument_class Arg> auto operator op(Arg arg) {                \
    return op_name(arg);                                                       \
  }

STATE_OPERATOR_OVERLOAD(+, Add, StateExpression, StateExpression);
template <size_t derivative = 1, IsStateExpression L, IsStateExpression R>
constexpr auto D(const Add<L, R> &add) {
  return D<derivative>(add.l) + D<derivative>(add.r);
}
STATE_OPERATOR_OVERLOAD(-, Sub, StateExpression, StateExpression);
template <size_t derivative = 1, IsStateExpression L, IsStateExpression R>
constexpr auto D(const Sub<L, R> &sub) {
  return D<derivative>(sub.l) - D<derivative>(sub.r);
}
STATE_OPERATOR_OVERLOAD(*, Mul, StateExpression, StateExpression);
template <size_t derivative = 1, IsStateExpression L, IsStateExpression R>
constexpr auto D(const Mul<L, R> &mul) {
  if constexpr (derivative == 0)
    return mul;
  else
    return D<derivative - 1>(D(mul.l) * mul.r + mul.l * D(mul.r));
}

STATE_OPERATOR_OVERLOAD(/, Div, StateExpression, StateExpression);
template <size_t derivative = 1, IsStateExpression L, IsStateExpression R>
constexpr auto D(const Div<L, R> &div) {
  if constexpr (derivative == 0)
    return div;
  else
    return D<derivative - 1>((D(div.l) * div.r - div.l * D(div.r)) /
                             (div.r * div.r));
}

STATE_UNARY_OPERATOR_OVERLOAD(-, Neg, StateExpression, StateExpression);
template <size_t derivative = 1, IsStateExpression Arg>
constexpr auto D(const Neg<Arg> &neg) {
  return -D<derivative>(neg.arg);
}

// UNARY PLUS is NoOp
template <IsStateExpression Arg> auto operator+(Arg arg) { return arg; }

STATE_OPERATOR_OVERLOAD(==, Equal, StateExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(!=, NotEqual, StateExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(<, Less, StateExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(>, Greater, StateExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(<=, LessEqual, StateExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(>=, GreaterEqual, StateExpression, StateBoolExpression);

STATE_OPERATOR_OVERLOAD(&&, And, StateBoolExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(||, Or, StateBoolExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(!=, Xor, StateBoolExpression, StateBoolExpression);
STATE_OPERATOR_OVERLOAD(==, BoolEqual, StateBoolExpression,
                        StateBoolExpression);
STATE_UNARY_OPERATOR_OVERLOAD(!, BoolNeg, StateBoolExpression,
                              StateBoolExpression);

} // namespace diffurch
