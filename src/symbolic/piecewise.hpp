#pragma once

#include "expression.hpp"

namespace State {

/*template <IsStateExpression Arg> struct Function_sign : StateExpression {*/
/*  Arg arg;*/
/*  Function_sign(Arg arg_) : arg(arg_) {}*/
/*  double value;*/
/*  auto operator()(const auto &state) const { return value; }*/
/*  auto operator()(const auto &state, double t) const {*/
/*    return sign(arg(state, t));*/
/*  }*/
/*  auto operator()(double t) const { return sign(arg(t)); }*/
/*  auto prev(const auto &state) const { return value; }*/
/**/
/*  auto get_events() const {*/
/*    return Events(*/
/*        arg.get_events(),*/
/*        StartEvent(nullptr,*/
/*                   [this](const auto &state) { value = sign(arg(state)); }),*/
/*        Event(arg == 0, nullptr, [this](auto &state) { value = -value; }));*/
/*  }*/
/*};*/
/*template <IsStateExpression Arg> auto sign(Arg arg) {*/
/*  return Function_sign(arg);*/
/*}*/
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

} // namespace State
