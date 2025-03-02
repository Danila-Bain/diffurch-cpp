#pragma once

#include "../util/math.hpp"
#include "expression.hpp"
#include <cstddef>
#include <tuple>

namespace diffurch {

template <IsStateExpression Arg> struct dsign : StateExpression {
  Arg arg;
  dsign(Arg arg_) : arg(arg_) {}
  double curr_value = 0;
  auto operator()(const auto &state) const { return curr_value; }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(arg.template get_events<current_coordinate>(),
                          std::make_tuple(StartEvent(nullptr,
                                                     [this](const auto &state) {
                                                       curr_value =
                                                           sign(arg(state));
                                                     }),
                                          Event(When(arg == 0), nullptr,
                                                [this](const auto &state) {
                                                  curr_value = -curr_value;
                                                })));
  }
};

template <IsStateExpression Arg> struct dstep : StateExpression {
  Arg arg;
  const double low_value;
  const double high_value;

  dstep(Arg arg_, double low = 0, double high = 1)
      : arg(arg_), low_value(low), high_value(high) {}

  double curr_value;

  auto operator()(const auto &state) const { return curr_value; }
  auto prev(const auto &state) const { return curr_value; }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        arg.template get_events<current_coordinate>(),
        std::make_tuple(
            StartEvent(nullptr,
                       [this](const auto &state) {
                         curr_value = step(arg(state), low_value, high_value);
                       }),
            Event(When(arg == 0), nullptr, [this](const auto &state) {
              curr_value = step(arg(state), low_value, high_value);
            })));
  }
};

template <IsStateExpression Arg> struct dabs : StateExpression {
  Arg arg;

  dabs(Arg arg_) : arg(arg_) {}

  double curr_sign = 1;

  auto operator()(const auto &state) const { return curr_sign * arg(state); }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(arg.template get_events<current_coordinate>(),
                          std::make_tuple(StartEvent(nullptr,
                                                     [this](const auto &state) {
                                                       curr_sign =
                                                           sign(arg(state));
                                                     }),
                                          Event(When(arg == 0), nullptr,
                                                [this](const auto &state) {
                                                  curr_sign = -curr_sign;
                                                })));
  }
};
template <std::size_t derivative = 1, IsStateExpression Arg>
constexpr auto D(const dabs<Arg> &abs_) {
  if constexpr (derivative == 0)
    return abs_;
  else
    return D<derivative - 1>(dsign(abs_.arg) * D(abs_.arg));
}

template <IsStateBoolExpression Condition, IsStateExpression ExprIfTrue,
          IsStateExpression ExprIfFalse>
struct dpiecewise : StateExpression {
  Condition condition;
  ExprIfTrue expr_if_true;
  ExprIfFalse expr_if_false;

  dpiecewise(Condition condition_, ExprIfTrue expr_if_true_,
             ExprIfFalse expr_if_false_)
      : condition(condition_), expr_if_true(expr_if_true_),
        expr_if_false(expr_if_false_) {};

  bool condition_value;

  auto operator()(const auto &state) const {
    return condition_value ? expr_if_true(state) : expr_if_false(state);
  }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        expr_if_true.template get_events<current_coordinate>(),
        expr_if_false.template get_events<current_coordinate>(),
        std::make_tuple(
            StartEvent(nullptr,
                       [this](const auto &state) {
                         condition_value = condition(state);
                       }),
            Event(WhenSwitch(condition), nullptr, [this](const auto &state) {
              condition_value = condition(state);
            })));
  }
};

} // namespace diffurch
