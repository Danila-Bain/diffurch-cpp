#pragma once

#include "expression.hpp"
#include <tuple>

namespace diffurch {

template <typename T> T sign(T x) { return (x > 0) - (x < 0); }

template <IsStateExpression Arg> struct Function_sign : StateExpression {
  Arg arg;
  Function_sign(Arg arg_) : arg(arg_) {}
  double curr_value = 0;
  auto operator()(const auto &state) const { return curr_value; }
  auto operator()(const auto &state, double t) const {
    return sign(arg(state, t));
  }
  auto operator()(double t) const { return sign(arg(t)); }
  auto prev(const auto &state) const { return curr_value; }

  auto get_events() {
    return std::tuple_cat(
        arg.get_events(),
        std::make_tuple(StartEvent(nullptr,
                                   [this](const auto &state) {
                                     std::cout << "start event called"
                                               << std::endl;
                                     curr_value = sign(arg(state));
                                   }),
                        Event(When(arg == 0), nullptr, [this](auto &state) {
                          curr_value = -curr_value;
                        })));
  }
};
template <IsStateExpression Arg> auto sign(Arg arg) {
  return Function_sign(arg);
}

template <IsStateExpression Arg> struct Function_step : StateExpression {
  Arg arg;
  double low_value;
  double high_value;
  double threshold;

  Function_step(Arg arg_, double low = 0, double high = 1, double thresh = 0)
      : arg(arg_), low_value(low), high_value(high), threshold(thresh) {}

  double curr_value = 0;

  auto operator()(const auto &state) const { return curr_value; }
  auto operator()(const auto &state, double t) const {
    return arg(state, t) < threshold ? low_value : high_value;
  }
  auto operator()(double t) const {
    return arg(t) < threshold ? low_value : high_value;
  }
  auto prev(const auto &state) const { return curr_value; }

  auto get_events() {
    return std::tuple_cat(
        arg.get_events(),
        std::make_tuple(
            StartEvent(nullptr,
                       [this](const auto &state) {
                         curr_value =
                             arg(state) < threshold ? low_value : high_value;
                       }),
            Event(When(arg == threshold), nullptr, [this](auto &state) {
              arg(state) < threshold ? low_value : high_value;
            })));
  }
};
template <IsStateExpression Arg> auto step(Arg arg, auto... args) {
  return Function_step(arg, args...);
}

/**/
/*template <IsStateExpression Arg> struct Function_abs : StateExpression {*/
/*  Arg arg;*/
/*  Function_abs(Arg arg_) : arg(arg_) {}*/
/*  auto operator()(const auto &state) const { return abs(arg(state)); }*/
/*  auto operator()(const auto &state, double t) const {*/
/*    return abs(arg(state, t));*/
/*  }*/
/*  auto operator()(double t) const { return abs(arg(t)); }*/
/*  auto prev(const auto &state) const { return abs(arg.prev(state)); }*/
/**/
/*  auto get_events() const { return Events(arg.get_events(), Event(arg == 0));
 * }*/
/*};*/
/*template <IsStateExpression Arg> auto abs(Arg arg) { return Function_abs(arg);
 * }*/

/*template <IsStateBoolExpression Condition, IsStateExpression ExprIfTrue,*/
/*          IsStateExpression ExprIfFalse>*/
/*struct Piecewise : StateExpression {*/
/*  Condition condition;*/
/*  ExprIfTrue expr_if_true;*/
/*  ExprIfFalse expr_if_false;*/
/**/
/*Piecewise(Condition condition_, ExprIfTrue expr_if_true_,*/
/*          ExprIfFalse expr_if_false_)*/
/*    : condition(condition), expr_if_true(expr_if_true_),*/
/*      expr_if_false(expr_if_false_) {};*/
/**/
/*bool condition_value;*/
/**/
/*auto operator()(const auto &state) const {*/
/*  return condition_value ? expr_if_true(state) : expr_if_false(state);*/
/*}*/
/*auto operator()(const auto &state, double t) const {*/
/*  return condition(state, t) ? expr_if_true(state, t)*/
/*                             : expr_if_false(state, t);*/
/*}*/
/*auto operator()(double t) const {*/
/*  return condition(t) ? expr_if_true(t) : expr_if_false(t);*/
/*}*/
/*auto prev(const auto &state) const {*/
/*  return condition.prev(state) ? expr_if_true.prev(state)*/
/*                               : expr_if_false.prev(state);*/
/*}*/
/**/
/*auto get_events() const {*/
/*  return Events(expr_if_true.get_events(), expr_if_false.get_events(),*/
/*                StartEvent(nullptr,*/
/*                           [this](const auto &state) {*/
/*                             condition_value = condition(state);*/
/*                           }),*/
/*                Event(WhenChanges(condition), nullptr, [this](auto &state)
 * {*/
/*                  condition_value = !condition_value;*/
/*                }));*/
/*}*/
/*};*/

} // namespace diffurch
