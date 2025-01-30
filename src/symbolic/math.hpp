#pragma once

#include "../events.hpp"
#include "expression.hpp"
#include <math.h>
#include <utility>

namespace State {

#define STATE_FUNCTION_OVERLOAD(func, func_derivative)                         \
  template <IsStateExpression Arg> struct Function_##func : StateExpression {  \
    Arg arg;                                                                   \
    Function_##func(Arg arg_) : arg(arg_) {}                                   \
    auto operator()(const auto &state) const { return func(arg(state)); }      \
    auto operator()(const auto &state, double t) const {                       \
      return func(arg(state, t));                                              \
    }                                                                          \
    auto operator()(double t) const { return func(arg(t)); }                   \
    auto prev(const auto &state) const { return func(arg.prev(state)); }       \
    auto get_events() const { return arg.get_events(); }                       \
  };                                                                           \
  template <IsStateExpression Arg> auto func(Arg arg) {                        \
    return Function_##func(arg);                                               \
  }                                                                            \
  template <size_t derivative = 1, IsStateExpression Arg>                      \
  constexpr auto D(const Function_##func<Arg> &func) {                         \
    if constexpr (derivative == 0)                                             \
      return func;                                                             \
    else                                                                       \
      return D<derivative - 1>(func_derivative(func.arg) * D(func.arg));       \
  }

/*double x = TEST([](auto t) { return t * t; });*/

/*double x = - sin (2);*/
STATE_FUNCTION_OVERLOAD(sin, cos);
STATE_FUNCTION_OVERLOAD(cos, -sin);
STATE_FUNCTION_OVERLOAD(tan, [](const auto &x) { return pow(cos(x), -2); });
/*STATE_FUNCTION_OVERLOAD(cot);*/
STATE_FUNCTION_OVERLOAD(exp, exp);
STATE_FUNCTION_OVERLOAD(log, [](const auto &x) { return 1 / x; });
STATE_FUNCTION_OVERLOAD(log10,
                        [](const auto &x) { return 1 / (x * std::log(10.)); });
STATE_FUNCTION_OVERLOAD(log2,
                        [](const auto &x) { return 1 / (x * std::log(2.)); });

/*VARIABLE_OVERLOAD_FUNCTION_2(pow);*/
/*VARIABLE_OVERLOAD_FUNCTION_2(atan2);*/

} // namespace State
